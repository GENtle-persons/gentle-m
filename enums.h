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
    IED_OK,
    IED_USEOFFSET,
    IV_BUTTON,
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
    MDI_REDO,
    MDI_RESTRICTION,
    MDI_RUN_PCR,
    MDI_TOGGLE_FEATURES,
    MDI_TOGGLE_RESTRICTION,
    MDI_TRANSFORM_SEQUENCE,
    MDI_UNDO,
    MDI_VIEW_MODE,
    MDI_DUMMY_END,

    
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
    MDI_LIGATION,
    MDI_MANAGE_DATABASE,
    MDI_PROJECT_CLOSE,
    MDI_PROJECT_LOAD,
    MDI_PROJECT_SAVE,
    MDI_QUIT ,
    MDI_REFRESH,
    MDI_TEXT_IMPORT,
    
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
    PC_VECTOR_EDIT,
    PC_WHAT_CUTS,
    PC_ZOOM,
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
    POD_CANCEL,
    POD_OK,
    POPUP_DUMMY,
    PRIMER_BACKWARD,
    PRIMER_BOTH,
    PRIMER_FORWARD,
    PRIMER_MUTATION,
    PROGRAM_OPTIONS ,
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
    SPLIT_1,
    SPLIT_2,
    SPLIT_3,
    SPLIT_ALIGNMENT,
    SPLIT_AMINOACIDS,
    TID_CANCEL,
    TID_OK,
    TREE_DUMMY ,
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
    
    Z___
    } ;

#endif

