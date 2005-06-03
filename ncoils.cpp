#include "ncoils.h"

//#include "wx/wx.h"

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


/* Rob Russell's attempt to make a COILS program */

string ncoils_function( const char *_sequence , int window ) {
	
	string ret ;

	int i,j,k,l;
	int verb;
//	int window;
	int pt;
	int which,weighted;
	int nseq;
	int t,tc;
	int seqlen;
	int mode;
	int min_seg;

	char heptfile[1000];
	char *buff;
	static char *env;
	char *seq,*title,*ident;

	float min_P;

	struct hept_pref *h;

	FILE *MAT;


	/* defaults */
//	window = 21;
	weighted = 0;
	verb = 0;
	mode = 0; /* 0 = column mode, 1 = fasta, 2 = concise */
	min_P = 0.5;

	if((env=getenv("COILSDIR"))==NULL) {
		fprintf(stderr,"error: environment variable COILSDIR must be set\n");
		exit(-1);
	}

	strcpy(&heptfile[0],env);
	strcpy(&heptfile[strlen(heptfile)],"/ncoils.matrix");

/*
  	// Ignoring command line parameters in a function...
	for(i=1; i<argc; ++i) {
           if(argv[i][0]!='-') exit_error();
	   if(strcmp(&argv[i][1],"m")==0) {
             if((i+1)>=argc) exit_error();
             strcpy(&heptfile[0],argv[i+1]);
             i++;
	   } else if(strcmp(&argv[i][1],"win")==0) {
             if((i+1)>=argc) exit_error();
             sscanf(argv[i+1],"%d",&window);
             i++;
	   } else if(strcmp(&argv[i][1],"c")==0) {
             mode=2;
	   } else if(strcmp(&argv[i][1],"min_seg")==0) {
             if((i+1)>=argc) exit_error();
             sscanf(argv[i+1],"%d",&min_seg);
             i++;
	   } else if(strcmp(&argv[i][1],"c")==0) {
             mode=2;
	   } else if((strcmp(&argv[i][1],"f")==0) || (strcmp(&argv[i][1],"fasta")==0)) {
	     mode=1;
	   } else if((strcmp(&argv[i][1],"min_P")==0)) {
             if((i+1)>=argc) exit_error();
             sscanf(argv[i+1],"%f",&min_P);
             i++;
	   } else if(strcmp(&argv[i][1],"help")==0) {
	     exit_error();
	   } else if(strcmp(&argv[i][1],"w")==0) {
	     weighted=1;
	   } else if(strcmp(&argv[i][1],"V")==0 || strcmp(&argv[i][1],"v")==0) {
	     verb=1;
           } else {
	     fprintf(stderr," can't understand flag/field %s\n",argv[i]);
             exit_error();
           }
        }
*/

	if(verb) printf("Matrix file is %s\n",heptfile);
	/* Read in matrix */
	if((MAT=fopen(heptfile,"r"))==NULL) {
		fprintf(stderr,"Error reading %s\n",heptfile);
		exit(-1);
	}
	h = read_matrix(MAT);
	if(verb) {
	   for(i=0; i<strlen(AAs); ++i) if(AAs[i]!='_') {
		pt=(int)(AAs[i]-'A');
		printf("AA %c %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f\n",AAs[i],
	     	   h->m[pt][0],h->m[pt][1],h->m[pt][2],h->m[pt][3],h->m[pt][4],
	     	   h->m[pt][5],h->m[pt][6]);
	   }
	   for(i=0; i<h->n; ++i) {
		printf("Window %4d %1d %f %f %f %f %f\n",
			h->f[i].win,h->f[i].w,h->f[i].m_cc,h->f[i].sd_cc,h->f[i].m_g,h->f[i].sd_g,h->f[i].sc);
	   }
	}

	/* See if there is a file for our chosen window length/weight scheme */
	which = -1;
	for(i=0; i<h->n; ++i) {
		if((h->f[i].win == window) && (h->f[i].w == weighted)) { /* match */
			if(verb) printf("Found fitting data for win %4d w %d\n",window,weighted);
			which = i;
		}
	}

	/* Read in a sequence from the standard input */
	nseq = 0;
	ident = (char*) malloc(100*sizeof(char));
	title = (char*) malloc(strlen(_sequence)*sizeof(char)+9);
	buff = (char*) malloc(strlen(_sequence)*sizeof(char)+9);
	seq = (char*) malloc(strlen(_sequence)*sizeof(char)+9);
	seqlen = 0 ;
	strcpy ( buff , _sequence ) ;
	strcpy ( seq , _sequence ) ;
	t = 0;
	tc = 0;
	
	strcpy ( title , "TEST!" ) ;

	/* There is a memory problem - the matrix data gets corrupted under OSF after this fgets */
	for(i=0; i<strlen(buff); ++i) if(buff[i]=='\n' || buff[i]=='\r') buff[i]='\0';
/*
	if(buff[0]=='>') {
		if(nseq>0) { 
			pred_coils(seq,ident,title,h,window,which,weighted,mode,min_P,&t,&tc,min_seg); 
			free(seq);
		}
        seqlen = 0;
		i=1;
		while((buff[i]!=' ') && (buff[i]!='\0') && (buff[i]!='\n') && (buff[i]!='\r')) { ident[i-1]=buff[i]; i++; }
		ident[i-1]='\0';
		i++; j=0;
		seq = (char*)malloc(sizeof(char));
		seq[0]='\0';
		while(buff[i]!='\n' && buff[i]!='\0' && buff[i]!='\r') {
			title[j]=buff[i]; i++;
			j++;
		}
		title[j]='\0';
		nseq++;
	} else {

		seq=(char*)realloc(seq,(seqlen+strlen(buff)+1)*sizeof(char));
		strcpy(&seq[seqlen],buff); 
		seqlen=strlen(seq);

	}
*/
//	return "!" ;

	strcpy(seq,buff); 
	seqlen=strlen(seq);
	
//	if(nseq>0) { 
		ret = pred_coils(seq,ident,title,h,window,which,weighted,mode,min_P,&t,&tc,min_seg); 
		free(seq);
//	}
	fprintf(stderr,"%8d sequences %8d aas %8d in coil\n",nseq,t,tc);
	free(title); free(ident); 

	return ret ;

}

