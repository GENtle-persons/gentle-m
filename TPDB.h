/** \file
	\brief TPDB class, and its tseqres helper class
*/
#ifndef _PDB_H_
#define _PDB_H_

#include "main.h"

class TVector ;

/// Helper class for TPDB, holds the TVector objects for each parsed PDB chain
class tseqres
    {
    public :
    char chain ; ///< Chain number
    wxString sequence ; ///< Sequence
    TVector *v ; ///< Pointer to the TVector
    } ;    

/// The TPDB class can parse PDB files and generated annotated TVector sequences
class TPDB
    {
    public :
    TPDB () ; ///< Constructor
	virtual ~TPDB () {} ; ///< Dummy destructor
	
    virtual void load ( wxString s ) ; ///< Load file
    virtual void paste ( wxString s ) ; ///< Take pasted text
    virtual void remap () ; ///< Parse and generate sequences

    bool success ; ///< Errors during parsing?
    vector <tseqres> seqres ; ///< The resulting sequences
    
    private :
    virtual void check4success () ; ///< Valid PDB?
    virtual int seq ( char c ) ; ///< Access (or create) a certain chain
    wxArrayString vs ; ///< The text lines
    
    /// The PDB name
    wxString _name , _desc ; ///< The PDB description
    } ;
    
#endif    
