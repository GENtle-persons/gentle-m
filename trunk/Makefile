# Generated automatically from Makefile.in by configure.
#
# File:		makefile.unx
# Author:	Julian Smart
# Created:	1998
# Updated:	
# Copyright:	(c) 1998 Julian Smart
#
# "%W% %G%"
#
# Makefile for mdi example (UNIX).

top_srcdir = ../..
top_builddir = ../..
program_dir = samples/GENtle

PROGRAM=main

MYOBJECTS = ABItype.o \
	ABIviewer.o \
	Alignment.o \
	AminoAcids.o \
	GenBank.o \
	ManageDatabase.o \
	MiscDialogs.o \
	MyChild.o \
	MyFrame.o \
	PlasmidCanvas.o \
	PlasmidCanvasLinear.o \
	PlasmidCanvasCircular.o  \
	PlasmidCanvasHandlers.o \
	PrimerDesign.o \
	RestrictionEnzymes.o \
	SequenceCanvas.o \
	SequenceTypes.o \
	TCalculator.o \
	TClone.o \
	TextImportDialog.o \
	TImageDisplay.o \
	TIMGreader.o \
	TItemEditDialog.o \
	TLigationDialog.o \
	TMainTree.o \
	TPrimer.o \
	TPrimerDialog.o \
	TRestrictionEditor.o \
	TStorage.o \
	TVector.o \
	TVectorEditor.o \
	TVectorTree.o

MYDEFS = $(MYOBJECTS:.o=.d)

OBJECTS =$(PROGRAM).o $(MYOBJECTS)
DEPFILES=$(PROGRAM).d $(MYDEFS)
LIBRARIES=-L/home/magnus/sqlite/bld/.libs -lsqlite
# LDLIBS=

include ../../src/makeprog.env

-include $(DEPFILES)
