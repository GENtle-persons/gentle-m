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


class TLinear
{
public :
	TLinear () {} ;
	// Variables
	string e1 , e2 , s1 , s2 ;
} ;


class TEnzyme
{
public :
	TEnzyme () { selected = false ; } ;
	// Operators
	virtual bool operator < ( const TEnzyme &e ) { return position < e.position ; }
	virtual bool operator == ( const TEnzyme &e ) { return position == e.position ; }
	// Variables
	string name ;
	int position ;
	bool selected ;
} ;

class TGene
{
public :
	TGene () { selected = false ; } ;
	TGene ( const TGene &g ) ;
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
    virtual void loadEnzymeList ( TStorage *st , string filename ) ;
    virtual void remap ( TVector *v ) ;
	virtual void setGeneSequence ( int i , string s );
	virtual string getGeneSequence ( int i );
	TClone () ;
	virtual ~TClone();
	virtual void load ( string s ) ;
	virtual void save ( string s = "" ) ;
	virtual void cleanup () ;
	virtual void parseLines ( vector <string> &v , char *t , long l ) ;
	virtual void separateNames ( string &s1 , string &s2 ) ;
	virtual int cmp ( const string &s1 , const string &s2 ) ;
	virtual int strcmpi ( const string &s1 , const string &s2 ) { return cmp ( s1 , s2 ) ; }
	virtual int a2i ( string &s ) { return atoi ( (char*) s.c_str() ) ; }
    TStorage *LS ;
	string filename , name , sequence , description ;
	int size ;
	bool isLinear , changed ;
	vector <TEnzyme> enzymes ;
	vector <TGene> genes ;
	bool success ;
	TLinear linear ;
} ;


#endif

