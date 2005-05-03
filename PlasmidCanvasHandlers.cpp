#include "PlasmidCanvas.h"

// Popup menu handlers
    
void PlasmidCanvas::invokeORFpopup ( int item , wxPoint pt )
    {
    wxMenu *cm = new wxMenu ;
    context_last_orf = item ;

    cm->Append ( ORF_AS_NEW_ITEM , txt("p_orf_as_new_item") ) ;

    wxMenu *dm = new wxMenu ;
    dm->Append ( ORF_COPY_DNA , txt("p_orf_copy_dna") ) ;
    dm->Append ( ORF_AS_NEW_DNA , txt("p_orf_as_new_dna") ) ;
    dm->Append ( ORF_BLAST_DNA , txt("m_blast_dna") ) ;
    cm->Append ( POPUP_DUMMY , txt("p_dna_sequence") , dm ) ;
    
    wxMenu *am = new wxMenu ;
    am->Append ( ORF_COPY_AA , txt("p_orf_copy_aa") ) ;
    am->Append ( ORF_AS_NEW_AA , txt("p_orf_as_new_aa") ) ;
    am->Append ( ORF_BLAST_AA , txt("m_blast_aa") ) ;
    cm->Append ( POPUP_DUMMY , txt("p_aa_sequence") , am ) ;
    
    PopupMenu ( cm , pt ) ;
    delete cm ;
    }
    
wxMenu *PlasmidCanvas::invokeItemPopup ( int item , wxPoint pt , bool doreturn )
    {
    wxMenu *cm = new wxMenu ;
    context_last_item = item ;
    int rf = p->vec->items[context_last_item].getRF() ;
    
    cm->Append ( PC_ITEM_EDIT , txt("p_item_edit") ) ;
    if ( p->vec->items[item].isVisible() ) 
        cm->Append ( PC_ITEM_HIDE , txt("d_item_hide") ) ;
    else
        cm->Append ( PC_ITEM_SHOW , txt("d_item_show") ) ;
    cm->Append ( PC_ITEM_DELETE , txt("d_del_item") ) ;
    cm->AppendSeparator () ;

    wxMenu *dm = new wxMenu ;
    dm->Append ( PC_ITEM_MARK , txt("p_item_mark") ) ;
    dm->Append ( PC_ITEM_MARK_SHOW , txt("p_item_mark_show") ) ;
    dm->AppendSeparator () ;
    dm->Append ( PC_ITEM_COPY_DNA , txt("p_item_copy_dna") ) ;
    dm->Append ( PC_ITEM_AS_NEW_SEQUENCE , txt("p_item_as_new_entry") ) ;
    dm->Append ( PC_ITEM_BLAST_DNA , txt("p_item_blast_dna") ) ;
    cm->Append ( POPUP_DUMMY , txt("p_dna_sequence") , dm ) ;

    if ( rf != 0 )
        {
        wxMenu *am = new wxMenu ;
        am->Append ( PC_ITEM_COPY_AA , txt("p_item_copy_aa") ) ;
        am->Append ( PC_ITEM_AS_NEW_AA_SEQUENCE , txt("p_item_as_new_aa_entry") ) ;
        am->Append ( PC_ITEM_BLAST_AA , txt("p_item_blast_aa") ) ;
        cm->Append ( POPUP_DUMMY , txt("p_aa_sequence") , am ) ;
        }

    lastvectorobject = -1 ;
    if ( doreturn ) return cm ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    return NULL ;
    }
    
void PlasmidCanvas::itemMark ( wxCommandEvent &ev )
    {
    wxString id = _T("DNA") ;
    if ( p->def == _T("AminoAcids") ) id = _T("AA") ;
    p->cSequence->mark ( id ,
                    p->vec->items[context_last_item].from ,
                    p->vec->items[context_last_item].to ) ;
    }
    
void PlasmidCanvas::itemMarkShow ( wxCommandEvent &ev )
    {
    itemMark ( ev ) ;
    p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;
    }
    
