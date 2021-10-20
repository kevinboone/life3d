NAME    := life3d
VERSION := 1.0a
CC      :=  g++
LIBS    := ${EXTRA_LIBS} 
TARGET	:= $(NAME)
SOURCES := $(shell find src/ -type f -name *.cpp)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.cpp=.o))
DEPS	:= $(OBJECTS:.o=.deps)
DESTDIR := /
PREFIX  := /usr
MANDIR  := $(DESTDIR)/$(PREFIX)/share/man
BINDIR  := $(DESTDIR)/$(PREFIX)/bin
SHARE   := $(DESTDIR)/$(PREFIX)/share/$(TARGET)
CFLAGS  := -O3 -fpie -fpic -Wall -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -DSHARE=\"$(SHARE)\" -DPREFIX=\"$(PREFIX)\" -I include ${EXTRA_CFLAGS}
LDFLAGS := -pie ${EXTRA_LDFLAGS}

all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) 

build/%.o: src/%.cpp
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET) 

install: $(TARGET)
	mkdir -p $(DESTDIR)/$(PREFIX) $(DESTDIR)/$(BINDIR) $(DESTDIR)/$(MANDIR)
	strip $(TARGET)
	install -m 755 $(TARGET) $(DESTDIR)/${BINDIR}

-include $(DEPS)

.PHONY: clean

