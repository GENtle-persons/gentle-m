
#define MAXHELPLENGTH 50000
#define MAXHELPW 2

#define DCOLS  60     /*   sequence display width */
#define DNAMES  15
#define DNUMBER 6
#define SCREENMARGIN 10
#define SCOREHEIGHT 50

#define MAXMENU 20
#define MAXCOLORS 16

#define MESSLENGTH 70
#define MESSLINES 10
#define MAXPROMPTLEN 300

#define MULTIPLEM 1
#define PROFILEM 2

#define NEW 1
#define OLD 0

#define HIGHLIGHT 1
#define NORMAL 0

#define MARGIN 1

#define A4 0
#define A3 1
#define USLETTER 2

#define PORTRAIT 1
#define LANDSCAPE 0

#define MAXFINDSTR 20

/// Part of the ClustalW package
typedef struct aln_pos {
	int seq;
	int res;
} aln_pos;

/// Part of the ClustalW package
typedef struct menu_item {
	int num;
	IteM i[MAXMENU];
	int mode[MAXMENU];
	char ptr[MAXMENU];
} menu_item;

/// Part of the ClustalW package
typedef struct color {
	char name[20];
	Uint4 val;
	float r,g,b;
	float pr,pg,pb;
} color;

#define NAMES 0
#define SEQS 1

/// Part of the ClustalW package
typedef struct panel_data {
	int type;                  /* = NAMES or SEQS */
	PaneL index;
	int   prf_no;
	char **lines;
	char **header;
	char **footer;
	char **colormask;
	int nhead;
	int nfoot;
        PnlActnProc callback;
	int pixelheight;
	int pixelwidth;
        int vlines;
        int vseqs;
        int vcols;
        int nseqs;
        int ncols;
        int firstseq;
        int firstvline;
        int firstvcol;
        int lockoffset;
        int *selected;
	int firstsel;
	int lastsel;
        int lineheight, charwidth, ascent, descent;
	BaR vscrollbar,hscrollbar;
	int *seqweight;
	int *subgroup;
	int *colscore;
	char *consensus;
	Boolean **residue_exception;
	short **segment_exception;
} panel_data;

/// Part of the ClustalW package
typedef struct spanel {
        PaneL names;
        PaneL seqs;
	Boolean modified;
} spanel;

/// Part of the ClustalW package
typedef struct range {
        int first;
        int last;
} range;

/*
   PROTOTYPES - subroutines with capitalised names are defined in NCBI toolkit
                and cannot be modified!
*/
void x_menu(void);
void ResizeWindowProc(WindoW w);

void shift(Handle a, int dx, int dy);
void swap(float *scores,int s1, int s2);
void sort_scores(float *scores,int f,int l);
void reload_alignment(void);

void DrawPanel(PaneL p);

void resize_multi_window(void);
void resize_prf_window(int numseqs1,int numseqs2);
void position_scrollbars(spanel p);

void color_seqs(void);
void color_prf1(void);
void color_prf2(void);

void select_panel(spanel p);
void deselect_panel(spanel p);
void correct_name_bars(Boolean reset);
void correct_seq_bars(Boolean reset);
void load_aln_data(spanel p, int fs, int ls, Boolean reset);
void load_aln(spanel p, int fs, int ls, Boolean reset);
void remove_gap_pos(int fseq,int lseq,int prf_no);

GrouP make_scroll_area(GrouP w,int prf_no,int nwidth,int swidth,int height,int firstseq,int nseqs,spanel *p);

void draw_seq_pointer(PaneL p,int pos,int format);
void draw_names(PaneL p);
void draw_seqs(PaneL p);
void draw_header(PaneL p);
void draw_footer(PaneL p);
void draw_colscores(PaneL p);
void draw_allseqs(PaneL p,int fseq,int lseq);
void draw_nameline(PaneL p,int fseq,int lseq,int format);
void draw_seqline(panel_data data,int seq,PoinT pt,int fcol,int lcol,int format);
void draw_seqcol(PaneL p,int col,int format);
void highlight_seqrange(PaneL p,int fcol,int lcol,int format);
void make_ruler(int length,char *name,char *seq);
void make_consensus(panel_data data,char *name,char *seq);
void make_colscores(panel_data data);
int calc_colscore(sint matrix[NUMRES][NUMRES],int col);
void calc_seg_exceptions(void);
int make_struct_data(int prf_no,int length,char *name,char *seq);
int make_gp_data(int prf_no,int length,char *name,char *seq);
void make_colormask(panel_data data);
void init_color_parameters(char *filename);
char *find_file(char *filename);
void white_on_black(void);
void black_on_white(void);
void text_colors(void);
void data_colors(void);
void switch_mode(void);
void show_segment_exceptions(void);
void check_menus(menu_item m,int mode);
FILE *  open_input_file(char *file_name);
void stripspace(char *str);