void PlasmidCanvas::itemEdit ( wxCommandEvent &ev )
    {
    invokeVectorEditor ( _T("item") , context_last_item ) ;
    }

void PlasmidCanvas::itemDelete ( wxCommandEvent &ev )
    {
    p->vec->undo.start ( txt("u_del_item") ) ;
    for ( int a = context_last_item+1 ; a < p->vec->items.size() ; a++ )
        p->vec->items[a-1] = p->vec->items[a] ;
    p->vec->items.pop_back () ;
    p->vec->updateDisplay() ;
    p->vec->undo.stop() ;
    Refresh () ;
    p->updateSequenceCanvas ( true ) ;
    p->treeBox->initme() ;
    }

void PlasmidCanvas::itemShowHide ( wxCommandEvent &ev )
    {
    bool newstate = !p->vec->items[context_last_item].isVisible() ;
    p->vec->items[context_last_item].setVisible ( newstate ) ;
    Refresh () ;
    p->updateSequenceCanvas ( true ) ;
    p->treeBox->SetItemBold ( p->vec->items[context_last_item].treeid , newstate ) ;
    }

// Restriction enzyme context menu
    
wxMenu *PlasmidCanvas::invokeRsPopup ( int rs , wxPoint pt , bool doreturn )
    {
    int a ;
    wxMenu *cm = new wxMenu ;
    context_last_rs = rs ;
    cm->Append ( PC_RS_EDIT , txt("p_rs_edit") ) ;
    cm->Append ( PC_RS_SHOW_HIDE , txt("m_show_hide_enzyme") ) ;
    cm->Append ( PC_RS_DEL , txt("p_rs_del") ) ;
    cm->AppendSeparator () ;
    cm->Append ( PC_RS_MARK , txt("p_rs_mark") ) ;
    cm->Append ( PC_RS_MARK_SHOW , txt("p_rs_mark_show") ) ;
    cm->Append ( PC_RS_INFO , txt("p_rs_info") ) ;
    cm->AppendSeparator () ;
    
    for ( a = 0 ; a < p->vec->cocktail.GetCount() && 
                    p->vec->cocktail[a] != p->vec->rc[context_last_rs].e->name ;
                    a++ ) ;
    if ( a == p->vec->cocktail.GetCount() )
        {
        cm->Append ( PC_RS_ADD2COCKTAIL , txt("p_rs_add_to_cocktail") ) ;
        cm->Append ( PC_RS_ADD_AND_CUT , txt("p_rs_add_to_cocktail_and_cut") ) ;
        }
    else
        {
        cm->Append ( PC_RS_ADD2COCKTAIL , txt("p_rs_del_from_cocktail") ) ;
        }
    
    if ( p->vec->cocktail.GetCount() > 0 )
        {
        cm->Append ( PC_RS_CUT_WITH_COCKTAIL , txt("p_cut_with_cocktail") ) ;
        cm->AppendSeparator() ;
        wxString c = txt("cocktail") ;
        c += _T(" :") ;
        cm->Append ( PC_DUMMY , c ) ;
        for ( a = 0 ; a < p->vec->cocktail.GetCount() ; a++ )
            {
            c = _T("  ") ;
            c += p->vec->cocktail[a] ;
            cm->Append ( PC_DUMMY , c ) ;
            }
        }
        
    lastrestrictionsite = -1 ;
    if ( doreturn ) return cm ;
    PopupMenu ( cm , pt ) ;
    delete cm ;
    return NULL ;
    }
    
void PlasmidCanvas::rsMark ( wxCommandEvent &ev )
    {
    int from = p->vec->rc[context_last_rs].pos - 
                p->vec->rc[context_last_rs].e->cut + 1 ;
    int to = from + p->vec->rc[context_last_rs].e->sequence.length() - 1 ;
    p->cSequence->mark ( _T("DNA") , from , to ) ;
    }
    
void PlasmidCanvas::rsMarkShow ( wxCommandEvent &ev )
    {
    rsMark ( ev ) ;
    p->cSequence->Scroll ( 0 , p->cSequence->getBatchMark() ) ;    
    }
    
