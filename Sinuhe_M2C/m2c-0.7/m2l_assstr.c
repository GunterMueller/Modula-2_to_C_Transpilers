/* Run-time function for Modula-2 string assignment.
 * Copyright 1993, 1994, 1995, 1996, 1997 Vladimir Makarov
 * This file is part of m2c.
 *
 * m2c is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * m2c is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with m2c. If not, see <http://www.gnu.org/licenses/>.
 */

/* This function is used only for the 3rd edition of Wirth's book. */

#include "m2lib.h"

void
m2_assstr (char *var, char *str, unsigned int vlong, unsigned int strlong)
{
  while (vlong--, strlong-- != 0)
    *var++ = (*str++);
  while (vlong-- != 0)
    *var++ = '\0';
}
