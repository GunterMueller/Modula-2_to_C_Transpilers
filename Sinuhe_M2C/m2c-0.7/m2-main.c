/* Driver of Modula-2 translator.
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
#include "m2-syntax.h"
#include "m2-semantics.h"
#include "m2-generator.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

/* Begin of declarations of some flags of Modula-2 translator. */

/* Compilation of all program modules with
   removing unused procedures (-all). */

static int all_flag;

/* Only compilation of Modula-2 modules to C code (-C). */

static int C_flag;

/* Only output make dependence for each Modula-2 module
   to standard output (-CM). */

static int CM_flag;

/* Only one Modula-2 module must be on command line.
   In addition to this requirement the module must be main module.
   All program modules in the current directory are processed as in
   update option case.  Object files of the program which is placed in other
   directories are used for loader execution time (if linkage is specified).
   Modula-2 translator assumes that implementation module and its object files
   is placed in the relative definition module directory. */

static int make_flag;

/* Output of make dependence lines that describes processing modules
   as in `-make' option specification case.  None files are generated. */

static int MAKE_flag;

/* Update of any modula module output (-update). */

static int update_flag;

/* Output of command lines of processes loaded (-v). */

static int v_flag;

/* Prohibit language extensions (-strict). */

int strict_flag;

/* The translator implements Modula-2 described in 3rd edition of Wirth's
   book (-3).  Implicitly the translator implements the language defined in the
   4th edition.

   The differences between the 3rd and 4th editions are the following.
   -  A string consisting of n characters is of type ARRAY [0..n-1] OF CHAR in
      the 3rd edition and ARRAY [0..n] OF CHAR in the 4th edition.
   -  Opaque export is restricted to pointers and to subranges of standard
      types for the 4th edition and only to pointers for the 3rd edition.
   -  Standard functions FLOAT and CHR accept an argument of type INTEGER,
      Functions TRUNC, HIGH, ORD, and SIZE are of type INTEGER.  The 3rd
      edition use type CARDINAL instead of INTEGER in these cases.
*/

int third_edition_flag;

/* Only upper case keywords and standard identifiers (-upper-case).
   Flag `-strict' means `-upper-case'. */

int only_upper_case_flag;

/* Flag of generation of dynamic tests of
   array indexes and pointer values (-test). */

int test_flag;

/* End of declarations of some flags of Modula-2 translator. */



/* Full file name of run-time library for Modula-2 program. */

static VLS m2_run_time_library;

/* List of directories in which imported modules are searched.  The names of
   the directories are separated by semicolons.  The following variable
   contains copy of value of environment variable `m2paths' (if it is
   defined). */

static char *m2_library_paths;


/* TRUE if making all (-all flag) program and picking up objects used in
   program. */

int pass_of_picking_used_objects_when_all_flag;

/* TRUE if making all (-all flag) program and generation of object files. */

int pass_of_generation_when_all_flag;

/* TRUE if syntax parser is in regime of testing that given file is
   implementation module. */

int testing_main_module;

/* TRUE if syntax parser is in regime of picking up names of modules
   explicitly imported by compiled module. */

int picking_names_of_imported_modules;

static void initiate_m2_libraries ();
static void initiate_names_of_imported_files ();
static long file_modification_time ();
static void set_user_signal_action ();
static void set_floating_point_exception_action ();
static void m2c_exit ();
static int make_MAKE ();
static int make_make ();
static int make_all ();
static int make_dependence ();
static int make_others ();
static int Modula_to_C ();
static int C_compilation ();
static int loading_by_C_compiler ();
static int modula_analyzer ();
static int create_icode_and_analyze_declarations ();
static void create_icode_of_imported_definition_modules ();
static void fatal_analyze_finish ();

/* Name of C file which is translation of current compiled module.  It may be
   temporary file depending on given command flags.  The memory for
   the following variable is dynamically allocated for. */

static char *current_modula_output_file_name;

/* The following VLS contains pointers to names of all object files (.o)
   already created by the translator.  It may be temporary file depending on
   given command flags and used C compiler.  The memory for the object file
   names is dynamically allocated for. */

static VLS names_of_c_output_files;

/* Delete all files with names which are referred from variable
   names_of_c_output_files.  Also free memory allocated for the corresponding
   object file names. */

static void
delete_c_output_files ()
{
  char **file_name_ptr;

  if (VLS_IS_UNDEFINED (names_of_c_output_files))
    return;
  for (file_name_ptr = (char **) VLS_BEGIN (names_of_c_output_files);
       file_name_ptr <= (char **) VLS_END (names_of_c_output_files);
       file_name_ptr++)
    {
      unlink (*file_name_ptr);
      M2C_FREE (*file_name_ptr);
    }
}

/* Set up VAR value equal to the environment variable value or to
   DEFAULT_VALUE (if the corresponding environment variable is not defined). */

#define SET_VALUE_FROM_ENVIRONMENT(var, environment_var, default_value) do{\
   var = getenv (environment_var);\
   if (var == NULL) var = default_value;}while(FALSE)

int
main (int argc, char *argv[])
{
  register int i, okay;
  int option_has_parameter;
  char *option;

  /* Tune the translator on used C compiler and on command line arguments. */
  if (!command_tune (argc, argv, OPTIONS))
    {
      fprintf (stderr, "m2c: invalid option description\n");
      exit (1);
    }
  current_modula_output_file_name = NULL;
  VLS_UNDEFINE (names_of_c_output_files);
  finish_procedure = m2c_exit;	/* Define finish function for fatal errors. */
  if (argument_count == 1)
    {
      output_description_of_flags ();
      m2c_exit (1);
    }
  all_flag = FALSE;
  C_flag = FALSE;
  CM_flag = FALSE;
  make_flag = FALSE;
  MAKE_flag = FALSE;
  update_flag = FALSE;
  v_flag = FALSE;
  strict_flag = FALSE;
  third_edition_flag = FALSE;
  test_flag = FALSE;
  only_upper_case_flag = FALSE;
  /* Process all command line options. */
  for (i = next_flag (TRUE), okay = TRUE; i != 0; i = next_flag (FALSE))
    {
      option = flag_characteristics (i, &option_has_parameter);
      if (option == NULL)
	{
	  if (option_has_parameter)
	    fprintf (stderr, "m2c: flag `%s' without parameter\n",
		     ((char **) VLS_BEGIN (argument_vector))[i]);
	  else
	    fprintf (stderr, "m2c: unknown flag `%s'\n",
		     ((char **) VLS_BEGIN (argument_vector))[i]);
	  okay = FALSE;
	}
      else if (strcmp (option, "-strict") == 0)
	strict_flag = TRUE;
      else if (strcmp (option, "-3") == 0)
	third_edition_flag = TRUE;
      else if (strcmp (option, "-all") == 0)
	all_flag = TRUE;
      else if (strcmp (option, "-C") == 0)
	C_flag = TRUE;
      else if (strcmp (option, "-CM") == 0)
	CM_flag = TRUE;
      else if (strcmp (option, "-make") == 0)
	make_flag = TRUE;
      else if (strcmp (option, "-MAKE") == 0)
	MAKE_flag = TRUE;
      else if (strcmp (option, "-test") == 0)
	test_flag = TRUE;
      else if (strcmp (option, "-update") == 0)
	update_flag = TRUE;
      else if (strcmp (option, "-upper-case") == 0)
	only_upper_case_flag = TRUE;
      else if (strcmp (option, "-v") == 0)
	v_flag = TRUE;
    }
  if (strict_flag)
    only_upper_case_flag = TRUE;
  if (v_flag)
    fprintf (stderr, "m2c version 0.4\n");
  if (!okay)
    m2c_exit (1);		/* Error on command line options. */
  SET_VALUE_FROM_ENVIRONMENT (m2_library_paths,
			      ENVIRONMENT_LIBRARY_PATHS_VARIABLE, "");
  VLS_CREATE (m2_run_time_library, 100);
  VLS_ADD_STRING (m2_run_time_library, STANDARD_LIBRARY_DIRECTORY);
  if (VLS_LENGTH (m2_run_time_library) != 0)
    VLS_ADD_STRING (m2_run_time_library, "/");
  VLS_ADD_STRING (m2_run_time_library, M2_RUN_TIME_LIBRARY);
  initiate_analyzer_data ();
  initiate_m2_libraries ();
  initiate_names_of_imported_files ();
  /* Set up action on some signals which may be generated by the user. */
  set_user_signal_action (TRUE);
  set_floating_point_exception_action ();
  pass_of_picking_used_objects_when_all_flag = FALSE;
  pass_of_generation_when_all_flag = FALSE;
  testing_main_module = FALSE;
  picking_names_of_imported_modules = FALSE;
  /* Test conformities of options and do rest of the translator work. */
  if (MAKE_flag)
    {
      if (make_flag || all_flag || update_flag || CM_flag)
	fprintf (stderr,
		 "m2c warning: `-MAKE' ignores `-make', `-all', `-update'\
 and `-CM' flags\n");
      okay = make_MAKE ();
    }
  else if (make_flag)
    {
      if (all_flag || update_flag || CM_flag)
	fprintf (stderr,
		 "m2c warning: `-make' flag ignores `-all', `-update'\
 and `-CM' flags\n");
      okay = make_make ();
    }
  else if (all_flag)
    {
      if (update_flag || CM_flag)
	fprintf (stderr,
		 "m2c warning: `-all' flag ignores `-update' and `-CM' flags\n");
      okay = make_all ();
    }
  else if (CM_flag)
    {
      if (C_flag || update_flag)
	fprintf (stderr,
		 "m2c warning: `-CM' flag ignores '-update' and `-C' flags\n");
      okay = make_dependence ();
    }
  else
    okay = make_others ();
  m2c_exit ((okay ? 0 : 1));
}



