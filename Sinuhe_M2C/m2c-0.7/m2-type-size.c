/* Functions for tunning on memory allocation of the used C translator.
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


#define MIN(a,b)  ((a)<(b) ? (a) : (b))

static void get_record_case_part_size_and_alignment ();

/* Round up V so that it will be multiple of D.  Return the result. */

static Tcard
round_up (v, d)
     register Tcard v;
     register int d;
{
  return (v % d == 0 ? v : v + (d - v % d));
}

/* Return span (number of the elements -1) of values of array index type
   INDEX_TYPE_PTR. */

Tcard
array_index_span (index_type_ptr)
     register ICN_pointer index_type_ptr;
{
  register ICN_pointer range_min_ptr, range_max_ptr;
  enum icode_node_mode mode;

  range_min_ptr = min_or_max (index_type_ptr, TRUE);
  range_max_ptr = min_or_max (index_type_ptr, FALSE);
  if (range_min_ptr != NULL && range_max_ptr != NULL)
    {
      mode = MODE (range_min_ptr);
      if (mode == ICNM_CARDINAL)
	return (CARDINAL_NODE (range_max_ptr)->cardinal_value
		- CARDINAL_NODE (range_min_ptr)->cardinal_value);
      else if (mode == ICNM_INTEGER)
	return (INTEGER_NODE (range_min_ptr)->integer_value < 0
		&& INTEGER_NODE (range_max_ptr)->integer_value > 0
		? ((Tcard) INTEGER_NODE (range_max_ptr)->integer_value
		   + (Tcard) - INTEGER_NODE (range_min_ptr)->integer_value)
		: (INTEGER_NODE (range_max_ptr)->integer_value
		   - INTEGER_NODE (range_min_ptr)->integer_value));
    }
  return 0;
}

/* The following function makes more precise size and alignment of type given
   as TYPE_PTR after standard method of size and alignment calculation.
   Input and output values are passed through parameters SIZE and ALIGN. */

static void
make_more_precise_size_and_align (type_ptr, size, align)
     register ICN_pointer type_ptr;
     register Tcard *size;
     register int *align;
{
  register enum icode_node_mode mode;

  if (type_ptr == NULL)
    return;
  mode = MODE (type_ptr);
#ifdef MAKING_MORE_PRECISE_SIZE_BY_TYPE_MODE
  *size = MAKING_MORE_PRECISE_SIZE_BY_TYPE_MODE (*size, mode);
#endif

#ifdef MAKING_MORE_PRECISE_SIZE_BY_TYPE
  *size = MAKING_MORE_PRECISE_SIZE_BY_TYPE (*size, type_ptr);
#endif

#ifdef MAKING_MORE_PRECISE_ALIGN_BY_TYPE_MODE
  *align = MAKING_MORE_PRECISE_ALIGN_BY_TYPE_MODE (*align, mode);
#endif

#ifdef MAKING_MORE_PRECISE_ALIGN_BY_TYPE
  *align = MAKING_MORE_PRECISE_ALIGN_BY_TYPE (*align, type_ptr);
#endif

}

/* The following function processes current field list element (field or
   case part) given as CURRENT_ELEMENT_PTR and calculates current size and
   alignment of construction which contains this filed list. */

static ICN_pointer
process_field_or_case_part (current_element_ptr, current_size, current_align)
     register ICN_pointer current_element_ptr;
     register Tcard *current_size;
     register int *current_align;
{
  Tcard displacement;
  Tcard element_size;
  int element_align;
  ICN_pointer next_element_ptr;

  if (MODE (current_element_ptr) == ICNM_FIELD)
    {
      get_type_size_and_alignment
	(FIELD_NODE (current_element_ptr)->field_type, &element_size,
	 &element_align);
      next_element_ptr = FIELD_NODE (current_element_ptr)->next_field;
    }
  else
    {
      get_record_case_part_size_and_alignment (current_element_ptr,
					       &element_size, &element_align);
      next_element_ptr
	= RECORD_CASE_PART_NODE (current_element_ptr)
	->record_element_after_case_part;
    }
  displacement = round_up (*current_size, element_align);
  *current_size = (displacement) + element_size;
  if (*current_align < element_align)
    *current_align = element_align;
  return next_element_ptr;
}

/* The following function calculates size and alignment of case part
   (it is represented by union in C)given as CASE_PART_PTR and returns
   the results through parameters SIZE and ALIGN. */

