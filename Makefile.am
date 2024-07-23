SUBDIRS = src tests

doxygen:	docs/code/html/index.html

docs/code/html/index.html: src/*.cpp src/*.h
	doxygen

.PHONY: doxygen
