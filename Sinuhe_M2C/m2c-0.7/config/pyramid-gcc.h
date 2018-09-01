/* Syntax analyzer of Modula-2 translator.
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

#include "pyramid.h"
#include "gcc.h"


/* If the following macros is defined than it is used for making more
   precise size of type with mode TYPE_MODE (see commentaries in m2-icode.h)
   after standard method of size calculation. */

/* Alignment of record and union is always greater or equal 4 bytes.
   Remember that Modula-2 record case part is implemented by C union.  The case
   when several fields are in case variant will be accounted for here. */

#define MAKING_MORE_PRECISE_SIZE_BY_TYPE_MODE(size,type_mode) \
  ((type_mode) == ICNM_RECORD || (type_mode) == ICNM_RECORD_CASE_PART\
   ? ((size) % 4 == 0 ? (size) : (size) + (4 - (size) % 4)) : (size))

/* If the following macros is defined than it is used for making more
   precise size of type given as ICODE_PTR (see commentaries in m2-icode.h)
   after standard method of size calculation and after call of
   previous macros (if it is defined). */

/*#define MAKING_MORE_PRECISE_SIZE_BY_TYPE(size,icode_ptr) */

/* If the following macros is defined than it is used for making more
   precise alignment of type with mode TYPE_MODE (see commentaries in
   m2-icode.h) after standard method of alignment calculation. */

/* Alignment of record and union is always greater or equal 4 bytes.
   Remember that Modula-2 record case part is implemented by C union.  The case
   when several fields are in case variant will be accounted for here. */

#define MAKING_MORE_PRECISE_ALIGN_BY_TYPE_MODE(align,type_mode) \
  ((type_mode) == ICNM_RECORD || (type_mode) == ICNM_RECORD_CASE_PART\
   ? 4 : (align))

/* If the following macros is defined than it is used for making more
   precise alignment of type given as ICODE_PTR (see commentaries in
   m2-icode.h) after standard method of alignment calculation and after call of
   previous macros (if it is defined). */

/*#define MAKING_MORE_PRECISE_ALIGN_BY_TYPE(align,icode_ptr) */
