# Our compiler. The default is g++, the gcc for c++
CC := g++
# The main options to set for our compiler
CCFLAGS := -std=c++11 -fdiagnostics-color=always -L./lib
# Compilation options
CCOPTS := -DENABLE_RENDER_AA_ALL -DREVERSE_COLOR_ORDER
# Our archiver (used to create libraries)
AR := ar

ARFLAGS := rcs
SDLFLAGS := -lSDL2 -lSDL2_gfx
BINDIR := bin
SRCDIR := src
OBJDIR := obj
LIBDIR := lib

COREFLAGS := -lcore -pthread

all: $(BINDIR)/pivionics $(BINDIR)/pivedit

.optional: $(BINDIR)/simple_edit

$(OBJDIR)/stringsplit.o: $(SRCDIR)/stringsplit.cpp $(SRCDIR)/stringsplit.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/window.o: $(SRCDIR)/window.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/element.o: $(SRCDIR)/element.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/container.o: $(SRCDIR)/container.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/circle.o: $(SRCDIR)/circle.cpp $(SRCDIR)/circle.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/compositor.o: $(SRCDIR)/compositor.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/renderer.o: $(SRCDIR)/renderer.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/sdlrenderer.o: $(SRCDIR)/sdlrenderer.cpp $(SRCDIR)/sdlrenderer.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< $(SDLFLAGS) -o $@

$(OBJDIR)/sdlcompositor.o: $(SRCDIR)/sdlcompositor.cpp $(SRCDIR)/sdlcompositor.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< $(SDLFLAGS) -o $@

$(LIBDIR)/libcore.a: $(OBJDIR)/stringsplit.o $(OBJDIR)/container.o $(OBJDIR)/circle.o $(OBJDIR)/compositor.o $(OBJDIR)/renderer.o $(OBJDIR)/sdlcompositor.o $(OBJDIR)/sdlrenderer.o $(OBJDIR)/window.o $(OBJDIR)/element.o
	$(AR) $(ARFLAGS) $@ $^

$(BINDIR)/pivionics: $(SRCDIR)/main.cpp $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(SDLFLAGS) $(COREFLAGS) -o $@

#$(BINDIR)/simple_edit: $(SRCDIR)/simple_edit.cpp $(LIBDIR)/libcore.a
#	$(CC) $(CCFLAGS) $(CCOPTS) $< $(COREFLAGS) -o $@

$(BINDIR)/pivedit: $(SRCDIR)/editor.cpp $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(SDLFLAGS) $(COREFLAGS) -o $@

clean:
	rm -f obj/*
	rm -f lib/*
	rm -f bin/*
