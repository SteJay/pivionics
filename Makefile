# Our compiler. The default is g++, the gcc for c++
CC := g++

# Our archiver (used to create lib/libcore.a)
AR := ar
ARFLAGS := rcs

# The following is used to link against the necessary SDL2 libraries:
SDLFLAGS := -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_image -lSDL2_net
# And these are used to link against lib/libcore.a and posix threading:
CURSESFLAGS := -lncurses
COREFLAGS := -lcore -pthread


# The main options to set for our compiler
# First the standard stuff - we're using C++11 and we need to include our lib 
# directory...
CCFLAGS := -std=c++11 -L./lib

# Compiler optimisation - use -O2 for production, or -g -O0 for debugging
#CCFLAGS += -O2
CCFLAGS += -g -O0

# I often find colourised debugging info useful; I also sometimes have more
# than one screen of it to read at once. Using the following option I can pipe
# the output of the build process through less and the colour won't be wiped:
CCFLAGS += -fdiagnostics-color=always

# Compile time options
CCOPTS := 
# My system has its byte order reversed as opposed to what SDL2 expects.
# Setting the following will compile with the correct byte order in this case:
CCOPTS += -DREVERSE_COLOR_ORDER

# Remove this setting to save space; otherwise a copy of the GPL will be
# physically included in the editor and the features to retrieve it will be
# activated. This is so that binaries can be created which can be ditributed
# without COPYING/LICENSE and still contain the license:
CCOPTS+= -DGPL_LICENSE

# The following are the directories we're reading from and writing to during
# the build process; unlikely to change.
BINDIR := bin
SRCDIR := src
OBJDIR := obj
LIBDIR := lib

# The Main Targets:
all: $(BINDIR)/pivedit $(BINDIR)/pivcon $(BINDIR)/pivrend

# My lil' socket wrapper...
$(OBJDIR)/sjsock.o: $(SRCDIR)/sjsock.cpp $(SRCDIR)/sjsock.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -lpthread -o $@

# The Object Files:
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

$(OBJDIR)/irregular.o: $(SRCDIR)/irregular.cpp $(SRCDIR)/irregular.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/box.o: $(SRCDIR)/box.cpp $(SRCDIR)/box.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/compositor.o: $(SRCDIR)/compositor.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/renderer.o: $(SRCDIR)/renderer.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/command.o: $(SRCDIR)/command.cpp $(SRCDIR)/command.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/console.o: $(SRCDIR)/console.cpp $(SRCDIR)/console.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< -o $@

$(OBJDIR)/text.o: $(SRCDIR)/text.cpp $(SRCDIR)/text.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< $(SDLFLAGS) -o $@

$(OBJDIR)/sdlrenderer.o: $(SRCDIR)/sdlrenderer.cpp $(SRCDIR)/sdlrenderer.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) $(CCOPTS) -c $< $(SDLFLAGS) -o $@

# Our Core Library:
$(LIBDIR)/libcore.a: $(OBJDIR)/command.o $(OBJDIR)/compositor.o $(OBJDIR)/renderer.o $(OBJDIR)/sdlrenderer.o $(OBJDIR)/element.o $(OBJDIR)/window.o $(OBJDIR)/container.o $(OBJDIR)/circle.o $(OBJDIR)/box.o $(OBJDIR)/irregular.o $(OBJDIR)/text.o $(OBJDIR)/stringsplit.o $(OBJDIR)/sjsock.o 
	$(AR) $(ARFLAGS) $@ $^

# The Editor Binary:
$(BINDIR)/pivedit: $(SRCDIR)/editor.cpp $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(SDLFLAGS) $(COREFLAGS) -o $@

# The Console Binary:
$(BINDIR)/pivcon: $(SRCDIR)/pivcon.cpp $(OBJDIR)/console.o $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(OBJDIR)/console.o $(CURSESFLAGS) $(SDLFLAGS) $(COREFLAGS) -o $@

# Here we are....
$(BINDIR)/pivrend: $(SRCDIR)/pivrend.cpp $(LIBDIR)/libcore.a
	$(CC) $(CCFLAGS) $(CCOPTS) $< $(SDLFLAGS) $(COREFLAGS) -o $@


# Remove a previous build:
clean:
	rm -f obj/*
	rm -f lib/*
	rm -f bin/*