/* Code for defining input and output file names for Modula-2 and C.
   Mainly it is depended on configuration files. */


/* Suffixes of files which the translator considers as C source files.
   The following variable value is not changed. */

static char *c_input_suffixes[] = C_INPUT_SUFFIXES;

/* Suffixes of files which the translator considers as Modula-2 source files.
   The following variable value is not changed. */

static char *modula_input_suffixes[] = MODULA_INPUT_SUFFIXES;

/* Return TRUE if string SUFFIX is in array INPUT_SUFFIXES.  The array length
   is equal to NUMBER_OF_SUFFIXES. */

static int
it_is_input_suffix (suffix, input_suffixes, number_of_suffixes)
     char *suffix;
     register char **input_suffixes;
     int number_of_suffixes;
{
  register int i;

  for (i = 0; i < number_of_suffixes; i++)
    if (strcmp (suffix, input_suffixes[i]) == 0)
      return TRUE;
  return FALSE;
}


/* Return pointer to dynamicly allocated memory containing name of
   file which is translation of Modula-2 source file MODULA_FILE_NAME.
   The result is depended on command line options and the used C compiler. */

static char *
modula_output_file_name (modula_file_name)
     register char *modula_file_name;
{
  register char *str;
  VLS resultant_file_name;

  VLS_CREATE (resultant_file_name, 100);
  if (IS_MODULA_OUTPUT_TEMPORARY)
    {
      char temporary_file_name[20];
      VLS_ADD_STRING (resultant_file_name, TEMPORARY_DIRECTORY);
      if (VLS_LENGTH (resultant_file_name) != 0)
	VLS_ADD_STRING (resultant_file_name, "/");
      sprintf (temporary_file_name, "m2c%05.5d", getpid ());
      VLS_ADD_STRING (resultant_file_name, temporary_file_name);
      VLS_ADD_STRING (resultant_file_name, MODULA_OUTPUT_SUFFIX);
    }
  else
    {
      str = NONSTANDARD_MODULA_OUTPUT_NAME;
      if (str != NULL)
	VLS_ADD_STRING (resultant_file_name, str);
      else
	{
	  VLS_ADD_STRING (resultant_file_name, base_file_name (modula_file_name));
	  VLS_ADD_STRING (resultant_file_name, MODULA_OUTPUT_SUFFIX);
	}
    }
  M2C_ALLOC (str, VLS_LENGTH (resultant_file_name));
  strcpy (str, VLS_BEGIN (resultant_file_name));
  VLS_DELETE (resultant_file_name);
  return str;
}

/* Return pointer to dynamicly allocated memory containing name of
   file which is output of C compiler (if the corresponding input file is
   SOURCE_NAME).  The result is depended on command line options and
   the used C compiler. */

static char *
c_output_file_name (source_name)
     char *source_name;
{
  register char *str;
  char *resultant_file_name;

  str = NONSTANDARD_C_OUTPUT_NAME;
  if (str != NULL)
    {
      M2C_ALLOC (resultant_file_name, strlen (str) + 1);
      strcpy (resultant_file_name, str);
    }
  else
    {
      str = base_file_name (source_name);
      M2C_ALLOC (resultant_file_name,
		 strlen (str) + strlen (C_OUTPUT_SUFFIX) + 1);
      strcpy (resultant_file_name, str);
      strcat (resultant_file_name, C_OUTPUT_SUFFIX);
    }
  return resultant_file_name;
}



/* This page contains functions for work with names of Modula-2 files which are
   implicitly or explicitly imported by current compiled module. */


/* The following VLS contains pointers to names of all Modula-2 files which
   are implicitly or explicitly imported by current compiled module.  The first
   pointer refers to the current compiled module file name the rest pointers
   refer to imported definition module file names.  The memory for file names
   is dynamically allocated for. */

static VLS names_of_imported_files;

/* The following function is to be called only once. */

static void
initiate_names_of_imported_files ()
{
  VLS_CREATE (names_of_imported_files, 500);
}

/* The following function is to be called from the start of picking files
   imported by the current compiled module. The function frees memory allocated
   for the file names from pred invocation to it. */

static void
start_picking_names_of_imported_files ()
{
  register unsigned int i;

  for (i = 0; i < VLS_LENGTH (names_of_imported_files) / sizeof (char *); i++)
    M2C_FREE (((char **) VLS_BEGIN (names_of_imported_files))[i]);
  VLS_NULLIFY (names_of_imported_files);
}

/* If FILE_NAME is already in NAMES_OF_IMPORTED_FILES then the function does
   nothing.  In other case the function allocates dynamically memory for
   FILE_NAME, copys it to this memory and adds pointer to the memory at the
   end of NAMES_OF_IMPORTED_FILES. */

static void
include_name_of_imported_file (file_name)
     char *file_name;
{
  register unsigned int i;
  char *str;

  for (i = 0; i < VLS_LENGTH (names_of_imported_files) / sizeof (char *); i++)
    if (strcmp (file_name, ((char **) VLS_BEGIN (names_of_imported_files))[i])
	== 0)
      break;
  if (i == VLS_LENGTH (names_of_imported_files) / sizeof (char *))
    {
      M2C_ALLOC (str, strlen (file_name) +1);
      strcpy (str, file_name);
      VLS_ADD_MEMORY (names_of_imported_files, &str, sizeof (char *));
    }
}

/* The function return name of imported file with number file_name_number in
   NAMES_OF_IMPORTED_FILES. */

static char *
name_of_imported_file (file_name_number)
     int file_name_number;
{
  return ((char **) VLS_BEGIN (names_of_imported_files))[file_name_number];
}



/* This page contains functions which processes command line argument according
   to given option.  Names of these functions start with word `make'.
   There are auxiliary functions (needed for make functions) on the page. */

/* The following function returns TRUE if file with given
   PARAMETER_NUMBER on the command line starts with keyword `MAIN' and
   returns FALSE otherwise. */

static int
test_main_module (parameter_number)
     int parameter_number;
{
  register int okay;

  if (strcmp (file_name_suffix (((char **) VLS_BEGIN (argument_vector))
				[parameter_number]),
	      MAIN_MODULE_SUFFIX) == 0)
    {
      /* New syntax parser regime is on. */
      testing_main_module = TRUE;
      okay = Modula_to_C (parameter_number);
      /* New syntax parser regime is off. */
      testing_main_module = FALSE;
      return okay && it_is_main_module;
    }
  return FALSE;
}

/* The following function is to be used when there are options `-MAKE', `-make'
   or `-all' on the command line.  The function tests that the command line
   does not contain names of definition or implementation modules and
   that there is the only main module name on the command line.  If this
   condition is true then the function returns number of the main module name
   on the command line.  Otherwise the function outputs warning and
   returns 0. */

static int
test_and_return_main_module ()
{
  register int parameter_number, main_module_number;
  register char *suffix;

  for (main_module_number = 0, parameter_number = next_parameter (TRUE);
       parameter_number != 0;
       parameter_number = next_parameter (FALSE))
    {
      suffix
	= file_name_suffix
	(((char **) VLS_BEGIN (argument_vector))[parameter_number]);
      if (it_is_input_suffix (suffix, modula_input_suffixes,
			      sizeof (modula_input_suffixes)
			      / sizeof (char *)))
	{
	  if (main_module_number == 0 && test_main_module (parameter_number))
	    main_module_number = parameter_number;
	  else
	    {
	      main_module_number = 0;
	      break;
	    }
	}
    }
  if (main_module_number == 0)
    {
      fprintf (stderr,
	       "m2c warning: The only main module must be given when\
 `-MAKE', `-make', `-all'\n"
	);
      m2c_exit (1);
    }
  return main_module_number;
}