int ncoils_main(int argc, char *argv[]) {

	int i,j,k,l;
	int verb;
	int window,pt;
	int which,weighted;
	int nseq;
	int t,tc;
	int seqlen;
	int mode;
	int min_seg;

	char heptfile[1000];
	char *buff;
	static char *env;
	char *seq,*title,*ident;

	float min_P;

	struct hept_pref *h;

	FILE *MAT;


	/* defaults */
	window = 21;
	weighted = 0;
	verb = 0;
	mode = 0; /* 0 = column mode, 1 = fasta, 2 = concise */
	min_P = 0.5;

	if((env=getenv("COILSDIR"))==NULL) {
		fprintf(stderr,"error: environment variable COILSDIR must be set\n");
		exit(-1);
	}

	strcpy(&heptfile[0],env);
	strcpy(&heptfile[strlen(heptfile)],"/new.mat");


	for(i=1; i<argc; ++i) {
           if(argv[i][0]!='-') exit_error();
	   if(strcmp(&argv[i][1],"m")==0) {
             if((i+1)>=argc) exit_error();
             strcpy(&heptfile[0],argv[i+1]);
             i++;
	   } else if(strcmp(&argv[i][1],"win")==0) {
             if((i+1)>=argc) exit_error();
             sscanf(argv[i+1],"%d",&window);
             i++;
	   } else if(strcmp(&argv[i][1],"c")==0) {
             mode=2;
	   } else if(strcmp(&argv[i][1],"min_seg")==0) {
             if((i+1)>=argc) exit_error();
             sscanf(argv[i+1],"%d",&min_seg);
             i++;
	   } else if(strcmp(&argv[i][1],"c")==0) {
             mode=2;
	   } else if((strcmp(&argv[i][1],"f")==0) || (strcmp(&argv[i][1],"fasta")==0)) {
	     mode=1;
	   } else if((strcmp(&argv[i][1],"min_P")==0)) {
             if((i+1)>=argc) exit_error();
             sscanf(argv[i+1],"%f",&min_P);
             i++;
	   } else if(strcmp(&argv[i][1],"help")==0) {
	     exit_error();
	   } else if(strcmp(&argv[i][1],"w")==0) {
	     weighted=1;
	   } else if(strcmp(&argv[i][1],"V")==0 || strcmp(&argv[i][1],"v")==0) {
	     verb=1;
           } else {
	     fprintf(stderr," can't understand flag/field %s\n",argv[i]);
             exit_error();
           }
        }

	if(verb) printf("Matrix file is %s\n",heptfile);
	/* Read in matrix */
	if((MAT=fopen(heptfile,"r"))==NULL) {
		fprintf(stderr,"Error reading %s\n",heptfile);
		exit(-1);
	}
	h = read_matrix(MAT);
	if(verb) {
	   for(i=0; i<strlen(AAs); ++i) if(AAs[i]!='_') {
		pt=(int)(AAs[i]-'A');
		printf("AA %c %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f\n",AAs[i],
	     	   h->m[pt][0],h->m[pt][1],h->m[pt][2],h->m[pt][3],h->m[pt][4],
	     	   h->m[pt][5],h->m[pt][6]);
	   }
	   for(i=0; i<h->n; ++i) {
		printf("Window %4d %1d %f %f %f %f %f\n",
			h->f[i].win,h->f[i].w,h->f[i].m_cc,h->f[i].sd_cc,h->f[i].m_g,h->f[i].sd_g,h->f[i].sc);
	   }
	}

	/* See if there is a file for our chosen window length/weight scheme */
	which = -1;
	for(i=0; i<h->n; ++i) {
		if((h->f[i].win == window) && (h->f[i].w == weighted)) { /* match */
			if(verb) printf("Found fitting data for win %4d w %d\n",window,weighted);
			which = i;
		}
	}

	/* Read in a sequence from the standard input */
	nseq = 0;
	ident = (char*) malloc(100*sizeof(char));
	title = (char*) malloc(100000*sizeof(char));
	buff  = (char*) malloc(100000*sizeof(char));
	t = 0;
	tc = 0;
	while(fgets(buff,99999,stdin)!=NULL) {
		/* There is a memory problem - the matrix data gets corrupted under OSF after this fgets */
		for(i=0; i<strlen(buff); ++i) if(buff[i]=='\n' || buff[i]=='\r') buff[i]='\0';
		if(buff[0]=='>') {
			if(nseq>0) { 
				pred_coils(seq,ident,title,h,window,which,weighted,mode,min_P,&t,&tc,min_seg); 
				free(seq);
			}
                        seqlen = 0;
			i=1;
			while((buff[i]!=' ') && (buff[i]!='\0') && (buff[i]!='\n') && (buff[i]!='\r')) { ident[i-1]=buff[i]; i++; }
			ident[i-1]='\0';
			i++; j=0;
			seq = (char*)malloc(sizeof(char));
			seq[0]='\0';
			while(buff[i]!='\n' && buff[i]!='\0' && buff[i]!='\r') {
				title[j]=buff[i]; i++;
				j++;
			}
			title[j]='\0';
			nseq++;
		} else {
/*			printf("Adding |%s| to |%s| = \n",buff,seq);  */
			seq=(char*)realloc(seq,(seqlen+strlen(buff)+1)*sizeof(char));
		        strcpy(&seq[seqlen],buff); 
                        seqlen=strlen(seq);
/*			printf("       |%s|\n",seq);  */
		}
	}
	if(nseq>0) { 
		pred_coils(seq,ident,title,h,window,which,weighted,mode,min_P,&t,&tc,min_seg); 
		free(seq);
	}
	fprintf(stderr,"%8d sequences %8d aas %8d in coil\n",nseq,t,tc);
	free(title); free(ident); 

	exit(0);

}

