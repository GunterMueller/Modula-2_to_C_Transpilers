/* Generator Modula-2 to C.
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

#include "m2-common.h"
#include "m2-generator.h"

#include <setjmp.h>

static void output_file_error ();
static void generate_necessary_type_definitions ();
static void generate_call ();
static void generate_expression ();
static void generate_block ();
static int block_code_may_be_used ();



/* This page contains C representations of primitive modula-2 types. */


static char char_type_representation[] = "unsigned char";
static char cardinal_type_representation[] = "unsigned";
static char real_type_representation[] = "float";
static char integer_type_representation[] = "int";
static char shortcard_type_representation[] = "unsigned short";
static char shortreal_type_representation[] = "float";
static char shortint_type_representation[] = "short";
static char longcard_type_representation[] = "unsigned long long";
static char longreal_type_representation[] = "double";
static char longint_type_representation[] = "long long";
static char byte_type_representation[] = "char";  // TODO: signed or unsigned?
static char word_type_representation[] = "int";
static char set_type_representation[] = "unsigned int";
static char void_type_representation[] = "void";
static char address_type_representation[] = "uintptr_t";




/* This page contains names (or its prefixes) of objects used in generated C
   program.  The page also contains commentaries about generated C program. */


/* Labels are used in the generated program.  Its names is the following string
   and label number. */

static char label_name_prefix[] = "label";

/* The following string is name of static variable in generated code of
   implementation module.  Nonzero value of the variable means that
   the module body has been executed.  The implementation module body
   is to be executed only once. */

static char implementation_module_initiation_flag_name[]
= "module_initiation_flag";

/* The following string is name of C parameter of function which implements
   Modula-2 procedure without parameters declared on the uppest level.
   Such function may be used as coroutine.  Nonzero parameter value means
   that given call is used coroutine creation (by standard procedure
   NEWPROCESS). */

static char coroutine_parameter_name[] = "coroutine_creation_flag";

/* The following string var_prefix is used in many cases:
   1) as name of structured local C variable which contains of all variables of
      given procedure;

   2) as prefix of name of structured global C variable which contains of all
      unit compilation variables on the uppest declaration level (the prefix
      is followed by compilation unit name);

   3) as prefix of name of C structure which declares procedure variables (the
      C variable mentioned in the first case is declared by the structure) and
      as prefix of name of static C variable which is pointer to this structure
      (if given procedure has nested procedures).

  In third case the prefix is followed by full procedure name.  For example,
                 module m;
		   var i: INTEGER;
		   module m1;
		     var c: CHAR;
		     procedure p;
		       var r: real;
		       procedure p1;
		         var s: BITSET;
			 begin end p1;
		       begin end p;
		     end m1;
		   end m.
  will generate the following declarations
        struct {int _i;
                char _c_m1;
       	}var_m;
        static struct var_p_m1_m{float _r;
                                 }*var_p_m1_m;
        static int _p1_p_m1_m()
        {struct{unsigned int _s;
               }var;
       	...
        }
        static int _p_m1_m(coroutine_creation_flag)int coroutine_creation_flag;
        {struct var_p_m1_m var
               ...
        }
*/

static char var_prefix[] = "var";

/* The following string par_prefix is used in many cases:
   1) as name of structured C parameter which contains of all parameters of
      given procedure;

   2) as prefix of name of C structure which declares procedure parameters (the
      C parameter mentioned in the first case is declared by the pointer to
      this structure) and as prefix of name of static C variable which is
      pointer to this structure (if given procedure has nested procedures).

   In second case the prefix is followed by full procedure name.  For example,
     module m;
       procedure p (var r: real);
	 procedure p1 (s: BITSET);
	   begin end p1;
	 begin end p;
     end m.
  will generate the following declarations
        struct par_p_m{float* _r;};
        struct par_p1_p_m{unsigned int _s;};
        static struct par_p_m *par_p_m;
        static int _p1_p_m(par)register struct par_p1_p_m *par;
        {
	    ...
        }
        static int _p_m(par)register struct par_p_m *par;
        {
	    ...
        }
*/

static char par_prefix[] = "par";

/* The two following strings
      name_of_saved_pointer_to_procedure_variables and
      name_of_saved_pointer_to_procedure_parameters
   are used as name of local C variables in procedure
   which has nested procedures.  These variables are pointers to the structures
   representing all procedure variables or parameters.  These variables are
   used for saving and restoring corresponding static variables.

   For example,
     module m;
       procedure p (var r: real);
         var i: integer;
	 procedure p1 (s: BITSET);
	   var c: char;
	   begin end p1;
	 begin end p;
     end m.
  will generate the following declarations
        struct par_p_m{float* _r;};
        struct par_p1_p_m{unsigned int _s;};
        static struct var_p_m{int _i;}*var_p_m;
        static struct par_p_m *par_p_m;
        static int _p1_p_m(par)register struct par_p1_p_m *par;
        {struct{char _c;}var;
	  ...
        }
        static int _p_m(par)register struct par_p_m *par;
        {struct var_p_m var, *saved_var_ptr;                  !!!
         struct par_p_m *saved_par_ptr;                       !!!
           saved_var_ptr=var_p_m; var_p_m=(&var);
           saved_par_ptr=par_p_m; par_p_m=par;
	   ...
 	   var_p_m=saved_var_ptr; par_p_m=saved_par_ptr;
	}
*/

static char name_of_saved_pointer_to_procedure_variables[] = "saved_var_ptr";

static char name_of_saved_pointer_to_procedure_parameters[] = "saved_par_ptr";

/* The two following strings are used as prefix of name of C structure which
   declares procedure parameters and as prefix of name of parameter which is
   declared in the C structure.  These strings are used only procedure whose
   name is not known.

   The prefixes are followed by unique numbers which are defined by
   addresses of icode representations of corresponding procedure type
   and parameter type.  For example,
      module m;
        var p: procedure(cardinal, char);
      end m.
   will generate the following declaration
     struct par_anonim_proc26480
        {unsigned int anonim_par264ac;
         char anonim_par264dc;};
*/

static char anonim_procedure_type_name_prefix[] = "anonim_proc";

static char anonim_parameter_type_name_prefix[] = "anonim_par";

/* Two following strings are names of members of C structure which
   implements open array parameter.  The first member value is address
   of passed array.  The second member value is value of standard procedure
   HIGH for the actual parameter.  The way of passing open array parameter is
   the same for value and variable parameters.  For example,
      module m;
        procedure p (a: array of char);
        begin end p;
      end m.
   will generate the following declarations
      struct par_p_m{
        struct{char* adr;unsigned int high;}_a;
      };
      static int _p_m(par)register struct par_p_m *par;
      {
         ...
      }
*/

static char open_array_address_name[] = "adr";

static char open_array_high_value_name[] = "high";

/* The following name is used for local variable which represents function
   result. */

static char function_result_name[] = "result";

/* This prefix is used for name of variable which contains string constant.
   The prefix is followed by string number.  Such implementation of string
   constant is needed for that the constant string is represented by the
   only copy. */

static char string_name_prefix[] = "string";

/* The following string is used as prefix of name of C structure which
   implements Modula-2 record within other type definition.
   The prefix is followed by unique number which is defined by
   address of icode representation of corresponding record.  For example,
      module m;
        type
          t = record i: integer;
	             r: record c: char; end;
	      end;
      end m.
   will generate the following declarations
      struct record26504{char _c;};
      struct _t_m{int _i;
        struct record26504 _r;
      };
*/

static char anonim_record_name_prefix[] = "record";

/* Cyclic pointer is represented as C structure.  It is the only way of
   implementation.  To implement cyclic pointer definition the two following
   strings are used as prefix of name of C structure and as name of the
   structure member.  The prefix is followed by unique number which is defined
   by address of icode representation of corresponding pointer type definition.
   Reference to pointer value is always the member value.

   For example,
      module m;
        type
          t = pointer to t1;
          t1 = pointer to t;
      end m.
   will generate the following declarations
      struct ptr_struct26480{struct ptr_struct26480** ptr_value;};
      struct ptr_struct264bc{struct ptr_struct26480* ptr_value;};

*/

static char pointer_structure_name_prefix[] = "ptr_struct";

static char pointer_member_name[] = "ptr_value";

/* Record case part is represented as C union and record case variant is
   represented as C structure.  Such implementation is used even if
   the record case part contains only one variants or the record case variant
   contains only one field.  The following prefixes
   record_case_part_name_prefix and record_case_variant_name_prefix are used
   as prefixes of names of the C union and the C structure.  The prefixes are
   followed by number of record case part (from end of the record) or of case
   variant (from the end of the record case part).

   For example,
      module m;
      type
       r = record
            case :boolean of
             false: ch: char|
             true: i:integer;
                   case b:boolean of
      	      false: c: cardinal|
                    true: f: real
      	     end;
            end;
           end;
      end m.
   will generate the following declaration
      struct _r_m{
        union{
	  struct{char _ch;}variant2;
	  struct{
	    int _i;unsigned short _b;
	    union{
	      struct{unsigned int _c;}variant2;
	      struct{float _f;}variant1;
	    }case1;
          }variant1;
        }case1;
      };
*/

static char record_case_part_name_prefix[] = "case";

static char record_case_variant_name_prefix[] = "variant";

/* The following prefix is used for name of static function which tests value
   of range.  This function is generated if option `-test' is in command line
   and the test is really needed.  The prefix is followed by unique number
   which is defined by address of icode representation of corresponding
   range type definition.

   For example, under mentioned above conditions
         var i: [1..10];
   will generate the following code
     static unsigned short
     test26490 (e)
       register unsigned short e;
     {
       if ((unsigned) 01 <= e && e <= (unsigned) 012)
         return e;
       m2_rngovf ();
     }
*/

static char range_test_function_name_prefix[] = "test";

/* Modula-2 actual parameters are passed as single structure.

   For example,
     module m;
       var k: integer;
       procedure p (c: cardinal; var i: integer);
         begin end p;
       procedure p1 (ch: char): integer;
         begin return 10; end p1;
     begin
       p (10, k);
       k := p1 (10C);
     end m.
   will generate the following code
     ...
     struct par_p_m{unsigned int _c;int* _i;};
     struct par_p1_m{char _ch;};
     ...
     main (...)
     {
       struct par_p_m par_p_m;
       struct par_p1_m par_p1_m;
       (par_p_m._c = (10), par_p_m._i = (&(var_m._k)),
        _p_m (&par_p_m));
       (var_m._k)
         = ((par_p1_m._ch = ('\10'), _p1_m (&par_p1_m)));
     }
*/

/* The value open array parameter is passed by its address (see commentaries for
   open_array_address_name).  If address of actual parameter can not be in C
   then a C structure is generated for each such call.  The C structure contains
   members for each such parameter.  The following strings are used as as prefix
   of name of the C structure and as prefix of name of the C member.  Prefix of
   name of the C structure and as prefix of name of the C member are followed by
   order number of corresponding call and order number of such parameter in the
   call.

   For example,
     module m;
     from SYSTEM import BYTE;
       procedure p (a: array of BYTE);
       begin end p;
     begin
       p (10C);
     end m.
   will generate the following code
     struct par_p_m {struct {char *adr; unsigned int high;} _a;};
     ...
     main ()
     {
       struct par_p_m par_p_m;
       struct {char temp_actual1;} temp_call_actuals1;
       (temp_call_actuals1.temp_actual1 = ('\10'),
        par_p_m._a.adr
        = (char *) (&temp_call_actuals1.temp_actual1),
        par_p_m._a.high = 00,
        _p_m (&par_p_m));
       ...
     }
*/

static char temp_call_actuals_name_prefix[] = "temp_call_actuals";

static char actual_name_prefix[] = "temp_actual";

/* The following strings temp_expression_variables_names_prefix and
   temp_variable_name_prefix are used as prefix of C structure name and as
   prefix of name of C member in the C structure.  These data are used as
   temporary variables.  The C structure corresponds to Modula-2 expression in
   which temporary variables are needed.  The C member corresponds to a
   temporary variable in the expression.  The prefixes are followed by number of
   corresponding Modula-2 expression in current block and number of temporary
   variable in the expression.

   Temporary variables are needed in following cases:
     1) call of type transfer function with constant (e.g. integer (10.0)
        generates (temp_expr_vars0.temp_var1=(1.0e+01),
	           *(int*)&temp_expr_vars0.temp_var1));
     2) pass of real constant as parameter of type WORD;
     3) implementation of functions with structured results (this Modula-2
        implementation permits structured results of function).
   Example of third case.
     module m;
       var b: boolean;
       type t = array [0..10] of char;
       procedure f (i: integer): t;
       begin end f;
     begin
       b := f(0) = f (1);
     end m.
   will generate the following code
     struct {unsigned short _b;} var_m;
     static int _f_m (par, result)
       register struct par_f_m *par; char result[013];
     {...}
     main ()
     {
       ...
       struct {
         char temp_var1[013];
         char temp_var2[013];
       } temp_expr_vars0;
       (var_m._b)
         = (m2_eq
	    (((par_f_m._i = (0),
	       _f_m (&par_f_m,
	             temp_expr_vars0.temp_var1),
	    temp_expr_vars0.temp_var1)),
	    ((par_f_m._i = (1),
	      _f_m (&par_f_m,
	            temp_expr_vars0.temp_var2),
	      temp_expr_vars0.temp_var2)), 013));
     }
*/

static char temp_expression_variables_names_prefix[] = "temp_expr_vars";

static char temp_variable_name_prefix[] = "temp_var";

/* The following string is prefix of name of local variable which is used for
   implementation of with statement.  The type of variable is pointer
   to type of corresponding designator after keyword with.  The prefix is
   followed by number of level of nesting with statement.

   For example,
      with r1 do
            with r2 do end;
      end;
   will generate the following statements
      {register struct record264ac* with_var1=(&(var_m._r1));
         {register struct record264ac* with_var2=(&(var_m._r2));
	   ...
	 }
       ...
      }
*/

static char with_variable_name_prefix[] = "with_var";

/* The following string is name of global variable which is used for
   implementation of standard procedure ABS.  The variable is defined in
   main module and represented as C union. */

static char union_name_for_abs_implementation[] = "abs_union";

/* The following array contains names of union
   UNION_NAME_FOR_ABS_IMPLEMENTATION members which are used for implementation
   of standard procedure ABS.  The order in the array is the same as
   in enumeration basic_type. */

static char *names_of_variables_for_abs_implementation[] =
{NULL, NULL, NULL,		/*BTM_SHORT_CARDINAL-BTM_LONG_REAL*/
 "abs_short_int", "abs_int", "abs_long_int",
 "abs_short_real", "abs_real", "abs_long_real"
};



/* This page contains declarations of the generator static variables and
   of the generator passes on blocks. */


/* Name of C file which is translation of current compiled module.  It may be
   temporary file depending on given command flags. */

static char *modula_output_file_name;

/* This is file with name MODULA_OUTPUT_FILE_NAME. */

static FILE *output_file;

/* The following enumeration declares the names of the generator passes on
   Modula-2 blocks.  The passes STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS
   and STATEMENTS_GENERATION_PASS are always executed.  Necessity of the order
   passes are determined on the pass
   STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS.  The generator starts
   processing of next block only when all necessary passes on the current
   block are executed. */


enum generator_pass
{
  /* See commentaries for string_name_prefix. */
  STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS,
  /* See commentaries for all_actuals_name. */
  ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS,
  /* See commentaries for temp_actuals_name. */
  TEMP_ARRAY_ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS,
  /* See commentaries for temp_variables_name. */
  TEMPORARY_VARIABLES_DECLARATION_GENERATION_PASS,
  STATEMENTS_GENERATION_PASS
};

/* Current block pass of the generator. */

static enum generator_pass current_generator_pass;

/* Line number in last generated C construction `#line ...'.  The variable
   value is undefined when it is zero or current generator pass is
   not STATEMENTS_GENERATION_PASS. */

static int last_generated_line_number;

/* Order number (1, 2, ...) of current block among blocks for which
   ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS is needed. */

static int current_block_number_among_blocks_with_actuals;

/* The following variable value is TRUE if current generator pass is
   STATEMENTS_GENERATION_PASS. */

static int it_is_statements_generation_pass;

/* See commentaries for generation_pass. */

static int actuals_structures_declaration_generation_is_needed;
static int temp_array_actuals_structures_declaration_generation_is_needed;
static int temporary_variables_declaration_generation_is_needed;

/* Number of string constant declarations so far generated in program (see
   commentaries for string_name_prefix.  The variable is used only on
   STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS. */

static int number_of_generated_string_constant_declarations;

/* Order number of expression among expressions with temporary variables in
   current block (see commentaries for temp_actuals_name). */

static int number_of_expression_with_temp_variables;

/* Order number of the current call in current block (see commentaries for
   temp_actuals_name). */

static int number_of_current_call;

/* Order number of the temporary variable in current expression of current
   block (see commentaries for  temp_variables_name). */

static int number_of_current_temporary_variable;

/* Order number of the temporary variable in current call of current
   block (see commentaries for temp_actuals_name.  The variable is used only on
   TEMP_ARRAY_ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS and
   STATEMENTS_GENERATION_PASS. */

static int number_of_temp_variable_in_current_call;

/* The following value is TRUE if current expression needs temporary
   variables for implementation (see commentaries for
   temp_expression_variables_names_prefix).  The variable is used
   only on TEMPORARY_VARIABLES_DECLARATION_GENERATION_PASS. */

static int current_expression_needs_temp_variables;

/* The following value is number of current level of nesting with
   statements (see commentaries for with_variable_name_prefix). */

static int current_level_of_nesting_with_statements;

/* The following values are number of generated labels (see commentaries
   for label_name_prefix), number of exit label for current loop statement
   and number of return label for current block.  The variables are used
   only on STATEMENTS_GENERATION_PASS. */

static int number_of_generated_labels;
static int number_of_exit_label_for_current_loop;
static int number_of_return_label_for_current_block;



/* This page contains macros and function for generation of expression
   varieties.  These varieties are the following or combinations of the
   following.
   - expression of type ADDRESS is represented as expression of type CARDINAL
     (vs. POINTER TO WORD as generally);
   - expression of type pointer implemented by C structure (see commentaries
     for pointer_structure_name_prefix) is represented by member of the
     corresponding structure;
   - object is represented by its address (by not its value);
   - int value is generated for character which is represented by one-character
     string. */

/* The following macros is used for encoding all flags of the expression
   variety by the single integer value. */

#define ENCODE_GENERATION_FLAGS(adr_as_card, through_struct_ptr, adr, ch)\
  (adr_as_card | through_struct_ptr << 1 | adr << 2 | ch << 3)

/* The following macros is used for decoding all flags of the expression
   variety from the single integer value. */

#define DECODE_GENERATION_FLAGS(flags, adr_as_card, through_str_ptr, adr, ch)\
  (adr_as_card = flags & 01, through_str_ptr = flags >> 1 & 01,\
   adr = flags >> 2 & 01, ch = flags >> 3 & 01)


/* The following macros define codes of all used expression varieties (see
   commentaries at the page begin). */

#define ADDRESS_AS_CARDINAL_CODE\
   ENCODE_GENERATION_FLAGS (TRUE, FALSE, FALSE, FALSE)

#define THROUGH_STRUCT_POINTER_CODE\
   ENCODE_GENERATION_FLAGS (FALSE, TRUE, FALSE, FALSE)

#define ADDRESS_CODE\
   ENCODE_GENERATION_FLAGS (FALSE, FALSE, TRUE, FALSE)

#define CHARACTER_CODE  ENCODE_GENERATION_FLAGS (FALSE, FALSE, FALSE, TRUE)

#define ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_CODE\
   ENCODE_GENERATION_FLAGS (TRUE, TRUE, FALSE, FALSE)

#define THROUGH_STRUCT_POINTER_OR_ADDRESS_CODE\
   ENCODE_GENERATION_FLAGS (FALSE, TRUE, TRUE, FALSE)

#define ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE\
   ENCODE_GENERATION_FLAGS (TRUE, FALSE, FALSE, TRUE)

#define THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE\
   ENCODE_GENERATION_FLAGS (FALSE, TRUE, FALSE, TRUE)

#define ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE\
   ENCODE_GENERATION_FLAGS (TRUE, TRUE, FALSE, TRUE)


/* The following macros are used for generation of all possible expression
   varieties (see commentaries at the page begin). */

#define GEN_ADDRESS_AS_CARDINAL(expr)\
  generate_expression_according_to_flags(expr, ADDRESS_AS_CARDINAL_CODE)

#define GEN_THROUGH_STRUCT_POINTER(expr)\
  generate_expression_according_to_flags(expr, THROUGH_STRUCT_POINTER_CODE)

#define GEN_ADDRESS(expr)\
  generate_expression_according_to_flags(expr, ADDRESS_CODE)

#define GEN_CHARACTER(expr)\
  generate_expression_according_to_flags(expr, CHARACTER_CODE)

#define GEN_ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER(expr)\
  generate_expression_according_to_flags\
    (expr, ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_CODE)

#define GEN_THROUGH_STRUCT_POINTER_OR_ADDRESS(expr)\
  generate_expression_according_to_flags\
    (expr, THROUGH_STRUCT_POINTER_OR_ADDRESS_CODE)

#define GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER(expr)\
  generate_expression_according_to_flags\
    (expr, ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE)

#define GEN_THROUGH_STRUCT_POINTER_OR_CHARACTER(expr)\
  generate_expression_according_to_flags\
    (expr, THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE)

#define GEN_ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER(expr)\
  generate_expression_according_to_flags\
    (expr, ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE)


/* The following function generates access to member name when ICODE_PTR
   represents type pointer implemented by C structure (see commentaries
   for pointer_structure_name_prefix).  The function is used for
   generation of expression of such type.  ICODE_PTR is to be not NULL. */

void add_pointer_member_name (icode_ptr)
     register ICN_pointer icode_ptr;
{
  if (MODE (icode_ptr) == ICNM_POINTER_TYPE
      &&
      POINTER_TYPE_NODE (icode_ptr)->corresponding_C_structure_was_generated)
    {
      if (fprintf (output_file, ".%s", pointer_member_name) < 0)
	output_file_error ();
    }
}

/* The following function generates variety of expression EXPR.  The
   expression itself is generated only on statement generation pass.  On
   the other pass the processing expression may generates various structures
   (see commentaries for generator_pass). The variety is defined by FLAGS.
   EXPR is to represent expression. */

