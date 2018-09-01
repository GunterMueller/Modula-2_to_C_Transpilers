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


/* Description of all options which can be used in Modula-2 translator
   command line.  The description is made up from two parts.
   Any parts (or both) may be absent in the description.
   First part contains suffixes of flags which are in the second part.
   Suffixes are separated by white space.  The second part starts with
   percents `%%' and contains any text in which description of flags are
   placed.  Any description of flag starts with character ``' followed by
   character `-' and finishes by character `''.  The flag suffix may be starts
   with white spaces.  It means that the corresponding flag has
   obligatory flag parameter. */

#define OPTIONS "string name name=def dir x letter output [p012] %%\
`-3' Support 3rd edition of Wirth's book (implicitly the 4th edition).\n\
`-strict' Prohibit language extensions.\n\
`-all' Compilation of all program modules with removing unused procedures.\n\
`-Bstring'\n\
`-c'\n\
`-C'  Only compilation of Modula-2 modules to C code.\n\
`-CM' Only output make dependence for each Modula-2 module\n\
      to standard output.\n\
`-CC' It is flag -C of cc.\n\
`-Dname=def'\n\
`-Dname'\n\
`-E'\n\
`-g'\n\
`-Idir'\n\
`-Ldir'\n\
`-lx'\n\
`-make'\n\
`-MAKE'\n\
`-ma'\n\
`-ms'\n\
`-M'\n\
`-o output'\n\
`-Oletter'\n\
`-p'\n\
`-pg'\n\
`-Pdir' Search of definition modules in this directory.\n\
`-R'\n\
`-S'\n\
`-test' Generation of dynamic tests.\n\
`-t[p012]'\n\
`-update' update of any modula module output.\n\
`-upper-case' only upper case keywords and standard identifiers\n\
              (-strict means -upper-case)\n\
`-Uname'\n\
`-v' Output of command lines of processes loaded.\n\
`-w'\n\
`-xletter'\n"

/* If IS_MODULA_OUTPUT_DELETED is TRUE than the output file is deleted
   by m2c in any case after its usage.  Otherwise the output file is
   deleted only on failure for its creation. */

#define IS_MODULA_OUTPUT_DELETED (!last_flag_place("-C")\
				  && !last_flag_place("-CM"))

/* If IS_MODULA_OUTPUT_TEMPORARY is TRUE than the unique temporary output file
   is created.  If IS_MODULA_OUTPUT_TEMPORARY is TRUE than
   IS_MODULA_OUTPUT_DELETED must be TRUE. */

#define IS_MODULA_OUTPUT_TEMPORARY (!last_flag_place("-C")\
				    && !last_flag_place("-CM"))

/* NONSTANDARD_MODULA_OUTPUT_NAME are accounted for when
   IS_MODULA_OUTPUT_TEMPORARY is FALSE.  If NONSTANDARD_MODULA_OUTPUT_NAME is
   not null string than the input module is compiled to the file with this
   name (not to the file with source name and MODULA_OUTPUT_SUFFIX).*/

#define NONSTANDARD_MODULA_OUTPUT_NAME NULL

/* MODULA_OUTPUT_SUFFIX is not accounted for only
   NONSTANDARD_MODULA_OUTPUT_NAME is not null string. */

#define MODULA_OUTPUT_SUFFIX ".c"


#ifndef C_COMPILER
#define C_COMPILER "pcc"
#endif

#define OBJECT_FILE_SUFFIX ".o"

/* Make flags must be not accounted for. */

#define C_COMPILATION_GUARD (!last_flag_place("-C") && !last_flag_place("-CM"))

#define C_INPUT_SUFFIXES {".c",".s"}

#define C_OUTPUT_EXISTS (!last_flag_place("-E") && !last_flag_place("-M"))


/* If IS_C_OUTPUT_DELETED is TRUE than the output file is deleted by m2c
   in any case after its usage (as C compiler does it).  Otherwise the C
   output file is deleted only on failure for its creation. */

