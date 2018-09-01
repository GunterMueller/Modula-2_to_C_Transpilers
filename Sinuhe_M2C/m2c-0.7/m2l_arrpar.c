/* Run-time function for Modula-2 value array parameter allocation.
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

#include <stdio.h>
#include "m2lib.h"

void
m2_arrpar (unsigned char **adr, unsigned int high, unsigned int elong)
{
  register unsigned char *var, *val;
  register unsigned int valong;

  val = (*adr);
  valong = (high + 1) * elong;
  var = (*adr) = malloc (valong);
  if (var == NULL)
    {
      fputs ("\n\r no memory for array parameter", stderr);
      m2_halt ();
    }
  while (valong-- != 0)
    *var++ = (*val++);
}
