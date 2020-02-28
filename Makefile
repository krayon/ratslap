#/* vim:set ts=4 sw=4 tw=80 cindent ai si cino=(0,ml,\:0:
# * ( settings from: http://datapax.com.au/code_conventions/ )
# */
#
#/**********************************************************************
#    RatSlap
#    Copyright (C) 2014-2016 Todd Harbour
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    version 2 ONLY, as published by the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program, in the file COPYING or COPYING.txt; if
#    not, see http://www.gnu.org/licenses/ , or write to:
#      The Free Software Foundation, Inc.,
#      51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# **********************************************************************/

# Include BUILD options
OPTIONS_FILE   = make.options.conf
-include $(OPTIONS_FILE)
BUILDOPTS      = $(foreach BO, $(OPTIONS), -D$(BO))

# Krayon's GPG code signing key
GPG_KEY        = 81ECF212

# Programs
CC             = gcc
LINK           = gcc -o
CTAGS          = ctags
MARKDOWN_GEN   = $(shell which markdown_py)
ARCHIVER       = tar -zcvf
ARCHIVE_EXT    = tar.gz

# Tune for current CPU (march implies mtune)
CARCH_FLAG     = -march=native

# Other CPU components
# -m3dnow
# -mmmx
# -msse
# -msse2
# -msse3
# -m3dnow
# -m32
# -m64
CPU_FLAG       =

# Other compile options
#   Optimizations
#     -O2
#   Use pipes, not temp files
#     -pipe
#   Require ansi compatibility
#     -ansi
#   Strict ISO C warnings
#     -pedantic
#   Warnings (all)
#     -Wall
#   Warnings should be considered errors
#     -Werror
#   Debug information (spec for gdb)
#     -ggdb
OPT_FLAGS      = -O2 -pipe -Wall -Werror -ggdb

INCDIR         =

LIBDIR         =

# Packages we depend on (these will be pkg-config'd)
PKGS           = libusb-1.0

LIBS           = $(shell pkg-config --libs $(PKGS))

CFLAGS        += $(CARCH_FLAG) $(CPU_FLAG) $(OPT_FLAGS) $(BUILDOPTS) $(shell pkg-config --cflags $(PKGS)) -DDEBUG -DINFO

# Markdown parser flags
MD_FLAGS      += -x abbr -x def_list -x footnotes -x tables -x toc -o html4



# Application name
APPNAME        = $(shell sed -n 's/^[ \t]*\#define[ \t]*APP_NAME[ \t]*"\([^"]*\)".*$$/\1/p'    app.h)
BINNAME        = $(shell sed -n 's/^[ \t]*\#define[ \t]*BIN_NAME[ \t]*"\([^"]*\)".*$$/\1/p'    app.h)

# Retrieve version from git
#     This is something like:
#         1.10.5-3-g6ab5527-dirty-ft-cool
#     Where:
#         1.10.5                          == tag / major version
#               -3                        == 3 commits ahead of tag
#                 -g6ab5527               == 'g'it commit starting with 6ab5527
#                          -dirty         == some files here changed or not in repository
#                                -ft-cool == ft-cool branch
MAJVER = $(shell bash -c \
	'\
	  echo -n "$$(git describe --always --tags --match="*" --dirty)" \
	')

APPBRANCH = $(shell bash -c \
	'\
	  git name-rev --always --name-only --no-undefined HEAD|sed "s@feature/@ft-@;s@hotfix/\(.*\)@\1.HF@;s@release/\(.*\)@\1.PRE@"\
	')
#APPVER="$(shell grep _APP_VERSION app.h|head -1|cut -d'"' -f2)"
APPVER = $(shell bash -c \
	'\
	  echo -n "$(MAJVER)"; \
	  [ ! -z "$(APPBRANCH)" ] && [ "$(APPBRANCH)" != "master" ] && [ "$(APPBRANCH)" != "$(MAJVER)" ] && echo -n "-$(APPBRANCH)" \
	')

BUILD_DATE     = $(shell date +'%Y-%m-%d %H:%M:%S%z')
BUILD_MONTH    = $(shell date +'%B')
BUILD_YEAR     = $(shell date +'%Y')
BUILD_COMMIT   = $(shell git rev-parse --verify 'HEAD^{commit}')

ARCHIVE_NAME   = $(BINNAME)-$(APPVER)
ARCHIVE_FILE   = $(ARCHIVE_NAME).$(ARCHIVE_EXT)

# Default binary(s) to build
PROGS          = $(BINNAME)

# Files to distribute
DIST_FILES     = $(PROGS) $(PROGS:=.asc) LICENSE README.creole Changelog

# Object files to build
OBJS           = log.o main.o

# Documents (markdown files)
MD_FILES       = $(wildcard *.md)
MD_OBJS        = $(patsubst %.md, %.html, ${MD_FILES})



# all (DEFAULT)
.PHONY: all
all: tags Changelog manpage.1 $(if $(strip $(MARKDOWN_GEN)),$(MD_OBJS),) $(OPTIONS_FILE) $(PROGS) done

# Error
.PHONY: err
err:
	@echo "No target specified (try dist)"

# Done
.PHONY: done
done:
	@echo "BUILD COMPLETE: $(APPNAME) ($(BINNAME)) v$(APPVER)"

# git update
.PHONY: gitup
gitup:
	@echo "$(APPNAME)"
	@# Ensure up to date
	@echo "Pulling from origin..."
	@git pull || true

# Sign
.PHONY: sign
sign: $(PROGS:=.asc)

# Tags
.PHONY: ctags
ctags:
	@# Generate CTags
	@echo "Generating tags file..."
	@$(CTAGS) -R --fields=+lS . || echo "No ctags found, skipping tags file..."