void PlasmidCanvas::rsAdd2Cocktail ( wxCommandEvent &ev )
    {
    int a , b ;
    for ( a = 0 ; a < p->vec->cocktail.GetCount() && 
                    p->vec->cocktail[a] != p->vec->rc[context_last_rs].e->name ;
                    a++ ) ;
    if ( a == p->vec->cocktail.GetCount() ) // Add to cocktail
        {
        p->vec->cocktail.Add ( p->vec->rc[context_last_rs].e->name ) ;
        }
    else // Remove from cocktail
        {
        p->vec->cocktail.RemoveAt ( a ) ;
/*        for ( b = a+1 ; b < p->vec->cocktail.GetCount() ; b++ )
           p->vec->cocktail[b-1] = p->vec->cocktail[b] ;
        p->vec->cocktail.pop_back () ;*/
        }
    }
    
void PlasmidCanvas::rsCutWithCocktail ( wxCommandEvent &ev )
    {
    p->vec->setAction ( _T("RESTRICTION") , 20 ) ;
    p->vec->doAction() ;
    }
    
void PlasmidCanvas::rsAddAndCut ( wxCommandEvent &ev )
    {
    rsAdd2Cocktail ( ev ) ;
    rsCutWithCocktail ( ev ) ;
    }

void PlasmidCanvas::rsEdit ( wxCommandEvent &ev )
    {
    invokeVectorEditor ( _T("enzyme") , context_last_rs ) ;
    }
    
void PlasmidCanvas::rsInfo ( wxCommandEvent &ev )
    {
    wxString command = _T("http://rebase.neb.com/rebase/enz/") ;
    command += p->vec->rc[context_last_rs].e->name ;
    command += _T(".html") ;
    command = myapp()->getHTMLCommand ( command ) ;
    wxExecute ( command ) ;
    }
    
void PlasmidCanvas::rsShowHide ( wxCommandEvent &ev )
    {
    if ( context_last_rs < 0 || context_last_rs >= p->vec->rc.size() ) return ;
    p->treeBox->ToggleEnzymeVisibility ( p->vec->rc[context_last_rs].e ) ;
    }
    
void PlasmidCanvas::rsHideLimit ( wxCommandEvent &ev )
    {
    p->cSequence->rsHideLimit ( ev ) ;
    }
    
void PlasmidCanvas::rsDel ( wxCommandEvent &ev )
    {
    p->vec->undo.start ( txt("u_del_enzyme") ) ;
    int a ;
    for ( a = 0 ; a < p->vec->cocktail.GetCount() && 
                    p->vec->cocktail[a] != p->vec->rc[context_last_rs].e->name ;
                    a++ ) ;
    if ( a < p->vec->cocktail.GetCount() )
        rsAdd2Cocktail ( ev ) ; // Effectively removing enzyme from cocktail
        
    for ( a = 0 ; a < p->vec->re.GetCount() &&
                        p->vec->re[a] != p->vec->rc[context_last_rs].e ; a++ ) ;
    for ( a++ ; a < p->vec->re.GetCount() ; a++ )
        p->vec->re[a-1] = p->vec->re[a] ;
    p->vec->re.RemoveAt ( p->vec->re.GetCount() ) ;
    p->vec->updateDisplay() ;
    p->vec->recalculateCuts() ;
    p->treeBox->initme() ;
    p->treeBox->SelectItem ( p->treeBox->vroot ) ;
    p->vec->undo.stop () ;
    Refresh () ;
    p->cSequence->arrange() ;
    p->cSequence->SilentRefresh() ;
    }

// Vector context menu

