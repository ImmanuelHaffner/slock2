.SUFFIXES:
	%:: SCCS/s.%
	%:: RCS/%
	%:: RCS/%,v
	%:: %,v
	%:: s.%

BUILDDIR		?= build
CFG					?= default
NAME				?= slock
VERSION			?= 1.2
SRCDIR			?= src
DOCSDIR			?= doc
LLVM_CONFIG ?= llvm-config

Q						?= @

BINDIR			:= $(BUILDDIR)/$(CFG)
BIN					:= $(BINDIR)/$(NAME)
PWD					:= $(shell pwd)
SRC					:= $(sort $(subst $(PWD), ".", $(shell find $(SRCDIR)/ -name '*.c')))
OBJ					:= $(SRC:$(SRCDIR)/%.c=$(BINDIR)/%.o)
DEP					:= $(OBJ:%.o=%.d)

DUMMY				:= $(shell mkdir -p $(sort $(dir $(OBJ))))


.PHONY: all doxy clean cleanall install uninstall

all: $(BIN)

-include $(CFG).cfg

-include $(DEP)


ifeq ($(DEBUG), 1)
	CFLAGS	+= -g -DDEBUG
else
	CFLAGS	?= -O2 -DNDEBUG
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
COLOR1 	?= black			# normal
COLOR2 	?= \#005577		# typing
DEFS 		= -DVERSION=\"${VERSION}\" -DHAVE_SHADOW_H -DCOLOR1="\"${COLOR1}\"" -DCOLOR2="\"${COLOR2}\""

CFLAGS			+= ${X11_CFLAGS} ${DEFS} -std=c99 -Wall -W -pedantic
# On OpenBSD and Darwin remove -lcrypt
LDFLAGS			+= ${X11_LDFLAGS} -L/usr/lib -lc -lcrypt


$(BIN): $(OBJ)
	@echo "===> LD $@"
	$(Q)$(CC) $(CFLAGS) -o $(BIN) $^ $(LDFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	@echo "===> $(CC) $<   ->   $@"
	$(Q)$(CC) $(CFLAGS) -MMD -c -o $@ $<

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
