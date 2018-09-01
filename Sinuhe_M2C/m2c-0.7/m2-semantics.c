/* Semantics analysis of Modula-2 translator.
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
#include "m2-semantics.h"

static ICN_pointer eval ();


/* This page contains functions and macros for testing icode constructions on
   type, expression, variable, constant and formal array parameter. */


/* The following structure is control for work of function
   generic_semantic_test. */

struct control_test_parameters
{
  /* Pointer to variable containing pointer to tested icode construction. */
  ICN_pointer *icode_subgraph_ptr;
  /* Pointer to variable which will contain semantic information of the tested
     icode construction after function generic_semantic_test call. */
  semantic_information *corresponding_semantics;
  /* Pointer to function (subgraph_is_type or subgraph_is_expression)
     which tests more generally the corresponding icode construction and
     determines its semantic information. */
  int (*icode_class_predicate) ();
  /* The following members are equal to TRUE when test on array parameter,
     test on variable or test on constant are needed additionally. */
  char array_test_is_needed, variable_test_is_needed, constant_test_is_needed;
};


/* The following function tests semantic object (type, constant, expression,
   variable or formal array parameter) according to CONTROL_TEST_PARAMETERS.
   If test is failed then error (ERROR_MESSAGE, ERROR_PARAMETER) is fixed and
   the function return FALSE.  Otherwise the function return TRUE only if
   the object type is correct. */

static int
generic_semantic_test (error_message, error_parameter, control_test_parameters)
     char *error_message;
     char *error_parameter;
     register struct control_test_parameters *control_test_parameters;
{
  if (((*control_test_parameters->icode_class_predicate)
       (*control_test_parameters->icode_subgraph_ptr,
	control_test_parameters->corresponding_semantics))
      && (!control_test_parameters->array_test_is_needed
	  || (!control_test_parameters->corresponding_semantics
	      ->it_is_array_parameter))
      && (!control_test_parameters->variable_test_is_needed
	  || (control_test_parameters->corresponding_semantics
	      ->it_is_designator))
      && (!control_test_parameters->constant_test_is_needed
	  || control_test_parameters->corresponding_semantics->it_is_constant))
    {
      if (control_test_parameters->corresponding_semantics->type != NULL)
	return TRUE;		/* The object type is correct. */
    }
  else if (*control_test_parameters->icode_subgraph_ptr != NULL)
    error_with_parameter (error_message, error_parameter);
  return FALSE;
}


/* The two following variables identify tested constructions (see corresponding
   functions below).  The function eval works with these variables so that
   r1 contains result of recent eval call and r2 contains result of preceding
   call. */

static ICN_pointer r1;
static ICN_pointer r2;


/* The two following variables contain semantic information about constructions
   identified by variables mentioned above.  These variables are set up
   after the call of corresponding test function. */

static semantic_information sinf1;
static semantic_information sinf2;



/* The following structure is control for test of object identified by R1
    on type. */

static struct control_test_parameters r1_type_test_parameters =
{&r1, &sinf1, subgraph_is_type, FALSE, FALSE, FALSE};

/* The two following functions are special cases of function
   generic_semantic_test (see commentaries for it). */

static int
r1_type_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r1_type_test_parameters);
}


/* The following structure is control for test of object identified by R2
   on type. */

static struct control_test_parameters r2_type_test_parameters =
{&r2, &sinf2, subgraph_is_type, FALSE, FALSE, FALSE};

static int
r2_type_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r2_type_test_parameters);
}


/* The following two macros are defined for convenience of using error without
   parameter. */

#define r1_type_test(error_message) \
  r1_type_test_with_error_parameter(error_message,0)

#define r2_type_test(error_message) \
  r2_type_test_with_error_parameter(error_message,0)



/* The following structure is control for test of object identified by R1
   on expression. */

static struct control_test_parameters r1_expression_test_parameters =
{&r1, &sinf1, subgraph_is_expression, TRUE, FALSE, FALSE};

/* The two following functions are special cases of function
   generic_semantic_test (see commentaries for it). */

static int
r1_expression_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r1_expression_test_parameters);
}


/* The following structure is control for test of object identified by R2
   on expression. */

static struct control_test_parameters r2_expression_test_parameters =
{&r2, &sinf2, subgraph_is_expression, TRUE, FALSE, FALSE};

static int
r2_expression_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r2_expression_test_parameters);
}


/* The following two macros are defined for convenience of using error without
   parameter. */

#define r1_expression_test(error_message) \
  r1_expression_test_with_error_parameter(error_message,0)

#define r2_expression_test(error_message) \
  r2_expression_test_with_error_parameter(error_message,0)



/* The following structure is control for test of object identified by R1
    on variable. */

static struct control_test_parameters r1_variable_test_parameters =
{&r1, &sinf1, subgraph_is_expression, TRUE, TRUE, FALSE};

/* The two following functions are special cases of function
   generic_semantic_test (see commentaries for it). */

static int
r1_variable_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r1_variable_test_parameters);
}


/* The following structure is control for test of object identified by R2
   on variable. */

static struct control_test_parameters r2_variable_test_parameters =
{&r2, &sinf2, subgraph_is_expression, TRUE, TRUE, FALSE};

int
r2_variable_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r2_variable_test_parameters);
}


/* The following two macros are defined for convenience of using error without
   parameter. */

#define r1_variable_test(error_message) \
  r1_variable_test_with_error_parameter(error_message,0)

#define r2_variable_test(error_message) \
  r2_variable_test_with_error_parameter(error_message,0)



/* The following structure is control for test of object identified by R1
   on constant. */

static struct control_test_parameters r1_constant_test_parameters =
{&r1, &sinf1, subgraph_is_expression, TRUE, FALSE, TRUE};

/* The two following functions are special cases of function
   generic_semantic_test (see commentaries for it). */

static int
r1_constant_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r1_constant_test_parameters);
}


/* The following structure is control for test of object (identified by R2)
   on constant. */

static struct control_test_parameters r2_constant_test_parameters =
{&r2, &sinf2, subgraph_is_expression, TRUE, FALSE, TRUE};

static int
r2_constant_test_with_error_parameter (error_message, error_parameter)
     char *error_message;
     char *error_parameter;
{
  return generic_semantic_test (error_message, error_parameter,
				&r2_constant_test_parameters);
}


/* The following two macros are defined for convenience of using error without
   parameter. */

#define r1_constant_test(error_message) \
  r1_constant_test_with_error_parameter(error_message,0)

#define r2_constant_test(error_message) \
  r2_constant_test_with_error_parameter(error_message,0)



/* This page contains additional functions for work with icode which
   are needed for semantic analysis. */


/* The function returns TRUE only if ICODE_PTR refers to some discrete type.
   The discrete type is any basic type (except for float types) or any
   enumeration type. */

static int
it_is_discrete_type (icode_ptr)
     register ICN_pointer icode_ptr;
{
  return (it_is_cardinal_type (icode_ptr)
	  || it_is_integer_type (icode_ptr)
	  || it_is_integer_cardinal_type (icode_ptr)
	  || it_is_character_type (icode_ptr)
	  || (icode_ptr != NULL && MODE (icode_ptr) == ICNM_ENUMERATION_TYPE));
}

/* The function implements less than operation for values of any discrete
   types (i.e. VALUE1 and VALUE2 may be of different types).  VALUE1 and VALUE2
   are to refer to constant values of discrete types.  If VALUE1 or VALUE2 is
   NULL then the function return FALSE. */

static int
discrete_less_then_operation (value1, value2)
     register ICN_pointer value1, value2;
{
  if (value1 == NULL || value2 == NULL)
    return FALSE;
  else if (!it_is_integer_type (VALUE_TYPE (value1)))
    {
      if (!it_is_integer_type (VALUE_TYPE (value2)))
	/* It may be for enumeration types. */
	return cardinal_value (value1) < cardinal_value (value2);
      else if (INTEGER_NODE (value2)->integer_value < 0)
	return FALSE;
      else
	return (cardinal_value (value1)
		< (Tcard) INTEGER_NODE (value2)->integer_value);
    }
  else if (it_is_integer_type (VALUE_TYPE (value2)))
    return (INTEGER_NODE (value1)->integer_value
	    < INTEGER_NODE (value2)->integer_value);
  else if (INTEGER_NODE (value1)->integer_value < 0)
    return TRUE;
  else
    return ((Tcard) INTEGER_NODE (value1)->integer_value
	    < cardinal_value (value2));
}

/* The function tests that cardinal number of VALUE belongs range of
   cardinal numbers of values of type TYPE.  The function return TRUE
   if test is successfull, i.e. type of VALUE or TYPE is not (subrange of)
   discrete type or else cardinal number of VALUE belongs range of cardinal
   numbers of values of type TYPE.  The parameter VALUE is to refer to icode
   node representing constant value. */

static int
test_discrete_inclusion (value, type)
     register ICN_pointer value, type;
{
  if (it_is_discrete_type (through_range_type (type))
      && it_is_discrete_type (VALUE_TYPE (value))
      && (discrete_less_then_operation (value, min_or_max (type, TRUE))
	  || discrete_less_then_operation (min_or_max (type, FALSE), value)))
    return FALSE;
  return TRUE;
}

/* The function returns TRUE only if ICODE_PTR refers to some scalar type.
   The scalar type is any basic type or any enumeration type. */

static int
it_is_scalar_type (icode_ptr)
     ICN_pointer icode_ptr;
{
  return it_is_discrete_type (icode_ptr) || it_is_real_type (icode_ptr);
}

/* The function returns declaration level of denotation (i.e. object having
   identifier) OBJECT.  OBJECT is to be NULL pointer or pointer to some
   denotation. */

static int
declaration_level (object)
     register ICN_pointer object;
{
  register int level;

  if (object != NULL)
    object = SCOPE (object);
  for (level = 0; object != NULL; object = SCOPE (object))
    if (MODE (object) == ICNM_PROCEDURE)
      level++;
  return level;
}

/* The function returns TRUE if denotation OBJECT is local or standard
   (including procedure from module SYSTEM) procedure.  OBJECT is to be NULL
   pointer or pointer to some denotation. */

static int
it_is_local_or_standard_procedure (object)
     register ICN_pointer object;
{
  return (declaration_level (object) != 0
	  || SCOPE (object) == NULL
	  || SCOPE (object) == ICN_POINTER (&ICN_D_system));
}

/* The function returns pointer to icode node representing integer type
   constant with value from node VALUE_PTR (it is to be node representing value
   with integer-cardinal type). */

static ICN_pointer
integer_value_from_integer_cardinal_value (value_ptr)
     ICN_pointer value_ptr;
{
  register ICN_pointer new_value_ptr;

  new_value_ptr = create_node (ICNM_INTEGER);
  INTEGER_NODE (new_value_ptr)->integer_value
    = CARDINAL_NODE (value_ptr)->cardinal_value;
  VALUE_TYPE (new_value_ptr)
    = type_of_integer_value (INTEGER_NODE (new_value_ptr)->integer_value);
  return find_in_or_else_include_to_table (new_value_ptr);
}

/* The function returns pointer to icode node representing cardinal type
   constant with value from node VALUE_PTR (it is to be node representing value
   with integer-cardinal type). */

static ICN_pointer
cardinal_value_from_integer_cardinal_value (value_ptr)
     ICN_pointer value_ptr;
{
  register ICN_pointer new_value_ptr;

  new_value_ptr = create_node (ICNM_CARDINAL);
  CARDINAL_NODE (new_value_ptr)->cardinal_value
    = CARDINAL_NODE (value_ptr)->cardinal_value;
  VALUE_TYPE (new_value_ptr)
    = type_of_cardinal_value (CARDINAL_NODE (new_value_ptr)->cardinal_value);
  return find_in_or_else_include_to_table (new_value_ptr);
}

/* The function returns pointer to icode node representing character type
   constant with value from node VALUE_PTR (it is to be node representing
   single string value, i.e. character). */

static ICN_pointer
character_value_from_single_character_string (value_ptr)
     ICN_pointer value_ptr;
{
  register ICN_pointer new_value_ptr;

  new_value_ptr = create_node (ICNM_CARDINAL);
  CARDINAL_NODE (new_value_ptr)->cardinal_value
    = CHAR_TO_INT ((*STRING_NODE (value_ptr)->string_value));
  VALUE_TYPE (new_value_ptr) = ICN_POINTER (&ICN_TD_char);
  return find_in_or_else_include_to_table (new_value_ptr);
}

/* The function returns pointer to icode node representing the same value as
   VALUE_PTR (it is to be node representing constant value) but with more
   accurate type (e.g. SHORTINT vs. INTEGER).  The original node is undefined
   after the call. */

static ICN_pointer
do_accurate_type_value (value_ptr)
     register ICN_pointer value_ptr;
{
  register ICN_pointer value_type;

  value_type = VALUE_TYPE (value_ptr);
  if (it_is_cardinal_type (value_type))
    VALUE_TYPE (value_ptr)
      = type_of_cardinal_value (CARDINAL_NODE (value_ptr)->cardinal_value);
  else if (it_is_integer_type (value_type))
    VALUE_TYPE (value_ptr)
      = type_of_integer_value (INTEGER_NODE (value_ptr)->integer_value);
  else if (it_is_real_type (value_type))
    VALUE_TYPE (value_ptr)
      = type_of_real_value (REAL_NODE (value_ptr)->real_value);
  else if (it_is_integer_cardinal_type (value_type))
    {
      VALUE_TYPE (value_ptr)
	= integer_cardinal_type_of_cardinal_value
	(CARDINAL_NODE (value_ptr)->cardinal_value);
      if (VALUE_TYPE (value_ptr) == NULL)
	VALUE_TYPE (value_ptr)
	  = type_of_cardinal_value
	  (CARDINAL_NODE (value_ptr)->cardinal_value);
    }
  return find_in_or_else_include_to_table (value_ptr);
}

/* The function sets up current source line equal to source line of
   node PTR.  It does not executed for identifier and basic type node
   because all occurrences of identifier are represented by pointer to
   the single node for the identifier.  Analogous thing is executed for
   other unique nodes (e.g. values) but in other functions. */

static void
set_source_line (ptr)
     register ICN_pointer ptr;
{
  if (ptr != NULL && MODE (ptr) != ICNM_IDENTIFIER
      && MODE (ptr) != ICNM_BASIC_TYPE)
    source_line_number = LINE (ptr);
}



/* Function for semantic analysis of case clause and case labels of case
   statement or of case part of record.  The function also sets up accurate
   values of labels.  CASE_PTR and TAG correspondingly are to refer to case
   clause (case statement or record case part) and to type of case expression
   (case statement) or of tag type (record case part). */

