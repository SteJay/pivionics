# Our compiler. The default is g++, the gcc for c++
CC := g++
# The main options to set for our compiler
CCFLAGS := -std=c++11 -fdiagnostics-color=always -L./lib
# Compilation options
CCOPTS := -DENABLE_RENDER_AA_ALL
# Our archiver (used to create libraries)
AR := ar

ARFLAGS := rcs
SDLFLAGS := -lSDL2 -lSDL2_gfx
BINDIR := bin
SRCDIR := src
OBJDIR := obj
LIBDIR := lib

COREFLAGS := -lcore -pthread

all: $(BINDIR)/pivionics_main $(BINDIR)/simple_edit

$(OBJDIR)/stringsplit.o: $(SRCDIR)/stringsplit.cpp $(SRCDIR)/stringsplit.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/window.o: $(SRCDIR)/window.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/element.o: $(SRCDIR)/element.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/circle.o: $(SRCDIR)/circle.cpp $(SRCDIR)/circle.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/display.o: $(SRCDIR)/display.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/renderer.o: $(SRCDIR)/renderer.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/sdlrenderer.o: $(SRCDIR)/sdlrenderer.cpp $(SRCDIR)/sdlrenderer.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/compositor.o: $(SRCDIR)/compositor.cpp $(SRCDIR)/compositor.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< $(SDLFLAGS) -o $@

$(LIBDIR)/libcore.a: $(OBJDIR)/stringsplit.o $(OBJDIR)/window.o $(OBJDIR)/element.o $(OBJDIR)/circle.o $(OBJDIR)/display.o $(OBJDIR)/renderer.o $(OBJDIR)/compositor.o $(OBJDIR)/sdlrenderer.o
	$(AR) $(ARFLAGS) $@ $^

$(BINDIR)/pivionics_main: $(SRCDIR)/main.cpp $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(SDLFLAGS) $(COREFLAGS) -o $@

$(BINDIR)/simple_edit: $(SRCDIR)/simple_edit.cpp $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(COREFLAGS) -o $@

clean:
	rm -f obj/*
	rm -f lib/*
	rm -f bin/*
