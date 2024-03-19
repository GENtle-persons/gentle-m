#ifndef _ENUMS_H_
#define _ENUMS_H_

enum {
    A___ = 6000,
    // Amino acid display menu
    AA_ALL,
    AA_KNOWN,
    AA_MAIN,
    AA_NONE,
    AA_ONE,
    AA_THREE,
    AA_THREE_1,
    AA_THREE_2,
    AA_THREE_3,
    AA_THREE_M1,
    AA_THREE_M2,
    AA_THREE_M3,
    ABI_HEIGHT,
    ABI_HELPLINES,
    ABI_INV_COMP,
    ABI_SLIDER,
    ABI_WIDTH,
    ABI_VIEW_ONLY,

    // Alignment
    AL_ADD,
    AL_ALG,
    AL_ALL,
    AL_CANCEL,
    AL_CUR,
    AL_DEL,
    AL_DOWN,
    AL_MATCH,
    AL_MATRIX,
    AL_MISMATCH,
    AL_OK,
    AL_PENALTY,
    AL_UP,
    ALIGN_BOLD,
    ALIGN_CONS,
    ALIGN_FEAT,
    ALIGN_HORIZ,
    ALIGN_IDENT,
    ALIGN_INVS,
    ALIGN_MONO,
    ALIGN_NORM,
    ALIGN_RNA,
    ALIGN_SEQ,
    ALIGN_SIML,
    ALIGN_SOA,
    ALIGN_SOAI,
    ALIGN_APPEARANCE,
    ALIGNMENT_SETTINGS,
    AMINOACIDS_BLAST_AA,
    AMINOACIDS_PHOTOMETER,
    AMINOACIDS_DESC,
    AMINOACIDS_EDIT_NAME,
    AMINOACIDS_SETTINGS,
    ED,
    ED_CANCEL,
    ED_OK,
    EE_BT_ICE,
    EE_LB_CE,
    EE_LB_GE,
    EE_LB_GR,
    EE_START,
    FRAME_SASH_1,
    FRAME_SASH_2,
    FRAME_SASH_BOTTOM,
    FRAME_SASH_TOP,
    IED_CANCEL,
    IED_COLOR,
    IED_LIST,
    IED_OK,
    IED_USEOFFSET,
    IV_BUTTON,
    IV_CB,
    IV_CB_INVERT,
    IV_IMAGE,
    IV_LIST,
    IV_MENU_COPY,
    IV_MENU_PRINT,
    IV_MENU_SAVE_AS_BITMAP,
    LD_CANCEL,
    LD_OK,
    LD_SOURCES,
    LD_TARGETS,
    MAIN_TREE_DUMMY,
    MCD_OK,

    // From ManageDatabase
    MD_PD_ADD,
    MD_PD_ADD_MYSQL,
    MD_PD_DBLIST,
    MD_PD_DEFAULT,
    MD_PD_DEL,
    MD_PM_DD_L,
    MD_PM_DD_R,
    MD_PM_DEL,
    MD_PM_EDIT,
    MD_PM_FILTER,
    MD_PM_FILTER_DESC,
    MD_PM_FILTER_DNA,
    MD_PM_FILTER_PRIMER,
    MD_PM_FILTER_PROTEIN,
    MD_PM_FILTER_SEQ,
    MD_PM_FILTER_ALIGN,
    MD_PM_LEFT,
    MD_PM_NEW,
    MD_PM_NEW_MYSQL,
    MD_PM_OPEN,
    MD_PM_REN,
    MD_PM_RIGHT,
    MD_PM_SAVE,
    MD_PM_SAVE_DB,
    MD_PM_TWOPANES,

    MDI_DUMMY_START,
    MDI_AS_NEW_FEATURE,
    MDI_CIRCULAR_LINEAR,
    MDI_COPY,
    MDI_COPY_TO_NEW,
    MDI_CUT,
    MDI_EDIT_MODE,
    MDI_EDIT_ORFS,
    MDI_EXPORT,
    MDI_EXTRACT_AA,
    MDI_FILL_KLENOW,
    MDI_FIND,
    MDI_MARK_ALL,
    MDI_ORFS,
    MDI_PASTE,
    MDI_PRINT_IMAGE,
    MDI_PRINT_REPORT,
    MDI_FILE_SAVE,
    MDI_FILE_SAVE_ALL,
    MDI_REDO,
    MDI_RESTRICTION,
    MDI_RUN_PCR,
    MDI_TOGGLE_FEATURES,
    MDI_TOGGLE_RESTRICTION,
    MDI_TRANSFORM_SEQUENCE,
    MDI_UNDO,
    MDI_VIEW_MODE,
    MDI_DUMMY_END,

    MDI_SEQUENCING_ASSISTANT,