static void
process_case_labels (tag, case_ptr)
     ICN_pointer tag, case_ptr;
{
  register ICN_pointer current_label, current_pred_label;
  register ICN_pointer label_value2, label_value1;
  register ICN_pointer current_variant, current_pred_variant, first_variant;
  register ICN_pointer current_label_base_type;
  enum icode_node_mode case_label_mode;
  int error_flag;
  Tint i1, i2;
  Tcard c1, c2;

  first_variant
    = (MODE (case_ptr) == ICNM_CASE_STATEMENT
       ? CASE_STATEMENT_NODE (case_ptr)->case_statement_variant_list
       : RECORD_CASE_PART_NODE (case_ptr)->record_case_variant_list);
  /* Process all variants. */
  for (current_variant = first_variant;
       MODE (current_variant) == ICNM_RECORD_CASE_VARIANT
       || MODE (current_variant) == ICNM_CASE_STATEMENT_VARIANT;
       current_variant
       = CASE_VARIANT_NODE (current_variant)->next_case_variant)
    {
      set_source_line (current_variant);
      /* Process all labels of the current variant. */
      for (current_label = current_variant, error_flag = TRUE;;)
	{
	  current_label = CASE_LABEL_LIST (current_label);
	  if (current_label == NULL)
	    break;
	  case_label_mode = MODE (current_label);
	  /* Test that the label is constant value. */
	  if (case_label_mode == ICNM_RECORD_CASE_LABEL_ELEMENT
	      || case_label_mode == ICNM_CASE_STATEMENT_LABEL_ELEMENT)
	    {
	      /* The single label. */
	      eval (CASE_LABEL_ELEMENT_NODE (current_label)->case_label_value);
	      if (r1_constant_test (ERR_nonconstant_case_label))
		{
		  error_flag = FALSE;
		  /* Consider further that it is label range L..L where
		     L is the corresponding single label. */
		  sinf2.type = sinf1.type;
		  r2 = r1;
		}
	    }
	  else
	    {
	      /* The label range. */
	      eval (CASE_LABEL_RANGE_NODE (current_label)
		    ->case_label_range_left_bound);
	      eval (CASE_LABEL_RANGE_NODE (current_label)
		    ->case_label_range_right_bound);
	      if (r2_constant_test (ERR_nonconstant_case_label)
		  && r1_constant_test (ERR_nonconstant_case_label))
		error_flag = FALSE;
	    }
	  if (!error_flag)
	    {
	      error_flag = TRUE;
	      if (!it_is_discrete_type (sinf2.type)
		  || !it_is_discrete_type (sinf1.type))
		error (ERR_case_label_type);
	      else
		{
		  /* Determine base type of label values. */
		  current_label_base_type
		    = result_of_compatible_types (sinf2.type, sinf1.type);
		  if (current_label_base_type == NULL)
		    error (ERR_case_label_types_inequality);
		  else
		    {
		      if (it_is_integer_type (current_label_base_type))
			{
			  /* Do accurate label values. */
			  if (it_is_integer_cardinal_type (sinf2.type))
			    r2
			      = integer_value_from_integer_cardinal_value (r2);
			  if (it_is_integer_cardinal_type (sinf1.type))
			    r1
			      = integer_value_from_integer_cardinal_value (r1);
			  i2 = INTEGER_NODE (r2)->integer_value;
			  i1 = INTEGER_NODE (r1)->integer_value;
			}
		      else
			{
			  c2 = cardinal_value (r2);
			  c1 = cardinal_value (r1);
			}
		      /* Test that label range is not empty. */
		      if ((it_is_integer_type (current_label_base_type)
			   && i2 > i1)
			  || (!it_is_integer_type (current_label_base_type)
			      && c2 > c1))
			error (ERR_case_label_value_bounds);
		      else
			{
			  /* Compare current label with all case preceding
			     labels. */
			  /* Process all preceding variants. */
			  for (error_flag = FALSE,
			       current_pred_variant = first_variant;
			       ;
			       current_pred_variant
			       = (CASE_VARIANT_NODE (current_pred_variant)
				  ->next_case_variant))
			    {
			      /* Process all labels of preceding variant. */
			      for (current_pred_label = current_pred_variant;;)
				{
				  current_pred_label
				    = CASE_LABEL_LIST (current_pred_label);
				  if (current_pred_label == NULL
				      || current_pred_label == current_label)
				    break;
				  if (CASE_LABEL_TYPE (current_pred_label)
				      == NULL)
				    continue;	/* error case label. */
				  /* Test type compatibility with
				     the preceding labels. */
				  if (result_of_compatible_types
				      (CASE_LABEL_TYPE (current_pred_label),
				       current_label_base_type)
				      == NULL)
				    {
				      error_flag = TRUE;
				      continue;
				    }
				  if (MODE (current_pred_label)
				      == ICNM_RECORD_CASE_LABEL_ELEMENT
				      || MODE (current_pred_label)
				      == ICNM_CASE_STATEMENT_LABEL_ELEMENT)
				    /* Consider further that it is label range
				       L..L where L is the corresponding
				       single label. */
				    label_value2
				      = label_value1
				      = (CASE_LABEL_ELEMENT_NODE
					 (current_pred_label)
					 ->case_label_value);
				  else
				    {
				      label_value2
					= (CASE_LABEL_RANGE_NODE
					   (current_pred_label)
					   ->case_label_range_left_bound);
				      label_value1
					= (CASE_LABEL_RANGE_NODE
					   (current_pred_label)
					   ->case_label_range_right_bound);
				    }
				  /* Preparation for the following label
				     value comparison. */
				  if (it_is_integer_type
				      (current_label_base_type)
				      && (it_is_integer_cardinal_type
					  (CASE_LABEL_TYPE
					   (current_pred_label))))
				    {
				      label_value2
					= integer_value_from_integer_cardinal_value
					(label_value2);
				      label_value1
					= integer_value_from_integer_cardinal_value
					(label_value1);
				    }
				  else if (it_is_integer_type
					   (CASE_LABEL_TYPE
					    (current_pred_label))
					   && (it_is_integer_cardinal_type
					       (current_label_base_type)))
				    {
				      i1 = c1;
				      i2 = c2;
				    }
				  /* Test on dublication of labels. */
				  if ((MODE (label_value2) == ICNM_INTEGER
				       && (((INTEGER_NODE (label_value2)
					     ->integer_value)
					    <= i2
					    &&
					    (INTEGER_NODE (label_value1)
					     ->integer_value)
					    >= i2)
					   ||
					   ((INTEGER_NODE (label_value2)
					     ->integer_value)
					    <= i1
					    &&
					    (INTEGER_NODE (label_value1)
					     ->integer_value)
					    >= i1)))
				      || (MODE (label_value2) != ICNM_INTEGER
					  && ((cardinal_value (label_value2)
					       <= c2
					       && cardinal_value (label_value1)
					       >= c2)
					      || (cardinal_value (label_value2)
						  <= c1
						  &&
						  cardinal_value (label_value1)
						  >= c1))))
				    error_flag = TRUE;
				}
			      if (current_variant == current_pred_variant)
				break;
			    }
			  /* Test that tag is type is compatible with type of
			     labels. */
			  if (subgraph_is_type (tag, &sinf1)
			      && (result_of_compatible_types
				  (tag, current_label_base_type)
				  == NULL))
			    error (ERR_case_label_and_tag_types_inequality);
			  else if (error_flag)
			    error (ERR_repeated_case_label);
			}
		    }
		}
	    }
	  if (error_flag)
	    r1 = r2 = current_label_base_type = NULL;
	  /* Set up accurate values of labels. */
	  CASE_LABEL_TYPE (current_label) = current_label_base_type;
	  if (MODE (current_label) == ICNM_RECORD_CASE_LABEL_ELEMENT
	      || MODE (current_label) == ICNM_CASE_STATEMENT_LABEL_ELEMENT)
	    CASE_LABEL_ELEMENT_NODE (current_label)->case_label_value = r1;
	  else
	    {
	      CASE_LABEL_RANGE_NODE (current_label)
		->case_label_range_left_bound = r2;
	      CASE_LABEL_RANGE_NODE (current_label)
		->case_label_range_right_bound = r1;
	    }
	}
    }
}



/* This page contain s source code for processing functional and procedural
   call. */


/* The function returns number of actual parameters of procedure (function)
   call with FIRST_ACTUAL_PARAMETER (it is to be pointer to actual parameter
   or NULL pointer). */

static int
number_of_actual_parameters (first_actual_parameter)
     register ICN_pointer first_actual_parameter;
{
  register int result;

  for (result = 0; first_actual_parameter != NULL;
       first_actual_parameter
       = ACTUAL_PARAMETER_NODE (first_actual_parameter)->next_actual_parameter)
    result++;
  return result;
}

/* The function processes all actual parameters and tests conformity (number
   of, modes of and types of) actual and the corresponding formal parameters
   of procedure PROCEDURE_NAME.  The function returns TRUE if no errors were
   fixed.  FORMAL and ACTUAL correspondingly are to refer to the first formal
   parameter and the first actual parameter. */

