/** \file
	\brief Contains the TClone CLONE import/export class, and helper classes
*/
#ifndef _TCLONE_H_
#define _TCLONE_H_

#include "main.h"

#include <math.h>
#include <fcntl.h>
#include <iostream>
#include <algorithm>

class TStorage ;

/// Temporarily stores an enzyme
class TClone_Enzyme
    {
    public :
	TClone_Enzyme () { /*selected = false ;*/ } ; ///< Constructor
	virtual ~TClone_Enzyme () {} ; ///< Dummy destructor
	
	// Operators
	virtual bool operator < ( const TClone_Enzyme &e ) { return position < e.position ; }
	virtual bool operator == ( const TClone_Enzyme &e ) { return position == e.position ; }
	
	// Variables
	wxString name ; ///< Name of the enzyme
	int position ; ///< Position of the cut
	} ;

/// Temporarily stores an item
class TClone_Gene
    {
    public :
	TClone_Gene () { /*selected = false ;*/ } ; ///< Default constructor
	TClone_Gene ( const TClone_Gene &g ) ; ///< Constructor
	virtual ~TClone_Gene () {} ; ///< Dummy destructor
	
	virtual int getRealEnd(); ///< Returns converted item end
	virtual int getRealBegin(); ///< Returns converted item start
	virtual void setCCW ( bool x ); ///< Sets orientation (Clockwise or Counter-Clockwise)
	virtual bool getCCW(); ///< Returns orientation
	virtual int cmp ( const wxString &s1 , const wxString &s2 ) ; ///< String comparison
	
	// Variables
	wxString fullname , shortname , direction , five , type ;
	int begin , end ;
	} ;

/// The CLONE format import/export class
class TClone
    {
    public :
	TClone () ; ///< Constructor
	virtual ~TClone(); ///< Destructor
    virtual void remap ( TVector *v ) ; ///< Convert parsed information to TVector
	virtual void load ( wxString s ) ; ///< Load CLONE format file
	virtual void save ( wxString s = _T("") ) ; ///< Save CLONE format file (currently deactivated)
    virtual void loadEnzymeList ( TStorage *st , wxString filename ) ; ///< Load the CLONE format enzyme list
    TStorage *LS ; ///< Pointer to the local database
	bool success ; ///< Errors during parsing?
	
	private :
//	virtual void setGeneSequence ( int i , wxString s );
//	virtual wxString getGeneSequence ( int i );
	virtual void cleanup () ; ///< Reset internal state
	virtual void parseLines ( wxArrayString &v , char *t , long l ) ; ///< Breaks text into lines
	virtual void separateNames ( wxString &s1 , wxString &s2 ) ; ///< ???
	virtual int cmp ( const wxString &s1 , const wxString &s2 ) ; ///< String comparison
	virtual int a2i ( wxString &s ) { return atoi ( (char*) s.c_str() ) ; }  ///< Converts string to integer
	
	wxString filename , name , sequence , description ;
	int size ; ///< Sequence length
	bool isLinear ; ///< Linear or circular
	vector <TClone_Enzyme> enzymes ; ///< Temporary list of enzymes
	vector <TClone_Gene> genes ; ///< Temporary list of items
	wxString linear_e1 , linear_e2 , linear_s1 , linear_s2 ;
	} ;
	
#endif

