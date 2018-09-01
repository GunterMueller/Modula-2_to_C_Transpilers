/* Library of m2c (VLS, command line of m2c, hash tables and other functions).
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


/* The following functions are used by macros for work with variable
   length strings (VLS).  Remember that they are internal functions -
   all work with VLS is executed through the macros. */


/* The following function implements macros VLS_TAILOR.  Length of memory
   allocated for VLS becames equal to VLS length (but memory for zero_length
   string will contain one byte).  Remember that the string place may be
   changed. */

void
VLS_tailor_function (vls)
     register VLS *vls;
{
  register int vls_length;
  register char *new_vls_start;

  if (!vls->vls_start)
    abort ();
  vls_length = (VLS_LENGTH (*vls) ? VLS_LENGTH (*vls) : 1);
  M2C_REALLOC (new_vls_start, vls->vls_start, vls_length);
  if (new_vls_start != vls->vls_start)
    {
      vls->vls_free += new_vls_start - vls->vls_start;
      vls->vls_start = new_vls_start;
    }
  vls->vls_boundary = vls->vls_start + vls_length;
}


/* The following function implements macros VLS_ADD_STRING (addition of string
   STR (with end marker is '\0') to the end of VLS).  Remember that
   the string place may be changed. */

void
VLS_add_string_function (vls, str)
     register VLS *vls;
     char *str;
{
  register int length;

  if (!vls->vls_start)
    abort ();
  if (str == NULL)
    return;
  if (vls->vls_free != vls->vls_start)
    VLS_SHORTEN (*vls, 1);
  length = strlen (str) + 1;
  if (vls->vls_free + length > vls->vls_boundary)
    VLS_expand (vls, length);
  memcpy (vls->vls_free, str, length);
  vls->vls_free += length;
}


/* The following function may be used when something is added to the string.
   The function changes size of memory allocated for VLS.  The size becames
   equal to about one and a half of VLS length accounting for addition of
   LENGTH bytes.  Remember that the string place may be changed. */

void
VLS_expand (vls, length)
     register VLS *vls;
     int length;
{
  register int vls_length;
  register char *new_vls_start;

  if (!vls->vls_start)
    abort ();
  vls_length = VLS_LENGTH (*vls) + length;
  vls_length += vls_length / 2 + 1;
  M2C_REALLOC (new_vls_start, vls->vls_start, vls_length);
  if (new_vls_start != vls->vls_start)
    {
      vls->vls_free += new_vls_start - vls->vls_start;
      vls->vls_start = new_vls_start;
    }
  vls->vls_boundary = vls->vls_start + vls_length;
}



/* Structures and functions for work with command line with the aid of
   its description.  This source code is needed for tunning on used C
   compiler by its description of flags.

   Posix terminology concerning command line is used here.  Command line
   is divided into command name and arguments.  The arguments are subdivided
   into flags, flag parameters and parameters.  Flag starts with `-'. */


/* The following structure is used for storing information about flags which
   may be present in command line. */

struct flag_description
{
  /* Flag prefix in the flag description on which flag in the command line
     is recognized.  For example, `-D' is flag name in flag description
     `-DMACRO=DEFN'. */
  char *flag_name;
  /* The rest in the flag description.  For previous example, it is
     `MACRO=DEFN'. */
  char *flag_suffix;
  /* TRUE if the flag suffix starts with white spaces. It means that
     the corresponding flag has obligatory flag parameter. */
  int separate_suffix_flag;
};


/* The following variable length string contains pointers to arguments
   in the command line.  The element with order number 0 points to the command
   name.  Remember that additional elements may be inserted. */

VLS argument_vector;


/* Number of elements in previous variable length string. */

int argument_count;


/* The following variable length string contains all flag descriptions.
   The elements are ordered by member flag_name. */

static VLS all_flags;


/* The following string is description of flags.  The description is made up
   from two parts.  Any parts (or both) may be absent in the description.
   First part contains suffixes of flags which are in the second part.
   Suffixes are separated by white space.  The second part starts with
   percents `%%' and contains any text in which description of flags are
   placed.  Any description of flag starts with character ``' followed by
   character `-' and finishes by character `''.  The flag suffix may be starts
   with white spaces.  It means that the corresponding flag has
   obligatory flag parameter. */

static char *description_of_flags;