static int
process_actual_parameters (formal, actual, procedure_name)
     register ICN_pointer formal, actual;
     char *procedure_name;
{
  register ICN_pointer actual_or_its_array_element_type, formal_type;
  register int error_was_fixed;
  Tcard formal_size, actual_size;
  int actual_alignment, formal_alignment;

  /* Test conformity of actual and formal paramaeters. */
  for (error_was_fixed = FALSE; formal != NULL && actual != NULL;
       formal = next_parameter_type (formal),
       actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
    {
      set_source_line (actual);
      ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
	= eval (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
      formal_type = FORMAL_PARAMETER_TYPE (formal);
      if (formal_type == NULL)
	continue;		/* Error type of formal parameter. */
      subgraph_is_expression (r1, &sinf1);
      if (sinf1.it_is_array_parameter
	  ||
	  (VAR_FORMAL_PARAMETER (formal)
	   &&
	   r1_variable_test_with_error_parameter
	   (ERR_nonvariable_for_var_formal_parameter, procedure_name))
	  ||
	  (!VAR_FORMAL_PARAMETER (formal)
	   &&
	   r1_expression_test_with_error_parameter
	   (ERR_actual_parameter_is_not_expression, procedure_name)))
	{
	  /* Actual parameter mode (variable or value) is correct here. */
	  get_type_size_and_alignment (formal_type,
				       &formal_size, &formal_alignment);
	  get_type_size_and_alignment (sinf1.type,
				       &actual_size, &actual_alignment);
	  actual_or_its_array_element_type = sinf1.type;
	  if (ARRAY_FORMAL_PARAMETER (formal))
	    {
	      if (MODE (actual_or_its_array_element_type) == ICNM_ARRAY_TYPE)
		actual_or_its_array_element_type
		  = (ARRAY_TYPE_NODE (actual_or_its_array_element_type)
		     ->base_type);
	      else if (it_is_string_type (actual_or_its_array_element_type))
		actual_or_its_array_element_type = ICN_POINTER (&ICN_TD_char);
	    }
	  /* Test conformity on array parameter and conformity types
	     actual and formals. */
	  if (!((ARRAY_FORMAL_PARAMETER (formal)
		 || !sinf1.it_is_array_parameter)
		&&
		((types_are_identical (formal_type,
				       actual_or_its_array_element_type)
		  ||
		  (formal_type == ICN_POINTER (&ICN_TD_address)
		   && (MODE (actual_or_its_array_element_type)
		       == ICNM_POINTER_TYPE))
		  ||
		  (!VAR_FORMAL_PARAMETER (formal)
		   &&
		   !ARRAY_FORMAL_PARAMETER (formal)
		   &&
		   types_are_assignment_compatible (formal_type, sinf1.type)))
		 ||
		 ((formal_type == ICN_POINTER (&ICN_TD_word)
		   || formal_type == ICN_POINTER (&ICN_TD_byte))
		  &&
		  formal_alignment <= actual_alignment
		  &&
		  (formal_size == actual_size
		   || (ARRAY_FORMAL_PARAMETER (formal)
		       && actual_size % formal_size == 0))))))
	    {
	      error_with_parameter (ERR_invalid_actual_parameter,
				    procedure_name);
	      error_was_fixed = TRUE;
	    }
	  /* Short cardinal can be passed as cardinal by value.  Test this
	     and for other analogous types. */
	  else if (sinf1.it_is_constant && !ARRAY_FORMAL_PARAMETER (formal)
		   && !test_discrete_inclusion (r1, formal_type))
	    {
	      error_with_parameter (ERR_actual_parameter_value,
				    procedure_name);
	      error_was_fixed = TRUE;
	    }
	}
      /* Test that actual is not local or standard procedure. */
      if (r1 != NULL && MODE (r1) == ICNM_PROCEDURE
	  && it_is_local_or_standard_procedure (r1))
	{
	  error_with_parameter
	    (ERR_local_procedure_as_parameter,
	     IDENTIFIER_NODE (IDENTIFIER (r1))->identifier_string);
	  error_was_fixed = TRUE;
	}
    }
  if (actual != formal)
    {
      /* Number of actuals is not equal to number of formals here. */
      error_with_parameter (ERR_number_of_actual_parameters, procedure_name);
      error_was_fixed = TRUE;
    }
  /* Process the rest of actual parameters for analysis. */
  for (; actual != NULL;
       actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
    ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
      = eval (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
  return !error_was_fixed;
}

/* The function processes call construction and tests conformity the call
   and procedure declaration.  The function returns pointer to finish
   representation of the call construction.  The result may be differ from
   CALL_CONSTRUCTION because of constant folding (for standard procedure call).
   CALL_CONSTRUCTION is to refer to functional or procedural call node. */

static ICN_pointer
process_call (call_construction)
     register ICN_pointer call_construction;
{
  register ICN_pointer actual, procedure_designator, procedure_type;
  int procedure_has_correct_designator, error_flag;
  int it_is_function_call, alignment;
  Tcard c;
  char *current_procedure_name;

  it_is_function_call = MODE (call_construction) == ICNM_FUNCTION_CALL;
  if (it_is_function_call)
    FUNCTION_CALL_NODE (call_construction)->function_designator
      = eval (FUNCTION_CALL_NODE (call_construction)->function_designator);
  else
    PROCEDURE_CALL_NODE (call_construction)->procedure_designator
      = eval (PROCEDURE_CALL_NODE (call_construction)->procedure_designator);
  procedure_has_correct_designator = TRUE;
  current_procedure_name = "";
  procedure_designator = r1;
  if (r1 != NULL && MODE (r1) == ICNM_PROCEDURE)
    {
      /* Call designator is procedure designator. */
      procedure_type = PROCEDURE_NODE (r1)->procedure_type;
      current_procedure_name
	= IDENTIFIER_NODE (IDENTIFIER (r1))->identifier_string;
    }
  else if (subgraph_is_expression (r1, &sinf1) && !sinf1.it_is_array_parameter
	   && sinf1.it_is_designator
	   && sinf1.type != NULL && MODE (sinf1.type) == ICNM_PROCEDURE_TYPE)
    /* Call designator is expression. */
    procedure_type = sinf1.type;
  else
    procedure_has_correct_designator = FALSE;
  actual = (it_is_function_call
	    ? (FUNCTION_CALL_NODE (call_construction)
	       ->function_actual_parameter_list)
	    : (PROCEDURE_CALL_NODE (call_construction)
	       ->procedure_actual_parameter_list));
  set_source_line (actual);
  if (!procedure_has_correct_designator)
    {
      /* Process exceptions. */
      if (it_is_function_call && subgraph_is_type (r1, &sinf1))
	{
	  /* Call designator is type name. */
	  procedure_has_correct_designator = TRUE;
	  if (number_of_actual_parameters (actual) != 1)
	    error_with_parameter (ERR_number_of_actual_parameters,
				  current_procedure_name);
	  else
	    {
	      register ICN_pointer result_type;

	      result_type = sinf1.type;
	      ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
		= eval (ACTUAL_PARAMETER_NODE (actual)
			->actual_parameter_expression);
	      r1_expression_test_with_error_parameter
		(ERR_actual_parameter_is_not_expression,
		 current_procedure_name);
	      VALUE_TYPE (call_construction) = result_type;
	    }
	}
      else if (procedure_designator != NULL
	       && MODE (procedure_designator) == ICNM_C_FUNCTION)
	{
	  /* Call designator is C function. */
	  procedure_has_correct_designator = TRUE;
	  if (it_is_function_call)
	    VALUE_TYPE (call_construction) = ICN_POINTER (&ICN_TD_integer);
	  for (; actual != NULL;
	       actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter)
	    {
	      set_source_line (actual);
	      ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
		= eval (ACTUAL_PARAMETER_NODE (actual)
			->actual_parameter_expression);
	      r1_expression_test_with_error_parameter
		(ERR_actual_parameter_is_not_expression,
		 current_procedure_name);
	    }
	}
    }
  else if (procedure_type != NULL
	   && (it_is_function_call
	       == (PROCEDURE_TYPE_NODE (procedure_type)->procedure_result_type
		   != NULL)))
    {
      /* It is user defined procedure (procedure_type != NULL) or
	 fully declared standard procedure (see file m2-icode.c). */
      process_actual_parameters (next_parameter_type (procedure_type), actual,
				 current_procedure_name);
      if (it_is_function_call)
	VALUE_TYPE (call_construction)
	  = PROCEDURE_TYPE_NODE (procedure_type)->procedure_result_type;
#ifndef COROUTINE_ENABLE
      if (procedure_designator == ICN_POINTER (&ICN_D_transfer)
	  || procedure_designator == ICN_POINTER (&ICN_D_newprocess))
	error_with_parameter (ERR_coroutines, current_procedure_name);
#endif
      if (procedure_designator == ICN_POINTER (&ICN_D_iotransfer))
	error (ERR_iotransfer);
    }
  else if (it_is_function_call
	   && (procedure_designator == ICN_POINTER (&ICN_D_abs)
	       || procedure_designator == ICN_POINTER (&ICN_D_adr)
	       || procedure_designator == ICN_POINTER (&ICN_D_cap)
	       || procedure_designator == ICN_POINTER (&ICN_D_chr)
	       || procedure_designator == ICN_POINTER (&ICN_D_float)
	       || procedure_designator == ICN_POINTER (&ICN_D_high)
	       || procedure_designator == ICN_POINTER (&ICN_D_min)
	       || procedure_designator == ICN_POINTER (&ICN_D_max)
	       || procedure_designator == ICN_POINTER (&ICN_D_odd)
	       || procedure_designator == ICN_POINTER (&ICN_D_ord)
	       || procedure_designator == ICN_POINTER (&ICN_D_size)
	       || procedure_designator == ICN_POINTER (&ICN_D_tsize)
	       || procedure_designator == ICN_POINTER (&ICN_D_trunc)
	       || procedure_designator == ICN_POINTER (&ICN_D_val)))
    {
      /* It is generic standard functions (procedure_type == NULL). */
      if (number_of_actual_parameters (actual)
	  != (procedure_designator == ICN_POINTER (&ICN_D_val) ? 2 : 1))
	error_with_parameter (ERR_number_of_actual_parameters,
			      current_procedure_name);
      else
	{
	  ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
	    = eval
	    (ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	  if (procedure_designator == ICN_POINTER (&ICN_D_val))
	    {
	      /* Function VAL. */
	      actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter;
	      set_source_line (actual);
	      ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
		= eval
		(ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression);
	      if ((r2_type_test_with_error_parameter
		   (ERR_actual_parameter_is_not_type,
		    current_procedure_name))
		  && (r1_expression_test_with_error_parameter
		      (ERR_actual_parameter_is_not_expression,
		       current_procedure_name)))
		{
		  if (!it_is_discrete_type (sinf2.type)
		      || (result_of_compatible_types
			  (sinf1.type, ICN_POINTER (&ICN_TD_cardinal))
			  == NULL))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      if (it_is_integer_type (sinf2.type))
			c = (INTEGER_NODE (min_or_max (sinf2.type, FALSE))
			     ->integer_value);
		      else
			c = (CARDINAL_NODE (min_or_max (sinf2.type, FALSE))
			     ->cardinal_value);
		      if (CARDINAL_NODE (r1)->cardinal_value > c)
			error_with_parameter (ERR_actual_parameter_value,
					      current_procedure_name);
		      else
			{
			  if (it_is_integer_type (sinf2.type))
			    {
			      call_construction = create_node (ICNM_INTEGER);
			      INTEGER_NODE (call_construction)->integer_value
				= CARDINAL_NODE (r1)->cardinal_value;
			    }
			  else
			    {
			      call_construction = create_node (ICNM_CARDINAL);
			      CARDINAL_NODE (call_construction)->cardinal_value
				= CARDINAL_NODE (r1)->cardinal_value;
			    }
			  VALUE_TYPE (call_construction) = sinf2.type;
			  call_construction
			    = find_in_or_else_include_to_table
			    (call_construction);
			}
		    }
		  else
		    VALUE_TYPE (call_construction) = sinf2.type;
		}
	    }
	  else if (procedure_designator == ICN_POINTER (&ICN_D_adr))
	    {
	      /* Function ADR. */
	      if (r1_variable_test_with_error_parameter
		  (ERR_actual_parameter_is_not_variable,
		   current_procedure_name))
		VALUE_TYPE (call_construction) = ICN_POINTER (&ICN_TD_address);
	    }
	  else if (procedure_designator == ICN_POINTER (&ICN_D_min)
		   || procedure_designator == ICN_POINTER (&ICN_D_max))
	    {
	      if (r1_type_test_with_error_parameter
		  (ERR_actual_parameter_is_not_type,
		   current_procedure_name))
		{
		  if (!it_is_scalar_type (sinf1.type)
		      && MODE (sinf1.type) != ICNM_RANGE_TYPE)
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else
		    call_construction
		      = min_or_max (sinf1.type, (procedure_designator
						 == ICN_POINTER (&ICN_D_min)));
		}
	    }
	  else if (procedure_designator == ICN_POINTER (&ICN_D_size)
		   || procedure_designator == ICN_POINTER (&ICN_D_tsize))
	    {
	      /* Function SIZE or TSIZE. */
	      if (procedure_designator == ICN_POINTER (&ICN_D_tsize))
		error_flag = !(r1_type_test_with_error_parameter
			       (ERR_actual_parameter_is_not_type,
				current_procedure_name));
	      else if (!subgraph_is_type (r1, &sinf1)
		       && (!subgraph_is_expression (r1, &sinf1)
			   || !sinf1.it_is_designator))
		{
		  error_with_parameter
		    (ERR_actual_parameter_is_not_variable_or_type,
		     current_procedure_name);
		  error_flag = TRUE;
		}
	      else
		error_flag = FALSE;
	      if (!error_flag)
		{
		  call_construction = create_node (ICNM_CARDINAL);
		  CARDINAL_NODE (call_construction)->value_type
		    = ICN_POINTER (&ICN_TD_cardinal_or_integer);
		  get_type_size_and_alignment
		    (sinf1.type,
		     &CARDINAL_NODE (call_construction)->cardinal_value,
		     &alignment);
		  call_construction
		    = do_accurate_type_value (call_construction);
		}
	    }
	  else if (procedure_designator == ICN_POINTER (&ICN_D_high))
	    {
	      /* Function HIGH. */
	      subgraph_is_expression (r1, &sinf1);
	      if (sinf1.it_is_array_parameter
		  || (r1_expression_test_with_error_parameter
		      (ERR_actual_parameter_is_not_expression,
		       current_procedure_name)))
		{
		  if (!sinf1.it_is_array_parameter
		      && MODE (sinf1.type) != ICNM_ARRAY_TYPE)
		    error_with_parameter (ERR_actual_parameter_is_not_array,
					  current_procedure_name);
		  else if (!sinf1.it_is_array_parameter)
		    {
		      call_construction = create_node (ICNM_CARDINAL);
		      CARDINAL_NODE (call_construction)->value_type
			= ICN_POINTER (&ICN_TD_cardinal_or_integer);
		      CARDINAL_NODE (call_construction)->cardinal_value
			= array_index_span (ARRAY_TYPE_NODE (sinf1.type)
					    ->array_index_type);
		      call_construction
			= do_accurate_type_value (call_construction);
		    }
		  else
		    VALUE_TYPE (call_construction)
		      = (third_edition_flag
			 ? ICN_POINTER (&ICN_TD_cardinal)
			 : ICN_POINTER (&ICN_TD_integer));
		}
	    }
	  else if (r1_expression_test_with_error_parameter
		   (ERR_actual_parameter_is_not_expression,
		    current_procedure_name))
	    {
	      /* It is standard function whose formal parameter is to be
		 expression. */
	      if (procedure_designator == ICN_POINTER (&ICN_D_abs))
		{
		  /* Function ABS. */
		  register ICN_pointer base_type;

		  base_type = through_range_type (sinf1.type);
		  if (!it_is_real_type (base_type)
		      && !it_is_integer_type (base_type)
		      && !it_is_integer_cardinal_type (base_type))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      if (it_is_real_type (base_type))
			{
			  call_construction = create_node (ICNM_REAL);
			  REAL_NODE (call_construction)->real_value
			    = (REAL_NODE (r1)->real_value < 0.0
			       ? -REAL_NODE (r1)->real_value
			       : REAL_NODE (r1)->real_value);
			}
		      else if (it_is_integer_type (base_type))
			{
			  call_construction = create_node (ICNM_INTEGER);
			  INTEGER_NODE (call_construction)->integer_value
			    = (INTEGER_NODE (r1)->integer_value < 0
			       ? -INTEGER_NODE (r1)->integer_value
			       : INTEGER_NODE (r1)->integer_value);
			}
		      else
			call_construction = r1;
		      VALUE_TYPE (call_construction) = base_type;
		      call_construction
			= do_accurate_type_value (call_construction);
		    }
		  else
		    VALUE_TYPE (call_construction) = base_type;
		}
	      else if (procedure_designator == ICN_POINTER (&ICN_D_cap))
		{
		  /* Function CAP. */
		  if (result_of_compatible_types
		      (sinf1.type, ICN_POINTER (&ICN_TD_char)) == NULL)
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      call_construction = create_node (ICNM_CARDINAL);
		      VALUE_TYPE (call_construction)
			= ICN_POINTER (&ICN_TD_char);
		      CARDINAL_NODE (call_construction)->cardinal_value
			= M2_cap ((int) cardinal_value (r1));
		      call_construction
			= find_in_or_else_include_to_table (call_construction);
		    }
		  else
		    VALUE_TYPE (call_construction)
		      = ICN_POINTER (&ICN_TD_char);
		}
	      else if (procedure_designator == ICN_POINTER (&ICN_D_chr))
		{
		  /* Function CHR. */
		  if (!(types_are_assignment_compatible
			(sinf1.type, ICN_POINTER (&ICN_TD_integer))))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      error_flag = FALSE;
		      if (MODE (r1) != ICNM_INTEGER)
			c = CARDINAL_NODE (r1)->cardinal_value;
		      else if (INTEGER_NODE (r1)->integer_value < 0)
			{
			  error_with_parameter (ERR_actual_parameter_value,
						current_procedure_name);
			  error_flag = TRUE;
			}
		      else
			c = INTEGER_NODE (r1)->integer_value;
		      if (!error_flag)
			{
			  if (c > CHARACTER_MAX)
			    error_with_parameter (ERR_actual_parameter_value,
						  current_procedure_name);
			  else
			    {
			      call_construction = create_node (ICNM_CARDINAL);
			      CARDINAL_NODE (call_construction)->value_type
				= ICN_POINTER (&ICN_TD_char);
			      CARDINAL_NODE (call_construction)->cardinal_value
				= c;
			      call_construction
				= find_in_or_else_include_to_table
				(call_construction);
			    }
			}
		    }
		  else
		    VALUE_TYPE (call_construction)
		      = ICN_POINTER (&ICN_TD_char);
		}
	      else if (procedure_designator == ICN_POINTER (&ICN_D_float))
		{
		  /* Function FLOAT. */
		  if ((!types_are_assignment_compatible
		       (sinf1.type, ICN_POINTER (&ICN_TD_integer))))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      call_construction = create_node (ICNM_REAL);
		      REAL_NODE (call_construction)->value_type
			= ICN_POINTER (&ICN_TD_real);
		      REAL_NODE (call_construction)->real_value
			= (MODE (r1) == ICNM_INTEGER
			   ? INTEGER_NODE (r1)->integer_value
			   : CARDINAL_NODE (r1)->cardinal_value);
		      call_construction
			= do_accurate_type_value (call_construction);
		    }
		  else
		    VALUE_TYPE (call_construction)
		      = ICN_POINTER (&ICN_TD_real);
		}
	      else if (procedure_designator == ICN_POINTER (&ICN_D_odd))
		{
		  /* Function ODD. */
		  if (!(types_are_assignment_compatible
			(sinf1.type, ICN_POINTER (&ICN_TD_integer))))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    if ((MODE (r1) == ICNM_INTEGER
			 && INTEGER_NODE (r1)->integer_value % 2 == 0)
			|| (MODE (r1) == ICNM_CARDINAL
			    && CARDINAL_NODE (r1)->cardinal_value % 2 == 0))
		      call_construction = ICN_POINTER (&ICN_V_false);
		    else
		      call_construction = ICN_POINTER (&ICN_V_true);
		  else
		    VALUE_TYPE (call_construction)
		      = ICN_POINTER (&ICN_TD_boolean);
		}
	      else if (procedure_designator == ICN_POINTER (&ICN_D_ord))
		{
		  /* Function ORD. */
		  if (!it_is_discrete_type (through_range_type (sinf1.type)))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      error_flag = FALSE;
		      if (MODE (r1) != ICNM_INTEGER)
			c = cardinal_value (r1);
		      else if (INTEGER_NODE (r1)->integer_value < 0)
			{
			  error_with_parameter (ERR_actual_parameter_value,
						current_procedure_name);
			  error_flag = TRUE;
			}
		      else
			c = INTEGER_NODE (r1)->integer_value;
		      if (!error_flag)
			{
			  call_construction = create_node (ICNM_CARDINAL);
			  CARDINAL_NODE (call_construction)->value_type
			    = ICN_POINTER (&ICN_TD_cardinal_or_integer);
			  CARDINAL_NODE (call_construction)->cardinal_value
			    = c;
			  call_construction
			    = do_accurate_type_value (call_construction);
			}
		    }
		  else
		    VALUE_TYPE (call_construction)
		      = (third_edition_flag
			 ? ICN_POINTER (&ICN_TD_cardinal)
			 : ICN_POINTER (&ICN_TD_integer));
		}
	      else if (procedure_designator == ICN_POINTER (&ICN_D_trunc))
		{
		  /* Function TRUNC. */
		  if (!it_is_real_type (sinf1.type))
		    error_with_parameter (ERR_actual_parameter_type,
					  current_procedure_name);
		  else if (sinf1.it_is_constant)
		    {
		      if (REAL_NODE (r1)->real_value > -1.0)
			{
			  call_construction = create_node (ICNM_CARDINAL);
			  CARDINAL_NODE (call_construction)->value_type
			    = ICN_POINTER (&ICN_TD_cardinal_or_integer);
			  CARDINAL_NODE (call_construction)->cardinal_value
			    = REAL_NODE (r1)->real_value;
			}
		      else
			{
			  call_construction = create_node (ICNM_INTEGER);
			  INTEGER_NODE (call_construction)->value_type
			    = ICN_POINTER (&ICN_TD_integer);
			  INTEGER_NODE (call_construction)->integer_value
			    = REAL_NODE (r1)->real_value;
			}
		      call_construction
			= do_accurate_type_value (call_construction);
		    }
		  else
		    VALUE_TYPE (call_construction)
		      = (third_edition_flag
			 ? ICN_POINTER (&ICN_TD_cardinal)
			 : ICN_POINTER (&ICN_TD_integer));
		}
	    }
	}
    }
  else if (it_is_function_call)
    /* All standard functions are looked already.  It is erroneously declared
       function. */
    procedure_has_correct_designator = FALSE;
  /* It may be standard procedure. */
  else if (procedure_designator == ICN_POINTER (&ICN_D_halt))
    {
      /* Procedure HALT. */
      if (actual != NULL)
	error_with_parameter (ERR_number_of_actual_parameters,
			      current_procedure_name);
    }
  else if (procedure_designator == ICN_POINTER (&ICN_D_excl)
	   || procedure_designator == ICN_POINTER (&ICN_D_incl))
    {
      /* Procedure EXCL or INCL. */
      if (number_of_actual_parameters (actual) != 2)
	error_with_parameter (ERR_number_of_actual_parameters,
			      current_procedure_name);
      else
	{
	  ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
	    = eval (ACTUAL_PARAMETER_NODE (actual)
		    ->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter;
	  set_source_line (actual);
	  ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
	    = eval (ACTUAL_PARAMETER_NODE (actual)
		    ->actual_parameter_expression);
	  if (r2_variable_test_with_error_parameter
	      (ERR_actual_parameter_is_not_variable, current_procedure_name)
	      &&
	      r1_expression_test_with_error_parameter
	      (ERR_actual_parameter_is_not_expression, current_procedure_name))
	    {
	      if (MODE (sinf2.type) != ICNM_SET_TYPE
		  || (result_of_compatible_types
		      (SET_TYPE_NODE (sinf2.type)->base_type, sinf1.type)
		      == NULL))
		error_with_parameter (ERR_actual_parameter_type,
				      current_procedure_name);
	      else if (sinf1.it_is_constant
		       && !(test_discrete_inclusion
			    (r1, SET_TYPE_NODE (sinf2.type)->base_type)))
		error_with_parameter (ERR_actual_parameter_value,
				      current_procedure_name);
	    }
	}
    }
  else if (procedure_designator == ICN_POINTER (&ICN_D_inc)
	   || procedure_designator == ICN_POINTER (&ICN_D_dec))
    {
      /* Procedure INC or DEC. */
      if (number_of_actual_parameters (actual) > 2 || actual == NULL)
	error_with_parameter (ERR_number_of_actual_parameters,
			      current_procedure_name);
      else
	{
	  ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
	    = eval (ACTUAL_PARAMETER_NODE (actual)
		    ->actual_parameter_expression);
	  actual = ACTUAL_PARAMETER_NODE (actual)->next_actual_parameter;
	  if (actual == NULL)
	    {
	      r2 = r1;
	      r1 = create_node (ICNM_CARDINAL);
	      CARDINAL_NODE (r1)->value_type
		= ICN_POINTER (&ICN_TD_short_cardinal_or_integer);
	      CARDINAL_NODE (r1)->cardinal_value = 1;
	      r1 = find_in_or_else_include_to_table (r1);
	    }
	  else
	    {
	      set_source_line (actual);
	      ACTUAL_PARAMETER_NODE (actual)->actual_parameter_expression
		= eval (ACTUAL_PARAMETER_NODE (actual)
			->actual_parameter_expression);
	    }
	  if (r2_variable_test_with_error_parameter
	      (ERR_actual_parameter_is_not_variable, current_procedure_name)
	      &&
	      r1_expression_test_with_error_parameter
	      (ERR_actual_parameter_is_not_expression, current_procedure_name)
	      &&
	      ((!it_is_discrete_type (through_range_type (sinf2.type))
		&& sinf2.type != ICN_POINTER (&ICN_TD_address))
	       || (!it_is_cardinal_type (through_range_type (sinf1.type))
		   && !it_is_integer_type (through_range_type (sinf1.type))
		   && !it_is_integer_cardinal_type (through_range_type
						    (sinf1.type))
		   && (sinf2.type != ICN_POINTER (&ICN_TD_address)
		       || sinf1.type != ICN_POINTER (&ICN_TD_address)))))
	    error_with_parameter (ERR_actual_parameter_type,
				  current_procedure_name);
	}
    }
  else if (procedure_type != NULL)
    error_with_parameter (ERR_function_used_as_procedure,
			  current_procedure_name);
  else
    procedure_has_correct_designator = FALSE;
  if (procedure_designator != NULL && !procedure_has_correct_designator)
    error_with_parameter ((it_is_function_call
			   ? ERR_function_call : ERR_call_statement),
			  current_procedure_name);
  return call_construction;
}