#define IS_C_OUTPUT_DELETED (!last_flag_place("-make")\
			     && !last_flag_place("-update")\
                             && !last_flag_place("-c")\
			     && !last_flag_place("-S")\
                             && number_of_parameters()==1)

/* If NONSTANDARD_C_OUTPUT_NAME is not null string than the input file
   is compiled to the file with this name (not to the file with source name
   and C_OUTPUT_SUFFIX). */

#define NONSTANDARD_C_OUTPUT_NAME NULL

/* If MAY_BE_C_NONSTANDARD_OBJECT is TRUE than C compiler can generate
   object file with nonstandard name. */

#define MAY_BE_C_NONSTANDARD_OBJECT FALSE

/* Used when is MAY_BE_C_NONSTANDARD_OBJECT TRUE. It defines flag which
   determine nonstandard name object file. */

#define C_NONSTANDARD_OBJECT_FLAG NULL

/* Used when is MAY_BE_C_NONSTANDARD_OBJECT TRUE.
   If IS_C_NONSTANDARD_OBJECT_SEPARATE is TRUE then flag parameter (object
   file name) is separate. */

#define IS_C_NONSTANDARD_OBJECT_SEPARATE FALSE

#define C_OUTPUT_SUFFIX (last_flag_place("-S")?".s":OBJECT_FILE_SUFFIX)


/* It is also tested that C_COMPILATION_GUARD is TRUE.  Make flags must be
   not accounted for. */

#define LOAD_GUARD (!last_flag_place("-E") && !last_flag_place("-M")\
		    && !last_flag_place("-S") && !last_flag_place("-c"))


#define LOAD_OUTPUT_NAME (last_flag_place("-o")?flag_parameter("-o"):"a.out")


#define TRANSLATE_FLAG_TO_C(opt) (!strcmp(opt,"-3") || !strcmp(opt,"-strict")\
 || !strcmp(opt,"-all") || !strcmp(opt,"-C") || !strcmp(opt,"-CM")\
 || !strcmp(opt,"-l") || !strcmp(opt,"-L")\
 || !strcmp(opt,"-make") || !strcmp(opt,"-MAKE") || !strcmp(opt,"-o")\
 || !strcmp(opt,"-P")\
 || !strcmp(opt,"-test") || !strcmp(opt,"-update")\
 || !strcmp(opt,"-upper-case")\
                   ?NULL:(!strcmp(opt,"-CC")?"-C":opt))

#define C_ADDITIONAL_FLAG(n) (n==0 && LOAD_GUARD?"-c":NULL)

#define TRANSLATE_FLAG_TO_LOAD(opt) (!strcmp(opt,"-3")\
 || !strcmp(opt,"-strict")\
 || !strcmp(opt,"-all") || !strcmp(opt,"-B") || !strcmp(opt,"-c")\
 || !strcmp(opt,"-C") || !strcmp(opt,"-CM") || !strcmp(opt,"-CC")\
 || !strcmp(opt,"-D")\
 || !strcmp(opt,"-D") || !strcmp(opt,"-E") || !strcmp(opt,"-M")\
 || !strcmp(opt,"-I") || !strcmp(opt,"-make") || !strcmp(opt,"-MAKE")\
 || !strcmp(opt,"-ma") || !strcmp(opt,"-ms")\
 || !strcmp(opt,"-O") || !strcmp(opt,"-P") || !strcmp(opt,"-R")\
 || !strcmp(opt,"-S")\
 || !strcmp(opt,"-test") || !strcmp(opt,"-t") || !strcmp(opt,"-U")\
 || !strcmp(opt,"-update") || !strcmp(opt,"-w")\
 || !strcmp(opt,"-upper-case") || !strcmp(opt,"-x")\
                  ?NULL:opt)

#define LOAD_ADDITIONAL_FLAG(n) NULL

/* If IS_ORDER_IMPORTANT is TRUE than the flag is on the same place (relative
   to parameters) in all executed proccess command lines as in the source
   command line. */

#define IS_ORDER_IMPORTANT(opt) (!strcmp(opt,"-l") && !strcmp(opt,"-L"))

/* The following file is to be included here! */

#include "common_part.h"