/* Insert ARG in argument vector in the place with number after+1 (0,1,...). */

void
insert_argument (after, arg)
     register int after;
     char *arg;
{
  register int i;

  if (after < (-1) && argument_count >= after)
    abort ();
  else
    {
      VLS_ADD_MEMORY (argument_vector, arg, sizeof (char *));
      for (i = argument_count - 1; i > after; i--)
	((char **) VLS_BEGIN (argument_vector))[i + 1]
	  = ((char **) VLS_BEGIN (argument_vector))[i];
      ((char **) VLS_BEGIN (argument_vector))[after + 1] = arg;
    }
  argument_count++;
}


/* The following internal function is used by function qsort for ordering
   elements in the variable all_flags. */

static int
compare_flags (flag1, flag2)
     struct flag_description *flag1, *flag2;
{
  return strcmp (flag1->flag_name, flag2->flag_name);
}


/* The following function is to be called first of all.  The function initiates
   variable argument vector and argument count by parameters ARGC and
   ARGV.  These parameters determine original command line.  The function also
   determines value of description_of_flags and forms variable length string
   all_flags (see commentaries for it).  Return FALSE if error in DESCRIPTION
   is fixed (in this case values of variables mentioned above are undefined).
   Otherwise return TRUE (it means success). */

int
command_tune (argc, argv, description)
     int argc;
     char **argv;
     register char *description;
{
  VLS suffixes, name;
  register int i;
  register int suffix_length;
  register char *end_name, *str;
  struct flag_description flag_description;

  description_of_flags = description;
  VLS_CREATE (all_flags, sizeof (struct flag_description) * 50);
  /* Forming ARGUMENT_VECTOR */
  VLS_CREATE (argument_vector, 100);
  argument_count = 0;
  for (i = 0; i < argc; i++)
    insert_argument (i - 1, argv[i]);
  /* Processing first part of the description (see commentaries for
     description_of_flag). */
  VLS_CREATE (suffixes, 100);
  for (;;)
    {
      while (*description == ' ' || *description == '\t'
	     || *description == '\n')
	description++;
      if (*description == '\0'
	  || (*description == '%' && description[1] == '%'))
	break;
      do
	{
	  VLS_ADD_CHAR (suffixes, *description);
	  description++;
	}
      while (*description != '\0' && *description != ' '
	     && *description != '\t'
	     && *description != '\n'
	     && (*description != '%' || description[1] != '%'));
      VLS_ADD_CHAR (suffixes, '\0');
    }
  if (*description != '%')
    return TRUE;
  /* Proccessing the second part of the description. */
  for (;;)
    {
      while (*description != '\0' && *description != '`')
	description++;
      if (*description == '\0')
	{
	  /* End of the description.  Sorting flag descriptions. */
	  qsort (VLS_BEGIN (all_flags),
		 VLS_LENGTH (all_flags) / sizeof (struct flag_description),
		 sizeof (struct flag_description), compare_flags);
	  return TRUE;
	}
      description++;
      if (*description != '-')
	{
	  /* Fixing error of the flag description start. */
	  VLS_DELETE (all_flags);
	  VLS_DELETE (suffixes);
	  return FALSE;
	}
      VLS_CREATE (name, 50);
      do
	{
	  VLS_ADD_CHAR (name, *description);
	  description++;
	}
      while (*description != '\0' && *description != '\'');
      VLS_ADD_CHAR (name, '\0');
      if (*description == '\0')
	{
	  /* Fixing error of the flag description finish. */
	  VLS_DELETE (all_flags);
	  VLS_DELETE (suffixes);
	  return FALSE;
	}
      end_name = VLS_END (name);
      /* Testing suffix precence. */
      flag_description.flag_suffix = NULL;
      flag_description.separate_suffix_flag = FALSE;
      for (str = VLS_BEGIN (suffixes);
	   str <= VLS_END (suffixes);
	   str += suffix_length + 1)
	{
	  suffix_length = strlen (str);
	  /* Testing that suffix is separated by white spaces.

	     `2' accounts for `-' and first flag character (flag name can not
	     be empty). */
	  if (suffix_length + 2 <= VLS_LENGTH (name) - 1
	      && strcmp (str, end_name - suffix_length) == 0)
	    {
	      flag_description.flag_suffix = str;
	      end_name -= suffix_length;
	      while (*(end_name - 1) == ' '
		     || *(end_name - 1) == '\t' || *(end_name - 1) == '\n')
		{
		  flag_description.separate_suffix_flag = TRUE;
		  suffix_length++;
		  end_name--;
		}
	      VLS_SHORTEN (name, suffix_length + 1);
	      VLS_ADD_CHAR (name, '\0');
	      break;
	    }
	}
      VLS_TAILOR (name);
      flag_description.flag_name = VLS_BEGIN (name);
      VLS_ADD_MEMORY (all_flags, (char *) &flag_description,
		      sizeof (struct flag_description));
    }
}


