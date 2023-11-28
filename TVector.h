/** \file
    \brief Contains the TVector class, as well as helper classes TAAProp, TORF, Tdna2aa, and TVectorItem
*/
#ifndef _TVECTOR_H_
#define _TVECTOR_H_

#include "main.h"
#include "TUndo.h"

class MyChild ;
class TRestrictionEnzyme ;
class TRestrictionCut ;
class TGenBank ;
class TVectorTree ;
class PlasmidCanvas ;
class TVectorEditor ;
class ChildBase ;
class SeqAA ;
class TEnzymeRules ;

//number of possible types
#define VIT_TYPES     10

#define VIT_GENE       1
#define VIT_CDS        2
#define VIT_REP_ORI    3
#define VIT_PROMOTER   4
#define VIT_TERMINATOR 5
#define VIT_MISC       6
#define VIT_PROT_BIND  7
#define VIT_ORI_T      8
#define VIT_SEQUENCING 9

/// This class manages amino acid properties; so 20 total. Used by the static part of TVector
class TAAProp
{
  public :
    TAAProp () ; ///< Constructor
    ~TAAProp () {} ; ///< Destructor (empty)
    void set_cf ( const int pa , const int pb , const int pt , const float f0 , const float f1 , const float f2 , const float f3 ) ; ///< Some data
    void set_data ( const float _mw , const float _pi , const wxString& _tla ) ; ///< Molecular weight, isoelectric point, three-letter acronym
    void set_hp ( const float _hp_kd , const float _hp_hw ) ; ///< More data
    void set_atoms ( const int C , const int H , const int N , const int O , const int S ) ; ///< Atoms in this amino acid
    void set_halflife ( const int mammal , const int yeast , const int ecoli ) ; ///< Half-life (not the game!)
    wxString get_halflife_text ( const int hl ) const ; ///< Returns half-life estimation
    wxString tla ; ///< Three-letter acronym
    float mw , pi ;
    float cf_f[4] ; ///< Some data
    int cf_pa , cf_pb , cf_pt ;
    int carbon , hydrogen , nitrogen , oxygen , sulfur ;
    int hl_mammal , hl_yeast , hl_ecoli ;
    float hp_kd , hp_hw ;
    vector <float> data ; ///< For temporary data only
} ;

/// This class manages open reading frames (ORFs)
class TORF
    {
  public :
    TORF () ; ///< Default constructor, empty
    TORF ( const int _f , const int _t , const int _r ) ; ///< Constructor
    ~TORF () {} ; ///< Destructor (empty)

    wxString getText () const ;
    void set ( const int _f , const int _t , const int _r ) ;
    inline int get_from () const { return from ; }
    inline int get_to () const { return to ; }
    inline int get_rf () const { return rf ; }

    float dist1 , dist2 ;
    float deg1 , deg2 ;

  private:
    /// \brief Start of ORF
    int from , to ; ///< End of ORF
    int rf ; ///< Reading frame

    } ;

/// This class stores a codon and the corresponding amino acid. Used by TVector
class Tdna2aa
    {
    public :
    Tdna2aa () {} ; ///< Default constructor, empty
    Tdna2aa ( const char _aa , const int i1 , const int i2 , const int i3 ) ; ///< Constructor
    char aa ; ///< Single-letter AA code
    int dna[3] ; ///< The three DNA positions that make up the codon
    } ;

/// This class stores a vector item.
class TVectorItem
{
 public :
    TVectorItem () ; ///< Default constructor
    TVectorItem ( const wxString& sn , const wxString& n , const int f , const int t , const char ty ) ; ///< Constructor
    ~TVectorItem () {} ; ///< Destructor (empty)

    wxBrush *getBrush () const ; ///< Returns a pointer to the brush used to draw the item
    wxColour getFontColor () const ; ///< Returns the font color
    void setColor ( const wxColour& col ) ; ///< Sets the font color
    bool isVisible () const ; ///< Is this item visible?
    void setVisible ( const bool b ) ; ///< Set visibility
    int getRF () const ; ///< Returns the reading frame
    void setRF ( const int x ) ; ///< Sets the reading frame
    int getOffset () const ; ///< returns the offset; -1 = no offset
    void setOffset ( const int o = -1 ) ; ///< Sets the offset; -1 = no offset
    void setType ( const wxString& s ) ; ///< Sets the item type
    int getOffsetAt ( const int i ) const; ///< Returns the offset of the item at a specific position

