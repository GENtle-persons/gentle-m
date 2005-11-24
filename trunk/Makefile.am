SUFFIXES=.cpp .rc

bin_PROGRAMS = GENtle

EXTRA_DIST = mondrian.xpm \
variables.csv \
blank.db \
default.tab \
bitmaps/align.bmp \
bitmaps/cocktail.bmp \
bitmaps/display_features.bmp \
bitmaps/helpcs.bmp \
bitmaps/new.xpm \
bitmaps/plasmid_linear.bmp \
bitmaps/print.xpm \
bitmaps/settings_tool.bmp \
bitmaps/alignment.bmp \
bitmaps/copy.bmp \
bitmaps/display_restriction.bmp \
bitmaps/help.xpm \
bitmaps/open.bmp \
bitmaps/preview.bmp \
bitmaps/project.bmp \
bitmaps/splash.bmp \
bitmaps/align_tool.bmp \
bitmaps/copy.xpm \
bitmaps/draft.xbm \
bitmaps/image.bmp \
bitmaps/open.xpm \
bitmaps/preview.xpm \
bitmaps/protein.bmp \
bitmaps/tick.bmp \
bitmaps/bitmap1.bmp \
bitmaps/cut.bmp \
bitmaps/drawing.xbm\
bitmaps/mode_both.bmp \
bitmaps/orfs.bmp \
bitmaps/primer.bmp \
bitmaps/redo.bmp \
bitmaps/undo.bmp \
bitmaps/bitmap2.bmp \
bitmaps/cut.xpm       \
bitmaps/flowchar.xbm \
bitmaps/mode_edit.bmp \
bitmaps/paste.bmp \
bitmaps/primer_export.bmp \
bitmaps/save.bmp \
bitmaps/web.bmp \
bitmaps/calc.bmp \
bitmaps/Helix.bmp \
bitmaps/mode_view.bmp \
bitmaps/paste.xpm \
bitmaps/primer_import.bmp \
bitmaps/save.xpm \
bitmaps/write.xbm\
bitmaps/calc_tool.bmp \
bitmaps/dirplain.xbm \
bitmaps/help.bmp \
bitmaps/new.bmp \
bitmaps/plasmid_circular.bmp \
bitmaps/print.bmp \
bitmaps/scissors.bmp


GENtle_LDADD = $(WX_LIBS) -L/usr/lib -L/usr/local/lib -lsqlite -lsqlite3 -lmysqlclient
GENtle_LDFLAGS = -s
GENtle_SOURCES = TXMLfile.cpp\
TVectorTree.cpp\
TVirtualGel.cpp\
TVectorEditorItems.cpp\
TVectorEditorEnzymes.cpp\
TVectorEditor.cpp\
TVector.cpp\
TUndo.cpp\
TUReadSeq.cpp\
TStorage.cpp\
TSilmutDialog.cpp\
TRestrictionEditor.cpp\
TPrimer.cpp\
TPrimerDialog.cpp\
TPDB.cpp\
TLigationDialog.cpp\
TMainTree.cpp\
todo.cpp\
TItemEditDialog.cpp\
TImageDisplay.cpp\
TIMGreader.cpp\
TextImportDialog.cpp\
TClone.cpp\
SequenceTypes.cpp\
TCalculator.cpp\
SequenceTypeRestriction.cpp\
SequenceTypePlot.cpp\
SequenceTypeFeature.cpp\
SequenceTypeDNA.cpp\
SequenceTypeAlign.cpp\
SequenceTypeABI.cpp\
SequenceTypeAA.cpp\
SequenceCanvas.cpp\
SendHTTP.cpp\
RestrictionEnzymes.cpp\
ProgramOptionsDialog.cpp\
PlasmidCanvasLinear.cpp\
PrimerDesign.cpp\
PlasmidCanvasHandlers.cpp\
PlasmidCanvasCircular.cpp\
PlasmidCanvas.cpp\
MyFrame.cpp\
MyChild.cpp\
MiscDialogs.cpp\
ManageDatabase.cpp\
main.cpp\
GenBank.cpp\
FindSequenceDialog.cpp\
ExternalNCBI.cpp\
ExternalInterface.cpp\
ExternalBLAST.cpp\
ChildBase.cpp\
CoolCanvas.cpp\
AminoAcids.cpp\
Alignment.cpp\
ABIviewer.cpp\
ABItype.cpp\
TAlignmentDialog.cpp\
AutoAnnotate.cpp\
TGraph.cpp\
TGraphDisplay.cpp\
TGraphScale.cpp\
CGview.cpp\
ureadseq.c\
ncoils.cpp\
TPhylip.cpp\
TPhyloTree.cpp\
PCR_Troubleshoot.cpp\
TProteolysis.cpp\
tinyxml/tinyxmlparser.cpp\
tinyxml/tinyxmlerror.cpp\
tinyxml/tinyxml.cpp\
tinyxml/tinystr.cpp\
clustalw/alnscore.cpp\
clustalw/amenu.cpp\
clustalw/calcgap.cpp\
clustalw/calcprf1.cpp\
clustalw/calcprf2.cpp\
clustalw/calctree.cpp\
clustalw/clustalw.cpp\
clustalw/gcgcheck.cpp\
clustalw/interfac.cpp\
clustalw/malign.cpp\
clustalw/palign.cpp\
clustalw/prfalign.cpp\
clustalw/random.cpp\
clustalw/readmat.cpp\
clustalw/sequence.cpp\
clustalw/showpair.cpp\
clustalw/trees.cpp\
clustalw/util.cpp\
ipc/element.cpp\
ipc/gp_out.cpp\
ipc/ipc.cpp\
ipc/pars.cpp\
enums.h \
lin_sqlite.h \
lin_sqlite3.h \
TXMLfile.h\
AutoAnnotate.h\
CGview.h\
TAlignmentDialog.h\
TGraph.h\
TVectorTree.h\
TVirtualGel.h\
TVectorEditor.h\
TVector.h\
TUndo.h\
TUReadSeq.h\
TStorage.h\
TSilmutDialog.h\
TRestrictionEditor.h\
TPrimer.h\
TPrimerDialog.h\
TPDB.h\
TLigationDialog.h\
TMainTree.h\
TItemEditDialog.h\
TImageDisplay.h\
TIMGreader.h\
TextImportDialog.h\
TClone.h\
TCalculator.h\
SequenceCanvas.h\
SendHTTP.h\
RestrictionEnzymes.h\
ProgramOptionsDialog.h\
PrimerDesign.h\
PlasmidCanvas.h\
MyFrame.h\
MyChild.h\
MiscDialogs.h\
ManageDatabase.h\
main.h\
GenBank.h\
FindSequenceDialog.h\
ExternalInterface.h\
TProteolysis.h\
PCR_Troubleshoot.h\
ChildBase.h\
CoolCanvas.h\
AminoAcids.h\
Alignment.h\
ABIviewer.h\
ABItype.h\
ureadseq.h\
TPhylip.h\
TPhyloTree.h\
ncoils.h\
tinyxml/tinyxml.h\
tinyxml/tinystr.h\
clustalw/clustalw.h\
clustalw/dayhoff.h \
clustalw/general.h \
clustalw/matrices.h \
clustalw/param.h \
clustalw/xmenu.h\
ipc/element.h\
ipc/global.h\
ipc/gp_out.h\
ipc/ipc.h\
ipc/parse.h
