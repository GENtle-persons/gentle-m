#ifndef _EXTERNAL_INTERFACE_H_
#define _EXTERNAL_INTERFACE_H_

#include "main.h"
#include "ChildBase.h"
#include <wx/splitter.h>

class ExternalInterface : public ChildBase
    {
    public :
    ExternalInterface(wxWindow *parent, const wxString& title) ;
    ~ExternalInterface () ;
    
    void initme () ;
    virtual wxString getName () ;

    DECLARE_EVENT_TABLE()
    } ;

#endif
