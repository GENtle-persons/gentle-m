#ifndef _ENUMS_H_
#define _ENUMS_H_

enum {
    EE_START = 6000 ,
    EE_LB_CE,
    EE_LB_GE,
    EE_LB_GR,
    EE_BT_ICE,

    ED_OK,
    ED_CANCEL,
    ED,
    
    TSD,
    TSD_OK,
    TSD_CANCEL,
    
    SEQ_COPY_AS,
    SEQ_COPY_TEXT,
    SEQ_COPY_IMAGE,
    SEQ_PRINT,
    SEQ_COPY_RESLUT_DNA,
    SEQ_COPY_RESLUT_AA,
    SEQ_NEW_FROM_RESLUT_DNA,
    SEQ_NEW_FROM_RESLUT_AA,

    RSE_NB,
    RSE_RES_LL,
    RSE_RES_DD,
    RSE_RES_RB,
    RSE_RES_AC,
    RSE_RES_OK,
    RSE_RES_CC,
    RSE_COC_LL,
    RSE_COC_CT,
    RSE_COC_RM,
    RSE_COC_CB,

    PROGRAM_OPTIONS ,
    POD_OK,
    POD_CANCEL,

    // menu items ids
    TREE_DUMMY ,
    MAIN_TREE_DUMMY,

    // From MiscDialogs
    MDI_QUIT ,
    MDI_ENZYME_EDITOR,
    MDI_FILE_OPEN,
    MDI_FILE_SAVE,
    MDI_CUT,
    MDI_COPY,
    MDI_PASTE,
    MDI_COPY_TO_NEW,
    MDI_REFRESH,
    MDI_CHANGE_TITLE,
    MDI_CHANGE_POSITION,
    MDI_CHANGE_SIZE,
    MDI_CHILD_QUIT,
    MDI_CIRCULAR_LINEAR,
    MDI_FILE_IMPORT,
    MDI_TEXT_IMPORT,
    MDI_LIGATION,
    MDI_ALIGNMENT,
    MDI_EXPORT,
    MDI_AS_NEW_FEATURE,
    MDI_MANAGE_DATABASE,
    MDI_VIEW_MODE,
    MDI_EDIT_MODE,
    MDI_PRINT_IMAGE,
    MDI_RESTRICTION,
    MDI_PROJECT_SAVE,
    MDI_PROJECT_LOAD,
    MDI_PROJECT_CLOSE,
    MDI_TRANSFORM_SEQUENCE,
    MDI_MARK_ALL,
    MDI_EXTRACT_AA,
    MDI_ABOUT,
    MDI_HELP,
    MDI_FIND,
    MDI_ORFS,
    MDI_EDIT_ORFS,
    MDI_PRINT_REPORT,
    MDI_FILL_KLENOW,
    MDI_RUN_PCR,
    SPLIT_1,
    SPLIT_2,
    SPLIT_3,

    // From ManageDatabase
    MD_PD_DBLIST,
    MD_PD_ADD,
    MD_PD_DEL,
    MD_PD_DEFAULT,
    MD_PM_LEFT,
    MD_PM_RIGHT,
    MD_PM_DD_L,
    MD_PM_DD_R,
    MD_PM_EDIT,
    MD_PM_SAVE,
    MD_PM_SAVE_DB,
    MD_PM_DEL,
    MD_PM_REN,
    MD_PM_NEW,
    MD_PM_FILTER,
    
    LD_SOURCES,
    LD_TARGETS,
    LD_OK,
    LD_CANCEL,
    
    TID_OK,
    TID_CANCEL,
    
    MCD_OK,
    
    
    TVE_OK,
    TVE_CANCEL,
    TVE_ILIST,
    TVE_LB_CE,
    TVE_LB_GE,
    TVE_LB_GR,
    TVE_EN_ADD_GR,
    TVE_EN_ADD_TO_GR,
    TVE_EN_ADD_TO_NEW_GR,
    TVE_EN_ADD_EN,
    TVE_EN_DEL_EN,
    TVE_I_ADD,
    TVE_I_DEL,
    TVE_I_CLR,
    TVE_I_COL,
    TVE_IMPORT,
    
    // Amino acid display menu
    AA_MAIN,
    AA_NONE,
    AA_ALL,
    AA_KNOWN,
    AA_THREE_1,
    AA_THREE_2,
    AA_THREE_3,
    AA_THREE_M1,
    AA_THREE_M2,
    AA_THREE_M3,
    AA_THREE,
    AA_ONE,
    
    // Alignment
    SPLIT_ALIGNMENT,
    ALIGNMENT_SETTINGS,
    AL_OK,
    AL_CANCEL,
    AL_ALL,
    AL_CUR,
    AL_UP,
    AL_DOWN,
    AL_ADD,
    AL_DEL,
    AL_ALG,
    AL_MATCH,
    AL_MISMATCH,
    AL_PENALTY,
    
