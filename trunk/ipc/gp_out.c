//#include "global.h"
#include "ipc.h"
#include "gp_out.h"
#include <math.h>
#include <string.h>

GPOUT::GPOUT ( TIPC *i )
	{
	ipc = i ;
	}    

void GPOUT::free_spectrum(spec_points *spectrum)
{
  while(spectrum->next)
    {
      spectrum=spectrum->next;
      free(spectrum->previous);
    }
  free(spectrum);
}

void GPOUT::print_spectrum(spec_points *spectrum, FILE *output)
{
  while(spectrum)
    {
      fprintf(output,"%f, %f\n", spectrum->x, spectrum->y);
      spectrum=spectrum->next;
    }
}

spec_points *GPOUT::add_new_point(spec_points *spectrum)
{
  spec_points *dummy;

  if(!(dummy=(spec_points*)malloc(sizeof(spec_points))))
  {
    printf("Out of memory\n");
    return 0;
  }
  spectrum->next=dummy;
  dummy->previous=spectrum;
  dummy->next=NULL;
  return dummy;
}

spec_points *GPOUT::calc_points()
{
  isotope *reiter;
  spec_points *point, *reiter2;
  double maxp=0;
  int end;

  for(reiter=ipc->peaks;reiter;reiter=reiter->next)
    if(reiter->p > maxp)
      maxp=reiter->p;

  reiter=ipc->peaks;
  point=(spec_points*)malloc(sizeof(spec_points));
  point->previous=NULL;
  point->next=NULL;
  point->x=floor(reiter->mass-1);
  point->y=0;
  reiter2=point;

  while(reiter)
    {
      if( (reiter->p)*10000 > maxp )
	{
	    
	  if(!(reiter2=add_new_point(reiter2)))
	    return 0;
  
	  reiter2->y=0;
	  reiter2->x=reiter->mass;

	  if(!(reiter2=add_new_point(reiter2)))
	    return 0;
	  reiter2->y=(reiter->p / maxp) * 100;
	  reiter2->x=reiter->mass;

	  if(!(reiter2=add_new_point(reiter2)))
	    return 0;
	  reiter2->y=0;
	  reiter2->x=reiter->mass;
	}
      reiter=reiter->next;
    }

  end=1+reiter2->x;
  if(!(reiter2=add_new_point(reiter2)))
    return 0;
  reiter2->x=end;
  reiter2->y=0;

  return point;
}

int GPOUT::make_gnuplot_output(char *gnuplotfile)
{ 
  char *data_file,*start_gp;
  spec_points *spectrum, *reiter;
  FILE *gp,*data;
  float xmin,xmax;

  data_file=(char *) strdup(gnuplotfile);
  
  free ( gnuplotfile ) ;
  if(!(gnuplotfile=(char*)malloc(strlen(data_file)+4)))
    return 0;  
  strcpy(gnuplotfile,data_file);
  strcat(gnuplotfile,".gnu");

  if(!(spectrum=calc_points())){
    printf("Error in calc_points");
    return 0;
  }

  if(!(data=fopen(data_file,"w")))
    {
      printf("Error while opening data file\n");
      return 0;
    }

  print_spectrum(spectrum, data);

  if(fclose(data))
    {
      printf("Error while closing data file\n");
      return 0;
    }

  if(!(gp=fopen(gnuplotfile,"w")))
    {
      printf("Error while opening gnuplot file\n");
      return 0;
    }

  xmin=spectrum->x;
  reiter=spectrum;
  while(reiter->next)
    reiter=reiter->next;
  xmax=reiter->x;

  fprintf(gp,"set xrange[%f:%f]\n",xmin,xmax);
  fprintf(gp,"set yrange[-1:101]\n");
  fprintf(gp,"set tics out\n");
  fprintf(gp,"set nox2tics\n");
  fprintf(gp,"set noy2tics\n");
  fprintf(gp,"set ylabel \"rel. Int.\"\n");
  fprintf(gp,"set xlabel \"m/z\"\n");
  fprintf(gp,"plot \"%s\" with lines\n",data_file);
  fprintf(gp,"pause -1 \"Press Return\"\n");

  if(fclose(gp))
    {
      printf("Error while closing gnuplot file\n");
      return 0;
    }

  free_spectrum(spectrum);

  if(!(start_gp=(char*)malloc(strlen(gnuplotfile)+8)))
    return 0;  
  strcpy(start_gp,"gnuplot ");
  strcat(start_gp,gnuplotfile);

#ifdef HAVE_SYSTEM

  if(!(NOT_RUN_GNUPLOT))
    system(start_gp);

#endif

  return 1;
}
