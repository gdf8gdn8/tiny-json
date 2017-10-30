.SUFFIXES:
.SUFFIXES: .c .o

VPATH = ./obj
CC=gcc 
CFLAGS=-Wall
OBJDIR = obj
OUTPUT_OPTION = -o $(OBJDIR)/$@

OBJECTS := $(notdir $(patsubst %.c,%.o,$(wildcard *.c)))

TARGET = jsonparser_test
all: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(addprefix $(OBJDIR)/,$(OBJECTS))

$(OBJDIR):
	@mkdir -p $@

.PHONY: clean
clean:
	rm -fr $(addprefix $(OBJDIR)/,$(Objects)) $(TARGET) *.o $(OBJDIR) a,out

run:  $(TARGET)
	./$(TARGET)