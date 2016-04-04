CC := g++
CCFLAGS := -std=c++11
SDLFLAGS := -lSDL2 -lSDL2_gfx
BINDIR := bin
SRCDIR := src
OBJDIR := obj



all: $(BINDIR)/pivionics_main $(BINDIR)/simple_edit

$(OBJDIR)/stringsplit.o: $(SRCDIR)/stringsplit.cpp $(SRCDIR)/stringsplit.h
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJDIR)/window.o: $(SRCDIR)/window.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJDIR)/element.o: $(SRCDIR)/element.cpp $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) -c $< -o $@

$(OBJDIR)/circle.o: $(SRCDIR)/circle.cpp $(SRCDIR)/circle.h $(SRCDIR)/core_elements.h
	$(CC) $(CCFLAGS) -c $< -o $@

$(BINDIR)/pivionics_main: $(SRCDIR)/main.cpp $(OBJDIR)/window.o $(OBJDIR)/element.o $(OBJDIR)/circle.o $(OBJDIR)/stringsplit.o
	$(CC) $(CCFLAGS) $(SDLFLAGS) $^ -o $@

$(BINDIR)/simple_edit: $(SRCDIR)/simple_edit.cpp $(OBJDIR)/element.o $(OBJDIR)/window.o $(OBJDIR)/circle.o $(OBJDIR)/stringsplit.o
	$(CC) $(CCFLAGS) $^ -o $@

clean:
	rm obj/*
	rm bin/*
