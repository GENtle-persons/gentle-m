
#ifndef MAIN_DEFINITIONS_H
#define MAIN_DEFINITIONS_H

#define TYPE_VECTOR 0 /**< \brief Vector type */
#define TYPE_FRAGMENT 1 /**< \brief Fragment type */
#define TYPE_SEQUENCE 2 /**< \brief Sequence type */
#define TYPE_PRIMER 3 /**< \brief Primer type */
#define TYPE_ALIGNMENT 4 /**< \brief Alignment type */
#define TYPE_AMINO_ACIDS 5 /**< \brief Amino acid type */
#define TYPE_MISC 6 /**< \brief Misc type */

#define FEAT_NONE 0  /**< \brief Do not draw feature  */
#define FEAT_ALPHA 1  /**< \brief Draw feature as alpha helix */
#define FEAT_BETA 2  /**< \brief Draw feature as beta sheet */
#define FEAT_MUT 3  /**< \brief Draw feature as mutation */
#define FEAT_NOLINE 4  /**< \brief Draw feature without line  */
#define FEAT_VLINE 5  /**< \brief Draw feature as vertical line */

#define DAM_METHYLATION 1 /**< \brief Show DAM methylation */
#define DCM_METHYLATION 2 /**< \brief Show DCM methylation */
#define ALL_METHYLATION_ENZYMES ( DAM_METHYLATION + DCM_METHYLATION ) /**< \brief Show all methylations */

#endif