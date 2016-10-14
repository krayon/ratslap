/* vim:set ts=4 sw=4 tw=80 et cindent ai si cino=(0,ml,\:0:
 * ( settings from: http://datapax.com.au/code_conventions/ )
 */

/**********************************************************************
    RatSlap
    Copyright (C) 2014-2016 Todd Harbour

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 ONLY, as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program, in the file COPYING or COPYING.txt; if
    not, see http://www.gnu.org/licenses/ , or write to:
      The Free Software Foundation, Inc.,
      51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 **********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "log.h"

#define max_src_len "16"

FILE *_logfile = NULL;

/*
LOG LINE FORMAT:

20140815T231826+1000 [E] demoprog.c:00135 Failed to allocate memory for stuff

*/

int log_init(void) {
    _logfile = stderr;

    return 0;
}

int log_end(void) {
    _logfile = NULL;

    return 0;
}

inline void std_output(FILE *strm, const char *srcfile, const int line
, const char *func, const char *head, const char *text, ...) {
    static char _logout[4096];
    static char _logtime[32];

    char *st; // start of string
    char *nl; // new line ptr
    va_list ap;
    time_t t;
    struct tm *tmp;

    if (!strm) return;

    t = time(NULL);
    tmp = localtime(&t);
    if (
       tmp == NULL
    || strftime(_logtime, sizeof(_logtime), "%0Y%0m%0dT%0H%0M%0S%z", tmp) == 0
    ) {
        //                     "20140815T231613+1000"
        snprintf(_logtime, 32, "===== UNKNOWN  =====");
    }

    va_start(ap, text);
        vsprintf(_logout, text, ap);
    va_end(ap);

    st = _logout;
    nl = strchr(_logout, '\n');
    while(nl) {
        *nl = '\0';
        fprintf(strm, "%s %s %"max_src_len"s:%05d:%-15s %s\n", _logtime, head
            , srcfile, line, func, st);
        st = nl + 1;
        nl = strchr(st, '\n');
    }

    if(*st) {
        fprintf(strm, "%s %s %"max_src_len"s:%05d:%-15s %s",   _logtime, head
            , srcfile, line, func, st);
    }
}
