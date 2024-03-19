/** \file
    \brief The TUReadSeq class
*/
#ifndef _TUREADSEQ_H_
#define _TUREADSEQ_H_

#include "main.h"

class TGenBank ;

/// This is a wrapper class for ureadseq, which can read many sequence formats
class TUReadSeq
    {
    public :
    TUReadSeq ( const wxString& filename = _T("") ) ; ///< Constructor
    ~TUReadSeq () {} ; ///< Destructor (empty)

    void getFormat () ; ///< Try to determine format
    int getSequenceNames () ; ///< Get the names of the sequences
    void getSequences () ; ///< Get the sequences
    void convert ( TGenBank &gb ) const ; ///< Convert the sequences to GenBank format

    static int getSeqType ( const wxString& t ) ; ///< Return the sequence type

    short error ; // Parsing error

    private :
    wxString filename ; ///< Name of source file
    short format ;
    size_t numseqs ;
    size_t skiplines ;
    wxArrayString seq_names ;
    wxArrayString seqs ;
    } ;


#endif

