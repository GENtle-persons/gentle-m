# Name des Testprogramms
check_PROGRAMS = my_tests

# Quelldateien für Testprogramm
my_tests_SOURCES = ../src/main.cpp main_test.cpp TXMLfile_test.cpp

common_OBJECTS =  \
	../src/GENtle-MyFrame.o \
	../src/GENtle-TVectorEditorItems.o \
	../src/GENtle-ProgramOptionsDialog.o \
	../src/GENtle-TXMLfile.o \
	../src/GENtle-TGraphDisplay.o \
	../src/GENtle-AutoAnnotate.o \
	../src/GENtle-TPhylip.o \
	../src/GENtle-RestrictionEnzymes.o \
	../src/GENtle-SequenceTypeRestriction.o \
	../src/GENtle-TProteolysis.o \
	../src/GENtle-CloningAssistant.o \
	../src/GENtle-SequenceTypeAA.o \
	../src/GENtle-ABIviewer.o \
	../src/GENtle-AlignmentAppearanceDialog.o \
	../src/GENtle-TMainTree.o \
	../src/GENtle-SequenceTypes.o \
	../src/GENtle-FindSequenceDialog.o \
	../src/GENtle-TDotPlot.o \
	../src/GENtle-TStorage.o \
	../src/GENtle-TIMGreader.o \
	../src/GENtle-Alignment.o \
	../src/GENtle-TVectorEditorEnzymes.o \
	../src/GENtle-SendHTTP.o \
	../src/GENtle-TSequencingAssistantDialog.o \
	../src/GENtle-TRestrictionIdentifier.o \
	../src/GENtle-PlasmidCanvasLinear.o \
	../src/GENtle-todo.o \
	../src/GENtle-TLigationDialog.o \
	../src/GENtle-TVectorEditor.o \
	../src/GENtle-TGraph.o \
	../src/GENtle-TImageDisplay.o \
	../src/GENtle-PlasmidCanvas.o \
	../src/GENtle-TClone.o \
	../src/GENtle-TUndo.o \
	../src/GENtle-util.o \
	../src/GENtle-ABItype.o \
	../src/GENtle-ncoils.o \
	../src/GENtle-CGview.o \
	../src/GENtle-ExternalInterface.o \
	../src/GENtle-TUReadSeq.o \
	../src/GENtle-TRestrictionEditor.o \
	../src/GENtle-PrimerDesign.o \
	../src/GENtle-OnlineTools.o \
	../src/GENtle-MiscDialogs.o \
	../src/GENtle-SequenceTypeABI.o \
	../src/GENtle-TStoreAllDialog.o \
	../src/GENtle-AminoAcids.o \
	../src/GENtle-CoolCanvas.o \
	../src/GENtle-TVector.o \
	../src/GENtle-TEliteLaChromLogDialog.o \
	../src/GENtle-SequenceTypeFeature.o \
	../src/GENtle-ExternalBLAST.o \
	../src/GENtle-SequenceCanvas.o \
	../src/GENtle-SequenceTypePlot.o \
	../src/GENtle-ExternalNCBI.o \
	../src/GENtle-TPhyloTree.o \
	../src/GENtle-GenBank.o \
	../src/GENtle-SequenceTypeDNA.o \
	../src/GENtle-TGraphScale.o \
	../src/GENtle-PlasmidCanvasHandlers.o \
	../src/GENtle-MyChild.o \
	../src/GENtle-ureadseq.o \
	../src/ipc/GENtle-element.o \
	../src/ipc/GENtle-pars.o \
	../src/ipc/GENtle-gp_out.o \
	../src/ipc/GENtle-ipc.o \
	../src/GENtle-CloneManager.o \
	../src/GENtle-SequenceTypeAlign.o \
	../src/GENtle-TPDB.o \
	../src/GENtle-TPrimerDialog.o \
	../src/GENtle-SCFtype.o \
	../src/GENtle-TCalculator.o \
	../src/GENtle-TVectorTree.o \
	../src/GENtle-TItemEditDialog.o \
	../src/GENtle-TAlignmentDialog.o \
	../src/GENtle-TextImportDialog.o \
	../src/GENtle-TPrimer.o \
	../src/GENtle-ChildBase.o \
	../src/GENtle-PCR_Troubleshoot.o \
	../src/GENtle-ManageDatabase.o \
	../src/GENtle-SequenceTypeAAstructure.o \
	../src/GENtle-PlasmidCanvasCircular.o \
	../src/GENtle-TVirtualGel.o \
	../src/GENtle-TSilmutDialog.o

#common_objects= \
#  ../src/TXMLfile.o \
#  ../src/TVector.o
#	../src/GENtle-main.o 
#  ../src/util.o

# ../src/TVectorItemEditor.o

# Google Test Bibliotheken
my_tests_CXXFLAGS = $(CXXFLAGS) $(GTEST_INCLUDE) -DTESTING
my_tests_CPPFLAGS = $(CPPFLAGS) $(GTEST_INCLUDE) -I../src -DTESTING
my_tests_LDADD = $(common_OBJECTS) $(WX_LIBS) $(LDFLAGS) -L/usr/lib -lsqlite3 $(GTEST_LIBS)  -lgtest_main
my_tests_LDFLAGS = $(LDFLAGS) -pthread

# Durchführen der Tests automatisch, wenn `make check` aufgerufen wird
TESTS = $(check_PROGRAMS)