/* The following function picks up names of all modules imported implicitly
   or explicitly by the module whose name has PARAMETER_NUMBER on the
   command line.  For that the import clauses of the corresponding definition
   and implementation modules are examined.  The function return TRUE if errors
   were not fixed and FALSE otherwise. */

static int
pick_import (parameter_number)
     int parameter_number;
{
  register int okay;

  start_picking_names_of_imported_files ();
  include_name_of_imported_file
    (((char **) VLS_BEGIN (argument_vector))[parameter_number]);
  /* New syntax parser regime is on. */
  picking_names_of_imported_modules = TRUE;
  okay = Modula_to_C (parameter_number);
  /* New syntax parser regime is off. */
  picking_names_of_imported_modules = FALSE;
  return okay;
}

/* The function adds all picked up names of imported implementation modules to
   the argument list containing NUMBER arguments after START_MARK-th argument.
   The function returns new length of the list.  Imported module will be added
   to the end of the list only if its name is absent in the list.

   This function used when there is `-make' option on the command line. */

static int
add_import_to_argument (start_mark, number)
     register int start_mark, number;
{
  register char *str, *name_without_suffix;
  register int i, length;
  register unsigned int imported_module_number;

  for (imported_module_number = 1;
       imported_module_number < (VLS_LENGTH (names_of_imported_files)
				 / sizeof (char *));
       imported_module_number++)
    {
      name_without_suffix
	= file_name_without_suffix
	(name_of_imported_file (imported_module_number));
      length = strlen (name_without_suffix);
      length += strlen (IMPLEMENTATION_MODULE_SUFFIX) + 1;
      M2C_ALLOC (str, length);
      strcpy (str, name_without_suffix);
      M2C_FREE (name_without_suffix);
      strcat (str, IMPLEMENTATION_MODULE_SUFFIX);
      for (i = 1; i <= number; i++)
	if (strcmp (((char **) VLS_BEGIN (argument_vector))
		    [i + start_mark], str) == 0)
	  break;
      if (i > number)
	{
	  insert_argument (start_mark + number, str);
	  number++;
	}
      else
	M2C_FREE (str);
    }
  return number;
}

/* The following function is used when there is `-make' option on the command
   line.  The function parameters define the command line argument list which
   starts with the file name of main module (MAIN_MODULE_ARGUMENT_NUMBER) and
   contains yet NUMBER_OF_IMPLEMENTATION_MODULES file names of implementation
   modules imported by the main module.

   The functions changes suffixes of all the list file names containing
   directory component on the suffix of object files.  Remember that Modula-2
   translator assumes that implementation module and its object files
   is placed in the relative definition module directory and that
   the translator does not create object files in other directories. */

static void
change_implementation_name_to_object (main_module_argument_number,
				      number_of_implementation_modules)
     int main_module_argument_number, number_of_implementation_modules;
{
  register int argument_number, length;
  register char *name_without_suffix, *str;

  for (argument_number = main_module_argument_number;
       argument_number
       <= main_module_argument_number + number_of_implementation_modules;
       argument_number++)
    {
      name_without_suffix
	= file_name_without_suffix
	(((char **) VLS_BEGIN (argument_vector))[argument_number]);
      if (name_without_suffix != base_file_name (name_without_suffix))
	{
	  length = strlen (name_without_suffix);
	  length += strlen (OBJECT_FILE_SUFFIX) + 1;
	  M2C_ALLOC (str, length);
	  strcpy (str, name_without_suffix);
	  strcat (str, OBJECT_FILE_SUFFIX);
	  ((char **) VLS_BEGIN (argument_vector))[argument_number] = str;
	}
      M2C_FREE (name_without_suffix);
    }
}

/* The following function outputs (stdout) one make entry that specify
   dependencies of the object files.  All names of object files does not
   contain directory component and are determined from the corresponding
   file names picked up in NAMES_OF_IMPORTED_FILES (see its structure). */

static void
print_module_dependence ()
{
  register int line_long, length;
  register unsigned int module_number;
  register char *str, *name_without_suffix;

  name_without_suffix = file_name_without_suffix (name_of_imported_file (0));
  str = base_file_name (name_without_suffix);
  if (printf ("%s%s:", str, OBJECT_FILE_SUFFIX) < 0)
    error (ERR_output_to_stdout);
  line_long = strlen (str) + strlen (OBJECT_FILE_SUFFIX) + 1;
  M2C_FREE (name_without_suffix);
  for (module_number = 0;
       module_number < VLS_LENGTH (names_of_imported_files) / sizeof (char *);
       module_number++)
    {
      str = name_of_imported_file (module_number);
      length = strlen (str);
      if (module_number != 0 && line_long + length > 75)
	{
	  line_long = 5;
	  fputs ("\\\n     ", stdout);
	}
      fputc (' ', stdout);
      fputs (str, stdout);
      line_long += length + 1;
    }
  fputc ('\n', stdout);
}


/* The following function processes command line arguments according to
   option `-MAKE'.  The function returns TRUE if processing is successful. */

static int
make_MAKE ()
{
  register char *str, *name_without_suffix;
  register int argument_number, line_long, length, main_module_argument_number;
  register int command_line_is_correct, import_clauses_are_correct;
  register int number_of_imported_files;

  main_module_argument_number = test_and_return_main_module ();
  command_line_is_correct = main_module_argument_number != 0;
  if (command_line_is_correct)
    {
      for (number_of_imported_files = 0,
	   argument_number = main_module_argument_number;
	   argument_number
	   <= main_module_argument_number + number_of_imported_files;
	   argument_number++)
	{
	  import_clauses_are_correct = pick_import (argument_number);
	  if (import_clauses_are_correct)
	    {
	      number_of_imported_files
		= add_import_to_argument (main_module_argument_number,
					  number_of_imported_files);
	      str = name_of_imported_file (0);
	      if (str == base_file_name (str))
		print_module_dependence ();
	    }
	}
      change_implementation_name_to_object (main_module_argument_number,
					    number_of_imported_files);
      name_without_suffix
	= file_name_without_suffix
	(
	  ((char **) VLS_BEGIN (argument_vector))[main_module_argument_number]
	);
      if (name_without_suffix == base_file_name (name_without_suffix))
	{
	  if (printf ("%s:", name_without_suffix) < 0)
	    error (ERR_output_to_stdout);
	  line_long = strlen (name_without_suffix) + 1;
	  for (argument_number = main_module_argument_number;
	       argument_number
	       <= main_module_argument_number + number_of_imported_files;
	       argument_number++)
	    {
	      str
		= file_name_without_suffix
		(((char **) VLS_BEGIN (argument_vector))[argument_number]);
	      length = strlen (str) + strlen (OBJECT_FILE_SUFFIX);
	      if (line_long + length > 75)
		{
		  line_long = 5;
		  fputs ("\\\n     ", stdout);
		}
	      if (printf (" %s%s", str, OBJECT_FILE_SUFFIX) < 0)
		error (ERR_output_to_stdout);
	      line_long += length + 1;
	      M2C_FREE (str);
	    }
	  fputc ('\n', stdout);
	}
      M2C_FREE (name_without_suffix);
    }
  return command_line_is_correct;
}

/* The following function processes command line arguments according to
   option `-make'.  The function returns TRUE if processing is successful. */

static int
make_make ()
{
  int argument_number, main_module_argument_number, command_line_is_correct;
  int import_clauses_are_correct, number_of_imported_files;

  main_module_argument_number = test_and_return_main_module ();
  command_line_is_correct = main_module_argument_number != 0;
  if (command_line_is_correct)
    {
      for (number_of_imported_files = 0,
	   argument_number = main_module_argument_number;
	   argument_number
	   <= main_module_argument_number + number_of_imported_files;
	   argument_number++)
	{
	  import_clauses_are_correct = pick_import (argument_number);
	  if (import_clauses_are_correct)
	    number_of_imported_files
	      = add_import_to_argument (main_module_argument_number,
					number_of_imported_files);
	}
      change_implementation_name_to_object (main_module_argument_number,
					    number_of_imported_files);
      update_flag = TRUE;
      command_line_is_correct = make_others ();
    }
  return command_line_is_correct;
}

/* The following function processes command line arguments according to
   option `-all'.  The function returns TRUE if processing is successful. */