    SH_SEARCH,
    SH_CANCEL,
    
    PC_DUMMY,
    PC_ZOOM,
    PC_ITEM_MARK,
    PC_ITEM_MARK_SHOW,
    PC_ITEM_EDIT,
    PC_ITEM_AS_NEW_SEQUENCE,
    PC_ITEM_DELETE,
    PC_ITEM_HIDE,
    PC_ITEM_SHOW,
    PC_RS_EDIT,
    PC_RS_SHOW_HIDE,
    PC_RS_DEL,
    PC_RS_MARK,
    PC_RS_MARK_SHOW,
    PC_RS_ADD2COCKTAIL,
    PC_RS_ADD_AND_CUT,
    PC_RS_CUT_WITH_COCKTAIL,
    PC_RS_INFO,
    PC_VECTOR_EDIT,
    PC_BLAST_DNA,
    PC_BLAST_AA,
    PC_COPY_IMAGE,
    PC_ITEM_BLAST_DNA,
    PC_ITEM_BLAST_AA,
    PC_ITEM_COPY_DNA,
    PC_ITEM_COPY_AA,
    PC_ITEM_AS_NEW_AA_SEQUENCE,
    
    PRIMER_BOTH,
    PRIMER_FORWARD,
    PRIMER_BACKWARD,
    PRIMER_MUTATION,
    
    SPLIT_AMINOACIDS,
    AMINOACIDS_SETTINGS,
    AMINOACIDS_EDIT_NAME,
    
    PD_LC,
    PD_OK,
    PD_CANCEL,
    PD_SEARCH,
    PD_RESET,
    PD_DIAG_LC,
    PD_DIAG_3L,
    PD_DIAG_5L,
    PD_DIAG_3R,
    PD_DIAG_5R,
    PD_DIAG_MINLEN,
    PD_DIAG_MAXLEN,
    PD_DIAG_MINDEG,
    PD_DIAG_MAXDEG,
    PD_IMPORT,
    PD_EXPORT,
    PD_EDIT,
    PD_DEL,
    
    ORF_COPY_DNA,
    ORF_COPY_AA,
    ORF_AS_NEW_ITEM,
    ORF_AS_NEW_DNA,
    ORF_AS_NEW_AA,
    ORF_BLAST_DNA,
    ORF_BLAST_AA,
    
    MT_CLOSE,
    
    ABI_HELPLINES,
    ABI_WIDTH,
    ABI_HEIGHT,
    ABI_SLIDER,
    
    POPUP_DUMMY,
    MDI_TOGGLE_FEATURES,
    MDI_TOGGLE_RESTRICTION,
    MD_PM_OPEN,
    MD_PM_FILTER_DNA,
    MD_PM_FILTER_PROTEIN,
    MD_PM_FILTER_PRIMER,
    
    IED_COLOR,
    IED_OK,
    IED_CANCEL,
    
    MDI_IMAGE_VIEWER,
    
    IV_BUTTON,
    IV_IMAGE,
    IV_LIST,
    IV_MENU_SAVE_AS_BITMAP,
    IV_MENU_COPY,
    IV_MENU_PRINT,
    
    MDI_CALCULATOR,
    TVE_NEW_ENZYME,
    TVE_NEW_PROTEASE,
    TVE_EDIT_PROTEASE,
    TVE_PROTEASE_LIST,
    IED_USEOFFSET,
    AMINOACIDS_BLAST_AA,
    AMINOACIDS_DESC,
    URLTEXT_DUMMY,
    PD_SILMUT,
    PD_SILMUT_MAX_XHG,
    PD_SILMUT_MAX_CUT,
    PD_SILMUT_EGR,
    PD_SILMUT_OK,
    PD_SILMUT_CANCEL,
    PD_SILMUT_LB,
    
    MDI_UNDO,
    MDI_REDO,
    
    AL_MATRIX,
    TVE_ICHOICE,
    
    FRAME_SASH_TOP,
    FRAME_SASH_1,
    FRAME_SASH_2,
    FRAME_SASH_BOTTOM,
    MD_PM_TWOPANES,
    SEQ_SAVE_IMAGE,
    MD_PM_FILTER_DESC,
    MD_PM_FILTER_SEQ,
    ABI_INV_COMP,
    PC_WHAT_CUTS,
    
    TVE_EN_DEL_GR,
    TVE_DEL_FROM_GROUP,
    
    ALIGN_BOLD,
    ALIGN_MONO,
    ALIGN_NORM,
    ALIGN_INVS,
    ALIGN_SOA,
    ALIGN_SOAI,
    ALIGN_SIML,
    ALIGN_SEQ,
    ALIGN_FEAT,
    ALIGN_RNA,
    ALIGN_CONS,
    ALIGN_IDENT,
    ALIGN_HORIZ,
    
    SEQ_UP,
    SEQ_DOWN,
    SEQ_TOP,
    SEQ_BOTTOM,
    
    } ;

#endif

