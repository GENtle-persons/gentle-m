#ifndef __T_SEQUENCER_DATA__
#define __T_SEQUENCER_DATA__

#include "main.h"

class TSequencerDataTracerItem
    {
    public :
    signed long data[4] ; //a , c , g , t ;
    } ;

class TSequencerDataSequenceItem
    {
    public :
    unsigned long peak_index = 0L ;
    unsigned char prob_a = 0 , prob_c = 0 , prob_t = 0 , prob_g = 0 ;
    char base = 0 ;
    } ;

class TSequencerData
    {
    public :
    wxString comment , seq ;
    vector <TSequencerDataTracerItem> tracer ;
    vector <TSequencerDataSequenceItem> sequence ;
    vector <unsigned long> tracer2[4] ; // A,C,G,T
    } ;

#define TRACER_ID_A 0
#define TRACER_ID_C 1
#define TRACER_ID_G 2
#define TRACER_ID_T 3

#endif