static int
make_all ()
{
  int argument_number, main_module_argument_number, okay;
  int import_clauses_are_correct, number_of_imported_files;

  main_module_argument_number = test_and_return_main_module ();
  okay = main_module_argument_number != 0;
  if (okay)
    {
      for (number_of_imported_files = 0,
	   argument_number = main_module_argument_number;
	   argument_number
	   <= main_module_argument_number + number_of_imported_files;
	   argument_number++)
	{
	  import_clauses_are_correct = pick_import (argument_number);
	  if (import_clauses_are_correct)
	    number_of_imported_files
	      = add_import_to_argument (main_module_argument_number,
					number_of_imported_files);
	}
      /* New analyzer regime is on.  Full name of all program objects and
	 dependences between them are picked up. */
      pass_of_picking_used_objects_when_all_flag = TRUE;
      create_block_structure_table ();
      for (argument_number = main_module_argument_number;
	   argument_number
	   <= main_module_argument_number + number_of_imported_files;
	   argument_number++)
	/* First pass on all program modules. */
	okay = Modula_to_C (argument_number);
      pass_of_picking_used_objects_when_all_flag = FALSE;
      /* New analyzer regime is off. */
      pass_of_generation_when_all_flag = TRUE;
      define_necessary_blocks ();
      /* Second pass on all program modules. */
      okay = make_others ();
    }
  return okay;
}

/* The following function processes command line arguments according to
   option `-CM'.  The function returns TRUE if processing is successful. */

static int
make_dependence ()
{
  register int argument_number, okay, import_clauses_are_correct;
  register char *suffix;

  for (argument_number = next_parameter (TRUE), okay = TRUE;
       argument_number != 0;
       argument_number = next_parameter (FALSE))
    {
      suffix =
	file_name_suffix
	(((char **) VLS_BEGIN (argument_vector))[argument_number]);
      if (it_is_input_suffix (suffix, modula_input_suffixes,
			      sizeof (modula_input_suffixes)
			      / sizeof (char *)))
	{
	  import_clauses_are_correct = pick_import (argument_number);
	  okay = okay && import_clauses_are_correct;
	  if (import_clauses_are_correct)
	    print_module_dependence ();
	}
    }
  return okay;
}

/* The following function return TRUE if output file is obsolete or does not
   exist.  Depending on options the output file may be C file or object file.
   The output file is obsolete if the corresponding implementation
   (ARGUMENT_NUMBER_OF_MODULE) or definition module or any imported definition
   module has been changed after the output file creation (or modification). */

static int
object_file_is_obsolete (argument_number_of_module)
     register int argument_number_of_module;
{
  long modification_time, max_modification_time_of_imported_modules;
  register int result;
  register unsigned int number_of_imported_file;
  char *str, *name_without_suffix;

  result = TRUE;
  if (pick_import (argument_number_of_module))
    {
      for (max_modification_time_of_imported_modules = 0,
	   number_of_imported_file = 0;
	   number_of_imported_file < (VLS_LENGTH (names_of_imported_files)
				      / sizeof (char *));
	   number_of_imported_file++)
	{
	  modification_time
	    = file_modification_time
	    (name_of_imported_file (number_of_imported_file));
	  if (modification_time == 0)
	    break;
	  if (modification_time > max_modification_time_of_imported_modules)
	    max_modification_time_of_imported_modules = modification_time;
	}
      if (modification_time != 0)
	{			/* All source files exist. */
	  if (!IS_MODULA_OUTPUT_DELETED)
	    {
	      name_without_suffix
		= file_name_without_suffix
		(((char **) VLS_BEGIN (argument_vector))
		 [argument_number_of_module]);
	      str = modula_output_file_name (name_without_suffix);
	      M2C_FREE (name_without_suffix);
	      /* Remember: modification_time==0 when the file doesn't exist. */
	      modification_time = file_modification_time (str);
	      if (modification_time
		  > max_modification_time_of_imported_modules)
		{
		  result = FALSE;
		  max_modification_time_of_imported_modules
		    = modification_time;
		}
	      M2C_FREE (str);
	    }
	  if (C_COMPILATION_GUARD && C_OUTPUT_EXISTS)
	    {
	      name_without_suffix
		= file_name_without_suffix
		(((char **) VLS_BEGIN (argument_vector))
		 [argument_number_of_module]);
	      str = c_output_file_name (name_without_suffix);
	      M2C_FREE (name_without_suffix);
	      result = (file_modification_time (str)
			<= max_modification_time_of_imported_modules);
	      if (!result)
		VLS_ADD_MEMORY (names_of_c_output_files, &str, sizeof (char *));
	      else
		M2C_FREE (str);
	    }
	}
    }
  return result;
}

/* The following function processes command line arguments when
   any mentioned above option is absent on command line.  The function
   returns TRUE if processing is successful. */

static int
make_others ()
{
  register int argument_number;
  register int all_right, okay_after_Modula_to_C, okay_after_C_to_object;
  register char *suffix;

  VLS_CREATE (names_of_c_output_files, 100);
  for (argument_number = next_parameter (TRUE), all_right = TRUE;
       argument_number != 0;
       argument_number = next_parameter (FALSE))
    {
      suffix
	= file_name_suffix
	(((char **) VLS_BEGIN (argument_vector))[argument_number]);
      if (it_is_input_suffix (suffix, modula_input_suffixes,
			      sizeof (modula_input_suffixes)
			      / sizeof (char *)))
	{
	  if (!update_flag || strcmp (suffix, DEFINITION_MODULE_SUFFIX) == 0
	      || object_file_is_obsolete (argument_number))
	    {
	      okay_after_Modula_to_C = Modula_to_C (argument_number);
	      all_right = all_right && okay_after_Modula_to_C;
	      if (okay_after_Modula_to_C && C_COMPILATION_GUARD
		  && strcmp (suffix, DEFINITION_MODULE_SUFFIX) != 0)
		{
		  okay_after_C_to_object
		    = C_compilation (TRUE, current_modula_output_file_name,
				     argument_number);
		  all_right = all_right && okay_after_C_to_object;
		}
	      if (current_modula_output_file_name != NULL)
		{
		  if (!okay_after_Modula_to_C || IS_MODULA_OUTPUT_DELETED)
		    unlink (current_modula_output_file_name);
		  M2C_FREE (current_modula_output_file_name);
		  current_modula_output_file_name = NULL;
		}
	    }
	}
      else if (C_COMPILATION_GUARD
	       && it_is_input_suffix (suffix, c_input_suffixes,
				      sizeof (c_input_suffixes)
				      / sizeof (char *)))
	{
	  okay_after_C_to_object
	    = C_compilation (FALSE,
			     ((char **) VLS_BEGIN (argument_vector))[argument_number],
			     argument_number);
	  all_right = all_right && okay_after_C_to_object;
	}
    }
  if (all_right && C_COMPILATION_GUARD && LOAD_GUARD)
    all_right = loading_by_C_compiler ();
  return all_right;
}



/* This page contains auxiliary functions for access to file attributes,
   process of signals, for the translator finish and for work with Unix
   processes. */


/* The following function returns time of last modification of file FILE_NAME.
   If the file does not exist the function returns 0. */

static long
file_modification_time (file_name)
     char *file_name;
{
  struct stat buf;
  long modification_time;

  if (stat (file_name, &buf) == -1)
    return 0;
  else
    return buf.st_mtime;
}

/* The two following functions are used for implementation of critical regions
   by putting of processing user signals (termination and interrupt). */

/* If value of the following variable TRUE then user signal will be put off. */

static int signal_ignore;

/* Value of the following variable is last user signal put off.  Only
   the last signal will be processed. */

static int last_signal;

/* The following function switches on regime of ignoring user signal. */

static void
signal_disable ()
{
  signal_ignore = TRUE;
}

/* The following function switches off regime of ignoring user signal and
   sends the last ignored user signal. */

static void
signal_enable ()
{
  if (signal_ignore && last_signal)
    {
      signal_ignore = FALSE;
      if (kill (getpid (), last_signal) == -1)
	abort ();
    }
  else
    signal_ignore = FALSE;
}

/* The following function is signal handler on user signals  interrupt and
   termination. */

static void
user_signal_action (sig)
     int sig;
{
  if (signal_ignore)
    {
      last_signal = sig;
      set_user_signal_action (FALSE);
    }
  else
    m2c_exit (sig);
}

/* The following function sets up signal handler on user signals
   interrupt and termination.  INITIATION_FLAG is to be TRUE for the first call
   of the function. */

static void
set_user_signal_action (initiation_flag)
     int initiation_flag;
{
  if (initiation_flag)
    {
      signal_ignore = FALSE;
      last_signal = 0;
    }
  if (signal (SIGINT, SIG_IGN) != SIG_IGN)
    signal (SIGINT, user_signal_action);
  if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
    signal (SIGTERM, user_signal_action);
}

