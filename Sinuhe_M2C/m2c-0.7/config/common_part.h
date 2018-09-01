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


/* Suffixes of implementation (main) module file and definition module file
   must be different.  Suffixes of implementation module and main module files
   can be the same. */

#define DEFINITION_MODULE_SUFFIX ".def"
#define IMPLEMENTATION_MODULE_SUFFIX ".mod"
#define MAIN_MODULE_SUFFIX ".mod"

#define MODULA_INPUT_SUFFIXES {DEFINITION_MODULE_SUFFIX,\
			       IMPLEMENTATION_MODULE_SUFFIX,\
			       MAIN_MODULE_SUFFIX}


/* The following utility is needed for the Modula-2 translator work with
   some C compilers. */

#ifndef MV
#define MV "mv"
#endif

/* The following directory is used for storing Modula-2 run time library and 
   definition, implementation standard modules and its object files. */

#ifndef STANDARD_LIBRARY_DIRECTORY
#define STANDARD_LIBRARY_DIRECTORY       "/usr/local/lib/m2lib/"
#endif

/* The following directory is used by `m2c' for storing temporary files. */

#ifndef TEMPORARY_DIRECTORY
#define TEMPORARY_DIRECTORY "/tmp/"
#endif

#define ENVIRONMENT_LIBRARY_PATHS_VARIABLE   "m2paths"

/* The name of Modula-2 run time library.  It is in
   STANDARD_LIBRARY_DIRECTORY. */

#ifndef M2_RUN_TIME_LIBRARY
#define M2_RUN_TIME_LIBRARY "m2lib.a" 
#endif

#define FALSE 0
#define TRUE 1


/* Maximum and minimum values of Modula-2 basic types. */

#define SHORTCARD_MAX USHRT_MAX
#define CARDINAL_MAX UINT_MAX
#define LONGCARD_MAX ULLONG_MAX

#define SHORTINT_MIN SHRT_MIN
#define INTEGER_MIN  INT_MIN
#define LONGINT_MIN LLONG_MIN

#define SHORTINT_MAX SHRT_MAX
#define INTEGER_MAX INT_MAX
#define LONGINT_MAX LLONG_MAX

#define SHORTREAL_MAX FLT_MAX	/* float */
#define REAL_MAX FLT_MAX	/* float */
#define LONGREAL_MAX DBL_MAX	/* double */
#define SHORTREAL_MIN (-SHORTREAL_MAX)	/* float */
#define REAL_MIN (-REAL_MAX)	/* float */
#define LONGREAL_MIN (-LONGREAL_MAX)	/* double */

/* Maximum cardinal value of Modula-2 basic type CHAR. */

#define CHARACTER_MAX UCHAR_MAX

#define BITS_IN_CHARACTER CHAR_BIT

/* Number of bits in a Modula-2 set.  Modula-2 set is always implemented in C 
   by unsigned int. */

#define BITS_IN_SET (int)(CHAR_BIT*sizeof(unsigned int))		/*unsigned int*/

/* Byte is synonym of char. */

#define BYTE_SIZE sizeof(char)

#define SET_SIZE  sizeof(unsigned int)

/* Internal translator representation of Modula-2 constant value. */

typedef unsigned long long Tcard;
typedef long long Tint;
typedef double Treal;
typedef unsigned int Tset;