/* This page contains source code for processing type definitions
   and expressions. */


/* The following recursive function processes all type definitions in record
   element list and case labels for record case part.  The record element list
   is given by first element RECORD_ELEMENT which is to be null pointer
   (empty list) or pointer to field or record case part. */

static void
process_record_element_list (record_element)
     register ICN_pointer record_element;
{
  register ICN_pointer case_variant, pred_field_type, result_type_of_field;
  int tag_type_is_erroneous;

  for (result_type_of_field = pred_field_type = NULL;
       MODE (record_element) == ICNM_FIELD
       || MODE (record_element) == ICNM_RECORD_CASE_PART;)
    {
      set_source_line (record_element);
      if (MODE (record_element) == ICNM_FIELD)
	{
	  if (FIELD_NODE (record_element)->field_type == pred_field_type)
	    /* Current field is in field list (e.g. a, b, c : type). */
	    FIELD_NODE (record_element)->field_type = result_type_of_field;
	  else
	    {
	      pred_field_type = FIELD_NODE (record_element)->field_type;
	      eval (pred_field_type);
	      FIELD_NODE (record_element)->field_type = NULL;
	      if (r1_type_test (ERR_field_type))
		FIELD_NODE (record_element)->field_type = r1;
	      result_type_of_field = FIELD_NODE (record_element)->field_type;
	    }
	  record_element = FIELD_NODE (record_element)->next_field;
	}
      else
	{
	  tag_type_is_erroneous = FALSE;
	  /* Remember that tag is represented as field node and record case
	     part node in general case (e.g. CASE b: BOOLEAN ...). */
	  if (RECORD_CASE_PART_NODE (record_element)->case_tag_type
	      == pred_field_type)
	    r1
	      = RECORD_CASE_PART_NODE (record_element)->case_tag_type
	      = result_type_of_field;
	  else
	    {
	      eval (RECORD_CASE_PART_NODE (record_element)->case_tag_type);
	      tag_type_is_erroneous = !r1_type_test (ERR_case_tag_is_not_type);
	    }
	  /* Test tag type. */
	  if (!tag_type_is_erroneous && !it_is_discrete_type (r1)
	      && r1 != NULL && MODE (r1) != ICNM_RANGE_TYPE)
	    {
	      error (ERR_record_case_tag_type);
	      tag_type_is_erroneous = TRUE;
	    }
	  RECORD_CASE_PART_NODE (record_element)->case_tag_type
	    = (tag_type_is_erroneous ? NULL : r1);
	  process_case_labels
	    (RECORD_CASE_PART_NODE (record_element)->case_tag_type,
	     record_element);
	  /* Process record element lists which are in case variants. */
	  for (case_variant
	       = (RECORD_CASE_PART_NODE (record_element)
		  ->record_case_variant_list);
	       MODE (case_variant) == ICNM_RECORD_CASE_VARIANT;
	       case_variant
	       = RECORD_CASE_VARIANT_NODE (case_variant)->next_case_variant)
	    {
	      set_source_line (case_variant);
	      process_record_element_list
		(RECORD_CASE_VARIANT_NODE (case_variant)
		 ->case_variant_elements);
	    }
	  record_element = (RECORD_CASE_PART_NODE (record_element)
			    ->record_element_after_case_part);
	}
    }
}

/* The following function processes (tests and sets up accurate bounds value
   and base type) range type definition.  RANGE_TYPE is to be pointer to
   range type. */

static void
process_range_type (range_type)
     register ICN_pointer range_type;
{
  register ICN_pointer common_type_for_bounds;
  register ICN_pointer common_type_for_bounds_and_base_type;
  register int bounds_has_invalid_type;

  if (RANGE_TYPE_NODE (range_type)->base_type != NULL)
    {
      eval (RANGE_TYPE_NODE (range_type)->base_type);
      RANGE_TYPE_NODE (range_type)->base_type = NULL;
      /* Test base type correctness. */
      if (r1_type_test (ERR_range_base_type))
	{
	  if (it_is_real_type (sinf1.type) || !it_is_scalar_type (sinf1.type))
	    error (ERR_range_base_type);
	  else
	    RANGE_TYPE_NODE (range_type)->base_type = r1;
	}
    }
  eval (RANGE_TYPE_NODE (range_type)->left_range_bound);
  eval (RANGE_TYPE_NODE (range_type)->right_range_bound);
  RANGE_TYPE_NODE (range_type)->left_range_bound
    = RANGE_TYPE_NODE (range_type)->right_range_bound
    = NULL;
  /* Test bounds and conformity of bounds and base type. */
  if (r2_constant_test (ERR_no_constant_in_range_bound)
      && r1_constant_test (ERR_no_constant_in_range_bound))
    {
      bounds_has_invalid_type = (!it_is_discrete_type (sinf2.type)
				 || !it_is_discrete_type (sinf1.type));
      if (!bounds_has_invalid_type)
	common_type_for_bounds
	  = result_of_compatible_types (sinf2.type, sinf1.type);
      if (bounds_has_invalid_type || common_type_for_bounds == NULL)
	error (ERR_range_bound_type);
      else
	{
	  /* Reduce integer-cardinal bound type to values of integer or
	     cardinal type only if common type is not integer-cardinal type. */
	  if (it_is_integer_type (common_type_for_bounds)
	      && it_is_integer_cardinal_type (sinf2.type))
	    r2 = integer_value_from_integer_cardinal_value (r2);
	  if (it_is_integer_type (common_type_for_bounds)
	      && it_is_integer_cardinal_type (sinf1.type))
	    r1 = integer_value_from_integer_cardinal_value (r1);
	  /* Reduce string bound type to character type. */
	  if (it_is_string_type (sinf2.type))
	    r2 = character_value_from_single_character_string (r2);
	  if (it_is_string_type (sinf1.type))
	    r1 = character_value_from_single_character_string (r1);
	  /* Test that range is not empty. */
	  if ((it_is_integer_type (common_type_for_bounds)
	       && (INTEGER_NODE (r2)->integer_value
		   > INTEGER_NODE (r1)->integer_value))
	      || (!it_is_integer_type (common_type_for_bounds)
		  && (CARDINAL_NODE (r2)->cardinal_value
		      > CARDINAL_NODE (r1)->cardinal_value)))
	    error (ERR_range_bound_value);
	  else if (RANGE_TYPE_NODE (range_type)->base_type == NULL)
	    {
	      /* Set up base type and reduce integer-cardinal bound type to
		 values of integer or cardinal type (e.g. integer or
		 cardinal). */
	      if (it_is_integer_type (common_type_for_bounds))
		RANGE_TYPE_NODE (range_type)->base_type
		  = (it_is_string_type (common_type_for_bounds)
		     ? ICN_POINTER (&ICN_TD_char) : common_type_for_bounds);
	      else
		{
		  if (it_is_integer_cardinal_type (sinf2.type))
		    r2 = cardinal_value_from_integer_cardinal_value (r2);
		  if (it_is_integer_cardinal_type (sinf1.type))
		    r1 = cardinal_value_from_integer_cardinal_value (r1);
		  /* Type of max value (right bound). */
		  RANGE_TYPE_NODE (range_type)->base_type
		    = CARDINAL_NODE (r1)->value_type;
		}
	      /* This code is needed to use the subrange as opaque type
		 (see the 4th edition of Wirth's book).  To economy memory use
		 explicitly defined base type of the subrange. */
	      if (RANGE_TYPE_NODE (range_type)->base_type ==
		  ICN_POINTER (&ICN_TD_short_cardinal))
		RANGE_TYPE_NODE (range_type)->base_type
		  = ICN_POINTER (&ICN_TD_cardinal);
	      else if (RANGE_TYPE_NODE (range_type)->base_type
		       == ICN_POINTER (&ICN_TD_short_integer))
		RANGE_TYPE_NODE (range_type)->base_type
		  = ICN_POINTER (&ICN_TD_integer);
	      RANGE_TYPE_NODE (range_type)->left_range_bound = r2;
	      RANGE_TYPE_NODE (range_type)->right_range_bound = r1;
	    }
	  else
	    {
	      /* Explicitly defined base type of range. */
	      common_type_for_bounds_and_base_type
		= result_of_compatible_types
		(common_type_for_bounds,
		 RANGE_TYPE_NODE (range_type)->base_type);
	      if (common_type_for_bounds_and_base_type == NULL)
		error (ERR_range_base_type);
	      else
		{
		  /* Reduce integer-cardinal bound type to values of integer
		     or cardinal type (e.g. integer or cardinal). */
		  if (it_is_integer_type (common_type_for_bounds_and_base_type)
		      && it_is_integer_cardinal_type (common_type_for_bounds))
		    {
		      r2 = integer_value_from_integer_cardinal_value (r2);
		      r1 = integer_value_from_integer_cardinal_value (r1);
		    }
		  if (it_is_cardinal_type
		      (common_type_for_bounds_and_base_type)
		      && it_is_integer_cardinal_type (sinf2.type))
		    r2 = cardinal_value_from_integer_cardinal_value (r2);
		  if (it_is_cardinal_type
		      (common_type_for_bounds_and_base_type)
		      && it_is_integer_cardinal_type (sinf1.type))
		    r1 = cardinal_value_from_integer_cardinal_value (r1);
		  if (common_type_for_bounds_and_base_type
		      != RANGE_TYPE_NODE (range_type)->base_type)
		    /* Base type does not contain bound values. */
		    error (ERR_range_base_type);
		  else
		    {
		      RANGE_TYPE_NODE (range_type)->left_range_bound = r2;
		      RANGE_TYPE_NODE (range_type)->right_range_bound = r1;
		    }
		}
	    }
	}
    }
}

/* The function processes (tests and set up result type) relation operation.
   The function returns pointer to finish representation of the original
   construction.  The result may be differ from CONSTRUCTION because of
   constant folding.  CONSTRUCTION is to refer to relational operation and MODE
   is to be its icode mode. */

static ICN_pointer
process_relation_operation (mode, construction)
     register enum icode_node_mode mode;
     register ICN_pointer construction;
{
  register enum icode_node_mode left_operand_mode;
  register char *error_message, result_value, operands_have_erroneous_type;

  LEFT_OPERAND (construction) = eval (LEFT_OPERAND (construction));
  RIGHT_OPERAND (construction) = eval (RIGHT_OPERAND (construction));
  VALUE_TYPE (construction) = ICN_POINTER (&ICN_TD_boolean);
  /* Define possible error message. */
  if (mode == ICNM_EQUAL)
    error_message = ERR_equal_operands;
  else if (mode == ICNM_UNEQUAL)
    error_message = ERR_unequal_operands;
  else if (mode == ICNM_LESS_THAN)
    error_message = ERR_less_than_operands;
  else if (mode == ICNM_GREATER_THAN)
    error_message = ERR_greater_than_operands;
  else if (mode == ICNM_LESS_OR_EQUAL)
    error_message = ERR_less_or_equal_or_set_inclusion_operands;
  else if (mode == ICNM_GREATER_OR_EQUAL)
    error_message = ERR_greater_or_equal_or_set_inclusion_operands;
  else
    abort ();
  if (r2_expression_test (error_message) && r1_expression_test (error_message))
    {
      /* Test types of operands. */
      operands_have_erroneous_type
	= result_of_compatible_types (sinf2.type, sinf1.type) == NULL;
      if (mode == ICNM_LESS_THAN || mode == ICNM_GREATER_THAN)
	operands_have_erroneous_type
	  = (operands_have_erroneous_type
	     || !it_is_scalar_type (through_range_type (sinf2.type)));
      else if (mode == ICNM_LESS_OR_EQUAL && mode == ICNM_GREATER_OR_EQUAL)
	operands_have_erroneous_type = (operands_have_erroneous_type
					|| (!it_is_scalar_type (through_range_type (sinf2.type))
					    && MODE (sinf2.type) != ICNM_SET_TYPE));
      if (operands_have_erroneous_type)
	error (error_message);
      else if (sinf2.it_is_constant && sinf1.it_is_constant)
	{
	  /* Do constant folding. */
	  if (mode == ICNM_GREATER_THAN || mode == ICNM_GREATER_OR_EQUAL)
	    {
	      register ICN_pointer temp;

	      temp = r1;
	      r1 = r2;
	      r2 = temp;
	    }
	  result_value = FALSE;
	  /* Evalute equality. */
	  if (mode != ICNM_LESS_THAN && mode != ICNM_GREATER_THAN)
	    {
	      left_operand_mode = MODE (r2);
	      if (left_operand_mode == ICNM_INTEGER)
		result_value = (INTEGER_NODE (r2)->integer_value
				== INTEGER_NODE (r1)->integer_value);
	      else if (left_operand_mode == ICNM_STRING
		       && MODE (r1) == ICNM_STRING)
		result_value = r2 == r1;
	      else if (left_operand_mode == ICNM_REAL)
		result_value
		  = REAL_NODE (r2)->real_value == REAL_NODE (r1)->real_value;
	      else if (left_operand_mode == ICNM_SET)
		result_value
		  = SET_NODE (r2)->set_value == SET_NODE (r1)->set_value;
	      else
		result_value = cardinal_value (r2) == cardinal_value (r1);
	    }
	  /* Additionaly evaluate less then. */
	  if (mode != ICNM_EQUAL && mode != ICNM_UNEQUAL)
	    {
	      left_operand_mode = MODE (r2);
	      if (left_operand_mode == ICNM_INTEGER)
		result_value = (result_value
				|| (INTEGER_NODE (r2)->integer_value
				    < INTEGER_NODE (r1)->integer_value));
	      else if (left_operand_mode == ICNM_REAL)
		result_value = (result_value
				|| (REAL_NODE (r2)->real_value
				    < REAL_NODE (r1)->real_value));
	      else if (left_operand_mode == ICNM_SET)
		result_value = (result_value
				|| (((SET_NODE (r2)->set_value
				      | SET_NODE (r1)->set_value)
				     ^ SET_NODE (r1)->set_value) == 0));
	      else
		result_value = (result_value
				|| cardinal_value (r2) < cardinal_value (r1));
	    }
	  if (mode == ICNM_UNEQUAL)
	    result_value = !result_value;
	  construction = (result_value
			  ? ICN_POINTER (&ICN_V_true)
			  : ICN_POINTER (&ICN_V_false));
	}
    }
  return construction;
}