    MDI_HOMEPAGE,
    MDI_ABOUT,
    MDI_ALIGNMENT,
    MDI_CALCULATOR,
    MDI_CHANGE_POSITION,
    MDI_CHANGE_SIZE,
    MDI_CHANGE_TITLE,
    MDI_CHILD_QUIT,
    MDI_ENZYME_EDITOR,
    MDI_FILE_IMPORT,
    MDI_FILE_OPEN,
    MDI_HELP,
    MDI_IMAGE_VIEWER,
    MDI_EXTERNAL_INTERFACE,
    MDI_LIGATION,
    MDI_CLONING_ASSISTANT,
    MDI_MANAGE_DATABASE,
    MDI_PROJECT_CLOSE,
    MDI_PROJECT_LOAD,
    MDI_PROJECT_SAVE,
    MDI_QUIT,
    MDI_REFRESH,
    MDI_TEXT_IMPORT,
    MDI_SEQUENCING_PRIMER,
    MDI_REMOVE_SEQUENCING_PRIMERS,
    MDI_AUTO_ANNOTATE,
    MDI_GRAPH,
    MDI_SPEAK,
    MDI_SIRNA,
    MDI_DOTPLOT,
    MDI_RESTRICTION_IDENTIFIER,

    MT_CLOSE,
    ORF_AS_NEW_AA,
    ORF_AS_NEW_DNA,
    ORF_AS_NEW_ITEM,
    ORF_BLAST_AA,
    ORF_BLAST_DNA,
    ORF_COPY_AA,
    ORF_COPY_DNA,
    PC_BLAST_AA,
    PC_BLAST_DNA,
    PC_COPY_IMAGE,
    PC_DUMMY,
    PC_ITEM_AS_NEW_AA_SEQUENCE,
    PC_ITEM_AS_NEW_SEQUENCE,
    PC_ITEM_BLAST_AA,
    PC_ITEM_BLAST_DNA,
    PC_ITEM_COPY_AA,
    PC_ITEM_COPY_DNA,
    PC_ITEM_DELETE,
    PC_ITEM_EDIT,
    PC_ITEM_HIDE,
    PC_ITEM_MARK,
    PC_ITEM_MARK_SHOW,
    PC_ITEM_SHOW,
    PC_RS_ADD_AND_CUT,
    PC_RS_ADD2COCKTAIL,
    PC_RS_CUT_WITH_COCKTAIL,
    PC_RS_DEL,
    PC_RS_EDIT,
    PC_RS_INFO,
    PC_RS_MARK,
    PC_RS_MARK_SHOW,
    PC_RS_SHOW_HIDE,
    PC_SAVE_IMAGE,
    PC_VECTOR_EDIT,
    PC_WHAT_CUTS,
    PC_ZOOM,
    PD_TROUBLESHOOT,
    PD_CANCEL,
    PD_DEL,
    PD_DIAG_3L,
    PD_DIAG_3R,
    PD_DIAG_5L,
    PD_DIAG_5R,
    PD_DIAG_LC,
    PD_DIAG_MAXDEG,
    PD_DIAG_MAXLEN,
    PD_DIAG_MINDEG,
    PD_DIAG_MINLEN,
    PD_EDIT,
    PD_EXPORT,
    PD_IMPORT,
    PD_LC,
    PD_OK,
    PD_RESET,
    PD_SEARCH,
    PD_SILMUT,
    PD_SILMUT_CANCEL,
    PD_SILMUT_EGR,
    PD_SILMUT_LB,
    PD_SILMUT_MAX_CUT,
    PD_SILMUT_MAX_XHG,
    PD_SILMUT_OK,
    PD_SILMUT_ACR,
    POD_CANCEL,
    POD_OK,
    POPUP_DUMMY,
    PRIMER_BACKWARD,
    PRIMER_BOTH,
    PRIMER_FORWARD,
    PRIMER_MUTATION,
    PROGRAM_OPTIONS,
    RSE_ADD_ALL,
    RSE_ADD2GEL,
    RSE_COC_CB,
    RSE_COC_CT,
    RSE_COC_LL,
    RSE_COC_RM,
    RSE_NB,
    RSE_RES_AC,
    RSE_RES_CC,
    RSE_RES_DD,
    RSE_RES_LL,
    RSE_RES_OK,
    RSE_RES_RB,
    RSE_PARTIAL_DIGESTION,
    SPD_DB,
    SEQ_BOTTOM,
    SEQ_COPY_AS,
    SEQ_COPY_IMAGE,
    SEQ_COPY_RESLUT_AA,
    SEQ_COPY_RESLUT_DNA,
    SEQ_COPY_TEXT,
    SEQ_DELETE_GAP,
    SEQ_DELETE_OTHER_GAPS,
    SEQ_DOWN,
    SEQ_FEAT_HIDE,
    SEQ_FEAT_SHOW,
    SEQ_INSERT_GAP,
    SEQ_INSERT_OTHER_GAPS,
    SEQ_NEW_FROM_RESLUT_AA,
    SEQ_NEW_FROM_RESLUT_DNA,
    SEQ_PRINT,
    SEQ_SAVE_IMAGE,
    SEQ_TOP,
    SEQ_UP,
    SH_CANCEL,
    SH_SEARCH,
    SH_LB,
    SH_TEXT,
    SH_CB_SEQUENCE,
    SH_CB_ITEMS,
    SH_CB_ENZYMES,
    SH_CB_TRANSLATION,
    SPLIT_1,
    SPLIT_2,
    SPLIT_3,
    SPLIT_ALIGNMENT,
    SPLIT_AMINOACIDS,
    TID_CANCEL,
    TID_OK,
    TREE_DUMMY,
    TSD,
    TSD_CANCEL,
    TSD_OK,
    TVE_CANCEL,
    TVE_DEL_FROM_GROUP,
    TVE_EDIT_PROTEASE,
    TVE_EN_ADD_EN,
    TVE_EN_ADD_GR,
    TVE_EN_ADD_TO_GR,
    TVE_EN_ADD_TO_NEW_GR,
    TVE_EN_DEL_EN,
    TVE_EN_DEL_GR,
    TVE_EN_IMPORT_REBASE,
    TVE_DEL_ENZYME,
    TVE_I_ADD,
    TVE_I_CLR,
    TVE_I_COL,
    TVE_I_DEL,
    TVE_ICHOICE,
    TVE_ILIST,
    TVE_IMPORT,
    TVE_LB_CE,
    TVE_LB_GE,
    TVE_LB_GR,
    TVE_NEW_ENZYME,
    TVE_NEW_PROTEASE,
    TVE_OK,
    TVE_PROTEASE_LIST,
    URLTEXT_DUMMY,

