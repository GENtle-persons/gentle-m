// TClone.h: interface for the TClone class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _TCLONE_H_
#define _TCLONE_H_

#include "main.h"

#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

class TStorage ;
/*
class TLinear
    {
    public :
	TLinear () {} ;
	// Variables
	} ;
*/

class TClone_Enzyme
    {
    public :
	TClone_Enzyme () { selected = false ; } ;
	// Operators
	virtual bool operator < ( const TClone_Enzyme &e ) { return position < e.position ; }
	virtual bool operator == ( const TClone_Enzyme &e ) { return position == e.position ; }
	// Variables
	string name ;
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
	virtual int strcmpi ( const string &s1 , const string &s2 ) ;
	// Variables
	string fullname , shortname , direction , five , type ;
	int begin , end ;
	bool selected;
	} ;

class TClone
    {
    public :
	TClone () ;
	virtual ~TClone();
    virtual void remap ( TVector *v ) ;
	virtual void load ( string s ) ;
	virtual void save ( string s = "" ) ;
    virtual void loadEnzymeList ( TStorage *st , string filename ) ;
    TStorage *LS ;
	bool success ;
	
	private :
	virtual void setGeneSequence ( int i , string s );
	virtual string getGeneSequence ( int i );
	virtual void cleanup () ;
	virtual void parseLines ( vector <string> &v , char *t , long l ) ;
	virtual void separateNames ( string &s1 , string &s2 ) ;
	virtual int cmp ( const string &s1 , const string &s2 ) ;
	virtual int strcmpi ( const string &s1 , const string &s2 ) { return cmp ( s1 , s2 ) ; }
	virtual int a2i ( string &s ) { return atoi ( (char*) s.c_str() ) ; }
	string filename , name , sequence , description ;
	int size ;
	bool isLinear , changed ;
	vector <TClone_Enzyme> enzymes ;
	vector <TClone_Gene> genes ;
	string linear_e1 , linear_e2 , linear_s1 , linear_s2 ;
	} ;
	
#endif