/* Output the second part (without `%%') of description of flags to stderr.
   See also commentaries for description_of_flags. */

void
output_description_of_flags ()
{
  register char *current_char_ptr;

  fprintf (stderr, "%s flags:\n", ((char **) VLS_BEGIN (argument_vector))[0]);
  for (current_char_ptr = description_of_flags;
       *current_char_ptr != '\0'
       && (*current_char_ptr != '%' || current_char_ptr[1] != '%');)
    current_char_ptr++;
  if (*current_char_ptr == '%')
    current_char_ptr += 2;
  fprintf (stderr, "%s\n", current_char_ptr);
}


/* This internal function return pointer to flag descriptor which
   describes the command line argument with number ARGUMENT_NUMBER.
   Return NULL if the corresponding flag descriptor is not found.
   The function uses bunary search. */

static struct flag_description *
flag_description (argument_number)
     register int argument_number;
{
  register struct flag_description *flag_description_ptr;
  register struct flag_description *flag_description_table;
  register int left, right, middle, comparison_code;
  register char *flag_in_command_line;

  flag_in_command_line
    = ((char **) VLS_BEGIN (argument_vector))[argument_number];
  flag_description_table = (struct flag_description *) VLS_BEGIN (all_flags);
  for (left = 0,
       right = VLS_LENGTH (all_flags) / sizeof (struct flag_description) - 1;
       left <= right;)
    {
      middle = (left + right) / 2;
      comparison_code = strcmp (flag_description_table[middle].flag_name,
				flag_in_command_line);
      if (comparison_code == 0)
	return flag_description_table + middle;
      else if (comparison_code > 0)
	right = middle - 1;
      else
	left = middle + 1;
    }
  /* This code is needed because flag in command line has parameter which
     is not separated by white spaces (e.g. flag in command line `-Ufoo' has
     name in flag description `-U').  In this case (so long the flag
     descriptor is not found) RIGHT is the number of last flag descriptor
     whose name is less than FLAG_IN_COMMAND_LINE. */
  for (flag_description_ptr = flag_description_table + right;
       (char *) flag_description_ptr >= VLS_BEGIN (all_flags)
       && flag_in_command_line[1] == flag_description_ptr->flag_name[1];
       flag_description_ptr--)
    if (flag_description_ptr->flag_suffix != NULL
	&& !flag_description_ptr->separate_suffix_flag
	&& strncmp (flag_description_ptr->flag_name, flag_in_command_line,
		    strlen (flag_description_ptr->flag_name)) == 0)
      return flag_description_ptr;
  return NULL;
}


/* The following variables are used by function next_parameter. */


/* Number of command line argument beginning with which the next parameter
   will be searched. */

static int next_parameter_number;


/* TRUE if all the rest arguments are parameters (i.e. `--' is processed in
   command line. */

static int only_parameters;


/* Return number of next parameter (first parameter if FLAG_OF_FIRST is TRUE)
   in the command line.  Return 0 if all parameters are processed. */

int
next_parameter (flag_of_first)
     int flag_of_first;
{
  register struct flag_description *flag_description_ptr;

  if (flag_of_first)
    {
      next_parameter_number = 1;
      only_parameters = FALSE;
    }
  else if (next_parameter_number == 0)
    abort ();
  for (; next_parameter_number < argument_count; next_parameter_number++)
    {
      if (only_parameters
	  || *((char **) VLS_BEGIN (argument_vector))[next_parameter_number] != '-'
	  || strcmp (((char **) VLS_BEGIN (argument_vector))
		     [next_parameter_number], "-") == 0)
	{
	  next_parameter_number++;
	  return next_parameter_number - 1;
	}
      else if (strcmp (
			((char **) VLS_BEGIN (argument_vector))[next_parameter_number],
			"--") == 0)
	only_parameters = TRUE;
      else
	{
	  flag_description_ptr = flag_description (next_parameter_number);
	  if (flag_description_ptr != NULL
	      && flag_description_ptr->separate_suffix_flag)
	    next_parameter_number++;
	}
    }
  return 0;
}


