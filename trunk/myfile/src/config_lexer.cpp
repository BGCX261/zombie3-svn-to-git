//#include "scrambled_symbols.h"
#include "myfilelog.hxx"
#include "config_parser.h"

#include <string.h>
#include <sstream>
using namespace std;

static int  colnum=0;
static int linenum=0;

#define MAXINPUTBUF 2048
char inputbuf[MAXINPUTBUF];

#define MAXTOKENLEN  1024
static char tokenbuf[MAXTOKENLEN+1];

FILE *configin;

typedef enum
  {
    WAITING_ALL=0,
    SCANNING_ID=1,
    SCANNING_STRING=2,
    SCANNING_DEC=3,
    SKIPPING_LINE=4
  } tLexerMode;

int config_flex_debug=0;

static tLexerMode _mode=WAITING_ALL;

static int _end_of_file_reached=0;

int configlex()
{
  static int read_index=-1;
  static int token_index=0;
  static int max_index=0;
  static int cached_char_flag=0;
  static char cached_char=0;
  
  char *pcurrentchar;
  
  while(1)
    {
      if (read_index==-1)
	{
	  max_index=fread(inputbuf,1,MAXINPUTBUF,configin);

	  if ( (max_index==0) && feof(configin))
	    {
	      // In order to be able to read configuration file
	      // with only one line without '\n' character, we 
	      // need to use the following tips: The first time
	      // we encounter the eof, we return the '\n' character.
	      // All the other time, we return 0.
	      if (_end_of_file_reached==0)
		{
		  _end_of_file_reached++;
		  return '\n';
		}
	      // Eof.
	      return 0;
	    }

	  if ( (max_index==0) && ferror(configin))
	    {
	      // Error.
	      return -1;
	    }

	  if (max_index==0)
	    {
	      // Watchdog! We should never encounter this case
	      // but we have to protect ourselves against such case.
	      // Error should be announced.
	      return -1;
	    }

	  read_index=0;
	}

      if (!cached_char_flag)
	{
	  pcurrentchar=&inputbuf[read_index];
	  read_index++;
	  if (read_index>=max_index)
	    {
	      read_index=-1;
	    }
	}
      else
	{
	  pcurrentchar=&cached_char;
	  cached_char_flag=0;
	  colnum--;
	}

      switch(_mode)
	{
	case WAITING_ALL:
	  {
	    if (*pcurrentchar=='\n')
	      {
		colnum=0;
		linenum++;
		return *pcurrentchar;
	      }

	    if ( (*pcurrentchar==' ') || (*pcurrentchar=='\t') || (*pcurrentchar=='\v') ||
		 (*pcurrentchar=='\r') )
	      {
		colnum++;
		// Skipping space.
		continue;
	      }

	    if (*pcurrentchar=='#')
	      {
 		colnum++;
		_mode=SKIPPING_LINE;
		continue;
	      }

	    if ( ( (*pcurrentchar>=65) && (*pcurrentchar<=90) ) ||
		 ( (*pcurrentchar>=97) && (*pcurrentchar<=122) ) ||
		 ( *pcurrentchar == 95) )
	      {
 		colnum++;
		_mode=SCANNING_ID;
		token_index=1;
		tokenbuf[0]=*pcurrentchar;
		continue;
	      }

	    if ( (*pcurrentchar>=48) && (*pcurrentchar<=57) )
	      {
 		colnum++;
		_mode=SCANNING_DEC;
		token_index=1;
		tokenbuf[0]=*pcurrentchar;
		continue;
	      }

	    if (*pcurrentchar=='"')
	      {
 		colnum++;
		_mode=SCANNING_STRING;
		token_index=1;
		tokenbuf[0]=*pcurrentchar;
		continue;
	      }

	    if (*pcurrentchar=='=')
	      {
 		colnum++;
		return EQUAL_TOKEN;
	      }

	    colnum++;
	    return *pcurrentchar;

	  }
	  break;

	case SCANNING_ID:
	  {
	    colnum++;
	    if ( ( (*pcurrentchar>=65) && (*pcurrentchar<=90) ) ||
		 ( (*pcurrentchar>=97) && (*pcurrentchar<=122) ) ||
		 ( (*pcurrentchar>=48) && (*pcurrentchar<=57) ) ||
		 ( *pcurrentchar == 95) )
	      {
		tokenbuf[token_index]=*pcurrentchar;
		token_index++;

		if (token_index>=MAXTOKENLEN)
		  {
		    // We reached the maximum allowed size for a token.
		    // Error should be reported.
		    return -2;
		  }
		continue;
	      }

	    tokenbuf[token_index]='\0';
	    configlval.astring=strdup(&tokenbuf[0]);
	    token_index=0;
	    _mode=WAITING_ALL;

	    cached_char=*pcurrentchar;
	    cached_char_flag=1;
	    // Here we don't have a character that can belong to an identifier.
	    // We can report to have found one identifier.
	    return ID_TOKEN;
	  }
	  break;

	case SCANNING_STRING:
	  {
	    colnum++;
	    if ( ( *pcurrentchar != '"') || ( (*pcurrentchar == '"') && (tokenbuf[token_index-1]=='\\') ) )
	      {
		tokenbuf[token_index]=*pcurrentchar;
		token_index++;

		if (token_index>=MAXTOKENLEN)
		  {
		    // We reached the maximum allowed size for a token.
		    // Error should be reported.
		    return -2;
		  }
		continue;
	      }

	    // Here we meet a closing string '"' character.
	    tokenbuf[token_index]='\0';
	    configlval.astring=strdup(&tokenbuf[1]);
	    token_index=0;
	    _mode=WAITING_ALL;

	    return STRING_TOKEN;

	  }
	  break;

	case SCANNING_DEC:
	  {
	    colnum++;
	    if ( (*pcurrentchar>=48) && (*pcurrentchar<=57) )
	      {
		tokenbuf[token_index]=*pcurrentchar;
		token_index++;
		
		if (token_index>=MAXTOKENLEN)
		  {
		    // We reached the maximum allowed size for a token.
		    // Error should be reported.
		    return -2;
		  }
		continue;
	      }

	    // Here we meet another character different from decimal character.
	    tokenbuf[token_index]='\0';
	    configlval.astring=strdup(&tokenbuf[0]);
	    token_index=0;
	    _mode=WAITING_ALL;

	    // We have to remember the character we met for next token scanning.
	    cached_char=*pcurrentchar;
	    cached_char_flag=1;

	    return DECIMAL_TOKEN;

	  }
	  break;

	case SKIPPING_LINE:
	  {
	    colnum++;
	    if (*pcurrentchar!='\n')
	      {
		continue;
	      }

	    linenum++;
	    colnum=0;
	    _mode=WAITING_ALL;

	    return *pcurrentchar;
	  }
	  break;

	default:
	  {
	    return *pcurrentchar;
	  }

	  break;
	}
      
    } // while(1)

  // End of file reached.
  // Actually this point is never reached. 
  // Just put here to avoid warning during compilation.
  return -1;
}


void configerror(const char *msg)
{
  stringstream ss;

  ss<<"Syntax error encountered while parsing Config File at line "<<linenum+1<<", column "<<colnum<<".\n";

  FileLog::Error(ss.str());
}


