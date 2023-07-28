#include "CloneManager.h"

TCloneManager::TCloneManager ()
{
	_success = false ;
}

TCloneManager::~TCloneManager ()
{
}

int TCloneManager::scan_item ( unsigned char *t , int a , TVector *v )
{
	int b ;
	wxString name , desc ;
	a += 4 ;
	unsigned int strange1 = *((unsigned int*)(t+a)) ;
	a += 4 ;
	if ( t[a] == 8 )
	{
		while ( t[a] != 255 || t[a+1] != 255 ) a += 4 ;
		return a ;
	}
	unsigned int strange2 = *((unsigned int*)(t+a)) ;
	a += 4 ;
	unsigned int from = *((unsigned int*)(t+a)) ;
	a += 4 ;
	unsigned int to = *((unsigned int*)(t+a)) ;
	a += 4 ;
	for ( b = a ; t[b] >= 32 && b < a+8 ; b++ ) name += t[b] ;
	a += 8 ;
	for ( b = a ; t[b] ; b++ ) desc += t[b] ;
	a = b ;
	while ( a % 4 > 0 ) a++ ;
	while ( t[a] != 255 && ( t[a] < 'A' || t[a] > 'Z' ) ) a += 4 ; // Ugly hack!
	
	int dir = 1 ;
	if ( to < from )
	{
		dir = -1 ;
		int x = from ;
		from = to ;
		to = x ;
	}
	int type = VIT_MISC ;
	if ( to - from > 80 ) type = VIT_GENE ;
	else if ( name.Left(3).Lower() == _T("ori") ) type = VIT_REP_ORI ;
	TVectorItem vi ( name , desc , from , to , type ) ;
	vi.setDirection ( dir ) ;
	v->items.push_back ( vi ) ;
	
	return a ;
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
		delete [] t ;
		return ;
	}
	
	int a , b ;
	TVector *v = new TVector ;
	wxString name = file.AfterLast('/').AfterLast('\\').BeforeLast('.') ;
	wxString seq , desc ;
	
	// Items
	a = 12 ;
	while ( t[a+2] == 0 && t[2+3] == 0 )
	{
		unsigned char a1 = t[a] ;
		unsigned char a2 = t[a+1] ;
		if ( a1 == 255 && a2 == 255 ) a = scan_item ( t , a , v ) ;
		else
		{
			a += 8 ;
		}
	}
	
	// Sequence
	while ( t[a] >= 65 ) seq += t[a++] ;
	
	// Rest
	for ( a = l - 1 ; t[a-1] || t[a-2] ; a-- ) ;
	for ( ; a < l ; a++ )
	{
		if ( t[a] == 10 ) continue ;
		if ( t[a] == 13 || t[a] == 0 ) desc += '\n' ;
		else desc += t[a] ;
	}

	// Set vector
	v->setName ( name ) ;
	v->setSequence ( seq ) ;
	v->setDescription ( desc ) ;
	if ( seq.length() > 3000 ) v->setCircular () ; // Guessing
	
	_v.push_back ( v ) ;
	_success = true ;
	delete [] t ;
}

int TCloneManager::countVectors ()
{
	return _v.size() ;
}

TVector *TCloneManager::getVector ( int a )
{
	return _v[a] ;
}

