// TClone.h: interface for the TClone class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TCLONE_H_
#define _TCLONE_H_

#include "main.h"

#include <math.h>
#include <fcntl.h>
//#include <string>
//#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

class TStorage ;

class TClone_Enzyme
    {
    public :
	TClone_Enzyme () { selected = false ; } ;
	// Operators
	virtual bool operator < ( const TClone_Enzyme &e ) { return position < e.position ; }
	virtual bool operator == ( const TClone_Enzyme &e ) { return position == e.position ; }
	// Variables
	wxString name ;
	int position ;
	bool selected ;
	} ;

class TClone_Gene
    {
    public :
	TClone_Gene () { selected = false ; } ;
	TClone_Gene ( const TClone_Gene &g ) ;
	virtual int getRealEnd();
	virtual int getRealBegin();
	virtual void setCCW ( bool x );
	virtual bool getCCW();
	virtual int cmp ( const wxString &s1 , const wxString &s2 ) ;
	// Variables
	wxString fullname , shortname , direction , five , type ;
	int begin , end ;
	bool selected;
	} ;

class TClone
    {
    public :
	TClone () ;
	virtual ~TClone();
    virtual void remap ( TVector *v ) ;
	virtual void load ( wxString s ) ;
	virtual void save ( wxString s = "" ) ;
    virtual void loadEnzymeList ( TStorage *st , wxString filename ) ;
    TStorage *LS ;
	bool success ;
	
	private :
	virtual void setGeneSequence ( int i , wxString s );
	virtual wxString getGeneSequence ( int i );
	virtual void cleanup () ;
	virtual void parseLines ( wxArrayString &v , char *t , long l ) ;
	virtual void separateNames ( wxString &s1 , wxString &s2 ) ;
	virtual int cmp ( const wxString &s1 , const wxString &s2 ) ;
	virtual int a2i ( wxString &s ) { return atoi ( (char*) s.c_str() ) ; }
	wxString filename , name , sequence , description ;
	int size ;
	bool isLinear , changed ;
	vector <TClone_Enzyme> enzymes ;
	vector <TClone_Gene> genes ;
	wxString linear_e1 , linear_e2 , linear_s1 , linear_s2 ;
	} ;
	
#endif