static void
generate_expression_according_to_flags (expr, flags)
     ICN_pointer expr;
     int flags;
{
  register int right_bracket_is_needed, address_as_cardinal_flag;
  register int through_struct_pointer_flag, address_flag, character_flag;
  semantic_information sinf;

  if (it_is_statements_generation_pass)
    {
      subgraph_is_expression (expr, &sinf);
      DECODE_GENERATION_FLAGS
	(flags, address_as_cardinal_flag, through_struct_pointer_flag,
	 address_flag, character_flag);
      right_bracket_is_needed = TRUE;
      if (address_as_cardinal_flag
	  && sinf.type == ICN_POINTER (&ICN_TD_address))
	{
	  /* Transform pointer to type of cardinal representation. */
	  if (fprintf (output_file, "((%s)", address_type_representation) < 0)
	    output_file_error ();
	}
      else if (address_flag && MODE (sinf.type) != ICNM_ARRAY_TYPE
	       && !it_is_string_type (sinf.type)
	       && !sinf.it_is_array_parameter)
	{
	  if (fputs ("(&", output_file) == EOF)
	    output_file_error ();
	}
      else if (character_flag && it_is_string_type (sinf.type))
	{
	  /* Character is represented by string. */
	  if (fputs ("(*", output_file) == EOF)
	    output_file_error ();
	}
      else
	right_bracket_is_needed = FALSE;
    }
  generate_expression (expr);
  if (it_is_statements_generation_pass)
    {
      if (sinf.it_is_designator && through_struct_pointer_flag)
	add_pointer_member_name (sinf.type);
      if (right_bracket_is_needed)
	{
	  if (fputc (')', output_file) == EOF)
	    output_file_error ();
	}
      if (address_flag && sinf.it_is_array_parameter)
	{
	  if (fprintf (output_file, ".%s", open_array_address_name) < 0)
	    output_file_error ();
	}
    }
}



/* The page contains auxiliary functions mainly for icode navigation for
   generation time. */


/* The following function fixes error of unsuccessful output to the object
   file. */

static void
output_file_error ()
{
  error_with_parameter (ERR_output_to_object_file, modula_output_file_name);
}

/* The function returns TRUE if OBJECT is nested in scope given by PARENT.
   OBJECT is to be NULL or refer to denotation. */

static int
it_is_parent (parent, object)
     register ICN_pointer parent, object;
{
  while (object != NULL)
    if (SCOPE (object) == parent)
      return TRUE;
    else
      object = SCOPE (object);
  return FALSE;
}

/* The following function returns the nearest (after DENOTATION) textually
   module which is declared in the same scope as DENOTATION.  If such module
   does not exist the function return NULL.  DENOTATION is to refer to
   denotation. */

static ICN_pointer
next_brother_module (denotation)
     register ICN_pointer denotation;
{
  register ICN_pointer declaration_element;

  for (declaration_element = NEXT_DECLARATION_ELEMENT (denotation);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_MODULE)
      {
	if (SCOPE (declaration_element) == SCOPE (denotation))
	  return declaration_element;
	else if (!it_is_parent (SCOPE (denotation), declaration_element))
	  break;
      }
  return NULL;
}

/* The following function returns the first (after non-null DENOTATION)
   textually module which is declared in scope defined by DENOTATION.  If such
   module does not exist the function return NULL. */

static ICN_pointer
first_son_module (denotation)
     register ICN_pointer denotation;
{
  register ICN_pointer declaration_element;

  for (declaration_element = NEXT_DECLARATION_ELEMENT (denotation);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_MODULE)
      {
	if (SCOPE (declaration_element) == denotation)
	  return declaration_element;
	else if (!it_is_parent (denotation, declaration_element))
	  break;
      }
  return NULL;
}

/* The following function returns the most nested scope which defines new
   declaration level (i.e. procedure or compilation unit) and which contains
   DENOTATION.  If DENOTATION itself defines new declaration level then the
   function returns DENOTATION.  DENOTATION is to be NULL or to refer to
   denotation. */

static ICN_pointer
declaration_level_scope (denotation)
     register ICN_pointer denotation;
{
  for (; denotation != NULL && MODE (denotation) != ICNM_PROCEDURE
       && (MODE (denotation) != ICNM_MODULE
	   || ((enum module_mode) MODULE_NODE (denotation)->module_mode
	       == MM_LOCAL_MODULE));
       denotation = SCOPE (denotation)) ;
  return denotation;
}

/* The following function returns TRUE if scope SCOPE defines new declaration
   level (i.e. procedure or compilation unit) and contains any variable
   declaration on this declaration level.  SCOPE is to refer to
   denotation. */

static int
declaration_level_scope_has_variable_declarations (scope)
     ICN_pointer scope;
{
  register ICN_pointer declaration_element;

  for (declaration_element = NEXT_DECLARATION_ELEMENT (scope);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_VARIABLE_DECLARATION
	&& declaration_level_scope (declaration_element) == scope)
      return TRUE;
  return FALSE;
}

/* The following function returns textually next (after declaration element
   START) variable declared immediately within SCOPE. */