/* The function processes (tests and set up result type) generic arithmetic
   operation (plus, minus or multiplication).  The function returns pointer to
   finish representation of the original construction.  The result may be
   differ from CONSTRUCTION because of constant folding.  CONSTRUCTION is to
   refer to plus, minus or multiplication node and MODE is to be its icode
   mode. */

static ICN_pointer
process_generic_arithmetic_operation (mode, construction)
     register enum icode_node_mode mode;
     register ICN_pointer construction;
{
  register ICN_pointer result_type;
  char *error_message;

  LEFT_OPERAND (construction) = eval (LEFT_OPERAND (construction));
  RIGHT_OPERAND (construction) = eval (RIGHT_OPERAND (construction));
  /* Define possible error message. */
  error_message = (mode == ICNM_ADDITION
		   ? ERR_addition_operands
		   : (mode == ICNM_SUBTRACTION
		      ? ERR_subtraction_operands : ERR_mult_operands));
  if (r2_expression_test (error_message) && r1_expression_test (error_message))
    {
      /* Test types of operands. */
      result_type = result_of_compatible_types (sinf2.type, sinf1.type);
      if (result_type == NULL
	  || (MODE (result_type) != ICNM_SET_TYPE
	      && (!it_is_scalar_type (result_type)
		  || it_is_character_type (result_type)
		  || MODE (result_type) == ICNM_ENUMERATION_TYPE)))
	error (error_message);
      else if (sinf2.it_is_constant && sinf1.it_is_constant)
	{
	  /* Do constant folding. */
	  if (MODE (result_type) == ICNM_SET_TYPE)
	    {
	      construction = create_node (ICNM_SET);
	      SET_NODE (construction)->set_value
		= (mode == ICNM_ADDITION
		   ? SET_NODE (r2)->set_value | SET_NODE (r1)->set_value
		   : (mode == ICNM_SUBTRACTION
		      ? (SET_NODE (r2)->set_value & ~SET_NODE (r1)->set_value)
		      : (SET_NODE (r2)->set_value
			 & SET_NODE (r1)->set_value)));
	    }
	  else if (it_is_real_type (result_type))
	    {
	      construction = create_node (ICNM_REAL);
	      REAL_NODE (construction)->real_value
		= (mode == ICNM_ADDITION
		   ? (REAL_NODE (r2)->real_value + REAL_NODE (r1)->real_value)
		   : (mode == ICNM_SUBTRACTION
		      ? (REAL_NODE (r2)->real_value
			 - REAL_NODE (r1)->real_value)
		      : (REAL_NODE (r2)->real_value
			 * REAL_NODE (r1)->real_value)));
	    }
	  else
	    {
	      /* Operands are integer number.  Reduce integer-cardinal type
		 value to integer or cardinal value if an operand has not
		 integer-cardinal type. */
	      if (it_is_integer_type (result_type)
		  && it_is_integer_cardinal_type (sinf2.type))
		r2 = integer_value_from_integer_cardinal_value (r2);
	      if (it_is_integer_type (result_type)
		  && it_is_integer_cardinal_type (sinf1.type))
		r1 = integer_value_from_integer_cardinal_value (r1);
	      if (it_is_integer_type (result_type))
		{
		  construction = create_node (ICNM_INTEGER);
		  INTEGER_NODE (construction)->integer_value
		    = (mode == ICNM_ADDITION
		       ? (INTEGER_NODE (r2)->integer_value
			  + INTEGER_NODE (r1)->integer_value)
		       : (mode == ICNM_SUBTRACTION
			  ? (INTEGER_NODE (r2)->integer_value
			     - INTEGER_NODE (r1)->integer_value)
			  : (INTEGER_NODE (r2)->integer_value
			     * INTEGER_NODE (r1)->integer_value)));
		}
	      else if (mode == ICNM_SUBTRACTION
		       && it_is_integer_cardinal_type (result_type)
		       && (CARDINAL_NODE (r2)->cardinal_value
			   < CARDINAL_NODE (r1)->cardinal_value))
		{
		  /* Result is negative therefore result type will integer
		     type. */
		  construction = create_node (ICNM_INTEGER);
		  result_type = ICN_POINTER (&ICN_TD_integer);
		  INTEGER_NODE (construction)->integer_value
		    = ((-(Tint) CARDINAL_NODE (r1)->cardinal_value)
		       + (Tint) CARDINAL_NODE (r2)->cardinal_value);
		}
	      else
		{
		  /* Result type will integer-cardinal type. */
		  construction = create_node (ICNM_CARDINAL);
		  CARDINAL_NODE (construction)->cardinal_value
		    = ((mode == ICNM_ADDITION
			? (CARDINAL_NODE (r2)->cardinal_value
			   + CARDINAL_NODE (r1)->cardinal_value)
			: (mode == ICNM_SUBTRACTION
			   ? (CARDINAL_NODE (r2)->cardinal_value
			      - CARDINAL_NODE (r1)->cardinal_value)
			   : (CARDINAL_NODE (r2)->cardinal_value
			      * CARDINAL_NODE (r1)->cardinal_value))));
		}
	    }
	  VALUE_TYPE (construction) = result_type;
	  /* Do more accurate type (e.g. SHORTCARD instead of CARDINAL). */
	  construction = do_accurate_type_value (construction);
	}
      else
	VALUE_TYPE (construction) = result_type;
    }
  return construction;
}

/* The following function processes (tests and evaluate definition of) type,
   constant or variable declarations.  It is needed when its identifier
   evaluated.  Also may be processed variables in the same variable declaration
   list (e.g. a, b, c: type;).  DECLARATION to be pointer to type, constant or
   variable declaration.

   This implementation allows that identifier may be used before
   its definition, for example
            VAR v : t ;
	    TYPE t = CARDINAL ;
	
   Therefore member ..._was_processed is used for the single processing and
   ..._is_being_processed is used in function eval for declaration loop
   fixing. */

static void
process_type_constant_variable_declaration (declaration)
     register ICN_pointer declaration;
{
  enum icode_node_mode declaration_mode;
  register ICN_pointer curref, pred_variable_type, result_variable_type;
  register ICN_pointer saved_current_scope, saved_r1, saved_r2;
  char *declaration_name;

  saved_current_scope = current_scope;
  saved_r1 = r1;
  saved_r2 = r2;
  /* For correct search of identifiers in the declaration definition (function
     eval). */
  current_scope = SCOPE (declaration);
  declaration_name
    = IDENTIFIER_NODE (IDENTIFIER (declaration))->identifier_string;
  declaration_mode = MODE (declaration);
  if (declaration_mode == ICNM_TYPE_DECLARATION
      && !TYPE_DECLARATION_NODE (declaration)->type_was_processed)
    {
      TYPE_DECLARATION_NODE (declaration)->type_is_being_processed = TRUE;
      eval (TYPE_DECLARATION_NODE (declaration)->type_definition);
      if (r1_type_test_with_error_parameter (ERR_no_type_in_type_definition,
					     declaration_name))
	TYPE_DECLARATION_NODE (declaration)->type_definition = r1;
      else
	TYPE_DECLARATION_NODE (declaration)->type_definition = NULL;
      TYPE_DECLARATION_NODE (declaration)->type_is_being_processed = FALSE;
      TYPE_DECLARATION_NODE (declaration)->type_was_processed = TRUE;
    }
  else if (declaration_mode == ICNM_CONSTANT_DECLARATION
	   && !CONSTANT_DECLARATION_NODE (declaration)->constant_was_processed)
    {
      CONSTANT_DECLARATION_NODE (declaration)->constant_is_being_processed
	= TRUE;
      eval (CONSTANT_DECLARATION_NODE (declaration)->constant_expression);
      if (r1_constant_test_with_error_parameter
	  (ERR_no_constant_expression_in_constant_definition,
	   declaration_name))
	CONSTANT_DECLARATION_NODE (declaration)->constant_expression = r1;
      else
	CONSTANT_DECLARATION_NODE (declaration)->constant_expression = NULL;
      CONSTANT_DECLARATION_NODE (declaration)->constant_is_being_processed
	= FALSE;
      CONSTANT_DECLARATION_NODE (declaration)->constant_was_processed = TRUE;
    }
  else if (declaration_mode == ICNM_VARIABLE_DECLARATION
	   && !VARIABLE_DECLARATION_NODE (declaration)->variable_was_processed)
    {
      pred_variable_type
	= VARIABLE_DECLARATION_NODE (declaration)->variable_type;
      /* Set all variables from the variable list with given declaration
	 as being processed. */
      for (curref = declaration;
	   curref != NULL && MODE (curref) == ICNM_VARIABLE_DECLARATION
	   && (VARIABLE_DECLARATION_NODE (curref)->variable_type
	       == pred_variable_type);
	   curref = NEXT_DECLARATION_ELEMENT (curref))
	VARIABLE_DECLARATION_NODE (curref)->variable_is_being_processed = TRUE;
      eval (VARIABLE_DECLARATION_NODE (declaration)->variable_type);
      if (r1_type_test_with_error_parameter (ERR_type_in_variable_declaration,
					     declaration_name))
	result_variable_type = r1;
      else
	result_variable_type = NULL;
      for (curref = declaration;
	   curref != NULL && MODE (curref) == ICNM_VARIABLE_DECLARATION
	   && (VARIABLE_DECLARATION_NODE (curref)->variable_type
	       == pred_variable_type);
	   curref = NEXT_DECLARATION_ELEMENT (curref))
	{
	  VARIABLE_DECLARATION_NODE (curref)->variable_is_being_processed
	    = FALSE;
	  VARIABLE_DECLARATION_NODE (curref)->variable_was_processed = TRUE;
	  VARIABLE_DECLARATION_NODE (curref)->variable_type
	    = result_variable_type;
	}
    }
  current_scope = saved_current_scope;
  r1 = saved_r1;
  r2 = saved_r2;
}

/* The function returns definition of REF which is to be null pointer or
   pointer to type or constant declaration. */

static ICN_pointer
type_or_constant_definition (declaration)
     register ICN_pointer declaration;
{
  register enum icode_node_mode mode_declaration;

  if (declaration != NULL)
    {
      mode_declaration = MODE (declaration);
      if (mode_declaration == ICNM_TYPE_DECLARATION)
	declaration = TYPE_DECLARATION_NODE (declaration)->type_definition;
      else if (mode_declaration == ICNM_CONSTANT_DECLARATION)
	declaration
	  = CONSTANT_DECLARATION_NODE (declaration)->constant_expression;
    }
  return declaration;
}

/* It is the pointer to type definition of set construction (e.g.
   BITSET {0, 1, 10}) which is being processed.  The set type is BITSET if
   the pointer is NULL. */

static ICN_pointer current_set_construction_type;

/* This is main recursive function which processes type definitions and
   expressions.  The function returns pointer to finish representation of
   the original construction.  The result may be differ from CONSTR because of
   constant folding and changing identifier by designated object.  The result
   is also placed in r1.  The function saves R1 in r2.  CONSTR is to refer to
   node which represents type definition or expression. */

