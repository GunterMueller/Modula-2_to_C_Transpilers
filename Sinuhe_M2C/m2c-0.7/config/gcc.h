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

#define OPTIONS "FILE PREFIX LEN LIBRARY DIR REG LETTERS MACRO MACRO=DEFN %%\
`-3' Support 3rd edition of Wirth's book (implicitly the 4th edition).\n\
`-strict' Prohibit language extensions.\n\
`-all' Compilation of all program modules with removing unused procedures.\n\
`-C'  Only compilation of Modula-2 modules to C code.\n\
`-CM' Only output make dependence for each Modula-2 module\n\
      to standard output.\n\
`-make'\n\
`-MAKE'\n\
`-PDIR' Search of definition modules in this directory.\n\
`-test' Generation of dynamic tests.\n\
`-update' update of any modula module output.\n\
`-upper-case' only upper case keywords and standard identifiers\n\
              (-strict means -upper-case)\n\
`-v' Output of command lines of processes loaded.\n\
`-o FILE'\n\
`-c'\n\
`-S'\n\
`-E'\n\
`-pipe'\n\
`-BPREFIX'\n\
`-bPREFIX'\n\
`-ansi'\n\
`-traditional'\n\
`-O'\n\
`-g'\n\
`-gg'\n\
`-w'\n\
`-W'\n\
`-Wimplicit'\n\
`-Wreturn-type'\n\
`-Wunused'\n\
`-Wswitch'\n\
`-Wcomment'\n\
`-Wtrigraphs'\n\
`-Wall'\n\
`-Wshadow'\n\
`-Wid-clash-LEN'\n\
`-Wpointer-arith'\n\
`-Wcast-qual'\n\
`-Wwrite-strings'\n\
`-p'\n\
`-pg'\n\
`-a'\n\
`-lLIBRARY'\n\
`-LDIR'\n\
`-nostdlib'\n\
Machine-dependent options for vax (-mMACHINESPEC):\n\
  `-munix' `-mgnu' `-mg'\n\
Specify machine-independent flags (-fFLAG). \n\
The negative form of -ffoo would be -fno-foo.In the table below, \n\
two forms are listed--the first which is not the default. \n\
  `-fpcc-struct-return'      `-fno-pcc-struct-return'\n\
  `-ffloat-store'            `-fno-float-store'\n\
  `-fno-asm'                 `-fasm'\n\
  `-fno-defer-pop'           `-fdefer-pop'\n\
  `-fstrength-reduce'        `-fno-strength-reduce'\n\
  `-fcombine-regs'           `-fno-combine-regs'\n\
  `-fforce-mem'              `-fno-force-mem'\n\
  `-fforce-addr'             `-fno-force-addr'\n\
  `-fomit-frame-pointer'     `-fno-omit-frame-pointer'\n\
  `-finline-functions'       `-fno-inline-functions'\n\
  `-fcaller-saves'           `-fno-caller-saves'\n\
  `-fkeep-inline-functions'  `-fno-keep-inline-functions'\n\
  `-fwritable-strings'       `-fno-writable-strings'\n\
  `-fcond-mismatch'          `-fno-cond-mismatch'\n\
  `-fno-function-cse'        `-ffunction-cse'\n\
  `-fvolatile'               `-fno-volatile'\n\
  `-fshared-data'            `-fno-shared-data'\n\
  `-funsigned-char'          `-fno-unsigned-char'\n\
  `-fsigned-char'            `-fno-signed-char'\n\
  `-fdelayed-branch'         `-fno-delayed-branch'\n\
  `-ffixed-REG'\n\
  `-fcall-used-REG'\n\
  `-fcall-saved-REG'G'\n\
`-dLETTERS'  Says to make debugging dumps at times specified by LETTERS.\n\
Here are the possible letters:\n\
       r, j, s, L, f, c, l, g, d, J, m.\n\