static ICN_pointer
next_variable_declaration (scope, start)
     ICN_pointer scope, start;
{
  register ICN_pointer declaration_element;

  for (declaration_element = NEXT_DECLARATION_ELEMENT (start);;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (declaration_element == NULL
	|| (MODE (declaration_element) == ICNM_VARIABLE_DECLARATION
	    && SCOPE (declaration_element) == scope))
      return declaration_element;
}


/* The function returns TRUE if PROCEDURE_PTR is exported from current
   compilation unit.  PROCEDURE_PTR is to refer procedure node. */

static int
procedure_is_exported_from_compilation_unit (procedure_ptr)
     ICN_pointer procedure_ptr;
{
  if (MODE (procedure_ptr) != ICNM_PROCEDURE)
    abort ();
  return
    (IN_DEFINITION_MODULE (procedure_ptr)
     || (find_denotation_in_second_space (current_compilation_unit,
					  IDENTIFIER (procedure_ptr))
	 != NULL
	 && find_identifier_declaration (current_compilation_unit,
					 IDENTIFIER (procedure_ptr))
	 == procedure_ptr));
}



/* This page contains functions for generation of names and declarations
   and some auxiliary functions. */

/* The function outputs generated name of OBJECT to object file.  If
   VAR_PREFIX_IS_NEEDED is TRUE than var_prefix (see commentaries for it) is
   output before the name.  OBJECT is to refer to field, formal parameter,
   variable, procedure, type declaration or to non-local module.

   All generated names start with `_'.  As a consequence C keyword can not be
   generated.  The generated name of field or formal parameter is the
   corresponding identifier.  The other kind object name is the corresponding
   identifier and identifiers of all scopes (procedure or module) which
   contain given object.  All the identifiers are separated by `_'.
   An exception of the rules is the procedure exported from the compilation
   unit.  For such procedure only the compilation unit name is output.
   Remember that the following situation may exist.
	 DEFINITION MODULE a; PROCEDURE proc; END a.
	 IMPLEMENTATION MODULE a;
	 MODULE b; EXPORT proc;...; END b;
	 END a.
*/

static void
output_object_full_name (object, var_prefix_is_needed)
     register ICN_pointer object;
     int var_prefix_is_needed;
{
  register enum icode_node_mode object_mode;
  register int export_flag;
  register ICN_pointer scope;

  if (var_prefix_is_needed)
    {
      if (fputs (var_prefix, output_file) == EOF)
	output_file_error ();
    }
  if (fputc ('_', output_file) == EOF)
    output_file_error ();
  object_mode = MODE (object);
  if (object_mode == ICNM_FIELD || object_mode == ICNM_FORMAL_PARAMETER)
    {
      if (fputs (IDENTIFIER_NODE (IDENTIFIER (object))->identifier_string,
		 output_file) == EOF)
	output_file_error ();
    }
  else if (object_mode == ICNM_VARIABLE_DECLARATION)
    {
      if (fputs (IDENTIFIER_NODE (IDENTIFIER (object))->identifier_string,
		 output_file) == EOF)
	output_file_error ();
      /* Output also names of all upper scopes. */
      for (scope = SCOPE (object);
	   scope != NULL && MODE (scope) != ICNM_PROCEDURE
	   && SCOPE (scope) != NULL;
	   scope = SCOPE (scope))
	{
	  if (fputc ('_', output_file) == EOF)
	    output_file_error ();
	  if (fputs (IDENTIFIER_NODE (IDENTIFIER (scope))->identifier_string,
		     output_file) == EOF)
	    output_file_error ();
	}
    }
  else if (object_mode == ICNM_PROCEDURE)
    {
      if (fputs (IDENTIFIER_NODE (IDENTIFIER (object))->identifier_string,
		 output_file) == EOF)
	output_file_error ();
      export_flag = procedure_is_exported_from_compilation_unit (object);
      /* Output also names of all upper scopes if the procedure is not
	 exported from the compilation unit.  For such procedures only the
	 compilation unit name is output.  Remember that the following
	 situation may exist.
            DEFINITION MODULE a; PROCEDURE proc; END a.
	    IMPLEMENTATION MODULE a;
	    MODULE b; EXPORT proc;...; END b;
	    END a. */
      for (scope = SCOPE (object); scope != NULL; scope = SCOPE (scope))
	if (!export_flag || SCOPE (scope) == NULL)
	  {
	    if (fputc ('_', output_file) == EOF)
	      output_file_error ();
	    if (fputs (IDENTIFIER_NODE (IDENTIFIER (scope))->identifier_string,
		       output_file) == EOF)
	      output_file_error ();
	  }
    }
  else if (object_mode == ICNM_TYPE_DECLARATION
	   || (object_mode == ICNM_MODULE
	       && ((enum module_mode) MODULE_NODE (object)->module_mode
		   != MM_LOCAL_MODULE)))
    {
      if (fputs (IDENTIFIER_NODE (IDENTIFIER (object))->identifier_string,
		 output_file) == EOF)
	output_file_error ();
      /* Output also names of all upper scopes. */
      for (scope = SCOPE (object); scope != NULL; scope = SCOPE (scope))
	{
	  if (fputc ('_', output_file) == EOF)
	    output_file_error ();
	  if (fputs (IDENTIFIER_NODE (IDENTIFIER (scope))->identifier_string,
		     output_file) == EOF)
	    output_file_error ();
	}
    }
  else
    abort ();
}

/* The following recursive function generates access to RECORD_ELEMENT.
   RECORD_ELEMENT is to refer to field in the original call.  In other
   (recursive) calls RECORD_ELEMENT is to refer to field, record case part or
   to record case variant. Modula-2 record implementation is described in
   commentaries for record_case_part_name_prefix and
   record_case_variant_name_prefix. */

static void
generate_field_access (record_element)
     register ICN_pointer record_element;
{
  register ICN_pointer current_record_element;
  register enum icode_node_mode mode;
  register int member_index;
  char *member_prefix;

  mode = MODE (record_element);
  if (mode == ICNM_RECORD_CASE_VARIANT)
    {
      member_index = 0;
      for (current_record_element = record_element;
	   MODE (current_record_element) == ICNM_RECORD_CASE_VARIANT;
	   current_record_element
	   = (RECORD_CASE_VARIANT_NODE (current_record_element)
	      ->next_case_variant))
	member_index++;
      member_prefix = record_case_variant_name_prefix;
    }
  else
    {
      member_index = 0;
      for (current_record_element = record_element;;)
	if (MODE (current_record_element) == ICNM_FIELD)
	  current_record_element
	    = FIELD_NODE (current_record_element)->next_field;
	else if (MODE (current_record_element) == ICNM_RECORD_CASE_PART)
	  {
	    member_index++;
	    current_record_element
	      = (RECORD_CASE_PART_NODE (current_record_element)
		 ->record_element_after_case_part);
	  }
	else
	  break;
      member_prefix = record_case_part_name_prefix;
    }
  if (MODE (current_record_element) != ICNM_RECORD)
    generate_field_access (current_record_element);
  if (mode == ICNM_FIELD)
    {
      if (fputc ('.', output_file) == EOF)
	output_file_error ();
      output_object_full_name (record_element, FALSE);
    }
  else
    {
      if (fprintf (output_file, ".%s%x", member_prefix,
		   (unsigned) member_index) < 0)
	output_file_error ();
    }
}

/* The function outputs STRING only when current pass is statements
   generation. */

static void
output_string_when_statements_generation (string)
     char *string;
{
  if (it_is_statements_generation_pass)
    {
      if (fputs (string, output_file) == EOF)
	output_file_error ();
    }
}

/* The table contains basic C type which implements corresponding discrete
   (except for enumeration types) Modula-2 type.  The discrete type is any
   basic type (except for float types) or any enumeration type.  The
   table element order is the same is enumeration basic_type. */

static char
*discrete_type_representation_table[ /* BTM_SHORT_CARDINAL - BTM_CHAR */ ] =
{shortcard_type_representation, cardinal_type_representation,
 longcard_type_representation, shortint_type_representation,
 integer_type_representation, longint_type_representation,
 NULL, NULL, NULL, char_type_representation
};

/* The function returns C type name which represents corresponding discrete
   Modula-2 type.  TYPE is to refer to discrete type or to range of it. */

static char *
discrete_type_representation (type)
     register ICN_pointer type;
{
  type = through_range_type (type);
  if (MODE (type) == ICNM_ENUMERATION_TYPE)
    type
      = type_of_cardinal_value
      (CARDINAL_NODE (min_or_max (type, FALSE))->cardinal_value);
  return (discrete_type_representation_table
	  [(int) BASIC_TYPE_NODE (type)->basic_type]);
}

/* The following recursive function generates left or right part (parameter
   it_is_left_part) of definition of C type which represents Modula-2
   {POINTER TO}* TYPE.  TYPE_INDIRECTION (>0) determines number of
   constructions `POINTER TO'.  TYPE is to refer to type definition.

   For example MODULA-2 type `ARRAY [2..10] OF CARDINAL' (with
   type_indirection == 2) has
   the left part `unsigned int (**' and right part `)[9]'. */

static void
output_type_definition_part (type, type_indirection, it_is_left_part)
     register ICN_pointer type;
     int type_indirection;
     register int it_is_left_part;
{
  register enum icode_node_mode type_mode;
  char *primitive_type_representation;

  if (type == NULL)
    {
      if (it_is_left_part)
	{
	  if (fputs (void_type_representation, output_file) == EOF)
	    output_file_error ();
	}
    }
  else
    {
      type_mode = MODE (type);
      if (type_mode == ICNM_POINTER_TYPE)
	{
	  if (POINTER_TYPE_NODE (type)
	      ->corresponding_C_structure_was_generated)
	    {
	      /* It is cyclic pointer type which is implemented by C struct. */
	      if (it_is_left_part)
		{
		  if (fprintf (output_file, "struct %s%lx",
			       pointer_structure_name_prefix,
			       (unsigned long) type) < 0)
		    output_file_error ();
		}
	    }
	  else
	    {
	      ++type_indirection;
	      output_type_definition_part (POINTER_TYPE_NODE (type)->base_type,
					   type_indirection, it_is_left_part);
	      return;
	    }
	}
      else if (type_mode == ICNM_ARRAY_TYPE)
	{
	  if (it_is_left_part)
	    {
	      output_type_definition_part (ARRAY_TYPE_NODE (type)->base_type,
					   0, it_is_left_part);
	      if (type_indirection != 0)
		{
		  if (fputc ('(', output_file) == EOF)
		    output_file_error ();
		}
	    }
	  else
	    {
	      if (type_indirection != 0)
		{
		  if (fputc (')', output_file) == EOF)
		    output_file_error ();
		}
	      if (fprintf (output_file, "[0%llo]",
			   array_index_span (ARRAY_TYPE_NODE (type)
					     ->array_index_type) + 1) < 0)
		output_file_error ();
	      output_type_definition_part (ARRAY_TYPE_NODE (type)->base_type,
					   0, it_is_left_part);
	    }
	}
      else if (type_mode == ICNM_PROCEDURE_TYPE)
	{
	  type = PROCEDURE_TYPE_NODE (type)->procedure_result_type;
	  if (type != NULL && (MODE (type) == ICNM_ARRAY_TYPE
			       || MODE (type) == ICNM_RECORD))
	    type = NULL;
	  if (it_is_left_part)
	    {
	      output_type_definition_part (type, 0, it_is_left_part);
	      if (fputs ("(*", output_file) == EOF)
		output_file_error ();
	      if (type_indirection != 0)
		{
		  if (fputc ('(', output_file) == EOF)
		    output_file_error ();
		}
	    }
	  else
	    {
	      if (type_indirection != 0)
		{
		  if (fputc (')', output_file) == EOF)
		    output_file_error ();
		}
	      if (fputs (")()", output_file) == EOF)
		output_file_error ();
	      output_type_definition_part (type, 0, it_is_left_part);
	    }
	}
      else if (type_mode == ICNM_RECORD)
	{
	  if (it_is_left_part)
	    {
	      if (fputs ("struct ", output_file) == EOF)
		output_file_error ();
	      if (RECORD_NODE (type)->record_type_identifier != NULL)
		output_object_full_name
		  (RECORD_NODE (type)->record_type_identifier, FALSE);
	      else
		{
		  if (fprintf (output_file, "%s%lx",
			       anonim_record_name_prefix,
                               (unsigned long) type) < 0)
		    output_file_error ();
		}
	    }
	}
      else if (type_mode == ICNM_QUALIFIED_EXPORT)
	{
	  if (it_is_left_part)
	    {
	      if (fprintf (output_file, "%s *", word_type_representation) < 0)
		output_file_error ();
	    }
	}
      else
	{
	  if (type == ICN_POINTER (&ICN_TD_short_real))
	    primitive_type_representation = shortreal_type_representation;
	  else if (type == ICN_POINTER (&ICN_TD_real))
	    primitive_type_representation = real_type_representation;
	  else if (type == ICN_POINTER (&ICN_TD_long_real))
	    primitive_type_representation = longreal_type_representation;
	  else if (type == ICN_POINTER (&ICN_TD_byte))
	    primitive_type_representation = byte_type_representation;
	  else if (type == ICN_POINTER (&ICN_TD_word))
	    primitive_type_representation = word_type_representation;
	  else if (type_mode == ICNM_SET_TYPE)
	    primitive_type_representation = set_type_representation;
	  else
	    primitive_type_representation
	      = discrete_type_representation (type);
	  if (it_is_left_part)
	    {
	      if (fputs (primitive_type_representation, output_file) == EOF)
		output_file_error ();
	    }
	}
    }
  /* Process indirections. */
  if (it_is_left_part)
    while (type_indirection-- != 0)
      if (fputc ('*', output_file) == EOF)
	output_file_error ();
}

/* The following recursive function is analogous to function
   output_type_definition_part. The first difference is that the function
   suggests that type_indirection == 0 (see previous function's
   commentaries.  The second difference is that the function tests type
   on cyclic pointer type implemented by a C structure.  And if the last
   is true then the function generates declaration of the member type
   (not the structure type itself). */

static void
output_type_definition_part_through_struct_pointer (type, it_is_left_part)
     register ICN_pointer type;
     register int it_is_left_part;
{
  if (MODE (type) == ICNM_POINTER_TYPE
      && POINTER_TYPE_NODE (type)->corresponding_C_structure_was_generated)
    output_type_definition_part (POINTER_TYPE_NODE (type)->base_type, 1,
				 it_is_left_part);
  else
    output_type_definition_part (type, 0, it_is_left_part);
}

/* The following functions generates declaration of object of type
   TYPE or POINTER TO TYPE (if INDIRECTION_FLAG is TRUE).  The name
   of declaration is NAME (if INDEX==0) or NAME followed by INDEX.
   TYPE is to refer to type definition. */

static void
generate_declaration_by_name (type, indirection_flag, name, name_index)
     ICN_pointer type;
     int indirection_flag;
     unsigned long name_index;
     char *name;
{
  output_type_definition_part (type, (indirection_flag ? 1 : 0), TRUE);
  if (name_index == 0)
    {
      if (fprintf (output_file, " %s", name) < 0)
	output_file_error ();
    }
  else
    {
      if (fprintf (output_file, " %s%lx", name, name_index) < 0)
	output_file_error ();
    }
  output_type_definition_part (type, (indirection_flag ? 1 : 0), FALSE);
}


/* The following function is analogous to generate_declaration_by_object but
   the name is determined by OBJECT (see function output_object_full_name).
   OBJECT is to refer to field, formal parameter, variable, procedure, type
   declaration or to non-local module. */

static void
generate_declaration_by_object (type, indirection_flag, object)
     ICN_pointer type;
     int indirection_flag;
     ICN_pointer object;
{
  output_type_definition_part (type, (indirection_flag ? 1 : 0), TRUE);
  if (fputc (' ', output_file) == EOF)
    output_file_error ();
  output_object_full_name (object, FALSE);
  output_type_definition_part (type, (indirection_flag ? 1 : 0), FALSE);
}

/* The recursive function generates all C type definitions needed for
   declaration of fields which are accessible from RECORD_ELEMENT (see also
   commentaries for generate_necessary_type_definitions). */

static void
generate_necessary_field_type_definitions (record_element)
     register ICN_pointer record_element;
{
  register ICN_pointer variant;
  register enum icode_node_mode record_element_mode;

  for (;;)
    {
      record_element_mode = MODE (record_element);
      if (record_element_mode == ICNM_FIELD)
	{
	  generate_necessary_type_definitions
	    (FIELD_NODE (record_element)->field_type);
	  record_element = FIELD_NODE (record_element)->next_field;
	}
      else if (record_element_mode == ICNM_RECORD_CASE_PART)
	{
	  for (variant = (RECORD_CASE_PART_NODE (record_element)
			  ->record_case_variant_list);
	       MODE (variant) == ICNM_RECORD_CASE_VARIANT;
	       variant = RECORD_CASE_VARIANT_NODE (variant)->next_case_variant)
	    generate_necessary_field_type_definitions
	      (RECORD_CASE_VARIANT_NODE (variant)->case_variant_elements);
	  record_element = (RECORD_CASE_PART_NODE (record_element)
			    ->record_element_after_case_part);
	}
      else
	break;
    }
}

/* The function generates all declarations of all fields which are accessible
   from RECORD_ELEMENT (see also commentaries for record_case_part_name_prefix
   and record_case_variant_name_prefix).  The C structures which implements
   Modula-2 record case variants are generated here. */

static void
generate_field_declarations (record_element)
     register ICN_pointer record_element;
{
  register ICN_pointer variant, saved_record_element;
  register enum icode_node_mode record_element_mode;
  register int case_number, variant_number;

  case_number = 0;
  saved_record_element = record_element;
  /* Fix number of case parts on current level. */
  for (;;)
    {
      record_element_mode = MODE (record_element);
      if (record_element_mode == ICNM_FIELD)
	record_element = FIELD_NODE (record_element)->next_field;
      else if (record_element_mode == ICNM_RECORD_CASE_PART)
	{
	  case_number++;
	  record_element = (RECORD_CASE_PART_NODE (record_element)
			    ->record_element_after_case_part);
	}
      else
	break;
    }
  record_element = saved_record_element;
  /* Generate declarations of fields. */
  for (;;)
    {
      record_element_mode = MODE (record_element);
      if (record_element_mode == ICNM_FIELD)
	{
	  generate_declaration_by_object
	    (FIELD_NODE (record_element)->field_type, FALSE, record_element);
	  if (fputs (";\n", output_file) == EOF)
	    output_file_error ();
	  record_element = FIELD_NODE (record_element)->next_field;
	}
      else if (record_element_mode == ICNM_RECORD_CASE_PART)
	{
	  if (fputs ("union{", output_file) == EOF)
	    output_file_error ();
	  variant_number = 0;
	  /* Fix number of variants of current case part. */
	  for (variant = (RECORD_CASE_PART_NODE (record_element)
			  ->record_case_variant_list);
	       MODE (variant) == ICNM_RECORD_CASE_VARIANT;
	       variant = RECORD_CASE_VARIANT_NODE (variant)->next_case_variant)
	    variant_number++;
	  /* Generate structures for each variant. */
	  for (variant = (RECORD_CASE_PART_NODE (record_element)
			  ->record_case_variant_list);
	       MODE (variant) == ICNM_RECORD_CASE_VARIANT;
	       variant = RECORD_CASE_VARIANT_NODE (variant)->next_case_variant)
	    {
	      if (fputs ("struct{", output_file) == EOF)
		output_file_error ();
	      generate_field_declarations
		(RECORD_CASE_VARIANT_NODE (variant)->case_variant_elements);
	      if (fprintf (output_file, "}%s%x;\n",
			   record_case_variant_name_prefix,
			   (unsigned) variant_number--) < 0)
		output_file_error ();
	    }
	  if (fprintf (output_file, "}%s%x;\n", record_case_part_name_prefix,
		       (unsigned) case_number--) < 0)
	    output_file_error ();
	  record_element = (RECORD_CASE_PART_NODE (record_element)
			    ->record_element_after_case_part);
	}
      else
	break;
    }
}

/* The function outputs name of formal parameter (if FORMAL_PARAMETER_TYPE is
   type of this formal parameter) or anonim_parameter_type_name_prefix
   (if corresponding formal parameter does not exist, e.g. in procedure
   type definition).  FORMAL_PARAMETER_TYPE is to refer to formal parameter or
   procedure parameter type node (see commentaries for icode). */

static
void
output_formal_parameter_type_name (formal_parameter_type)
     ICN_pointer formal_parameter_type;
{
  if (MODE (formal_parameter_type) != ICNM_FORMAL_PARAMETER)
    {
      if (fprintf (output_file, "%s%lx", anonim_parameter_type_name_prefix,
		   (unsigned long) formal_parameter_type) < 0)
	output_file_error ();
    }
  else
    output_object_full_name (formal_parameter_type, FALSE);
}

/* The function outputs name of C structure which contains parameters of
   procedure of type PROCEDURE_TYPE.  The name is par_prefix followed by
   procedure name (if PROCEDURE_TYPE is procedure type of this procedure) or
   anonim_procedure_type_name_prefix (if corresponding procedure does not exist
   , e.g. in procedure type definition).  PROCEDURE_TYPE is to refer to type
   of procedure with parameters. */

static
void
output_parameter_structure_name (procedure_type)
     register ICN_pointer procedure_type;
{
  register ICN_pointer first_parameter_type;

  first_parameter_type = next_parameter_type (procedure_type);
  if (first_parameter_type == NULL)
    abort ();
  if (fprintf (output_file, "%s", par_prefix) < 0)
    output_file_error ();
  if (MODE (first_parameter_type) != ICNM_FORMAL_PARAMETER)
    {
      if (fprintf (output_file, "_%s%lx", anonim_procedure_type_name_prefix,
		   (unsigned long) procedure_type) < 0)
	output_file_error ();
    }
  else
    /* PROCEDURE_TYPE is procedure type of real procedure because
       first_parameter_type is represented by real formal parameter. */
    output_object_full_name (SCOPE (first_parameter_type), FALSE);
}

/* The function generates all C type definitions (if it has not been generated
   yet) needed for declaration of object of TYPE.  These definitions are
   structures for record type, for cyclic pointer type implemented by C struct
   and C structure which contains formal parameters of procedure type.
   TYPE is to be NULL or to refer to type definition. */

static void
generate_necessary_type_definitions (type)
     register ICN_pointer type;
{
  register enum icode_node_mode type_mode;
  register ICN_pointer formal_parameter;
  int first_parameter_was_processed;

  if (type == NULL)
    return;
  else
    {
      type_mode = MODE (type);
      if (type_mode == ICNM_ARRAY_TYPE)
	generate_necessary_type_definitions
	  (ARRAY_TYPE_NODE (type)->base_type);
      else if (type_mode == ICNM_PROCEDURE_TYPE)
	{
	  if (PROCEDURE_TYPE_NODE (type)
	      ->corresponding_C_parameter_structure_was_generated)
	    return;
	  /* It will be needed on the following passes. */
	  LAST_BLOCK_USING_PROCEDURE_TYPE (type) = 0;
	  PROCEDURE_TYPE_NODE (type)
	    ->corresponding_C_parameter_structure_was_generated = TRUE;
	  generate_necessary_type_definitions
	    (PROCEDURE_TYPE_NODE (type)->procedure_result_type);
	  for (formal_parameter = next_parameter_type (type);
	       formal_parameter != NULL;
	       formal_parameter = next_parameter_type (formal_parameter))
	    generate_necessary_type_definitions
	      (FORMAL_PARAMETER_TYPE (formal_parameter));
	  /* Generate C structure which contains formal parameters of given
	     procedure or procedure type. */
	  for (first_parameter_was_processed = FALSE,
	       formal_parameter = next_parameter_type (type);
	       formal_parameter != NULL;
	       formal_parameter = next_parameter_type (formal_parameter))
	    {
	      if (!first_parameter_was_processed)
		{
		  if (fputs ("struct ", output_file) == EOF)
		    output_file_error ();
		  output_parameter_structure_name (type);
		  if (fputc ('{', output_file) == EOF)
		    output_file_error ();
		  first_parameter_was_processed = TRUE;
		}
	      if (ARRAY_FORMAL_PARAMETER (formal_parameter))
		{
		  if (fputs ("struct{", output_file) == EOF)
		    output_file_error ();
		  generate_declaration_by_name
		    (FORMAL_PARAMETER_TYPE (formal_parameter), TRUE,
		     open_array_address_name, 0);
		  if (fprintf (output_file, ";%s %s;}",
			       cardinal_type_representation,
			       open_array_high_value_name) < 0)
		    output_file_error ();
		  output_formal_parameter_type_name (formal_parameter);
		  if (fputs (";\n", output_file) == EOF)
		    output_file_error ();
		}
	      else
		{
		  if (MODE (formal_parameter) != ICNM_FORMAL_PARAMETER)
		    generate_declaration_by_name
		      (FORMAL_PARAMETER_TYPE (formal_parameter),
		       VAR_FORMAL_PARAMETER (formal_parameter),
		       anonim_parameter_type_name_prefix,
		       (unsigned long) formal_parameter);
		  else
		    {
		      output_type_definition_part
			(FORMAL_PARAMETER_TYPE (formal_parameter),
			 (VAR_FORMAL_PARAMETER (formal_parameter) ? 1 : 0),
			 TRUE);
		      if (fputc (' ', output_file) == EOF)
			output_file_error ();
		      output_object_full_name (formal_parameter, FALSE);
		      output_type_definition_part
			(FORMAL_PARAMETER_TYPE (formal_parameter),
			 (VAR_FORMAL_PARAMETER (formal_parameter) ? 1 : 0),
			 FALSE);
		    }
		  if (fputs (";\n", output_file) == EOF)
		    output_file_error ();
		}
	    }
	  if (first_parameter_was_processed)
	    {
	      if (fputs ("};\n", output_file) == EOF)
		output_file_error ();
	    }
	}
      else if (type_mode == ICNM_POINTER_TYPE)
	{
	  if (!POINTER_TYPE_NODE (type)->pointer_is_being_processed)
	    {
	      POINTER_TYPE_NODE (type)->pointer_is_being_processed = TRUE;
	      generate_necessary_type_definitions (POINTER_TYPE_NODE (type)
						   ->base_type);
	      POINTER_TYPE_NODE (type)->pointer_is_being_processed = FALSE;
	    }
	  /* This is cyclic pointer type which is implemented by C struct. */
	  else if (!(POINTER_TYPE_NODE (type)
		     ->corresponding_C_structure_was_generated))
	    {
	      POINTER_TYPE_NODE (type)->corresponding_C_structure_was_generated
		= TRUE;
	      if (fprintf (output_file, "struct %s%lx{",
			   pointer_structure_name_prefix,
                           (unsigned long) type) < 0)
		output_file_error ();
	      generate_declaration_by_name
		(POINTER_TYPE_NODE (type)->base_type, TRUE,
		 pointer_member_name, 0);
	      if (fputs (";};\n", output_file) == EOF)
		output_file_error ();
	    }
	}
      else if (type_mode == ICNM_RECORD
	       && !RECORD_NODE (type)->corresponding_C_structure_was_generated)
	{
	  RECORD_NODE (type)->corresponding_C_structure_was_generated = TRUE;
	  generate_necessary_field_type_definitions
	    (RECORD_NODE (type)->record_element_list);
	  if (fputs ("struct ", output_file) == EOF)
	    output_file_error ();
	  if (RECORD_NODE (type)->record_type_identifier != NULL)
	    output_object_full_name
	      (RECORD_NODE (type)->record_type_identifier, FALSE);
	  else
	    {
	      if (fprintf (output_file, "%s%lx", anonim_record_name_prefix,
			   (unsigned long) type) < 0)
		output_file_error ();
	    }
	  if (fputc ('{', output_file) == EOF)
	    output_file_error ();
	  generate_field_declarations
	    (RECORD_NODE (type)->record_element_list);
	  if (fputs ("};\n", output_file) == EOF)
	    output_file_error ();
	}
    }
}

/* The function returns TRUE if DENOTATION contains procedure.  Generally
   DENOTATION is a procedure but it may refer to any denotation. */

static int
it_has_nested_procedures (denotation)
     register ICN_pointer denotation;
{
  register ICN_pointer declaration_element;

  for (declaration_element = NEXT_DECLARATION_ELEMENT (denotation);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_MODULE
	|| MODE (declaration_element) == ICNM_PROCEDURE)
      {
	if (!it_is_parent (denotation, declaration_element))
	  break;
	else if (MODE (declaration_element) == ICNM_PROCEDURE
		 && block_code_may_be_used (declaration_element))
	  return TRUE;
      }
  return FALSE;
}

/* The recursive function generates declarations of all variables of all
   local modules in SCOPE at the same declaration level (only procedures or
   compilation unit define new declaration level).  SCOPE is to be procedure or
   module (originally only compilation unit). */

static void
generate_variable_declarations_of_local_modules (scope)
     ICN_pointer scope;
{
  register ICN_pointer local_module, variable;

  for (local_module = first_son_module (scope);
       local_module != NULL;
       local_module = next_brother_module (local_module))
    if (MODE (local_module) == ICNM_MODULE)
      {
	for (variable = next_variable_declaration (local_module, local_module);
	     variable != NULL;
	     variable = next_variable_declaration (local_module, variable))
	  {
	    generate_declaration_by_object
	      (VARIABLE_DECLARATION_NODE (variable)->variable_type, FALSE,
	       variable);
	    if (fputs (";\n", output_file) == EOF)
	      output_file_error ();
	  }
	generate_variable_declarations_of_local_modules (local_module);
      }
}

/* The function returns type the representation of which is to be result type
   of generated C function.  The result type is to be representation of pointer
   to the type if INDIRECTION_IS_NEEDED is TRUE after the call.  If the type
   is NULL then the result type is to be void.  The latter case occurs when
   the procedure is proper procedure or function procedure with array or
   record result type.  PROCEDURE is to refer to procedure. */

static ICN_pointer
get_C_function_type (procedure, indirection_is_needed)
     register ICN_pointer procedure;
     register int *indirection_is_needed;
{
  register ICN_pointer result_type;

  if (MODE (procedure) != ICNM_PROCEDURE)
    abort ();
  result_type
    = PROCEDURE_TYPE_NODE (PROCEDURE_NODE (procedure)->procedure_type)
    ->procedure_result_type;
  *indirection_is_needed = FALSE;
  if (result_type == NULL || MODE (result_type) == ICNM_ARRAY_TYPE
      || MODE (result_type) == ICNM_RECORD)
    return NULL;
  else if (MODE (result_type) == ICNM_POINTER_TYPE
	   && (POINTER_TYPE_NODE (result_type)
	       ->corresponding_C_structure_was_generated))
    {
      *indirection_is_needed = TRUE;
      return POINTER_TYPE_NODE (result_type)->base_type;
    }
  else
    return result_type;
}

/* The function generates interface of COMPILATION_UNIT.  The interface
   contains extern declarations of exported procedures (if
   IT_IS_IMPORTED_MODULE is TRUE) and C struture which contains exported
   variables of the compilation unit (and internal variables if
   IT_IS_IMPORTED_MODULE is FALSE).  COMPILATION_UNIT is to refer to a
   compilation unit. */

static void
generate_compilation_unit_interface (compilation_unit, it_is_imported_module)
     register ICN_pointer compilation_unit;
     int it_is_imported_module;
{
  register ICN_pointer declaration_element, C_type, object, variable;
  int indirection_is_needed;

  if (it_is_imported_module)
    /* Generate extern declarations of functions exported by the compilation
       unit. */
    for (declaration_element = compilation_unit;
	 declaration_element != NULL;
	 declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
      {
	if (IN_DEFINITION_MODULE (declaration_element))
	  object
	    = find_identifier_declaration (compilation_unit,
					   IDENTIFIER (declaration_element));
	if (IN_DEFINITION_MODULE (declaration_element)
	    && object != NULL
	    && MODE (object) == ICNM_PROCEDURE
	    && block_code_may_be_used (object)
	    && (!it_is_imported_module
		|| PROCEDURE_NODE (object)->procedure_is_used))
	  {
	    if (fputs ("extern ", output_file) == EOF)
	      output_file_error ();
	    C_type = get_C_function_type (object, &indirection_is_needed);
	    output_type_definition_part
	      (C_type, (indirection_is_needed ? 1 : 0), TRUE);
	    if (fputc (' ', output_file) == EOF)
	      output_file_error ();
	    output_object_full_name (object, FALSE);
	    if (fputs ("()", output_file) == EOF)
	      output_file_error ();
	    output_type_definition_part
	      (C_type, (indirection_is_needed ? 1 : 0), FALSE);
	    if (fputs (";\n", output_file) == EOF)
	      output_file_error ();
	  }
      }
  /* Generate sructure containing th declarations of variables exported by
     the compilation unit. */
  if (declaration_level_scope_has_variable_declarations (compilation_unit))
    {
      if (it_is_imported_module)
	{
	  if (fputs ("extern ", output_file) == EOF)
	    output_file_error ();
	}
      if (fputs ("struct {", output_file) == EOF)
	output_file_error ();
      for (declaration_element = compilation_unit;
	   declaration_element != NULL;
	   declaration_element
	   = NEXT_DECLARATION_ELEMENT (declaration_element))
	if (IN_DEFINITION_MODULE (declaration_element)
	    && MODE (declaration_element) == ICNM_VARIABLE_DECLARATION)
	  {
	    generate_declaration_by_object
	      (VARIABLE_DECLARATION_NODE (declaration_element)->variable_type,
	       FALSE, declaration_element);
	    if (fputs (";\n", output_file) == EOF)
	      output_file_error ();
	  }
      if (!it_is_imported_module)
	{
	  /* Generate the structure members which implement non-exported
	     (internal) variables of the compilation unit. */
	  for (variable = next_variable_declaration (compilation_unit,
						     compilation_unit);
	       variable != NULL;
	       variable = next_variable_declaration (compilation_unit,
						     variable))
	    if (!IN_DEFINITION_MODULE (variable))
	      {
		generate_declaration_by_object
		  (VARIABLE_DECLARATION_NODE (variable)->variable_type,
		   FALSE, variable);
		if (fputs (";\n", output_file) == EOF)
		  output_file_error ();
	      }
	  generate_variable_declarations_of_local_modules (compilation_unit);
	}
      if (fputc ('}', output_file) == EOF)
	output_file_error ();
      output_object_full_name (compilation_unit, TRUE);
      if (fputs (";\n", output_file) == EOF)
	output_file_error ();
    }
}

/* The function generates all declarations defined in COMPILATION_UNIT
   (needed type definitions, C code fragments in declaration parts, variables
   and C union used for implementation of standard procedure ABS).
   COMPILATION_UNIT is to refer to a compilation unit. */

static void
generate_all_compilation_unit_declarations (compilation_unit)
     register ICN_pointer compilation_unit;
{
  register ICN_pointer declaration_element, variable, type;
  enum icode_node_mode declaration_element_mode;

  /* Generate type definitions of the compilation unit objects
     and output C code fragments from the compilation unit declarations. */
  for (declaration_element = NEXT_DECLARATION_ELEMENT (compilation_unit);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    {
      declaration_element_mode = MODE (declaration_element);
      if (declaration_element_mode == ICNM_VARIABLE_DECLARATION
	  || (declaration_element_mode == ICNM_PROCEDURE
	      && !IN_DEFINITION_MODULE (declaration_element)
	      && block_code_may_be_used (declaration_element))
	  || declaration_element_mode == ICNM_TYPE_DECLARATION)
	{
	  if (declaration_element_mode == ICNM_VARIABLE_DECLARATION)
	    type
	      = VARIABLE_DECLARATION_NODE (declaration_element)->variable_type;
	  else if (declaration_element_mode == ICNM_PROCEDURE)
	    type = PROCEDURE_NODE (declaration_element)->procedure_type;
	  else
	    type
	      = TYPE_DECLARATION_NODE (declaration_element)->type_definition;
	  generate_necessary_type_definitions (type);
	}
      else if (declaration_element_mode == ICNM_C_CODE_IN_DECLARATIONS)
	{
	  if (fprintf
	      (output_file, "\n%s\n",
	       C_CODE_IN_DECLARATIONS_NODE (declaration_element)->C_text) < 0)
	    output_file_error ();
	}
    }
  /* Generate C struture containing all variables on declaration level of
     the compilation unit. */
  if ((enum module_mode) MODULE_NODE (compilation_unit)->module_mode
      != MM_MAIN_MODULE)
    generate_compilation_unit_interface (compilation_unit, FALSE);
  else if (declaration_level_scope_has_variable_declarations
	   (compilation_unit))
    {
      /* It is main module, i.e. exported variables do not exist. */
      if (fputs ("struct {", output_file) == EOF)
	output_file_error ();
      for (variable = next_variable_declaration (compilation_unit,
						 compilation_unit);
	   variable != NULL;
	   variable = next_variable_declaration (compilation_unit, variable))
	{
	  generate_declaration_by_object
	    (VARIABLE_DECLARATION_NODE (variable)->variable_type, FALSE,
	     variable);
	  if (fputs (";\n", output_file) == EOF)
	    output_file_error ();
	}
      generate_variable_declarations_of_local_modules (compilation_unit);
      if (fputc ('}', output_file) == EOF)
	output_file_error ();
      output_object_full_name (compilation_unit, TRUE);
      if (fputs (";\n", output_file) == EOF)
	output_file_error ();
    }
  if (fputs ("extern char *m2_testptr();\n", output_file) == EOF)
    output_file_error ();
  /* Output C union used for implementation standard procedure ABS. */
  if ((enum module_mode) MODULE_NODE (compilation_unit)->module_mode
      != MM_MAIN_MODULE)
    {
      if (fputs ("extern ", output_file) == EOF)
	output_file_error ();
    }
  if (fprintf
      (output_file, "union{%s %s;%s %s;%s %s;\n%s %s;%s %s;%s %s;}%s;\n",
       shortint_type_representation,
       names_of_variables_for_abs_implementation[(int) BTM_SHORT_INTEGER],
       integer_type_representation,
       names_of_variables_for_abs_implementation[(int) BTM_INTEGER],
       longint_type_representation,
       names_of_variables_for_abs_implementation[(int) BTM_LONG_INTEGER],
       shortreal_type_representation,
       names_of_variables_for_abs_implementation[(int) BTM_SHORT_REAL],
       real_type_representation,
       names_of_variables_for_abs_implementation[(int) BTM_REAL],
       longreal_type_representation,
       names_of_variables_for_abs_implementation[(int) BTM_LONG_REAL],
       union_name_for_abs_implementation) < 0)
    output_file_error ();
}

/* The function generates (if it is necessary) all declarations defined in
   imported COMPILATION_UNIT (needed type definitions and exported variables).
   COMPILATION_UNIT is to refer to a compilation unit. */

static void
generate_all_imported_module_declarations (compilation_unit)
     register ICN_pointer compilation_unit;
{
  register ICN_pointer declaration_element, object;
  enum icode_node_mode declaration_element_mode;

  if (MODULE_NODE (compilation_unit)->module_declaration_was_generated)
    return;
  MODULE_NODE (compilation_unit)->module_declaration_was_generated = TRUE;
  for (declaration_element = compilation_unit;
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    {
      if (IN_DEFINITION_MODULE (declaration_element))
	object
	  = find_identifier_declaration
	  (compilation_unit, IDENTIFIER (declaration_element));
      if (IN_DEFINITION_MODULE (declaration_element) && object != NULL)
	{
	  declaration_element_mode = MODE (object);
	  if (declaration_element_mode == ICNM_VARIABLE_DECLARATION
	      || (declaration_element_mode == ICNM_PROCEDURE
		  && block_code_may_be_used (object)
		  && PROCEDURE_NODE (object)->procedure_is_used)
	      || declaration_element_mode == ICNM_TYPE_DECLARATION)
	    if (declaration_element_mode == ICNM_VARIABLE_DECLARATION)
	      generate_necessary_type_definitions
		(VARIABLE_DECLARATION_NODE (object)->variable_type);
	    else if (declaration_element_mode == ICNM_PROCEDURE)
	      generate_necessary_type_definitions
		(PROCEDURE_NODE (object)->procedure_type);
	    else
	      generate_necessary_type_definitions
		(TYPE_DECLARATION_NODE (object)->type_definition);
	}
    }
  generate_compilation_unit_interface (compilation_unit, TRUE);
}

/* The function generates various structures representing variables of
   PROCEDURE (see commentaries for var_prefix).  If IT_IS_WITHIN_PROCEDURE_BODY
   is TRUE than structures is to be in procedure body.  The function return
   FALSE if the procedure has not variables.  In this case the structures are
   not generated.  PROCEDURE is to refer to a procedure. */

static int
generate_structures_for_procedure_variables (procedure,
					     it_is_within_procedure_body)
     register ICN_pointer procedure;
     register int it_is_within_procedure_body;
{
  register ICN_pointer variable;
  int nested_procedures_exist;

  if (!declaration_level_scope_has_variable_declarations (procedure))
    return FALSE;
  nested_procedures_exist = it_has_nested_procedures (procedure);
  if (it_is_within_procedure_body && nested_procedures_exist)
    {
      if (fputs ("struct ", output_file) == EOF)
	output_file_error ();
      output_object_full_name (procedure, TRUE);
      if (fprintf (output_file, " %s, *%s;\n", var_prefix,
		   name_of_saved_pointer_to_procedure_variables) < 0)
	output_file_error ();
    }
  if ((it_is_within_procedure_body && !nested_procedures_exist)
      || (!it_is_within_procedure_body && nested_procedures_exist))
    {
      if (nested_procedures_exist)
	{
	  if (fprintf (output_file, "static struct ") < 0)
	    output_file_error ();
	  output_object_full_name (procedure, TRUE);
	  if (fputc ('{', output_file) == EOF)
	    output_file_error ();
	}
      else
	{
	  if (fputs ("struct{", output_file) == EOF)
	    output_file_error ();
	}
      for (variable = next_variable_declaration (procedure, procedure);
	   variable != NULL;
	   variable = next_variable_declaration (procedure, variable))
	{
	  generate_declaration_by_object
	    (VARIABLE_DECLARATION_NODE (variable)->variable_type, FALSE,
	     variable);
	  if (fputs (";\n", output_file) == EOF)
	    output_file_error ();
	}
      generate_variable_declarations_of_local_modules (procedure);
      if (!it_is_within_procedure_body)
	{
	  if (fputs ("}*", output_file) == EOF)
	    output_file_error ();
	  output_object_full_name (procedure, TRUE);
	  if (fputs (";\n", output_file) == EOF)
	    output_file_error ();
	}
      else
	{
	  if (fprintf (output_file, "}%s;\n", var_prefix) < 0)
	    output_file_error ();
	}
    }
  return TRUE;
}

/* The function generates various structures representing formal parameters of
   PROCEDURE (see commentaries for par_prefix).  If IT_IS_WITHIN_PROCEDURE_BODY
   is TRUE than structures is to be in procedure body.  PROCEDURE is to refer
   to a procedure. */

static void
generate_structures_for_procedure_parameters (procedure,
					      it_is_within_procedure_body)
     register ICN_pointer procedure;
     int it_is_within_procedure_body;
{
  register ICN_pointer procedure_type;

  procedure_type = PROCEDURE_NODE (procedure)->procedure_type;
  if (next_parameter_type (procedure_type) != NULL
      && it_has_nested_procedures (procedure))
    {
      if (!it_is_within_procedure_body)
	{
	  if (fputs ("static ", output_file) == EOF)
	    output_file_error ();
	}
      if (fputs ("struct ", output_file) == EOF)
	output_file_error ();
      output_parameter_structure_name (procedure_type);
      if (fputs (" *", output_file) == EOF)
	output_file_error ();
      if (it_is_within_procedure_body)
	{
	  if (fputs (name_of_saved_pointer_to_procedure_parameters,
		     output_file) == EOF)
	    output_file_error ();
	}
      else
	output_parameter_structure_name (procedure_type);
      if (fputs (";\n", output_file) == EOF)
	output_file_error ();
    }
}

/* The function generates all declarations corresponding to PROCEDURE (forward
   declaration of the procedure, structures for variables and parameters).
   PROCEDURE is to refer to a procedure. */

static void
generate_procedure_declarations (procedure)
     register ICN_pointer procedure;
{
  register ICN_pointer result_type;
  int indirection_is_needed;

  result_type = get_C_function_type (procedure, &indirection_is_needed);
  if (!procedure_is_exported_from_compilation_unit (procedure))
    {
      if (fputs ("static ", output_file) == EOF)
	output_file_error ();
    }
  output_type_definition_part (result_type, (indirection_is_needed ? 1 : 0),
			       TRUE);
  if (fputc (' ', output_file) == EOF)
    output_file_error ();
  output_object_full_name (procedure, FALSE);
  if (fputs ("()", output_file) == EOF)
    output_file_error ();
  output_type_definition_part (result_type, (indirection_is_needed ? 1 : 0),
			       FALSE);
  if (fputs (";\n", output_file) == EOF)
    output_file_error ();
  generate_structures_for_procedure_variables (procedure, FALSE);
  generate_structures_for_procedure_parameters (procedure, FALSE);
}

/* The function initiates data and generates all declarations needed for
   generation of CURRENT_COMPILATION_UNIT.  CURRENT_COMPILATION_UNIT is to be
   refer to the compiled unit. */

static void
generate_all_declarations (current_compilation_unit)
     register ICN_pointer current_compilation_unit;
{
  register ICN_pointer declaration_element, imported_module;

  current_block_number_among_blocks_with_actuals = 0;
  last_generated_line_number = 0;
  current_level_of_nesting_with_statements = 0;
  number_of_generated_labels = 0;
  number_of_generated_string_constant_declarations = 0;
  generate_all_compilation_unit_declarations (current_compilation_unit);
  for (declaration_element
       = NEXT_DECLARATION_ELEMENT (current_compilation_unit);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_IMPORT)
      {
	if (MODULE_EXPORTER (declaration_element) == NULL)
	  imported_module = find_denotation (ICN_POINTER (NULL),
					     IDENTIFIER (declaration_element));
	else
	  imported_module
	    = find_denotation (ICN_POINTER (NULL),
			       MODULE_EXPORTER (declaration_element));
	if (imported_module != NULL && MODE (imported_module) == ICNM_MODULE
	    && imported_module != ICN_POINTER (&ICN_D_system)
	    && imported_module != ICN_POINTER (&ICN_D_cproc))
	  generate_all_imported_module_declarations (imported_module);
      }
  for (declaration_element = current_compilation_unit;
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_PROCEDURE
	&& !IN_DEFINITION_MODULE (declaration_element)
	&& block_code_may_be_used (declaration_element))
      generate_procedure_declarations (declaration_element);
}



/* This page contains functions for generation of expression. */

/* The function generates line directive corresponding to source line of
   CONSTRUCTION (it is statement now).  The generation is executed only if
   statement generation pass is current and it is new line (see commentaries
   for last_generated_line_number). */

static void
generate_line_directive (construction)
     register ICN_pointer construction;
{
  if (it_is_statements_generation_pass
      && LINE (construction) != last_generated_line_number)
    {
      last_generated_line_number = LINE (construction);
      if (fprintf (output_file, "\n#line %d \"%s\"\n",
		   last_generated_line_number, input_file_name) < 0)
	output_file_error ();
    }
}

/* The function outputs C representation of Modula-2 constant.  CONSTANT is
   to refer to constant. */

static void
output_constant_value (constant)
     register ICN_pointer constant;
{
  register enum icode_node_mode constant_mode;

  constant_mode = MODE (constant);
  if (constant_mode == ICNM_INTEGER)
    {
      if (fprintf (output_file, (INTEGER_NODE (constant)->value_type
				 == ICN_POINTER (&ICN_TD_long_integer)
				 ? "%lldll" : "%lld"),
		   INTEGER_NODE (constant)->integer_value) < 0)
	output_file_error ();
    }
  else if (constant_mode == ICNM_CARDINAL)
    {
      if (it_is_integer_cardinal_type (CARDINAL_NODE (constant)->value_type))
	{
	  if (fprintf (output_file, "%lld",
		       CARDINAL_NODE (constant)->cardinal_value) < 0)
	    output_file_error ();
	}
      else if (CARDINAL_NODE (constant)->value_type
	       == ICN_POINTER (&ICN_TD_char))
	{
	  if (fprintf (output_file, "\'\\%llo\'",
		       CARDINAL_NODE (constant)->cardinal_value) < 0)
	    output_file_error ();
	}
      else
	{
	  if (fprintf (output_file, (CARDINAL_NODE (constant)->value_type
				     == ICN_POINTER (&ICN_TD_long_cardinal)
				     ? "(unsigned long long)0%lloll"
				     : "(unsigned)0%llo"),
		       CARDINAL_NODE (constant)->cardinal_value) < 0)
	    output_file_error ();
	}
    }
  else if (constant_mode == ICNM_REAL)
    {
      if (fprintf (output_file, "%.20e", REAL_NODE (constant)->real_value) < 0)
	output_file_error ();
    }
  else if (constant_mode == ICNM_STRING)
    {
      if (fprintf (output_file, "(%s *)%s%x",
		   char_type_representation, string_name_prefix,
		   (unsigned) C_STRING_CONSTANT_NUMBER (constant)) < 0)
	output_file_error ();
    }
  else if (constant_mode == ICNM_SET)
    {
      if (fprintf (output_file, "0%o", SET_NODE (constant)->set_value) < 0)
	output_file_error ();
    }
}

/* The function generates C function for test that value belongs to range or
   enumeration.  The generation is executed only if test flag is on, first pass
   (string constants declarations generation) is current and TYPE is type of
   range or enumeration.  The C function for given type is generated only once.
   TYPE is to be not NULL. */

static void
generate_test_function (type)
     register ICN_pointer type;
{
  register char *str;

  if (test_flag
      &&
      current_generator_pass == STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS
      && ((MODE (type) == ICNM_RANGE_TYPE
	   && !RANGE_TYPE_NODE (type)->range_test_function_was_generated)
	  || (MODE (type) == ICNM_ENUMERATION_TYPE
	      && !(ENUMERATION_TYPE_NODE (type)
		   ->enumeration_value_test_function_was_generated))))
    {
      if (MODE (type) == ICNM_RANGE_TYPE)
	RANGE_TYPE_NODE (type)->range_test_function_was_generated = TRUE;
      else
	(ENUMERATION_TYPE_NODE (type)
	 ->enumeration_value_test_function_was_generated) = TRUE;
      str = discrete_type_representation (type);
      if (fprintf (output_file, "static %s %s%lx(e)register %s e;{\n",
		   str, range_test_function_name_prefix, (unsigned long) type,
                   str)
	  < 0)
	output_file_error ();
      if (fputs ("if(", output_file) == EOF)
	output_file_error ();
      output_constant_value (min_or_max (type, TRUE));
      if (fputs ("<=", output_file) == EOF)
	output_file_error ();
	{
	  if (fputs ("e && e", output_file) == EOF)
	    output_file_error ();
	}
      if (fputs ("<=", output_file) == EOF)
	output_file_error ();
      output_constant_value (min_or_max (type, FALSE));
      if (fputs (")return e;\nm2_rngovf();}\n", output_file) == EOF)
	output_file_error ();
    }
}

/* The following function generates EXPRESSION according to FLAGS and possibly
   with the call of range test function.  The call of procedure for test of
   value is generated only when the test flag is on, the expression is not
   constant and EXPRESSION_TYPE is of range type.  The expression itself
   (possibly with the test procedure call) is generated only on statement
   generation pass.  On the other pass the processing expression may generates
   various structures (see commentaries for generator_pass).  EXPRESSION is
   to represent expression. */

static void
generate_expression_with_test (expression_type, expression, flags)
     register ICN_pointer expression_type, expression;
     int flags;
{
  semantic_information sinf;

  if (test_flag && MODE (expression_type) == ICNM_RANGE_TYPE)
    {
      subgraph_is_expression (expression, &sinf);
      if (!sinf.it_is_constant)
	{
	  generate_test_function (expression_type);	/*only for range type*/
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file, "%s%lx((%s)(",
			   range_test_function_name_prefix,
			   (unsigned long) expression_type,
			   discrete_type_representation (expression_type)) < 0)
		output_file_error ();
	    }
	}
    }
  generate_expression_according_to_flags (expression, flags);
  if (test_flag && MODE (expression_type) == ICNM_RANGE_TYPE
      && it_is_statements_generation_pass && !sinf.it_is_constant)
    {
      if (fputs ("))", output_file) == EOF)
	output_file_error ();
    }
}

/* String constant which is longer then MAX_STRING_PIECE_ON_A_LINE is
   divided on several lines. */

#define MAX_STRING_PIECE_ON_A_LINE 45

/* The following function generates EXPRESSION. The expression itself is
   generated only on statement generation pass.  On the other pass
   the processing expression may generates various structures (see
   commentaries for generator_pass).  EXPRESSION is to represent expression. */

static void
generate_expression (expression)
     register ICN_pointer expression;
{
  register ICN_pointer ic_ptr;
  register enum icode_node_mode expression_mode;
  register int n, it_is_set_operation, string_char;
  int align;
  Tcard size;
  char *operation_representation;
  Tcard min_index;
  semantic_information sinf;

  if (expression == NULL)
    abort ();
  expression_mode = MODE (expression);
  if (expression_mode != ICNM_PROCEDURE)
    output_string_when_statements_generation ("(");
  switch (expression_mode)
    {
    case ICNM_INTEGER:
    case ICNM_CARDINAL:
    case ICNM_REAL:
    case ICNM_SET:
      if (it_is_statements_generation_pass)
	output_constant_value (expression);
      break;
    case ICNM_STRING:
      if (it_is_statements_generation_pass)
	output_constant_value (expression);
      else if ((current_generator_pass
		== STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS)
	       && !STRING_NODE (expression)->string_declaration_was_generated)
	{
	  STRING_NODE (expression)->string_declaration_was_generated = TRUE;
	  C_STRING_CONSTANT_NUMBER (expression)
	    = ++number_of_generated_string_constant_declarations;
	  if (fprintf (output_file, "static char %s%x[]=\"",
		       string_name_prefix,
		       (unsigned) C_STRING_CONSTANT_NUMBER (expression)) < 0)
	    output_file_error ();
	  for (n = 0;; n++)
	    {
	      string_char
		= CHAR_TO_INT (STRING_NODE (expression)->string_value[n]);
	      if (string_char == '\0')
		break;
	      if ((n + 1) % MAX_STRING_PIECE_ON_A_LINE == 0)
		{
		  if (fputs ("\\\n", output_file) == EOF)
		    output_file_error ();
		}
	      if (fprintf (output_file,
			   (isprint (string_char) && string_char != '\''
			    && string_char != '\"' && string_char != '\\'
			    ? "%c"
			    : (string_char < 8
			       ? "\\00%o"
			       : (string_char < 64 ? "\\0%o" : "\\%o"))),
			   (unsigned int) string_char) < 0)
		output_file_error ();
	    }
	  if (fputs ("\";\n", output_file) == EOF)
	    output_file_error ();
	}
      break;
    case ICNM_VARIABLE_DECLARATION:
      if (it_is_statements_generation_pass)
	{
	  ic_ptr = declaration_level_scope (expression);
	  if (MODE (ic_ptr) == ICNM_PROCEDURE)
	    {
	      if (ic_ptr == procedure_over_denotation (current_scope))
		{
		  if (fprintf (output_file, "%s.", var_prefix) < 0)
		    output_file_error ();
		}
	      else
		{
		  output_object_full_name (ic_ptr, TRUE);
		  if (fputs ("->", output_file) == EOF)
		    output_file_error ();
		}
	    }
	  else
	    {
	      output_object_full_name (ic_ptr, TRUE);
	      if (fputc ('.', output_file) == EOF)
		output_file_error ();
	    }
	  output_object_full_name (expression, FALSE);
	}
      break;
    case ICNM_FORMAL_PARAMETER:
      if (it_is_statements_generation_pass)
	{
	  ic_ptr = PARAMETER_TYPE_OF_FORMAL_PARAMETER (expression);
	  if (VAR_FORMAL_PARAMETER (ic_ptr)
	      && !ARRAY_FORMAL_PARAMETER (ic_ptr))
	    {
	      if (fputc ('*', output_file) == EOF)
		output_file_error ();
	    }
	  ic_ptr = PROCEDURE_NODE (SCOPE (expression))->procedure_type;
	  if (SCOPE (expression) == procedure_over_denotation (current_scope))
	    {
	      if (fputs (par_prefix, output_file) == EOF)
		output_file_error ();
	    }
	  else
	    output_parameter_structure_name (ic_ptr);
	  if (fputs ("->", output_file) == EOF)
	    output_file_error ();
	  output_formal_parameter_type_name (expression);
	}
      break;
    case ICNM_PROCEDURE:
      if (it_is_statements_generation_pass)
	output_object_full_name (expression, FALSE);
      break;
    case ICNM_FIELD:
      if (it_is_statements_generation_pass)
	{
	  for (n = 0, ic_ptr = current_scope;
	       SCOPE (expression) != ic_ptr; ic_ptr = SCOPE (ic_ptr))
	    n++;
	  if (fprintf
	      (output_file, "(*%s%x)", with_variable_name_prefix,
	       (unsigned) (current_level_of_nesting_with_statements - n)) < 0)
	    output_file_error ();
	  generate_field_access (expression);
	}
      break;
    case ICNM_LESS_THAN:
    case ICNM_GREATER_THAN:
    case ICNM_OR:
    case ICNM_AND:
    case ICNM_REAL_DIVISION:
    case ICNM_ADDITION:
    case ICNM_SUBTRACTION:
    case ICNM_MULT:
    case ICNM_DIV:
    case ICNM_MOD:
      subgraph_is_expression (LEFT_OPERAND (expression), &sinf);
      it_is_set_operation = MODE (sinf.type) == ICNM_SET_TYPE;
      GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER (LEFT_OPERAND (expression));
      if (expression_mode == ICNM_LESS_THAN)
	operation_representation = "<";
      else if (expression_mode == ICNM_GREATER_THAN)
	operation_representation = ">";
      else if (expression_mode == ICNM_OR)
	operation_representation = "||";
      else if (expression_mode == ICNM_AND)
	operation_representation = "&&";
      else if (expression_mode == ICNM_REAL_DIVISION)
	operation_representation = (it_is_set_operation ? "^" : "/");
      else if (expression_mode == ICNM_ADDITION)
	operation_representation = (it_is_set_operation ? "|" : "+");
      else if (expression_mode == ICNM_SUBTRACTION)
	operation_representation = (it_is_set_operation ? "&~" : "-");
      else if (expression_mode == ICNM_MULT)
	operation_representation = (it_is_set_operation ? "&" : "*");
      else if (expression_mode == ICNM_DIV)
	operation_representation = "/";
      else
	operation_representation = "%";
      output_string_when_statements_generation (operation_representation);
      GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER (RIGHT_OPERAND (expression));
      break;
    case ICNM_EQUAL:
    case ICNM_UNEQUAL:
      subgraph_is_expression (LEFT_OPERAND (expression), &sinf);
      if (MODE (sinf.type) == ICNM_ARRAY_TYPE
	  || MODE (sinf.type) == ICNM_RECORD
	  || (it_is_string_type (sinf.type)
	      && (subgraph_is_expression (RIGHT_OPERAND (expression), &sinf),
		  it_is_string_type (sinf.type))))
	{
	  output_string_when_statements_generation
	    ((expression_mode == ICNM_EQUAL ? "m2_eq(" : "m2_ne("));
	  if (MODE (sinf.type) == ICNM_RECORD)
	    output_string_when_statements_generation ("&");
	  GEN_ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER
	    (LEFT_OPERAND (expression));
	  output_string_when_statements_generation
	    ((MODE (sinf.type) == ICNM_RECORD ? ",&" : ","));
	  GEN_ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER
	    (RIGHT_OPERAND (expression));
	  if (it_is_statements_generation_pass)
	    {
	      get_type_size_and_alignment (sinf.type, &size, &align);
	      if (fprintf (output_file, ",0%llo)", size / BYTE_SIZE) < 0)
		output_file_error ();
	    }
	}
      else
	{
	  GEN_ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER
	    (LEFT_OPERAND (expression));
	  output_string_when_statements_generation
	    ((expression_mode == ICNM_EQUAL ? "==" : "!="));
	  GEN_ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER
	    (RIGHT_OPERAND (expression));
	}
      break;
    case ICNM_LESS_OR_EQUAL:
    case ICNM_GREATER_OR_EQUAL:
      subgraph_is_expression (LEFT_OPERAND (expression), &sinf);
      if (MODE (sinf.type) == ICNM_SET_TYPE)
	{
	  output_string_when_statements_generation
	    ((expression_mode == ICNM_LESS_OR_EQUAL
	      ? "m2_setin(" : "m2_nites("));
	  GEN_ADDRESS_AS_CARDINAL (LEFT_OPERAND (expression));
	  output_string_when_statements_generation (",");
	  GEN_ADDRESS_AS_CARDINAL (RIGHT_OPERAND (expression));
	  output_string_when_statements_generation (")");
	}
      else
	{
	  GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER (LEFT_OPERAND (expression));
	  output_string_when_statements_generation
	    ((expression_mode == ICNM_LESS_OR_EQUAL ? "<=" : ">="));
	  GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER (RIGHT_OPERAND (expression));
	}
      break;
    case ICNM_IN:
      output_string_when_statements_generation ("in(");
      GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER (LEFT_OPERAND (expression));
      output_string_when_statements_generation (",");
      GEN_ADDRESS_AS_CARDINAL (RIGHT_OPERAND (expression));
      output_string_when_statements_generation (")");
      break;
    case ICNM_SIGN_CONVERSION:
    case ICNM_NOT:
      output_string_when_statements_generation
	((expression_mode == ICNM_SIGN_CONVERSION ? "-" : "!"));
      GEN_ADDRESS_AS_CARDINAL (OPERAND (expression));
      break;
    case ICNM_DEREFERENCING:
      output_string_when_statements_generation ("*");
      if (test_flag && it_is_statements_generation_pass)
	{
	  if (fputc ('(', output_file) == EOF)
	    output_file_error ();
	  output_type_definition_part
	    (DEREFERENCING_NODE (expression)->value_type, 1, TRUE);
	  output_type_definition_part
	    (DEREFERENCING_NODE (expression)->value_type, 1, FALSE);
	  if (fputs (")m2_testptr(", output_file) == EOF)
	    output_file_error ();
	}
      GEN_THROUGH_STRUCT_POINTER (OPERAND (expression));
      if (test_flag && it_is_statements_generation_pass)
	{
	  if (fputc (')', output_file) == EOF)
	    output_file_error ();
	}
      break;
    case ICNM_QUALIFICATION:
      generate_expression (LEFT_OPERAND (expression));
      if (it_is_statements_generation_pass)
	generate_field_access (RIGHT_OPERAND (expression));
      break;
    case ICNM_INDEXING:
      subgraph_is_expression (LEFT_OPERAND (expression), &sinf);
      generate_expression (LEFT_OPERAND (expression));
      operation_representation = "-";
      if (it_is_statements_generation_pass)
	{
	  if (sinf.it_is_array_parameter)
	    {
	      min_index = 0;
	      if (fprintf (output_file, ".%s", open_array_address_name) < 0)
		output_file_error ();
	    }
	  else
	    {
	      ic_ptr
		= min_or_max (ARRAY_TYPE_NODE (sinf.type)->array_index_type,
			      TRUE);
	      if (MODE (ic_ptr) == ICNM_INTEGER)
		if (INTEGER_NODE (ic_ptr)->integer_value < 0)
		  {
		    operation_representation = "+";
		    min_index = (-INTEGER_NODE (ic_ptr)->integer_value);
		  }
		else
		  min_index = INTEGER_NODE (ic_ptr)->integer_value;
	      else
		min_index = CARDINAL_NODE (ic_ptr)->cardinal_value;
	    }
	}
      output_string_when_statements_generation ("[");
      generate_expression_with_test
	(ARRAY_TYPE_NODE (sinf.type)->array_index_type,
	 RIGHT_OPERAND (expression), ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE);
      if (it_is_statements_generation_pass)
	{
	  if (min_index == 0)
	    {
	      if (fputc (']', output_file) == EOF)
		output_file_error ();
	    }
	  else
	    {
	      if (fprintf (output_file, "%s0%llo]", operation_representation,
			   min_index) < 0)
		output_file_error ();
	    }
	}
      break;
    case ICNM_ELEMENT_IN_SET_VALUE:
      if (it_is_statements_generation_pass)
	{
	  if (fprintf (output_file, "(%s)1<<", set_type_representation) < 0)
	    output_file_error ();
	}
      generate_expression_with_test
	(SET_TYPE_NODE (VALUE_TYPE (expression))->base_type,
	 OPERAND (expression), ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE);
      break;
    case ICNM_RANGE_IN_SET_VALUE:
      if (it_is_statements_generation_pass)
	{
	  if (fprintf (output_file, "~(%s)0<<", set_type_representation) < 0)
	    output_file_error ();
	}
      generate_expression_with_test
	(SET_TYPE_NODE (VALUE_TYPE (expression))->base_type,
	 LEFT_OPERAND (expression), ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE);
      if (it_is_statements_generation_pass)
	{
	  if (fprintf (output_file, "&~(%s)0>>%d-", set_type_representation,
		       BITS_IN_SET - 1) < 0)
	    output_file_error ();
	}
      generate_expression_with_test
	(SET_TYPE_NODE (VALUE_TYPE (expression))->base_type,
	 RIGHT_OPERAND (expression), ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE);
      break;
    case ICNM_FUNCTION_CALL:
      generate_call (expression);
      break;
    default:
      abort ();
    }
  if (expression_mode != ICNM_PROCEDURE)
    output_string_when_statements_generation (")");
}



/* The page contains function for generation of call and other functions
   corresponding to temporary variables (the variables may be used for the
   generation of the call). */


/* The function finishes generation of structure which contains temporary
   variables needed for current expression.  The generation is executed only
   on temporary variables declaration generation pass and when the current
   expression really needs temporary variables.  The function also counts
   number of temporary variables in block. The function is to be called
   after processing all expression. */

static void
finish_expression ()
{
  if (current_generator_pass == TEMPORARY_VARIABLES_DECLARATION_GENERATION_PASS
      && current_expression_needs_temp_variables)
    {
      if (fprintf (output_file, "}%s%x;\n",
		   temp_expression_variables_names_prefix,
		   (unsigned) number_of_expression_with_temp_variables) < 0)
	output_file_error ();
    }
  number_of_expression_with_temp_variables++;
  current_expression_needs_temp_variables = FALSE;
}


/* The function generate of declaration of temporary variable of TYPE (see
   commentaries for temp_variable_name_prefix).  The temporary variables of
   expression are contained in C structure.  The generation is executed only
   on temporary variables declaration generation pass (see commentaries for
   generator_pass).  The call of the function on string constants declarations
   generation pass results in necessity of pass of temporary variable
   declaration generation.  TYPE is to refer to type definition. */

static void
generate_temp_variable_declaration (type)
     register ICN_pointer type;
{
  if (current_generator_pass == STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS)
    temporary_variables_declaration_generation_is_needed = TRUE;
  else if (current_generator_pass
	   == TEMPORARY_VARIABLES_DECLARATION_GENERATION_PASS)
    {
      if (!current_expression_needs_temp_variables)
	{
	  if (fputs ("struct{", output_file) == EOF)
	    output_file_error ();
	}
      current_expression_needs_temp_variables = TRUE;
      generate_declaration_by_name
	(through_integer_cardinal_type (type), FALSE,
	 temp_variable_name_prefix,
	 (unsigned int) number_of_current_temporary_variable);
      if (fputs (";\n", output_file) == EOF)
	output_file_error ();
    }
}

/* The following function generates function or procedure call.
   The call itself is generated only on statement generation pass.  On
   the other pass the processing call may generate various structures
   (see commentaries for generator_pass).  CALL is to refer to function
   or procedure call. */

static void
generate_call (call)
     register ICN_pointer call;
{
  register ICN_pointer actual, procedure_designator, formal, first_actual;
  register ICN_pointer procedure_type, result_type;
  enum icode_node_mode mode;
  int struct_for_temp_vars_is_needed, fltemp, it_is_designator;
  int it_is_local_procedure, it_may_be_used_for_coroutine;
  Tcard formal_size, actual_size;
  int align;
  semantic_information sinf;

  if (MODE (call) == ICNM_PROCEDURE_CALL)
    {
      procedure_designator = PROCEDURE_CALL_NODE (call)->procedure_designator;
      first_actual
	= PROCEDURE_CALL_NODE (call)->procedure_actual_parameter_list;
    }
  else
    {
      procedure_designator = FUNCTION_CALL_NODE (call)->function_designator;
      first_actual = FUNCTION_CALL_NODE (call)->function_actual_parameter_list;
    }
  number_of_current_call++;
  mode = MODE (procedure_designator);
  if (mode == ICNM_C_FUNCTION)
    /* Call of C function. */
    {
      if (it_is_statements_generation_pass)
	{
	  if (fprintf (output_file, "%s(",
		       IDENTIFIER_NODE (IDENTIFIER (procedure_designator))
		       ->identifier_string) < 0)
	    output_file_error ();
	}
      for (actual = first_actual;
	   actual != NULL;
	   actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
	{
	  if (actual != first_actual && it_is_statements_generation_pass)
	    output_string_when_statements_generation (",");
	  GEN_THROUGH_STRUCT_POINTER
	    (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	}
      output_string_when_statements_generation
	((MODE (call) == ICNM_PROCEDURE_CALL ? ");" : ")"));
    }
  else if (subgraph_is_type (procedure_designator, &sinf))
    /* Call of type transformation. */
    {
      int temporary_variable_is_needed;
      register ICN_pointer temp_variable_type, result_type;

      procedure_type = sinf.type;
      output_string_when_statements_generation ("(");
      if (!it_is_statements_generation_pass)
	generate_expression
	  (ACTUAL_PARAMETER_NODE (first_actual)->actual_parameter_expression);
      subgraph_is_expression
	(ACTUAL_PARAMETER_NODE (first_actual)->actual_parameter_expression,
	 &sinf);
      temp_variable_type = sinf.type;
      /* This code is needed for correct generation INTEGER (2) or
	 analogous. Otherwise temporary variable will be of type SHORTCARD. */
      if (temp_variable_type == ICN_POINTER (&ICN_TD_short_cardinal))
	temp_variable_type = ICN_POINTER (&ICN_TD_cardinal);
      else if (temp_variable_type == ICN_POINTER (&ICN_TD_short_integer))
	temp_variable_type = ICN_POINTER (&ICN_TD_integer);
      else if (temp_variable_type
	       == ICN_POINTER (&ICN_TD_short_cardinal_or_integer))
	temp_variable_type = ICN_POINTER (&ICN_TD_cardinal_or_integer);
      temporary_variable_is_needed
	= !sinf.it_is_designator && !it_is_string_type (temp_variable_type);
      if (temporary_variable_is_needed)
	{
	  number_of_current_temporary_variable++;
	  generate_temp_variable_declaration (temp_variable_type);
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf
		  (output_file, "%s%x.%s%x",
		   temp_expression_variables_names_prefix,
		   (unsigned) number_of_expression_with_temp_variables,
		   temp_variable_name_prefix,
		   (unsigned) number_of_current_temporary_variable) < 0)
		output_file_error ();
	      add_pointer_member_name (temp_variable_type);
	      if (fputc ('=', output_file) == EOF)
		output_file_error ();
	      generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
				   ->actual_parameter_expression);
	      if (fputc (',', output_file) == EOF)
		output_file_error ();
	    }
	}
      if (it_is_statements_generation_pass)
	{
	  if (MODE (procedure_type) == ICNM_ARRAY_TYPE)
	    procedure_type = ARRAY_TYPE_NODE (procedure_type)->base_type;
	  if (fputs ("*(", output_file) == EOF)
	    output_file_error ();
	  output_type_definition_part (procedure_type, 1, TRUE);
	  output_type_definition_part (procedure_type, 1, FALSE);
	  if (fputc (')', output_file) == EOF)
	    output_file_error ();
	  if (MODE (temp_variable_type) != ICNM_ARRAY_TYPE
	      && !it_is_string_type (temp_variable_type))
	    {
	      if (fputc ('&', output_file) == EOF)
		output_file_error ();
	    }
	  if (temporary_variable_is_needed)
	    {
	      if (fprintf
		  (output_file, "%s%x.%s%x",
		   temp_expression_variables_names_prefix,
		   (unsigned) number_of_expression_with_temp_variables,
		   temp_variable_name_prefix,
		   (unsigned) number_of_current_temporary_variable) < 0)
		output_file_error ();
	      add_pointer_member_name (temp_variable_type);
	    }
	  else
	    generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
				 ->actual_parameter_expression);
	}
      output_string_when_statements_generation (")");
    }
  else
    /* All except for type tarnsformation and C function. */
    {
      subgraph_is_expression (procedure_designator, &sinf);
      procedure_type = sinf.type;
      it_is_designator = sinf.it_is_designator;
      if (procedure_type != NULL)
	result_type
	  = PROCEDURE_TYPE_NODE (procedure_type)->procedure_result_type;
      it_is_local_procedure
	= (!it_is_designator
	   && procedure_over_denotation (SCOPE (procedure_designator)) != NULL);
      if (procedure_designator == ICN_POINTER (&ICN_D_abs))
	/* ABS. */
	{
	  enum basic_type basic_type;
	  int generation_flag;

	  subgraph_is_expression (ACTUAL_PARAMETER_NODE (first_actual)
				  ->actual_parameter_expression, &sinf);
	  if (it_is_statements_generation_pass)
	    sinf.type = through_range_type (sinf.type);
	  generation_flag = (it_is_statements_generation_pass
			     && (it_is_integer_type (sinf.type)
				 || it_is_real_type (sinf.type)));
	  if (generation_flag)
	    {
	      basic_type = BASIC_TYPE_NODE (sinf.type)->basic_type;
	      if (fprintf
		  (output_file, "(%s.%s=", union_name_for_abs_implementation,
		   names_of_variables_for_abs_implementation[(int) basic_type])
		  < 0)
		output_file_error ();
	    }
	  generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
			       ->actual_parameter_expression);
	  if (generation_flag)
	    {
	      if (fprintf
		  (output_file, ")<0?-%s.%s:%s.%s",
		   union_name_for_abs_implementation,
		   names_of_variables_for_abs_implementation[(int) basic_type],
		   union_name_for_abs_implementation,
		   names_of_variables_for_abs_implementation[(int) basic_type])
		  < 0)
		output_file_error ();
	    }
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_adr))
	/* ADR. */
	{
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file, "((%s *)", word_type_representation)
		  < 0)
		output_file_error ();
	    }
	  GEN_ADDRESS (ACTUAL_PARAMETER_NODE (first_actual)
		       ->actual_parameter_expression);
	  output_string_when_statements_generation (")");
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_cap))
	/* CAP. */
	{
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file, "(%s)m2_cap(",
			   char_type_representation) < 0)
		output_file_error ();
	    }
	  GEN_CHARACTER (ACTUAL_PARAMETER_NODE (first_actual)
			 ->actual_parameter_expression);
	  output_string_when_statements_generation (")");
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_chr)
	       || procedure_designator == ICN_POINTER (&ICN_D_float)
	       || procedure_designator == ICN_POINTER (&ICN_D_ord)
	       || procedure_designator == ICN_POINTER (&ICN_D_trunc))
	/* CHR, FLOAT, ORD and TRUNC. */
	{
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf
		  (output_file, "(%s)",
		   (procedure_designator == ICN_POINTER (&ICN_D_chr)
		    ? char_type_representation
		    : (procedure_designator == ICN_POINTER (&ICN_D_float)
		       ? real_type_representation
		       : (third_edition_flag
			  ? cardinal_type_representation
			  : integer_type_representation)))) < 0)
		output_file_error ();
	    }
	  GEN_CHARACTER (ACTUAL_PARAMETER_NODE (first_actual)
			 ->actual_parameter_expression);
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_dec)
	       || procedure_designator == ICN_POINTER (&ICN_D_inc))
	/* DEC, INC. */
	{
	  subgraph_is_expression
	    (ACTUAL_PARAMETER_NODE (first_actual)->actual_parameter_expression,
	     &sinf);
	  actual = ACTUAL_PARAMETER_NODE (first_actual)->next_actual_parameter;
	  if (test_flag
	      && (MODE (sinf.type) == ICNM_RANGE_TYPE
		  || MODE (sinf.type) == ICNM_ENUMERATION_TYPE))
	    {
	      generate_test_function (sinf.type);
	      if (it_is_statements_generation_pass)
		{
		  if (fprintf (output_file, "%s%lx((%s)(",
			       range_test_function_name_prefix,
			       (unsigned long) sinf.type,
			       discrete_type_representation (sinf.type)) < 0)
		    output_file_error ();
		}
	    }
	  if (sinf.type == ICN_POINTER (&ICN_TD_address))
	    output_string_when_statements_generation ("(*(char **)&");
	  generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
			       ->actual_parameter_expression);
	  if (sinf.type == ICN_POINTER (&ICN_TD_address))
	    output_string_when_statements_generation (")");
	  output_string_when_statements_generation
	    ((procedure_designator == ICN_POINTER (&ICN_D_dec)
	      ? (actual == NULL ? "--" : "-=")
	      : (actual == NULL ? "++" : "+=")));
	  if (actual != NULL)
	    /* Second parameter is given. */
	    if (sinf.type == ICN_POINTER (&ICN_TD_address))
	      GEN_ADDRESS_AS_CARDINAL (ACTUAL_PARAMETER_NODE (actual)
			       ->actual_parameter_expression);
	    else
	      generate_expression (ACTUAL_PARAMETER_NODE (actual)
				   ->actual_parameter_expression);
	  if (test_flag
	      && (MODE (sinf.type) == ICNM_RANGE_TYPE
		  || MODE (sinf.type) == ICNM_ENUMERATION_TYPE))
	    output_string_when_statements_generation ("))");
	  output_string_when_statements_generation (";");
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_incl)
	       || procedure_designator == ICN_POINTER (&ICN_D_excl))
	/* INCL, EXCL. */
	{
	  generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
			       ->actual_parameter_expression);
	  subgraph_is_expression
	    (ACTUAL_PARAMETER_NODE (first_actual)->actual_parameter_expression,
	     &sinf);
	  actual = ACTUAL_PARAMETER_NODE (first_actual)->next_actual_parameter;
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file,
			   (procedure_designator == ICN_POINTER (&ICN_D_excl)
			    ? "&=~((%s)1<<" : "|=((%s)1<<"),
			   set_type_representation) < 0)
		output_file_error ();
	    }
	  generate_expression_with_test
	    (SET_TYPE_NODE (sinf.type)->base_type,
	     ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression,
	     CHARACTER_CODE);
	  output_string_when_statements_generation (");");
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_halt))
	/* HALT. */
	output_string_when_statements_generation ("m2_halt();");
      else if (procedure_designator == ICN_POINTER (&ICN_D_high))
	{
	  if (it_is_statements_generation_pass && !third_edition_flag)
	    fprintf (output_file, "(%s)", integer_type_representation);
	  generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
			       ->actual_parameter_expression);
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file, ".%s", open_array_high_value_name) < 0)
		output_file_error ();
	    }
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_odd))
	/* ODD. */
	{
	  generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
			       ->actual_parameter_expression);
	  output_string_when_statements_generation ("%2!=0");
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_val))
	/* VAL. */
	{
	  register ICN_pointer first_actual_expression;

	  first_actual_expression = (ACTUAL_PARAMETER_NODE (first_actual)
				     ->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (first_actual)->next_actual_parameter;
	  if (test_flag
	      && MODE (first_actual_expression) == ICNM_ENUMERATION_TYPE)
	    {
	      subgraph_is_expression (ACTUAL_PARAMETER_NODE (actual)
				      ->actual_parameter_expression, &sinf);
	      if (!sinf.it_is_constant)
		{
		  generate_test_function (first_actual_expression);
		  if (it_is_statements_generation_pass)
		    {
		      if (fprintf (output_file, "%s%lx((%s)(",
				   range_test_function_name_prefix,
				   (unsigned long) first_actual_expression,
				   discrete_type_representation
				   (first_actual_expression))
			  < 0)
			output_file_error ();
		    }
		}
	      generate_expression (ACTUAL_PARAMETER_NODE (actual)
				   ->actual_parameter_expression);
	      if (!sinf.it_is_constant)
		output_string_when_statements_generation ("))");
	    }
	  else
	    {
	      if (it_is_statements_generation_pass)
		{
		  if (fprintf
		      (output_file, "(%s)",
		       discrete_type_representation (first_actual_expression))
		      < 0)
		    output_file_error ();
		}
	      generate_expression
		(ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	    }
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_transfer))
	/* TRANSFER. */
	{
	  output_string_when_statements_generation ("m2_transfer(");
	  GEN_THROUGH_STRUCT_POINTER_OR_ADDRESS
	    (ACTUAL_PARAMETER_NODE (first_actual)
	     ->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (first_actual)->next_actual_parameter;
	  output_string_when_statements_generation (",");
	  GEN_THROUGH_STRUCT_POINTER_OR_ADDRESS
	    (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	  output_string_when_statements_generation (");");
	}
      else if (procedure_designator == ICN_POINTER (&ICN_D_newprocess))
	/* NEWPROCESS. */
	{
	  output_string_when_statements_generation ("m2_newprocess(");
	  generate_expression (ACTUAL_PARAMETER_NODE (first_actual)
			       ->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (first_actual)->next_actual_parameter;
	  output_string_when_statements_generation (",");
	  GEN_THROUGH_STRUCT_POINTER
	    (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter;
	  output_string_when_statements_generation (",");
	  generate_expression
	    (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter;
	  output_string_when_statements_generation (",");
	  GEN_THROUGH_STRUCT_POINTER_OR_ADDRESS
	    (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	  output_string_when_statements_generation (");");
	}
      else
	/* User-defined procedures. */
	{
	  ICN_pointer type_of_formal;
	  int two_brackets_are_needed, temp_variable_flag;

	  struct_for_temp_vars_is_needed = FALSE;
	  output_string_when_statements_generation ("(");
	  for (formal = next_parameter_type (procedure_type),
	       actual = first_actual;
	       actual != NULL;
	       formal = next_parameter_type (formal),
	       actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
	    {
	      subgraph_is_expression (ACTUAL_PARAMETER_NODE (actual)
				      ->actual_parameter_expression, &sinf);
	      sinf.type = through_integer_cardinal_type (sinf.type);
	      temp_variable_flag
		= (ARRAY_FORMAL_PARAMETER (formal)
		   && !VAR_FORMAL_PARAMETER (formal)
		   && !sinf.it_is_array_parameter && !sinf.it_is_designator
		   && MODE (sinf.type) != ICNM_ARRAY_TYPE
		   && !it_is_string_type (sinf.type));
	      if (temp_variable_flag)
		{
		  type_of_formal = FORMAL_PARAMETER_TYPE (formal);
		  temp_variable_flag
		    = (temp_variable_flag
		       && (type_of_formal == ICN_POINTER (&ICN_TD_word)
			   || type_of_formal == ICN_POINTER (&ICN_TD_byte)));
		}
	      if ((it_is_statements_generation_pass
		   ||
		   current_generator_pass
		   ==
		   TEMP_ARRAY_ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS)
		  && temp_variable_flag)
		number_of_temp_variable_in_current_call++;
	      if (current_generator_pass
		  == STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS)
		actuals_structures_declaration_generation_is_needed = TRUE;
	      else if (current_generator_pass
		       == ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS)
		{
		  /* Generate struct for procedure actuals. */
		  if (LAST_BLOCK_USING_PROCEDURE_TYPE (procedure_type)
		      != current_block_number_among_blocks_with_actuals)
		    {
		      LAST_BLOCK_USING_PROCEDURE_TYPE (procedure_type)
			= current_block_number_among_blocks_with_actuals;
		      if (fputs ("struct ", output_file) == EOF)
			output_file_error ();
		      output_parameter_structure_name (procedure_type);
		      if (fputc (' ', output_file) == EOF)
			output_file_error ();
		      output_parameter_structure_name (procedure_type);
		      if (fputs (";\n", output_file) == EOF)
			output_file_error ();
		    }
		}
	      if ((current_generator_pass
		   == TEMP_ARRAY_ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS
		   || (current_generator_pass
		       == STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS))
		  && temp_variable_flag)
		{
		  if (current_generator_pass
		      == STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS)
		    temp_array_actuals_structures_declaration_generation_is_needed
		      = TRUE;
		  else
		    /* Generate temp var for procedure actual. */
		    {
		      if (!struct_for_temp_vars_is_needed)
			{
			  if (fputs ("struct{", output_file) == EOF)
			    output_file_error ();
			}
		      struct_for_temp_vars_is_needed = TRUE;
		      generate_declaration_by_name
			(sinf.type, FALSE, actual_name_prefix,
			 (unsigned int) number_of_temp_variable_in_current_call);
		      if (fputs (";\n", output_file) == EOF)
			output_file_error ();
		    }
		}
	      /* Generate one actual pass (assignment to member of structure
		 for actual and usage of temp variable if it is needed).
		 The following code implements only one case (see if guard). */
	      if (!ARRAY_FORMAL_PARAMETER (formal)
		  && !VAR_FORMAL_PARAMETER (formal)
		  && MODE (FORMAL_PARAMETER_TYPE (formal)) != ICNM_ARRAY_TYPE)
		{
		  temp_variable_flag
		    = (it_is_real_type (sinf.type)
		       && (FORMAL_PARAMETER_TYPE (formal)
			   == ICN_POINTER (&ICN_TD_word)
			   || (FORMAL_PARAMETER_TYPE (formal)
			       == ICN_POINTER (&ICN_TD_byte))));
		  if (!sinf.it_is_designator && temp_variable_flag)
		    {
		      number_of_current_temporary_variable++;
		      generate_temp_variable_declaration (sinf.type);
		      if (it_is_statements_generation_pass)
			{
			  if (fprintf
			      (output_file, "%s%x.%s%x=",
			       temp_expression_variables_names_prefix,
			       (unsigned) number_of_expression_with_temp_variables,
			       temp_variable_name_prefix,
			       (unsigned) number_of_current_temporary_variable)
			      < 0)
			    output_file_error ();
			}
		      generate_expression (ACTUAL_PARAMETER_NODE (actual)
					   ->actual_parameter_expression);
		      output_string_when_statements_generation (",");
		    }
		  if (it_is_statements_generation_pass)
		    {
		      output_parameter_structure_name (procedure_type);
		      if (fputc ('.', output_file) == EOF)
			output_file_error ();
		      output_formal_parameter_type_name (formal);
		      add_pointer_member_name (FORMAL_PARAMETER_TYPE (formal));
		      if (fputc ('=', output_file) == EOF)
			output_file_error ();
		    }
		  if (temp_variable_flag)
		    {
		      if (it_is_statements_generation_pass)
			{
			  if (fprintf (output_file, "(*(%s *)&",
				       (FORMAL_PARAMETER_TYPE (formal)
					== ICN_POINTER (&ICN_TD_word)
					? word_type_representation
					: byte_type_representation)) < 0)
			    output_file_error ();
			}
		      if (sinf.it_is_designator)
			generate_expression (ACTUAL_PARAMETER_NODE (actual)
					     ->actual_parameter_expression);
		      else if (it_is_statements_generation_pass)
			{
			  if (fprintf
			      (output_file, "%s%x.%s%x",
			       temp_expression_variables_names_prefix,
			       (unsigned) number_of_expression_with_temp_variables,
			       temp_variable_name_prefix,
			       (unsigned) number_of_current_temporary_variable)
			      < 0)
			    output_file_error ();
			}
		      output_string_when_statements_generation (")");
		    }
		  else
		    {
		      if (it_is_statements_generation_pass
			  && (FORMAL_PARAMETER_TYPE (formal)
			      == ICN_POINTER (&ICN_TD_address)))
			{
			  if (fprintf (output_file, "(%s *)",
				       word_type_representation) < 0)
			    output_file_error ();
			}
		      else if (it_is_statements_generation_pass
			       && ((FORMAL_PARAMETER_TYPE (formal)
				    == ICN_POINTER (&ICN_TD_word))
				   || (FORMAL_PARAMETER_TYPE (formal)
				       == ICN_POINTER (&ICN_TD_byte))))
			{
			  if (fprintf (output_file, "(%s)",
				       (FORMAL_PARAMETER_TYPE (formal)
					== ICN_POINTER (&ICN_TD_word))
				       ? word_type_representation
				       : byte_type_representation) < 0)
			    output_file_error ();
			}
		      else if (it_is_statements_generation_pass
			       && (sinf.type == ICN_POINTER (&ICN_TD_address)))
			{
			  if (fputs ("(", output_file) == EOF)
			    output_file_error ();
			  output_type_definition_part_through_struct_pointer
			    (FORMAL_PARAMETER_TYPE (formal), TRUE);
			  output_type_definition_part_through_struct_pointer
			    (FORMAL_PARAMETER_TYPE (formal), FALSE);
		          if (fputs (")", output_file) == EOF)
			    output_file_error ();
			}
		      generate_expression_with_test
			(FORMAL_PARAMETER_TYPE (formal),
			 ACTUAL_PARAMETER_NODE (actual)
			 ->actual_parameter_expression,
			 THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE);
		    }
		}
	      else if (!it_is_statements_generation_pass)
		generate_expression (ACTUAL_PARAMETER_NODE (actual)
				     ->actual_parameter_expression);
	      /* Generate one actual pass (assignment to member of structure
		 for actual and usage of temp variable if it is needed).
		 The following code implements all others cases (see analogous
		 commentary upper). */
	      if (it_is_statements_generation_pass)
		{
		  if (!ARRAY_FORMAL_PARAMETER (formal))
		    {
		      if (VAR_FORMAL_PARAMETER (formal))
			{
			  output_parameter_structure_name (procedure_type);
			  if (fputc ('.', output_file) == EOF)
			    output_file_error ();
			  output_formal_parameter_type_name (formal);
			  if (fputc ('=', output_file) == EOF)
			    output_file_error ();
			  type_of_formal = FORMAL_PARAMETER_TYPE (formal);
			  if (type_of_formal == ICN_POINTER (&ICN_TD_address))
			    {
			      if (fprintf (output_file, "(%s **)",
					   word_type_representation) < 0)
				output_file_error ();
			    }
			  else if (type_of_formal
				   == ICN_POINTER (&ICN_TD_word))
			    {
			      if (fprintf (output_file, "(%s *)",
					   word_type_representation) < 0)
				output_file_error ();
			    }
			  else if (type_of_formal
				   == ICN_POINTER (&ICN_TD_byte))
			    {
			      if (fprintf (output_file, "(%s *)",
					   byte_type_representation) < 0)
				output_file_error ();
			    }
			  else if (MODE (type_of_formal) == ICNM_ARRAY_TYPE)
			    {
			      if (fputc ('(', output_file) == EOF)
				output_file_error ();
			      output_type_definition_part
				(type_of_formal, 1, TRUE);
			      output_type_definition_part
				(type_of_formal, 1, FALSE);
			      if (fputc (')', output_file) == EOF)
				output_file_error ();
			    }
			  GEN_ADDRESS (ACTUAL_PARAMETER_NODE (actual)
				       ->actual_parameter_expression);
			}
		      else if (MODE (FORMAL_PARAMETER_TYPE (formal))
			       == ICNM_ARRAY_TYPE)
			{
			  if (fprintf (output_file, "m2_assarr(") < 0)
			    output_file_error ();
			  output_parameter_structure_name (procedure_type);
			  if (fputc ('.', output_file) == EOF)
			    output_file_error ();
			  output_formal_parameter_type_name (formal);
			  if (fputc (',', output_file) == EOF)
			    output_file_error ();
			  generate_expression (ACTUAL_PARAMETER_NODE (actual)
					       ->actual_parameter_expression);
			  get_type_size_and_alignment
			    (FORMAL_PARAMETER_TYPE (formal), &formal_size,
			     &align);
			  if (fprintf (output_file, ",0%llo)", formal_size) < 0)
			    output_file_error ();
			}
		    }
		  else if (sinf.it_is_array_parameter)
		    {
		      output_parameter_structure_name (procedure_type);
		      if (fputc ('.', output_file) == EOF)
			output_file_error ();
		      output_formal_parameter_type_name (formal);
		      if (fprintf (output_file, ".%s=",
				   open_array_address_name) < 0)
			output_file_error ();
		      generate_expression (ACTUAL_PARAMETER_NODE (actual)
					   ->actual_parameter_expression);
		      if (fprintf (output_file, ".%s,",
				   open_array_address_name)
			  < 0)
			output_file_error ();
		      output_parameter_structure_name (procedure_type);
		      if (fputc ('.', output_file) == EOF)
			output_file_error ();
		      output_formal_parameter_type_name (formal);
		      if (fprintf (output_file, ".%s=",
				   open_array_high_value_name) < 0)
			output_file_error ();
		      generate_expression (ACTUAL_PARAMETER_NODE (actual)
					   ->actual_parameter_expression);
		      if (fprintf (output_file, ".%s",
				   open_array_high_value_name) < 0)
			output_file_error ();
		    }
		  else
		    {
		      if (temp_variable_flag)
			{
			  if (fprintf
			      (output_file, "%s%x.%s%x",
			       temp_call_actuals_name_prefix,
			       (unsigned) number_of_current_call,
			       actual_name_prefix,
			       (unsigned) number_of_temp_variable_in_current_call)
			      < 0)
			    output_file_error ();
			  add_pointer_member_name (sinf.type);
			  if (fputc ('=', output_file) == EOF)
			    output_file_error ();
			  GEN_THROUGH_STRUCT_POINTER
			    (ACTUAL_PARAMETER_NODE (actual)
			     ->actual_parameter_expression);
			  if (fprintf (output_file, ",") < 0)
			    output_file_error ();
			  output_parameter_structure_name (procedure_type);
			  if (fputc ('.', output_file) == EOF)
			    output_file_error ();
			  output_formal_parameter_type_name (formal);
			  if (fprintf (output_file, ".%s=",
				       open_array_address_name) < 0)
			    output_file_error ();
			  type_of_formal = FORMAL_PARAMETER_TYPE (formal);
			  if (type_of_formal == ICN_POINTER (&ICN_TD_word))
			    {
			      if (fprintf (output_file, "(%s *)",
					   word_type_representation) < 0)
				output_file_error ();
			    }
			  else if (type_of_formal
				   == ICN_POINTER (&ICN_TD_byte))
			    {
			      if (fprintf (output_file, "(%s *)",
					   byte_type_representation) < 0)
				output_file_error ();
			    }
			  if (fprintf
			      (output_file, "(&%s%x.%s%x)",
			       temp_call_actuals_name_prefix,
			       (unsigned) number_of_current_call,
			       actual_name_prefix,
			       (unsigned) number_of_temp_variable_in_current_call)
			      < 0)
			    output_file_error ();
			}
		      else
			{
			  output_parameter_structure_name (procedure_type);
			  if (fputc ('.', output_file) == EOF)
			    output_file_error ();
			  output_formal_parameter_type_name (formal);
			  if (fprintf (output_file, ".%s=",
				       open_array_address_name) < 0)
			    output_file_error ();
			  type_of_formal = FORMAL_PARAMETER_TYPE (formal);
			  if (type_of_formal == ICN_POINTER (&ICN_TD_address))
			    {
			      if (fprintf (output_file, "(%s **)",
					   word_type_representation) < 0)
				output_file_error ();
			    }
			  else if (type_of_formal
				   == ICN_POINTER (&ICN_TD_word))
			    {
			      if (fprintf (output_file, "(%s *)",
					   word_type_representation) < 0)
				output_file_error ();
			    }
			  else if (type_of_formal
				   == ICN_POINTER (&ICN_TD_byte))
			    {
			      if (fprintf (output_file, "(%s *)",
					   byte_type_representation) < 0)
				output_file_error ();
			    }
			  GEN_ADDRESS (ACTUAL_PARAMETER_NODE (actual)
				       ->actual_parameter_expression);
			}
		      get_type_size_and_alignment
			(FORMAL_PARAMETER_TYPE (formal), &formal_size, &align);
		      get_type_size_and_alignment
			(sinf.type, &actual_size, &align);
		      if (fprintf (output_file, ",") < 0)
			output_file_error ();
		      output_parameter_structure_name (procedure_type);
		      if (fputc ('.', output_file) == EOF)
			output_file_error ();
		      output_formal_parameter_type_name (formal);
		      if (fprintf (output_file, ".%s",
				   open_array_high_value_name) < 0)
			output_file_error ();
		      if (fprintf (output_file, "=0%llo",
				   actual_size / formal_size - 1) < 0)
			output_file_error ();
		    }
		}
	      output_string_when_statements_generation (",");
	    }
	  /* End of processing parameters. */
	  if (current_generator_pass
	      == TEMP_ARRAY_ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS
	      && struct_for_temp_vars_is_needed)
	    {
	      if (fprintf (output_file, "}%s%x;\n",
			   temp_call_actuals_name_prefix,
			   (unsigned) number_of_current_call) < 0)
		output_file_error ();
	    }
	  if (it_is_designator)
	    output_string_when_statements_generation ("(*");
	  generate_expression (procedure_designator);
	  if (it_is_designator)
	    output_string_when_statements_generation (")");
	  if (it_is_statements_generation_pass)
	    {
#ifdef COROUTINE_ENABLE
	      it_may_be_used_for_coroutine
		= (!it_is_local_procedure && result_type == NULL
		   && next_parameter_type (procedure_type) == NULL);
#else
	      it_may_be_used_for_coroutine = FALSE;
#endif
	      if (fputs ((it_may_be_used_for_coroutine ? "(0" : "("),
			 output_file) == EOF)
		output_file_error ();
	      if (next_parameter_type (procedure_type) != NULL)
		{
		  if (fprintf (output_file, "&") < 0)
		    output_file_error ();
		  output_parameter_structure_name (procedure_type);
		}
	    }
	  two_brackets_are_needed = TRUE;
	  if (result_type != NULL)
	    mode = MODE (result_type);
	  if (result_type != NULL
	      && (mode == ICNM_ARRAY_TYPE || mode == ICNM_RECORD))
	    {
	      number_of_current_temporary_variable++;
	      generate_temp_variable_declaration (result_type);
	      if (it_is_statements_generation_pass)
		{
		  if (it_may_be_used_for_coroutine
		      || next_parameter_type (procedure_type) != NULL)
		    {
		      if (fputc (',', output_file) == EOF)
			output_file_error ();
		    }
		  two_brackets_are_needed = FALSE;
		  if (mode != ICNM_ARRAY_TYPE)
		    {
		      if (fputc ('&', output_file) == EOF)
			output_file_error ();
		    }
		  if (fprintf
		      (output_file, "%s%x.%s%x),%s%x.%s%x)",
		       temp_expression_variables_names_prefix,
		       (unsigned) number_of_expression_with_temp_variables,
		       temp_variable_name_prefix,
		       (unsigned) number_of_current_temporary_variable,
		       temp_expression_variables_names_prefix,
		       (unsigned) number_of_expression_with_temp_variables,
		       temp_variable_name_prefix,
		       (unsigned) number_of_current_temporary_variable) < 0)
		    output_file_error ();
		}
	    }
	  if (two_brackets_are_needed)
	    output_string_when_statements_generation
	      ((MODE (call) == ICNM_PROCEDURE_CALL ? "));" : "))"));
	}
    }
}



/* This page contains function for generation of statements. */


/* The following function generates list statement which starts with STATEMENT.
   The statements itself are generated only on statement generation pass.  On
   the other pass the processing statements may generate various structures
   (see commentaries for generator_pass).  STATEMENT is to refer to
   statement. */

static void
generate_statement (statement)
     register ICN_pointer statement;
{
  int align;
  semantic_information sinf1, sinf2;

  for (; statement != NULL; statement = NEXT_STATEMENT (statement))
    switch (MODE (statement))
      {
      case ICNM_BLOCK_BEGIN:
	generate_line_directive (statement);
	current_scope = BLOCK_BEGIN_NODE (statement)->block_scope;
	break;
      case ICNM_RETURN_WITHOUT_RESULT:
	generate_line_directive (statement);
	if (it_is_statements_generation_pass)
	  {
	    if (fprintf (output_file, "goto %s%x;", label_name_prefix,
			 (unsigned) number_of_return_label_for_current_block)
		< 0)
	      output_file_error ();
	  }
	break;
      case ICNM_RETURN_WITH_RESULT:
	{
	  enum icode_node_mode result_type_mode;
	  Tcard return_expression_type_size;
	  register ICN_pointer upper_level_procedure, result_type;

	  generate_line_directive (statement);
	  subgraph_is_expression
	    (RETURN_WITH_RESULT_NODE (statement)->returned_expression, &sinf1);
	  upper_level_procedure = procedure_over_denotation (current_scope);
	  result_type
	    = PROCEDURE_TYPE_NODE (PROCEDURE_NODE (upper_level_procedure)
				   ->procedure_type)
	    ->procedure_result_type;
	  result_type_mode = MODE (result_type);
	  if (result_type_mode == ICNM_ARRAY_TYPE
	      || result_type_mode == ICNM_RECORD)
	    {
	      if (it_is_statements_generation_pass)
		{
		  if (fprintf (output_file,
			       (result_type_mode == ICNM_RECORD
				? "%s=" : "m2_assarr(%s,"),
			       function_result_name) < 0)
		    output_file_error ();
		}
	      GEN_THROUGH_STRUCT_POINTER
		(RETURN_WITH_RESULT_NODE (statement)->returned_expression);
	      if (it_is_statements_generation_pass
		  && result_type_mode == ICNM_ARRAY_TYPE)
		{
		  get_type_size_and_alignment
		    (sinf1.type, &return_expression_type_size, &align);
		  if (fprintf (output_file, ",0%llo)",
			       return_expression_type_size / BYTE_SIZE) < 0)
		    output_file_error ();
		}
	    }
	  else
	    {
	      if (it_is_statements_generation_pass)
		{
		  if (fprintf (output_file, "%s=", function_result_name) < 0)
		    output_file_error ();
		}
	      if (result_type != ICN_POINTER (&ICN_TD_address)
		  && sinf1.type == ICN_POINTER (&ICN_TD_address))
		{
		  if (it_is_statements_generation_pass)
		    {
		      if (fputc ('(', output_file) == EOF)
			output_file_error ();
		      output_type_definition_part_through_struct_pointer
			(result_type, TRUE);
		      output_type_definition_part_through_struct_pointer
			(result_type, FALSE);
		      if (fputc (')', output_file) == EOF)
			output_file_error ();
		    }
		  generate_expression_with_test
		    (result_type,
		     RETURN_WITH_RESULT_NODE (statement)->returned_expression,
		     ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE);
		}
	      else
		{
		  if (it_is_statements_generation_pass
		      && result_type == ICN_POINTER (&ICN_TD_address)
		      && sinf1.type != ICN_POINTER (&ICN_TD_address))
		    {
		      if (fprintf (output_file, "(%s *)",
				   word_type_representation) < 0)
			output_file_error ();
		    }
		  generate_expression_with_test
		    (result_type,
		     RETURN_WITH_RESULT_NODE (statement)->returned_expression,
		     THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE);
		}
	    }
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file, ";goto %s%x;", label_name_prefix,
			   (unsigned) number_of_return_label_for_current_block)
		  < 0)
		output_file_error ();
	    }
	  finish_expression ();
	}
	break;
      case ICNM_LOOP_STATEMENT:
	{
	  int saved_number_of_exit_label_for_current_loop;

	  generate_line_directive (statement);
	  if (it_is_statements_generation_pass)
	    {
	      saved_number_of_exit_label_for_current_loop
		= number_of_exit_label_for_current_loop;
	      number_of_exit_label_for_current_loop
		= ++number_of_generated_labels;
	      if (fputs ("for(;;){", output_file) == EOF)
		output_file_error ();
	    }
	  generate_statement
	    (LOOP_STATEMENT_NODE (statement)->loop_statements);
	  if (it_is_statements_generation_pass)
	    {
	      if (fprintf (output_file, "}%s%x:;", label_name_prefix,
			   (unsigned) number_of_exit_label_for_current_loop)
		  < 0)
		output_file_error ();
	    }
	  number_of_exit_label_for_current_loop
	    = saved_number_of_exit_label_for_current_loop;
	}
	break;
      case ICNM_EXIT_STATEMENT:
	generate_line_directive (statement);
	if (it_is_statements_generation_pass)
	  {
	    if (fprintf (output_file, "goto %s%x;", label_name_prefix,
			 (unsigned) number_of_exit_label_for_current_loop) < 0)
	      output_file_error ();
	  }
	break;
      case ICNM_WHILE_STATEMENT:
	generate_line_directive (statement);
	output_string_when_statements_generation ("while(");
	generate_expression
	  (WHILE_STATEMENT_NODE (statement)->while_expression);
	finish_expression ();
	output_string_when_statements_generation ("){");
	generate_statement
	  (WHILE_STATEMENT_NODE (statement)->while_statements);
	output_string_when_statements_generation ("}");
	break;
      case ICNM_REPEAT_STATEMENT:
	generate_line_directive (statement);
	output_string_when_statements_generation ("do{");
	generate_statement
	  (REPEAT_STATEMENT_NODE (statement)->repeat_statements);
	output_string_when_statements_generation ("}while(!");
	generate_expression
	  (REPEAT_STATEMENT_NODE (statement)->until_expression);
	finish_expression ();
	output_string_when_statements_generation (");");
	break;
      case ICNM_IF_STATEMENT:
	{
	  register ICN_pointer continuation;

	  generate_line_directive (statement);
	  output_string_when_statements_generation ("if(");
	  generate_expression (IF_STATEMENT_NODE (statement)->if_expression);
	  finish_expression ();
	  output_string_when_statements_generation ("){");
	  generate_statement (IF_STATEMENT_NODE (statement)->if_statements);
	  for (continuation = IF_STATEMENT_NODE (statement)->if_continuation;
	       continuation != NULL
	       && MODE (continuation) == ICNM_ELSIF_CLAUSE;
	       continuation
	       = ELSIF_CLAUSE_NODE (continuation)->elsif_continuation)
	    {
	      output_string_when_statements_generation ("}else if(");
	      generate_expression
		(ELSIF_CLAUSE_NODE (continuation)->elsif_expression);
	      finish_expression ();
	      output_string_when_statements_generation ("){");
	      generate_statement
		(ELSIF_CLAUSE_NODE (continuation)->elsif_statements);
	    }
	  if (continuation != NULL)
	    {
	      output_string_when_statements_generation ("}else{");
	      generate_statement (continuation);
	    }
	  output_string_when_statements_generation ("}");
	}
	break;
      case ICNM_FOR_STATEMENT:
	{
	  int increment_is_negative;
	  Tcard increment;
	  register ICN_pointer control_variable_type;

	  generate_line_directive (statement);
	  output_string_when_statements_generation ("for(");
	  generate_expression
	    (FOR_STATEMENT_NODE (statement)->for_control_variable);
	  output_string_when_statements_generation ("=");
	  control_variable_type
	    = VARIABLE_DECLARATION_NODE (FOR_STATEMENT_NODE (statement)
					 ->for_control_variable)
	    ->variable_type;
	  if (it_is_statements_generation_pass
	      && control_variable_type == ICN_POINTER (&ICN_TD_address))
	    {
	      if (fprintf (output_file, "(%s *)", word_type_representation)
		  < 0)
		output_file_error ();
	    }
	  generate_expression_with_test
	    (control_variable_type,
	     FOR_STATEMENT_NODE (statement)->for_starting_value,
	     ADDRESS_AS_CARDINAL_OR_CHARACTER_CODE);
	  output_string_when_statements_generation (";");
	  GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER
	    (FOR_STATEMENT_NODE (statement)->for_control_variable);
	  increment_is_negative = FALSE;
	  if (FOR_STATEMENT_NODE (statement)->for_increment == NULL)
	    increment = 1;
	  else if (MODE (FOR_STATEMENT_NODE (statement)->for_increment)
		   == ICNM_CARDINAL)
	    increment
	      = CARDINAL_NODE (FOR_STATEMENT_NODE (statement)
			       ->for_increment)
	      ->cardinal_value;
	  else if (INTEGER_NODE (FOR_STATEMENT_NODE (statement)
				 ->for_increment)
		   ->integer_value >= 0)
	    increment
	      = INTEGER_NODE (FOR_STATEMENT_NODE (statement)
			      ->for_increment)
	      ->integer_value;
	  else
	    {
	      increment_is_negative = TRUE;
	      increment
		= (-INTEGER_NODE (FOR_STATEMENT_NODE (statement)
				  ->for_increment)
		   ->integer_value);
	    }
	  output_string_when_statements_generation
	    ((increment_is_negative ? ">=" : "<="));
	  GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER
	    (FOR_STATEMENT_NODE (statement)->for_limit);
	  output_string_when_statements_generation (";");
	  generate_expression
	    (FOR_STATEMENT_NODE (statement)->for_control_variable);
	  if (MODE (control_variable_type) == ICNM_RANGE_TYPE)
	    generate_test_function (control_variable_type);
	  if (it_is_statements_generation_pass)
	    {
	      if (test_flag && MODE (control_variable_type) == ICNM_RANGE_TYPE)
		{
		  if (fprintf
		      (output_file, "=%s%lx((%s)(",
		       range_test_function_name_prefix,
		       (unsigned long) control_variable_type,
		       discrete_type_representation (control_variable_type))
		      < 0)
		    output_file_error ();
		  generate_expression (FOR_STATEMENT_NODE (statement)
				       ->for_control_variable);
		  if (fputc ((increment_is_negative ? '-' : '+'), output_file)
		      == EOF)
		    output_file_error ();
		  if (fprintf (output_file, "%llu))){", increment) < 0)
		    output_file_error ();
		}
	      else if (control_variable_type == ICN_POINTER (&ICN_TD_address))
		{
		  if (fprintf (output_file, "=(%s *)(",
			       word_type_representation) < 0)
		    output_file_error ();
		  GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER
		    (FOR_STATEMENT_NODE (statement)->for_control_variable);
		  if (fprintf (output_file,
			       (increment_is_negative
				? "-%llu)){" : "+%llu)){\n"), increment) < 0)
		    output_file_error ();
		}
	      else if (increment == 1)
		{
		  if (fputs ((increment_is_negative ? "--){" : "++){"),
			     output_file) == EOF)
		    output_file_error ();
		}
	      else
		{
		  if (fprintf (output_file,
			       (increment_is_negative
				? "-=%llu){" : "+=%llu){"), increment) < 0)
		    output_file_error ();
		}
	    }
	  finish_expression ();
	  generate_statement (FOR_STATEMENT_NODE (statement)->for_statements);
	  output_string_when_statements_generation ("}");
	}
	break;
      case ICNM_WITH_STATEMENT:
	{
	  register ICN_pointer saved_current_scope, scope, saved_record_scope;

	  generate_line_directive (statement);
	  subgraph_is_expression
	    (WITH_STATEMENT_NODE (statement)->with_designator, &sinf1);
	  if (it_is_statements_generation_pass)
	    {
	      if (fputs ("{register ", output_file) == EOF)
		output_file_error ();
	      generate_declaration_by_name
		(sinf1.type, TRUE, with_variable_name_prefix,
		 (unsigned int) current_level_of_nesting_with_statements + 1);
	      if (fputs ("=(&", output_file) == EOF)
		output_file_error ();
	    }
	  generate_expression
	    (WITH_STATEMENT_NODE (statement)->with_designator);
	  current_level_of_nesting_with_statements++;
	  finish_expression ();
	  output_string_when_statements_generation (");");
	  saved_record_scope = RECORD_NODE (sinf1.type)->scope;
	  RECORD_NODE (sinf1.type)->scope = current_scope;
	  current_scope = sinf1.type;
	  for (scope = current_scope;
	       scope != NULL && SCOPE (scope) != sinf1.type;
	       scope = SCOPE (scope)) ;
	  if (scope != NULL)
	    SCOPE (scope) = saved_record_scope;
	  generate_statement
	    (WITH_STATEMENT_NODE (statement)->with_statements);
	  current_level_of_nesting_with_statements--;
	  saved_current_scope = current_scope;
	  if (scope == NULL
	      || RECORD_NODE (current_scope)->scope != saved_record_scope)
	    current_scope = RECORD_NODE (current_scope)->scope;
	  RECORD_NODE (saved_current_scope)->scope = saved_record_scope;
	  if (scope != NULL)
	    SCOPE (scope) = SCOPE (saved_current_scope);
	  output_string_when_statements_generation ("}");
	}
	break;
      case ICNM_C_CODE_IN_STATEMENTS:
	if (it_is_statements_generation_pass)
	  {
	    if (fputs (C_CODE_IN_STATEMENTS_NODE (statement)->C_text,
		       output_file) == EOF)
	      output_file_error ();
	  }
	break;
      case ICNM_CASE_STATEMENT:
	{
	  register ICN_pointer case_variant, case_label;
	  register ICN_pointer left_case_range_bound, right_case_range_bound;
	  Tcard cardinal_case_value;
	  Tint integer_case_value;

	  generate_line_directive (statement);
	  subgraph_is_expression
	    (CASE_STATEMENT_NODE (statement)->case_expression, &sinf1);
	  output_string_when_statements_generation ("switch(");
	  GEN_ADDRESS_AS_CARDINAL_OR_CHARACTER
	    (CASE_STATEMENT_NODE (statement)->case_expression);
	  finish_expression ();
	  output_string_when_statements_generation ("){");
	  for (case_variant
	       = CASE_STATEMENT_NODE (statement)->case_statement_variant_list;
	       MODE (case_variant) == ICNM_CASE_STATEMENT_VARIANT;
	       case_variant
	       = CASE_STATEMENT_VARIANT_NODE (case_variant)->next_case_variant)
	    {
	      if (it_is_statements_generation_pass)
		for (case_label = (CASE_STATEMENT_VARIANT_NODE (case_variant)
				   ->case_label_list);
		     case_label != NULL;
		     case_label = CASE_LABEL_LIST (case_label))
		  {
		    if (MODE (case_label) == ICNM_CASE_STATEMENT_LABEL_ELEMENT)
		      left_case_range_bound
			= right_case_range_bound
			  = (CASE_STATEMENT_LABEL_ELEMENT_NODE (case_label)
			     ->case_label_value);
		    else
		      {
			left_case_range_bound
			  = (CASE_STATEMENT_LABEL_RANGE_NODE (case_label)
			     ->case_label_range_left_bound);
			right_case_range_bound
			  = (CASE_STATEMENT_LABEL_RANGE_NODE (case_label)
			     ->case_label_range_right_bound);
		      }
		    if (MODE (left_case_range_bound) == ICNM_INTEGER)
		      {
			for (integer_case_value
			     = (INTEGER_NODE (left_case_range_bound)
				->integer_value);
			     integer_case_value
			     <= (INTEGER_NODE (right_case_range_bound)
				 ->integer_value);
			     integer_case_value++)
			  if (fprintf (output_file, "case %lld:",
				       integer_case_value) < 0)
			    output_file_error ();
		      }
		    else
		      {
			for (cardinal_case_value
			     = cardinal_value (left_case_range_bound);
			     cardinal_case_value
			     <= cardinal_value (right_case_range_bound);
			     cardinal_case_value++)
			  if (fprintf (output_file, "case 0%llo:",
				       cardinal_case_value) < 0)
			    output_file_error ();
		      }
		  }
	      if (CASE_STATEMENT_VARIANT_NODE (case_variant)->case_label_list
		  == NULL)
		output_string_when_statements_generation ("default:");
	      generate_statement (CASE_STATEMENT_VARIANT_NODE (case_variant)
				  ->case_variant_elements);
	      output_string_when_statements_generation ("break;");
	    }
	  output_string_when_statements_generation ("}");
	}
	break;
      case ICNM_ASSIGNMENT:
	{
	  Tcard assignment_variable_size, assignment_expression_size;

	  generate_line_directive (statement);
	  subgraph_is_expression
	    (ASSIGNMENT_NODE (statement)->assignment_variable, &sinf1);
	  subgraph_is_expression
	    (ASSIGNMENT_NODE (statement)->assignment_expression, &sinf2);
	  if (MODE (sinf1.type) == ICNM_ARRAY_TYPE)
	    {
	      if (it_is_statements_generation_pass)
		{
		  get_type_size_and_alignment
		    (sinf1.type, &assignment_variable_size, &align);
		  get_type_size_and_alignment
		    (sinf2.type, &assignment_expression_size, &align);
		  if (fputs
		      ((assignment_variable_size > assignment_expression_size
			&& third_edition_flag
			? "m2_assstr(" : "m2_assarr("), output_file) == EOF)
		    output_file_error ();
		}
	      GEN_THROUGH_STRUCT_POINTER
		(ASSIGNMENT_NODE (statement)->assignment_variable);
	      output_string_when_statements_generation (",");
	      GEN_THROUGH_STRUCT_POINTER
		(ASSIGNMENT_NODE (statement)->assignment_expression);
	      if (it_is_statements_generation_pass)
		{
		  if (fprintf (output_file, ",0%llo",
			       assignment_variable_size / BYTE_SIZE) < 0)
		    output_file_error ();
		  if (assignment_variable_size > assignment_expression_size
		      && third_edition_flag)
		    {
		      if (fprintf (output_file, ",0%llo",
				   assignment_expression_size / BYTE_SIZE) < 0)
			output_file_error ();
		    }
		  if (fputs (");", output_file) == EOF)
		    output_file_error ();
		}
	    }
	  else
	    {
	      GEN_THROUGH_STRUCT_POINTER
		(ASSIGNMENT_NODE (statement)->assignment_variable);
	      output_string_when_statements_generation ("=");
	      if (sinf1.type != ICN_POINTER (&ICN_TD_address)
		  && sinf2.type == ICN_POINTER (&ICN_TD_address))
		{
		  if (it_is_statements_generation_pass)
		    {
		      if (fputc ('(', output_file) == EOF)
			output_file_error ();
		      output_type_definition_part_through_struct_pointer
			(sinf1.type, TRUE);
		      output_type_definition_part_through_struct_pointer
			(sinf1.type, FALSE);
		      if (fputc (')', output_file) == EOF)
			output_file_error ();
		    }
		  generate_expression_with_test
		    (sinf1.type,
		     ASSIGNMENT_NODE (statement)->assignment_expression,
		     ADDRESS_AS_CARDINAL_OR_THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE);
		}
	      else
		{
		  if (it_is_statements_generation_pass
		      && sinf1.type == ICN_POINTER (&ICN_TD_address)
		      && sinf2.type != ICN_POINTER (&ICN_TD_address))
		    {
		      if (fprintf (output_file, "(%s *)",
				   word_type_representation) < 0)
			output_file_error ();
		    }
		  generate_expression_with_test
		    (sinf1.type,
		     ASSIGNMENT_NODE (statement)->assignment_expression,
		     THROUGH_STRUCT_POINTER_OR_CHARACTER_CODE);
		}
	      output_string_when_statements_generation (";");
	    }
	  finish_expression ();
	}
	break;
      case ICNM_PROCEDURE_CALL:
	generate_line_directive (statement);
	generate_call (statement);
	finish_expression ();
	break;
      default:
	abort ();
      }
}



/* This page contains functions for generation (all passes) of implementations
   of modules or procedures. */


/* The function executes PASS (see commentaries for generator_pass) on
   the block of MODULE_OR_PROCEDURE.  MODULE_OR_PROCEDURE is to refer to
   any module or procedure. */

static void
execute_pass (module_or_procedure, pass)
     ICN_pointer module_or_procedure;
     register enum generator_pass pass;
{
  current_generator_pass = pass;
  it_is_statements_generation_pass = pass == STATEMENTS_GENERATION_PASS;
  number_of_expression_with_temp_variables = 0;
  number_of_current_call = 0;
  number_of_current_temporary_variable = 0;
  number_of_temp_variable_in_current_call = 0;
  current_expression_needs_temp_variables = FALSE;
  if (pass == ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS)
    current_block_number_among_blocks_with_actuals++;
  generate_statement (BLOCK_BEGIN (module_or_procedure));
}

/* The recursive function executes block generation for all local modules which
   are immediately contained in MODULE_OR_PROCEDURE.  MODULE_OR_PROCEDURE is to
   refer to any module or procedure. */

static void
generate_local_module_blocks (module_or_procedure)
     register ICN_pointer module_or_procedure;
{
  for (module_or_procedure = LOCAL_MODULE_LIST (module_or_procedure);
       module_or_procedure != NULL;
       module_or_procedure = MODULE_NODE (module_or_procedure)->module_brother)
    generate_block (module_or_procedure);
}

/* The function generates calls of bodies of compilation units which are
   immediately imported by COMPILATION_UNIT.  COMPILATION_UNIT is to refer to
   main or implementation module.  The imported module body may be called
   once more (the body will be executed only once in any case). */

static void
generate_imported_compilation_unit_calls (compilation_unit)
     ICN_pointer compilation_unit;
{
  register ICN_pointer imported_module, declaration_element;
  register ICN_pointer pred_imported_module, imported_module_identifier;

  pred_imported_module = NULL;
  for (declaration_element = NEXT_DECLARATION_ELEMENT (compilation_unit);
       declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_IMPORT)
      {
	if (MODULE_EXPORTER (declaration_element) == NULL)
	  imported_module_identifier = IDENTIFIER (declaration_element);
	else
	  imported_module_identifier = MODULE_EXPORTER (declaration_element);
	imported_module
	  = find_denotation (ICN_POINTER (NULL), imported_module_identifier);
	if (imported_module != NULL
	    && imported_module != ICN_POINTER (&ICN_D_system)
	    && imported_module != ICN_POINTER (&ICN_D_cproc)
	    && imported_module != pred_imported_module)
	  {
	    output_object_full_name (imported_module, FALSE);
	    if (fputs ("();", output_file) == EOF)
	      output_file_error ();
	    pred_imported_module = imported_module;
	  }
      }
}

/* The function executes necessary passes of generation of declarations (except
   for STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS).  Necessity of such
   passes is defined on STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS.  The
   passes are executed on MODULE_OR_PROCEDURE.  MODULE_OR_PROCEDURE is to refer
   to any module or procedure. */

static void
execute_declaration_generation_passes (module_or_procedure)
     register ICN_pointer module_or_procedure;
{
  if (actuals_structures_declaration_generation_is_needed)
    execute_pass (module_or_procedure,
		  ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS);
  if (temp_array_actuals_structures_declaration_generation_is_needed)
    execute_pass (module_or_procedure,
		  TEMP_ARRAY_ACTUALS_STRUCTURES_DECLARATION_GENERATION_PASS);
  if (temporary_variables_declaration_generation_is_needed)
    execute_pass (module_or_procedure,
		  TEMPORARY_VARIABLES_DECLARATION_GENERATION_PASS);
}

/* The recursive function generates body (without own declarations, prologue
   but with epilogue and blocks of local modules) of module MODULE.
   IT_IS_MAIN_MODULE is TRUE if the module is main module.
   NUMBER_OF_RETURN_LABEL_FOR_CURRENT_BLOCK defines name of return label (see
   commentaries for label_prefix).  MODULE is to refer to any module. */

static void
generate_module_body (module, number_of_return_label_for_current_block,
		      it_is_main_module)
     register ICN_pointer module;
     int number_of_return_label_for_current_block, it_is_main_module;
{
  generate_local_module_blocks (module);
  execute_pass (module, STATEMENTS_GENERATION_PASS);
  if (fprintf (output_file, "%s%x:", label_name_prefix,
	       (unsigned) number_of_return_label_for_current_block) < 0)
    output_file_error ();
  if (fputs ((it_is_main_module ? "exit(0);}\n" : ";}\n"), output_file) == EOF)
    output_file_error ();
}

/* The recursive function generates full code for block of MODULE_OR_PROCEDURE.
   Non-local modules and procedures are implemented by C functions.  Local
   modules are implemented by nested C blocks.  The following function executes
   all necessary passes (except for
   STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS -- see commentaries for
   generator_pass).  MODULE_OR_PROCEDURE is to refer to any module or
   procedure. */

static void
generate_block (module_or_procedure)
     register ICN_pointer module_or_procedure;
{
  if (!block_code_may_be_used (module_or_procedure))
    return;
  number_of_return_label_for_current_block = ++number_of_generated_labels;
  if (MODE (module_or_procedure) == ICNM_PROCEDURE)
    {
      register ICN_pointer result_type_implementation, procedure_type;
      register ICN_pointer result_type, formal_parameter;
      int indirection_is_needed, it_may_be_used_as_coroutine;
      int nested_procedures_exist, it_has_parameteres, align;
      int semicolon_is_not_needed, it_has_variables;
      Tcard parameter_size;

      result_type_implementation
	= get_C_function_type (module_or_procedure, &indirection_is_needed);
      procedure_type = PROCEDURE_NODE (module_or_procedure)->procedure_type;
      result_type
	= PROCEDURE_TYPE_NODE (procedure_type)->procedure_result_type;
      it_has_parameteres = next_parameter_type (procedure_type) != NULL;
      if (!procedure_is_exported_from_compilation_unit (module_or_procedure))
	{
	  if (fputs ("static ", output_file) == EOF)
	    output_file_error ();
	}
      output_type_definition_part (result_type_implementation,
				   (indirection_is_needed ? 1 : 0), TRUE);
      if (fputc (' ', output_file) == EOF)
	output_file_error ();
      output_object_full_name (module_or_procedure, FALSE);
      if (fputc ('(', output_file) == EOF)
	output_file_error ();
#ifdef COROUTINE_ENABLE
      it_may_be_used_as_coroutine
	= (result_type == NULL && !it_has_parameteres
	   && procedure_over_denotation (SCOPE (module_or_procedure)) == NULL);
#else
      it_may_be_used_as_coroutine = FALSE;
#endif
      if (it_may_be_used_as_coroutine)
	{
	  if (fputs (coroutine_parameter_name, output_file) == EOF)
	    output_file_error ();
	}
      if (it_has_parameteres)
	{
	  if (fputs (par_prefix, output_file) == EOF)
	    output_file_error ();
	}
      if (result_type != NULL && result_type_implementation == NULL)
	{
	  if (fprintf (output_file,
		       (it_has_parameteres || it_may_be_used_as_coroutine
			? ",%s" : "%s"),
		       function_result_name) < 0)
	    output_file_error ();
	}
      if (fputc (')', output_file) == EOF)
	output_file_error ();
      output_type_definition_part (result_type_implementation,
				   (indirection_is_needed ? 1 : 0), FALSE);
      if (it_may_be_used_as_coroutine)
	{
	  if (fprintf (output_file, "int %s;", coroutine_parameter_name) < 0)
	    output_file_error ();
	}
      if (it_has_parameteres)
	{
	  if (fputs ("register struct ", output_file) == EOF)
	    output_file_error ();
	  output_parameter_structure_name (procedure_type);
	  if (fprintf (output_file, " *%s;", par_prefix) < 0)
	    output_file_error ();
	}
      if (result_type != NULL && result_type_implementation == NULL)
	{
	  generate_declaration_by_name
	    (result_type, MODE (result_type) == ICNM_RECORD,
	     function_result_name, 0);
	  if (fputc (';', output_file) == EOF)
	    output_file_error ();
	}
      if (fputs ("\n{", output_file) == EOF)
	output_file_error ();
      it_has_variables
	= generate_structures_for_procedure_variables (module_or_procedure,
						       TRUE);
      generate_structures_for_procedure_parameters (module_or_procedure, TRUE);
      if (result_type != NULL && result_type_implementation != NULL)
	{
	  if (fputs ("register ", output_file) == EOF)
	    output_file_error ();
	  generate_declaration_by_name (result_type_implementation,
					indirection_is_needed,
					function_result_name, 0);
	  if (fputs (";\n", output_file) == EOF)
	    output_file_error ();
	}
      execute_declaration_generation_passes (module_or_procedure);
      nested_procedures_exist = it_has_nested_procedures (module_or_procedure);
      if (it_may_be_used_as_coroutine)
	{
	  if (fprintf (output_file, "if(%s)m2_newpr1(",
		       coroutine_parameter_name) < 0)
	    output_file_error ();
	  if (it_has_variables)
	    {
	      if (fprintf (output_file, "&%s,", var_prefix) < 0)
		output_file_error ();
	    }
	  else
	    {
	      if (fputs ("0,", output_file) == EOF)
		output_file_error ();
	    }
	  if (nested_procedures_exist && it_has_variables)
	    {
	      if (fputc ('&', output_file) == EOF)
		output_file_error ();
	      output_object_full_name (module_or_procedure, TRUE);
	    }
	  else
	    {
	      if (fputc ('0', output_file) == EOF)
		output_file_error ();
	    }
	  if (fputs (");else{", output_file) == EOF)
	    output_file_error ();
	}
      if (nested_procedures_exist)
	{
	  if (it_has_variables)
	    {
	      if (fprintf (output_file, "%s=",
			   name_of_saved_pointer_to_procedure_variables) < 0)
		output_file_error ();
	      output_object_full_name (module_or_procedure, TRUE);
	      if (fputs (";", output_file) == EOF)
		output_file_error ();
	      output_object_full_name (module_or_procedure, TRUE);
	      if (fprintf (output_file, "=(&%s);", var_prefix) < 0)
		output_file_error ();
	    }
	  if (it_has_parameteres)
	    {
	      if (fprintf (output_file, "%s=",
			   name_of_saved_pointer_to_procedure_parameters) < 0)
		output_file_error ();
	      output_parameter_structure_name (procedure_type);
	      if (fputs (";", output_file) == EOF)
		output_file_error ();
	      output_parameter_structure_name (procedure_type);
	      if (fprintf (output_file, "=%s;", par_prefix) < 0)
		output_file_error ();
	    }
	}
      if (it_may_be_used_as_coroutine)
	{
	  if (fputc ('}', output_file) == EOF)
	    output_file_error ();
	}
      /* Generate allocating memory for array parameters. */
      for (formal_parameter = next_parameter_type (procedure_type);
	   formal_parameter != NULL;
	   formal_parameter = next_parameter_type (formal_parameter))
	if (ARRAY_FORMAL_PARAMETER (formal_parameter)
	    && !VAR_FORMAL_PARAMETER (formal_parameter))
	  {
	    get_type_size_and_alignment
	      (FORMAL_PARAMETER_TYPE (formal_parameter), &parameter_size,
	       &align);
	    if (fprintf (output_file, "m2_arrpar(&%s->", par_prefix) < 0)
	      output_file_error ();
	    output_formal_parameter_type_name (formal_parameter);
	    if (fprintf (output_file, ".%s,%s->", open_array_address_name,
			 par_prefix) < 0)
	      output_file_error ();
	    output_formal_parameter_type_name (formal_parameter);
	    if (fprintf (output_file, ".%s", open_array_high_value_name) < 0)
	      output_file_error ();
	    if (fprintf (output_file, ",0%llo);", parameter_size / BYTE_SIZE)
		< 0)
	      output_file_error ();
	  }
      generate_local_module_blocks (module_or_procedure);
      execute_pass (module_or_procedure, STATEMENTS_GENERATION_PASS);
      if (fprintf (output_file, "%s%x:", label_name_prefix,
		   (unsigned) number_of_return_label_for_current_block) < 0)
	output_file_error ();
      semicolon_is_not_needed = FALSE;
      if (nested_procedures_exist)
	{
	  if (it_has_variables)
	    {
	      output_object_full_name (module_or_procedure, TRUE);
	      if (fprintf (output_file, "=%s;\n",
			   name_of_saved_pointer_to_procedure_variables) < 0)
		output_file_error ();
	      semicolon_is_not_needed = TRUE;
	    }
	  if (it_has_parameteres)
	    {
	      output_parameter_structure_name (procedure_type);
	      if (fputs ("=", output_file) == EOF)
		output_file_error ();
	      if (fprintf (output_file, "%s;\n",
			   name_of_saved_pointer_to_procedure_parameters) < 0)
		output_file_error ();
	      semicolon_is_not_needed = TRUE;
	    }
	}
      /* Generate freeing memory for array parameters. */
      for (formal_parameter = next_parameter_type (procedure_type);
	   formal_parameter != NULL;
	   formal_parameter = next_parameter_type (formal_parameter))
	if (ARRAY_FORMAL_PARAMETER (formal_parameter)
	    && !VAR_FORMAL_PARAMETER (formal_parameter))
	  {
	    if (fprintf (output_file, "free(%s->", par_prefix) < 0)
	      output_file_error ();
	    output_formal_parameter_type_name (formal_parameter);
	    if (fprintf (output_file, ".%s);", open_array_address_name) < 0)
	      output_file_error ();
	    semicolon_is_not_needed = TRUE;
	  }
      if (result_type != NULL && result_type_implementation != NULL)
	{
	  if (fprintf (output_file, "return %s;", function_result_name) < 0)
	    output_file_error ();
	  semicolon_is_not_needed = TRUE;
	}
      if (fputs ((semicolon_is_not_needed ? "}\n" : ";}\n"), output_file)
	  == EOF)
	output_file_error ();
    }
  else if ((enum module_mode) MODULE_NODE (module_or_procedure)->module_mode
	   == MM_LOCAL_MODULE)
    {
      if (fputc ('{', output_file) == EOF)
	output_file_error ();
      execute_declaration_generation_passes (module_or_procedure);
      generate_module_body (module_or_procedure,
			    number_of_return_label_for_current_block, FALSE);
    }
  else if ((enum module_mode) MODULE_NODE (module_or_procedure)->module_mode
	   == MM_DEFINITION_WITH_IMPLEMENTATION_MODULE)
    {
      if (fprintf (output_file, "static int %s=0;\n%s ",
		   implementation_module_initiation_flag_name,
		   void_type_representation) < 0)
	output_file_error ();
      output_object_full_name (module_or_procedure, FALSE);
      if (fputs ("(){\n", output_file) == EOF)
	output_file_error ();
      execute_declaration_generation_passes (module_or_procedure);
      if (fprintf (output_file, "if(%s)return;%s=1;",
		   implementation_module_initiation_flag_name,
		   implementation_module_initiation_flag_name) < 0)
	output_file_error ();
      generate_imported_compilation_unit_calls (module_or_procedure);
      generate_module_body (module_or_procedure,
			    number_of_return_label_for_current_block, FALSE);
    }
  else if ((enum module_mode) MODULE_NODE (module_or_procedure)->module_mode
	   == MM_MAIN_MODULE)
    {
      if (fputs ("int m2argc;char **m2argv,**m2envp;\n", output_file) == EOF)
	output_file_error ();
      /* The following generates warning for some compilers.  Therefore
	 the exit declaration is not generated. */
      /* if (fputs ("extern exit();\n", output_file) == EOF)
	 output_file_error (); */
      if (fputs ("main(argc,argv,envp)int argc;char **argv,**envp;{\n",
		 output_file) == EOF)
	output_file_error ();
      execute_declaration_generation_passes (module_or_procedure);
      if (fputs ("m2argc=argc;m2argv=argv;m2envp=envp;\n", output_file) == EOF)
	output_file_error ();
      generate_imported_compilation_unit_calls (module_or_procedure);
      generate_module_body (module_or_procedure,
			    number_of_return_label_for_current_block, TRUE);
    }
  else
    abort ();
}

/* The following recursive function executes
   STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS (see commentaries for
   generator_pass) for MODULE_OR_PROCEDURE and its local modules.
   MODULE_OR_PROCEDURE is to refer to any module or procedure. */

static void
execute_string_constants_declarations_generation_pass (module_or_procedure)
     ICN_pointer module_or_procedure;
{
  register ICN_pointer local_module;

  if (!block_code_may_be_used (module_or_procedure))
    return;
  for (local_module = LOCAL_MODULE_LIST (module_or_procedure);
       local_module != NULL;
       local_module = MODULE_NODE (local_module)->module_brother)
    execute_string_constants_declarations_generation_pass (local_module);
  actuals_structures_declaration_generation_is_needed = FALSE;
  temp_array_actuals_structures_declaration_generation_is_needed = FALSE;
  temporary_variables_declaration_generation_is_needed = FALSE;
  execute_pass (module_or_procedure,
		STRING_CONSTANTS_DECLARATIONS_GENERATION_PASS);
}



/* This page contains functions which are needed for generation with
   flag `-all'. */


/* The function places unique name of OBJECT in *UNIQUE_NAME.  The unique
   name is object identifier, scope, upper scope, ... and compilation unit
   identifier separated by dot.  OBJECT is to refer to denotation (or NULL).
   */

static void
make_unique_object_name (unique_name, object)
     register VLS *unique_name;
     ICN_pointer object;
{
  VLS_ADD_CHAR (*unique_name, '\0');
  for (; object != NULL; object = SCOPE (object))
    {
      object = declaration_level_scope (object);
      VLS_ADD_STRING
	(*unique_name,
	 IDENTIFIER_NODE (IDENTIFIER (object))->identifier_string);
      if (SCOPE (object) != NULL)
	VLS_ADD_STRING (*unique_name, ".");
    }
}

/* The following structure contains a reference to block. */

struct block_link
{
  struct block_link *next_block_link;
  struct block_structure *block;
};

/* The following structure contains information about references to blocks
   from given block.  The structure does not exist for local modules (although
   the local modules are processed by set_current_block_structure and
   insert_block_link calls).  Because necessity of local module code is
   determined only by the necessity of code of procedure or compilation unit
   which contains the local module. */

struct block_structure
{
  /* TRUE if block code must be present in executable program (i.e. the
     block is achievable through references to blocks from the blocks
     of compilation units. */
  char block_may_be_used;
  /* Unique name of block (see commentaries for make_unique_object_name. */
  char *block_name;
  /* Pointer to first structure containing a reference to block from
     given block. */
  struct block_link *first_block_link;
};

/* The hash table of block structures.  The key is block_name. */

static hash_table block_structure_table;

/* The hash function for block_structure_table (see commentaries for
   hash_table). */

static unsigned
hash_function (el_ptr)
     hash_table_entry el_ptr;
{
  register char *block_name;
  register unsigned hash_value, block_name_length, i;

  block_name = ((struct block_structure *) el_ptr)->block_name;
  block_name_length = strlen (block_name);
  hash_value = 0;
  for (i = 0; i < block_name_length; i++, block_name++)
    hash_value += (*block_name);
  return hash_value;
}

/* The compare function for block_structure_table (see commentaries for
   hash_table). */

static int
compare_function (el1_ptr, el2_ptr)
     hash_table_entry *el1_ptr, *el2_ptr;
{
  return !strcmp (((struct block_structure *) el1_ptr)->block_name,
		  ((struct block_structure *) el2_ptr)->block_name);
}

/* Create and initiate block_structure_table (see commentaries for
   create_hash_table). */

void
create_block_structure_table ()
{
  block_structure_table = create_hash_table (1000, hash_function,
					     compare_function);
}

/* The function returns hash table entry which contains pointer to
   block_structure with BLOCK_NAME.  If the pointer is NULL then such
   block_structure does not exist.  In this case the entry is reserved
   for insertion pointer to such block_structure if RESERVATION_IS_NEEDED is
   TRUE (see also commentaries for find_hash_table_entry). */

static struct block_structure **
find_block_structure_table_entry (block_name, reservation_is_needed)
     char *block_name;
     int reservation_is_needed;
{
  struct block_structure block_structure;

  block_structure.block_name = block_name;
  return ((struct block_structure **)
	  find_hash_table_entry (block_structure_table,
				 (hash_table_entry) & block_structure,
				 reservation_is_needed));
}

/* The variable contains pointer to block_structure corresponding to the
   block being processed (if the block being processed is local module than
   more exactly to procedure or compilation unit containing the local module).
   Remember that block_structure does not exist for local modules.  Because
   necessity of local module code is determined only by the necessity of code
   of procedure or compilation unit which contains the local module. */

static struct block_structure *current_block_structure;

/* The functions set up current_block_structure (see commentaries upper) for
   BLOCK.  If the corresponding current_block_structure does not
   exist it is created. BLOCK is to refer to any module or
   procedure. */

static void
set_current_block_structure (block)
     ICN_pointer block;
{
  register struct block_structure **entry_ptr;
  register char *block_structure_memory;
  VLS unique_name;

  block = declaration_level_scope (block);
  VLS_CREATE (unique_name, 100);
  make_unique_object_name (&unique_name, block);
  entry_ptr = find_block_structure_table_entry (VLS_BEGIN (unique_name), TRUE);
  if (*entry_ptr == NULL)
    {
      M2C_ALLOC (block_structure_memory, sizeof (**entry_ptr));
      *entry_ptr = (struct block_structure *) block_structure_memory;
      (*entry_ptr)->first_block_link = NULL;
      M2C_ALLOC ((*entry_ptr)->block_name, VLS_LENGTH (unique_name));
      strcpy ((*entry_ptr)->block_name, VLS_BEGIN (unique_name));
      (*entry_ptr)->block_may_be_used = SCOPE (block) == NULL;
    }
  VLS_DELETE (unique_name);
  current_block_structure = (*entry_ptr);
}

/* The function inserts block_link from the current block (i.e block
   corresponding to current_block_structure) to block BLOCK (if block BLOCK is
   local module than more exactly to procedure or compilation unit containing
   the local module).  BLOCK is to refer to any module or procedure. */

static void
insert_block_link (block)
     ICN_pointer block;
{
  register struct block_structure **entry_ptr;
  VLS unique_name;
  struct block_link *block_link_ptr;
  char *block_structure_memory;

  block = declaration_level_scope (block);
  VLS_CREATE (unique_name, 100);
  make_unique_object_name (&unique_name, block);
  entry_ptr = find_block_structure_table_entry (VLS_BEGIN (unique_name), TRUE);
  if (*entry_ptr == NULL)
    /* It is new block_structure. */
    {
      M2C_ALLOC (block_structure_memory, sizeof (**entry_ptr));
      *entry_ptr = (struct block_structure *) block_structure_memory;
      (*entry_ptr)->first_block_link = NULL;
      M2C_ALLOC ((*entry_ptr)->block_name, VLS_LENGTH (unique_name));
      strcpy ((*entry_ptr)->block_name, VLS_BEGIN (unique_name));
      /* Compilation unit block is always needed. */
      (*entry_ptr)->block_may_be_used = SCOPE (block) == NULL;
    }
  VLS_DELETE (unique_name);
  for (block_link_ptr = current_block_structure->first_block_link;
       block_link_ptr != NULL && block_link_ptr->block != (*entry_ptr);)
    block_link_ptr = block_link_ptr->next_block_link;
  if (block_link_ptr == NULL)
    /* Link to block BLOCK from current block does not exist yet.  Add the link
       at the end of the block link list. */
    {
      M2C_ALLOC (block_structure_memory, sizeof (struct block_link));
      block_link_ptr = (struct block_link *) block_structure_memory;
      block_link_ptr->block = (*entry_ptr);
      block_link_ptr->next_block_link
	= current_block_structure->first_block_link;
      current_block_structure->first_block_link = block_link_ptr;
    }
}

/* The recursive function is used for determination of necessary blocks by
   passing through all block links from block ORIGINAL_BLOCK the necessity of
   which has been determined. */

static void
process_block_links (original_block)
     register struct block_structure *original_block;
{
  register struct block_link *block_link;

  if (original_block->block_may_be_used)
    return;
  original_block->block_may_be_used = TRUE;
  for (block_link = original_block->first_block_link;
       block_link != NULL;
       block_link = block_link->next_block_link)
    process_block_links (block_link->block);
}

/* The function determines which blocks in block_structure_table are to be
   present in executable program  (i.e. the block is achievable through
   references to blocks from the blocks of compilation units).  Originally
   compilation unit block is always needed. */

void
define_necessary_blocks ()
{
  register unsigned int i;
  register struct block_structure *block_structure_ptr;

  for (i = 0; i < block_structure_table->size; i++)
    {
      block_structure_ptr
	= (struct block_structure *) block_structure_table->entries[i];
      if (block_structure_ptr != NULL
	  && block_structure_ptr->block_may_be_used)
	{
	  block_structure_ptr->block_may_be_used = FALSE;
	  process_block_links (block_structure_ptr);
	}
    }
}

/* The recursive function processes expression EXPR to find reference to a
   block (procedure) and to insert the corresponding block link for the
   current block (i.e block corresponding to current_block_structure). */

static void
process_expression (expr)
     register ICN_pointer expr;
{
  register ICN_pointer actual;

  if (expr == NULL)
    return;
  switch (MODE (expr))
    {
    case ICNM_INTEGER:
    case ICNM_CARDINAL:
    case ICNM_REAL:
    case ICNM_SET:
    case ICNM_STRING:
    case ICNM_VARIABLE_DECLARATION:
    case ICNM_FORMAL_PARAMETER:
    case ICNM_FIELD:
      break;
    case ICNM_PROCEDURE:
      insert_block_link (expr);
      break;
    case ICNM_LESS_THAN:
    case ICNM_GREATER_THAN:
    case ICNM_OR:
    case ICNM_AND:
    case ICNM_REAL_DIVISION:
    case ICNM_ADDITION:
    case ICNM_SUBTRACTION:
    case ICNM_MULT:
    case ICNM_DIV:
    case ICNM_MOD:
    case ICNM_EQUAL:
    case ICNM_UNEQUAL:
    case ICNM_LESS_OR_EQUAL:
    case ICNM_GREATER_OR_EQUAL:
    case ICNM_IN:
    case ICNM_QUALIFICATION:
    case ICNM_INDEXING:
    case ICNM_RANGE_IN_SET_VALUE:
      process_expression (LEFT_OPERAND (expr));
      process_expression (RIGHT_OPERAND (expr));
      break;
    case ICNM_SIGN_CONVERSION:
    case ICNM_NOT:
    case ICNM_DEREFERENCING:
    case ICNM_ELEMENT_IN_SET_VALUE:
      process_expression (OPERAND (expr));
      break;
    case ICNM_FUNCTION_CALL:
      process_expression (FUNCTION_CALL_NODE (expr)->function_designator);
      for (actual = FUNCTION_CALL_NODE (expr)->function_actual_parameter_list;
	   actual != NULL;
	   actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
	process_expression (ACTUAL_PARAMETER_NODE (actual)
			    ->actual_parameter_expression);
      break;
    default:
      break;			/* may be C procedure, types & etc.*/
    }
}

/* The recursive function processes statement list starting with STATEMENT to
   find reference to a block (procedure) and to insert the corresponding block
   link for the current block (i.e block corresponding to
   current_block_structure).  STATEMENT is to refer to statement (or NULL). */

static void
process_statement_list (statement)
     register ICN_pointer statement;
{
  ICN_pointer if_cont, variant, actual, case_label;

  for (; statement != NULL; statement = NEXT_STATEMENT (statement))
    switch (MODE (statement))
      {
      case ICNM_BLOCK_BEGIN:
      case ICNM_RETURN_WITHOUT_RESULT:
	break;
      case ICNM_RETURN_WITH_RESULT:
	process_expression (RETURN_WITH_RESULT_NODE (statement)
			    ->returned_expression);
	break;
      case ICNM_LOOP_STATEMENT:
	process_statement_list (LOOP_STATEMENT_NODE (statement)
				->loop_statements);
	break;
      case ICNM_EXIT_STATEMENT:
	break;
      case ICNM_WHILE_STATEMENT:
	process_expression (WHILE_STATEMENT_NODE (statement)
			    ->while_expression);
	process_statement_list (WHILE_STATEMENT_NODE (statement)
				->while_statements);
	break;
      case ICNM_REPEAT_STATEMENT:
	process_statement_list (REPEAT_STATEMENT_NODE (statement)
				->repeat_statements);
	process_expression (REPEAT_STATEMENT_NODE (statement)
			    ->until_expression);
	break;
      case ICNM_IF_STATEMENT:
	process_expression (IF_STATEMENT_NODE (statement)->if_expression);
	process_statement_list (IF_STATEMENT_NODE (statement)->if_statements);
	for (if_cont = IF_STATEMENT_NODE (statement)->if_continuation;
	     if_cont != NULL && MODE (if_cont) == ICNM_ELSIF_CLAUSE;
	     if_cont = ELSIF_CLAUSE_NODE (if_cont)->elsif_continuation)
	  {
	    process_expression (ELSIF_CLAUSE_NODE (if_cont)->elsif_expression);
	    process_statement_list (ELSIF_CLAUSE_NODE (if_cont)
				    ->elsif_statements);
	  }
	if (if_cont != NULL)
	  process_statement_list (if_cont);
	break;
      case ICNM_FOR_STATEMENT:
	process_expression (FOR_STATEMENT_NODE (statement)
			    ->for_control_variable);
	process_expression (FOR_STATEMENT_NODE (statement)
			    ->for_starting_value);
	process_expression (FOR_STATEMENT_NODE (statement)->for_increment);
	process_expression (FOR_STATEMENT_NODE (statement)->for_limit);
	process_statement_list (FOR_STATEMENT_NODE (statement)
				->for_statements);
	break;
      case ICNM_WITH_STATEMENT:
	process_expression (WITH_STATEMENT_NODE (statement)->with_designator);
	process_statement_list (WITH_STATEMENT_NODE (statement)
				->with_statements);
	break;
      case ICNM_C_CODE_IN_STATEMENTS:
	break;
      case ICNM_CASE_STATEMENT:
	process_expression (CASE_STATEMENT_NODE (statement)->case_expression);
	for (variant
	     = CASE_STATEMENT_NODE (statement)->case_statement_variant_list;
	     MODE (variant) == ICNM_CASE_STATEMENT_VARIANT;
	     variant
	     = CASE_STATEMENT_VARIANT_NODE (variant)->next_case_variant)
	  {
	    for (case_label
		 = CASE_STATEMENT_VARIANT_NODE (variant)->case_label_list;
		 case_label != NULL;
		 case_label = CASE_LABEL_LIST (case_label))
	      if (MODE (case_label) == ICNM_CASE_STATEMENT_LABEL_ELEMENT)
		process_expression
		  (CASE_STATEMENT_LABEL_ELEMENT_NODE (case_label)
		   ->case_label_value);
	      else
		{
		  process_expression
		    (CASE_STATEMENT_LABEL_RANGE_NODE (case_label)
		     ->case_label_range_left_bound);
		  process_expression
		    (CASE_STATEMENT_LABEL_RANGE_NODE (case_label)
		     ->case_label_range_right_bound);
		}
	    process_statement_list (CASE_STATEMENT_VARIANT_NODE (variant)
				    ->case_variant_elements);
	  }
	break;
      case ICNM_ASSIGNMENT:
	process_expression (ASSIGNMENT_NODE (statement)->assignment_variable);
	process_expression (ASSIGNMENT_NODE (statement)
			    ->assignment_expression);
	break;
      case ICNM_PROCEDURE_CALL:
	process_expression (PROCEDURE_CALL_NODE (statement)
			    ->procedure_designator);
	for (actual = (PROCEDURE_CALL_NODE (statement)
		       ->procedure_actual_parameter_list);
	     actual != NULL;
	     actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
	  process_expression (ACTUAL_PARAMETER_NODE (actual)
			      ->actual_parameter_expression);
	break;
      default:
	abort ();
      }
}

/* The function returns TRUE if BLOCK is needed in executable program or
   flag `-all' is not given.  BLOCK is to refer to compilation unit or
   procedure.  If procedure code is necessary for the program then all local
   modules contained in the procedure are also necessary for the program. */

static int
block_code_may_be_used (block)
     ICN_pointer block;
{
  VLS unique_name;
  register struct block_structure **entry;

  if (!pass_of_generation_when_all_flag)
    return TRUE;
  VLS_CREATE (unique_name, 100);
  make_unique_object_name (&unique_name, block);
  entry = find_block_structure_table_entry (VLS_BEGIN (unique_name), FALSE);
  VLS_DELETE (unique_name);
  return *entry != NULL && (*entry)->block_may_be_used;
}



/* This page contains main function of the generator. */


/* The variable contains place where the generator finishes its work. */

static jmp_buf jump_to_finish;

/* The variable contains code of generation (0 - success). */

static int code_of_generation;

/* The function closes output file and finishes generation with CODE
   (0 - success). */

static void
finish_generation (code)
     int code;
{
  if (output_file != NULL)
    {
      if (fclose (output_file) != 0)
	{
	  output_file = NULL;
	  error_with_parameter (ERR_file_closing, modula_output_file_name);
	}
    }
  code_of_generation = code;
  longjmp (jump_to_finish, TRUE);
}

/* This is main function of Modula-2 generator of C code.  Object file name
   of current compilation unit is OUTPUT_FILE_NAME.  The function returns TRUE
   if the generation is successful. */

int
generator (output_file_name)
     char *output_file_name;
{
  ICN_pointer ref, scref;

  modula_output_file_name = output_file_name;
  if (setjmp (jump_to_finish))
    return !code_of_generation;
  output_file = NULL;
  finish_procedure = finish_generation;	/* for fatal error processing */
  if (pass_of_picking_used_objects_when_all_flag)
    /* Pick up of dependences between all objects. */
    {
      ref = first_block_begin ();
      for (;;)
	{
	  if (ref == NULL)
	    break;
	  scref = BLOCK_BEGIN_NODE (ref)->block_scope;
	  BLOCK_BEGIN (scref) = ref;
	  set_current_block_structure (scref);
	  process_statement_list (ref);
	  ref = BLOCK_BEGIN_NODE (ref)->next_block_begin;
	}
    }
  else
    {
      output_file = fopen (modula_output_file_name, "w");
      if (output_file == NULL)
	error_with_parameter (ERR_object_file_opening,
			      modula_output_file_name);
      /* runtime library */
      if (fprintf (output_file, "#include \"m2lib.h\"\n") < 0)
	output_file_error();
      /* for gdb, dbx. */
      if (fprintf (output_file, "#line 1 \"%s\"\n#line 1 \"%s\"\n",
		   input_file_name, modula_output_file_name) < 0)
	output_file_error ();
      generate_all_declarations (current_compilation_unit);
      ref = first_block_begin ();
      for (;;)
	{
	  if (ref == NULL)
	    break;
	  scref = BLOCK_BEGIN_NODE (ref)->block_scope;
	  BLOCK_BEGIN (scref) = ref;
	  if (MODE (scref) != ICNM_MODULE
	      || ((enum module_mode) MODULE_NODE (scref)->module_mode
		  != MM_LOCAL_MODULE))
	    {
	      execute_string_constants_declarations_generation_pass (scref);
	      generate_block (scref);
	    }
	  ref = BLOCK_BEGIN_NODE (ref)->next_block_begin;
	}
    }
  finish_generation (0);
}
