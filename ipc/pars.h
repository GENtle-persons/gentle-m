/***************************************************************************
 *  file                 :  pars.h                                         *
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

#ifndef _PARS_H_
#define _PARS_H_

#define MAX_DIGITS 10
#define MAX_PEP_LINE 81

#include <string.h>
#include <ctype.h>

int pars_peptid(char *formel);
int pars_chem_form(char *formel);
int is_symbol(char *probe);
int add_component(char *symbol,int number);
int pars_amino_acid(char *formel);
int add_amino_acid(char acid);
int print_sum(void);

#endif
