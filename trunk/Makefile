#
# File:		Makefile for samples
# Author:	Robert Roebling
# Created:	1999
# Updated:	
# Copyright:	(c) 1998 Robert Roebling
#
# This makefile requires a Unix version of wxWindows
# to be installed on your system. This is most often
# done typing "make install" when using the complete
# sources of wxWindows or by installing the two
# RPM packages wxGTK.XXX.rpm and wxGTK-devel.XXX.rpm
# under Linux.

# this makefile may also be used as a template for simple makefiles for your
# own programs, the comments explain which parts of it you may have to modify
# to use it for your own programs


# you may change WX_CONFIG value to use a specific wx-config and to pass it
# some options, for example "--inplace" to use an uninstalled wxWindows
# version
#
# by default, wx-config from the PATH is used
WX_CONFIG := wx-config

# set this to the name of the main executable file
PROGRAM = GENtle

# if your program has more than one source file, add more .o files to the line
# below
OBJECTS = ABItype.o \
	ABIviewer.o \
	Alignment.o \
	AminoAcids.o \
	ChildBase.o \
	CoolCanvas.o \
	ExternalInterface.o \
	ExternalNCBI.o \
	ExternalBLAST.o \
	FindSequenceDialog.o \
	GenBank.o \
	main.o \
	ManageDatabase.o \
	MiscDialogs.o \
	MyChild.o \
	MyFrame.o \
	PlasmidCanvasCircular.o \
	PlasmidCanvas.o \
	PlasmidCanvasHandlers.o \
	PlasmidCanvasLinear.o \
	PrimerDesign.o \
	ProgramOptionsDialog.o \
	RestrictionEnzymes.o \
	SendHTTP.o \
	SequenceCanvas.o \
	SequenceTypeAA.o \
	SequenceTypeABI.o \
	SequenceTypeAlign.o \
	SequenceTypeDNA.o \
	SequenceTypeFeature.o \
	SequenceTypePlot.o \
	SequenceTypeRestriction.o \
	SequenceTypes.o \
	TCalculator.o \
	TClone.o \
	TextImportDialog.o \
	TImageDisplay.o \
	TIMGreader.o \
	TItemEditDialog.o \
	TLigationDialog.o \
	TMainTree.o \
	todo.o \
	TPDB.o \
	TPrimer.o \
	TPrimerDialog.o \
	TRestrictionEditor.o \
	TSilmutDialog.o \
	TStorage.o \
	TUndo.o \
	TUReadSeq.o \
	TVector.o \
	TVectorEditor.o \
	TVectorEditorEnzymes.o \
	TVectorEditorItems.o \
	TVectorTree.o \
	TVirtualGel.o \
	TXMLfile.o \
	ureadseq.o \
	tinyxml/tinystr.o \
	tinyxml/tinyxml.o \
	tinyxml/tinyxmlparser.o \
	tinyxml/tinyxmlerror.o \
	clustalw/alnscore.o \
	clustalw/amenu.o \
	clustalw/calcgap.o \
	clustalw/calcprf1.o \
	clustalw/calcprf2.o \
	clustalw/calctree.o \
	clustalw/clustalw.o \
	clustalw/gcgcheck.o \
	clustalw/interfac.o \
	clustalw/malign.o \
	clustalw/palign.o \
	clustalw/prfalign.o \
	clustalw/random.o \
	clustalw/readmat.o \
	clustalw/sequence.o \
	clustalw/showpair.o \
	clustalw/trees.o \
	clustalw/util.o



# you shouldn't have to edit anything below this line
CXX = $(shell $(WX_CONFIG) --cxx)

.SUFFIXES:	.o .cpp
		.o .c

.cpp.o :
	$(CXX) -c `$(WX_CONFIG) --cxxflags` -o $@ $<

.c.o :
	$(CXX) -c `$(WX_CONFIG) --cxxflags` -o $@ $<

all:    $(PROGRAM)

$(PROGRAM):	$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) `$(WX_CONFIG) --libs` -L/usr/lib -lsqlite -lmysqlclient

clean: 
	rm -f *.o $(PROGRAM)
	rm -f tinyxml/*.o
	rm -f clustalw/*.o