/* The following function is signal handler of floating point exception. */

static void
floating_point_exception_action ()
{
  set_floating_point_exception_action ();
  error (ERR_floating_point_exception);
}

/* The following function sets up signal handler of floating point
   exception. */

static void
set_floating_point_exception_action ()
{
  if (signal (SIGFPE, SIG_IGN) != SIG_IGN)
    signal (SIGFPE, floating_point_exception_action);
}


/* The following function finishes the translator run with CODE.  The code
   equal to zero means success.  In the failure case the function delete
   last C file whose generation has not been ended. */

static void
m2c_exit (code)
     int code;
{
  if (code != 0 && current_modula_output_file_name != NULL)
    {
      unlink (current_modula_output_file_name);
      M2C_FREE (current_modula_output_file_name);
      current_modula_output_file_name = NULL;
    }
  if (IS_C_OUTPUT_DELETED)
    delete_c_output_files ();
  exit (code);
}

/* The following function waits for finish of process with PID and return
   TRUE in the case of normal process finish.  The function returns FALSE and
   finishes the translator if the process is finished unsuccessfully. */

static int
wait_for_process_finish (pid)
     register int pid;
{
  int status, signal;

  signal_disable ();
  while (pid != wait (&status)) ;
  signal = (status & 0177);
  if (signal != 0 && signal != SIGINT)
    {
      fprintf (stderr, "m2c: fatal error in son proccess\n");
      m2c_exit (1);
    }
  return !status;
}

/* The following function creates process with FILE_NAME and PROCESS_ARGUMENTS
   and return its identifier.  If the creation is unsuccessful then error
   message is output and the the translator is finished.  The command line of
   the process is output to stderr if the translator option `-v' is present.
   FILE_NAME is searched according to environment variable PATH. */

static int
new_process (file_name, process_arguments)
     char *file_name;
     register char **process_arguments;
{
  register int pid;
  register char **current_argument;

  if (v_flag)
    {
      for (current_argument = process_arguments;
	   *current_argument != NULL; current_argument++)
	fprintf (stderr, " %s", *current_argument);
      fputc ('\n', stderr);
    }
  pid = fork ();
  if (pid == -1)
    {
      perror ("m2c");
      m2c_exit (1);
    }
  if (pid == 0)
    {
      execvp (file_name, process_arguments);
      fprintf (stderr,
	       "m2c: can\'t find %s - modify shell variable PATH\n",
	       file_name);
      perror ("   ");
      m2c_exit (1);
    }
  return pid;
}

/* It is combination of new_process and wait_for_process_finish (see
   commentaries for them). */

static int
create_and_wait_for_process (file_name, process_arguments)
     char *file_name;
     register char *process_arguments[];
{
  return wait_for_process_finish (new_process (file_name, process_arguments));
}



/* Code for work with Modula-2 libraries, i.e. directories which contains
   implementation, definition and the coresponding object modules. */

/* The following VLS contains pointers to names of all directories (except
   for current directory) in which imported definitin modules are searched.
   The memory for the directory names is dynamically allocated for.

   Order of searching the directories is:
	current directory
	standard library directory
	directories given in options `-P'
	directories given in shell variables `m2paths'.
*/

static VLS m2_libraries;

/* The following function initiates variables m2_libraries.  The function
   is called only when the translator is starting. */

static void
initiate_m2_libraries ()
{
  register char *current_library_begin, *flag_name, *current_library_end_mark;
  char *library_name;
  register int i, length;
  int parameter;

  VLS_CREATE (m2_libraries, 100);
  library_name = STANDARD_LIBRARY_DIRECTORY;
  VLS_ADD_MEMORY (m2_libraries, &library_name, sizeof (char *));
  /* Process `-P...' options. */
  for (i = next_flag (TRUE); i != 0; i = next_flag (FALSE))
    {
      flag_name = flag_characteristics (i, &parameter);
      if (flag_name == NULL)
	abort ();
      if (strcmp (flag_name, "-P") == 0
	  && ((char **) VLS_BEGIN (argument_vector))[i][2] != '\0')
	{
	  length = strlen (((char **) VLS_BEGIN (argument_vector))[i] + 2);
	  M2C_ALLOC (library_name, length + 1);
	  strcpy (library_name,
		  ((char **) VLS_BEGIN (argument_vector))[i] + 2);
	  VLS_ADD_MEMORY (m2_libraries, &library_name, sizeof (char *));
	}
    }
  /* Process environment variable `m2path'. */
  current_library_begin = m2_library_paths;
  for (current_library_end_mark = current_library_begin;
       current_library_begin != NULL && *current_library_begin != '\0';
       current_library_begin = current_library_end_mark)
    {
      while (*current_library_end_mark != ';'
	     && *current_library_end_mark != '\0')
	current_library_end_mark++;
      if (current_library_begin != current_library_end_mark)
	{
	  M2C_ALLOC (library_name,
		     (unsigned) (current_library_end_mark
				 - current_library_begin + 1));
	  for (i = 0;
	       i < current_library_end_mark - current_library_begin;
	       i++)
	    library_name[i] = current_library_begin[i];
	  library_name[current_library_end_mark - current_library_begin]
	    = '\0';
	  VLS_ADD_MEMORY (m2_libraries, &library_name, sizeof (char *));
	}
      if (*current_library_end_mark == ';')
	current_library_end_mark++;
    }
}

/* The following function returns name of library in which definition
   module MODULE_NAME is placed.  The oreder of search is given in
   commentaries for m2_libraries.  If the definition module is not found
   the function returns NULL. */

static char *
definition_module_library (module_name)
     char *module_name;
{
  register char *library_name;
  register int next_library_number;
  long modification_time;
  VLS file_name;

  /* First library is current directory (LIBRARY_NAME == NULL). */
  for (library_name = NULL, next_library_number = 0;;)
    {
      VLS_CREATE (file_name, 100);
      VLS_ADD_STRING (file_name, library_name);
      /* Remember that LIBRARY_NAME is NULL or nonblank string.
	 Therefore next statement does not test root directory. */
      if (VLS_LENGTH (file_name) != 0)
	VLS_ADD_STRING (file_name, "/");
      VLS_ADD_STRING (file_name, module_name);
      VLS_ADD_STRING (file_name, DEFINITION_MODULE_SUFFIX);
      modification_time = file_modification_time (VLS_BEGIN (file_name));
      VLS_DELETE (file_name);
      if (modification_time != 0)
	break;
      else if (next_library_number
	       == VLS_LENGTH (m2_libraries) / sizeof (char *))
	{
	  /* All libraries are searched.  The corresponding definition module
	     does not exist in the libraries. */
	  library_name = NULL;
	  break;
	}
      else
	{
	  library_name
	    = ((char **) VLS_BEGIN (m2_libraries))[next_library_number];
	  next_library_number++;
	}
    }
  return library_name;
}



/* This page contains code needed for forming new command line for
   C compilation or loading with the aid of used C compiler. */


/* The following function returns flag characteristics and presentation of
   the translator flag given on command line as ARG-th argument.  Results
   are returned through formals new_flag_name, new_parameter or
   flag_is_ordered.  If IT_IS_FOR_C_COMPILATION is TRUE the the results are
   needed for C compilation.  Otherwise the results are needed for loading all
   object files by used C compiler.

   If NEW_FLAG_NAME is NULL after the call then the presentation of the
   corresponding translator flag is to be absent on command line for C
   compilation or loading.  If NEW_PARAMETER is NULL after the call then
   the presentation of the corresponding translator flag has not separate
   parameter on command line for C compilation or loading.  The order of the
   new presentation of the corresponding translator flag is to be the same
   on command line for C compilation or loading if FLAG_IS_ORDERED is TRUE.

   The memory for the formals new_flag_name, new_parameter is dynamically
   allocated for.
*/