    AA_LB,
    AA_IP,
    AA_PROTEOLYSIS,
    SEQ_AA_VIEW,
    SEQ_AA_VIEW_ORIGINAL,
    SEQ_AA_VIEW_REAL,
    SEQ_AA_VIEW_CONDENSED,
    PCR_SPIN,
    MDI_CLOSE,
    MDI_NEXT_WINDOW,
    MDI_PREV_WINDOW,
    MDI_SEL_AS_NEW_PRIMER,
    PC_RS_HIDE_LIMIT,
    MDI_TOGGLE_IDNA,
    SEQ_AA_BACKTRANSLATE,
    SEQ_INSERT_RESTRICTION_SITE_LEFT,
    SEQ_INSERT_RESTRICTION_SITE_RIGHT,
    GES_USE,
    GES_USE_MINCUTOFF,
    GES_USE_MAXCUTOFF,
    GES_COL_1,
    GES_COL_2,
    GES_COL_3,
    GES_USE_COLOR_CODING,
    VG_LABEL,
    VG_PERCENT,

    STRAND_COPY_35,
    STRAND_COPY_53,
    STRAND_COPY_BOTH,
    STRAND_NEW_35,
    STRAND_NEW_53,
    STRAND_NEW_BOTH,

    CGVIEW_RUN_CGVIEWER,
    CGVIEW_CHOOSE_JAR,
    CGVIEW_CHOOSE_BACKGROUND_COLOR,

    GRAPH_SWAP_SIDE,
    GRAPH_ZOOM_X,
    GRAPH_ZOOM_Y,
    GRAPH_ZOOM_LINKED,
    GRAPH_SAVE_AS_IMAGE,
    GRAPH_COPY_AS_IMAGE,
    MDI_AA_IPC,
    TIPC_DIA_LIMIT,

    SPEAK_PLAY,
    SPEAK_STOP,
    SPEAK_PAUSE,

    PHYLIP_CMD_PROTPARS,
    PHYLIP_CMD_DNAPARS,
    PHYLIP_CMD_DNAMOVE,
    PHYLIP_CMD_DNAPENNY,
    PHYLIP_CMD_DNACOMP,
    PHYLIP_CMD_DNAINVAR,
    PHYLIP_CMD_DNAML,
    PHYLIP_CMD_DNAMLK,
    PHYLIP_CMD_PROML,
    PHYLIP_CMD_PROMLK,
    PHYLIP_CMD_DNADIST,
    PHYLIP_CMD_PROTDIST,
    PHYLIP_CMD_RESTDIST,
    PHYLIP_CMD_RESTML,
    PHYLIP_CMD_SEQBOOT,
    PHYLIP_CMD_FITCH,
    PHYLIP_CMD_KITSCH,
    PHYLIP_CMD_NEIGHBOR,
    PHYLIP_CMD_CONTML,
    PHYLIP_CMD_GENDIST,
    PHYLIP_CMD_CONTRAST,
    PHYLIP_CMD_PARS,
    PHYLIP_CMD_MIX,
    PHYLIP_CMD_MOVE,
    PHYLIP_CMD_PENNY,
    PHYLIP_CMD_DOLLOP,
    PHYLIP_CMD_DOLMOVE,
    PHYLIP_CMD_DOLPENNY,
    PHYLIP_CMD_CLIQUE,
    PHYLIP_CMD_FACTOR,
    PHYLIP_CMD_DRAWGRAM,
    PHYLIP_CMD_DRAWTREE,
    PHYLIP_CMD_TREEDIST,
    PHYLIP_CMD_CONSENSE,
    PHYLIP_CMD_RETREE,
    PHYLIP_CMD_SETUP,