static ICN_pointer
eval (constr)
     register ICN_pointer constr;
{
  register ICN_pointer result_type;
  register ICN_pointer saved_r1, saved_current_set_construction_type;
  enum icode_node_mode constr_mode;
  int error_was_fixed, saved_source_line_number;
  char *error_message;

  saved_r1 = r1;
  if (constr != NULL)
    {
      saved_source_line_number = source_line_number;
      set_source_line (constr);
      constr_mode = MODE (constr);
      switch (constr_mode)
	{
	case ICNM_INTEGER:
	case ICNM_CARDINAL:
	case ICNM_REAL:
	case ICNM_STRING:
	case ICNM_BASIC_TYPE:
	  /* Undo set_source_line (constr) because this node has unique
	     copy. */
	  source_line_number = saved_source_line_number;
	  break;
	case ICNM_IDENTIFIER:
	  {
	    register ICN_pointer source_constr, declaration;
	    enum icode_node_mode declaration_mode;

	    source_constr = constr;
	    declaration = NULL;
	    constr
	      = find_identifier_declaration (current_scope, source_constr);
	    if (constr != NULL && MODE (constr) == ICNM_PROCEDURE)
	      PROCEDURE_NODE (constr)->procedure_is_used = TRUE;
	    if (constr == NULL && it_is_definition_module)
	      constr = find_denotation_in_second_space (current_scope,
							source_constr);
	    if (constr == NULL && (!it_is_definition_module || constr == NULL))
	      {
		if (find_identifier_declaration_or_its_export_import
		    (current_scope, source_constr) == NULL)
		  error_with_parameter
		    (ERR_undeclared_identifier,
		     IDENTIFIER_NODE (source_constr)->identifier_string);
	      }
	    else
	      {
		declaration_mode = MODE (constr);
		if (declaration_mode == ICNM_TYPE_DECLARATION)
		  {
		    error_was_fixed = (TYPE_DECLARATION_NODE (constr)
				       ->type_is_being_processed);
		    declaration = constr;
		  }
		else if (declaration_mode == ICNM_CONSTANT_DECLARATION)
		  {
		    error_was_fixed = (CONSTANT_DECLARATION_NODE (constr)
				       ->constant_is_being_processed);
		    declaration = constr;
		  }
		else if (declaration_mode == ICNM_VARIABLE_DECLARATION)
		  {
		    error_was_fixed = (VARIABLE_DECLARATION_NODE (constr)
				       ->variable_is_being_processed);
		    declaration = constr;
		  }
		else if (declaration_mode == ICNM_QUALIFIED_EXPORT)
		  {
		    /* It is opaque type. */
		    declaration
		      = find_identifier_declaration
		      (QUALIFIED_EXPORT_NODE (constr)->scope, source_constr);
		    if (declaration != NULL)
		      error_was_fixed = (TYPE_DECLARATION_NODE (declaration)
					 ->type_is_being_processed);
		  }
	      }
	    if (declaration != NULL)
	      {
		set_source_line (declaration);
		if (error_was_fixed)
		  error_with_parameter
		    (ERR_declaration_cycle,
		     IDENTIFIER_NODE (source_constr)->identifier_string);
		else
		  process_type_constant_variable_declaration (declaration);
	      }
	    constr = type_or_constant_definition (constr);
	    source_line_number = saved_source_line_number;
	    break;
	  }
	case ICNM_QUALIFIED_EXPORT:
	case ICNM_ENUMERATION_TYPE:
	  source_line_number = saved_source_line_number;
	  break;
	case ICNM_SET_TYPE:
	  eval (SET_TYPE_NODE (constr)->base_type);
	  SET_TYPE_NODE (constr)->base_type = NULL;
	  if (r1_type_test (ERR_set_type))
	    {
	      enum icode_node_mode base_type_mode;
	      Tcard max_value_of_base_type;

	      base_type_mode = MODE (r1);
	      if ((base_type_mode != ICNM_ENUMERATION_TYPE
		   && base_type_mode != ICNM_RANGE_TYPE)
		  || through_range_type (r1) == ICN_POINTER (&ICN_TD_char))
		error (ERR_set_base_type);
	      else
		{
		  error_was_fixed = FALSE;
		  if (it_is_integer_type (through_range_type (r1)))
		    {
		      if (INTEGER_NODE (min_or_max (r1, TRUE))->integer_value
			  < 0)
			{
			  error (ERR_set_base_type_value);
			  error_was_fixed = TRUE;
			}
		      else
			max_value_of_base_type
			  = INTEGER_NODE
			  (min_or_max (r1, FALSE))->integer_value;
		    }
		  else
		    max_value_of_base_type
		      = CARDINAL_NODE (min_or_max (r1, FALSE))->cardinal_value;
		  if (!error_was_fixed)
		    {
		      if (max_value_of_base_type >= BITS_IN_SET)
			error (ERR_set_base_type_value);
		      else
			SET_TYPE_NODE (constr)->base_type = r1;
		    }
		}
	    }
	  source_line_number = saved_source_line_number;
	  break;
	case ICNM_POINTER_TYPE:
	  {
	    register ICN_pointer base_type, object;

	    /* Pointers permits cyclic declarations.  Therefore identifier
	       after pointer will changed only on corresponding type
	       declaration (not type definition).  Function
	       complete_processing_pointer_definitions does change on the type
	       definition after processing all other type definitions. */
	    base_type = NULL;
	    if (POINTER_TYPE_NODE (constr)->base_type != NULL
		&& (MODE (POINTER_TYPE_NODE (constr)->base_type)
		    == ICNM_IDENTIFIER))
	      object
		= find_identifier_declaration
		(current_scope, POINTER_TYPE_NODE (constr)->base_type);
	    if (POINTER_TYPE_NODE (constr)->base_type != NULL
		&& (MODE (POINTER_TYPE_NODE (constr)->base_type)
		    == ICNM_IDENTIFIER)
		&& object != NULL
		&& (MODE (object) == ICNM_TYPE_DECLARATION
		    || MODE (object) == ICNM_QUALIFIED_EXPORT))
	      base_type = object;
	    else
	      {
		eval (POINTER_TYPE_NODE (constr)->base_type);
		if (r1_type_test (ERR_no_type_in_pointer_type))
		  base_type = r1;
	      }
	    POINTER_TYPE_NODE (constr)->base_type = base_type;
	    source_line_number = saved_source_line_number;
	    break;
	  }
	case ICNM_ARRAY_TYPE:
	  eval (ARRAY_TYPE_NODE (constr)->array_index_type);
	  eval (ARRAY_TYPE_NODE (constr)->base_type);
	  ARRAY_TYPE_NODE (constr)->array_index_type
	    = ARRAY_TYPE_NODE (constr)->base_type = NULL;
	  if (r2_type_test (ERR_no_type_in_index_type)
	      && r1_type_test (ERR_no_type_in_array_element))
	    {
	      if (r2 != ICN_POINTER (&ICN_TD_char)
		  && MODE (r2) != ICNM_ENUMERATION_TYPE
		  && MODE (r2) != ICNM_RANGE_TYPE)
		error (ERR_array_index_type);
	      else
		{
		  ARRAY_TYPE_NODE (constr)->array_index_type = r2;
		  ARRAY_TYPE_NODE (constr)->base_type = r1;
		}
	    }
	  source_line_number = saved_source_line_number;
	  break;
	case ICNM_RANGE_TYPE:
	  process_range_type (constr);
	  source_line_number = saved_source_line_number;
	  break;
	case ICNM_PROCEDURE_TYPE:
	  {
	    register ICN_pointer parameter;

	    if (PROCEDURE_TYPE_NODE (constr)->procedure_result_type != NULL)
	      {
		eval (PROCEDURE_TYPE_NODE (constr)->procedure_result_type);
		PROCEDURE_TYPE_NODE (constr)->procedure_result_type = NULL;
		if (r1_type_test (ERR_no_type_in_procedure_result_type))
		  PROCEDURE_TYPE_NODE (constr)->procedure_result_type = r1;
	      }
	    for (parameter = next_parameter_type (constr); parameter != NULL;
		 parameter = next_parameter_type (parameter))
	      {
		set_source_line (parameter);
		eval (FORMAL_PARAMETER_TYPE (parameter));
		FORMAL_PARAMETER_TYPE (parameter) = NULL;
		if (r1_type_test (ERR_no_type_in_formal_parameter_type))
		  FORMAL_PARAMETER_TYPE (parameter) = r1;
	      }
	    source_line_number = saved_source_line_number;
	    break;
	  }
	case ICNM_RECORD:
	  process_record_element_list
	    (RECORD_NODE (constr)->record_element_list);
	  source_line_number = saved_source_line_number;
	  break;
	case ICNM_EQUAL:
	case ICNM_UNEQUAL:
	case ICNM_LESS_THAN:
	case ICNM_GREATER_THAN:
	case ICNM_LESS_OR_EQUAL:
	case ICNM_GREATER_OR_EQUAL:
	  constr = process_relation_operation (constr_mode, constr);
	  break;
	case ICNM_IN:
	  {
	    register ICN_pointer base_type;
	    LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	    RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	    VALUE_TYPE (constr) = ICN_POINTER (&ICN_TD_boolean);
	    if (r2_expression_test (ERR_in_operands)
		&& r1_expression_test (ERR_in_operands))
	      {
		if (MODE (sinf1.type) == ICNM_SET_TYPE)
		  base_type = SET_TYPE_NODE (sinf1.type)->base_type;
		if (MODE (sinf1.type) != ICNM_SET_TYPE
		    ||
		    result_of_compatible_types (sinf2.type, base_type) == NULL)
		  error (ERR_in_operands);
		else if (sinf1.it_is_constant && sinf2.it_is_constant)
		  {
		    int left_value_in_set_element_range;
		    Tcard left_value;

		    if (MODE (r2) == ICNM_INTEGER)
		      {
			left_value_in_set_element_range
			  = (INTEGER_NODE (r2)->integer_value
			     >= (INTEGER_NODE (min_or_max (base_type, TRUE))
				 ->integer_value)
			     || INTEGER_NODE (r2)->integer_value
			     <= (INTEGER_NODE (min_or_max (base_type, FALSE))
				 ->integer_value));
			if (left_value_in_set_element_range)
			  left_value = INTEGER_NODE (r2)->integer_value;
		      }
		    else
		      {
			left_value = cardinal_value (r2);
			left_value_in_set_element_range
			  = (left_value
			     >= (CARDINAL_NODE (min_or_max (base_type, TRUE))
				 ->cardinal_value)
			     ||
			     left_value
			     <= (CARDINAL_NODE (min_or_max (base_type, TRUE))
				 ->cardinal_value));
		      }
		    constr = (left_value_in_set_element_range
			      && SET_NODE (r1)->set_value >> left_value & 01
			      ? ICN_POINTER (&ICN_V_true)
			      : ICN_POINTER (&ICN_V_false));
		  }
	      }
	    break;
	  }
	case ICNM_IDENTITY:
	  OPERAND (constr) = eval (OPERAND (constr));
	  if (r1_expression_test (ERR_identity_operand))
	    {
	      result_type = through_range_type (sinf1.type);
	      if (!it_is_scalar_type (result_type)
		  || it_is_character_type (result_type)
		  || MODE (result_type) == ICNM_ENUMERATION_TYPE)
		error (ERR_identity_operand);
	      else if (sinf1.it_is_constant)
		constr = r1;
	      else
		VALUE_TYPE (constr) = result_type;
	    }
	  break;
	case ICNM_SIGN_CONVERSION:
	  OPERAND (constr) = eval (OPERAND (constr));
	  if (r1_expression_test (ERR_sign_inversion_operand))
	    {
	      result_type = through_range_type (sinf1.type);
	      if (!it_is_real_type (result_type)
		  && !it_is_integer_type (result_type)
		  && !it_is_integer_cardinal_type (result_type))
		error (ERR_sign_inversion_operand);
	      else if (sinf1.it_is_constant)
		{
		  if (it_is_real_type (result_type))
		    {
		      constr = create_node (ICNM_REAL);
		      REAL_NODE (constr)->value_type
			= ICN_POINTER (&ICN_TD_real);
		      REAL_NODE (constr)->real_value
			= (-REAL_NODE (r1)->real_value);
		    }
		  else
		    {
		      constr = create_node (ICNM_INTEGER);
		      INTEGER_NODE (constr)->value_type
			= ICN_POINTER (&ICN_TD_integer);
		      INTEGER_NODE (constr)->integer_value
			= (it_is_integer_type (result_type)
			   ? -INTEGER_NODE (r1)->integer_value
			   : -(Tint) CARDINAL_NODE (r1)->cardinal_value);
		    }
		  constr = do_accurate_type_value (constr);
		}
	      else
		VALUE_TYPE (constr) = result_type;
	    }
	  break;
	case ICNM_OR:
	case ICNM_AND:
	  LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	  RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	  VALUE_TYPE (constr) = ICN_POINTER (&ICN_TD_boolean);
	  error_message = (constr_mode == ICNM_OR
			   ? ERR_or_operands : ERR_and_operands);
	  if (r2_expression_test (error_message)
	      && r1_expression_test (error_message))
	    {
	      if (through_range_type (sinf2.type)
		  != ICN_POINTER (&ICN_TD_boolean)
		  ||
		  through_range_type (sinf1.type)
		  != ICN_POINTER (&ICN_TD_boolean))
		error (error_message);
	      else if (sinf2.it_is_constant && sinf1.it_is_constant)
		{
		  constr = create_node (ICNM_CARDINAL);
		  CARDINAL_NODE (constr)->value_type
		    = ICN_POINTER (&ICN_TD_boolean);
		  CARDINAL_NODE (constr)->cardinal_value
		    = (constr_mode == ICNM_AND
		       ? (CARDINAL_NODE (r2)->cardinal_value
			  && CARDINAL_NODE (r1)->cardinal_value)
		       : (CARDINAL_NODE (r2)->cardinal_value
			  || CARDINAL_NODE (r1)->cardinal_value));
		  constr = find_in_or_else_include_to_table (constr);
		}
	    }
	  break;
	case ICNM_NOT:
	  OPERAND (constr) = eval (OPERAND (constr));
	  VALUE_TYPE (constr) = ICN_POINTER (&ICN_TD_boolean);
	  if (r1_expression_test (ERR_not_operand))
	    {
	      if (through_range_type (sinf1.type)
		  != ICN_POINTER (&ICN_TD_boolean))
		error (ERR_not_operand);
	      else if (sinf1.it_is_constant)
		{
		  constr = create_node (ICNM_CARDINAL);
		  CARDINAL_NODE (constr)->value_type
		    = ICN_POINTER (&ICN_TD_boolean);
		  CARDINAL_NODE (constr)->cardinal_value
		    = !CARDINAL_NODE (r1)->cardinal_value;
		  constr = find_in_or_else_include_to_table (constr);
		}
	    }
	  break;
	case ICNM_REAL_DIVISION:
	  LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	  RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	  if (r2_expression_test (ERR_real_division_operands)
	      && r1_expression_test (ERR_real_division_operands))
	    {
	      result_type
		= result_of_compatible_types (sinf2.type, sinf1.type);
	      if (result_type == NULL
		  || (!it_is_real_type (sinf2.type)
		      && MODE (sinf2.type) != ICNM_SET_TYPE))
		error (ERR_real_division_operands);
	      else if (sinf2.it_is_constant && sinf1.it_is_constant)
		{
		  if (MODE (r1) != ICNM_REAL)
		    {
		      constr = create_node (ICNM_SET);
		      SET_NODE (constr)->set_value
			= (SET_NODE (r2)->set_value
			   ^ SET_NODE (r1)->set_value);
		    }
		  else
		    {
		      constr = create_node (ICNM_REAL);
		      REAL_NODE (constr)->real_value
			= (REAL_NODE (r2)->real_value
			   / REAL_NODE (r1)->real_value);
		    }
		  VALUE_TYPE (constr) = result_type;
		  constr = do_accurate_type_value (constr);
		}
	      else
		VALUE_TYPE (constr) = result_type;
	    }
	  break;
	case ICNM_ADDITION:
	case ICNM_SUBTRACTION:
	case ICNM_MULT:
	  constr = process_generic_arithmetic_operation (constr_mode, constr);
	  break;
	case ICNM_DIV:
	case ICNM_MOD:
	  LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	  RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	  error_message
	    = (constr_mode == ICNM_DIV ? ERR_div_operands : ERR_mod_operands);
	  if (r2_expression_test (error_message)
	      && r1_expression_test (error_message))
	    {
	      result_type
		= result_of_compatible_types (sinf2.type, sinf1.type);
	      if (result_type == NULL
		  || (!it_is_integer_type (result_type)
		      && !it_is_cardinal_type (result_type)
		      && !it_is_integer_cardinal_type (result_type)))
		error (error_message);
	      else if (sinf2.it_is_constant && sinf1.it_is_constant)
		{
		  if (it_is_integer_type (result_type)
		      && it_is_integer_cardinal_type (sinf2.type))
		    r2 = integer_value_from_integer_cardinal_value (r2);
		  if (it_is_integer_type (result_type)
		      && it_is_integer_cardinal_type (sinf1.type))
		    r1 = integer_value_from_integer_cardinal_value (r1);
		  if (it_is_integer_type (result_type))
		    {
		      constr = create_node (ICNM_INTEGER);
		      if (constr_mode == ICNM_MOD)
			INTEGER_NODE (constr)->integer_value
			  = (INTEGER_NODE (r2)->integer_value
			     % INTEGER_NODE (r1)->integer_value);
		      else
			INTEGER_NODE (constr)->integer_value
			  = (INTEGER_NODE (r2)->integer_value
			     / INTEGER_NODE (r1)->integer_value);
		    }
		  else
		    {
		      constr = create_node (ICNM_CARDINAL);
		      if (constr_mode == ICNM_MOD)
			CARDINAL_NODE (constr)->cardinal_value
			  = (CARDINAL_NODE (r2)->cardinal_value
			     % CARDINAL_NODE (r1)->cardinal_value);
		      else
			CARDINAL_NODE (constr)->cardinal_value
			  = (CARDINAL_NODE (r2)->cardinal_value
			     / CARDINAL_NODE (r1)->cardinal_value);
		    }
		  VALUE_TYPE (constr) = result_type;
		  constr = do_accurate_type_value (constr);
		}
	      else
		VALUE_TYPE (constr) = result_type;
	    }
	  break;
	case ICNM_DEREFERENCING:
	  OPERAND (constr) = eval (OPERAND (constr));
	  if (r1_variable_test (ERR_dereferencing_operands))
	    {
	      if (MODE (sinf1.type) != ICNM_POINTER_TYPE)
		error (ERR_dereferencing_operands);
	      else
		VALUE_TYPE (constr)
		  = POINTER_TYPE_NODE (sinf1.type)->base_type;
	    }
	  break;
	case ICNM_QUALIFICATION:
	  {
	    register ICN_pointer qualified_object;

	    LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	    if (r1 != NULL && MODE (r1) == ICNM_MODULE)
	      {
		/* It may be exported object. */

		qualified_object
		  = find_denotation_in_second_space (r1,
						     RIGHT_OPERAND (constr));
		if (qualified_object == NULL)
		  error_with_parameter
		    (ERR_unexported_identifier,
		     (IDENTIFIER_NODE (RIGHT_OPERAND (constr))
		      ->identifier_string));
		else if (MODE (qualified_object) == ICNM_QUALIFIED_EXPORT
			 &&
			 QUALIFIED_EXPORT_NODE (qualified_object)->opaque_type)
		  constr = qualified_object;
		else
		  {
		    constr
		      = type_or_constant_definition
		      (find_identifier_declaration (r1,
						    RIGHT_OPERAND (constr)));
		    if (constr != NULL && MODE (constr) == ICNM_PROCEDURE)
		      PROCEDURE_NODE (constr)->procedure_is_used = TRUE;
		  }
	      }
	    else if (r1_variable_test (ERR_record_variable))
	      {
		/* It may be field. */
		if (MODE (sinf1.type) != ICNM_RECORD)
		  error (ERR_record_variable);
		else
		  {
		    qualified_object
		      = find_denotation (sinf1.type, RIGHT_OPERAND (constr));
		    if (qualified_object == NULL)
		      error_with_parameter
			(ERR_undeclared_field,
			 (IDENTIFIER_NODE (RIGHT_OPERAND (constr))
			  ->identifier_string));
		    else
		      {
			VALUE_TYPE (constr)
			  = FIELD_NODE (qualified_object)->field_type;
			RIGHT_OPERAND (constr) = qualified_object;
		      }
		  }
	      }
	    break;
	  }
	case ICNM_INDEXING:
	  LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	  RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	  subgraph_is_expression (r2, &sinf2);
	  if (sinf2.it_is_array_parameter || r2_variable_test (ERR_vnoarr))
	    {
	      if (!sinf2.it_is_array_parameter
		  && MODE (sinf2.type) != ICNM_ARRAY_TYPE)
		error (ERR_vnoarr);
	      else if (r1_expression_test (ERR_index_expression))
		{
		  if (sinf2.it_is_array_parameter)
		    {
		      VALUE_TYPE (constr) = sinf2.type;
		      sinf2.type = ICN_POINTER (&ICN_TD_cardinal);
		    }
		  else
		    {
		      VALUE_TYPE (constr)
			= ARRAY_TYPE_NODE (sinf2.type)->base_type;
		      sinf2.type
			= ARRAY_TYPE_NODE (sinf2.type)->array_index_type;
		    }
		  if (!types_are_assignment_compatible (sinf2.type,
							sinf1.type))
		    error (ERR_index_expression_type);
		  else if (sinf1.it_is_constant
			   && !test_discrete_inclusion (r1, sinf2.type))
		    error (ERR_index_value);
		}
	    }
	  break;
	case ICNM_SET_VALUE_START:
	  eval (LEFT_OPERAND (constr));
	  /* It is needed because set constructions may be nested (e.g.
	     {1, cardinal_number (color_set {blue, green})} .*/
	  saved_current_set_construction_type = current_set_construction_type;
	  current_set_construction_type = LEFT_OPERAND (constr) = NULL;
	  if (r1_type_test (ERR_set_constructor_base_type))
	    {
	      if (MODE (r1) != ICNM_SET_TYPE)
		error (ERR_set_constructor_base_type);
	      else
		VALUE_TYPE (constr) = current_set_construction_type = r1;
	    }
	  if (RIGHT_OPERAND (constr) == NULL)
	    {
	      if (current_set_construction_type != NULL)
		{
		  constr = create_node (ICNM_SET);
		  SET_NODE (constr)->value_type
		    = current_set_construction_type;
		  SET_NODE (constr)->set_value = 0;
		  constr = find_in_or_else_include_to_table (constr);
		}
	    }
	  else
	    {
	      RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	      if (r2 != NULL && MODE (r2) == ICNM_SET_TYPE)
		constr = r1;
	    }
	  current_set_construction_type = saved_current_set_construction_type;
	  break;
	case ICNM_ELEMENT_IN_SET_VALUE:
	case ICNM_RANGE_IN_SET_VALUE:
	  if (constr_mode == ICNM_ELEMENT_IN_SET_VALUE)
	    {
	      OPERAND (constr) = eval (OPERAND (constr));
	      /* Do range for further processing. */
	      r2 = r1;
	    }
	  else
	    {
	      LEFT_OPERAND (constr) = eval (LEFT_OPERAND (constr));
	      RIGHT_OPERAND (constr) = eval (RIGHT_OPERAND (constr));
	    }
	  VALUE_TYPE (constr) = current_set_construction_type;
	  if (r2_expression_test (ERR_set_element_expression)
	      && r1_expression_test (ERR_set_element_expression))
	    {
	      register ICN_pointer base_set_type;

	      result_type
		= result_of_compatible_types (sinf2.type, sinf1.type);
	      error_was_fixed = (result_type == NULL
				 || !it_is_discrete_type (result_type));
	      if (current_set_construction_type == NULL)
		base_set_type = current_set_construction_type;
	      else
		base_set_type
		  = SET_TYPE_NODE (current_set_construction_type)->base_type;
	      if (!error_was_fixed)
		result_type
		  = result_of_compatible_types (result_type, base_set_type);
	      if (error_was_fixed || result_type == NULL)
		error (ERR_set_element_expression_type);
	      else if ((sinf2.it_is_constant
			&& !test_discrete_inclusion (r2, base_set_type))
		       || (sinf1.it_is_constant
			   && !test_discrete_inclusion (r1, base_set_type)))
		error (ERR_set_element_value);
	      else if (sinf2.it_is_constant && sinf1.it_is_constant)
		{
		  Tcard right_value, left_value;

		  if (MODE (r2) == ICNM_INTEGER)
		    left_value = (Tcard) INTEGER_NODE (r2)->integer_value;
		  else
		    left_value = cardinal_value (r2);
		  if (MODE (r1) == ICNM_INTEGER)
		    right_value = (Tcard) INTEGER_NODE (r1)->integer_value;
		  else
		    right_value = cardinal_value (r1);
		  constr = create_node (ICNM_SET);
		  SET_NODE (constr)->value_type
		    = current_set_construction_type;
		  SET_NODE (constr)->set_value
		    = (~(Tset) 0 << left_value
		       &
		       ~(Tset) 0 >> (BITS_IN_SET - (unsigned) right_value - 1));
		  constr = find_in_or_else_include_to_table (constr);
		}
	    }
	  break;
	case ICNM_FUNCTION_CALL:
	  constr = process_call (constr);
	  break;
	default:
	  abort ();
	}
    }
  r2 = saved_r1;
  return r1 = constr;
}



