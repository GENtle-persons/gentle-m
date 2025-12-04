#ifndef _UTIL_H_
#define _UTIL_H_

#include <wx/dynarray.h>

WX_DECLARE_OBJARRAY(float, wxArrayFloat);

class TRestrictionEnzyme;

/** \brief Compares the values of two int pointers */
int cmpint(/*const*/ int * const first, /*const*/ int * const second) ; // no const to match SCMPFUNC

/** \brief Compares the values of two TRestrictionEnzyme pointers */
int cmpre(const TRestrictionEnzyme * const first, const TRestrictionEnzyme * const second) ;

#endif

