/***************************************************************************
 *  file                 :  element.h                                      *
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

#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include <string.h>
#include <ctype.h>

#define MAX_LINE    30

class TIPC ;
class TIPC_PARS ;

class TIPC_ELEMENT
	{
	public :
 	TIPC_ELEMENT ( TIPC *i ) ;
 	
 	private :
    friend class TIPC ;
    friend class TIPC_PARS ;
    int init_elements(void);
    FILE* open_file(char *filename);
    void add_element(element *ce);
    void add_isotope(isotope *ci,element *ce);
    char *get_token(char *linebuffer);
    
  	element *elements;
    TIPC *ipc ;
    char * ELEMENTFILE ;
	} ;    

/*

#define ELEMENTFILE  "ipc/elemente"
#define MAX_LINE    30

int init_elements(void);
FILE* open_file(char *filename);
void add_element(element *ce);
void add_isotope(isotope *ci,element *ce);
char *get_token(char *linebuffer);
*/

#endif