    PHYLIP_DIRECT_LINES,
    PHYLIP_TREE_LIST,

    PHYLOBOX,

    TGD_LB_SCALES,
    TGD_LB_DATA,
    TGD_BT_DATA,
    TGD_BT_SCALES,
    TGD_OK,
    PCR_TROUBLESHOOT_LIST,
    PRO_PROTEASES,
    PRO_CUTS,
    PRO_IGNORE,
    PRO_SHOW_UNCUT,
    PRO_PARTIAL,
    PRO_FRAGMENTS_ALL,
    PRO_FRAGMENTS_NONE,
    PRO_SORT_RESULTS,
    PRO_CREATE_REPORT,
    PRO_RESULTS,

    POD_AA_COL,
    CM_OPEN_FEATURE,
    PRO_SEP_FRAGMENTS,
    PRO_SEP_RES,
    PRO_SEP_NUM_PROT,
    PC_TURNING_POINT,
    VG_MARKER,
    MDI_PRINT_RESTRICTION_LIST,
    FD_ADD_HIGHLIGHTS,
    FD_SET_HIGHLIGHT_COLOR,
    FD_RESET_HIGHLIGHTS,

    ALIGN_APPEARANCE_LINE_COLOR_BUTTON1,
    ALIGN_APPEARANCE_LINE_COLOR_BUTTON2,
    ALIGN_APPEARANCE_LINE_COLOR_BUTTON3,
    ALIGN_APPEARANCE_LINE_COLOR_BUTTON4,
    ALIGN_APPEARANCE_LINE_COLOR_BUTTON5,
    ALIGN_APPEARANCE_LINE_COLOR_BUTTON6,
    ALIGN_APPEARANCE_TEXT_FOREGROUND,
    ALIGN_APPEARANCE_TEXT_BACKGROUND,
    ALIGN_APPEARANCE_RESET,

    SAD_SEQUENCE,
    SAD_ABI1,
    SAD_ABI2,

    SAD_LIST,
    SAD_ALL,
    SAD_NONE,
    SAD_INVERT,
    SAD_OPEN_SEQUENCE_1,
    SAD_OPEN_SEQUENCE_2,

    ELL_LB_DATES,
    ELL_LB_USERS,
    ELL_B_EXIT,
    ELL_B_EXCEL,

    AA_FONTSIZE,

    DP_SEQ1,
    DP_SEQ2,
    DP_WINDOW_SIZE,
    DP_MISMATCH_LIMIT,
    DP_OPEN_SEQ1,

    RI_MIN_BP,
    RI_MIN_PERCENT,
    RI_GROUP,
    RI_DNA_LIST,
    RI_ENZYMES_LIST,

    Y___,
    Z___,

    ONLINE_TOOLS_BEGIN,
    ONLINE_TOOLS_PHOBIUS,
    ONLINE_TOOLS_POLY_PHOBIUS,
    ONLINE_TOOLS_MOTIF_SCAN,
    ONLINE_TOOLS_PVAL_FPSCAN,
    ONLINE_TOOLS_ELM,
    ONLINE_TOOLS_JPRED,
    ONLINE_TOOLS_CALCPI,
    ONLINE_TOOLS_GOR,
    ONLINE_TOOLS_HNN,
    ONLINE_TOOLS_NEB_CUTTER,
    ONLINE_TOOLS_NOMAD,
    ONLINE_TOOLS_MITOPROT,
    ONLINE_TOOLS_MYRISTOYLATOR,
    ONLINE_TOOLS_SULFINATOR,
    ONLINE_TOOLS_SUMOPLOT,
    ONLINE_TOOLS_TARGETP,
    ONLINE_TOOLS_2ZIP,
    ONLINE_TOOLS_SAPS,
    ONLINE_TOOLS_MULTALIN,
    ONLINE_TOOLS_WEBLOGO,
    ONLINE_TOOLS_TRANSLATE,
    ONLINE_TOOLS_PREPS,
    ONLINE_TOOLS_DGPI,
    ONLINE_TOOLS_PLASMAPPER,
    ONLINE_TOOLS_END
    } ;

#endif