`-pedantic'\n\
`-static'\n\
 These options control the C preprocessor:\n\
`-CC' It is flag -C of gcc.\n\
`-IDIR' \n\
`-I-'\n\
`-i FILE' \n\
`-nostdinc'\n\
`-M' \n\
`-MM'\n\
`-DMACRO'\n\
`-DMACRO=DEFN'\n\
`-UMACRO'\n\
`-trigraphs'"

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

#define NONSTANDARD_MODULA_OUTPUT_NAME (last_flag_place("-o")?\
                                        flag_parameter("-o"):NULL)

/* MODULA_OUTPUT_SUFFIX is not accounted for only
   NONSTANDARD_MODULA_OUTPUT_NAME is not null string. */

#define MODULA_OUTPUT_SUFFIX ".c"


#ifndef C_COMPILER
#define C_COMPILER "gcc"
#endif

#define OBJECT_FILE_SUFFIX ".o"


/* Make flags must be not accounted for. */

#define C_COMPILATION_GUARD (!last_flag_place("-C") && !last_flag_place("-CM"))

#define C_INPUT_SUFFIXES {".c",".i",".s"}

#define C_OUTPUT_EXISTS (!last_flag_place("-E") && !last_flag_place("-M")\
			 && !last_flag_place("-MM"))

/* If IS_C_OUTPUT_DELETED is TRUE than the output file is deleted by m2c
   in any case after its usage (as C compiler does it).  Otherwise the C
   output file is deleted only on failure for its creation. */

#define IS_C_OUTPUT_DELETED (!last_flag_place("-make")\
			     && !last_flag_place("-update")\
                             && !last_flag_place("-c")\
			     && !last_flag_place("-S"))

/* If NONSTANDARD_C_OUTPUT_NAME is not null string than the input file
   is compiled to the file with this name (not to the file with source name
   and C_OUTPUT_SUFFIX). */

#define NONSTANDARD_C_OUTPUT_NAME ((last_flag_place("-c")\
				    || last_flag_place("-S"))\
				   && last_flag_place("-o")\
				   ?flag_parameter("-o"):NULL)

/* If MAY_BE_C_NONSTANDARD_OBJECT is TRUE than C compiler can generate
   object file with nonstandard name. */

#define MAY_BE_C_NONSTANDARD_OBJECT TRUE

/* Used when is MAY_BE_C_NONSTANDARD_OBJECT TRUE. It defines flag which
   determine nonstandard name object file. */

#define C_NONSTANDARD_OBJECT_FLAG "-o"

/* Used when is MAY_BE_C_NONSTANDARD_OBJECT TRUE.
   If IS_C_NONSTANDARD_OBJECT_SEPARATE is TRUE then flag parameter (object
   file name) is separate. */

#define IS_C_NONSTANDARD_OBJECT_SEPARATE TRUE

#define C_OUTPUT_SUFFIX (last_flag_place("-S")?".s":OBJECT_FILE_SUFFIX)


/* It is also tested that C_COMPILATION_GUARD is TRUE.  Make flags must be
   not accounted for. */

#define LOAD_GUARD (!last_flag_place("-E") && !last_flag_place("-M")\
		    && !last_flag_place("-MM") && !last_flag_place("-S")\
                    && !last_flag_place("-c"))


#define LOAD_OUTPUT_NAME (last_flag_place("-o")?flag_parameter("-o"):"a.out")


#define TRANSLATE_FLAG_TO_C(opt) (!strcmp(opt,"-3") || !strcmp(opt,"-strict")\
  || !strcmp(opt,"-all")  || !strcmp(opt,"-C") || !strcmp(opt,"-CM")\
  || !strcmp(opt,"-make") || !strcmp(opt,"-MAKE") || !strcmp(opt,"-P")\
  || !strcmp(opt,"-test") || !strcmp(opt,"-update")\
  || !strcmp(opt,"-upper-case")\
  || !strcmp(opt,"-l") || !strcmp(opt,"-L") || !strcmp(opt,"-nostdlib")\
  || (!strcmp(opt,"-o") && NONSTANDARD_C_OUTPUT_NAME==NULL)\
 ?NULL:(!strcmp(opt,"-CC")?"-C":opt))

#define C_ADDITIONAL_FLAG(n) (n==0 && LOAD_GUARD?"-c":NULL)

#define TRANSLATE_FLAG_TO_LOAD(opt) (!strcmp(opt,"-3")\
  || !strcmp(opt,"-strict")\
  || !strcmp(opt,"-all")  || !strcmp(opt,"-C") || !strcmp(opt,"-CM")\
  || !strcmp(opt,"-make") || !strcmp(opt,"-MAKE") || !strcmp(opt,"-P")\
  || !strcmp(opt,"-test") || !strcmp(opt,"-update")\
  || !strcmp(opt,"-upper-case")\
  || !strcmp(opt,"-c") || !strcmp(opt,"-S") || !strcmp(opt,"-E")\
  || !strcmp(opt,"-ansi") || !strcmp(opt,"-traditional")\
  || !strcmp(opt,"-O") || !strcmp(opt,"-w")\
  || !strcmp(opt,"-W") || !strcmp(opt,"-Wimplicit")\
  || !strcmp(opt,"-Wreturn-type") || !strcmp(opt,"-Wunused")\
  || !strcmp(opt,"-Wswitch") || !strcmp(opt,"-Wcomment")\
  || !strcmp(opt,"-Wtrigraphs") || !strcmp(opt,"-Wall")\
  || !strcmp(opt,"-Wshadow") || !strcmp(opt,"-Wid-clash-")\
  || !strcmp(opt,"-Wpointer-arith") || !strcmp(opt,"-Wcast-qual")\
  || !strcmp(opt,"-Wwrite-strings") || !strcmp(opt,"-munix")\
  || !strcmp(opt,"-mgnu") || !strcmp(opt,"-mg")\
  || !strcmp(opt,"-fpcc-struct-return")\
  || !strcmp(opt,"-ffloat-store") || !strcmp(opt,"-fno-asm")\
  || !strcmp(opt,"-fno-defer-pop") || !strcmp(opt,"-fstrength-reduce")\
  || !strcmp(opt,"-fcombine-regs") || !strcmp(opt,"-fforce-mem")\
  || !strcmp(opt,"-fforce-addr") || !strcmp(opt,"-fomit-frame-pointer")\
  || !strcmp(opt,"-finline-functions") || !strcmp(opt,"-fcaller-saves")\
  || !strcmp(opt,"-fkeep-inline-functions")\
  || !strcmp(opt,"-fwritable-strings")\
  || !strcmp(opt,"-fcond-mismatch") || !strcmp(opt,"-fno-function-cse")\
  || !strcmp(opt,"-fvolatile") || !strcmp(opt,"-fshared-data")\
  || !strcmp(opt,"-funsigned-char") || !strcmp(opt,"-fsigned-char")\
  || !strcmp(opt,"-fdelayed-branch") || !strcmp(opt,"-ffixed")\
  || !strcmp(opt,"-fcall-used") || !strcmp(opt,"-fcall-saved")\
  || !strcmp(opt,"-fno-pcc-struct-return")\
  || !strcmp(opt,"-fno-float-store") || !strcmp(opt,"-fasm")\
  || !strcmp(opt,"-fdefer-pop") || !strcmp(opt,"-fno-strength-reduce")\
  || !strcmp(opt,"-fno-combine-regs") || !strcmp(opt,"-fno-force-mem")\
  || !strcmp(opt,"-fno-force-addr") || !strcmp(opt,"-fno-omit-frame-pointer")\
  || !strcmp(opt,"-fno-inline-functions") || !strcmp(opt,"-fno-caller-saves")\
  || !strcmp(opt,"-fno-keep-inline-functions")\
  || !strcmp(opt,"-fno-writable-strings")\
  || !strcmp(opt,"-fno-cond-mismatch") || !strcmp(opt,"-ffunction-cse")\
  || !strcmp(opt,"-fno-volatile") || !strcmp(opt,"-fno-shared-data")\
  || !strcmp(opt,"-fno-unsigned-char") || !strcmp(opt,"-fno-signed-char")\
  || !strcmp(opt,"-fno-delayed-branch")\
  || !strcmp(opt,"-d") || !strcmp(opt,"-pedantic")\
  || !strcmp(opt,"-static") || !strcmp(opt,"-CC")\
  || !strcmp(opt,"-I") || !strcmp(opt,"-I-")\
  || !strcmp(opt,"-i") || !strcmp(opt,"-nostdinc")\
  || !strcmp(opt,"-M") || !strcmp(opt,"-MM")\
  || !strcmp(opt,"-D") || !strcmp(opt,"-U") || !strcmp(opt,"-trigraphs")\
                  ?NULL:opt)

#define LOAD_ADDITIONAL_FLAG(n) NULL

/* If IS_ORDER_IMPORTANT is TRUE than the flag is on the same place (relative
   to parameters) in all executed proccess command lines as in the source
   command line. */

#define IS_ORDER_IMPORTANT(opt) (!strcmp(opt,"-l"))

/* The following file is to be included here! */

#include "common_part.h"