/* Return number of parameters in the command line.  The function does not
 affect the result of function next_parameter. */

int
number_of_parameters ()
{
  register int current_parameter_number, result;
  int saved_next_parameter_number, saved_only_parameters;

  saved_next_parameter_number = next_parameter_number;
  saved_only_parameters = only_parameters;
  result = 0;
  for (current_parameter_number = next_parameter (TRUE);
       current_parameter_number != 0;
       current_parameter_number = next_parameter (FALSE))
    result++;
  next_parameter_number = saved_next_parameter_number;
  only_parameters = saved_only_parameters;
  return result;
}


/* The following variables are used by function next_flag. */

/* Number of command line argument beginning with which the next flag
   will be searched. */

static int next_flag_number;


/* TRUE if all the rest arguments are parameters (i.e. `--' is processed in
   command line. */

static int no_more_flags;


/* Return number of next flag (first parameter if FLAG_OF_FIRST is TRUE)
   in the command line.  Return 0 if all flags are processed. */

int
next_flag (flag_of_first)
     int flag_of_first;
{
  register struct flag_description *flag_description_ptr;
  register int result;

  if (flag_of_first)
    {
      next_flag_number = 1;
      no_more_flags = FALSE;
    }
  else if (next_flag_number == 0)
    abort ();
  for (; next_flag_number < argument_count; next_flag_number++)
    {
      if (no_more_flags)
	return 0;
      else if (strcmp
	       (((char **) VLS_BEGIN (argument_vector))[next_flag_number],
		"--") == 0)
	no_more_flags = TRUE;
      else if (*((char **) VLS_BEGIN (argument_vector))[next_flag_number]
	       == '-'
	       && strcmp (((char **) VLS_BEGIN (argument_vector))
			  [next_flag_number], "-") != 0)
	{
	  result = next_flag_number;
	  flag_description_ptr = flag_description (next_flag_number);
	  if (flag_description_ptr != NULL
	      && flag_description_ptr->separate_suffix_flag)
	    next_flag_number++;	/* Skip the flag parameter. */
	  next_flag_number++;
	  return result;
	}
    }
  return 0;
}


/* Return pointer to flag name which describes the command line argument
   with number ARGUMENT_NUMBER.  Return NULL if the corresponding flag
   description is not found.  Remember that flag name may be differ from
   flag in the command line (e.g. '-U' and `-Ufoo').  If the flag description
   is found than set up correspondingly *flag_has_parameter. */

char *
flag_characteristics (argument_number, flag_has_parameter)
     int argument_number, *flag_has_parameter;
{
  register struct flag_description *flag_description_ptr;

  *flag_has_parameter = FALSE;
  flag_description_ptr = flag_description (argument_number);
  if (flag_description_ptr != NULL)
    {
      *flag_has_parameter = flag_description_ptr->separate_suffix_flag;
      if (!flag_description_ptr->separate_suffix_flag
	  || (argument_number < argument_count - 1
	      && (*((char **) VLS_BEGIN (argument_vector))
		  [argument_number + 1] != '-'
		  || strcmp (((char **) VLS_BEGIN (argument_vector))
			     [argument_number + 1],
			     "-") == 0)))
	return flag_description_ptr->flag_name;
      else
	return NULL;
    }
  else
    return NULL;
}


/* Return number of last flag with given flag name in the command line.
   Return 0 if the flag is not in the command line. */

int
last_flag_place (flag_name)
     char *flag_name;
{
  register char *current_flag_name;
  register int argument_number, last, saved_next_flag_number;
  register int saved_no_more_flags;
  int param;

  saved_next_flag_number = next_flag_number;
  saved_no_more_flags = no_more_flags;
  last = 0;
  argument_number = next_flag (TRUE);
  while (argument_number != 0)
    {
      current_flag_name = flag_characteristics (argument_number, &param);
      if (current_flag_name != NULL
	  && strcmp (current_flag_name, flag_name) == 0)
	last = argument_number;
      argument_number = next_flag (FALSE);
    }
  next_flag_number = saved_next_flag_number;
  no_more_flags = saved_no_more_flags;
  return last;
}


