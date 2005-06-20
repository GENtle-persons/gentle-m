/***************************************************************************
 *  file                 :  pars.c                                         *
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

//#include "global.h"
#include "ipc.h"
#include "pars.h"
#include "element.h"

TIPC_PARS::TIPC_PARS ( TIPC *i )
	{
	MAX_DIGITS = 10 ;
	MAX_PEP_LINE = 81 ;
	ipc = i ;
	}    

int TIPC_PARS::is_symbol(char *probe)
{
  element *cur;

  cur=ipc->element->elements;
  while(cur)
    {
      if(!strcmp(probe,cur->symbol))
	return 1;
      cur=cur->next;
    }
  return 0;
}

int TIPC_PARS::add_component(char *symbol,int number)
{
  element *el;
  ipc_compound *newco=NULL,*co;

  el=ipc->element->elements;
  while(el)
    {
      if(!strcmp(symbol,el->symbol))
	{
	  newco=(ipc_compound *)malloc(sizeof(ipc_compound));
	  newco->isotopes=el->isotopes;
	  newco->amount=number;
	  newco->next=NULL;
	}
      el=el->next;
    }

  if(!newco)
    {
      printf("Unknown element: %s. Check input or file elemente\n",symbol);
      return 0;
    }

  co=ipc->verbindung;
  if(!ipc->verbindung)
    {
      newco->previous=NULL;
      ipc->verbindung=newco;
      return 1;
    }

  while(co)
    {
      if( co->isotopes == newco->isotopes )
	{
	  co->amount+=newco->amount;
	  free(newco);
	  return 1;
	}
      co=co->next;
    }

  co=ipc->verbindung;
  while(co->next)
    co=co->next;
  co->next=newco;
  newco->previous=co;
  return 1;
}

int TIPC_PARS::pars_chem_form(char *formel)
{
  char par[MAX_DIGITS],par1[MAX_DIGITS];
  int m=0,number=0;
  
  if(!formel) return 0;

  while(*formel)
    {
      m=0;
      while( isalpha(*formel) )
	{
	  par[m]=*formel;
	  par[m+1]='\0';
	  m++;
	  *formel++;
	  /*Adding omiited 1, e.g. CCl4 ->C1Cl4 */
	  if(is_symbol(par) && isupper(*formel))
	    {
	      add_component(par,1);
	      m=0;
	    }
	}
      m=0;

      while( isdigit(*formel))
	{
	  par1[m]=*formel;
	  m++;
	  *formel++;
	}
      par1[m]='\0';
      number=atoi(par1);
      /* Adding omitted last 1, e.g. CH3Cl -> CH3Cl1 */
      if(!number) number=1;
      if(!add_component(par,number))
	return 0;
    }
  return 1;
}

int TIPC_PARS::add_amino_acid(char acid)
{
  switch(acid)
    {
	    case 'A':
	      add_component("C",3);
	      add_component("H",5);
	      add_component("N",1);
	      add_component("O",1);
	      break;
	    case 'R':
	      add_component("C",6);
	      add_component("H",12);
	      add_component("N",4);
	      add_component("O",1);
	      break;
	    case 'N':
	      add_component("C",4);
	      add_component("H",6);
	      add_component("N",2);
	      add_component("O",2);
	      break;
	    case 'B':
	    case 'D':
	      add_component("C",4);
	      add_component("H",5);
	      add_component("N",1);
	      add_component("O",3);
	      break;
            case 'C':
	      add_component("C",3);
	      add_component("H",5);
	      add_component("N",1);
	      add_component("O",1);
	      add_component("S",1);
	      break;
            case 'E':
	      add_component("C",5);
	      add_component("H",7);
	      add_component("N",1);
	      add_component("O",3);
	      break;
	    case 'Z':
	    case 'Q':
	      add_component("C",5);
	      add_component("H",8);
	      add_component("N",2);
	      add_component("O",2);
	      break;
	    case 'X':
	    case 'G':
	      add_component("C",2);
	      add_component("H",3);
	      add_component("N",1);
	      add_component("O",1);
	      break;
            case 'H':
	      add_component("C",6);
	      add_component("H",7);
	      add_component("N",3);
	      add_component("O",1);
	      break;
	    case 'I':
	    case 'L':
	      add_component("C",6);
	      add_component("H",11);
	      add_component("N",1);
	      add_component("O",1);
	      break;
	    case 'K':
	      add_component("C",6);
	      add_component("H",12);
	      add_component("N",2);
	      add_component("O",1);
	      break;
	    case 'M':
	      add_component("C",5);
	      add_component("H",9);
	      add_component("N",1);
	      add_component("O",1);
	      add_component("S",1);
	      break;
	    case 'F':
	      add_component("C",9);
	      add_component("H",9);
	      add_component("N",1);
	      add_component("O",1);
	      break;
	    case 'P':
	      add_component("C",5);
	      add_component("H",7);
	      add_component("N",1);
	      add_component("O",1);
	      break;
	    case 'S':
	      add_component("C",3);
	      add_component("H",5);
	      add_component("N",1);
	      add_component("O",2);
	      break;
	    case 'T':
	      add_component("C",4);
	      add_component("H",7);
	      add_component("N",1);
	      add_component("O",2);
	      break;
	    case 'W':
	      add_component("C",11);
	      add_component("H",10);
	      add_component("N",2);
	      add_component("O",1);
	      break;
	    case 'Y':
	      add_component("C",9);
	      add_component("H",9);
	      add_component("N",1);
	      add_component("O",2);
	      break;
	    case 'V':
	      add_component("C",5);
	      add_component("H",9);
	      add_component("N",1);
	      add_component("O",1);
	      break;
	    default:
	      printf("Unknown symbol: %c\n",acid);
	      return 0;
	    }
  return 1;
}

int TIPC_PARS::pars_amino_acid(char *formel)
{
  while(*formel)
    {
      if(!add_amino_acid(*formel))
	return 0;
      *formel++;
    }
  add_component("O",1);
  add_component("H",2);

  return 1;
}

int TIPC_PARS::pars_peptid(char *formel)
{
  FILE *peptid_file;
  char linebuffer[MAX_PEP_LINE];
  int index=0;

  if(!(peptid_file=fopen(formel,"r")))
    {
      printf("Can´t open file: %s\n",formel);
      return 0;
    }

  while( fgets(linebuffer,MAX_PEP_LINE,peptid_file) )
    {
      while( linebuffer[index] != '\n' )
	{
	  add_amino_acid(linebuffer[index]);
	  index++;
	}
      index=0;
    }

      add_component("O",1);
      add_component("H",2);

      fclose(peptid_file);
      
      return 1;
}


int TIPC_PARS::print_sum()
{
  element *el;
  ipc_compound *co;

  printf("\nChemical formula: ");
  co=ipc->verbindung;

  while(co)
    {
      el=ipc->element->elements;
      while( el && ( co->isotopes != el->isotopes) )
	el=el->next;

      if(!el) return 0;
      printf("%s%i ",el->symbol,co->amount);
      co=co->next;
    }

  printf("\n");
  return 1;
}
