/***************************************************************************
 *  file                 :  gp_out.h                                       *
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

#ifndef _GPOUT_H_
#define _GPOUT_H_

class TIPC ;

typedef struct spec_points {
  struct spec_points *next, *previous;
  float x,y;
} spec_points;

class GPOUT
	{
	public :
 	GPOUT ( TIPC *i = NULL ) ;
 	
	private :
	friend class TIPC ;
    int make_gnuplot_output(char *gnuplotfile);
    spec_points *calc_points(void);
    spec_points *add_new_point(spec_points *spectrum);
    void print_spectrum(spec_points *spectrum, FILE *output);
    void free_spectrum(spec_points *spectrum);

    
    TIPC *ipc ;
    } ;


/*
int make_gnuplot_output(char *gnuplotfile);
spec_points *calc_points(void);
spec_points *add_new_point(spec_points *spectrum);
void print_spectrum(spec_points *spectrum, FILE *output);
void free_spectrum(spec_points *spectrum);
*/

#endif
