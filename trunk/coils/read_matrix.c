#include <ncoils.h>


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