    void doRemove ( const int f , const int t , const int l ) ; ///< Remove a part of the item
    wxString implodeParams () const ; ///< Join item parameters for storage
    void explodeParams ( const wxString& _s ) ; ///< Extract item parameters from stored form
    wxTreeItemId getTreeID () const { return treeid ; } ; ///< The current ID of the item in TVectorTree
    void setTreeID ( wxTreeItemId newid ) { treeid = newid ; } ///< Set new item ID in TVectorTree
    int getDirection () const { return direction ; } ///< Return the direction
    void setDirection ( int newdir ) { direction = newdir ; } ///< Set the direction
    char getType () const { return type ; } ///< Returns the item type
    void setType ( char newtype ) { type = newtype ; } ///< Sets the item type
    int getMem () const ; ///< Estimates the memory usage of this item (debugging use only)

    // Parameters
    const wxString getParam ( const wxString& p , const wxString& def = _T("") ) const ; ///< Returns the value of a parameter; optional default value
    wxArrayString getParamKeys () const ; ///< Get a list of all parameter keys
    void setParam ( const wxString& p , const wxString& v ) ; ///< Set a parameter key/value pair
    void setParam ( const wxString& p , const int v ) ; ///< Set a parameter key/value pair

    // dna2aa stuff
    void translate ( TVector * const v , SeqAA * const aa , vector <Tdna2aa> &dna2aa ) ; ///< Translate DNA to amino acids
    void getArrangedAA ( TVector * const v , wxString &s , const int disp , SeqAA *aa = NULL ) ; ///< Generate the amino acid sequence in place, not const
    wxString getAminoAcidSequence () ; ///< Return the amino acid sequence
    void setLastVector ( TVector * const v) ; ///< Set the last TVector to own this item

    // Variables
    /// \brief Item description
    wxString desc , name ; ///< Item name

    int from , ///< Item start
        to ; ///< Item end

    wxString toString() const ;

  private:
    void initParams () ; ///< Reset parameters

    signed char direction ; ///< The item direction; 1=clockwise, -1=counter-clockwise
    char type ; ///< The type of the item in VIT_XXX form
    wxTreeItemId treeid ; ///< The item ID in the TVectorTree

    /// \brief Parameter keys
    wxArrayString pname , pvalue ; ///< Parameter values
    vector <Tdna2aa> dna2aa_item ; ///< The cache of the translated amino acids
    TVector *lastVector ; ///< The last TVector to own this item

    // Visual information
    friend class TVectorEditor ;
    friend class MyChild ;
    friend class PlasmidCanvas ;
    friend class MyFrame ;
    int r1 , r2 ;
    int r3 , r4 ; // Only used in linar display mode
    float a1 , a2 ;
} ;

