#ifndef __SCF_H__
#define __SCF_H__

#include "main.h"

/** \class TSCFtype
	\brief Reads files in SCF format
*/
class SCFtype
	{
	public :
	SCFtype () ;
	bool parse ( const wxString& filename ) ;
	
	TSequencerData sd ;
	
	private :
	void read_data_block ( void *v , unsigned int mode ) ;
	void read_tracer_block ( void *v , unsigned int mode , unsigned int sample_size ) ;
	void fix_diff ( const int mode ) ;
	} ;

#endif
