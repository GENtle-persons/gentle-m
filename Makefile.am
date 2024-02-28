SUFFIXES=.cpp .rc

bin_PROGRAMS = GENtle
OBJEXT=o

EXTRA_DIST = mondrian.xpm \
variables.csv \
blank.db \
default.tab \
marker.txt \
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

GENtle_LDADD = $(WX_LIBS) $(LDFLAGS) -L/usr/lib -lsqlite3 -ltinyxml
GENtle_LDFLAGS = $(LDFLAGS)
GENtle_SOURCES = $(common_sources) main.cpp

common_sources = \
ABItype.cpp \
ABItype.h \
ABIviewer.cpp \
ABIviewer.h \
Alignment.cpp \
Alignment.h \
AlignmentAppearanceDialog.cpp \
AlignmentAppearanceDialog.h \
AminoAcids.cpp \
AminoAcids.h \
AutoAnnotate.cpp \
AutoAnnotate.h \
CGview.cpp \
CGview.h \
ChildBase.cpp \
ChildBase.h \
CloneManager.cpp \
CloneManager.h \
CloningAssistant.cpp \
CloningAssistant.h \
CoolCanvas.cpp \
CoolCanvas.h \
ExternalBLAST.cpp \
ExternalInterface.cpp \
ExternalInterface.h \
ExternalNCBI.cpp \
FindSequenceDialog.cpp \
FindSequenceDialog.h \
GenBank.cpp \
GenBank.h \
ManageDatabase.cpp \
ManageDatabase.h \
MiscDialogs.cpp \
MiscDialogs.h \
MyChild.cpp \
MyChild.h \
MyFrame.cpp \
MyFrame.h \
OnlineTools.cpp \
OnlineTools.h \
PCR_Troubleshoot.cpp \
PCR_Troubleshoot.h \
PlasmidCanvas.cpp \
PlasmidCanvas.h \
PlasmidCanvasCircular.cpp \
PlasmidCanvasHandlers.cpp \
PlasmidCanvasLinear.cpp \
PrimerDesign.cpp \
PrimerDesign.h \
ProgramOptionsDialog.cpp \
ProgramOptionsDialog.h \
RestrictionEnzymes.cpp \
RestrictionEnzymes.h \
SCFtype.cpp \
SCFtype.h \
SendHTTP.cpp \
SendHTTP.h \
SequenceCanvas.cpp \
SequenceCanvas.h \
SequenceTypeAA.cpp \
SequenceTypeAAstructure.cpp \
SequenceTypeABI.cpp \
SequenceTypeAlign.cpp \
SequenceTypeDNA.cpp \
SequenceTypeFeature.cpp \
SequenceTypePlot.cpp \
SequenceTypeRestriction.cpp \
SequenceTypes.cpp \
TAlignmentDialog.cpp \
TAlignmentDialog.h \
TCalculator.cpp \
TCalculator.h \
TClone.cpp \
TClone.h \
TDotPlot.cpp \
TDotPlot.h \
TEliteLaChromLogDialog.cpp \
TEliteLaChromLogDialog.h \
TGraph.cpp \
TGraph.h \
TGraphDisplay.cpp \
TGraphScale.cpp \
TIMGreader.cpp \
TIMGreader.h \
TImageDisplay.cpp \
TImageDisplay.h \
TItemEditDialog.cpp \
TItemEditDialog.h \
TLigationDialog.cpp \
TLigationDialog.h \
TMainTree.cpp \
TMainTree.h \
TPDB.cpp \
TPDB.h \
TPhylip.cpp \
TPhylip.h \
TPhyloTree.cpp \
TPhyloTree.h \
TPrimer.cpp \
TPrimer.h \
TPrimerDialog.cpp \
TPrimerDialog.h \
TProteolysis.cpp \
TProteolysis.h \
TRestrictionEditor.cpp \
TRestrictionEditor.h \
TRestrictionIdentifier.cpp \
TRestrictionIdentifier.h \
TSequencingAssistantDialog.cpp \
TSequencingAssistantDialog.h \
TSilmutDialog.cpp \
TSilmutDialog.h \
TStorage.cpp \
TStorage.h \
TStoreAllDialog.cpp \
TStoreAllDialog.h \
TUReadSeq.cpp \
TUReadSeq.h \
TUndo.cpp \
TUndo.h \
TVector.cpp \
TVector.h \
TVectorEditor.cpp \
TVectorEditor.h \
TVectorEditorEnzymes.cpp \
TVectorEditorItems.cpp \
TVectorTree.cpp \
TVectorTree.h \
TVirtualGel.cpp \
TVirtualGel.h \
TXMLfile.cpp \
TXMLfile.h \
TextImportDialog.cpp \
TextImportDialog.h \
enums.h  \
ipc/element.cpp \
ipc/gp_out.cpp \
ipc/ipc.cpp \
ipc/pars.cpp \
lin_sqlite.h \
lin_sqlite3.h \
main.h \
ncoils.cpp \
ncoils.h \
todo.cpp \
ureadseq.c \
ureadseq.h \
util.cpp \
ipc/element.h \
ipc/global.h \
ipc/gp_out.h \
ipc/ipc.h \
ipc/parse.h

tinyxml_sources = \
tinyxml/tinyxmlparser.cpp \
tinyxml/tinyxmlerror.cpp \
tinyxml/tinyxml.cpp \
tinyxml/tinystr.cpp \
tinyxml/tinyxml.h \
tinyxml/tinystr.h

CLUSTALW_sources = \
clustalw/alnscore.cpp \
clustalw/amenu.cpp \
clustalw/calcgap.cpp \
clustalw/calcprf1.cpp \
clustalw/calcprf2.cpp \
clustalw/calctree.cpp \
clustalw/clustalw.cpp \
clustalw/gcgcheck.cpp \
clustalw/interfac.cpp \
clustalw/malign.cpp \
clustalw/palign.cpp \
clustalw/prfalign.cpp \
clustalw/random.cpp \
clustalw/readmat.cpp \
clustalw/sequence.cpp \
clustalw/showpair.cpp \
clustalw/trees.cpp \
clustalw/util.cpp \
clustalw/clustalw.h \
clustalw/dayhoff.h \
clustalw/general.h \
clustalw/matrices.h \
clustalw/param.h \
clustalw/xmenu.h

ICON=GENtle.icns
BUNDLE=GENtle.app
macos_bundle: $(BUNDLE)/Contents/MacOS/$(bin_PROGRAMS)
$(BUNDLE)/Contents/MacOS/$(bin_PROGRAMS) $(BUNDLE)/Contents/Resources/$(ICON) $(BUNDLE)/Contents/Info.plist: $(bin_PROGRAMS) Info.plist $(ICON)
	mkdir -p "$(BUNDLE)/Contents/MacOS"
	echo -n 'APPL????' > "$(BUNDLE)/Contents/PkgInfo"
	cp "$(bin_PROGRAMS)" "$(BUNDLE)/Contents/MacOS/"
	cp mondrian.xpm codon_catalog.csv variables.csv blank.db default.tab marker.txt "$(BUNDLE)/Contents/MacOS/"
	cp -r bitmaps help "$(BUNDLE)/Contents/MacOS/"
	cp Info.plist "$(BUNDLE)/Contents/"
	mkdir -p "$(BUNDLE)/Contents/Resources"
	cp "$(ICON)" "$(BUNDLE)/Contents/Resources/"
	dylibbundler -od -b -x "$(BUNDLE)/Contents/MacOS/$(bin_PROGRAMS)" -d "$(BUNDLE)/Contents/libs"

.PHONY: bundle
