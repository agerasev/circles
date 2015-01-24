include ../build-info.mk

C = gcc
CXX = g++
LINKFLAGS = $(_DESKTOP_LIBS)

OBJDIR = obj

CFLAGS = -I./source $(_CFLAGS) $(_DESKTOP_CFLAGS)
CXXFLAGS = -I./source $(_CXXFLAGS) $(_DESKTOP_CXXFLAGS)

HEADERS = $(_HEADERS)
SOURCES = $(_DESKTOP_SOURCES) $(_SOURCES)
OBJ = $(SOURCES:%=$(OBJDIR)/%.o)
BIN = mynativeapp

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LINKFLAGS) $^ -o $@

$(OBJDIR)/%.cpp.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$(OBJDIR)/%.c.o: %.c $(HEADERS)
	$(C) $(CFLAGS) -c $< -o $@
