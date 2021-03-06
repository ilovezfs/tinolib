/* If you include this, you know what you are doing:
 *
 * Compile time debugging:
 * DP(("format", args)) prints debug to stderr
 * xDP(("format", args)) prints only if global debugging is on
 *
 * #define TINO_DP_all TINO_DP_ON
 * to enable global debugging
 *
 * #define TINO_DP_main TINO_DP_OFF
 * to disable global debugging (even yours)
 *
 * #define TINO_DP_<module> TINO_DP_ON
 * to activate partial debugging in a certain module
 * (this overrides other defines)
 *
 * Copyright (C)2004-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef tino_INC_debug_h
#define tino_INC_debug_h

#include <stdio.h>
#include <stdarg.h>

#ifndef	DP
#define	DP	TINO_DP_main
#endif

#ifndef xDP
#define	xDP	TINO_DP_all
#endif

#ifndef TINO_DP_main
#define	TINO_DP_main	TINO_DP_ON
#endif

#ifndef TINO_DP_all
#define	TINO_DP_all	TINO_DP_OFF
#endif

#ifndef	TINO_DP_proc
#define TINO_DP_proc	TINO_DP_all
#endif

#ifndef	TINO_DP_sock
#define TINO_DP_sock	TINO_DP_all
#endif

#ifndef	TINO_DP_buf
#define TINO_DP_buf	TINO_DP_all
#endif

#ifndef	TINO_DP_block
#define TINO_DP_block	TINO_DP_all
#endif

#ifndef	TINO_DP_xml
#define TINO_DP_xml	TINO_DP_all
#endif

#ifndef	TINO_DP_data
#define TINO_DP_data	TINO_DP_all
#endif

#ifndef	TINO_DP_curl
#define TINO_DP_curl	TINO_DP_all
#endif

#ifndef TINO_DP_array
#define	TINO_DP_array	TINO_DP_all
#endif

#ifndef TINO_DP_str
#define	TINO_DP_str	TINO_DP_all
#endif

#ifndef TINO_DP_io
#define	TINO_DP_io	TINO_DP_all
#endif

/* This is an ellipsis, marking missing things
 * Define this to "0" to get a compile warning.
 */
#ifndef	TINO_XXX
#define	TINO_XXX
#endif

#if 1
#define TINO_DP_ON(X)	do { tino_debug_fn=__FUNCTION__; tino_debugprintf X; } while (0)
#else
#define TINO_DP_ON(X)	do { tino_debugprintf X; } while (0)
#endif
#define TINO_DP_OFF(X)	do { ; } while (0)

static const char	*tino_debug_fn;

static void
tino_debugprintf(const char *s, ...)
{
  va_list	list;

  fflush(stdout);
  fprintf(stderr, "[%s", (tino_debug_fn ? tino_debug_fn : "(unknown)"));
  va_start(list, s);
  vfprintf(stderr, s, list);
  va_end(list);
  fprintf(stderr, "]\n");
  fflush(stderr);
}

#endif
