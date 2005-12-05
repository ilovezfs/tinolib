/* $Header$
 *
 * Additionally file helpers
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.7  2005-12-05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.6  2005/08/02 04:44:41  tino
 * C++ changes
 *
 * Revision 1.5  2005/08/01 17:35:39  tino
 * tino_file_filenameptr
 *
 * Revision 1.4  2005/06/04 14:35:28  tino
 * now passes unit test
 *
 * Revision 1.3  2005/04/25 23:07:01  tino
 * some new routines
 *
 * Revision 1.2  2005/04/24 13:44:11  tino
 * tino_file_notdir
 *
 * Revision 1.1  2005/04/24 12:55:38  tino
 * started GAT support and filetool added
 */

#ifndef tino_INC_filetool_h
#define tino_INC_filetool_h

#include "fatal.h"
#include "alloc.h"
#include "str.h"

/* well, be prepared for windows
 */
#define	DRIVE_SEP_CHAR	0	/* untested	*/
#define	PATH_SEP_CHAR	'/'


/* Glue some path and name together.
 * If buffer is NULL it is allocated.
 *
 * This is intelligent:
 * If name is a full path, path is ignored.
 * Else only parts of path are taken for name.
 *
 * With windows it must become magic:
 * The drive prefix must be choosen from path and name.
 */
static char *
tino_file_glue_path(char *buf, size_t max, const char *path, const char *name)
{
#if DRIVE_SEP_CHAR
  int		drive;
#endif
  int		offset;
  size_t	len;

  if (!buf)
    {
      if (max<BUFSIZ)
	max	= BUFSIZ;
      buf	= (char *)tino_alloc(max);
    }

  offset	= 0;
#if DRIVE_SEP_CHAR
  drive	= 0;
  if (path && isalpha(*path) && path[1]==DRIVE_SEP_CHAR)
    {
      drive	= *path;
      path	+= 2;
    }
  if (name && isalpha(*name) && name[1]==DRIVE_SEP_CHAR)
    {
      drive	= *name;
      name	+= 2;
    }
  TINO_FATAL_IF(max<3);
  if (drive)
    {
      buf[0]	= drive;
      buf[1]	= DRIVE_SEP_CHAR;
      offset	= 2;
    }
#endif

  /* speedup if name is absolute
   */
  if (!name || name[0]!=PATH_SEP_CHAR)
    {
      buf[offset]	= 0;
      if (path)
        tino_strxcpy(buf+offset, path, max-offset);
      if (!name)
        return buf;

      len	= strlen(buf);
      if (len && buf[len-1]!=PATH_SEP_CHAR && len+1<max)
        buf[len++]	= PATH_SEP_CHAR;
      offset	= len;
    }
  tino_strxcpy(buf+offset, name, max-offset);
  return buf;
}

static int
tino_file_dirfileoffset(const char *buf, int file)
{
  int	i, offset;

  TINO_FATAL_IF(!buf);
  if (file)
    file	= 1;
  offset	= 0;
#if DRIVE_SEP_CHAR
  if (isalpha(*buf) && buf[1]==DRIVE_SEP_CHAR)
    offset	+= 2;
#endif
  for (i=offset; buf[i]; i++)
    if (buf[i]==PATH_SEP_CHAR)
      offset	= i+file;
  return offset;
}

static char *
tino_file_dirname(char *buf, size_t max, const char *name)
{
  int	offset;

  offset	= tino_file_dirfileoffset(name, 0);
  if (!buf)
    {
      if (max<BUFSIZ)
	max	= offset+1;
      buf	= (char *)tino_alloc(max);
    }
  if (max>offset)
    max	= offset+1;
  return tino_strxcpy(buf, name, max);
}

static char *
tino_file_filename(char *buf, size_t max, const char *name)
{
  int		offset;

  offset	= tino_file_dirfileoffset(name, 1);
  if (!buf)
    {
      if (max<BUFSIZ)
	max	= strlen(name+offset)+1;
      buf	= (char *)tino_alloc(max);
    }
  return tino_strxcpy(buf, name+offset, max);
}

static const char *
tino_file_filenameptr(const char *path)
{
  return path+tino_file_dirfileoffset(path, 1);
}

#ifdef TINO_TEST_UNIT
TESTCMP("/B", tino_file_glue_path(NULL, 0, "/A", "/B"));
TESTCMP("/B", tino_file_glue_path(NULL, 0, "A", "/B"));
TESTCMP("A/B", tino_file_glue_path(NULL, 0, "A", "B"));
TESTCMP("/A/B", tino_file_glue_path(NULL, 0, "/A", "B"));
TESTCMP("B", tino_file_glue_path(NULL, 0, NULL, "B"));
TESTCMP("A", tino_file_glue_path(NULL, 0, "A", NULL));
TESTCMP("", tino_file_glue_path(NULL, 0, NULL, NULL));
TESTCMP("", tino_file_dirname(NULL, 0, "/"));
TESTCMP("A", tino_file_dirname(NULL, 0, "A/B"));
TESTCMP("", tino_file_filename(NULL, 0, "A/"));
TESTCMP("B", tino_file_filename(NULL, 0, "A/B"));
#endif

#endif
