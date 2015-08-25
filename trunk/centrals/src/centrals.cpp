#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#if defined(linux)
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#endif 

#include <sys/types.h>

#include <sstream>
#include <list>

#if defined(TRACE)
#include <iostream>
#endif

//#if ! defined(SERVERTYPE)
//#cerr "This file must be compiled by defining the SERVERTYPE macro (Example: -SERVERTYPE=master)!"
//#define SERVERTYPE       default
//#endif
#define SERVERTYPE	master

#define CONCAT3(a,b,c)     a##b.c
#define MAKE_STRING1(a)  #a
#define MAKE_STRING(a)  MAKE_STRING1(a)

#define CONFIG_FILE1(x)    CONCAT3(centrals_,x,conf)
#define CONFIG_FILE(x)     CONFIG_FILE1(x)


/// \brief Default running directory of the centrals daemon.

#if defined(linux)
#define RUNNING_DIR	"/tmp"
#elif defined(WIN32)
#define RUNNING_DIR	"C:\\Windows\\Temp"

#pragma comment(lib,"mythread")
#pragma comment(lib,"myfile")
#pragma comment(lib,"mynet")
#pragma comment(lib,"myudpnet")
#pragma comment(lib,"message")
#pragma comment(lib,"bridgenet")

#endif

#include "priv/cthandler.hxx"
#include "priv/sconnmanager.hxx"
//#include "priv/udplistener.hxx"
#include "configfile.hxx"
#include "myfilelog.hxx"
#include "loghelper.hxx"
#include "mythread.hxx"
#include "mytimer.hxx"

using namespace std;

static FileLog *_filelog;

/// \brief Daemon lock file descriptor.
/// It is a global variable because signal handler need to access it.
#if defined(linux)
static int g_lfp;
#endif

/// \brief Variable controlling the shutdown of the daemon.
/// As long as this variable is set to false, then the daemon is running.
static bool _is_shutdown_requested=false;

// Configuration file of the centrals server.
static ConfigFile _configfile;

// Variables containing all value that can be changed through
// configuration file.
static string centralslockfile;
static string centralslogfile;
static tSInt32 sport;
static tSInt32 cport;
//static tSInt32 uport;
static string dbhost;
static tSInt32 dbport;
static string dblogin;
static string dbpassword;
static string dbname;
static tSInt32 _debug;
static string username;
static string groupname;

//centrals type configuration
static tSInt32 centralstype;	//1->Master,2->Slave.