static void
new_flag_presentation (arg, new_flag_name, new_parameter, flag_is_ordered,
		       it_is_for_C_compilation)
     char **new_flag_name, **new_parameter;
     int arg, *flag_is_ordered, it_is_for_C_compilation;
{
  char *temp_copy_of_new_flag_name, *flag_name, *temp_flag_name_copy;
  int flag_length, flag_name_length, flag_has_separate_parameter;

  flag_length = strlen (((char **) VLS_BEGIN (argument_vector))[arg]);
  flag_name = flag_characteristics (arg, &flag_has_separate_parameter);
  *flag_is_ordered = IS_ORDER_IMPORTANT (flag_name);
  flag_name_length = strlen (flag_name);
  M2C_ALLOC (temp_flag_name_copy, flag_name_length + 1);
  strcpy (temp_flag_name_copy, flag_name);
  /* Temporary copy of flag name is needed because macros TRANSLATE...
     can change it. */
  if (it_is_for_C_compilation)
    temp_copy_of_new_flag_name = TRANSLATE_FLAG_TO_C (temp_flag_name_copy);
  else
    temp_copy_of_new_flag_name = TRANSLATE_FLAG_TO_LOAD (temp_flag_name_copy);
  *new_parameter = NULL;
  *new_flag_name = NULL;
  if (temp_copy_of_new_flag_name != NULL)
    {
      M2C_ALLOC (*new_flag_name,
		 strlen (temp_copy_of_new_flag_name) + flag_length
		 - flag_name_length + 1);
      strcpy (*new_flag_name, temp_copy_of_new_flag_name);
      /* Add possible nonseparate flag parameter. */
      strcat (*new_flag_name,
	      ((char **) VLS_BEGIN (argument_vector))[arg] + flag_name_length);
      if (flag_has_separate_parameter)
	{
	  M2C_ALLOC (*new_parameter,
		     strlen (((char **) VLS_BEGIN (argument_vector))[arg + 1]) + 1);
	  strcpy (*new_parameter,
		  ((char **) VLS_BEGIN (argument_vector))[arg + 1]);
	}
    }
  M2C_FREE (temp_flag_name_copy);
}

/* The structure describes an presentation of ordered flag (e.g. `-l..') from
   the translator command line and is used for generation command line for C
   compilation or loading. */

struct ordered_flag_presentation
{
  int place_on_translator_command_line;
  char *flag_name, *parameter;
};

/* The following VLS contains declared above structures for presentations of
   all ordered flags from the translator command line which are used for
   generation command line for C compilation or loading.  The structures are
   oredered by member place_on_translator_command_line. */

static VLS presentations_of_ordered_flags;

/* The following function sets up varible presentations_of_ordered_flags.
   If IT_IS_FOR_C_COMPILATION is TRUE these presentations of flags are
   needed for C compilation.  Otherwise the presentations are needed for
   loading all object files by used C compiler. */

static void
set_up_presentations_of_ordered_flags (it_is_for_c_compilation)
     int it_is_for_c_compilation;
{
  register int argument_number;
  char *flag_name_presentation, *parameter_presentation;
  int flag_is_ordered;
  struct ordered_flag_presentation ordered_flag_presentation;

  VLS_CREATE (presentations_of_ordered_flags, 100);
  for (argument_number = next_flag (TRUE); argument_number != 0;
       argument_number = next_flag (FALSE))
    {
      new_flag_presentation (argument_number, &flag_name_presentation,
			     &parameter_presentation, &flag_is_ordered,
			     it_is_for_c_compilation);
      if (flag_name_presentation != NULL && flag_is_ordered)
	{
	  ordered_flag_presentation.place_on_translator_command_line
	    = argument_number;
	  ordered_flag_presentation.flag_name = flag_name_presentation;
	  ordered_flag_presentation.parameter = parameter_presentation;
	  VLS_ADD_MEMORY (presentations_of_ordered_flags,
			  &ordered_flag_presentation,
			  sizeof (ordered_flag_presentation));
	}
    }
}

/* The following function frees all dynamicaly allocated memory for
   PRESENTATIONS_OF_ORDERED_FLAGS. */

static void
delete_presentations_of_ordered_flags ()
{
  register struct ordered_flag_presentation *ordered_flag_presentation_ptr;

  for ((ordered_flag_presentation_ptr
	= (struct ordered_flag_presentation *)
	VLS_BEGIN (presentations_of_ordered_flags));
       ((char *) ordered_flag_presentation_ptr
	<= VLS_END (presentations_of_ordered_flags));
       ordered_flag_presentation_ptr++)
    {
      M2C_FREE (ordered_flag_presentation_ptr->flag_name);
      M2C_FREE (ordered_flag_presentation_ptr->parameter);
    }
  VLS_DELETE (presentations_of_ordered_flags);
}


/* The following function add some presentations of ordered flags to the end
   of new command line NEW_ARGUMENT_VECTOR.  Added ordered flag starts with
   START-th element of PRESENTATIONS_OF_ORDERED_FLAGS and ends before the first
   element with place greater or equal to PLACE_BOUND.  The function returns
   index of this element in PRESENTATIONS_OF_ORDERED_FLAGS. */

static int
add_ordered_flags_to_new_command_line (new_argument_vector, start, place_bound)
     register VLS *new_argument_vector;
     register int start, place_bound;
{
  register struct ordered_flag_presentation *ordered_flag_presentation_ptr;

  ordered_flag_presentation_ptr
    = ((struct ordered_flag_presentation *)
       VLS_BEGIN (presentations_of_ordered_flags) + start);
  while (((char *) ordered_flag_presentation_ptr
	  <= VLS_END (presentations_of_ordered_flags))
	 && (ordered_flag_presentation_ptr->place_on_translator_command_line
	     < place_bound))
    {
      VLS_ADD_MEMORY (*new_argument_vector,
		      &ordered_flag_presentation_ptr->flag_name,
		      sizeof (char *));
      if (ordered_flag_presentation_ptr->parameter != NULL)
	VLS_ADD_MEMORY (*new_argument_vector,
			&ordered_flag_presentation_ptr->parameter,
			sizeof (char *));
      ordered_flag_presentation_ptr++;
    }
  return (ordered_flag_presentation_ptr
	  - (struct ordered_flag_presentation *)
	  VLS_BEGIN (presentations_of_ordered_flags));
}



/* This page contains code which start Modula-2 to C translation,
   C to object file translation (or other C compilation, e.g. preprocessing)
   and loading (linking) object files into executable program. */


/* Source lines of translated module and all definition modules imported
   implicitly or explicitly by given module. */

static unsigned all_lines;

/* The following function translates Modula-2 file given on command
   line as ARG-th argument.  The function returns TRUE if the translation
   is successful. After the successful translation the function outputs
   processed source line and used icode bytes to stderr.  Only syntactic
   and semantics analyzer is called when regime of testing that given
   file is a main module and picking names of imported modules is on.  */

static int
Modula_to_C (arg)
     register int arg;
{
  char *name_without_suffix;
  register int okay;

  input_file_name = ((char **) VLS_BEGIN (argument_vector))[arg];
  source_line_number = 0;
  new_error_flag = FALSE;
  number_of_errors = 0;
  finish_procedure = m2c_exit;
  all_lines = 0;
  initiate_pool ();
  initiate_table_and_environment ();
  name_without_suffix = file_name_without_suffix (input_file_name);
  okay = modula_analyzer (input_file_name, name_without_suffix);
  if (okay && !testing_main_module && !picking_names_of_imported_modules)
    {
      current_modula_output_file_name
	= modula_output_file_name (name_without_suffix);
      okay = generator (current_modula_output_file_name);
      if (pass_of_picking_used_objects_when_all_flag)
	M2C_FREE (current_modula_output_file_name);
    }
  M2C_FREE (name_without_suffix);
  if (!picking_names_of_imported_modules && !testing_main_module
      && !pass_of_picking_used_objects_when_all_flag)
    fprintf (stderr, "%s - %u lines, %u bytes\n",
	     ((char **) VLS_BEGIN (argument_vector))[arg],
	     all_lines, max_number_of_bytes_for_nodes);
  delete_table ();
  free_pool ();
  return okay;
}

/* The following function processes C file according to the translator command
   line options.  For example it may be creation of object file or assembler
   file.  The processed file name is INPUT_FILE_NAME and is
   ORIGINAL_ARGUMENT_NUMBER-th argument of the translator command line.
   AFTER_MODULA is TRUE if the processed file has been generated from Modula-2
   file.  In this case ORIGINAL_ARGUMENT_NUMBER identifys this Modula-2 file.

   The function returns TRUE if the translation is successful. */

