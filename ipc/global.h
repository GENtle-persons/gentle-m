/***************************************************************************
 *  file                 :  global.h                                       *
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

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define PREFIX "."
#define IPC_VERSION "1.2"
//#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#define USE_DIGITS 5

/// Part of the IPC package
typedef struct isotope{
  struct isotope *next,*previous;
  double p;
  double mass;
} isotope;

/// Part of the IPC package
typedef struct ipc_compound {
  struct ipc_compound *next,*previous;
  int amount;
  struct isotope *isotopes;
} ipc_compound ;

/// Part of the IPC package
typedef struct element {
  struct element *next,*previous;
  char *symbol;
  struct isotope *isotopes;
} element;

//extern element *elements;
//extern ipc_compound *verbindung;
//extern isotope *peaks;

//extern int fast_calc;

#endif
