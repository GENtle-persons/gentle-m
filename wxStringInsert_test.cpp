#include <wx/string.h>
#include <wx/wxcrtvararg.h>


void wxStringInsert ( wxString &s , int from , wxString t )
    {
    s = s.Mid ( 0 , from ) + t + s.Mid ( from ) ;
    }


int main()
    {
    printf("huhu");
    wxString a="0123456789";
    wxString b="b";
    wxString r1=a;
    wxStringInsert(r1,5,b);
    wxString r2=a;
    r2.insert(5,b);
    wxPrintf("wxStringInsert: '%s'\n",r1);
    wxPrintf("wxString::insert: '%s'\n",r2);
    return 0;
    }