.PHONY: tags
tags: ctags

# Clean up
.PHONY: clean
clean:
	@echo "Cleaning up..."
	
	@for f in $(OBJS); do \
		echo "  deleting: $$f"; \
		rm -f $$f; \
	done
	
	@echo "  deleting: Changelog";
	@rm -f Changelog;
	
	@echo "  deleting: manpage.1";
	@rm -f manpage.1;
	
	@for f in $(MD_OBJS); do \
		echo "  deleting: $$f"; \
		rm -f $$f; \
	done
	
	@echo "  deleting: tags";
	@rm -f tags;
	
	@echo "  deleting: git.h";
	@rm -f git.h;
	
	@echo "  deleting: log.h";
	@rm -f log.h;
	
	@if diff $(OPTIONS_FILE).DEFAULT $(OPTIONS_FILE) >/dev/null; then \
		echo "  deleting: $(OPTIONS_FILE)"; \
		rm -Rf "$(OPTIONS_FILE)"; \
	else \
		echo "   keeping: $(OPTIONS_FILE) (MODIFIED)"; \
	fi
	
	@if [ -d "$(ARCHIVE_NAME)" ]; then \
		echo "  deleting: $(ARCHIVE_NAME)"; \
		rm -Rf "$(ARCHIVE_NAME)"; \
	fi
	
	@echo "Done."

# Clean up - pre-distribution
.PHONY: distclean
distclean: clean
	@echo "Cleaning (for distribution)..."
	
	@for f in $(ARCHIVE_FILE).asc $(ARCHIVE_FILE) $(PROGS:=.asc) $(PROGS); do \
		if [ -f "$${f}" ]; then; \
			echo "  deleting: $${f}"; \
			rm "$${f}"; \
		fi; \
	done



Changelog: gitup
	@# Generate Changelog
	@echo "Generating Changelog..."
	@git log --color=never --pretty=tformat:"%ai %an <%aE>%n%w(76,4,4)%h %s%n%+b" >Changelog

git.h: gitup git.h.TEMPLATE
	@# Generating GIT header
	@echo "Generating git header file..."
	@cat git.h.TEMPLATE >git.h
	@sed -i 's#//SOURCE//#// WARNING // Auto-generated file, DO NOT MODIFY //#' git.h
	@sed -i 's#\%\%APP_VERSION\%\%#$(APPVER)#'                                  git.h
	@sed -i 's#\%\%BUILD_DATE\%\%#$(BUILD_DATE)#'                               git.h
	@sed -i 's#\%\%BUILD_COMMIT\%\%#$(BUILD_COMMIT)#'                           git.h

log.h: log.h.TEMPLATE
	@# Generating log header
	@echo "Generating log header file..."
	@cat log.h.TEMPLATE >log.h
	@for o in $(OPTIONS:DEBUG%=LOG%); do \
	    sed -i 's/\(#define '$${o}' *\)NULL.*$$/\1_logfile/' log.h; \
	done

manpage.1: manpage.1.TEMPLATE
	@# Generating manpage
	@echo "Generating man page file..."
	@cat manpage.1.TEMPLATE >manpage.1
	@sed -i 's#\%\%APP_VERSION\%\%#$(APPVER)#'                                  manpage.1
	@sed -i 's#\%\%BUILD_MONTH\%\%#$(BUILD_MONTH)#'                             manpage.1
	@sed -i 's#\%\%BUILD_YEAR\%\%#$(BUILD_YEAR)#'                               manpage.1

$(OPTIONS_FILE): $(OPTIONS_FILE).DEFAULT
	@cp $(OPTIONS_FILE).DEFAULT $(OPTIONS_FILE)

%.o: %.c
	$(CC) $(CFLAGS) $(INCDIR) -c "$<" -o "$@"

%.html: %.md markdown.TEMPLATE.html
	@# Generating HTML
	@sed '/^/,/^%%%%%BODY%%%%%/{/^%%%%%BODY%%%%%/,$$d}' <markdown.TEMPLATE.html  >"$@"
	@TITLE="$(shell sed -n 's/^# \([^#]*\) #$$/\1/p;q' <"$<")"; \
		echo "Generating $$TITLE ($<)"; \
		sed -i 's/%%%%%TITLE%%%%%/'"$$TITLE"'/'             "$@"
	$(MARKDOWN_GEN) $(MD_FLAGS) "$<"                                            >>"$@"
	@echo                                                                       >>"$@"
	@sed '1,/^%%%%%BODY%%%%%/d'                         <markdown.TEMPLATE.html >>"$@"

%.asc: %
	@echo "Signing: $${f}..."
	@rm "$@" 2>/dev/null || true
	gpg -o $@ --local-user $(GPG_KEY) --armor --detach-sign $<

$(BINNAME): gitup git.h log.h $(OBJS)
	@echo "Linking $(BINNAME)..."
	
	$(LINK) "$(BINNAME)" $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS)

$(ARCHIVE_FILE): $(DIST_FILES)
	@echo "Making $(ARCHIVE_FILE)..."
	
	@if [ -d "$(ARCHIVE_NAME)" ]; then \
		echo "Directory '$(ARCHIVE_NAME)' exists"; \
		exit 1; \
	fi
	
	@if [ -f "$(ARCHIVE_FILE)" ]; then \
		echo "Archive '$(ARCHIVE_FILE)' exists"; \
		exit 2; \
	fi
	
	@mkdir "$(ARCHIVE_NAME)"
	
	@cp -a $(DIST_FILES) "$(ARCHIVE_NAME)/"
	@$(ARCHIVER) "$(ARCHIVE_FILE)" "$(ARCHIVE_NAME)/"

# Build distribution bundle
dist: $(ARCHIVE_FILE) $(ARCHIVE_FILE).asc