wxMenu *PlasmidCanvas::invokeVectorPopup ( wxPoint pt , bool doreturn )
    {
    wxMenu *cm = new wxMenu ;
    cm->Append ( PC_VECTOR_EDIT , txt("p_vector_edit") ) ;
    
    if ( p->def == _T("dna") )
       {
        cm->Append(MDI_TRANSFORM_SEQUENCE, txt("t_transform_sequence") );
        if ( p->vec->getType() != TYPE_AMINO_ACIDS )
           cm->Append(PC_RS_HIDE_LIMIT, txt("m_hide_enzymes_limit") );

        // Strands
           {
           wxMenu *pm = new wxMenu ;
           cm->Append ( POPUP_DUMMY , txt("m_strands") , pm ) ;
           pm->Append( STRAND_COPY_53 , txt("m_strand_copy_53") );
           pm->Append( STRAND_COPY_35 , txt("m_strand_copy_35") );
           pm->Append( STRAND_COPY_BOTH , txt("m_strand_copy_both") );
           pm->Append( STRAND_NEW_53 , txt("m_strand_new_53") );
           pm->Append( STRAND_NEW_35 , txt("m_strand_new_35") );
           pm->Append( STRAND_NEW_BOTH , txt("m_strand_new_both") );
           }    
        
        // PCR
        if ( getMarkFrom() == -1 )
           {
           cm->Append(MDI_RUN_PCR, txt("m_pcr") );
           }
        else
           {
           wxMenu *pm = new wxMenu ;
           cm->Append ( POPUP_DUMMY , txt("m_pcr") , pm ) ;
           pm->Append( MDI_RUN_PCR , txt("m_pcr") );
           pm->Append ( PRIMER_FORWARD , txt("m_primer_forward") ) ;
           pm->Append ( PRIMER_BACKWARD , txt("m_primer_backward") ) ;
           pm->Append ( PRIMER_BOTH , txt("m_primer_both") ) ;
           if ( getMarkFrom() + 2 == getMarkTo() )
              pm->Append ( PRIMER_MUTATION , txt("m_primer_mutation") ) ;
           }
        
        if ( p->vec->hasStickyEnds() && p->vec->isLinear() )
            cm->Append ( MDI_FILL_KLENOW , txt("p_fill_klenow") ) ;
    
        if ( getMarkFrom() != -1 )
            {
            wxMenu *mm = new wxMenu ;
            cm->Append ( POPUP_DUMMY , txt("p_selection") , mm ) ;
            mm->Append ( MDI_CUT , txt("m_cut") ) ;
            mm->Append ( MDI_COPY , txt("m_copy") ) ;
            mm->Append ( MDI_COPY_TO_NEW , txt("m_copy_to_new") ) ;
            mm->Append ( PC_WHAT_CUTS , txt("m_what_cuts") ) ;
            mm->Append ( MDI_AS_NEW_FEATURE , txt("m_as_new_feature") ) ;
            if ( p->aa_state != AA_NONE && p->aa_state != AA_ALL )
               {
               mm->Append ( MDI_EXTRACT_AA , txt("m_extract_aa") ) ;
               mm->Append ( PC_BLAST_AA , txt("m_blast_aa") ) ;
               }
    
            mm->Append ( PC_BLAST_DNA , txt("m_blast_dna") ) ;
            
            }
        if ( p->vec->cocktail.GetCount() > 0 )
            {
            cm->Append ( PC_RS_CUT_WITH_COCKTAIL , txt("p_cut_with_cocktail") ) ;
            }
            
        if ( doreturn ) return cm ;
        cm->AppendSeparator();
        }
    
    wxMenu *pm = new wxMenu ;
    cm->Append ( POPUP_DUMMY , txt("m_plasmid_map") , pm ) ;
    pm->Append ( PC_SAVE_IMAGE , txt("m_save_image") ) ;
    pm->Append ( PC_COPY_IMAGE , txt("m_copy_image") ) ;
    pm->Append ( MDI_PRINT_IMAGE , txt("m_print_image") ) ;
    
    if ( p->def == _T("dna") )
       {
       cm->Append(MDI_ORFS, txt("m_orfs") );
       cm->Append(MDI_EDIT_ORFS, txt("m_edit_orfs") );
       }

    PopupMenu ( cm , pt ) ;
    delete cm ;
    }

