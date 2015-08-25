%name-prefix="config"
%debug

%{
#ifndef __cplusplus
#error "This file should be compiled with a C++ compiler"
#endif

//#include "scrambled_symbols.h"
#include "priv/configfileimpl.hxx"
#include "mymutex.hxx"
#include <stdio.h>

static ConfigFileImpl *config_file;
static Mutex protect_config_file;

extern FILE *configin;
extern void configerror(const char *msg);
extern int configlex();

int config_err=0;

%}

%union {
  char *astring;  /* /!\ Each token must be freed after usage.*/
}

%token <astring> DECIMAL_TOKEN
%token <astring> EQUAL_TOKEN
%token <astring> ID_TOKEN
%token <astring> STRING_TOKEN

%%
config_list:   config
             | config_list config
             | error '\n'
               {
		 config_err++;
	       }
              ;

config:     ID_TOKEN EQUAL_TOKEN STRING_TOKEN '\n'
            {
	      config_file->SetValue($1,$3);
	      free($1); free($3);
	    }
          | ID_TOKEN EQUAL_TOKEN DECIMAL_TOKEN '\n'
            {
	      config_file->SetValue($1,$3);
	      free($1); free($3);
	    }
          | '\n'
            ;

%%

bool Config_Start_Read(const char *file, ConfigFileImpl *cfi)
{
  protect_config_file.Lock();

  config_file=cfi;

  configin=fopen(file,"r");

  if (configin==NULL) 
    {
      protect_config_file.Unlock();
      config_file=0;
      return false;
    }

  configparse();

  fclose(configin);

  config_file=0;
  protect_config_file.Unlock();

  return config_err==0;
}