void exit_error() {
	fprintf(stderr,"NCOILS: R.B. Russell, A.N. Lupas, 1999\n");
	fprintf(stderr," based on Lupas, Van Dyck & Stock (1991) Science 252,1162-1164\n");
	fprintf(stderr,"format: ncoils [options] < [sequence file]\n");
	fprintf(stderr,"       -f or -fasta        [fasta output - coils as 'x', like '-x' in seg]\n");
	fprintf(stderr,"       -c                  [concise mode - which sequences have any coils (and how many)]\n");
	fprintf(stderr,"       -min_seg <int>      [for concise mode - only report sequence if >= min coil segments]\n");
	fprintf(stderr,"       -min_P <float>      [minimum P to define coil segment; DEFAULT = 0.5]\n");
	fprintf(stderr,"       -win <int>          [window size; DEFAULT = 21]\n");
	fprintf(stderr,"       -w                  [weight heptad positions a&d the same as b,c,e,f,g]\n");
	fprintf(stderr,"       -v                  [verbose/debug mode - print extra junk]\n");
	fprintf(stderr,"       -max_seq_len <int>  [longest sequence tolerated; DEFAULT = 100 000]\n");
	exit(-1); 
}

string pred_coils(char *seq,char *ident,char *title,struct hept_pref *h,int win, int which, int weighted,int mode, float min_P, int *t, int *tc, int min_seg) {

    string ret ;

	int i,j;
	int len,pos,aa_pt;
	int pt;
	int total_coil_segments;
	int are_there_coils;

	float actual_win;
	float this_score,Gg,Gcc,power;
	float t1,t2,t3,t4;
	float *score;
	float *P;

	char tmp[10000] ;
	
	char *hept_seq;
	
	len=strlen(seq);

	score    = (float*)malloc(len*sizeof(float));
	P        = (float*)malloc(len*sizeof(float));
	hept_seq =  (char*)malloc(len*sizeof(char));

/*	printf("Sequence is %s length is %d\n",seq,len); */
	for(i=0; i<len; ++i) { P[i]=0.0; score[i] = 0.0; hept_seq[i] = 'x'; }

	for(i=0; i<(len-win+1); ++i) {
		this_score = 1.0;
		actual_win=0.0;
                for(j=0; ((j<win) && ((i+j)<len)); ++j) {
		   aa_pt = (int)(seq[i+j]-'A');
		   if((aa_pt>=0) && (aa_pt<26) && (AAs[aa_pt]!='_')) {
			pos = j%7; /* where are we in the heptad?  pos modulus 7 */
/*			printf("AA %c in hept %c %7.5f\n",seq[i+j],('a'+pos),h->m[aa_pt][pos]);  */
			if(weighted && (pos==0 || pos==3)) { power = 2.5; }
			else { power = 1.0; }
			actual_win+=power;
			if(h->m[aa_pt][pos]!=-1) {
				this_score*=pow(h->m[aa_pt][pos],power);
			} else {
				this_score*=pow(h->smallest,power);
			}
		    }
                }
		if(actual_win>0) {
		   this_score = pow(this_score,(1/(float)actual_win));
		} else {
		   this_score = 0;
	        }
                for(j=0; ((j<win) && ((i+j)<len)); ++j) {
		    aa_pt = (int)(seq[i+j]-'A');
		    if((aa_pt>=0) && (aa_pt<26) && (AAs[aa_pt]!='_')) {
			pos = j%7; /* where are we in the heptad?  pos modulus 7 */
			if(this_score>score[i+j]) { score[i+j]=this_score; hept_seq[i+j]='a'+pos; }
		    }
		}
       }


	if(mode==1) {
		printf(">%s %s\n",ident,title);
	}
	are_there_coils=0;
	total_coil_segments=0;
	for(i=0; i<len; ++i) {
		/* Calculate P */
		t1 = 1/(h->f[which].sd_cc);
		t2 = (score[i]-(h->f[which].m_cc))/h->f[which].sd_cc;
		t3 = fabs(t2);
		t4 = pow(t3,2);
		t4 = t3*t3;
		Gcc = t1 * exp(-0.5*t4);
/*		printf("Gcc %f %f %f %f %f\n",t1cc,t2cc,t3cc,t4cc,Gcc); */
		t1 = 1/(h->f[which].sd_g);
		t2 = (score[i]-(h->f[which].m_g))/h->f[which].sd_g;
		t3 = fabs(t2);
		t4 = pow(t3,2);
		t4 = t3*t3;
		Gg = t1 * exp(-0.5*t4);
/*		printf("Gg %f %f %f %f %f\n",t1g,t2g,t3g,t4g,Gg); */
		P[i] = Gcc/(h->f[which].sc*Gg+Gcc);
		if(P[i]>=min_P) {
                   are_there_coils=1;
                   if((i==0) || (P[i-1]<min_P)) { total_coil_segments++; }
		   (*tc)++; 
                }
		(*t)++;
		if(mode==1) {
			if(P[i]>=min_P) ret += "x" ; //{ printf("x"); }
			else ret += seq[i] ;//{ printf("%c",seq[i]); }
			if(((i+1)%60)==0) ret += "\n" ; //{ printf("\n"); }
		} else if(mode==0) {
			sprintf(tmp,"%4d %c %c %7.3f %7.3f (%7.3f %7.3f)\n",i+1,seq[i],hept_seq[i],score[i],P[i],Gcc,Gg);
			ret += tmp ;
		}
	}
	if(mode==1) { printf("\n"); } 
        if((mode==2) && (are_there_coils==1) && (total_coil_segments>=min_seg)) {
		if(total_coil_segments==1) {
			sprintf(tmp,"Pred %4d coil segment  : %s %s\n",total_coil_segments,ident,title);
//			ret += tmp ;
		} else {
			sprintf(tmp,"Pred %4d coil segments : %s %s\n",total_coil_segments,ident,title);
//			ret += tmp ;
		}
	}

	free(P); free(score); free(hept_seq);
	
	return ret ;
}


