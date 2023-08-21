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
	~TClone_Enzyme () {} ; ///< Dummy destructor
	
	// Operators
	bool operator < ( const TClone_Enzyme &e ) { return position < e.position ; }
	bool operator == ( const TClone_Enzyme &e ) { return position == e.position ; }
	
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
	~TClone_Gene () {} ; ///< Dummy destructor
	
	int getRealEnd(); ///< Returns converted item end
	int getRealBegin(); ///< Returns converted item start
	void setCCW ( bool x ); ///< Sets orientation (Clockwise or Counter-Clockwise)
	bool getCCW(); ///< Returns orientation
	int cmp ( const wxString &s1 , const wxString &s2 ) ; ///< String comparison
	
	// Variables
	wxString fullname , shortname , direction , five , type ;
	int begin , end ;
	} ;

/// The CLONE format import/export class
class TClone
    {
    public :
	TClone () ; ///< Constructor
	~TClone(); ///< Destructor
    void remap ( TVector *v ) ; ///< Convert parsed information to TVector
	void load ( wxString s ) ; ///< Load CLONE format file
	void save ( wxString s = _T("") ) ; ///< Save CLONE format file (currently deactivated)
    void loadEnzymeList ( TStorage *st , wxString filename ) ; ///< Load the CLONE format enzyme list
    TStorage *LS ; ///< Pointer to the local database
	bool success ; ///< Errors during parsing?
	
	private :
//	void setGeneSequence ( int i , wxString s );
//	wxString getGeneSequence ( int i );
	void cleanup () ; ///< Reset internal state
	void parseLines ( wxArrayString &v , char *t , long l ) ; ///< Breaks text into lines
	void separateNames ( wxString &s1 , wxString &s2 ) ; ///< ???
	int cmp ( const wxString &s1 , const wxString &s2 ) ; ///< String comparison
	int a2i ( wxString &s ) { return atoi ( (const char*) s.c_str() ) ; }  ///< Converts string to integer
	
	wxString filename , name , sequence , description ;
	int size ; ///< Sequence length
	bool isLinear ; ///< Linear or circular
	vector <TClone_Enzyme> enzymes ; ///< Temporary list of enzymes
	vector <TClone_Gene> genes ; ///< Temporary list of items
	wxString linear_e1 , linear_e2 , linear_s1 , linear_s2 ;
	} ;
	
#endif

