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

# Programs
CC             = gcc
LINK           = gcc -o
CTAGS          = ctags
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
#   Debug information (spec for gdb)
#     -ggdb
OPT_FLAGS      = -O2 -pipe -Wall -ggdb

INCDIR         =

LIBDIR         =

# Packages we depend on (these will be pkg-config'd)
PKGS           = libusb-1.0

LIBS           = $(shell pkg-config --libs $(PKGS))

CFLAGS        += $(CARCH_FLAG) $(CPU_FLAG) $(OPT_FLAGS) $(shell pkg-config --cflags $(PKGS)) -DDEBUG -DINFO



# Application name
APPNAME        = $(shell sed -n 's/^[ \t]*\#define[ \t]*APP_NAME[ \t]*"\([^"]*\)".*$$/\1/p'    app.h)
BINNAME        = $(shell sed -n 's/^[ \t]*\#define[ \t]*BIN_NAME[ \t]*"\([^"]*\)".*$$/\1/p'    app.h)

# Retrieve version
#     This is something like:
#         1.10.5-3-g6ab5527-dirty
#     Where:
#         1.10.5                  == tag
#               -3                == 3 commits ahead of tag
#                 -g6ab5527       == commit starting with g6ab5527
#                          -dirty == some files are not in the repository
#APPVER="$(shell grep _APP_VERSION app.h|head -1|cut -d'"' -f2)"
APPVER = $(shell bash -c \
    '\
        git describe          --tags --match="*" --dirty &>/dev/null \
    &&  git describe --always --tags --match="*" --dirty             \
    ||  (echo -n "0.0.1-"; git describe --always --tags --dirty)     \
    ')

BUILD_DATE     = $(shell date +'%Y-%m-%d %H:%M:%S%z')

ARCHIVE_NAME   = $(BINNAME)-$(APPVER)

# Default binary(s) to build
PROGS          = $(BINNAME)

# Files to distribute
DIST_FILES     = $(PROGS) LICENSE README.creole Changelog

# Object files to build
OBJS           = main.o

# Default target
.PHONY: _PHONY
_PHONY: all



all: tags Changelog $(PROGS) done



gitup:
	@echo "$(APPNAME)"
	@# Ensure up to date
	@echo "Pulling from origin..."
	@git pull || true

Changelog: gitup
	@# Generate Changelog
	@echo "Generating Changelog..."
	@#git log --pretty=tformat:"%C(auto)[%ci] %h %s" >Changelog
	@git log >Changelog

git.h: gitup git.h.TEMPLATE
	@# Generating GIT header
	@echo "Generating git header file..."
	@cat git.h.TEMPLATE >git.h
	@sed -i 's#//SOURCE//#// WARNING // Auto-generated file, DO NOT MODIFY //#' git.h
	@sed -i 's#\%\%APP_VERSION\%\%#$(APPVER)#'                                  git.h
	@sed -i 's#\%\%BUILD_DATE\%\%#$(BUILD_DATE)#'                               git.h



# Error
err:
	@echo "No target specified (try dist)"

# Done
done:
	@echo "BUILD COMPLETE: $(APPNAME) ($(BINNAME)) v$(APPVER)"

# Tags
ctags:
	@# Generate CTags
	@echo "Generating tags file..."
	@$(CTAGS) *
tags: ctags

.c.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $*.o

$(BINNAME): git.h $(OBJS)
	@echo "Linking $(BINNAME)..."
	
	$(LINK) "$(BINNAME)" $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS)

dist: gitup $(DIST_FILES)
	@echo "Making $(ARCHIVE_NAME).$(ARCHIVE_EXT)..."

	@if [ -d "$(ARCHIVE_NAME)" ]; then \
		echo "Directory '$(ARCHIVE_NAME)' exists"; \
		exit 1; \
	fi

	@if [ -f "$(ARCHIVE_NAME).$(ARCHIVE_EXT)" ]; then \
		echo "Archive '$(ARCHIVE_NAME).$(ARCHIVE_EXT)' exists"; \
		exit 2; \
	fi

	@mkdir "$(ARCHIVE_NAME)"
	
	@cp -a $(DIST_FILES) "$(ARCHIVE_NAME)/"
	@$(ARCHIVER) "$(ARCHIVE_NAME).$(ARCHIVE_EXT)" "$(ARCHIVE_NAME)/"

clean:
	@echo "Cleaning up..."
	
	@for f in $(OBJS); do \
		echo "  deleting: $$f"; \
		rm -f $$f; \
	done
	
	@echo "  deleting: Changelog";
	@rm -f Changelog;
	
	@echo "  deleting: tags";
	@rm -f tags;
	
	@echo "  deleting: git.h";
	@rm -f git.h;
	
	@if [ -d "$(ARCHIVE_NAME)" ]; then \
		echo "  deleting: $(ARCHIVE_NAME)"; \
		rm -Rf "$(ARCHIVE_NAME)"; \
	fi
	
	@echo "Done."

distclean: clean
	@echo "Cleaning (for distribution)..."
	
	@if [ -f "$(ARCHIVE_NAME).$(ARCHIVE_EXT)" ]; then \
		echo "  deleting: $(ARCHIVE_NAME).$(ARCHIVE_EXT)"; \
		rm "$(ARCHIVE_NAME).$(ARCHIVE_EXT)"; \
	fi
	
	@for f in $(PROGS); do \
		[ -f "$${f}" ] && echo "  deleting: $$f" && rm "$${f}" || true; \
	done