static int
C_compilation (after_modula, input_file_name, original_argument_number)
     int after_modula;
     char *input_file_name;
     int original_argument_number;
{
  VLS C_argument_vector;
  register int i, okay;
  register int begin_of_nonordered_flags_for_C;
  register int bound_of_nonordered_flags_for_C;
  char *C_additional_flag;
  char *C_presentation_of_flag_name, *parameter;
  char *output_file_name, *str, *name_without_suffix;
  int flag_is_ordered, ordered_flag_start;

  VLS_CREATE (C_argument_vector, 100);
  str = C_COMPILER;
  VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
  /* Add all additional flags for C. */
  for (i = 0;; i++)
    {
      C_additional_flag = C_ADDITIONAL_FLAG (i);
      if (C_additional_flag == NULL)
	break;
      VLS_ADD_MEMORY (C_argument_vector, &C_additional_flag, sizeof (char *));
    }
  /* Add all nonordered flags for C. */
  begin_of_nonordered_flags_for_C = VLS_LENGTH (C_argument_vector);
  for (i = next_flag (TRUE); i != 0; i = next_flag (FALSE))
    {
      new_flag_presentation (i, &C_presentation_of_flag_name,
			     &parameter, &flag_is_ordered, TRUE);
      if (C_presentation_of_flag_name != NULL && !flag_is_ordered)
	{
	  VLS_ADD_MEMORY (C_argument_vector, &C_presentation_of_flag_name,
			  sizeof (char *));
	  if (parameter != NULL)
	    VLS_ADD_MEMORY (C_argument_vector, &parameter, sizeof (char *));
	}
    }
  bound_of_nonordered_flags_for_C = VLS_LENGTH (C_argument_vector);
  set_up_presentations_of_ordered_flags (TRUE);
  /* Add all ordered flags for C which is before original place of
     input file on the translator command line. */
  ordered_flag_start
    = add_ordered_flags_to_new_command_line (&C_argument_vector, 0,
					     original_argument_number);
  VLS_ADD_MEMORY (C_argument_vector, &input_file_name, sizeof (char *));
  /* Add all ordered flags for C which is after original place of
     input file on the translator command line. */
  ordered_flag_start
    = add_ordered_flags_to_new_command_line (&C_argument_vector,
					     ordered_flag_start,
					     argument_count);
  if (C_OUTPUT_EXISTS)
    {
      /* Add flag for output file if used C compiler has such opportunities. */
      name_without_suffix
	= file_name_without_suffix (((char **) VLS_BEGIN (argument_vector))
				    [original_argument_number]);
      output_file_name = c_output_file_name (name_without_suffix);
      M2C_FREE (name_without_suffix);
      if (after_modula && IS_MODULA_OUTPUT_TEMPORARY
	  && MAY_BE_C_NONSTANDARD_OBJECT)
	{
	  if (IS_C_NONSTANDARD_OBJECT_SEPARATE)
	    {
	      str = C_NONSTANDARD_OBJECT_FLAG;
	      VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
	      VLS_ADD_MEMORY (C_argument_vector, &output_file_name,
			      sizeof (char *));
	    }
	  else
	    {
	      M2C_ALLOC (str,
			 (strlen (output_file_name)
			  + strlen (C_NONSTANDARD_OBJECT_FLAG) + 1));
	      strcpy (str, C_NONSTANDARD_OBJECT_FLAG);
	      strcat (str, output_file_name);
	      VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
	    }
	}
    }
  str = NULL;
  VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
  okay = create_and_wait_for_process (C_COMPILER,
				      VLS_BEGIN (C_argument_vector));
  if (okay && C_OUTPUT_EXISTS && after_modula && IS_MODULA_OUTPUT_TEMPORARY
      && !MAY_BE_C_NONSTANDARD_OBJECT)
    {
      /* Used C compiler can not generate object file with arbitrary name.
	 Use mv utility for that. */
      char *mv_argument_vector[4];
      mv_argument_vector[0] = MV;
      name_without_suffix = file_name_without_suffix (input_file_name);
      str = base_file_name (name_without_suffix);
      M2C_ALLOC (mv_argument_vector[1],
		 strlen (str) + strlen (C_OUTPUT_SUFFIX) + 1);
      strcpy (mv_argument_vector[1], str);
      M2C_FREE (name_without_suffix);
      strcat (mv_argument_vector[1], C_OUTPUT_SUFFIX);
      mv_argument_vector[2] = output_file_name;
      mv_argument_vector[3] = NULL;
      okay = create_and_wait_for_process (MV, mv_argument_vector);
      if (!okay)
	unlink (mv_argument_vector[1]);
      M2C_FREE (mv_argument_vector[1]);
    }
  if (C_OUTPUT_EXISTS)
    {
      if (okay)
	VLS_ADD_MEMORY (names_of_c_output_files, &output_file_name, sizeof (char *));
      else
	M2C_FREE (output_file_name);
    }
  signal_enable ();
  /* Free all dynamically allocated memory earlier for forming command line. */
  if (after_modula && IS_MODULA_OUTPUT_TEMPORARY
      && MAY_BE_C_NONSTANDARD_OBJECT && !IS_C_NONSTANDARD_OBJECT_SEPARATE)
    M2C_FREE (((char **) (VLS_BEGIN (C_argument_vector)
			  + VLS_LENGTH (C_argument_vector)))[-2]);
  for (i = begin_of_nonordered_flags_for_C;
       i < bound_of_nonordered_flags_for_C; i += sizeof (char *))
    M2C_FREE (*(char **) (VLS_BEGIN (C_argument_vector) + i));
  VLS_DELETE (C_argument_vector);
  delete_presentations_of_ordered_flags ();
  return okay;
}

/* The following function links all object files generated earlier or given
   on the translator command line according to the command line options.
   The function returns TRUE if the translation is successful. */

static int
loading_by_C_compiler ()
{
  VLS C_argument_vector;
  register int i, generated_earlier_object_file_number;
  register int ordered_flag_start, all_right_after_loading;
  char *additional_flag, *suffix;
  char *loading_presentation_of_flag_name, *parameter;
  int flag_is_ordered;
  int begin_of_nonordered_flags_for_loading;
  int bound_of_nonordered_flags_for_loading;
  char *str;

  VLS_CREATE (C_argument_vector, 100);
  str = C_COMPILER;
  VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
  /* Add all additional flags for loading. */
  for (i = 0;; i++)
    {
      additional_flag = LOAD_ADDITIONAL_FLAG (i);
      if (additional_flag == NULL)
	break;
      VLS_ADD_MEMORY (C_argument_vector, &additional_flag, sizeof (char *));
    }
  /* Add all nonordered flags for loading. */
  begin_of_nonordered_flags_for_loading = VLS_LENGTH (C_argument_vector);
  for (i = next_flag (TRUE); i != 0; i = next_flag (FALSE))
    {
      new_flag_presentation (i, &loading_presentation_of_flag_name,
			     &parameter, &flag_is_ordered, FALSE);
      if (loading_presentation_of_flag_name != NULL && !flag_is_ordered)
	{
	  VLS_ADD_MEMORY (C_argument_vector,
			  &loading_presentation_of_flag_name, sizeof (char *));
	  if (parameter != NULL)
	    VLS_ADD_MEMORY (C_argument_vector, &parameter, sizeof (char *));
	}
    }
  bound_of_nonordered_flags_for_loading = VLS_LENGTH (C_argument_vector);
  set_up_presentations_of_ordered_flags (FALSE);
  generated_earlier_object_file_number = 0;
  ordered_flag_start = 0;
  for (i = next_parameter (TRUE); i != 0; i = next_parameter (FALSE))
    {
      /* Add all ordered flags for loading which is before the corresponding
	 input file on the translator command line. */
      ordered_flag_start
	= add_ordered_flags_to_new_command_line (&C_argument_vector,
						 ordered_flag_start, i);
      suffix = file_name_suffix (((char **) VLS_BEGIN (argument_vector))[i]);
      if (strcmp (suffix, DEFINITION_MODULE_SUFFIX) != 0)
	{
	  if (it_is_input_suffix (suffix, modula_input_suffixes,
				  (sizeof (modula_input_suffixes)
				   / sizeof (char *)))
	      || it_is_input_suffix (suffix, c_input_suffixes,
				     (sizeof (c_input_suffixes)
				      / sizeof (char *))))
	    {
	      /* Add all object files generated earlier. */
	      VLS_ADD_MEMORY (C_argument_vector,
			      &(((char **) VLS_BEGIN (names_of_c_output_files))
				[generated_earlier_object_file_number]),
			      sizeof (char *));
	      generated_earlier_object_file_number++;
	    }
	  else
	    /* Add object file given on the translator command line. */
	    VLS_ADD_MEMORY (C_argument_vector,
			    &((char **) VLS_BEGIN (argument_vector))[i],
			    sizeof (char *));
	}
    }
  /* Add all ordered flags for loading which is after all input file on
     the translator command line. */
  ordered_flag_start
    = add_ordered_flags_to_new_command_line (&C_argument_vector,
					     ordered_flag_start,
					     argument_count);
  str = VLS_BEGIN (m2_run_time_library);
  VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
  str = NULL;
  VLS_ADD_MEMORY (C_argument_vector, &str, sizeof (char *));
  all_right_after_loading
    = create_and_wait_for_process (C_COMPILER, VLS_BEGIN (C_argument_vector));
  signal_enable ();
  /* Free all dynamically allocated memory earlier for forming command line. */
  for (i = begin_of_nonordered_flags_for_loading;
       i < bound_of_nonordered_flags_for_loading; i += sizeof (char *))
    M2C_FREE (*(char **) (VLS_BEGIN (C_argument_vector) + i));
  VLS_DELETE (C_argument_vector);
  delete_presentations_of_ordered_flags ();
  return all_right_after_loading;
}