void PlasmidCanvas::vecEdit ( wxCommandEvent &ev )
    {
    invokeVectorEditor ( _T("") , 0 ) ;
    }
    
void PlasmidCanvas::blastDNA ( wxCommandEvent &ev )
    {
    if ( getMarkFrom() == -1 ) return ;
    wxString seq ;
    int a ;
    for ( a = getMarkFrom() ; a <= getMarkTo() ; a++ )
        {
        seq += p->vec->getNucleotide ( a-1 ) ;
        }
    if ( seq.IsEmpty() ) return ;
    myapp()->frame->blast ( seq , _T("blastn") ) ;
    }
        
void PlasmidCanvas::blastAA ( wxCommandEvent &ev )
    {
    wxString seq = p->doExtractAA() ;
    int a ;
    for ( a = 0 ; a < seq.length() && seq.GetChar(a) == ' ' ; a++ ) ;
    if ( a == seq.length() )
       {
       wxMessageBox ( txt("t_no_or_empty_sequence") , 
                      txt("t_blast_failed") ,
                      wxOK | wxICON_ERROR  ) ;
       return ;
       }
    myapp()->frame->blast ( seq , _T("blastp") ) ;
    }

void PlasmidCanvas::RunPrimerEditor ( vector <TPrimer> &pl , int mut )
    {
    TPrimerDesign *subframe = new TPrimerDesign ( 
            myapp()->frame->getCommonParent() , 
            txt("t_pcr") ,
            p->vec ,
            pl , mut ) ;
    
    // Give it an icon
#ifdef __WXMSW__
    subframe->SetIcon(wxIcon("chrt_icn"));
#else
    subframe->SetIcon(wxIcon( mondrian_xpm ));
#endif

    subframe->initme () ;

    subframe->Show() ;
    subframe->Maximize() ;

    
    myapp()->frame->mainTree->addChild ( subframe , TYPE_PRIMER ) ;
    myapp()->frame->setChild ( subframe ) ;
    myapp()->frame->activateChild ( myapp()->frame->children.GetCount()-1 ) ;
//    myapp()->frame->children.Add ( subframe ) ;
    }

// Primer handler

void PlasmidCanvas::OnPrimerForward ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int end = getMarkFrom() + 30 ;
    if ( end >= p->vec->getSequenceLength() ) end = p->vec->getSequenceLength() ;
    pl.push_back ( TPrimer ( getMarkFrom() , end , true ) ) ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::OnPrimerBackward ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int start = getMarkTo() - 30 ;
    if ( start < 1 ) start = 1 ;
    pl.push_back ( TPrimer ( start , getMarkTo() , false ) ) ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::OnPrimerBoth ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int end = getMarkFrom() + 30 ;
    if ( end >= p->vec->getSequenceLength() ) end = p->vec->getSequenceLength() ;
    pl.push_back ( TPrimer ( getMarkFrom() , end , true ) ) ;
    int start = getMarkTo() - 30 ;
    if ( start < 1 ) start = 1 ;
    pl.push_back ( TPrimer ( start , getMarkTo() , false ) ) ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::OnPrimerMutation ( wxCommandEvent &ev )
    {
    vector <TPrimer> pl ;
    int start = getMarkFrom() - 10 ;
    int end = getMarkFrom() + 20 ;
    if ( end >= p->vec->getSequenceLength() ) end = p->vec->getSequenceLength() ;
    if ( start < 1 ) start = 1 ;
    pl.push_back ( TPrimer ( start , end , true ) ) ;
    
    start = getMarkFrom() - 20 ;
    end = getMarkTo() + 10 ;
    if ( end >= p->vec->getSequenceLength() ) end = p->vec->getSequenceLength() ;
    if ( start < 1 ) start = 1 ;    
    pl.push_back ( TPrimer ( start , getMarkTo() , false ) ) ;
    
    RunPrimerEditor ( pl ) ;
    }

// **** ORF popup menu handlers    

