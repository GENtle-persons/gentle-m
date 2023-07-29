/***************************************************************************
 *  file                 :  ipc.c                                          *
 *  copyright            : (C) 2001-2004 by Dirk Nolting                   *
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

#include "ipc.h"
#include "pars.h"
#include "element.h"
#include "gp_out.h"
#include <time.h>
#include <signal.h>
#include <math.h>

TIPC::TIPC ()
	{
    verbindung = NULL ;
    peaks = NULL ;
    fast_calc = 0 ;
    
    pars = new TIPC_PARS ( this ) ;
    gpout = new GPOUT ( this ) ;
    element = new TIPC_ELEMENT ( this ) ;
	}    

TIPC::~TIPC ()
	{
	}	

void TIPC::free_list(isotope *target)
{
  while(target->next)
    {
      target=target->next;
      free(target->previous);
    }
  free(target);
}

void TIPC::cut_peaks(isotope *spectrum)
{
  int dummy=1;

  while( (spectrum->next) && (dummy<fast_calc) )
    {
      dummy++;
      spectrum=spectrum->next;
    }

  if(spectrum->next)
    {
      free_list(spectrum->next);
      spectrum->next=NULL;
    }
}

void TIPC::summarize_peaks()
{  isotope *dummy,*d2;

 for(dummy=peaks;dummy;dummy=dummy->next)
   /* Differenz wegen Rundungsfehlern */
    while( dummy->next && (dummy->next->mass - dummy->mass < 0.00000001) )
      {
	d2=dummy->next;
	dummy->next=d2->next;
	if(dummy->next)
	  dummy->next->previous=dummy;
	dummy->p+=d2->p;
	free(d2);
      }
}

isotope *TIPC::add_peak(isotope *base,isotope *peak)
{
 static isotope *reiter;

  if(!(base->mass))
    {
      peak->next=NULL;
      peak->previous=NULL;
      reiter=peak;
      return peak;
    }

  if( peak->mass >= reiter->mass )
    while( (reiter->next) && (reiter->mass < peak->mass))
      reiter=reiter->next;
  else
    {
      while( (reiter->previous) && (reiter->mass > peak->mass) )
        reiter=reiter->previous;
      reiter=reiter->next;
    }

  if( (reiter->mass) >= (peak->mass) )
    {
      peak->next=reiter;
      peak->previous=reiter->previous;
      peak->previous->next=peak;
      reiter->previous=peak;
      return base;
    }
  else
    {
      reiter->next=peak;
      peak->next=NULL;
      peak->previous=reiter;
      return base;
    }
  return 0;
}

int TIPC::calculate_peaks(){
  ipc_compound *c;
  isotope *npeaks,*p,*i,*np1;
  int anzahl;

  if(!(peaks=(isotope*)malloc(sizeof(isotope))))
    return 0;
  peaks->mass=0;
  peaks->p=1;
  peaks->previous=NULL;
  peaks->next=NULL;

  for(c=verbindung;c;c=c->next){
    for(anzahl=0;anzahl < c->amount;anzahl++){

      if(!(npeaks=(isotope*)malloc(sizeof(isotope))))
	return 0;
      npeaks->mass=0;

      for(p=peaks;p;p=p->next){
	for(i=c->isotopes;i;i=i->next){

	  if(!(np1=(isotope*)malloc(sizeof(isotope))))
	    return 0;

	  np1->mass=p->mass + i->mass;
	  np1->p=p->p * i->p;
	  if( !(npeaks=add_peak(npeaks,np1)) )
	    return 0;
	}
      }
      free_list(peaks);
      peaks=npeaks;
      summarize_peaks();
      if(fast_calc)
	cut_peaks(peaks);
    }
  }
  return 1;
}


void TIPC::print_result(int digits,int charge){
  isotope *d;
  double maxp=0,relint=0,sump=0;
  int permutationen=0;

  printf("\n");
 
  for(d=peaks;d;d=d->next)
    {
      permutationen++;
      sump+=d->p;
      d->mass=d->mass / charge;
      d->mass=(rint( d->mass * pow(10,digits) ) / pow(10,digits) );
    }

  summarize_peaks();
  for(d=peaks;d;d=d->next)
    if(d->p > maxp)
      maxp=d->p;

  for(d=peaks;d;d=d->next)
    {
      if( ( relint=(d->p/maxp)*100) > MIN_INT )
	printf("M= %f, p= %e, rel. Int.= %f%%\n",
	       d->mass,d->p,relint);
    }
  if(!(fast_calc))
    printf("\nNumber of  permutations: %i\n",permutationen);
  else
    {
      sump=(rint(sump*10000)/100); 
      printf("\nCovered Intensity: %2.2f%% \n",sump);
    }
}

int TIPC::ipc_main2 ( const char *filename , const char *aaseq , int f )
	{
    char *gnuplotfile=NULL;
    
    verbindung=NULL;
    peaks=NULL;
    fast_calc=f; // -f
    
    if(!element->init_elements()) return 1 ;
    
	if(!(gnuplotfile=strdup(filename))) return 1 ;

	if(!pars->pars_amino_acid((char*)aaseq)) return 2 ; // -a

    if(!calculate_peaks()) return 3 ;
    
    if(!(gpout->make_gnuplot_output(gnuplotfile))) return 4 ;
    
    if ( verbindung ) delete verbindung ;
    if ( peaks ) delete peaks ;

	return 0 ;
	}    

