#ifndef _TUREADSEQ_H_
#define _TUREADSEQ_H_

#include "main.h"

class TGenBank ;

// Wrapper class for ureadseq
class TUReadSeq
    {
    public :
    TUReadSeq ( wxString _filename = "" ) ;
    virtual void getFormat () ;
    virtual int getSequenceNames () ;
    virtual void getSequences () ;
    virtual void convert ( TGenBank &gb ) ;

    static int getSeqType ( wxString t ) ;
    
//    private :
    wxString filename ;
    short error , format , numseqs ;
    long skiplines ;
    wxArrayString seq_names , seqs ;
    } ;


#endif