wxString PlasmidCanvas::getDNAorAA ( int from , int to , int dir , bool dna )
    {
    int a ;
    wxString r , s ;
    if ( to < from ) to += p->vec->getSequenceLength() ;
    
    for ( a = from ; a <= to ; a++ )
        {
        int b = a ;
        if ( a >= p->vec->getSequenceLength() ) b -= p->vec->getSequenceLength() ;
        r += p->vec->getNucleotide ( b ) ;
        }
    
    if ( dir < 0 )
        {
        s = _T("") ;
        for ( a = 0 ; a < r.length() ; a++ )
           s = ( (wxChar) p->vec->getComplement ( r.GetChar(a) ) ) + s ;
        r = s ;
        }
    if ( !dna )
        {
        s = _T("") ;
        for ( a = 0 ; a+2 < r.length() ; a += 3 )
           {
           wxString t = p->vec->dna2aa ( r.substr ( a , 3 ) ) ;
           if ( t != _T("|") ) s += t ;
           }
        r = s ;
        }
    return r ;
    }

void PlasmidCanvas::orfCopyDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->getORF(context_last_orf)->from ;
    int to = p->vec->getORF(context_last_orf)->to ;
    wxString s = getDNAorAA ( from , to , p->vec->getORF(context_last_orf)->rf ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::orfCopyAA ( wxCommandEvent &ev )
    {
    int from = p->vec->getORF(context_last_orf)->from ;
    int to = p->vec->getORF(context_last_orf)->to ;
    wxString s = getDNAorAA ( from , to , p->vec->getORF(context_last_orf)->rf , false ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::orfAsNewItem ( wxCommandEvent &ev )
    {
//    char t[1000] ;
    int from = p->vec->getORF(context_last_orf)->from+1 ;
    int to = p->vec->getORF(context_last_orf)->to+1 ;

    if ( to > p->vec->getSequenceLength() )
        to -= p->vec->getSequenceLength() ;

    TVectorItem nvi ;
//    sprintf ( t , txt("t_new_item_title") , from , to ) ;
    nvi.name = wxString::Format ( txt("t_new_item_title") , from , to ) ; //t ;
    nvi.direction = (p->vec->getORF(context_last_orf)->rf>0)?1:-1 ;
    nvi.type = VIT_CDS ;
    nvi.from = from ;
    nvi.to = to ;
    nvi.setParam ( _T("/codon_start") , 1 ) ;
    p->vec->undo.start ( txt("u_orf_new_item") ) ;
    p->vec->items.push_back ( nvi ) ;
    invokeVectorEditor ( _T("item") , p->vec->items.size()-1 , true ) ;
    p->vec->undo.stop() ;
    p->vec->updateDisplay() ;
    Refresh () ;
    }

void PlasmidCanvas::orfAsNewDNA ( wxCommandEvent &ev )
    {
    TVector *nv = new TVector ;
    int from = p->vec->getORF(context_last_orf)->from ;
    int to = p->vec->getORF(context_last_orf)->to ;
    nv->setSequence ( getDNAorAA ( from , to , p->vec->getORF(context_last_orf)->rf ) ) ;
    nv->setName ( p->vec->getName() + _T(" (") + wxString ( txt("t_orf_extracted") ) + _T(")") ) ;
    nv->setDescription ( p->vec->getDescription() + _T("\n") + wxString ( txt("t_orf_extracted") ) ) ;
    nv->setChanged () ;
    nv->setCircular ( false ) ;
    nv->recalculateCuts() ;
    nv->updateDisplay() ;
    myapp()->frame->newFromVector ( nv ) ;
    }

void PlasmidCanvas::orfAsNewAA ( wxCommandEvent &ev )
    {
    int from = p->vec->getORF(context_last_orf)->from ;
    int to = p->vec->getORF(context_last_orf)->to ;
    wxString seq = getDNAorAA ( from , to , p->vec->getORF(context_last_orf)->rf , false ) ;
    wxString n = p->vec->getName() + _T(" (") ;
    n += txt ("t_orf_extracted") ;
    n += _T(")") ;
    TVector *vvv = p->vec->getAAvector ( from+1 , to+1 , p->vec->getORF(context_last_orf)->rf ) ;
    TAminoAcids *aaa = myapp()->frame->newAminoAcids ( vvv , n ) ;
    aaa->vec->setChanged() ;
    }

void PlasmidCanvas::orfBlastDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->getORF(context_last_orf)->from ;
    int to = p->vec->getORF(context_last_orf)->to ;
    wxString s = getDNAorAA ( from , to , p->vec->getORF(context_last_orf)->rf ) ;
    if ( s.IsEmpty() ) return ;
    myapp()->frame->blast ( s , _T("blastn") ) ;
    }
        
void PlasmidCanvas::orfBlastAA ( wxCommandEvent &ev )
    {
    int from = p->vec->getORF(context_last_orf)->from ;
    int to = p->vec->getORF(context_last_orf)->to ;
    wxString s = getDNAorAA ( from , to , p->vec->getORF(context_last_orf)->rf , false ) ;
    if ( s.IsEmpty() ) return ;
    myapp()->frame->blast ( s , _T("blastp") ) ;
    }

// *** More item handlers

void PlasmidCanvas::itemBlastDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    wxString s = getDNAorAA ( from , to , dir ) ;
    if ( s.IsEmpty() ) return ;
    myapp()->frame->blast ( s , _T("blastn") ) ;
    }
        
void PlasmidCanvas::itemBlastAA ( wxCommandEvent &ev )
    {
/*    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf == 0 ) return ;
    from += dir * ( rf - 1 ) ;
    to += dir * ( rf - 1 ) ;
    wxString s = getDNAorAA ( from , to , dir , false ) ;*/
    wxString s = p->vec->items[context_last_item].getAminoAcidSequence () ;
    if ( s.IsEmpty() ) return ;
    myapp()->frame->blast ( s , _T("blastp") ) ;
    }

void PlasmidCanvas::itemCopyAA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf == 0 ) return ;
    from += dir * ( rf - 1 ) - 1 ;
    to += dir * ( rf - 1 ) - 1 ;
    wxString s = getDNAorAA ( from , to , dir , false ) ;
    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::itemAsNewAA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf == 0 ) return ;
    from += dir * ( rf - 1 ) - 1 ;
    to += dir * ( rf - 1 ) - 1 ;
    wxString n = p->vec->items[context_last_item].name ;
    n += _T(" (") + p->vec->getName() + _T(")") ;
    TVector *nv = p->vec->getAAvector ( from+1 , to+1 , dir ) ;
    TAminoAcids *aaa = myapp()->frame->newAminoAcids ( nv , n ) ;
    delete nv ;
    aaa->vec->setChanged() ;
    }
    
