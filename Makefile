.SUFFIXES:
	%:: SCCS/s.%
	%:: RCS/%
	%:: RCS/%,v
	%:: %,v
	%:: s.%

BUILDDIR		?= build
CFG					?= default
INSTALL			?= install
NAME				?= slock2
VERSION			?= 1.0
SRCDIR			?= src
DOCSDIR			?= doc

Q						?= @

BINDIR			:= $(BUILDDIR)/$(CFG)
BIN					:= $(BINDIR)/$(NAME)
PWD					:= $(shell pwd)
SRC					:= $(sort $(subst $(PWD), ".", $(shell find $(SRCDIR)/ -name '*.cc')))
OBJ					:= $(SRC:$(SRCDIR)/%.cc=$(BINDIR)/%.o)
DEP					:= $(OBJ:%.o=%.d)

DUMMY				:= $(shell mkdir -p $(sort $(dir $(OBJ))))


.PHONY: all doxy clean cleanall install uninstall

all: $(BIN)

-include $(CFG).cfg

-include $(INSTALL).cfg

-include $(DEP)


ifeq ($(DEBUG), 1)
	CFLAGS	+= -g -DDEBUG
else
	CFLAGS	+= -O2 -DNDEBUG
endif

ifeq ($(VERBOSE), 1)
	CFLAGS	+= -v
endif


# paths
X11_INCLUDE		= /usr/X11R6/include
X11_PATH			= /usr/X11R6/lib

X11_CFLAGS 	:= -I${X11_INCLUDE}
X11_LDFLAGS := -L${X11_PATH} -lX11 -lXext

# flags
# On *BSD remove -DHAVE_SHADOW_H from CPPFLAGS and add -DHAVE_BSD_AUTH
COLOR_INACTIVE 	?= \"black\"			# normal
COLOR_ACTIVE 		?= \"\#005577\"		# typing
COLOR_ERROR			?= \"red\"
DEFS 		 = -DVERSION=\"${VERSION}\" \
					 -DCOLOR_INACTIVE=${COLOR_INACTIVE} \
					 -DCOLOR_ACTIVE=${COLOR_ACTIVE} \
					 -DCOLOR_ERROR=${COLOR_ERROR} \
					 -DEVENT_HANDLER=\"$(EVENT_HANDLER)\" \
					 -DHAVE_SHADOW_H #\
					 -DHAVE_BSD_AUTH

CFLAGS			+= ${X11_CFLAGS} ${DEFS} -Wall -W -pedantic
CXXFLAGS		+= ${CFLAGS} -std=c++11
# On OpenBSD and Darwin remove -lcrypt
LDFLAGS			+= ${X11_LDFLAGS} -L/usr/lib -lc -lcrypt


$(BIN): $(OBJ)
	@echo "===> LD $@"
	$(Q)$(CXX) $(CXXFLAGS) -o $(BIN) $^ $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cc
	@echo "===> $(CXX) $<   ->   $@"
	$(Q)$(CXX) $(CXXFLAGS) -MMD -c -o $@ $<

doxy:
	@echo "===> DOXYGEN"
	$(Q)doxygen

clean:
	@echo "===> CLEAN"
	$(Q)rm -fr $(BINDIR)/
	$(Q)rm -fr $(TESTBINDIR)

cleanall: clean
	@echo "===> CLEANALL"
	$(Q)rm -fr $(DOCSDIR)/


# Install mode. On BSD systems MODE=2755 and GROUP=auth
# On others MODE=4755 and GROUP=root
#MODE=2755
#GROUP=auth

install: all
	sudo cp "./build/$(CFG)/$(NAME)" "$(INSTALL_PATH)"
	sudo chmod a+x "$(INSTALL_PATH)"
	sudo chmod u+s "$(INSTALL_PATH)"
	sudo cp slock2-events.sh "$(EVENT_HANDLER)"
	sudo chmod a+x "$(EVENT_HANDLER)"


uninstall: unsupported

unsupported:
	@echo "Not yet implemented."
