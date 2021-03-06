/* Directory handling functions.
 *
 * Copyright (C)2005-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
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

#ifndef tino_INC_dir_h
#define tino_INC_dir_h

#include "err.h"
#include "slist.h"

#include <sys/types.h>
#include <dirent.h>

static int
tino_dir_read_imp(const char *name, TINO_SLIST *l)
{
  TINO_SLIST	list;
  DIR		*dir;
  struct dirent	*dp;

  xDP(("tino_dir_read(%s)", name));

  list	= tino_slist_new();
  *l	= list;
  if ((dir=opendir(name))==NULL)
    {
      tino_err("cannot open dir: %s", name);
      return 1;
    }
  while ((dp=readdir(dir))!=NULL)
    if (!(dp->d_name[0]=='.' &&
	  (dp->d_name[1]==0 ||
	   (dp->d_name[1]=='.' && dp->d_name[2]==0))))
      {
	xDP(("tino_dir_read() %s", dp->d_name));
	tino_slist_add(list, dp->d_name);
      }
  if (closedir(dir))
    {
      tino_err("error reading dir: %s", name);
      return 2;
    }
  return 0;
}

/* Read in a complete directory
 */
static TINO_SLIST
tino_dir_read(const char *name)
{
  TINO_SLIST	list;

  tino_dir_read_imp(name, &list);
  return list;
}

/* Read in a complete directory
 *
 * Return NULL on error
 */
static TINO_SLIST
tino_dir_read_err(const char *name)
{
  TINO_SLIST	list;

  if (tino_dir_read_imp(name, &list))
    {
      tino_slist_destroy(list);
      return 0;
    }
  return list;
}

#endif