/* Return pointer to parameter of last flag with given flag name in the
   command line.  Return NULL if the flag is absent in the command line.
   The function must be called only for flags which have parameter (separated
   by white spaces). */

char *
flag_parameter (flag_name)
     char *flag_name;
{
  int argument_number;

  argument_number = last_flag_place (flag_name);
  return (argument_number == 0 ? NULL
	  : ((char **) VLS_BEGIN (argument_vector))[argument_number + 1]);
}




/* The value of the following variable is changed for syntactic, semantic
   analyses and generation times.  The variable stores current number of
   processed source line or source line of processed language construction. */

int source_line_number;


/* The value of the following variable is name of current file processed.
   Remember that all implicitly or explicitly imported definition modules are
   processed before processing the implementation module. */

char *input_file_name;


/* Allocate memory and place FILE_NAME without suffix (it starts with `.')
   in this memory. */

char *
file_name_without_suffix (file_name)
     register char *file_name;
{
  register char *result;
  register int result_length;

  result_length = strlen (file_name);
  while (result_length != 0 && file_name[--result_length] != '.') ;
  if (result_length == 0)
    result_length = strlen (file_name);
  M2C_ALLOC (result, result_length + 1);
  strncpy (result, file_name, result_length);
  result[result_length] = '\0';
  return result;
}


/* Return pointer to first char (it is `.') of suffix of FILE_NAME.
   If the suffix is absent than result is empty string. */

char *
file_name_suffix (file_name)
     register char *file_name;
{
  register char *last_period;

  for (last_period = NULL; *file_name != '\0'; file_name++)
    if (*file_name == '.')
      last_period = file_name;
  return (last_period == NULL ? file_name : last_period);
}


/* Return pointer to first char after last `/' in FILE_NAME.
   If the directory name is absent than result is FILE_NAME. */

char *
base_file_name (file_name)
     register char *file_name;
{
  register int directory_name_length;

  directory_name_length = strlen (file_name);
  while (directory_name_length >= 0 && file_name[directory_name_length] != '/')
    directory_name_length--;
  return file_name + directory_name_length + 1;
}


/* Test correctness of name of input file which contains module representing
   icode module node COMPILATION_UNIT.  Correct name of file must be
   the corresponding module name with suffix DEFINITION_MODULE_SUFFIX (for
   definition module), IMPLEMENTATION_MODULE_SUFFIX (for implementation module)
   or MAIN_MODULE_SUFFIX (for main module).  Fix error if the input file name
   is not correct. */

void
test_input_file_name (compilation_unit)
     register ICN_pointer compilation_unit;
{
  register enum module_mode module_mode;
  register char *suffix;
  VLS correct_file_name;

  if (IDENTIFIER (compilation_unit) != NULL)
    {
      module_mode
	= (enum module_mode) MODULE_NODE (compilation_unit)->module_mode;
      if (module_mode == MM_DEFINITION_MODULE)
	suffix = DEFINITION_MODULE_SUFFIX;
      else if (module_mode == MM_DEFINITION_WITH_IMPLEMENTATION_MODULE)
	suffix = IMPLEMENTATION_MODULE_SUFFIX;
      else
	suffix = MAIN_MODULE_SUFFIX;
      VLS_CREATE (correct_file_name, 100);
      VLS_ADD_STRING
	(correct_file_name,
	 IDENTIFIER_NODE (IDENTIFIER (compilation_unit))->identifier_string);
      VLS_ADD_STRING (correct_file_name, suffix);
      if (strcmp (VLS_BEGIN (correct_file_name),
		  base_file_name (input_file_name)) != 0)
	error_with_parameter (ERR_invalid_file_name,
			      VLS_BEGIN (correct_file_name));
      VLS_DELETE (correct_file_name);
    }
}


/* Return value of hexadecimal digit represented character CH.

   Remember that on some UNIX systems the macros toupper is defined only
   for letters. */

int
value_of_hexadecimal_digit (ch)
     register int ch;
{
  return (isdigit (ch) ? (ch) - '0'
	  : ((islower (ch) ? toupper (ch) : ch) - ('A' - 10)));
}


