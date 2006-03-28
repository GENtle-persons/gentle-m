#include "CloneManager.h"

TCloneManager::TCloneManager ()
{
	_success = false ;
}

TCloneManager::~TCloneManager ()
{
}

void TCloneManager::load ( wxString file )
{
	wxFile f ( file , wxFile::read ) ;
	long l = f.Length() ;
	unsigned char *t = new unsigned char [l+5] ;
	f.Read ( t , l ) ;
	f.Close() ;
	
	if ( t[0] != 26 || t[1] != 'S' || t[2] != 'E' || t[3] != 'S' )
	{
		delete t ;
		return ;
	}
	
	int a , b ;
	wxString name , seq ;
	bool found = false ;
	for ( a = 0 ; a + 44 < l ; a++ )
	{
		if ( t[a] == 255 && t[a+1] == 255 && t[a+44] == 255 && t[a+45] == 255 )
		{
			found = true ;
			break ;
		}
	}
	
	if ( !found )
	{
		delete t ;
		return ;
	}

	a += 44 ;
	name = file.AfterLast('/').AfterLast('\\').BeforeLast('.') + _T(" / ") ;
	for ( b = a + 20 ; t[b] >= 13 ; b++ )
		name += t[b] ;
	for ( b = a + 36 ; t[b] >= 65 ; b++ )
		seq += t[b] ;
	
	TVector *v = new TVector ;
	v->setName ( name ) ;
	v->setSequence ( seq ) ;
	
	_v.push_back ( v ) ;
	_success = true ;
	delete t ;
}

int TCloneManager::countVectors ()
{
	return _v.size() ;
}

TVector *TCloneManager::getVector ( int a )
{
	return _v[a] ;
}

