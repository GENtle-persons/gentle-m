/***************************************************************************
 *  file                 :  ipc.h                                          *
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

#ifndef _IPC_H
#define _IPC_H

#include "global.h"

#define MIN_DIF 0.0009
#define MIN_INT 0.009

int ipc_main(int argc,char **argv);
int calculate_peaks(void);
void print_result(int digits,int charge);
void free_list(isotope *target);
void summarize_peaks(void);
isotope *add_peak(isotope *base,isotope *peak);
void usage(void);
void cut_peaks(isotope *spectrum);

#endif