/* This page contains functions for processing export (nonqualified)
   and import clauses. */

/* The following recursive function processes export or import node
   EXPORT_IMPORT and creates new export or import nodes which represents
   implicit export or import initiated by EXPORT_IMPORT and originally by
   ORIGINAL_EXPORT_IMPORT.  EXPORT_IMPORT and ORIGINAL_EXPORT_IMPORT are to
   refer to nonqualified export or import node. */

static void
create_implicit_export_or_import_nodes (export_import, original_export_import)
     ICN_pointer export_import, original_export_import;
{
  register ICN_pointer object, new_node, final_node;

  object = find_identifier_declaration (SCOPE (export_import),
					IDENTIFIER (export_import));
  if (object == NULL)
    return;
  else if (MODE (object) == ICNM_TYPE_DECLARATION)
    object = TYPE_DECLARATION_NODE (object)->type_definition;
  if (object != NULL
      && (MODE (object) == ICNM_ENUMERATION_TYPE
	  || MODE (object) == ICNM_MODULE))
    {
      if (MODE (object) == ICNM_MODULE)
	object = EXPORT_LIST (object);
      else
	object = next_enumeration_constant (object);
      if (object == NULL)
	return;
      do
	{
	  new_node = create_node (MODE (export_import));
	  IDENTIFIER (new_node) = IDENTIFIER (object);
	  SCOPE (new_node) = SCOPE (original_export_import);
	  MODULE_EXPORTER (new_node)
	    = MODULE_EXPORTER (original_export_import);
	  final_node = find_in_or_else_include_to_table (new_node);
	  if (final_node == new_node)
	    create_implicit_export_or_import_nodes (new_node,
						    original_export_import);
	  else if (MODE (final_node) != MODE (new_node)
		   || (MODULE_EXPORTER (final_node)
		       != MODULE_EXPORTER (new_node)))
	    error_with_parameter
	      ((MODE (new_node) == ICNM_EXPORT
		? ERR_implicitly_exported_identifier_declaration
		: ERR_implicitly_imported_identifier_declaration),
	       IDENTIFIER_NODE (IDENTIFIER (new_node))->identifier_string);
	  if (MODE (object) == ICNM_EXPORT)
	    object = EXPORT_LIST (object);
	  else
	    object = next_enumeration_constant (object);
	}
      while (object != NULL);
    }
}

/* The function processes all nonqualified export nodes of the declaration
   list starting with DECLARATION.  DECLARATION is to refer to any declaration
   node. Processing is executed in reverse order, i.e. from more nested to less
   nested export clauses. */

void
process_export (declaration)
     register ICN_pointer declaration;
{
  register ICN_pointer pred_declaration, next_declaration;

  /* Reverse declaration list for processing export clauses from
     more nested to less nested export clauses. */
  for (pred_declaration = NULL; declaration != NULL;
       pred_declaration = declaration, declaration = next_declaration)
    {
      next_declaration = NEXT_DECLARATION_ELEMENT (declaration);
      NEXT_DECLARATION_ELEMENT (declaration) = pred_declaration;
    }

  /* Process export clauses and restore original order of declaration list. */
  for (declaration = pred_declaration, pred_declaration = NULL;
       declaration != NULL;
       pred_declaration = declaration, declaration = next_declaration)
    {
      next_declaration = NEXT_DECLARATION_ELEMENT (declaration);
      NEXT_DECLARATION_ELEMENT (declaration) = pred_declaration;
      if (MODE (declaration) == ICNM_EXPORT)
	{
	  set_source_line (declaration);
	  if (find_denotation (EXPORT_NODE (declaration)->module_exporter,
			       IDENTIFIER (declaration)) != NULL)
	    create_implicit_export_or_import_nodes (declaration, declaration);
	  else
	    error_with_parameter
	      (ERR_undeclared_identifier_export,
	       IDENTIFIER_NODE (IDENTIFIER (declaration))->identifier_string);
	}
    }
}

/* The function processes all import nodes of the declaration list starting
   with DECLARATION.  DECLARATION is to refer to any declaration node.
   Processing is executed in normal order, i.e. from less nested to more
   nested export clauses.  All export nodes are to be processed. */

void
process_import (declaration)
     register ICN_pointer declaration;
{
  register ICN_pointer identifier_after_current_from_clause, module_exporter;

  for (identifier_after_current_from_clause = NULL; declaration != NULL;
       declaration = NEXT_DECLARATION_ELEMENT (declaration))
    if (MODE (declaration) == ICNM_IMPORT)
      {
	set_source_line (declaration);
	if (IMPORT_NODE (declaration)->module_exporter == NULL)
	  {
	    /* Nonqualified import (without from clause). */
	    if (find_identifier_declaration_or_its_export_import
		(SCOPE (SCOPE (declaration)), IDENTIFIER (declaration))
		!= NULL)
	      create_implicit_export_or_import_nodes
		(declaration, declaration);
	    else
	      error_with_parameter
		(ERR_undeclared_identifier_import,
		 IDENTIFIER_NODE (IDENTIFIER (declaration))
		 ->identifier_string);
	  }
	else
	  {
	    /* Qualified import (with from clause). */
	    module_exporter
	      = find_identifier_declaration_or_its_export_import
	      (SCOPE (SCOPE (declaration)),
	       IMPORT_NODE (declaration)->module_exporter);
	    if (module_exporter
		!= NULL && MODE (module_exporter) == ICNM_MODULE)
	      {
		if (module_exporter != ICN_POINTER (&ICN_D_cproc))
		  {
		    if (find_denotation_in_second_space
			(module_exporter, IDENTIFIER (declaration)) != NULL)
		      create_implicit_export_or_import_nodes
			(declaration, declaration);
		    else
		      error_with_parameter
			(ERR_unexported_identifier_import,
			 IDENTIFIER_NODE (IDENTIFIER (declaration))
			 ->identifier_string);
		  }
	      }
	    else if (identifier_after_current_from_clause
		     != IMPORT_NODE (declaration)->module_exporter)
	      {
		identifier_after_current_from_clause
		  = IMPORT_NODE (declaration)->module_exporter;
		error_with_parameter
		  ((module_exporter == NULL
		    ? ERR_import_from_undeclared_module
		    : ERR_no_module_after_from),
		   IDENTIFIER_NODE (identifier_after_current_from_clause)
		   ->identifier_string);
	      }
	  }
      }
}



/* This page contains function for processing statements. */

/* TRUE if the statement being processed is in loop-statement. */

static int it_is_in_loop;


/* The following recursive function processes statements of the list starting
   with STATEMENT and all nested statements.  STATEMENT is to refer to any
   statement node (or NULL). */