/** \brief This class stores all sequence information, both DNA and amino acids

    This class is of extreme importance. It carries sequence data, annotated items,
    methods for sequence manipulation, and static data, e.e., about amino acid
    properties and translation tables. So don't let the name fool you, this class
    does much more than storing vectors.
*/
class TVector
    {
    public :
    TVector ( ChildBase *win = NULL ) ; ///< Constructor
    ~TVector () ; ///< Destructor
    void init () ; ///< Set up basic values
    void clear () ; ///< Reset internal state

    // Restriction enzymes
    void recalculateCuts () ; ///< Recalculate restriction enzyme cuts
    void getCuts ( const TRestrictionEnzyme * const e , vector <TRestrictionCut> &ret , const bool clear_vector = true , const int max = 10000000 ) const ; ///< Gets the cuts of restriction enzyme in this sequence
    bool reduceToFragment ( TRestrictionCut left , TRestrictionCut right ) ; ///< Cuts off everything except what is betreen these two cuts
    void doRestriction () ; ///< Performs restriction. See TRestrictionEditor
    void sortRestrictionSites () ; ///< Sorts the restriction sites by point of cut
    int countCuts ( const wxString& enzyme ) const ; ///< Counts the number of cuts for an enzyme in this sequence
    void methylationSites ( wxArrayInt &vi , int what = ALL_METHYLATION_ENZYMES ) ; ///< Finds methylation sites

    // Nucleotide access/conversion
    bool basematch ( const char b1 , const char b2 ) const ; ///< matches DNA bases; b1 in IUPAC, b2 in SIUPAC. This is superior to the "=" operator :-)
    void setIUPAC ( const char b , const wxString& s , char * pac = NULL ) const ;
    char getNucleotide ( const int pos , const bool complement = false ) const ; ///< Returns the base at a certain position, or its complement
    void setNucleotide ( const int pos , const char t ) ; ///< Sets a base at a certain position
    char getComplement ( const char c ) const ; ///< Returns the complement of a given base

    // Vector/sequence access
    void ligate_right ( TVector &v , const bool inverted = false ) ; ///< Adds another TVector to the right of this one
    void closeCircle () ; ///< Closes a linear sequence into a circular (intramolecular ligation)
    void turn ( const int off ) ; ///< Turns a circular sequence "off" bases

    TVector *getAAvector ( const int from , const int to , const int dir = 1 ) const ; ///< Returns part of the sequence translated into amino acids
    void doAction () ; ///< Perform an action (restriction, usually). Internal use
    void doRemove ( const int from , const int to , const bool update = true , const bool enableUndo = true ) ; ///< Removes part of the sequence
    void insert_char ( const char x , const int pos , const bool overwrite = false ) ; ///< Inserts a character into the sequence

    float getAAmw ( const char aa ) const ; ///< Returns the molecular weight of an amino acid
    float getAApi ( const char aa ) const ; ///< Returns the isoelectric point of an amino acid
    wxString dna2aa ( const wxString& codon , const int translation_table = -1 ) const ; ///< Translates a codon into an amino acid
    void setAction ( const wxString& _action , int _action_value = 0 ) ; ///< Sets the action for doAction()
    void setDatabase ( const wxString& s ) { database = s ; } ///< Sets the database in which the sequence is stored
    const wxString getDatabase () const { return database ; } ///< Returns the database name in which the sequence is stored
    TVector *newFromMark ( const int from , const int to ) const ; ///< Generates a new sequence based on a part of this one

    void setChanged ( const bool c = true ) ; ///< The sequence has been changed.
    bool isChanged () const { return changed ; } ///< Was the sequence changed?

    void ClearORFs () ; ///< Clear the found open reading frames
    void addORFs ( const int off ) ; ///< Add open reading frames for a specific reading frame

    void removeBlanksFromSequence () ; ///< Removes blank chars (spaces etc.) from the sequence
    void removeBlanksFromVector () ; ///< Removes blank chars (spaces etc.) from the sequence, altering items if necessary

    wxString getParams () const ; ///< Return sequence parameters
    void setParams ( const wxString& t ) ; ///< Set sequence parameters
    wxString getParam ( const wxString& key ) const ; ///< Return the value of a parameter
    void setParam ( const wxString& key , const wxString& value ) ; ///< Set a parameter key/value pair
    void setWindow ( ChildBase * const c ) ; ///< Set the window which owns this sequence
    void setCircular ( const bool c = true ) ; ///< This is a circular sequence (or linear, if c == false)
    bool isCircular () const ; ///< Is this a circular sequence?
    bool isLinear () const ; ///< Is this a linear sequence?
    const wxString one2three ( const int a ) const ; ///< Converts one letter amino acid code to three-letter acronym
    char three2one ( wxString s ) const ; ///< Converts three-letter acronym to one letter amino acid code
    void setStickyEnd ( const bool left , const bool upper , const wxString& s ) ; ///< Sets one of the four possible sticky ends
    wxString getStickyEnd ( const bool left , const bool upper ) const ; ///< Returns one of the possible sticky ends
    bool hasStickyEnds () const ; ///< Does this sequence have sticky ends?
    void callUpdateUndoMenu () ; ///< Refreshes the Undo menu
    void setFromVector ( TVector v ) ; ///< Makes this sequence a copy of another one (v)
    void doRemoveNucleotide ( const int x ) ; ///<Removes single base at position x
    int getItemLength ( const int a ) const ; ///< Return the length of item a
    TVector *backtranslate ( const wxString& mode = _T("") ) ; ///< Generate a new DNA sequence from this amino acid sequence
    wxString getStrand53 () const ; ///< Returns the 5'->3' strand of the sequence
    wxString getStrand35 () const ; ///< Returns the 3'->5' strand of the sequence, as 5'->3'

    TAAProp getAAprop ( const char a ) const ; ///< Returns the properties of the one-letter code amino acid
    int find_item ( const wxString& s ) const ; ///< Finds an item with that name
    bool isEnzymeHidden ( const wxString&s ) const ; ///< Is enzyme "s" hidden?
    void hideEnzyme ( const wxString& s , bool hideit = true ) ; ///< Set enzyme hidden state

    wxString getSubstring ( const int mf , const int mt ) ; ///< Returns a sequence substring
    wxString transformSequence ( const bool inverse , const bool reverse ) const ; ///< Transforms the sequence
    wxString getSequence () const ; ///< Returns the sequence
    wxString *getSequencePointer () ; ///< Internal use; only used by SequenceCanvas::OnCharHook
    char getSequenceChar ( const int x ) const; ///< Gets base at position x
    void setSequence ( const wxString& ns ) ; ///< Sets the sequence
    void addToSequence ( const wxString& x ) ; ///< Appends to the sequence
    void alterSequence ( const int pos , const char c ) ; ///< Changes the base at position pos into c
    int getSequenceLength() const ; ///< Returns the sequence length
    void eraseSequence ( const int from , const int len ) ; ///< Removes part of the sequence
    void removeAlignmentArtifacts ( const char what = '-' ) ; ///< Removes alignment artifacts from the sequence

    const wxString getDescription () const ; ///< Returns the sequence description
    void setDescription ( const wxString& s ) ; ///< Sets the sequence description
    void addDescription ( const wxString& s ) ; ///< Appends to the sequence description
    const wxString getName () const ; ///< Returns the sequence name
    void setName ( const wxString& s ) ; ///< Sets the sequence name
    void addName ( const wxString& s ) ; ///< Appends to the sequence name
    void setGenomeMode ( bool gm = true ) ; ///< Turns genome mode on/off
    bool getGenomeMode () const ; ///< Returns the genome mode
    int getMem () const ; ///< Returns memory usage estimate for this sequence. Debugging use only
    bool getVectorCuts ( TVector *v ) ; ///< Returns wether or not isoenzymes should be joined
    TEnzymeRules *getEnzymeRule () const ; ///< Gets the enzyme rules to follow
    int showGC () const ; ///< Returns 0 for "no", otherwise the number of blocks
    TORF *getORF ( const int a ) ; ///< Returns an open reading frame
    int countORFs () ; ///< Returns the number of found open reading frames
    void updateDisplay ( const bool update = true ) ; ///< Recalc visual information on next draw
    bool displayUpdate () const ; ///< Update the display?
    void setType ( const int newtype ) ; ///< Set the sequence type
    int getType () const ; ///< Returns the sequence type
    int getMethylationSiteIndex ( const int pos ) const ; ///< Returns the index of a potential methylation site
    int getMethylationSite ( const int index ) const ; ///< Returns the position of a methylation site
    int countMethylationSites () const ; ///< Returns the number of found methylation sites
    void prepareFeatureEdit ( const int pos , const bool overwrite ) ; ///< Changes feature names or cuts features that are about to be edited
    TEnzymeRules *getEnzymeRules () const ; ///< Returns the restriction enzyme display rules for this vector
    void setEnzymeRules ( TEnzymeRules *er ) ; ///< Sets the restriction enzyme display rules for this vector
    void getItemsAtPosition ( const int pos , wxArrayInt &vi , const bool limit = false ) const ; ///< Retrieves a list of items at that position (pos is 0-based)
    bool hasItemsAtPosition ( const int pos ) const ; ///< Returns wether there are items at that position (pos is 0-based)
    int countCodonTables () const ;
    wxString getCodonTableName ( const int x ) const ;
    void addRestrictionEnzyme ( TRestrictionEnzyme * const e ) ;
    void resetTurn () ; ///< Sets the turned variable to zero

    // Variables
    vector <TVectorItem> items ; ///< Items/features/annotations
    vector <TRestrictionCut> rc ; ///< Restriction enzyme cuts

    wxArrayTRestrictionEnzyme re ,  ///< Manually specified restriction enzymes
                              re2 ; ///< Automatically added restriction enzymes

    wxArrayString proteases , ///< Proteases used
                  cocktail ;  ///< Enzymes from the last restriction
    TUndo undo ; ///< Undo information

  private :
    wxString invert ( const wxString& s ) const ; ///< Inverts a string
    wxString vary_base ( const char b ) const ; ///< Turns a SIUPAC into a string of A, C, G, T
    void makeAA2DNA ( const wxString& mode = _T("") ) ; ///< "Translate" amino acid sequence into DNA; can be specified for an organism
    wxString mergeCodons ( wxString c1 , wxString c2 ) const ; ///< Used by makeAA2DNA for generating "abstract" (SIUPAC) DNA
    void setCodonTable ( const int table , const wxString& sequence , const wxString& name ) ; ///< Sets up the codon_tables variable
    void evaluate_key_value ( const wxString& key , const wxString& value ) ; ///< Used in setParam() and setParams()
    wxString get_translation_table ( const int translation_table ) const ;

    int type ; ///< The sequence type
    TEnzymeRules *enzyme_rules ; ///< Pointer to the restriction enzyme display rules
    bool recalcvisual ; ///< Recalculate the layout of the sequence?
    vector <TORF> worf ; ///< Open Reading Frames
    wxArrayInt methyl ; ///< Methylation sites
    wxArrayString hiddenEnzymes ; ///< Enzymes that are not shown
    wxString sequence ; ///< The sequence that all this fuss is about
    wxString _lu ;  ///< Left upper sticky end
    wxString _ll ; ///< Left lower sticky end
    wxString _ru ; ///< Right upper sticky end
    wxString _rl ; ///< Right lower sticky end
    wxString name ; ///< The name of the sequence
    wxString desc ; ///< The sequence description
    bool circular ; ///< Is this sequence circular?
    bool changed ; ///< Was this sequence changed?
    bool genomeMode ; ///< Is this a genome (using different display routines)?
    ChildBase *window ; ///< The window this TVector belongs to (might be NULL for most methods)
    int turned ; ///< A circular sequence can be turned; this is the number of bases (negative for "left")
    int action_value ; ///< Used by doAction()
    wxString database ; ///< The database this vector was loaded from
    wxString action ; ///< Used by doAction()
    wxString aa ; ///< Hell if I remember what this is for
    wxString AA2DNA[256] ; ///< AA-to-DNA "translation" table; used by makeAA2DNA()
    wxArrayString paramk ; ///< Parameter keys
    wxArrayString paramv ; ///< Parameter values

    static char IUPAC[256] ; ///< The ACTG chars in binary encoding (IUPAC_A, IUPAC_C, IUPAC_T, IUPAC_G)
    static char SIUPAC[256] ; ///< The extended IUPAC codes in binary encoding
    static char COMPLEMENT[256] ; ///< The complement to each SIUPAC base
    static char ACGT[256] ; ///< Different values for A, C, G, T; used in dna2aa()
    static vector <TAAProp> aaprop ; ///< The 20 amino acids and their properties
    static wxArrayString codon_tables ; ///< The codon tables for different organisms
    static wxArrayString codon_table_names ; ///< The names of these codon tables
    } ;

#endif

