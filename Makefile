CC := g++
CCFLAGS := -std=c++11

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

$(BINDIR)/pivionics_main: $(SRCDIR)/main.cpp $(OBJDIR)/window.o $(OBJDIR)/element.o $(OBJDIR)/stringsplit.o
	$(CC) $(CCFLAGS) $^ -o $@

$(BINDIR)/simple_edit: $(SRCDIR)/simple_edit.cpp $(OBJDIR)/element.o $(OBJDIR)/window.o $(OBJDIR)/stringsplit.o
	$(CC) $(CCFLAGS) $^ -o $@

clean:
	rm obj/*
	rm bin/*
