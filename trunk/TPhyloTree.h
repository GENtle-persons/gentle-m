#ifndef _TPHYLOTREE_
#define _TPHYLOTREE_

#include "main.h"

class TPhyloTreeBox ;

class TPTree
	{
	public :
	TPTree () ;
	virtual ~TPTree () ;
	virtual wxString scanNewick ( wxString s ) ;
	virtual void setWeight ( double w ) { weight = w ; }
	virtual void setName ( wxString n ) { name = n ; }
	virtual float getWeight () { return weight ; }
	virtual wxString getName () { return name ; }
	virtual bool isLeaf () { return children.size() == 0 ; }
	virtual double getMaxWeight () ;
	virtual double getCurrentWeight () ;
	virtual int getMaxDepth () ;
	virtual int getCurrentDepth () ;
	virtual int countLeafs () ;
	virtual int numberLeafs ( int i = 0 ) ;
	
	virtual void filterDepth ( int depth , vector <TPTree*> &vt ) ;
	virtual void getAll ( vector <TPTree*> &vt ) ;
	virtual void averageY () ;
	
	virtual void drawRecursive ( wxDC &dc , wxString mode ) ;
	
	wxRect rect ;
	vector <TPTree*> children ;
	
	private :
	virtual void dedigitize ( wxString &s ) ;
	
	double weight ;
	wxString name ;
	TPTree* parent ;
	int leafNumber ;
	} ;

/**	\class TPhyloTree
	\brief The phylogenetic tree module
*/
class TPhyloTree : public ChildBase
    {
    public :
    TPhyloTree(wxWindow *parent, const wxString& title) ; ///< Constructor
    
    virtual void initme () ; ///< Initialization
    virtual wxString getName () ; ///< Returns the gel module name
    
    virtual void setNewickTrees ( wxString s , TAlignment *_ali = NULL ) ;
    virtual void setModeStrange () ;
    virtual void setModeDrawgram () ;
    virtual void setRealNames ( TAlignment *ali ) ;
    
    virtual void OnDirectLines(wxCommandEvent& event); ///< Toggle direct lines
    virtual void OnTreeList(wxCommandEvent& event); ///< Tree list event
    
    private :
	 friend class TPhyloTreeBox ;

    virtual void setNewickTree ( wxString s ) ;
    virtual void updateTreeList () ;
    virtual void updateDisplay () ;

	 TPhyloTreeBox *box ;
	 TPTree *tree ;
	 wxListBox *treelist ;
	 vector <TPTree *> trees ;
	 wxString mode ;
	 bool directlines ;
	 wxCheckBox *cb_directlines ;
	 TAlignment *ali ;
    
    DECLARE_EVENT_TABLE()
    } ;

class TPhyloTreeBox : public wxControl
	{
	public :
	TPhyloTreeBox ( wxWindow *parent , int id = -1 ) ; ///< Constructor
	
	virtual void OnDraw(wxDC& dc) ; ///< Draw event handler
	virtual void OnPaint(wxPaintEvent& event) ; ///< Paint event handler
	
	virtual void OnEvent(wxMouseEvent& event); ///< Mouse event handler
	virtual void OnSaveAsBitmap(wxCommandEvent &event); ///< Save image handler
	virtual void OnCopy(wxCommandEvent &event); ///< Copy event handler
	virtual void OnPrint(wxCommandEvent &event); ///< Print event handler
	virtual void OnDummy(wxCommandEvent& WXUNUSED(event)){}; ///< Dummy event handler
	
	private :
	
	virtual void WriteIntoBitmap(wxBitmap &bmp) ;
	
	TPhyloTree *_parent ;
	
	DECLARE_EVENT_TABLE()
	} ;

#endif