void
process_statements (statement)
     register ICN_pointer statement;
{
  register ICN_pointer saved_current_scope;
  int error_was_fixed, saved_it_is_in_loop;

  for (; statement != NULL; statement = NEXT_STATEMENT (statement))
    {
      set_source_line (statement);
      switch (MODE (statement))
	{
	case ICNM_C_CODE_IN_STATEMENTS:
	  break;
	case ICNM_BLOCK_BEGIN:
	  it_is_in_loop = FALSE;
	  current_scope = BLOCK_BEGIN_NODE (statement)->block_scope;
	  break;
	case ICNM_RETURN_WITHOUT_RESULT:
	  {
	    register ICN_pointer procedure_denotation;

	    procedure_denotation = procedure_over_denotation (current_scope);
	    if (MODE (procedure_denotation) == ICNM_PROCEDURE
		&& PROCEDURE_NODE (procedure_denotation)->procedure_type != NULL
		&&
		(PROCEDURE_TYPE_NODE
		 (PROCEDURE_NODE (procedure_denotation)->procedure_type)
		 ->procedure_result_type)
		!= NULL)
	      error (ERR_return_without_expression);
	    break;
	  }
	case ICNM_RETURN_WITH_RESULT:
	  {
	    register ICN_pointer procedure_denotation;

	    RETURN_WITH_RESULT_NODE (statement)->returned_expression
	      = eval
	      (RETURN_WITH_RESULT_NODE (statement)->returned_expression);
	    procedure_denotation = procedure_over_denotation (current_scope);
	    error_was_fixed = MODE (procedure_denotation) != ICNM_PROCEDURE;
	    if (!error_was_fixed)
	      {
		procedure_denotation
		  = PROCEDURE_NODE (procedure_denotation)->procedure_type;
		if (procedure_denotation != NULL)
		  procedure_denotation
		    = (PROCEDURE_TYPE_NODE (procedure_denotation)
		       ->procedure_result_type);
		error_was_fixed = procedure_denotation == NULL;
	      }
	    if (error_was_fixed)
	      error (ERR_return_with_expression);
	    else if (r1_expression_test (ERR_expression_after_return))
	      {
		if (!types_are_assignment_compatible (procedure_denotation,
						      sinf1.type))
		  error (ERR_expression_type_after_return);
		else if (sinf1.it_is_constant
			 && !test_discrete_inclusion (r1, procedure_denotation))
		  error (ERR_expression_value_after_return);
	      }
	    break;
	  }
	case ICNM_LOOP_STATEMENT:
	  saved_it_is_in_loop = it_is_in_loop;
	  it_is_in_loop = TRUE;
	  process_statements
	    (LOOP_STATEMENT_NODE (statement)->loop_statements);
	  it_is_in_loop = saved_it_is_in_loop;
	  break;
	case ICNM_EXIT_STATEMENT:
	  if (!it_is_in_loop)
	    error (ERR_exit_statement);
	  break;
	case ICNM_WHILE_STATEMENT:
	  WHILE_STATEMENT_NODE (statement)->while_expression
	    = eval (WHILE_STATEMENT_NODE (statement)->while_expression);
	  if (r1_expression_test (ERR_expression_after_while)
	      && sinf1.type != ICN_POINTER (&ICN_TD_boolean))
	    error (ERR_expression_after_while);
	  process_statements
	    (WHILE_STATEMENT_NODE (statement)->while_statements);
	  break;
	case ICNM_REPEAT_STATEMENT:
	  process_statements
	    (REPEAT_STATEMENT_NODE (statement)->repeat_statements);
	  REPEAT_STATEMENT_NODE (statement)->until_expression
	    = eval (REPEAT_STATEMENT_NODE (statement)->until_expression);
	  if (r1_expression_test (ERR_expression_after_until)
	      && sinf1.type != ICN_POINTER (&ICN_TD_boolean))
	    error (ERR_expression_after_until);
	  break;
	case ICNM_IF_STATEMENT:
	  {
	    register ICN_pointer if_cont;

	    IF_STATEMENT_NODE (statement)->if_expression
	      = eval (IF_STATEMENT_NODE (statement)->if_expression);
	    if (r1_expression_test (ERR_expression_after_if)
		&& sinf1.type != ICN_POINTER (&ICN_TD_boolean))
	      error (ERR_expression_after_if);
	    process_statements (IF_STATEMENT_NODE (statement)->if_statements);
	    /* Process elsif parts. */
	    for (if_cont = IF_STATEMENT_NODE (statement)->if_continuation;
		 if_cont != NULL && MODE (if_cont) == ICNM_ELSIF_CLAUSE;
		 if_cont = ELSIF_CLAUSE_NODE (if_cont)->elsif_continuation)
	      {
		set_source_line (if_cont);
		ELSIF_CLAUSE_NODE (if_cont)->elsif_expression
		  = eval (ELSIF_CLAUSE_NODE (if_cont)->elsif_expression);
		if (r1_expression_test (ERR_expression_after_elsif)
		    && sinf1.type != ICN_POINTER (&ICN_TD_boolean))
		  error (ERR_expression_after_elsif);
		process_statements
		  (ELSIF_CLAUSE_NODE (if_cont)->elsif_statements);
	      }
	    /* Process else part (if it exists). */
	    process_statements (if_cont);
	    break;
	  }
	case ICNM_FOR_STATEMENT:
	  {
	    register ICN_pointer loop_variable_type;
	    register ICN_pointer loop_control_variable;
	    register ICN_pointer iteration_value_type;

	    error_was_fixed
	      = (MODE (FOR_STATEMENT_NODE (statement)->for_control_variable)
		 != ICNM_IDENTIFIER);
	    if (!error_was_fixed)
	      loop_control_variable
		= find_identifier_declaration_or_its_export_import
		(current_scope,
		 FOR_STATEMENT_NODE (statement)->for_control_variable);
	    if (error_was_fixed || loop_control_variable == NULL
		|| MODE (loop_control_variable) != ICNM_VARIABLE_DECLARATION)
	      {
		error (ERR_loop_control_variable);
		loop_variable_type = NULL;
	      }
	    else
	      {
		loop_variable_type
		  = (VARIABLE_DECLARATION_NODE (loop_control_variable)
		     ->variable_type);
		FOR_STATEMENT_NODE (statement)->for_control_variable
		  = loop_control_variable;
	      }
	    FOR_STATEMENT_NODE (statement)->for_starting_value
	      = eval (FOR_STATEMENT_NODE (statement)->for_starting_value);
	    FOR_STATEMENT_NODE (statement)->for_limit
	      = eval (FOR_STATEMENT_NODE (statement)->for_limit);
	    if (r1_expression_test (ERR_start_value_or_limit_is_not_expression)
		&& (r2_expression_test
		    (ERR_start_value_or_limit_is_not_expression)))
	      {
		iteration_value_type
		  = result_of_compatible_types (sinf1.type, sinf2.type);
		if (iteration_value_type == NULL
		    || !it_is_discrete_type (through_range_type (sinf2.type)))
		  error (ERR_types_of_loop_bounds);
		else if (result_of_compatible_types (loop_variable_type,
						     iteration_value_type)
			 == NULL)
		  error (ERR_types_of_control_variable_and_bounds);
		else if (sinf2.it_is_constant
			 && !test_discrete_inclusion (r2, loop_variable_type))
		  error (ERR_loop_start_value);
	      }
	    if (FOR_STATEMENT_NODE (statement)->for_increment != NULL)
	      {
		FOR_STATEMENT_NODE (statement)->for_increment
		  = eval (FOR_STATEMENT_NODE (statement)->for_increment);
		if (r1_constant_test (ERR_nonconstant_loop_increment)
		    && !it_is_integer_type (sinf1.type)
		    && !it_is_integer_cardinal_type (sinf1.type)
		    && !it_is_cardinal_type (sinf1.type))
		  error (ERR_loop_increment_type);
	      }
	    process_statements
	      (FOR_STATEMENT_NODE (statement)->for_statements);
	    break;
	  }
	case ICNM_WITH_STATEMENT:
	  {
	    register ICN_pointer upper_scope;
	    register ICN_pointer temp;

	    WITH_STATEMENT_NODE (statement)->with_designator
	      = eval (WITH_STATEMENT_NODE (statement)->with_designator);
	    error_was_fixed = TRUE;
	    if (r1_variable_test (ERR_variable_after_with))
	      {
		if (MODE (sinf1.type) != ICNM_RECORD)
		  error (ERR_variable_after_with);
		else
		  {
		    /* Add record to the end of scope list. */
		    saved_current_scope = RECORD_NODE (sinf1.type)->scope;
		    RECORD_NODE (sinf1.type)->scope = current_scope;
		    current_scope = sinf1.type;
		    /* Test that the record is not in the scope list. */
		    for (upper_scope = current_scope;
			 upper_scope != NULL
			 && SCOPE (upper_scope) != current_scope;)
		      upper_scope = SCOPE (upper_scope);
		    if (upper_scope != NULL)
		      /* The record is not in the scope list (i.e.
			 the single with-statement with given record). */
		      SCOPE (upper_scope) = saved_current_scope;
		    error_was_fixed = FALSE;
		  }
	      }
	    process_statements
	      (WITH_STATEMENT_NODE (statement)->with_statements);
	    if (!error_was_fixed)
	      {
		/* Restore original scope list. */
		temp = current_scope;
		if (upper_scope == NULL
		    ||
		    RECORD_NODE (current_scope)->scope != saved_current_scope)
		  current_scope = RECORD_NODE (current_scope)->scope;
		RECORD_NODE (temp)->scope = saved_current_scope;
		if (upper_scope != NULL)
		  SCOPE (upper_scope) = SCOPE (temp);
	      }
	    break;
	  }
	case ICNM_CASE_STATEMENT:
	  {
	    register ICN_pointer variant;

	    CASE_STATEMENT_NODE (statement)->case_expression
	      = eval (CASE_STATEMENT_NODE (statement)->case_expression);
	    if (r1_expression_test (ERR_expression_after_case)
		&& !it_is_discrete_type (through_range_type (sinf1.type)))
	      {
		sinf1.type = NULL;
		error (ERR_expression_after_case);
	      }
	    process_case_labels (sinf1.type, statement);
	    for (variant = (CASE_STATEMENT_NODE (statement)
			    ->case_statement_variant_list);
		 MODE (variant) == ICNM_CASE_STATEMENT_VARIANT;
		 variant = (CASE_STATEMENT_VARIANT_NODE (variant)
			    ->next_case_variant))
	      {
		set_source_line (variant);
		process_statements (CASE_STATEMENT_VARIANT_NODE (variant)
				    ->case_variant_elements);
	      }
	    break;
	  }
	case ICNM_ASSIGNMENT:
	  ASSIGNMENT_NODE (statement)->assignment_variable
	    = eval (ASSIGNMENT_NODE (statement)->assignment_variable);
	  ASSIGNMENT_NODE (statement)->assignment_expression
	    = eval (ASSIGNMENT_NODE (statement)->assignment_expression);
	  if (r1 != NULL && MODE (r1) == ICNM_PROCEDURE
	      && it_is_local_or_standard_procedure (r1))
	    error (ERR_local_procedure_assignment);
	  else if (r2_variable_test (ERR_assignment_variable)
		   && r1_expression_test (ERR_assignment_expression))
	    {
	      if (!types_are_assignment_compatible (sinf2.type, sinf1.type))
		/* Test of string constant assignment. */
		{
		  if (MODE (sinf2.type) != ICNM_ARRAY_TYPE
		      || (ARRAY_TYPE_NODE (sinf2.type)->base_type
			  != ICN_POINTER (&ICN_TD_char))
		      || !it_is_string_type (sinf1.type)
		      ||
		      (array_index_span
		       (ARRAY_TYPE_NODE (sinf2.type)->array_index_type) + 1
		       < (unsigned) string_length_from_its_type (sinf1.type)))
		    error (ERR_types_in_assignment);
		}
	      else if (MODE (sinf2.type) == ICNM_PROCEDURE_TYPE
		       && sinf1.it_is_constant
		       && it_is_local_or_standard_procedure (r1))
		error (ERR_local_procedure_assignment);
	      else if (sinf1.it_is_constant
		       && !test_discrete_inclusion (r1, sinf2.type))
		error (ERR_assignment_expression_value);
	    }
	  break;
	case ICNM_PROCEDURE_CALL:
	  process_call (statement);
	  break;
	default:
	  abort ();
	}
    }
}



/* This page contains functions for processing declarations. */


/* The recursive function processes compilation unit declarations of the list
   starting with START.  START is to refer to any declaration element
   node (or NULL).  Export (nonqualified) and import clauses is already to be
   processed. */

void
process_declarations (start)
     ICN_pointer start;
{
  char *declaration_name;
  register enum icode_node_mode declaration_element_mode;
  int it_is_from_definition_module;
  register ICN_pointer declaration_element;

  /* Process all type, constant and variable declartions. */
  for (declaration_element = start; declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    {
      declaration_element_mode = MODE (declaration_element);
      if (declaration_element_mode == ICNM_TYPE_DECLARATION
	  || declaration_element_mode == ICNM_CONSTANT_DECLARATION
	  || declaration_element_mode == ICNM_VARIABLE_DECLARATION)
	process_type_constant_variable_declaration (declaration_element);
    }

  for (declaration_element = start; declaration_element != NULL;
       declaration_element = NEXT_DECLARATION_ELEMENT (declaration_element))
    {
      set_source_line (declaration_element);
      declaration_element_mode = MODE (declaration_element);
      if (declaration_element_mode != ICNM_C_CODE_IN_DECLARATIONS)
	declaration_name
	  = IDENTIFIER_NODE
	  (IDENTIFIER (declaration_element))->identifier_string;
      if (declaration_element_mode == ICNM_QUALIFIED_EXPORT)
	{
	  /* Process qualified export (presence of exported object and
	     presence of type definition of opaque type). */
	  register ICN_pointer exported_object;

	  exported_object
	    = find_identifier_declaration
	    (SCOPE (declaration_element), IDENTIFIER (declaration_element));
	  if (exported_object == NULL)
	    {
	      if (!it_is_definition_module)
		error_with_parameter
		  (ERR_qualified_export_of_undeclared_identifier,
		   declaration_name);
	    }
	  else if (MODE (exported_object) == ICNM_IMPORT)
	    error_with_parameter (ERR_qualified_export_of_imported_identifier,
				  declaration_name);
	  else if (!it_is_definition_module
		   && QUALIFIED_EXPORT_NODE (declaration_element)->opaque_type)
	    {
	      int error_is_fixed = FALSE;
	      int address_align, type_align;
	      Tcard address_size, type_size;

	      if (MODE (exported_object) != ICNM_TYPE_DECLARATION)
		error_is_fixed = TRUE;
	      else if (TYPE_DECLARATION_NODE (exported_object)->type_definition
		       != NULL)
		{
		  get_type_size_and_alignment
		    (ICN_POINTER (&ICN_TD_address), &address_size, &address_align);
		  get_type_size_and_alignment
		    (TYPE_DECLARATION_NODE (exported_object)->type_definition,
		     &type_size, &type_align);
		  if (type_align != address_align
		      || (!strict_flag && type_size != address_size)
		      ||
		      (strict_flag
		       &&
		       MODE (TYPE_DECLARATION_NODE (exported_object)
			     ->type_definition) != ICNM_POINTER_TYPE
		       && MODE (TYPE_DECLARATION_NODE (exported_object)
				->type_definition) != ICNM_QUALIFIED_EXPORT
		       &&
		       (third_edition_flag
			||
			(through_range_type
			 (TYPE_DECLARATION_NODE (exported_object)
			  ->type_definition)
			 != ICN_POINTER (&ICN_TD_cardinal)
			 &&
			 through_range_type
			 (TYPE_DECLARATION_NODE (exported_object)
			  ->type_definition)
			 != ICN_POINTER (&ICN_TD_integer)))))
		    error_is_fixed = TRUE;
		}
	      if (error_is_fixed)
		{
		  source_line_number = 0;
		  error_with_parameter (ERR_opaque_type_is_not_correct,
					declaration_name);
		}
	    }
	}
      else if (declaration_element_mode == ICNM_MODULE
	       && MODULE_NODE (declaration_element)->module_priority != NULL)
	{
	  current_scope = SCOPE (declaration_element);
	  MODULE_NODE (declaration_element)->module_priority
	    = eval (MODULE_NODE (declaration_element)->module_priority);
	  if (r1_constant_test_with_error_parameter (ERR_nonconstant_priority,
						     declaration_name))
	    {
	      if (!types_are_assignment_compatible (sinf1.type,
						    &ICN_TD_cardinal))
		error_with_parameter (ERR_priority_type, declaration_name);
	      else if (it_is_integer_type (sinf1.type))
		if (INTEGER_NODE (r1)->integer_value < 0)
		  error_with_parameter (ERR_negative_priority,
					declaration_name);
		else
		  {
		    register ICN_pointer priority;

		    priority = create_node (ICNM_CARDINAL);
		    CARDINAL_NODE (priority)->cardinal_value
		      = INTEGER_NODE (r1)->integer_value;
		    CARDINAL_NODE (priority)->value_type
		      = ICN_POINTER (&ICN_TD_cardinal);
		    MODULE_NODE (declaration_element)->module_priority
		      = do_accurate_type_value (priority);
		  }
	    }
	}
      else if (declaration_element_mode == ICNM_PROCEDURE)
	{
	  /* Process procedure declaration (presence and conformity of
	     procedure declarations in definition and corresponding
	     implementation module).  See commentaries for procedure
	     declaration node. */
	  register ICN_pointer main_declaration, secondary_declaration;

	  current_scope = SCOPE (declaration_element);
	  main_declaration
	    = find_identifier_declaration
	    (current_compilation_unit, IDENTIFIER (declaration_element));
	  secondary_declaration
	    = find_denotation_in_second_space
	    (current_compilation_unit, IDENTIFIER (declaration_element));
	  it_is_from_definition_module
	    = (secondary_declaration == declaration_element);
	  if (it_is_definition_module || !it_is_from_definition_module)
	    PROCEDURE_NODE (declaration_element)->procedure_type
	      = eval (PROCEDURE_NODE (declaration_element)->procedure_type);
	  if (!it_is_definition_module)
	    {
	      if (it_is_from_definition_module)
		{
		  if (main_declaration == declaration_element)
		    {
		      source_line_number = 0;
		      error_with_parameter
			(ERR_procedure_absent_in_implementation_module,
			 declaration_name);
		    }
		}
	      else if (secondary_declaration != NULL)
		if (MODE (secondary_declaration) != ICNM_PROCEDURE)
		  error_with_parameter
		    (ERR_declarations_difference_in_corresponding_modules,
		     declaration_name);
		else if (!(types_are_identical
			   (PROCEDURE_NODE (declaration_element)
			    ->procedure_type,
			    PROCEDURE_NODE (secondary_declaration)
			    ->procedure_type)))
		  error_with_parameter
		    (ERR_declarations_difference_in_corresponding_modules,
		     declaration_name);
	    }
	}
    }
}

/* The following function complete processing compilation unit pointer
   definition in the declaration element list starting with DECLARATION.
   DECLARATION is to refer to any declaration element node (or NULL).

   Pointers permits cyclic declarations.  Therefore identifier after pointer
   is changed only on corresponding type declaration (not type definition)
   for first processing time (see function eval).  The following function does
   change on the type definition after processing all other type
   definitions. */

void
complete_processing_pointer_definitions (declaration)
     register ICN_pointer declaration;
{
  register enum icode_node_mode mode;
  register ICN_pointer declaration_type, current_type, pointer_base_type;

  for (; declaration != NULL;
       declaration = NEXT_DECLARATION_ELEMENT (declaration))
    {
      mode = MODE (declaration);
      if (mode == ICNM_VARIABLE_DECLARATION)
	declaration_type
	  = VARIABLE_DECLARATION_NODE (declaration)->variable_type;
      else if (mode == ICNM_FIELD)
	declaration_type = FIELD_NODE (declaration)->field_type;
      else if (mode == ICNM_TYPE_DECLARATION)
	declaration_type
	  = TYPE_DECLARATION_NODE (declaration)->type_definition;
      else
	declaration_type = NULL;
      for (current_type = declaration_type; current_type != NULL;)
	{
	  mode = MODE (current_type);
	  if (mode != ICNM_ARRAY_TYPE && mode != ICNM_POINTER_TYPE)
	    break;		/* Pointer definition can not be achieved from here. */
	  else if (mode == ICNM_POINTER_TYPE)
	    {
	      pointer_base_type = POINTER_TYPE_NODE (current_type)->base_type;
	      if (pointer_base_type != NULL
		  && MODE (pointer_base_type) == ICNM_TYPE_DECLARATION)
		POINTER_TYPE_NODE (current_type)->base_type
		  = TYPE_DECLARATION_NODE (pointer_base_type)->type_definition;
	    }
	  current_type = BASE_TYPE (current_type);
	  if (current_type == declaration_type)
	    break;		/* All cycle was processed. */
	}
    }
}
