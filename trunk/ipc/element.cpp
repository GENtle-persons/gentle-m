/***************************************************************************
 *  file                 :  element.c                                      *
 *  copyright            : (C) 2001 by Dirk Nolting	                   *
 *  email                : nolting@uni-duesseldorf.de                      *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "global.h"
#include "element.h"
#include <unistd.h>

TIPC_ELEMENT::TIPC_ELEMENT ( TIPC *i )
	{
 	elements=NULL;
	ipc = i ;
	ELEMENTFILE = strdup ( "ipc/elemente" ) ;
//	ELEMENTFILE = new char[1000] ;
// 	strcpy ( ELEMENTFILE , "ipc/elemente" ) ;
	}    


FILE *TIPC_ELEMENT::open_file(char *filename)
{
  FILE *datenfile;
  char *pwd;

  pwd=getcwd(NULL,0);

  if( (chdir(PREFIX)) == -1 )
    {
      printf("Can't change to directory %s\n",PREFIX);
      return 0;
    }
 
  if(!(datenfile=fopen(filename,"r")))
    {
      printf("Can't open file: %s.\n",filename);
      return 0;
    }
 if( (chdir(pwd)) == -1 )
    {
      printf("Can't change to directory %s\n",pwd);
      return 0;
    }
 return datenfile;
}

char *TIPC_ELEMENT::get_token(char *linebuffer)
{
  static char *buffer;
  static char token[MAX_LINE];
  int c=0;

  if(linebuffer)
    buffer=linebuffer;

  while( (*buffer!=' ') && (*buffer!='\n') && (*buffer!='\r') )
    {
      token[c]=*buffer;
      *buffer++;
      c++;
    }
  token[c]='\0';
  *buffer++;
  return token;
}

void TIPC_ELEMENT::add_element(element *ce)
{
  element *cur;
  
  if(!(elements))
    {
      elements=ce;
      elements->previous=NULL;
      return;
    }

  cur=elements;
  while(cur->next)
    cur=cur->next;

  cur->next=ce;
  ce->previous=cur;
}

void TIPC_ELEMENT::add_isotope(isotope *ci,element *ce)
{
  isotope *cur;

  if(!(ce->isotopes))
    {
      ce->isotopes=ci;
      ci->previous=NULL;
      return;
    }

  cur=ce->isotopes;
  while(cur->next)
    cur=cur->next;
  cur->next=ci;
  ci->previous=cur;
  return;
}

int TIPC_ELEMENT::init_elements()
{
  FILE *data;
  char linebuffer[MAX_LINE];
  element *ce;
  isotope *ci=NULL;
  char *par;

  if(!(data=open_file(ELEMENTFILE)))
    return 0;
   
  while(fgets(linebuffer,MAX_LINE,data))
    {
      if(isalpha(*linebuffer))
	{
	  ce=(element*)malloc(sizeof(element));
	  ce->isotopes = NULL;
	  ce->symbol=strdup(get_token(linebuffer));
	  ce->next=NULL;
	  add_element(ce);
	}
      if(isdigit(*linebuffer))
	{
	  ci=(isotope *)malloc(sizeof(isotope));
	  par=strdup(get_token(linebuffer));
	  ci->mass=atof(par);
	  par=strdup(get_token(0));
	  ci->p=atof(par);
	  ci->next=NULL;
	  add_isotope(ci,ce);
	}
    }
    
  fclose(data);
  return 1;
}