static void
get_record_case_part_size_and_alignment (case_part_ptr, size, align)
     register ICN_pointer case_part_ptr;
     register Tcard *size;
     register int *align;
{
  ICN_pointer list_element_ptr, case_variant_ptr;
  Tcard variant_size;
  int variant_align;

  *size = 0;
  *align = 0;
  for (case_variant_ptr = (RECORD_CASE_PART_NODE (case_part_ptr)
			   ->record_case_variant_list);
       MODE (case_variant_ptr) == ICNM_RECORD_CASE_VARIANT;
       case_variant_ptr = (RECORD_CASE_VARIANT_NODE (case_variant_ptr)
			   ->next_case_variant))
    {
      variant_size = 0;
      variant_align = 0;
      for (list_element_ptr = (RECORD_CASE_VARIANT_NODE (case_variant_ptr)
			       ->case_variant_elements);
	   MODE (list_element_ptr) == ICNM_RECORD_CASE_PART
	   || MODE (list_element_ptr) == ICNM_FIELD;)
	list_element_ptr
	  = process_field_or_case_part (list_element_ptr,
					&variant_size, &variant_align);
      if (*size < variant_size)
	*size = variant_size;
      if (*align < variant_align)
	*align = variant_align;
    }
  make_more_precise_size_and_align (case_part_ptr, size, align);
}

/* Table of sizes of all Modula-2 basic type (except for string types).
   See order of basic types in file m2-icode.h. */

static char basic_type_size_table[] =
{				/*BTM_SHORT_CARDINAL - BTM_LONG_CARDINAL_OR_INTEGER*/
  [BTM_SHORT_CARDINAL] = sizeof (unsigned short),
  [BTM_CARDINAL]       = sizeof (unsigned),
  [BTM_LONG_CARDINAL]  = sizeof (unsigned long long),
  [BTM_SHORT_INTEGER]  = sizeof (short),
  [BTM_INTEGER]        = sizeof (int),
  [BTM_LONG_INTEGER]   = sizeof (unsigned long),
  [BTM_SHORT_REAL]     = sizeof (float),
  [BTM_REAL]           = sizeof (float),
  [BTM_LONG_REAL]      = sizeof (double),
  [BTM_CHAR]           = sizeof (unsigned char),
  [BTM_BYTE]           = BYTE_SIZE,
  [BTM_WORD]           = sizeof (int),
  [BTM_NIL]            = sizeof (intptr_t),
  [BTM_SHORT_CARDINAL_OR_INTEGER] = sizeof (short),
  [BTM_CARDINAL_OR_INTEGER]       = sizeof (int),
  [BTM_LONG_CARDINAL_OR_INTEGER]  = sizeof (long long),
};


/* The following function calculates size and alignment of type definition
   TYPE_PTR and returns the results through parameters SIZE and ALIGN. */

void
get_type_size_and_alignment (type_ptr, size, align)
     register ICN_pointer type_ptr;
     register Tcard *size;
     register int *align;
{
  enum icode_node_mode mode;
  int size_table_index;

  *size = 0;
  *align = 0;
  type_ptr = through_range_type (type_ptr);
  if (type_ptr != NULL)
    {
      mode = MODE (type_ptr);
      if (it_is_string_type (type_ptr))
	{
	  *size = (string_length_from_its_type (type_ptr)
		   * (Tcard) basic_type_size_table[(int) BTM_CHAR]);
	  *align = MIN (MAX_ALIGNMENT, basic_type_size_table[(int) BTM_CHAR]);
	}
      else
	{
	  if (mode == ICNM_SET_TYPE)
	    {
	      *size = SET_SIZE;
	      *align = MIN (MAX_ALIGNMENT, *size);
	    }
	  else if (mode == ICNM_ARRAY_TYPE)
	    {
	      get_type_size_and_alignment (ARRAY_TYPE_NODE (type_ptr)->base_type,
					   size, align);
	      *size *= (array_index_span
			(ARRAY_TYPE_NODE (type_ptr)->array_index_type)
			+ 1);
	    }
	  else if (mode == ICNM_RECORD)
	    {
	      for (type_ptr = RECORD_NODE (type_ptr)->record_element_list;
		   MODE (type_ptr) == ICNM_RECORD_CASE_PART
		   || MODE (type_ptr) == ICNM_FIELD;)
		type_ptr = process_field_or_case_part (type_ptr, size, align);
	      *size = round_up (*size, *align);
	    }
	  else if (mode == ICNM_ENUMERATION_TYPE)
	    get_type_size_and_alignment
	      (type_of_cardinal_value
	       (CARDINAL_NODE (min_or_max (type_ptr, FALSE))->cardinal_value),
	       size, align);
	  else
	    {
	      if (mode == ICNM_BASIC_TYPE)
		size_table_index = (int) BASIC_TYPE_NODE (type_ptr)->basic_type;
	      else
		/*ICNM_POINTER_TYPE, ICNM_QUALIFIED_EXPORT, ICNM_PROCEDURE_TYPE*/
		size_table_index = (int) BTM_NIL;
	      *size = basic_type_size_table[size_table_index];
	      *align = MIN (MAX_ALIGNMENT, *size);
	    }
	}
      make_more_precise_size_and_align (type_ptr, size, align);
    }
}
