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

//using namespace std ;

//#include "tsilmut.h"

#define CLEAR_BACKGROUND 1
#define PRINTING 2


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

class TLinear
{
public :
	TLinear () {} ;
	// Variables
	string e1 , e2 , s1 , s2 ;
} ;



class TClone
{
public :
    virtual void loadEnzymeList ( TStorage *st , string filename ) ;
    virtual void remap ( TVector *v ) ;
	virtual void setGeneSequence ( int i , string s );
	virtual string getGeneSequence ( int i );
//	virtual void getRestrictionSites(int sel,vector <int> &cuts);
//	virtual void updateCurrentEnzymes();
//	virtual void update();
//	virtual void ligate ( TClone &c );
	TClone () ;
	virtual ~TClone();
	virtual void load ( string s ) ;
	virtual void save ( string s = "" ) ;
	virtual void cleanup () ;
//	virtual void visualize () ;
//	virtual void showPlasmid ( CDC &dc , int opt = 0 ) ;
	virtual void parseLines ( vector <string> &v , char *t , long l ) ;
	virtual void separateNames ( string &s1 , string &s2 ) ;
	virtual int cmp ( const string &s1 , const string &s2 ) ;
	virtual int strcmpi ( const string &s1 , const string &s2 ) { return cmp ( s1 , s2 ) ; }
	virtual int a2i ( string &s ) { return atoi ( (char*) s.c_str() ) ; }
//	virtual cX ( float w , float r ) ;
//	virtual cY ( float w , float r ) ;
	// Variables
//	CSize cs ;
    TStorage *LS ;
	string filename , name , sequence , description ;
	int size ;
	bool isLinear , changed ;
	TLinear linear ;
	vector <TEnzyme> enzymes ;
	vector <TGene> genes ;
	bool success ;
//	HWND hwin ;
//	CWnd *thisDlg ;
//	TSilmut *silmut ;
} ;


#endif