void PlasmidCanvas::itemAsNewSequence ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    int rf = p->vec->items[context_last_item].getRF() ;

    if ( rf == 0 ) // No coding sequence, just copy the DNA and get outta here
        {
        itemMark ( ev ) ;
        p->OnCopyToNew ( ev ) ;
        }
    else // Coding sequence
        {
        from += dir * ( rf - 1 ) - 1 ;
        to += dir * ( rf - 1 ) - 1 ;
        wxString n = p->vec->items[context_last_item].name ;
        n += _T(" (") + p->vec->getName() + _T(")") ;
    
        TVector *nv = new TVector ;
        nv->setSequence ( getDNAorAA ( from , to , dir , true ) ) ;
        nv->setName ( n ) ;
        nv->setChanged () ;
        nv->setCircular ( false ) ;
        nv->recalculateCuts() ;
        nv->updateDisplay() ;
        myapp()->frame->newFromVector ( nv ) ;
        }
    }    

void PlasmidCanvas::itemCopyDNA ( wxCommandEvent &ev )
    {
    int from = p->vec->items[context_last_item].from ;
    int to = p->vec->items[context_last_item].to ;
    int dir = p->vec->items[context_last_item].direction ;
    
    int rf = p->vec->items[context_last_item].getRF() ;
    if ( rf != 0 )
        {
        from += dir * ( rf - 1 ) - 1 ;
        to += dir * ( rf - 1 ) - 1 ;
        }

    wxString s = getDNAorAA ( from , to , dir ) ;

    if (wxTheClipboard->Open())
        {
        wxTheClipboard->SetData( new wxTextDataObject(s) );
        wxTheClipboard->Close();
        }    
    }

