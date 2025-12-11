#ifndef __TEliteLaChromLogDialog__H__
#define __TEliteLaChromLogDialog__H__

#include <wx/wx.h>
#include <wx/listctrl.h> // wxListCtrl
#include <wx/dialog.h> // wxDialog

#define ELL_DATE 0
#define ELL_TIME 1
#define ELL_SAMPLE 2
#define ELL_FILE 3
#define ELL_METHOD 4
#define ELL_USER 5
#define ELL_VIAL 6
#define ELL_VOLUME 7
#define ELL_AUTOSAMPLER 8
#define ELL_PROGRAM 9
#define ELL_FORMA 10
#define ELL_DATE_TIME 11
#define ELL_DATA1 9
#define ELL_DATA2 10
#define ELL_DATA3 11

class TEliteLaChromLogDialog : public wxDialog
{
    public :
    TEliteLaChromLogDialog ( wxWindow *parent, const wxString& title , const wxString& basefile ) ;

    virtual void OnCharHook(wxKeyEvent& event) ; ///< Key event handler
    virtual void OnDatesList(wxCommandEvent &event);
    virtual void OnUserList(wxCommandEvent &event);
    virtual void OnExcel(wxCommandEvent &event);

    private :
    void ReadFile ( const wxString& filename , wxArrayString &data ) ;
    void Uniquify ( wxArrayString &data , std::vector <wxArrayString> &tabs ) ;
    void FilterLines () ;
    void UpdateLines () ;
    void SortTabs ( std::vector <wxArrayString> &tabs ) ;
    void MergeWith ( std::vector <wxArrayString> &tabs , int col ) ;

    wxString filename_apex , filename_area , filename_estdconc ;
    wxArrayString data_apex , data_area , data_estdconc ;
    wxArrayString unique[12] , col_headers ;
    std::vector <wxArrayString> tabs_apex , tabs_area , tabs_estdconc , tabs_merged , tabs_display ;

    wxListBox *unique_dates , *unique_users ;
    wxListCtrl *lines ;

    DECLARE_EVENT_TABLE()
} ;

#endif
