#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <string>

using namespace std ;

#define AAs "A_CDEFGHI_KLMN_PQRST_VW_Y_"
#define PI  3.1415

/* Include file for Ncoils */

struct hept_pref {
	float **m;		/* 20 x 7 amino acid heptad weights */
	float smallest;		/* Smallest of the above */
	int n;			/* statistical fitting data (weighted) */
	struct fit_dat *f;
};

struct fit_dat {
	int win;		/* Window length */
	float m_cc, sd_cc;	/* mean/sd for coiled-coils */
	float m_g,  sd_g;	/* mean/sd for globular */
	float sc;		/* scaling factor */
	int w;			/* 1= weighted, 0=un-weighted */
};


struct hept_pref *read_matrix(FILE *MAT);
void exit_error();
string pred_coils(char *seq,char *ident,char *title,struct hept_pref *h,int win,int which,
   int weighted,int fasta,float min_P, int *t, int *tc, int min_segs);

string ncoils_function( char *_sequence ) ;
