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
    TUReadSeq ( wxString _filename = "" ) ; ///< Constructor
    virtual void getFormat () ; ///< Try to determine format
    virtual int getSequenceNames () ; ///< Get the names of the sequences
    virtual void getSequences () ; ///< Get the sequences
    virtual void convert ( TGenBank &gb ) ; ///< Convert the sequences to GenBank format

    static int getSeqType ( wxString t ) ; ///< Return the sequence type
    
    private :
    wxString filename ; ///< Name of source file
    short error , format , numseqs ;
    long skiplines ;
    wxArrayString seq_names , seqs ;
    } ;


#endif

