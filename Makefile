TARGET=openvcl

SRCDIR=src
OBJDIR=obj
DEPENDFILE=.depend
TMP=/tmp
PACKAGE=openvcl$(VCLVERSION)

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
HEADERS=$(wildcard $(SRCDIR)/*.h)
OBJECTS=$(subst $(SRCDIR)/,$(OBJDIR)/,$(patsubst %.cpp,%.o,$(SOURCES)))

CXXFLAGS:=$(CXXFLAGS) -ansi -pedantic -Wall -Werror -g
LDFLAGS:=$(LDFLAGS) -g

.PHONY: all examples clean distclean package install

ifneq (,$(findstring install,$(MAKECMDGOALS)))
ifeq (,$(PREFIX))
ifneq (,$(PS2DEV))
$(warning Selecting $(PS2DEV) as installation root. ($$PS2DEV))
PREFIX=$(PS2DEV)
else
$(error Could not figure out installation path. Please set PREFIX to the root or setup your PS2DEV path.)
endif
else
$(warning Installation root is set to $(PREFIX).)
endif
endif

all:	$(OBJDIR) $(TARGET) examples

$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

examples:
	$(MAKE) -C examples all

clean:
	$(MAKE) -C examples clean
	-rm -f $(TARGET) $(OBJECTS)

distclean: clean
	-$(MAKE) -C examples clean
	-$(MAKE) -C contrib/masp distclean
	-rm -f $(DEPENDFILE) src/*~ *~
	-rmdir $(OBJDIR)

package: distclean
	rm -rf "$(TMP)/$(PACKAGE)"
	mkdir "$(TMP)/$(PACKAGE)"
	cp * "$(TMP)/$(PACKAGE)" -R
	find "$(TMP)/$(PACKAGE)" -type d | while read f ; do chmod 755 "$$f" ; done
	find "$(TMP)/$(PACKAGE)" -type f | while read f ; do chmod 644 "$$f" ; done
	(cd "$(TMP)"; tar cvzf "$(TMP)/$(PACKAGE).tar.gz" "$(PACKAGE)")

install: $(OBJDIR) $(TARGET)
	-install -d $(PREFIX)/bin/
	install -s $(TARGET) $(PREFIX)/bin/
 
# compile c file
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir $(OBJDIR)

# automatic dependency updates
$(DEPENDFILE): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -MM $(SOURCES) | sed "s/\([^:]*\):/$(subst /,\/,$(OBJDIR))\/\1:/" > $(DEPENDFILE)
-include $(DEPENDFILE)