int TIPC::ipc_main(int argc,char **argv){
/*  long seconds;
  int d=1,zeig_summenformel=0,calc_peaks=1,gnuplot=0,use_digits=USE_DIGITS,charge=1;
  char *gnuplotfile=NULL;

  verbindung=NULL;
  peaks=NULL;
  fast_calc=0;
  
  if(!argv[d])
    {
      usage();
      return 1;
    }

#ifdef HAVE_SIGNAL
  signal(SIGHUP,SIG_IGN);
#endif

  if(!init_elements()){
   printf("Error in init_elements\n");
   return 100;
  }
  while(argv[d])
    {
      if(!strcmp(argv[d],"-f"))
	{
	  d++;
	  if(!argv[d])
	    {
	      printf("Missing argument for -f\n");
	      return 101;
	    }
	  fast_calc=strtol(argv[d],NULL,10);
	}

      else if(!strcmp(argv[d],"-z"))
	{
	  d++;
	  if(!argv[d])
	    {
	      printf("Missing argument for -z\n");
	      return 2;
	    }
	  charge=strtol(argv[d],NULL,10);
	}

      else if(!strcmp(argv[d],"-d"))
	{
	  d++;
	  if(!argv[d])
	    {
	      printf("Missing argument for -d\n");
	      return 3;
	    }
	  use_digits=strtol(argv[d],NULL,10);
	}

      else if(!strcmp(argv[d],"-c")){
	d++;
	if(!argv[d])
	  {
	    printf("Missing argument for -c\n");
	    return 4;
	  }
	 if(!pars_chem_form(argv[d])){
	   printf("Parser error.\n");
	   return 5;
	 }
      }

      else if(!strcmp(argv[d],"-g")){
	d++;
	if(!argv[d]){
	  printf("Missing argument for -g\n");
	  return 6;
	}
	gnuplot=1;
	if(!(gnuplotfile=strdup(argv[d]))){
	  printf("Not enough memory\n");
	  return 7;
	}
      }


      else if(!strcmp(argv[d],"-p")){
	d++;
	if(!argv[d]){
	  printf("Missing argument for -p\n");
	  return 8;
	}
	if(!(pars_peptid(argv[d]))){
	  printf("Error in peptid parser.\n");
	  return 9;
	}
      }

      else if(!strcmp(argv[d],"-a")){
	d++;
	if(!argv[d]){
	  printf("Missing argument for -a\n");
	  return 10;
	}
	if(!pars_amino_acid(argv[d])){
	  printf("Error in pars_amino_acid.\n");
	  return 11;
	}
      }

      else if(!strcmp(argv[d],"-s"))
	zeig_summenformel=1;

      else if(!strcmp(argv[d],"-h"))
	{
	  usage();
	  return 12;
	}

      else if(!strcmp(argv[d],"-x"))
	calc_peaks=0;

      else
	{
	  printf("Unknown flag: %s\n",argv[d]);
	  usage();
	  return 13;
	}
      d++;
      }

  if(zeig_summenformel)
    {
      if(!print_sum())
	{
	  printf("error while showing chemical formula.\n");
	  return 14;
	}
    }
#ifdef HAVE_TIME
  seconds=time(0);
#endif

  if(calc_peaks)
    if(!calculate_peaks()){
      printf("Error in calculate_peaks\n");
      return 15;
    }

  print_result(use_digits,charge);

#ifdef HAVE_TIME
  seconds=time(0)-seconds;
  printf("Computing time: %li seconds.\n",seconds);
#endif

 if(gnuplot)
    if(!(make_gnuplot_output(gnuplotfile))){
      printf("Error while generating gnuplot file\n");
      return 16;
    }

  */
  return 0;
}


void TIPC::usage()
{
  printf("\nThis is IPC v %s\nCopyright Dirk Nolting 2001-2004\n\n",IPC_VERSION);
  printf("\nSynopsis:\n ipc -d <int> -z <int> -f <int> <-a <amino acid> -c <chemical formula> -p <File> -g <name> -s -x -h\n\n");

  printf("-c <chemical formula> calculates the isotopic pattern of the \n");
  printf("   given chemical formula. Additive with -p\n");

  printf("-p <File> reads peptide sequenz (one letter notation) from the \n");
  printf("   given file and calculates the isotopic pattern. Additive with -c\n");

  printf("-a <amino acids> calculate isotopic pattern from given amino acids\n");
  printf("   in one letter notation\n");

  printf("-s gives the chemical formula. Usefull for -a or -p\n");
  printf("-g creates gnuplot output and stores it in the file <name> and <name>.gnu\n");

  printf("-x no calculation of the isotopic pattern. Usefull for -s\n");
  printf("-f fast calc, calculates only first <int> peaks\n");
  printf("-d <int> digits significant\n");
  printf("-z assume <int> charges on ion \n");
  printf("-h show this text\n\n");
}