struct hept_pref *read_matrix(FILE *IN) {

	int i,j;
	int pt,aa_len;
	int win;

	float m_g,sd_g,m_cc,sd_cc,sc;
	float hept[7];

	char buff[1000],junk[1000];

	struct hept_pref *h;

	aa_len = strlen(AAs);

	h = (struct hept_pref*)malloc(sizeof(struct hept_pref));

	h->m = (float**)malloc(aa_len*sizeof(float*));
	for(i=0; i<aa_len; ++i) {
		h->m[i]=(float*)malloc(7*sizeof(float));
		for(j=0; j<7; ++j) {
			h->m[i][j]=-1;
		}
	}
	h->f = (struct fit_dat*)malloc(sizeof(struct fit_dat));
	h->n = 0;
	h->smallest=1.0;

	while(fgets(buff,999,IN)!=NULL) {
		if(buff[0]!='%') {
		   if((strncmp(buff,"uw ",3)==0) || (strncmp(buff,"w ",2)==0)) {
			i = h->n;
		        if(strncmp(buff,"uw ",3)==0) { h->f[i].w=0; }
			else { h->f[i].w=1; }
			sscanf(buff,"%s %d %f %f %f %f %f",
				&junk[0],&win,&m_cc,&sd_cc,&m_g,&sd_g,&sc);
				h->f[i].win   = win;
				h->f[i].m_cc  = (float)m_cc; 
				h->f[i].sd_cc = (float)sd_cc;
				h->f[i].m_g   = (float)m_g;
				h->f[i].sd_g  = (float)sd_g;
				h->f[i].sc    = (float)sc;
			h->n++;
			h->f = (struct fit_dat*)realloc(h->f,((h->n)+1)*sizeof(struct fit_dat)); 
			if((h->n)>=9) { 
				fprintf(stderr,"Error: too many window parms in matrix file\n");
				exit(-1);
			}
		    } else if(buff[0]>='A' && buff[0]<='Z') { /* AA data */
			pt = (int)(buff[0]-'A');
			if(h->m[pt][0]==-1) {
				sscanf(buff,"%s%f%f%f%f%f%f%f",
					&junk[0],
					&hept[0],&hept[1],&hept[2],&hept[3],&hept[4],
					&hept[5],&hept[6]);
				for(i=0; i<7; ++i) {
					h->m[pt][i] = (float)hept[i];
					if(h->m[pt][i]>0) {
						if(h->m[pt][i]<h->smallest) { h->smallest = h->m[pt][i]; }
					} else {
						h->m[pt][i]=-1; /* Don't permit zero values */
					}
				}
/*				printf("AA %c %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f\n",buff[0],
					h->m[pt][0],h->m[pt][1],h->m[pt][2],h->m[pt][3],h->m[pt][4],
					h->m[pt][5],h->m[pt][6]);
*/

			} else {
				fprintf(stderr,"Warning: multiple entries for AA %c in matrix file\n",buff[0]);
			}
		    } else {
			fprintf(stderr,"Warning: strange characters in matrix file\n");
			fprintf(stderr,"Ignoring line: %s",buff);
		    }
		}
	}
	return h;
}