void PlasmidCanvas::OnFillKlenow(wxCommandEvent& event)
    {
    p->vec->undo.start ( txt("u_fill_klenow") ) ;
    TVector *v = p->vec ;
    wxString l = v->getStickyEnd(true,true) + v->getStickyEnd(true,false) ;
    wxString r = v->getStickyEnd(false,true) + v->getStickyEnd(false,false) ;
    int a ;
    if ( !v->getStickyEnd(true,false).IsEmpty() )
        for ( a = 0 ; a < l.length() ; a++ )
           l[(uint)a] = v->getComplement ( l[(uint)a] ) ;
    if ( !v->getStickyEnd(false,false).IsEmpty() )
        for ( a = 0 ; a < r.length() ; a++ )
           r[(uint)a] = v->getComplement ( r[(uint)a] ) ;
    v->setSequence ( l + v->getSequence() + r ) ;
    for ( a = 0 ; a < v->items.size() ; a++ )
        {
        v->items[a].from += l.length() ;
        v->items[a].to += l.length() ;
        }
    v->setStickyEnd ( true , true , _T("") ) ;
    v->setStickyEnd ( false , true , _T("") ) ;
    v->setStickyEnd ( true , false , _T("") ) ;
    v->setStickyEnd ( false , false , _T("") ) ;
    v->updateDisplay() ;
    v->recalculateCuts() ;
    Refresh () ;
    SeqDNA *dna = (SeqDNA*) p->cSequence->findID ( _T("DNA") ) ;
    dna->initFromTVector ( v ) ;
    p->updateSequenceCanvas() ;
    p->cSequence->arrange () ;
    p->vec->undo.stop () ;
    p->cSequence->Refresh () ;
    p->treeBox->initme() ;
    }

void PlasmidCanvas::OnRunPCR(wxCommandEvent& event)
    {
    vector <TPrimer> pl ;
    RunPrimerEditor ( pl ) ;
    }

void PlasmidCanvas::setZoom ( int factor )
    {
    zoom = factor ;
    p->vec->updateDisplay() ;
    int vx , vy ;
    GetClientSize ( &vx , &vy ) ;
    vx = vx * factor / 100 ;
    vy = vy * factor / 100 ;
    SetScrollbars ( 1 , 1 , vx , vy , false ) ;
    SetVirtualSize ( vx , vy ) ;
    Refresh () ;
    }
    
void PlasmidCanvas::OnWhatCuts(wxCommandEvent& event)
    {
    p->cSequence->OnWhatCuts(event) ;
    }
    
void PlasmidCanvas::OnStrandCopy35(wxCommandEvent& event)
	{
 	p->cSequence->OnStrandCopy35 ( event ) ;
	}    

void PlasmidCanvas::OnStrandCopy53(wxCommandEvent& event)
	{
 	p->cSequence->OnStrandCopy53 ( event ) ;
	}    

void PlasmidCanvas::OnStrandCopyBoth(wxCommandEvent& event)
	{
 	p->cSequence->OnStrandCopyBoth ( event ) ;
	}    

void PlasmidCanvas::OnStrandNew35(wxCommandEvent& event)
	{
 	p->cSequence->OnStrandNew35 ( event ) ;
	}    

void PlasmidCanvas::OnStrandNew53(wxCommandEvent& event)
	{
 	p->cSequence->OnStrandNew53 ( event ) ;
	}    

void PlasmidCanvas::OnStrandNewBoth(wxCommandEvent& event)
	{
 	p->cSequence->OnStrandNewBoth ( event ) ;
	}    