/* The following function is used for evaluation of value of standard Modula-2
   function CAP.  Return corresponding upper case letter if CH is letter or
   CH otherwise.

   Remember that on some UNIX systems the macros toupper is defined only
   for letters. */

int
M2_cap (ch)
     register int ch;
{
  return (islower (ch) ? toupper (ch) : ch);
}



/* The following functions implements all hash tables in the translator.
   The table element may be only a pointer (NULL is reserved for empty table
   entry). The size of hash tables is not fixed.  The hash table will be
   expanded when its occupancy will became big.*/


/* Return the nearest prime number which is greater than NUMBER. */

static unsigned
higher_prime_number (number)
     register unsigned number;
{
  register unsigned i;

  for (number = (number / 2) * 2 + 3;; number += 2)
    {
      for (i = 3; i * i <= number; i += 2)
	if (number % i == 0)
	  break;
      if (i * i > number)
	return number;
    }
}


/* Create and return hash table with length slightly longer than SIZE.  This
   hash table is initiated as empty (all the hash table entries are NULL).
   The hash table will use function HASH_FUNCTION for evaluation of
   table element hash value and function COMPARE_FUNCTION for comparison of
   two table elements. */

hash_table
create_hash_table (size, hash_function, compare_function)
     unsigned size;
     unsigned (*hash_function) ();
     int (*compare_function) ();
{
  register hash_table result;
  register hash_table_entry *entry_ptr;
  register char *s;

  size = higher_prime_number (size);
  M2C_ALLOC (s, sizeof (*result));
  result = (hash_table) s;
  M2C_ALLOC (s, size * sizeof (hash_table_entry));
  result->entries = (hash_table_entry *) s;
  result->size = size;
  result->hash_function = hash_function;
  result->compare_function = compare_function;
  result->number_of_elements = 0;
  for (entry_ptr = result->entries;
       entry_ptr < result->entries + size;
       entry_ptr++)
    *entry_ptr = NULL;
  return result;
}


/* Free memory allocated for hash table HTAB.  Naturally the hash
   table must already exist. */

void
delete_hash_table (htab)
     hash_table htab;
{
  M2C_FREE (htab->entries);
  M2C_FREE (htab);
}


/* Internal function which may be used when something is added to the table.
   The function changes (about two-fold) size of memory allocated for
   the entries and repeatedly inserts the table elements.  Remember that
   the place of the table entries is changed. */

static void
expand_hash_table (htab)
     hash_table htab;
{
  register hash_table new_htab;
  register hash_table_entry *entry_ptr, *new_entry_ptr;

  new_htab = create_hash_table (htab->size * 2, htab->hash_function,
				htab->compare_function);
  for (entry_ptr = htab->entries; entry_ptr < htab->entries + htab->size;
       entry_ptr++)
    if (*entry_ptr != NULL)
      {
	new_entry_ptr = find_hash_table_entry (new_htab, *entry_ptr, TRUE);
	if (*new_entry_ptr != NULL)
	  abort ();
	*new_entry_ptr = (*entry_ptr);
      }
  M2C_FREE (htab->entries);
  *htab = (*new_htab);
  M2C_FREE (new_htab);
}


/* The following variable is used for debugging. Its value is number
   of collisions fixed for time of work with all hash tables. */

static int collisions = 0;


/* The following function searches hash table entry which contains element
   equal to ELEMENT or empty entry in which ELEMENT may be placed (if the
   element does not exist in the table).  RESERVE is to be equal TRUE
   if the element should be placed in the table.  The table is expanded if
   occupancy is more than 75%. */

hash_table_entry *
find_hash_table_entry (htab, element, reserve)
     register hash_table htab;
     hash_table_entry element;
     int reserve;
{
  register hash_table_entry *entry_ptr;
  register unsigned hash_value;

  if (htab->size / 4 <= htab->number_of_elements / 3)
    expand_hash_table (htab);
  hash_value = (*htab->hash_function) (element);
  for (;; collisions++)
    {
      entry_ptr = htab->entries + (hash_value) % htab->size;
      hash_value++;		/* secondary hashing */
      if (*entry_ptr == NULL)
	{
	  if (reserve)
	    htab->number_of_elements++;
	  break;
	}
      else if ((*htab->compare_function) (*entry_ptr, element))
	break;
    }
  return entry_ptr;
}