/* This page contains functions for start of syntactic and semantics
   analyze of Modula-2 source file. */

/* Modula-2 file which is being processed.  Always the only such file is
   opened.  If the value is NULL when a single file is not opened. */

FILE *current_input_file;

/* Current line number of current input file. */

int current_line_number;

/* Pointer to module node presenting compilation unit which is being
   processed now. */

ICN_pointer current_compilation_unit;

/* Pointer to icode node (e.g. record, procedure and module) which opens
   current scope. */

ICN_pointer current_scope;

/* TRUE if current compilation unit is a main module. */

int it_is_main_module;

/* TRUE if current compilation unit is definition module.  The variable value
   is used only in regime of testing that given file is a main module. */

int it_is_definition_module;

/* This variable is used for long jump when fatal error is fixed for
   syntactic and semantics analyze of Modula-2 source file. */

static jmp_buf jump_for_fatal_error;

/* The following function creates icode presentation of Modula-2
   source file with attributes FULL_FILE_NAME and
   FILE_NAME_WITHOUT_SUFFIX.  The function executes also the semantics
   analyze but does not execute it in regime of testing that given file
   is a main file or picking names of imported module.  The function
   returns TRUE if the syntactic and semantics analyze is successful.
   The function repeats processing implementation module if corresponding
   definition module has not been processed yet. */

static int
modula_analyzer (full_file_name, file_name_without_suffix)
     char *full_file_name, *file_name_without_suffix;
{
  register ICN_pointer block_scope, block_begin_node_ptr;
  register char *module_name;
  register int result;
  VLS def_file_name;

  current_input_file = NULL;
  if (setjmp (jump_for_fatal_error))
    goto finish;
  finish_procedure = fatal_analyze_finish;
  module_name = base_file_name (file_name_without_suffix);
  if (create_icode_and_analyze_declarations (full_file_name,
					     ERR_input_file_opening))
    {
      if (current_input_file != NULL)
        fclose (current_input_file);
      current_input_file = NULL;
      VLS_CREATE (def_file_name, 100);
      VLS_ADD_STRING (def_file_name, definition_module_library (module_name));
      if (VLS_LENGTH (def_file_name) != 0)
	VLS_ADD_STRING (def_file_name, "/");
      VLS_ADD_STRING (def_file_name, module_name);
      VLS_ADD_STRING (def_file_name, DEFINITION_MODULE_SUFFIX);
      if (create_icode_and_analyze_declarations
	  (VLS_BEGIN (def_file_name), ERR_definition_module_absence))
	abort ();
      VLS_DELETE (def_file_name);
      if (create_icode_and_analyze_declarations (full_file_name,
						 ERR_input_file_opening))
	abort ();
    }
  if (!it_is_definition_module && !testing_main_module
      && !picking_names_of_imported_modules)
    {
      /* Semantics analyze of all statements (declarations are already
	 analyzed). */
      block_begin_node_ptr = first_block_begin ();
      for (;;)
	{
	  if (block_begin_node_ptr == NULL)
	    break;
	  block_scope = BLOCK_BEGIN_NODE (block_begin_node_ptr)->block_scope;
	  BLOCK_BEGIN (block_scope) = block_begin_node_ptr;
	  process_statements (block_begin_node_ptr);
	  block_begin_node_ptr
	    = BLOCK_BEGIN_NODE (block_begin_node_ptr)->next_block_begin;
	}
    }
finish:
  if (current_input_file != NULL)
    fclose (current_input_file);
  return number_of_errors == 0;
}


/* The following function is used as value of finish_procedure (see
   commentaries for it). */

static void
fatal_analyze_finish (code)
     int code;
{
  longjmp (jump_for_fatal_error, TRUE);
}

/* The following recursive (see create_icode_of_imported_definition_modules)
   function creates icode representation of Modula-2 source file FULL_FILE_NAME
   (and all implicitly and explicitly imported definition modules) and executes
   semantics analyze of it (if normal analyzer regime is on).  If the source
   file is not found then error ERROR_MESSAGE is fixed.  The function returns
   TRUE if processed module is an implementation module and corresponding
   definition module has not been processed yet.  It means that only first
   keyword in the source file was processed and that it is necessary to
   process corresponding definition module and after that to repeat
   processing the source file. */

static int
create_icode_and_analyze_declarations (full_file_name, error_message)
     char *full_file_name, *error_message;
{
  register int repeat_flag;

  current_line_number = 1;
  input_file_name = full_file_name;
  current_input_file = fopen (input_file_name, "r");
  if (current_input_file == NULL)
    error_with_parameter (error_message, input_file_name);
  repeat_flag = compilation_unit ();
  fclose (current_input_file);
  current_input_file = NULL;
  if (repeat_flag)
    return TRUE;
  if (!picking_names_of_imported_modules && !testing_main_module
      && !pass_of_picking_used_objects_when_all_flag)
    all_lines += current_line_number;
  if (!testing_main_module)
    create_icode_of_imported_definition_modules
      (current_compilation_unit, ERR_definition_module_absence);
  if (!testing_main_module && !picking_names_of_imported_modules)
    {
      /* semantics analyze of declarations. */
      process_export (current_compilation_unit);
      process_import (current_compilation_unit);
      process_declarations (current_compilation_unit);
      complete_processing_pointer_definitions (current_compilation_unit);
    }
  return FALSE;
}

/* The following recursive function (see create_icode_and_analyze_declarations)
   creates icode representation of all definition modules implicitly and
   explicitly imported by module given as MODULE_NODE_PTR and executes
   semantics analyze of them.  The function can not be called in regime of 
   testing that given file is a main module. */

static void
create_icode_of_imported_definition_modules (module_node_ptr)
     ICN_pointer module_node_ptr;
{
  register ICN_pointer declaration_element;
  register ICN_pointer imported_module_identifier_node_ptr;
  register int saved_current_line_number, saved_source_line_number;
  register int saved_it_is_definition_module, saved_it_is_main_module;
  ICN_pointer saved_current_scope;
  char *prefix, *imported_module_name, *saved_input_file_name;
  VLS def_file_name;

  if (testing_main_module)
    abort ();
  for (declaration_element = module_node_ptr; declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    if (MODE (declaration_element) == ICNM_IMPORT
	&& SCOPE (SCOPE (declaration_element)) == NULL)
      {
	imported_module_identifier_node_ptr
	  = (IMPORT_NODE (declaration_element)->module_exporter == NULL
	     ? IDENTIFIER (declaration_element)
	     : IMPORT_NODE (declaration_element)->module_exporter);
	if (find_denotation (ICN_POINTER (NULL),
			     imported_module_identifier_node_ptr) == NULL)
	  {
	    /* The imported definition module is not analyzed yet.
	       Process it. */
	    imported_module_name
	      = IDENTIFIER_NODE (imported_module_identifier_node_ptr)
	      ->identifier_string;
	    prefix = definition_module_library (imported_module_name);
	    VLS_CREATE (def_file_name, 100);
	    VLS_ADD_STRING (def_file_name, prefix);
	    if (VLS_LENGTH (def_file_name) != 0)
	      VLS_ADD_STRING (def_file_name, "/");
	    VLS_ADD_STRING (def_file_name, imported_module_name);
	    VLS_ADD_STRING (def_file_name, DEFINITION_MODULE_SUFFIX);
	    if (picking_names_of_imported_modules)
	      include_name_of_imported_file (VLS_BEGIN (def_file_name));
	    else
	      {
		saved_current_line_number = current_line_number;
		saved_source_line_number = source_line_number;
		saved_it_is_definition_module = it_is_definition_module;
		saved_it_is_main_module = it_is_main_module;
		saved_current_scope = current_scope;
		saved_input_file_name = input_file_name;
		if (create_icode_and_analyze_declarations
		    (VLS_BEGIN (def_file_name), ERR_definition_module_absence))
		  abort ();
		current_line_number = saved_current_line_number;
		source_line_number = saved_source_line_number;
		it_is_definition_module = saved_it_is_definition_module;
		it_is_main_module = saved_it_is_main_module;
		current_scope = saved_current_scope;
		input_file_name = saved_input_file_name;
		current_compilation_unit = module_node_ptr;
	      }
	    VLS_DELETE (def_file_name);
	  }
      }
}