void set_scorecutoff(BaR bar, GraphiC p, Nlm_Int2 newval, Nlm_Int2 oldval);
void set_lengthcutoff(BaR bar, GraphiC p, Nlm_Int2 newval, Nlm_Int2 oldval);
void set_scorescale(BaR bar, GraphiC p, Nlm_Int2 newval, Nlm_Int2 oldval);
void set_go_penalty(TexT t);
void set_ge_penalty(TexT t);
void set_gap_dist(TexT t);
void set_ntrials(TexT t);
void set_ran_seed(TexT t);
void set_div_seq(TexT t);
void set_pw_go_penalty(TexT t);
void set_pw_ge_penalty(TexT t);
void set_gp(TexT t);
void set_ktuple(TexT t);
void set_topdiags(TexT t);
void set_window(TexT t);
void set_hyd_res(TexT t);
void set_matrix(GrouP g);
void set_dnamatrix(GrouP g);
void set_user_matrix(ButtoN but);
void set_user_dnamatrix(ButtoN but);
int get_user_matrixname(char *usermtrxname,short *usermat,short *aa_xref,int usermatnum,int *matnum,PrompT mattext);
void set_format(GrouP g);
void set_button(ButtoN l,Boolean *value);
void set_toggle(PopuP l,Boolean *value);
void set_pref_penalties(PopuP l);
void set_hyd_penalties(PopuP l);
void set_var_penalties(PopuP l);
void set_endgaps(PopuP l);
void set_align_endgappenalties(PopuP l);
void set_realign_endgappenalties(PopuP l);
void set_case(PopuP l);
void set_snos(PopuP l);

void setRange(PopuP l);

void set_save_paras(PopuP l);
void set_transitions(TexT t);
void set_save_log(IteM i);
void set_neg_matrix(PopuP l);
void set_ambiguities(PopuP l);
void set_aln_mode(PopuP g);
void set_pscroll_mode(ButtoN l);
void set_show_segments(IteM l);
void set_font_size(PopuP g);
void set_residue_exceptions(IteM i);
void set_segment_exceptions(IteM i);
void set_segment_dnascale(BaR bar, GraphiC p, Nlm_Int2 newval, Nlm_Int2 oldval);
void set_fs_toggle(PopuP l);
void set_score_matrix(GrouP g);
void set_segment_matrix(GrouP g);
void set_score_user_matrix(ButtoN but);
void set_segment_user_matrix(ButtoN but);
void set_score_dnamatrix(GrouP g);
void set_segment_dnamatrix(GrouP g);
void set_score_user_dnamatrix(ButtoN but);
void set_segment_user_dnamatrix(ButtoN but);
void set_pagesize(PopuP g);
void set_orientation(PopuP g);
void set_header(PopuP l);
void set_ruler(PopuP l);
void set_curve(PopuP l);
void set_resno(PopuP l);
void set_resize(PopuP l);
void set_fres(TexT t);
void set_lres(TexT t);
void set_fpres(TexT t);
void set_lpres(TexT t);
void set_blocklen(TexT t);
void set_output_clustal(ButtoN l);
void set_output_nbrf(ButtoN l);
void set_output_phylip(ButtoN l);
void set_output_gcg(ButtoN l);
void set_output_gde(ButtoN l);
void set_output_nexus(ButtoN l);

void set_output_pim(ButtoN l); /* Ramu */

void set_output_fasta(ButtoN l); /* Ramu */

void set_pw_matrix(GrouP g);
void set_pw_dnamatrix(GrouP g);
void set_pw_user_matrix(ButtoN but);
void set_pw_user_dnamatrix(ButtoN but);
void set_output_order(PopuP g);
void set_output_tree_clustal(ButtoN l);
void set_output_tree_phylip(ButtoN l);
void set_output_tree_distances(ButtoN l);
void set_output_tree_nexus(ButtoN l);
void set_inverted(IteM i);
void set_tossgaps(IteM i);
void set_kimura(IteM i);
void set_boot_format(PopuP g);
void set_use_ss1(PopuP l);
void set_use_ss2(PopuP l);
void set_helix_gp(TexT t);
void set_strand_gp(TexT t);
void set_loop_gp(TexT t);
void set_terminal_gp(TexT t);
void set_helix_minus(TexT t);
void set_helix_plus(TexT t);
void set_strand_minus(TexT t);
void set_strand_plus(TexT t);
void set_ss_output(ButtoN b);
void set_gp_output(ButtoN b);
void calc_segment_exceptions(IteM i);

void write_ps_file(spanel p,char *ps_file,char *par_file,int pagesize,
int orientation,Boolean header, Boolean ruler, Boolean resno, Boolean resize,
int first_printres,int last_printres,
int blocklen,Boolean show_curve);

