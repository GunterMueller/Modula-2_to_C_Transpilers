/* Common definitions for all GNU Modula-2 translator files.
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

#include "m2-config.h"
#include "m2-errors.h"
#include "m2-icode.h"
#include "m2-type-size.h"
#include "m2-library.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>


/* It is necessary when char is not unsigned char. */

#define CHAR_TO_INT(ch) ((ch)&0377)


/* The following type used for storing semantic information about language
   construction representing types and expressions. */

typedef struct
{
  /* Pointer to subgraph representing type definition
     of expressions or types. */
  ICN_pointer type;


  /* The all following members are defined only for expressions. */

  /* TRUE if the corresponding expression is a constant expression. */
  char it_is_constant;
  /* TRUE if the corresponding expression is a variable designator. */
  char it_is_designator;
  /* TRUE if the corresponding expression is an open array parameter. */
  char it_is_array_parameter;
} semantic_information;


extern int only_upper_case_flag;
extern int test_flag;
extern int pass_of_picking_used_objects_when_all_flag;
extern int pass_of_generation_when_all_flag;
extern int testing_main_module;
extern int picking_names_of_imported_modules;
extern ICN_pointer current_compilation_unit;
extern ICN_pointer current_scope;
extern FILE *current_input_file;
extern int current_line_number;
extern int strict_flag;
extern int third_edition_flag;
extern int it_is_definition_module;
extern int it_is_main_module;