///////////////////////////////////////////////////////////////////////////////
/// \brief Basic checking for security.
/// 
/// This function checks that the user that runs the daemon is a valid user
/// (exists in the /etc/passwd or NIS) and is not root.
static bool CheckRunningUser(const char *progname)
{
#if defined(linux)
  struct passwd *pw;
  int uid;

  // Make sure the current user exists.
  uid = getuid();
  pw = getpwuid(uid);

  if (pw == NULL) 
    {
      cerr<<"[ERROR] Invalid uid "<<uid<<" for the user currently running this program.\n";
      return false;
    }

  if (getuid()==0)
    {
      cerr<<"[ERROR] "<<progname<<" is run as root.\n";
      return false;
    }

  return true;
#else
  return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Change effective user and group of current process to the given ones
///
/// 'username' and 'groupname' can be either the uid or the name.
static bool ChangeUserGroup(const string &username, const string groupname)
{
#if defined(linux)
  struct passwd *pw;
  struct group *gr;
  int uid;
  int gid;
  
  // Checking whether the username is fully composed of digits?
  if (strspn(username.c_str(), "1234567890") != username.size()) 
    {
      // Here, the given username is not fully composed of digit,
      // then trying to find it into /etc/passwd.
      if ((pw = getpwnam(username.c_str())) == NULL) 
	{
	  cerr<<"[ERROR] Invalid user name: '"<<username<<"'\n";
	  return false;
	}
    }
  else 
    {
      // Here, the given username is fully composed of digit,
      // then trying to find it into /etc/passwd.
      if ((pw = getpwuid(atoi(username.c_str()))) == NULL) 
	{
	  cerr<<"[ERROR] Invalid user id: '"<<username<<"'\n";
	  return false;
	}
  }

  uid = pw->pw_uid;

  // Same job as previous but for groupname now.
  // Checking whether groupname is fully composed of digits?
  if (strspn(groupname.c_str(), "1234567890") != groupname.size()) 
    {
      if ((gr = getgrnam(groupname.c_str())) == NULL) 
	{
	  cerr<<"[ERROR] Invalid group name: '"<<groupname<<"'\n";
	  return false;
	}

      gid = gr->gr_gid;
    }
  else 
    {
      gid = atoi(groupname.c_str());
    }
  

  // Change user group and initialize the group access list for 
  // the target user, and setgid() to the target group. 
  if (((setgid(gid)) != 0) || (initgroups(username.c_str(), gid) != 0))
    {
      cerr<<"[ERROR] Failed to setgid to "<<gid<<".\n";
      return false;
    }
  
  // Change user id.
  if ((setuid(uid)) != 0) 
    {
      cerr<<"[ERROR] Failed to setuid to "<<uid<<".\n";
      return false;
    }

  return true;
#else
  return true;
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// \brief A call to this function request the daemon shutdown.
/// 
/// In current implementation, the possible caller is the 
/// signal handler when receiving the SIGTERM signal.
///
/// Note that we didn't protect the modification of the '_is_shutdown_requested' 
/// variable by mutexes because since it is only a boolean, we can consider
/// the operation as atomic and immune to concurrent access problems.
////////////
void RequestDaemonShutdown()
{
  _is_shutdown_requested=true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns whether or not a shutdown has been requested.
/// 
/// This is a private function that can only be called by function of current
/// file. It is only used by the main loop controlling the daemon execution.
///
/// Note that we didn't protect the access of the '_is_shutdown_requested' 
/// variable by mutexes because since it is only a boolean, we can consider
/// the operation as atomic and immune to concurrent access problems.
////////////
static bool ShutdownRequested()
{
  return _is_shutdown_requested;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Perform a shutdown of the daemon and returing the given error code.
///
/// The main purpose of this function is to remove the lock file. We can 
/// consider that the daemon has been shutdown when the lock file has been
/// removed. This is the way the etc/init.d/centrals script is using to wait for
/// daemon shutdown.
///
/// Arguments:
/// \arg return_code: the code to return when terminating (exit).
////////////
static void Shutdown(int return_code)
{
#if defined(linux)
  lockf(g_lfp,F_ULOCK,0); /* Unlock first ... */
  close(g_lfp);     /* ... then close file ... */
  remove(centralslockfile.c_str());  /* ... then delete the lock file. */
#endif

  delete _filelog;
  _filelog=0;

  /* 
     Current function doesn't return if return_code is different from 0
  */
  if (return_code)
    {
      exit(return_code);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Signal handler that will be called for some signal.
////
/// This signal handler will be installed for the following signals:
/// \li SIGTERM
/// \li SIGINT
///     Force all active connections to shutdown and perform a clean shutdown.
///
/// \li SIGHUP
///     No action yet but maybe in future release.
///
/// \li SIGUSR1
///     Toggle debug mode between On/Off. This will add additional traces into 
///     log files. Be aware that this will consume more CPU and will drastically
///     increase the size of log files.
///
/// \li SIGPIPE
///     Just to be sure to handle such case since we use sockets.
///
/// \li SIGURG
///     Should never occur but I think it is good to do so. Maybe certain type
///     of attack may use this kind of strategy. If we detect something, then 
///     when can close the associated connection sending such out of band data.
///
////////////
#if defined(linux)
static void SignalHandler(int sig)
{
  switch(sig) 
    {
    case SIGHUP:
      break;
    case SIGINT:
    case SIGTERM:
      RequestDaemonShutdown();
      break;
    case SIGUSR1:
      /* Signal USR1 can be used to toggle debug mode. */
      _filelog->ToggleDebugMode();
      break;
    case SIGPIPE:
      LOGERROR("SignalHandler: received SIGPIPE signal! Just ignoring it ...");
      /* Just ignoring */
      break;
    case SIGURG:
      LOGERROR("SignalHandler: received SIGURG signal! Just ignoring it ...");
      /* Just ignoring */
      break;
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////
/// \brief Just install signal handlers.
/// 
////////////
void InstallSignalHandler()
{
#if defined(linux)
  signal(SIGCHLD,SIG_IGN); /* Ignore child */
  signal(SIGTSTP,SIG_IGN); /* Ignore tty signals */
  signal(SIGTTOU,SIG_IGN);
  signal(SIGTTIN,SIG_IGN);
  signal(SIGHUP,SignalHandler); /* catch hangup signal */
  signal(SIGTERM,SignalHandler); /* catch kill signal */
  signal(SIGINT,SignalHandler); /* catch ctrl+c signal */
  signal(SIGUSR1,SignalHandler); /* catch usr1 signal for debug */
  signal(SIGPIPE,SignalHandler); /* This signal maybe sent when reading on a closed socket. */
  signal(SIGURG,SignalHandler); /* This signal maybe sent by attacker. */
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Forks a new process and starts the execution of the daemon into this
/// new process.
///
/// This function (private to the file) forks a new process. The father process 
/// will terminate its execution with 0 as error code while the son will keep 
/// running by detaching from his father, create lock file into /var/run and 
/// install the signal handler.
///
/// NOTE: The call to this function never returns for the father. It only 
/// returns for his son.
////////////
static void Daemonize()
{
#if defined(linux)
  int i;
  char str[10];

  if(getppid()==1) return; /* Already a daemon. */

  i=fork();

  if (i<0) 
    {
      LOGERROR("Failed to fork new process! Aborting ...");
      cerr<<"Failed to fork new process! Aborting ..."<<endl;
      exit(5); /* Fork error if no more memory or process table full. */
    }

  if (i>0) 
    {
      exit(0); /* Father exits here */
    }

  //
  /* Child (daemon) continues here ...*/

  /* Obtain a new process group in order to be independant from 
   father process. Attaching to init process. */
  setsid(); 

  LOGINFO("Starting up ...");

  /* Closing all file descriptors (inherited from father process).*/
  for (i=getdtablesize();i>=0;--i) close(i); 

  /* 
     From now on, there is no need to print on stdout and stderr
     since everything has been just closed.

     Now, if something needs to be logged, use log file.
  */

  /* Handle standard I/O (redirect everything to /dev/null) */
  i=open("/dev/null",O_RDWR); dup(i); dup(i); 

  /* Set newly created file permissions */
  umask(027); 

  /* Change running directory */
  chdir(RUNNING_DIR); 

  LOGDEBUG("after chdir(RUNNING_DIR)");
  /* Create lock file. */
  g_lfp=open(centralslockfile.c_str(),O_RDWR|O_CREAT,0640);
  if (g_lfp<0)
    {
      LOGFATAL("Failed to create lock file. Forcing shutting down ...");
      exit(6);
    }

  if (lockf(g_lfp,F_TLOCK,0)<0) 
    {
      close(g_lfp);

      LOGERROR("Failed to lock file. A daemon is already running. Forcing shutting down ...");
      exit(7);
    }

  sprintf(str,"%d\n",getpid());
  write(g_lfp,str,strlen(str)); /* Record pid to lockfile */

  LOGINFO("Daemonized");
#endif
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Main processing of the daemon starts here. Creating the connection
/// managers to listen to service daemons and www server.
///
/// Argument
/// \arg None.
////////////
static void Process_Incoming_Connection()
{
  //CConnManager *cthandler;
  CTHandler    *cthandler;
  SConnManager *sconnmanager;
  //UdpListener  *udplistener;

  tUInt32 timeout;

  LOGINFO("CentralS: Daemon is up.");

	  
  LOGINFO("###################Configuration########################");
  LOGINFO("LockFile:\t"<<centralslockfile<<endl);
  LOGINFO("LogFile:\t"<<centralslogfile<<endl);
  LOGINFO("DBHost:\t"<<dbhost<<endl);
  LOGINFO("DBLogin:\t"<<dblogin<<endl);
  LOGINFO("DBName:\t"<<dbname<<endl);
  LOGINFO("SPort:\t"<<sport<<endl);
  LOGINFO("CPort:\t"<<cport<<endl);
  //LOGDEBUG("UPort:\t"<<uport<<endl);
  LOGINFO("DBPort:\t"<<dbport<<endl);
  //LOGDEBUG("PermanentHandlerNum:\t"<<permanenthandlernum<<endl);
  //LOGDEBUG("HandlerNumMax:\t"<<handlernummax<<endl);
  //LOGDEBUG("ConnMaxOfHandler:\t"<<connmaxofhandler<<endl);
  LOGINFO("###################Configuration########################");

  sconnmanager = new SConnManager(sport,_filelog);
  cthandler = new CTHandler(cport,_filelog);
  
  //udplistener = new UdpListener(uport,_filelog);
  
  sconnmanager->Start();
  cthandler->Start();
  //udplistener->Start();

  LOGINFO("CentralS: Managers started...");
  
  // Main loop controling the daemon execution.
  while (!ShutdownRequested())
  {
      // Sleep for 1000 ms to check again for shutdown request.
      Thread::SleepMs(1000);
  } // while (!ShutdownRequested())

  LOGINFO("CentralS: Shutdown signal received. Shutting down threads ...");

  sconnmanager->Shutdown();
  cthandler->Shutdown();
  //udplistener->Shutdown(); 

  timeout=0;
  while ( (timeout++<100) && ((cthandler!=0) || (sconnmanager!=0)))
  {
     Thread::SleepMs(100);

	if ((sconnmanager!=0) && (sconnmanager->State()==Thread::FINISHED_STATE))
	{
	  sconnmanager->Join();
	  delete sconnmanager;
	  sconnmanager=0;
	}
    if ((cthandler!=0) && (cthandler->State()==Thread::FINISHED_STATE))
	{
	  cthandler->Join();
	  delete cthandler;
	  cthandler=0;
	}
	//if ((udplistener!=0) && (udplistener->State()==Thread::FINISHED_STATE))
	//{
	//  udplistener->Join();
	//  delete udplistener;
	//  udplistener=0;
	//}
  }

  if (timeout>=100)
  {
    if (cthandler!=0)
	{
       LOGWARNING("CentralS: Cannot cleanly shutdown CConnManager thread. Sorry but we won't join this thread!");
	}
      
    if (sconnmanager!=0)
	{
	   LOGWARNING("CentralS: Cannot cleanly shutdown SConnManager thread. Sorry but we won't join this thread!");
	}
  }

  LOGINFO("CentralS: Shut down ...");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Read config file and set variables accordingly.
///
/// Returns 'true' if successful or 'false' if config file hasn't been found
/// or if one of the mandatory variable couldn't be found into the configuration
/// file (error in then logged into the log file).
////////////
bool ReadConfigFile()
{
  int errcount;
  const char **str_param;
  const char *str_param_list[]=
    {
      "LockFile",
      "LogFile",
      "DBHost",
      "DBLogin",
      "DBPassword",
      "DBName",
#if defined(linux)
      "User",
      "Group",
#endif
      0
    };

  string **str_param_strg;
  string *str_param_strg_list[]=
    {
      &centralslockfile,
      &centralslogfile,
      &dbhost,
      &dblogin,
      &dbpassword,
      &dbname,
#if defined(linux)
      &username,
      &groupname,
#endif
      0
    };

  const char **int_param;
  const char *int_param_list[]=
    {
      "SPort",
	  "CPort",
      //"UPort",
      "DBPort",
      "Debug",
      0
    };

  tSInt32 **int_param_strg;
  tSInt32 *int_param_strg_list[]=
    {
      &sport,
      &cport,
	  //&uport,
      &dbport,
      &_debug,
      0
    };

  errcount=0;

  // We accept 3 (for Linux) or 2 (for Windows) different locations for the configuration file.
  string path1=string("/etc/"); path1.append(string(MAKE_STRING(CONFIG_FILE(SERVERTYPE))));
  string path2=string("etc/"); path2.append(string(MAKE_STRING(CONFIG_FILE(SERVERTYPE))));
  string path3=string("./"); path3.append(string(MAKE_STRING(CONFIG_FILE(SERVERTYPE))));

  // Reading configuration file 
#if defined(linux)
  if (!_configfile.Read(path3) && !_configfile.Read(path2) && !_configfile.Read(path1))
#elif defined(WIN32)
  //if (!_configfile.Read(path3) && !_configfile.Read(path3))
#else
#error "Unsupported OS"
  if (false)
#endif
    //{
      //cerr<<"[ERROR] Failed to read config file.\n";
      //return FALSE;
    //}

  // Assign all string variables one by one. If one is missing, then it is reported
  // and will lead to error.
  for (str_param=&str_param_list[0],
	 str_param_strg=&str_param_strg_list[0]; 
       (*str_param!=0) && (*str_param_strg!=0); 
       str_param++,str_param_strg++)
    {
      if (!_configfile.GetValue(*str_param,**str_param_strg))
	{
	  cerr<<"[ERROR] Missing '"<<*str_param<<"' parameter in config file.\n";
	  errcount++;
	}      
    }
  
  // Assign all integer variables one by one. If one is missing, then it is reported
  // and will lead to error.
  for (int_param=&int_param_list[0],
	 int_param_strg=&int_param_strg_list[0]; 
       (*int_param!=0) && (*int_param_strg!=0); 
       int_param++,int_param_strg++)
    {
      if (!_configfile.GetValue(*int_param,*int_param_strg))
	  {
	    cerr<<"[ERROR] Missing '"<<*int_param<<"' parameter in config file.\n";
	    errcount++;
	  }      
    }

  // Successful only if no error encountered.
  return errcount==0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Entry point. Read configuration file, change its effective user/group
// fork the daemon and run it into a separated and independant child process.
////////////
int main(int argc, char **argv)
{
    //DB *mysqldb;

    if (!ReadConfigFile())
    {
      exit(1);
    }

#if defined(linux)
    if (getuid()==0)
    {
      if (!ChangeUserGroup(username,groupname))
        {
          exit(2);
        }
      
      if (!CheckRunningUser(argv[0]))
	  {
	    exit(3);
      }
      
    }
#endif
  
  // Registering the logfile name found in the config file into the 
  // FileLog facility.
  unlink(centralslogfile.c_str());
  _filelog=new FileLog();
  _filelog->SetFileName(centralslogfile);

//#if defined(DEBUG)
  _filelog->SetMode(_debug);
//#endif

  //// Just testing database connectivity before forking the daemon.
  //mysqldb=new DB(dbhost,dblogin,dbpassword,dbname,(tUInt32)dbport);
  //
  //if (!mysqldb->Connect())
  //  {
  //    cerr<<"[ERROR] Failed to open database '"<<dbname<<"' on '"<<dbhost<<"' with login '"<<dblogin<<"' on port '"<<dbport<<"'."<<endl;
  //    exit(4);
  //  }
  //LOGINFO("Succeeded to open database '"<<dbname<<"' on '"<<dbhost<<"'!");
  //mysqldb->Disconnect();
  //delete mysqldb;
  //mysqldb=0;

  // Make the current program a daemon. Only child process (daemon)
  // will return from this function as the father process will abort 
  // running.
  Daemonize();

  // Below is only executed by child process (daemon).

  // Install signal handler.
  InstallSignalHandler();

  // Start TCP/IP services and process incoming connection.
  // As long as the daemon is up, the following function will
  // loop. We will return this function only if the daemon is
  // shutdown.
  Process_Incoming_Connection();

  // Remove lock file and terminate process.
  Shutdown(0);

  cerr<<"centrals stopped.\n";

  return 0;
}
