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

#include "m2-common.h"
#include "m2-scanner.h"
#include "m2-syntax.h"

static ICN_pointer expression ();
static ICN_pointer rest_of_designator ();
static ICN_pointer simple_type ();
static ICN_pointer field_list_sequence ();
static void optional_priority ();
static void procedure_heading ();
static void import_list ();


/* This macros is used in macros SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL
   and TEST_CURRENT_SYMBOL for implementation of pass of macro parameters
   containing commas. */

#define OR ,


/* The following macros is common part of macros TEST_CURRENT_SYMBOL and
   SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL.  This part declares set string
   and initiates it (if it is needed) by symbol table CONTEXT. */

#define SYMBOL_SET_STRING_DECLARATION(set_string) \
     static char *set_string = NULL;\
     if (set_string == NULL) set_string = new_symbol_set_string(context)


/* The following macros tests that current symbol is an expected symbol.  All
   expected symbols are in table SYMBOLS.  If it is false than error MESSAGE
   is fixed and syntax error recovery is on.  Set strings are used to speed
   up testing (see commentaries for macros SYMBOL_SET_STRING_DECLARATION). */

#define TEST_CURRENT_SYMBOL(symbols, message) do{\
     static enum symbol context[] = {symbols, SYMBOL_END_MARK};\
     SYMBOL_SET_STRING_DECLARATION (symbol_set_string);\
     test_current_symbol (symbol_set_string, message);\
   }while(FALSE)


/* The following macros is analogous to macros TEST_CURRENT_SYMBOL.  This
   macros also sets up flag to TRUE if current symbol is an expected symbol
   after testing and possibly syntax error recovery. */

#define SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL(flag, symbols, message) do{\
     static enum symbol context[] = {symbols, SYMBOL_END_MARK};\
     SYMBOL_SET_STRING_DECLARATION (symbol_set_string);\
     flag = test_current_symbol (symbol_set_string, message);\
   }while(FALSE)


/* The following macros sets up flag to TRUE if current symbol is an expected
   symbol.  All expected symbols are in table SYMBOLS.  Set strings are used
   to speed up the work. */

#define SET_UP_PRESENCE_FLAG_FOR_CURRENT_SYMBOL(flag, symbols) do{\
     static enum symbol context[] = {symbols, SYMBOL_END_MARK};\
     SYMBOL_SET_STRING_DECLARATION (symbol_set_string);\
     flag = symbol_is_in_set_string (current_symbol, symbol_set_string);\
   }while(FALSE)


/* Modula-2 syntax parser is implemented by recursive functions.  Each function
   analyzes and creates icode representation (see commentaries in file
   m2-icode.h) of some language construction.  Syntax of the construction is
   placed in commentaries of the corresponding function.  If the function
   returns pointer to icode node the pointer identifies icode subgraph which
   represents the corresponding construction.

   Modula-2 syntax parser works in three regimes.  Parsing in the first
   regime (global variable testing_main_module is equal to TRUE) serves for
   testing that given module is a main module.  The work in this regime
   finishes after parsing first keyword.  Parsing in the second regime
   (global variable picking_names_of_imported_modules is equal to TRUE)
   serves for collection of names of imported modules.  The work in this
   regime finishes after parsing import clauses.  The second regime serves
   for parsing all compilation unit.

   To describe the syntax, the same formalism (extended Backus-Naur Formalism
   called EBNF) as in Wirth's book is used.  But there exists one extension.
   The construction {A/s} denotes A, AsA, AsAsA and etc.

   Regular and optional parts of the language are implemented by C control
   statements.  The language recursion is implemented by recursive calls
   of the corresponding functions.

   Local syntax error recovery is described in scanner.  Global syntax error
   recovery is implemented by the stop symbols method with statement level.
   Parsing error construction is ceased only when any stop symbol is achieved.
   Here the stop symbols are symbols expected after the corresponding
   construction.*/



/* Code for parsing expressions and statements. */


/* qualident = {IDENTIFIER/"."} */

static ICN_pointer
qualident ()
{
  register ICN_pointer qualident_ptr, qualification_node_ptr;

  for (qualident_ptr = NULL;; next_symbol ())
    {
      if (current_symbol == SYMBOL_IDENTIFIER)
	{
	  if (qualident_ptr != NULL)
	    QUALIFICATION_NODE (qualident_ptr)->right_operand
	      = icode_symbol_presentation;
	  else
	    qualident_ptr = icode_symbol_presentation;
	}
      if (match_symbol (ERR_in_qualified_identifier, SYMBOL_IDENTIFIER)
	  != SYMBOL_PERIOD)
	break;
      qualification_node_ptr = create_node (ICNM_QUALIFICATION);
      QUALIFICATION_NODE (qualification_node_ptr)->left_operand
	= qualident_ptr;
      qualident_ptr = qualification_node_ptr;
    }
  return qualident_ptr;
}


static ICN_pointer
case_label_list (it_is_in_record)
     register int it_is_in_record;
/* case_label_list = {expression [".."expression]/ ","} */
{
  register ICN_pointer case_label_node_ptr, predecessor_in_label_list;
  register ICN_pointer expression_ptr, case_variant_node_ptr;
  int case_label_line;

  case_variant_node_ptr
    = create_node ((it_is_in_record
		    ? ICNM_RECORD_CASE_VARIANT : ICNM_CASE_STATEMENT_VARIANT));
  for (predecessor_in_label_list = case_variant_node_ptr;; next_symbol ())
    {
      case_label_line = source_line_number;
      expression_ptr = expression ();
      TEST_CURRENT_SYMBOL (SYMBOL_RANGE OR SYMBOL_COMMA OR SYMBOL_COLON,
			   ERR_in_case_label_list);
      if (current_symbol == SYMBOL_RANGE)
	{
	  case_label_node_ptr
	    = create_node ((it_is_in_record
			    ? ICNM_RECORD_CASE_LABEL_RANGE
			    : ICNM_CASE_STATEMENT_LABEL_RANGE));
	  next_symbol ();
	  CASE_LABEL_RANGE_NODE (case_label_node_ptr)
	    ->case_label_range_left_bound = expression_ptr;
	  CASE_LABEL_RANGE_NODE (case_label_node_ptr)
	    ->case_label_range_right_bound = expression ();
	}
      else
	{
	  case_label_node_ptr
	    = create_node ((it_is_in_record
			    ? ICNM_RECORD_CASE_LABEL_ELEMENT
			    : ICNM_CASE_STATEMENT_LABEL_ELEMENT));
	  LINE (case_label_node_ptr) = case_label_line;
	  CASE_LABEL_ELEMENT_NODE (case_label_node_ptr)->case_label_value
	    = expression_ptr;
	}
      CASE_LABEL_LIST (predecessor_in_label_list) = case_label_node_ptr;
      predecessor_in_label_list = case_label_node_ptr;
      TEST_CURRENT_SYMBOL (SYMBOL_COMMA OR SYMBOL_COLON,
			   ERR_in_case_label_list);
      if (current_symbol != SYMBOL_COMMA)
	break;
    }
  return case_variant_node_ptr;
}


/* designator = IDENTIFIER rest_of_designator */

static ICN_pointer
designator ()
{
  register ICN_pointer identifier_node_ptr;

  if (current_symbol == SYMBOL_IDENTIFIER)
    identifier_node_ptr = icode_symbol_presentation;
  else
    identifier_node_ptr = NULL;
  match_symbol (ERR_in_qualified_identifier, SYMBOL_IDENTIFIER);
  return rest_of_designator (identifier_node_ptr);
}


/* rest_of_designator = {"." IDENTIFIER | "[" {expression/","} "]" | "^"} */

static ICN_pointer
rest_of_designator (first_designator_identifier_ptr)
     ICN_pointer first_designator_identifier_ptr;
{
  register ICN_pointer designator_ptr, operation_ptr;

  for (designator_ptr = first_designator_identifier_ptr;;)
    if (current_symbol == SYMBOL_PERIOD)
      {
	operation_ptr = create_node (ICNM_QUALIFICATION);
	next_symbol ();
	QUALIFICATION_NODE (operation_ptr)->left_operand = designator_ptr;
	designator_ptr = operation_ptr;
	if (current_symbol == SYMBOL_IDENTIFIER)
	  QUALIFICATION_NODE (operation_ptr)->right_operand
	    = icode_symbol_presentation;
	match_symbol (ERR_in_qualified_identifier, SYMBOL_IDENTIFIER);
      }
    else if (current_symbol == SYMBOL_LEFT_BRACKET)
      {
	next_symbol ();
	for (;; next_symbol ())
	  {
	    operation_ptr = create_node (ICNM_INDEXING);
	    INDEXING_NODE (operation_ptr)->left_operand = designator_ptr;
	    designator_ptr = operation_ptr;
	    INDEXING_NODE (operation_ptr)->right_operand = expression ();
	    if (current_symbol != SYMBOL_COMMA)
	      break;
	  }
	match_symbol (ERR_in_index_value, SYMBOL_RIGHT_BRACKET);
      }
    else if (current_symbol == SYMBOL_DEREFERENCING)
      {
	operation_ptr = create_node (ICNM_DEREFERENCING);
	next_symbol ();
	DEREFERENCING_NODE (operation_ptr)->operand = designator_ptr;
	designator_ptr = operation_ptr;
      }
    else
      break;
  return designator_ptr;
}


/* The following function used by function expression for testing that
   the current symbol is an expected symbol.  All expected symbols are in
   SYMBOL_TABLE.  If it is false the function does nothing.  Otherwise
   the function skips the symbol, creates icode node with the corresponding
   mode from MODE_TABLE and returns pointer to it. */

static ICN_pointer
test_current_symbol_and_create_node (symbol_table, mode_table)
     enum symbol *symbol_table;
     ICN_mode *mode_table;
{
  register ICN_pointer result;
  register int n;

  n = number_of_current_symbol_in_table (symbol_table);
  if (n >= 0)
    {
      result = create_node ((enum icode_node_mode) mode_table[n]);
      next_symbol ();
      return result;
    }
  return NULL;
}


/* expression=
    {unary_operator
     {
      {
       {"NOT"} (constant | "(" expression ")" |
                [qualident] "{" [{expression [".."expression] /","}] "}" |
                qualident rest_of_designator [ "(" [{expression/","}] ")" ]
               )
      /mul_operator}
     /add_operator}
    /relation}*2

    Construction `*2' means maximum number of repeats (of operands).
 */

static ICN_pointer
expression ()
{
  int number_of_relation_operators, set_with_qualident;
  int line_of_node, presence_flag;
  register ICN_pointer icode_node_ptr;
  register ICN_pointer primary_ptr, factor_ptr, term_ptr, expression_ptr;
  register ICN_pointer unary_operation_ptr;
  register ICN_pointer first_not_node_ptr, last_not_node_ptr;
  register ICN_pointer first_actual_parameter_ptr, predecessor;
  register ICN_pointer expression_in_set_ptr;

  for (number_of_relation_operators = 0, expression_ptr = NULL;;)
    {
      {
	static enum symbol unop_symbol_table[] =
	{
	  SYMBOL_ADDITION, SYMBOL_SUBTRACTION, SYMBOL_END_MARK
	};
	static ICN_mode unop_mode_table[] =
	{
	  (ICN_mode) ICNM_IDENTITY, (ICN_mode) ICNM_SIGN_CONVERSION
	};

	unary_operation_ptr
	  = test_current_symbol_and_create_node (unop_symbol_table,
						 unop_mode_table);
      }
      for (term_ptr = NULL;;)
	{
	  for (factor_ptr = NULL;;)
	    {
	      for (first_not_node_ptr = last_not_node_ptr = NULL;
		   current_symbol == SYMBOL_NOT;)
		{
		  icode_node_ptr = create_node (ICNM_NOT);
		  next_symbol ();
		  if (first_not_node_ptr == NULL)
		    first_not_node_ptr = icode_node_ptr;
		  if (last_not_node_ptr != NULL)
		    NOT_NODE (last_not_node_ptr)->operand = icode_node_ptr;
		  last_not_node_ptr = icode_node_ptr;
		}
	      TEST_CURRENT_SYMBOL
		(SYMBOL_INTEGER OR SYMBOL_REAL OR SYMBOL_CHAR OR SYMBOL_STRING
		 OR SYMBOL_LEFT_PARENTHESIS OR SYMBOL_IDENTIFIER
		 OR SYMBOL_LEFT_SET_BRACKET, ERR_in_expression);
	      SET_UP_PRESENCE_FLAG_FOR_CURRENT_SYMBOL
		(presence_flag, SYMBOL_INTEGER OR SYMBOL_REAL OR SYMBOL_CHAR
		 OR SYMBOL_STRING);
	      if (presence_flag)
		{
		  primary_ptr = icode_symbol_presentation;
		  next_symbol ();
		}
	      else
		primary_ptr = NULL;
	      if (primary_ptr == NULL)
		{
		  if (current_symbol == SYMBOL_LEFT_PARENTHESIS)
		    {
		      next_symbol ();
		      primary_ptr = expression ();
		      match_symbol (ERR_in_expression,
				    SYMBOL_RIGHT_PARENTHESIS);
		    }
		  else
		    {
		      line_of_node = source_line_number;
		      if (current_symbol == SYMBOL_IDENTIFIER)
			{
			  set_with_qualident = TRUE;
			  primary_ptr = qualident ();
			}
		      else
			set_with_qualident = FALSE;
		      if (current_symbol == SYMBOL_LEFT_SET_BRACKET)
			{
			  icode_node_ptr = create_node (ICNM_SET_VALUE_START);
			  LINE (icode_node_ptr) = line_of_node;
			  if (set_with_qualident)
			    SET_VALUE_START_NODE (icode_node_ptr)->left_operand
			      = primary_ptr;
			  else
			    SET_VALUE_START_NODE (icode_node_ptr)->left_operand
			      = ICN_POINTER (&ICN_TD_bitset);
			  primary_ptr = icode_node_ptr;
			  predecessor = NULL;
			  if (match_symbol (ERR_in_expression,
					    SYMBOL_LEFT_SET_BRACKET)
			      != SYMBOL_RIGHT_SET_BRACKET)
			    {
			      for (;; next_symbol ())
				{
				  line_of_node = source_line_number;
				  expression_in_set_ptr = expression ();
				  TEST_CURRENT_SYMBOL
				    (SYMBOL_RANGE OR SYMBOL_COMMA
				     OR SYMBOL_RIGHT_SET_BRACKET, ERR_in_set);
				  if (current_symbol == SYMBOL_RANGE)
				    {
				      icode_node_ptr
					= create_node
					(ICNM_RANGE_IN_SET_VALUE);
				      next_symbol ();
				      RANGE_IN_SET_VALUE_NODE (icode_node_ptr)
					->left_operand = expression_in_set_ptr;
				      RANGE_IN_SET_VALUE_NODE (icode_node_ptr)
					->right_operand = expression ();
				    }
				  else
				    {
				      icode_node_ptr
					= create_node
					(ICNM_ELEMENT_IN_SET_VALUE);
				      LINE (icode_node_ptr) = line_of_node;
				      ELEMENT_IN_SET_VALUE_NODE
					(icode_node_ptr)->operand
					= expression_in_set_ptr;
				    }
				  if (predecessor != NULL)
				    RIGHT_OPERAND (predecessor)
				      = icode_node_ptr;
				  else
				    predecessor = icode_node_ptr;
				  TEST_CURRENT_SYMBOL
				    (SYMBOL_COMMA OR SYMBOL_RIGHT_SET_BRACKET,
				     ERR_in_set);
				  if (current_symbol != SYMBOL_COMMA)
				    break;
				  icode_node_ptr = create_node (ICNM_ADDITION);
				  LEFT_OPERAND (icode_node_ptr) = predecessor;
				  predecessor = icode_node_ptr;
				}
			    }
			  match_symbol (ERR_in_set, SYMBOL_RIGHT_SET_BRACKET);
			  SET_VALUE_START_NODE (primary_ptr)->right_operand
			    = predecessor;
			}
		      else if (set_with_qualident)
			{
			  primary_ptr = rest_of_designator (primary_ptr);
			  if (current_symbol == SYMBOL_LEFT_PARENTHESIS)
			    {
			      icode_node_ptr
				= create_node (ICNM_FUNCTION_CALL);
			      next_symbol ();
			      FUNCTION_CALL_NODE (icode_node_ptr)
				->function_designator = primary_ptr;
			      primary_ptr = icode_node_ptr;
			      first_actual_parameter_ptr = NULL;
			      if (current_symbol != SYMBOL_RIGHT_PARENTHESIS)
				for (predecessor = NULL;; next_symbol ())
				  {
				    icode_node_ptr
				      = create_node (ICNM_ACTUAL_PARAMETER);
				    ACTUAL_PARAMETER_NODE (icode_node_ptr)
				      ->actual_parameter_expression
				      = expression ();
				    if (first_actual_parameter_ptr == NULL)
				      first_actual_parameter_ptr
					= icode_node_ptr;
				    if (predecessor != NULL)
				      ACTUAL_PARAMETER_NODE (predecessor)
					->next_actual_parameter
					= icode_node_ptr;
				    predecessor = icode_node_ptr;
				    TEST_CURRENT_SYMBOL
				      (SYMBOL_COMMA
				       OR SYMBOL_RIGHT_PARENTHESIS,
				       ERR_in_actual_parameters);
				    if (current_symbol != SYMBOL_COMMA)
				      break;
				  }
			      FUNCTION_CALL_NODE (primary_ptr)
				->function_actual_parameter_list
				= first_actual_parameter_ptr;
			      match_symbol (ERR_in_actual_parameters,
					    SYMBOL_RIGHT_PARENTHESIS);
			    }
			}
		    }
		}
	      if (last_not_node_ptr != NULL)
		{
		  NOT_NODE (last_not_node_ptr)->operand = primary_ptr;
		  primary_ptr = first_not_node_ptr;
		}
	      if (factor_ptr != NULL)
		RIGHT_OPERAND (factor_ptr) = primary_ptr;
	      else
		factor_ptr = primary_ptr;
	      {
		static enum symbol mulop_symbol_table[] =
		{
		  SYMBOL_AND, SYMBOL_MULTIPLICATION, SYMBOL_DIV, SYMBOL_MOD,
		  SYMBOL_DIVISION, SYMBOL_END_MARK
		};
		static ICN_mode mulop_mode_table[] =
		{
		  (ICN_mode) ICNM_AND, (ICN_mode) ICNM_MULT,
		  (ICN_mode) ICNM_DIV, (ICN_mode) ICNM_MOD,
		  (ICN_mode) ICNM_REAL_DIVISION
		};

		icode_node_ptr
		  = test_current_symbol_and_create_node (mulop_symbol_table,
							 mulop_mode_table);
	      }
	      if (icode_node_ptr == NULL)
		break;
	      LEFT_OPERAND (icode_node_ptr) = factor_ptr;
	      factor_ptr = icode_node_ptr;
	    }
	  if (unary_operation_ptr != NULL)
	    {
	      OPERAND (unary_operation_ptr) = factor_ptr;
	      factor_ptr = unary_operation_ptr;
	      unary_operation_ptr = NULL;
	    }
	  if (term_ptr != NULL)
	    RIGHT_OPERAND (term_ptr) = factor_ptr;
	  else
	    term_ptr = factor_ptr;
	  {
	    static enum symbol addop_symbol_table[] =
	    {
	      SYMBOL_ADDITION, SYMBOL_SUBTRACTION, SYMBOL_OR, SYMBOL_END_MARK
	    };
	    static ICN_mode addop_mode_table[] =
	    {
	      (ICN_mode) ICNM_ADDITION, (ICN_mode) ICNM_SUBTRACTION,
	      (ICN_mode) ICNM_OR
	    };

	    icode_node_ptr
	      = test_current_symbol_and_create_node (addop_symbol_table,
						     addop_mode_table);
	  }
	  if (icode_node_ptr == NULL)
	    break;
	  LEFT_OPERAND (icode_node_ptr) = term_ptr;
	  term_ptr = icode_node_ptr;
	}
      if (expression_ptr != NULL)
	RIGHT_OPERAND (expression_ptr) = term_ptr;
      else
	expression_ptr = term_ptr;
      number_of_relation_operators++;
      if (number_of_relation_operators > 1)
	break;
      else
	{
	  {
	    static enum symbol relation_symbol_table[] =
	    {
	      SYMBOL_EQUAL, SYMBOL_UNEQUAL, SYMBOL_LESS_THAN,
	      SYMBOL_GREATER_THAN, SYMBOL_LESS_THAN_OR_EQUAL,
	      SYMBOL_GREATER_THAN_OR_EQUAL, SYMBOL_IN, SYMBOL_END_MARK
	    };
	    static ICN_mode relation_mode_table[] =
	    {
	      (ICN_mode) ICNM_EQUAL, (ICN_mode) ICNM_UNEQUAL,
	      (ICN_mode) ICNM_LESS_THAN, (ICN_mode) ICNM_GREATER_THAN,
	      (ICN_mode) ICNM_LESS_OR_EQUAL, (ICN_mode) ICNM_GREATER_OR_EQUAL,
	      (ICN_mode) ICNM_IN
	    };

	    icode_node_ptr
	      = test_current_symbol_and_create_node (relation_symbol_table,
						     relation_mode_table);
	  }
	  if (icode_node_ptr == NULL)
	    break;
	}
      LEFT_OPERAND (icode_node_ptr) = expression_ptr;
      expression_ptr = icode_node_ptr;
    }
  return expression_ptr;
}


/* statement_sequence =
     {
      [
      designator ":=" expression |
      designator["(" [{expression/","}]")"] |
      "EXIT" |
      "RETURN" [expression] |
      "IF" expression "THEN" statement_sequence
          {"ELSIF" expression "THEN" statement_sequence}
          ["ELSE" statement_sequence] "END" |
      "CASE" expression "OF"{[case_label_list ":" statement_sequence]/"|"}
          ["ELSE" statement_sequence] "END" |
      "WHILE" expression "DO" statement_sequence "END" |
      "REPEAT" statement_sequence "UNTIL" expression  |
      "FOR" IDENTIFIER":=" expression "TO" expression ["BY" expression]
         "DO" statmentsequnence "END" |
      "LOOP" statement_sequence "END"  |
      "WITH" designator "DO" statement_sequence "END" | CTEXT
      ]
     /";"}
 */

static ICN_pointer
statement_sequence ()
{
  register ICN_pointer current_statement_ptr;
  register ICN_pointer node_ptr, pred_node_ptr, first_node_ptr;
  register ICN_pointer statement_list_head, pred_statement_ptr;
  int line_of_identifier, presence_flag;
  char *C_text_in_icode_memory;

  for (pred_statement_ptr = statement_list_head = NULL;;)
    {
      TEST_CURRENT_SYMBOL
	(SYMBOL_IDENTIFIER OR SYMBOL_EXIT OR SYMBOL_RETURN OR SYMBOL_IF
	 OR SYMBOL_CASE OR SYMBOL_WHILE OR SYMBOL_REPEAT OR SYMBOL_FOR
	 OR SYMBOL_LOOP OR SYMBOL_WITH OR SYMBOL_C_TEXT OR SYMBOL_SEMICOLON
	 OR SYMBOL_END OR SYMBOL_ELSE OR SYMBOL_ELSIF OR SYMBOL_BAR
	 OR SYMBOL_UNTIL, ERR_in_statement);
      ADD_STOP_SYMBOL (SYMBOL_SEMICOLON);
      if (current_symbol == SYMBOL_IDENTIFIER)
	{
	  line_of_identifier = source_line_number;
	  node_ptr = designator ();
	  TEST_CURRENT_SYMBOL
	    (SYMBOL_ASSIGNMENT OR SYMBOL_LEFT_PARENTHESIS OR SYMBOL_SEMICOLON
	     OR SYMBOL_END OR SYMBOL_ELSE OR SYMBOL_ELSIF OR SYMBOL_BAR
	     OR SYMBOL_UNTIL, ERR_in_statement);
	  if (current_symbol == SYMBOL_ASSIGNMENT)
	    {
	      current_statement_ptr = create_node (ICNM_ASSIGNMENT);
	      next_symbol ();
	      ASSIGNMENT_NODE (current_statement_ptr)->assignment_variable
		= node_ptr;
	      ASSIGNMENT_NODE (current_statement_ptr)->assignment_expression
		= expression ();
	    }
	  else
	    {
	      current_statement_ptr = create_node (ICNM_PROCEDURE_CALL);
	      LINE (current_statement_ptr) = line_of_identifier;
	      PROCEDURE_CALL_NODE (current_statement_ptr)->procedure_designator
		= node_ptr;
	      first_node_ptr = NULL;
	      if (current_symbol == SYMBOL_LEFT_PARENTHESIS)
		{
		  next_symbol ();
		  if (current_symbol != SYMBOL_RIGHT_PARENTHESIS)
		    {
		      for (pred_node_ptr = NULL;;)
			{
			  node_ptr = create_node (ICNM_ACTUAL_PARAMETER);
			  ACTUAL_PARAMETER_NODE (node_ptr)
			    ->actual_parameter_expression = expression ();
			  if (first_node_ptr == NULL)
			    first_node_ptr = node_ptr;
			  if (pred_node_ptr != NULL)
			    ACTUAL_PARAMETER_NODE (pred_node_ptr)
			      ->next_actual_parameter = node_ptr;
			  pred_node_ptr = node_ptr;
			  TEST_CURRENT_SYMBOL
			    (SYMBOL_COMMA OR SYMBOL_RIGHT_PARENTHESIS,
			     ERR_in_actual_parameters);
			  if (current_symbol == SYMBOL_COMMA)
			    next_symbol ();
			  else if (current_symbol == SYMBOL_RIGHT_PARENTHESIS
				   || it_is_stop_symbol[(int) current_symbol])
			    break;
			}
		    }
		  match_symbol (ERR_in_actual_parameters,
				SYMBOL_RIGHT_PARENTHESIS);
		}
	      PROCEDURE_CALL_NODE (current_statement_ptr)
		->procedure_actual_parameter_list = first_node_ptr;
	    }
	}
      else if (current_symbol == SYMBOL_EXIT)
	{
	  current_statement_ptr = create_node (ICNM_EXIT_STATEMENT);
	  next_symbol ();
	}
      else if (current_symbol == SYMBOL_RETURN)
	{
	  current_statement_ptr = create_node (ICNM_RETURN_WITHOUT_RESULT);
	  next_symbol ();
	  TEST_CURRENT_SYMBOL
	    (SYMBOL_ADDITION OR SYMBOL_SUBTRACTION OR SYMBOL_NOT
	     OR SYMBOL_INTEGER OR SYMBOL_REAL OR SYMBOL_CHAR OR SYMBOL_STRING
	     OR SYMBOL_LEFT_PARENTHESIS OR SYMBOL_IDENTIFIER
	     OR SYMBOL_SEMICOLON OR SYMBOL_END OR SYMBOL_ELSE OR SYMBOL_ELSIF
	     OR SYMBOL_BAR OR SYMBOL_UNTIL, ERR_in_statement);
	  SET_UP_PRESENCE_FLAG_FOR_CURRENT_SYMBOL
	    (presence_flag, SYMBOL_SEMICOLON OR SYMBOL_END OR SYMBOL_ELSE
	     OR SYMBOL_ELSIF OR SYMBOL_BAR OR SYMBOL_UNTIL);
	  if (!presence_flag)
	    {
	      delete_node (current_statement_ptr);
	      current_statement_ptr = create_node (ICNM_RETURN_WITH_RESULT);
	      RETURN_WITH_RESULT_NODE (current_statement_ptr)
		->returned_expression = expression ();
	    }
	}
      else if (current_symbol == SYMBOL_IF)
	{
	  current_statement_ptr = create_node (ICNM_IF_STATEMENT);
	  next_symbol ();
	  IF_STATEMENT_NODE (current_statement_ptr)->if_expression
	    = expression ();
	  match_symbol (ERR_in_if_condition, SYMBOL_THEN);
	  ADD_STOP_SYMBOL (SYMBOL_ELSIF);
	  ADD_STOP_SYMBOL (SYMBOL_ELSE);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  IF_STATEMENT_NODE (current_statement_ptr)->if_statements
	    = statement_sequence ();
	  TEST_CURRENT_SYMBOL (SYMBOL_END OR SYMBOL_ELSE OR SYMBOL_ELSIF,
			       ERR_in_if_statement);
	  for (pred_node_ptr = current_statement_ptr;;)
	    {
	      TEST_CURRENT_SYMBOL (SYMBOL_END OR SYMBOL_ELSE OR SYMBOL_ELSIF,
				   ERR_in_if_statement);
	      if (current_symbol == SYMBOL_ELSIF)
		next_symbol ();
	      else if (current_symbol == SYMBOL_END
		       || current_symbol == SYMBOL_ELSE
		       || it_is_stop_symbol[(int) current_symbol])
		break;
	      node_ptr = create_node (ICNM_ELSIF_CLAUSE);
	      ELSIF_CLAUSE_NODE (node_ptr)->elsif_expression = expression ();
	      match_symbol (ERR_in_if_condition, SYMBOL_THEN);
	      ELSIF_CLAUSE_NODE (node_ptr)->elsif_statements
		= statement_sequence ();
	      if (MODE (pred_node_ptr) == ICNM_IF_STATEMENT)
		IF_STATEMENT_NODE (pred_node_ptr)->if_continuation = node_ptr;
	      else
		ELSIF_CLAUSE_NODE (pred_node_ptr)->elsif_continuation
		  = node_ptr;
	      pred_node_ptr = node_ptr;
	    }
	  REMOVE_STOP_SYMBOL (SYMBOL_ELSIF);
	  REMOVE_STOP_SYMBOL (SYMBOL_ELSE);
	  if (current_symbol == SYMBOL_ELSE)
	    {
	      next_symbol ();
	      if (MODE (pred_node_ptr) == ICNM_IF_STATEMENT)
		IF_STATEMENT_NODE (pred_node_ptr)->if_continuation
		  = statement_sequence ();
	      else
		ELSIF_CLAUSE_NODE (pred_node_ptr)->elsif_continuation
		  = statement_sequence ();
	    }
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_statement, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_CASE)
	{
	  current_statement_ptr = create_node (ICNM_CASE_STATEMENT);
	  next_symbol ();
	  CASE_STATEMENT_NODE (current_statement_ptr)->case_expression
	    = expression ();
	  match_symbol (ERR_in_case_value, SYMBOL_OF);
	  ADD_STOP_SYMBOL (SYMBOL_ELSE);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  for (first_node_ptr = pred_node_ptr = NULL;;)
	    {
	      SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL
		(presence_flag, SYMBOL_ADDITION OR SYMBOL_SUBTRACTION
		 OR SYMBOL_NOT OR SYMBOL_INTEGER OR SYMBOL_REAL OR SYMBOL_CHAR
		 OR SYMBOL_STRING OR SYMBOL_LEFT_PARENTHESIS
		 OR SYMBOL_IDENTIFIER OR SYMBOL_END OR SYMBOL_ELSE
		 OR SYMBOL_BAR, ERR_in_case_statement);
	      if (current_symbol == SYMBOL_BAR)
		next_symbol ();
	      else if (current_symbol == SYMBOL_ELSE
		       || current_symbol == SYMBOL_END
		       || (!presence_flag
			   && it_is_stop_symbol[(int) current_symbol]))
		break;
	      else
		{
		  node_ptr = case_label_list (FALSE);
		  match_symbol (ERR_in_case_label_list, SYMBOL_COLON);
		  ADD_STOP_SYMBOL (SYMBOL_BAR);
		  CASE_STATEMENT_VARIANT_NODE (node_ptr)->case_variant_elements
		    = statement_sequence ();
		  if (first_node_ptr == NULL)
		    first_node_ptr = node_ptr;
		  if (pred_node_ptr != NULL)
		    CASE_STATEMENT_VARIANT_NODE (pred_node_ptr)
		      ->next_case_variant = node_ptr;
		  pred_node_ptr = node_ptr;
		  CASE_STATEMENT_VARIANT_NODE (node_ptr)->next_case_variant
		    = current_statement_ptr;
		}
	    }
	  REMOVE_STOP_SYMBOL (SYMBOL_ELSE);
	  if (current_symbol == SYMBOL_ELSE)
	    {
	      node_ptr = create_node (ICNM_CASE_STATEMENT_VARIANT);
	      next_symbol ();
	      CASE_STATEMENT_VARIANT_NODE (node_ptr)->case_variant_elements
		= statement_sequence ();
	      CASE_STATEMENT_VARIANT_NODE (node_ptr)->next_case_variant
		= current_statement_ptr;
	      if (first_node_ptr == NULL)
		first_node_ptr = node_ptr;
	      if (pred_node_ptr != NULL)
		CASE_STATEMENT_VARIANT_NODE (pred_node_ptr)->next_case_variant
		  = node_ptr;
	    }
	  if (first_node_ptr != NULL)
	    CASE_STATEMENT_NODE (current_statement_ptr)
	      ->case_statement_variant_list = first_node_ptr;
	  else
	    CASE_STATEMENT_NODE (current_statement_ptr)
	      ->case_statement_variant_list = current_statement_ptr;
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_statement, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_WHILE)
	{
	  current_statement_ptr = create_node (ICNM_WHILE_STATEMENT);
	  next_symbol ();
	  WHILE_STATEMENT_NODE (current_statement_ptr)->while_expression
	    = expression ();
	  match_symbol (ERR_in_while_condition, SYMBOL_DO);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  WHILE_STATEMENT_NODE (current_statement_ptr)->while_statements
	    = statement_sequence ();
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_statement, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_REPEAT)
	{
	  current_statement_ptr = create_node (ICNM_REPEAT_STATEMENT);
	  next_symbol ();
	  ADD_STOP_SYMBOL (SYMBOL_UNTIL);
	  REPEAT_STATEMENT_NODE (current_statement_ptr)->repeat_statements
	    = statement_sequence ();
	  REMOVE_STOP_SYMBOL (SYMBOL_UNTIL);
	  match_symbol (ERR_in_statement, SYMBOL_UNTIL);
	  REPEAT_STATEMENT_NODE (current_statement_ptr)->until_expression
	    = expression ();
	}
      else if (current_symbol == SYMBOL_FOR)
	{
	  current_statement_ptr = create_node (ICNM_FOR_STATEMENT);
	  next_symbol ();
	  if (current_symbol == SYMBOL_IDENTIFIER)
	    FOR_STATEMENT_NODE (current_statement_ptr)->for_control_variable
	      = icode_symbol_presentation;
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	  match_symbol (ERR_symbol_assignment_expectation, SYMBOL_ASSIGNMENT);
	  FOR_STATEMENT_NODE (current_statement_ptr)->for_starting_value
	    = expression ();
	  match_symbol (ERR_in_for_statement_start_value, SYMBOL_TO);
	  FOR_STATEMENT_NODE (current_statement_ptr)->for_limit
	    = expression ();
	  TEST_CURRENT_SYMBOL (SYMBOL_BY OR SYMBOL_DO,
			       ERR_in_for_statement_limit_or_increment);
	  if (current_symbol == SYMBOL_BY)
	    {
	      next_symbol ();
	      FOR_STATEMENT_NODE (current_statement_ptr)->for_increment
		= expression ();
	    }
	  match_symbol (ERR_in_for_statement_limit_or_increment, SYMBOL_DO);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  FOR_STATEMENT_NODE (current_statement_ptr)->for_statements
	    = statement_sequence ();
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_statement, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_LOOP)
	{
	  current_statement_ptr = create_node (ICNM_LOOP_STATEMENT);
	  next_symbol ();
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  LOOP_STATEMENT_NODE (current_statement_ptr)->loop_statements
	    = statement_sequence ();
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_statement, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_WITH)
	{
	  current_statement_ptr = create_node (ICNM_WITH_STATEMENT);
	  next_symbol ();
	  WITH_STATEMENT_NODE (current_statement_ptr)->with_designator
	    = designator ();
	  match_symbol (ERR_in_with_statement_variable, SYMBOL_DO);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  WITH_STATEMENT_NODE (current_statement_ptr)->with_statements
	    = statement_sequence ();
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_statement, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_C_TEXT)
	{
	  current_statement_ptr
	    = create_node_with_string
	    (ICNM_C_CODE_IN_STATEMENTS,
	     C_CODE_IN_DECLARATIONS_NODE (icode_symbol_presentation)->C_text,
	     &C_text_in_icode_memory);
	  C_CODE_IN_STATEMENTS_NODE (current_statement_ptr)->C_text
	    = C_text_in_icode_memory;
	  delete_node (icode_symbol_presentation);
	  next_symbol ();
	}
      else
	current_statement_ptr = NULL;
      if (current_statement_ptr != NULL)
	{
	  if (statement_list_head == NULL)
	    statement_list_head = current_statement_ptr;
	  if (pred_statement_ptr != NULL)
	    NEXT_STATEMENT (pred_statement_ptr) = current_statement_ptr;
	  pred_statement_ptr = current_statement_ptr;
	}
      REMOVE_STOP_SYMBOL (SYMBOL_SEMICOLON);
      SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL
	(presence_flag, SYMBOL_SEMICOLON OR SYMBOL_END OR SYMBOL_ELSE
	 OR SYMBOL_ELSIF OR SYMBOL_BAR OR SYMBOL_UNTIL, ERR_in_statement);
      if (current_symbol == SYMBOL_SEMICOLON)
	next_symbol ();
      else if (presence_flag || it_is_stop_symbol[(int) current_symbol])
	break;
    }
  return statement_list_head;
}



/* Code for parsing declarations. */


/* If current scope is open scope (record, procedure) than its value
   is TRUE. */

static int current_scope_is_open;


/* The following variable is used for forming declaration list. */

static ICN_pointer pred_declaration_ptr;


/* The following variable is used for forming export list (it starts with
   module node). */

static ICN_pointer pred_export_ptr;

/* Add declaration to the end of declaration list.  Pointer to last element
   in the list is always in the variable pred_declaration_ptr. */

static void
add_declaration_to_list (declaration_ptr)
     register ICN_pointer declaration_ptr;
{
  NEXT_DECLARATION_ELEMENT (pred_declaration_ptr) = declaration_ptr;
  pred_declaration_ptr = declaration_ptr;
}

/* The following function creates declaration node (with members mode
   equal to MODE, scope referring to current_scope, identifier referring
   to current identifier and member in_definition_module equal to
   IN_DEFINITION_MODULE), includes it to the hash table, add the node to
   the declaration list and returns pointer to it.  If current symbol is
   not identifier or error is fixed then the node is not created and NULL
   is returned.  Fixed errors are standard identifier redeclaration in
   open scope and redeclaration identifier in the current scope.  Error
   is not fixed (although as early the node is not created and NULL is
   returned) if this is repeated declaration of import_node, export_node
   from the same MODULE_EXPORTER and export_qualified_node (only for
   local modules).  MODULE_EXPORTER refers to identifier_node in the case
   of processing import_node and to module_node in the case of processing
   export_node. */

static ICN_pointer
create_and_include_declaration_node (mode, in_definition_module,
				     module_exporter)
     int mode, in_definition_module;
     ICN_pointer module_exporter;
{
  register ICN_pointer declaration_node_ptr, old_declaration_node_ptr;
  register struct stdid *stdid_ptr;
  register int error_flag;

  declaration_node_ptr = NULL;
  if (current_symbol == SYMBOL_IDENTIFIER)
    {
      if (!current_scope_is_open)
	stdid_ptr
	  = find_stdid
	    (IDENTIFIER_NODE (icode_symbol_presentation)->identifier_string,
	     strlen
	     (IDENTIFIER_NODE (icode_symbol_presentation)->identifier_string));
      if (!current_scope_is_open && stdid_ptr != 0
	  && (!stdid_ptr->lower_case_stdid || !only_upper_case_flag))
	error (ERR_standard_indentifier_declaration);
      else
	{
	  declaration_node_ptr = create_node (mode);
	  IDENTIFIER (declaration_node_ptr) = icode_symbol_presentation;
	  SCOPE (declaration_node_ptr) = current_scope;
	  IN_DEFINITION_MODULE (declaration_node_ptr) = in_definition_module;
	  old_declaration_node_ptr =
	    find_in_or_else_include_to_table (declaration_node_ptr);
	  if (declaration_node_ptr != old_declaration_node_ptr)
	    {
	      declaration_node_ptr = NULL;
	      error_flag = TRUE;
	      if (MODE (old_declaration_node_ptr) == mode)
		{
		  if (mode == ICNM_QUALIFIED_EXPORT
		      && !it_is_definition_module)
		    /* The error for redeclartion of opaque type is fixed
		       because IT_IS_DEFINITION_MODULE is TRUE in this case. */
		    error_flag = FALSE;
		  else if (mode == ICNM_IMPORT)
		    error_flag
		      = ((IMPORT_NODE (old_declaration_node_ptr)
			  ->module_exporter != NULL
			  && MODE (IMPORT_NODE (old_declaration_node_ptr)
				   ->module_exporter) != ICNM_IDENTIFIER
			  ? IDENTIFIER (IMPORT_NODE (old_declaration_node_ptr)
					->module_exporter)
			  : (IMPORT_NODE (old_declaration_node_ptr)
			     ->module_exporter))
			 != module_exporter);
		  else if (mode == ICNM_EXPORT)
		    error_flag = (module_exporter
				  != (EXPORT_NODE (old_declaration_node_ptr)
				      ->module_exporter));
		}
	      if (error_flag)
		error_with_parameter
		  (ERR_repeated_identifier_declaration,
		   IDENTIFIER_NODE (icode_symbol_presentation)
		   ->identifier_string);
	    }
	  else
	    add_declaration_to_list (declaration_node_ptr);
	}
    }
  return declaration_node_ptr;
}


/* Add export node PTR to the end of export node list.  Pointer to last element
   in the list is always in the variable pred_export_ptr. */

#define ADD_EXPORT_TO_LIST(ptr) {EXPORT_LIST(pred_export_ptr)=ptr;\
				 pred_export_ptr=ptr;}


/* The following function is analogous to function
   create_and_include_declaration_node.  But this function returns nothing
   and sets up member opaque_type equal to IT_IS_OPAQUE_TYPE. */

static void
create_and_include_qualified_export_node (it_is_opaque_type)
     int it_is_opaque_type;
{
  register ICN_pointer node_ptr;

  node_ptr = create_and_include_declaration_node (ICNM_QUALIFIED_EXPORT,
						  FALSE, NULL);
  if (node_ptr != NULL)
    QUALIFIED_EXPORT_NODE (node_ptr)->opaque_type = it_is_opaque_type;
}


/* The following function tests that the current symbol is symbol C text.
   If it is false the function does nothing and returns FALSE.  Otherwise
   the function adds the corresponding icode node to the end of the declaration
   list, skips the symbol and and returns TRUE. */

static int
test_and_process_C_text ()
{
  if (current_symbol == SYMBOL_C_TEXT)
    {
      add_declaration_to_list (icode_symbol_presentation);
      next_symbol ();
      return TRUE;
    }
  return FALSE;
}


/* { constant_declaration = IDENTIFIER "=" const_expression ";" | CTEXT } */

static void
constant_declaration_list ()
{
  register ICN_pointer constant_declaration_ptr, expression_ptr;

  for (;;)
    {
      TEST_CURRENT_SYMBOL
	(SYMBOL_IDENTIFIER OR SYMBOL_C_TEXT OR SYMBOL_CONST
	 OR SYMBOL_TYPE OR SYMBOL_VAR OR SYMBOL_PROCEDURE OR SYMBOL_MODULE
	 OR SYMBOL_BEGIN OR SYMBOL_END, ERR_in_constant_declaration_part);
      if (current_symbol == SYMBOL_IDENTIFIER)
	{
	  constant_declaration_ptr
	    = create_and_include_declaration_node
	      (ICNM_CONSTANT_DECLARATION, it_is_definition_module, NULL);
	  next_symbol ();
	  match_symbol (ERR_symbol_equal_expectation, SYMBOL_EQUAL);
	  new_error_flag = FALSE;
	  expression_ptr = expression ();
	  if (constant_declaration_ptr != NULL && !new_error_flag)
	    CONSTANT_DECLARATION_NODE (constant_declaration_ptr)
	      ->constant_expression = expression_ptr;
	  match_symbol (ERR_in_constant_expression, SYMBOL_SEMICOLON);
	}
      else if (!test_and_process_C_text ())
	break;
    }
}


/* The following function is used in function type for forming
   type definition node list on member basic_type (e.g. POINTER TO
   POINTER TO ARRAY s OF ...).  The function adds node ADDED_NODE to the list
   and sets up correspondingly variable *FIRST_NODE_PTR
   and *PREDECESSOR_NODE_PTR. */

static void
new_type_definition_node (first_node_ptr, predecessor_node_ptr, added_node)
     register ICN_pointer *first_node_ptr, *predecessor_node_ptr;
     register ICN_pointer added_node;
{
  if (*first_node_ptr == NULL)
    *first_node_ptr = added_node;
  if (*predecessor_node_ptr != NULL)
    BASE_TYPE (*predecessor_node_ptr) = added_node;
  *predecessor_node_ptr = added_node;
}


/* type = {"POINTER" "TO" | "ARRAY" {simple_type/","} "OF"}
     ("RECORD" field_list_sequence "END" |
      "PROCEDURE" ( | "(" [ { [ "VAR" ][ "ARRAY" "OF" ] qualident/ ","} ] ")"
          ( |":"qualident)) |
      ["SET" "OF"] simple_type
     )
 */

static ICN_pointer
type ()
{
  int saved_current_scope_is_open;
  register ICN_pointer current_type_node_ptr, first_parameter_type_ptr;
  register ICN_pointer last_parameter_type_ptr, parameter_type_ptr;
  ICN_pointer first_type_node_ptr, pred_type_node_ptr;

  for (first_type_node_ptr = pred_type_node_ptr = NULL;;)
    {
      TEST_CURRENT_SYMBOL
	(SYMBOL_POINTER OR SYMBOL_ARRAY OR SYMBOL_RECORD OR SYMBOL_PROCEDURE
	 OR SYMBOL_SET OR SYMBOL_LEFT_PARENTHESIS OR SYMBOL_IDENTIFIER
	 OR SYMBOL_LEFT_BRACKET, ERR_in_type);
      if (current_symbol == SYMBOL_POINTER)
	{
	  current_type_node_ptr = create_node (ICNM_POINTER_TYPE);
	  next_symbol ();
	  match_symbol (ERR_keyword_to_expectation, SYMBOL_TO);
	}
      else if (current_symbol == SYMBOL_ARRAY)
	{
	  next_symbol ();
	  for (;; next_symbol ())
	    {
	      current_type_node_ptr = create_node (ICNM_ARRAY_TYPE);
	      ARRAY_TYPE_NODE (current_type_node_ptr)->array_index_type
		= simple_type ();
	      if (current_symbol != SYMBOL_COMMA)
		break;
	      new_type_definition_node
		(&first_type_node_ptr, &pred_type_node_ptr,
		 current_type_node_ptr);
	    }
	  match_symbol (ERR_in_index_type, SYMBOL_OF);
	}
      else
	break;
      new_type_definition_node
	(&first_type_node_ptr, &pred_type_node_ptr, current_type_node_ptr);
    }
  if (current_symbol == SYMBOL_RECORD)
    {
      current_type_node_ptr = create_node (ICNM_RECORD);
      next_symbol ();
      RECORD_NODE (current_type_node_ptr)->scope = current_scope;
      current_scope = current_type_node_ptr;
      saved_current_scope_is_open = current_scope_is_open;
      ADD_STOP_SYMBOL (SYMBOL_END);
      RECORD_NODE (current_type_node_ptr)->record_element_list
	= field_list_sequence (current_type_node_ptr);
      REMOVE_STOP_SYMBOL (SYMBOL_END);
      current_scope = RECORD_NODE (current_type_node_ptr)->scope;
      current_scope_is_open = saved_current_scope_is_open;
      match_symbol (ERR_in_field_declaration, SYMBOL_END);
    }
  else if (current_symbol == SYMBOL_PROCEDURE)
    {
      current_type_node_ptr = create_node (ICNM_PROCEDURE_TYPE);
      next_symbol ();
      first_parameter_type_ptr = last_parameter_type_ptr = NULL;
      if (current_symbol == SYMBOL_LEFT_PARENTHESIS)
	{
	  if (next_symbol () != SYMBOL_RIGHT_PARENTHESIS)
	    for (;;)
	      {
		parameter_type_ptr
		  = create_node (ICNM_PROCEDURE_PARAMETER_TYPE);
		TEST_CURRENT_SYMBOL
		  (SYMBOL_VAR OR SYMBOL_ARRAY OR SYMBOL_IDENTIFIER,
		   ERR_in_formal_parameter_declaration);
		if (last_parameter_type_ptr != NULL)
		  NEXT_DECLARATION_ELEMENT (last_parameter_type_ptr)
		    = parameter_type_ptr;
		last_parameter_type_ptr = parameter_type_ptr;
		if (first_parameter_type_ptr == NULL)
		  first_parameter_type_ptr = last_parameter_type_ptr;
		if (current_symbol == SYMBOL_VAR)
		  {
		    next_symbol ();
		    VAR_FORMAL_PARAMETER (last_parameter_type_ptr) = TRUE;
		  }
		TEST_CURRENT_SYMBOL (SYMBOL_ARRAY OR SYMBOL_IDENTIFIER,
				     ERR_in_formal_parameter_declaration);
		if (current_symbol == SYMBOL_ARRAY)
		  {
		    next_symbol ();
		    match_symbol (ERR_keyword_of_expectation, SYMBOL_OF);
		    ARRAY_FORMAL_PARAMETER (last_parameter_type_ptr) = TRUE;
		  }
		FORMAL_PARAMETER_TYPE (last_parameter_type_ptr) = qualident ();
		if (current_symbol == SYMBOL_COMMA)
		  next_symbol ();
		else if (current_symbol == SYMBOL_RIGHT_PARENTHESIS
			 || it_is_stop_symbol[(int) current_symbol])
		  break;
	      }
	  match_symbol (ERR_in_formal_parameter_declaration,
			SYMBOL_RIGHT_PARENTHESIS);
	  TEST_CURRENT_SYMBOL (SYMBOL_COLON OR SYMBOL_SEMICOLON, ERR_in_type);
	  if (current_symbol == SYMBOL_COLON)
	    {
	      next_symbol ();
	      PROCEDURE_TYPE_NODE (current_type_node_ptr)
		->procedure_result_type = qualident ();
	    }
	}
      NEXT_DECLARATION_ELEMENT (current_type_node_ptr)
	= first_parameter_type_ptr;
    }
  else
    {
      if (current_symbol == SYMBOL_SET)
	{
	  current_type_node_ptr = create_node (ICNM_SET_TYPE);
	  next_symbol ();
	  match_symbol (ERR_keyword_of_expectation, SYMBOL_OF);
	  new_type_definition_node (&first_type_node_ptr, &pred_type_node_ptr,
				    current_type_node_ptr);
	}
      current_type_node_ptr = simple_type ();
    }
  new_type_definition_node (&first_type_node_ptr, &pred_type_node_ptr,
			    current_type_node_ptr);
  return first_type_node_ptr;
}


/* The following function is used in function field_list_sequence for forming
   list on member next_case_variant.  The function adds node ADDED_NODE to
   the list and sets up correspondingly variable *FIRST_CASE_VARIANT_NODE_PTR
   and *PRED_CASE_VARIANT_NODE_PTR.  Member next_case_variant of added node is
   set up to RECORD_CASE_PART_NODE_PTR. */

static void
new_case_variant_node (first_case_variant_node_ptr, pred_case_variant_node_ptr,
		       added_node, record_case_part_node_ptr)
     register ICN_pointer *first_case_variant_node_ptr;
     register ICN_pointer *pred_case_variant_node_ptr;
     register ICN_pointer added_node, record_case_part_node_ptr;
{
  if (*first_case_variant_node_ptr == NULL)
    *first_case_variant_node_ptr = added_node;
  if (*pred_case_variant_node_ptr != NULL)
    RECORD_CASE_VARIANT_NODE (*pred_case_variant_node_ptr)->next_case_variant
      = added_node;
  *pred_case_variant_node_ptr = added_node;
  RECORD_CASE_VARIANT_NODE (added_node)->next_case_variant
    = record_case_part_node_ptr;
}


/* The following function is used in function field_list_sequence for forming
   list on member next_field.  The function adds node ADDED_NODE to
   the list and sets up correspondingly variable *FIRST_FIELD_PTR
   and *PRED_FIELD_PTR.  Member next_field or record_element_after_case_part
   of added node is set up to HEAD (it is to be record node or
   case variant node) for forming cyclic list. */

static void
new_field_or_case_part_node (first_field_ptr, pred_field_ptr, added_node, head)
     register ICN_pointer *first_field_ptr, *pred_field_ptr;
     register ICN_pointer added_node, head;
{
  if (added_node != NULL)
    {
      if (*first_field_ptr == NULL)
	*first_field_ptr = added_node;
      if (*pred_field_ptr != NULL)
	{
	  if (MODE (*pred_field_ptr) == ICNM_FIELD)
	    FIELD_NODE (*pred_field_ptr)->next_field = added_node;
	  else
	    RECORD_CASE_PART_NODE (*pred_field_ptr)
	      ->record_element_after_case_part = added_node;
	}
      *pred_field_ptr = added_node;
      if (MODE (added_node) == ICNM_FIELD)
	FIELD_NODE (added_node)->next_field = head;
      else
	RECORD_CASE_PART_NODE (added_node)->record_element_after_case_part
	  = head;
    }
}


/* field_list_sequence =
    { [ "CASE" [IDENTIFIER] ":" qualident "OF"
           { [case_label_list ":" field_list_sequence] / "|"}
           [ "ELSE" field_list_sequence] "END" |
        {IDENTIFIER/ ","} ":" type
      ]
    / ";"}

   HEAD is used for forming cyclic list (it is to be record node or
   case variant node).
 */

static ICN_pointer
field_list_sequence (head)
     ICN_pointer head;
{
  int tag_is_present, presense_flag;
  register ICN_pointer current_field_or_case_part, case_variant_node_ptr;
  register ICN_pointer node_ptr, field_node_list, case_part_ptr;
  ICN_pointer pred_field_ptr, first_field_ptr;
  ICN_pointer pred_case_variant_node_ptr, first_case_variant_node_ptr;

  for (first_field_ptr = pred_field_ptr = NULL;;)
    {
      TEST_CURRENT_SYMBOL
	(SYMBOL_CASE OR SYMBOL_IDENTIFIER OR SYMBOL_SEMICOLON
	 OR SYMBOL_BAR OR SYMBOL_END, ERR_in_field_declaration);
      if (current_symbol == SYMBOL_CASE)
	{
	  case_part_ptr = create_node (ICNM_RECORD_CASE_PART);
	  next_symbol ();
	  TEST_CURRENT_SYMBOL (SYMBOL_IDENTIFIER OR SYMBOL_COLON,
			       ERR_in_case_tag_type);
	  if (current_symbol == SYMBOL_IDENTIFIER)
	    {
	      tag_is_present = TRUE;
	      current_field_or_case_part
		= create_and_include_declaration_node
		  (ICNM_FIELD, FALSE, NULL);
	      if (current_field_or_case_part != NULL)
		new_field_or_case_part_node (&first_field_ptr, &pred_field_ptr,
					     current_field_or_case_part, head);
	      next_symbol ();
	    }
	  else
	    tag_is_present = FALSE;
	  match_symbol (ERR_in_case_tag_type, SYMBOL_COLON);
	  node_ptr = qualident ();
	  if (tag_is_present && current_field_or_case_part != NULL)
	    FIELD_NODE (current_field_or_case_part)->field_type = node_ptr;
	  current_field_or_case_part = case_part_ptr;
	  RECORD_CASE_PART_NODE (current_field_or_case_part)->case_tag_type
	    = node_ptr;
	  new_field_or_case_part_node (&first_field_ptr, &pred_field_ptr,
				       current_field_or_case_part, head);
	  match_symbol (ERR_in_case_tag_type, SYMBOL_OF);
	  ADD_STOP_SYMBOL (SYMBOL_ELSE);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  for (first_case_variant_node_ptr = pred_case_variant_node_ptr = NULL;
	       ;)
	    {
	      SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL
		(presense_flag, SYMBOL_ADDITION OR SYMBOL_SUBTRACTION
		 OR SYMBOL_NOT OR SYMBOL_INTEGER OR SYMBOL_REAL OR SYMBOL_CHAR
		 OR SYMBOL_STRING OR SYMBOL_LEFT_PARENTHESIS
		 OR SYMBOL_IDENTIFIER OR SYMBOL_ELSE OR SYMBOL_BAR
		 OR SYMBOL_END, ERR_in_record_case_part);
	      if (current_symbol == SYMBOL_BAR)
		next_symbol ();
	      else if (current_symbol == SYMBOL_ELSE
		       || current_symbol == SYMBOL_END
		       || (!presense_flag
			   && it_is_stop_symbol[(int) current_symbol]))
		break;
	      else
		{
		  case_variant_node_ptr = case_label_list (TRUE);
		  new_case_variant_node
		    (&first_case_variant_node_ptr, &pred_case_variant_node_ptr,
		     case_variant_node_ptr, current_field_or_case_part);
		  match_symbol (ERR_in_case_label_list, SYMBOL_COLON);
		  RECORD_CASE_VARIANT_NODE (case_variant_node_ptr)
		    ->case_variant_elements
		    = field_list_sequence (case_variant_node_ptr);
		}
	    }
	  REMOVE_STOP_SYMBOL (SYMBOL_ELSE);
	  if (current_symbol == SYMBOL_ELSE)
	    {
	      case_variant_node_ptr = create_node (ICNM_RECORD_CASE_VARIANT);
	      next_symbol ();
	      RECORD_CASE_VARIANT_NODE (case_variant_node_ptr)
		->case_variant_elements
		= field_list_sequence (case_variant_node_ptr);
	      new_case_variant_node
		(&first_case_variant_node_ptr, &pred_case_variant_node_ptr,
		 case_variant_node_ptr, current_field_or_case_part);
	    }
	  if (first_case_variant_node_ptr != NULL)
	    RECORD_CASE_PART_NODE (current_field_or_case_part)
	      ->record_case_variant_list = first_case_variant_node_ptr;
	  else
	    RECORD_CASE_PART_NODE (current_field_or_case_part)
	      ->record_case_variant_list = current_field_or_case_part;
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	  match_symbol (ERR_in_record_case_part, SYMBOL_END);
	}
      else if (current_symbol == SYMBOL_IDENTIFIER)
	{
	  for (field_node_list = NULL;;)
	    {
	      current_field_or_case_part
		= create_and_include_declaration_node
		  (ICNM_FIELD, FALSE, NULL);
	      /* Test is needed because error may be fixed
		 in create_and_include_declaration_node. */
	      if (current_field_or_case_part != NULL)
		{
		  if (field_node_list == NULL)
		    field_node_list = current_field_or_case_part;
		  new_field_or_case_part_node
		    (&first_field_ptr, &pred_field_ptr,
		     current_field_or_case_part, head);
		}
	      match_symbol (ERR_in_field_identifier_list, SYMBOL_IDENTIFIER);
	      TEST_CURRENT_SYMBOL (SYMBOL_COMMA OR SYMBOL_COLON,
				   ERR_in_field_identifier_list);
	      if (current_symbol == SYMBOL_COMMA)
		next_symbol ();
	      else if (current_symbol == SYMBOL_COLON
		       || it_is_stop_symbol[(int) current_symbol])
		break;
	    }
	  match_symbol (ERR_in_field_identifier_list, SYMBOL_COLON);
	  node_ptr = type ();
	  for (; field_node_list != NULL
	       && MODE (field_node_list) == ICNM_FIELD;
	       field_node_list = FIELD_NODE (field_node_list)->next_field)
	    FIELD_NODE (field_node_list)->field_type = node_ptr;
	}
      if (current_symbol == SYMBOL_SEMICOLON)
	next_symbol ();
      else if (current_symbol == SYMBOL_END
	       || current_symbol == SYMBOL_BAR
	       || it_is_stop_symbol[(int) current_symbol])
	break;
    }
  if (first_field_ptr != NULL)
    return first_field_ptr;
  else
    return head;
}


/* simple_type =
    "(" {IDENTIFIER/ ","} ")" |
    qualident | [qualident] "[" expression ".." expression "]"
 */

static ICN_pointer
simple_type ()
{
  int identifier_number, range_has_qualident;
  register ICN_pointer simple_type_ptr, node_ptr;
  register ICN_pointer first_enumeration_constant_ptr;
  register ICN_pointer enumeration_constant_value_ptr, expression_ptr;

  TEST_CURRENT_SYMBOL
    (SYMBOL_LEFT_PARENTHESIS OR SYMBOL_IDENTIFIER OR SYMBOL_LEFT_BRACKET,
     ERR_in_simple_type);
  if (current_symbol == SYMBOL_LEFT_PARENTHESIS)
    {
      next_symbol ();
      simple_type_ptr = create_node (ICNM_ENUMERATION_TYPE);
      identifier_number = 0;
      ADD_STOP_SYMBOL (SYMBOL_LEFT_PARENTHESIS);
      for (first_enumeration_constant_ptr = NULL;;)
	{
	  node_ptr
	    = create_and_include_declaration_node
	      (ICNM_CONSTANT_DECLARATION, it_is_definition_module, NULL);
	  if (node_ptr != NULL)
	    {
	      if (first_enumeration_constant_ptr == NULL)
		first_enumeration_constant_ptr = node_ptr;
	      enumeration_constant_value_ptr = create_node (ICNM_CARDINAL);
	      CARDINAL_NODE (enumeration_constant_value_ptr)->cardinal_value
		= identifier_number;
	      identifier_number++;
	      CARDINAL_NODE (enumeration_constant_value_ptr)->value_type
		= simple_type_ptr;
	      CONSTANT_DECLARATION_NODE (node_ptr)->constant_expression
		= find_in_or_else_include_to_table
		(enumeration_constant_value_ptr);
	    }
	  match_symbol (ERR_in_enumeration_type, SYMBOL_IDENTIFIER);
	  TEST_CURRENT_SYMBOL (SYMBOL_COMMA OR SYMBOL_RIGHT_PARENTHESIS,
			       ERR_in_enumeration_type);
	  if (current_symbol == SYMBOL_COMMA)
	    next_symbol ();
	  if (current_symbol == SYMBOL_RIGHT_PARENTHESIS
	      || it_is_stop_symbol[(int) current_symbol])
	    break;
	}
      ENUMERATION_TYPE_NODE (simple_type_ptr)->first_enumeration_constant
	= first_enumeration_constant_ptr;
      REMOVE_STOP_SYMBOL (SYMBOL_LEFT_PARENTHESIS);
      match_symbol (ERR_in_enumeration_type, SYMBOL_RIGHT_PARENTHESIS);
    }
  else
    {
      if (current_symbol == SYMBOL_IDENTIFIER)
	{
	  range_has_qualident = TRUE;
	  simple_type_ptr = qualident ();
	}
      else
	range_has_qualident = FALSE;
      if (!range_has_qualident || current_symbol == SYMBOL_LEFT_BRACKET)
	{
	  match_symbol (ERR_in_simple_type, SYMBOL_LEFT_BRACKET);
	  node_ptr = simple_type_ptr;
	  expression_ptr = expression ();
	  simple_type_ptr = create_node (ICNM_RANGE_TYPE);
	  if (range_has_qualident)
	    RANGE_TYPE_NODE (simple_type_ptr)->base_type = node_ptr;
	  RANGE_TYPE_NODE (simple_type_ptr)->left_range_bound = expression_ptr;
	  match_symbol (ERR_in_range_type, SYMBOL_RANGE);
	  RANGE_TYPE_NODE (simple_type_ptr)->right_range_bound = expression ();
	  match_symbol (ERR_in_range_type, SYMBOL_RIGHT_BRACKET);
	}
    }
  return simple_type_ptr;
}


/* {variable_declaration = {IDENTIFIER/ "," } ":" type ";" | CTEXT} */

static void
variable_declaration_list ()
{
  register ICN_pointer variable_declaration_node_ptr;
  register ICN_pointer variable_declaration_list, variable_type_ptr;

  for (;;)
    {
      TEST_CURRENT_SYMBOL
	(SYMBOL_IDENTIFIER OR SYMBOL_C_TEXT OR SYMBOL_CONST
	 OR SYMBOL_TYPE OR SYMBOL_VAR OR SYMBOL_PROCEDURE OR SYMBOL_MODULE
	 OR SYMBOL_BEGIN OR SYMBOL_END, ERR_in_variable_declaration_part);
      if (current_symbol == SYMBOL_IDENTIFIER)
	{
	  ADD_STOP_SYMBOL (SYMBOL_COLON);
	  for (variable_declaration_list = NULL;;)
	    {
	      variable_declaration_node_ptr
		= create_and_include_declaration_node
		  (ICNM_VARIABLE_DECLARATION, it_is_definition_module, NULL);
	      if (variable_declaration_list == NULL)
		variable_declaration_list = variable_declaration_node_ptr;
	      match_symbol (ERR_in_variable_list, SYMBOL_IDENTIFIER);
	      if (current_symbol == SYMBOL_COMMA)
		next_symbol ();
	      else if (current_symbol == SYMBOL_COLON
		       || it_is_stop_symbol[(int) current_symbol])
		break;
	    }
	  REMOVE_STOP_SYMBOL (SYMBOL_COLON);
	  match_symbol (ERR_in_variable_list, SYMBOL_COLON);
	  new_error_flag = FALSE;
	  variable_type_ptr = type ();
	  if (!new_error_flag)
	    for (; variable_declaration_list != NULL;
		 variable_declaration_list
		 = NEXT_DECLARATION_ELEMENT (variable_declaration_list))
	      if (MODE (variable_declaration_list)
		  == ICNM_VARIABLE_DECLARATION)
		VARIABLE_DECLARATION_NODE (variable_declaration_list)
		  ->variable_type = variable_type_ptr;
	  match_symbol (ERR_in_type, SYMBOL_SEMICOLON);
	}
      else if (!test_and_process_C_text ())
	break;
    }
}


/* If current symbol is identifier and is not equal to identifier of current
   scope than error ERR is fixed. */

static void
test_identifier_after_block (err)
     char *err;
{
  if (current_symbol == SYMBOL_IDENTIFIER
      && IDENTIFIER (current_scope) != NULL
      && IDENTIFIER (current_scope) != icode_symbol_presentation)
    error (err);
}


/* The following function creates declaration node MODE initiates its
   members scope by current_scope, in_definition_module by IN_DEFINITION_MODULE
   and identifier by current identifier (if current symbol is identifier) and
   returns pointer to the node. */

static ICN_pointer
create_and_initiate_declaration_node (mode, in_definition_module)
     int mode, in_definition_module;
{
  register ICN_pointer declaration_node_ptr;

  declaration_node_ptr = create_node (mode);
  SCOPE (declaration_node_ptr) = current_scope;
  IN_DEFINITION_MODULE (declaration_node_ptr) = in_definition_module;
  if (current_symbol == SYMBOL_IDENTIFIER)
    IDENTIFIER (declaration_node_ptr) = icode_symbol_presentation;
  return declaration_node_ptr;
}


/* The following variable is used for forming list of all block begin node
   in current compilation unit (member next_block_begin is used for this). */

static ICN_pointer last_block_begin_node_ptr;


/* block = {"CONST" constant_declaration_list |
            "TYPE" {IDENTIFIER "=" type ";" | CTEXT} |
	    "VAR" variable_declaration_list |
	     procedure_heading ";" block IDENTIFIER ";" |
	     "MODULE" IDENTIFIER optional_priority ";" import_list
	       [ "EXPORT" [ "QUALIFIED" ] {IDENTIFIER/","} ";" ]
	       block IDENTIFIER ";" |
	     CTEXT
	    }
	    [ "BEGIN" statement_sequence] "END" */

static void
block ()
{
  int it_is_qualified_export;
  register ICN_pointer declaration_ptr, node_ptr;
  register ICN_pointer pred_local_module_ptr, first_local_module_ptr;

  ADD_STOP_SYMBOL (SYMBOL_BEGIN);
  ADD_STOP_SYMBOL (SYMBOL_END);
  ADD_STOP_SYMBOL (SYMBOL_CONST);
  ADD_STOP_SYMBOL (SYMBOL_TYPE);
  ADD_STOP_SYMBOL (SYMBOL_VAR);
  ADD_STOP_SYMBOL (SYMBOL_PROCEDURE);
  ADD_STOP_SYMBOL (SYMBOL_MODULE);
  ADD_STOP_SYMBOL (SYMBOL_C_TEXT);
  for (pred_local_module_ptr = first_local_module_ptr = NULL;;)
    {
      TEST_CURRENT_SYMBOL
	(SYMBOL_C_TEXT OR SYMBOL_CONST OR SYMBOL_TYPE OR SYMBOL_VAR
	 OR SYMBOL_PROCEDURE OR SYMBOL_MODULE OR SYMBOL_BEGIN OR SYMBOL_END,
	 ERR_in_block);
      if (current_symbol == SYMBOL_CONST)
	{
	  next_symbol ();
	  constant_declaration_list ();
	}
      else if (current_symbol == SYMBOL_TYPE)
	{
	  next_symbol ();
	  for (;;)
	    {
	      TEST_CURRENT_SYMBOL
		(SYMBOL_IDENTIFIER OR SYMBOL_C_TEXT OR SYMBOL_CONST
		 OR SYMBOL_TYPE OR SYMBOL_VAR OR SYMBOL_PROCEDURE
		 OR SYMBOL_MODULE OR SYMBOL_BEGIN OR SYMBOL_END,
		 ERR_in_type_declaration_part);
	      if (current_symbol == SYMBOL_IDENTIFIER)
		{
		  declaration_ptr
		    = create_and_include_declaration_node
		      (ICNM_TYPE_DECLARATION, FALSE, NULL);
		  next_symbol ();
		  match_symbol (ERR_symbol_equal_expectation, SYMBOL_EQUAL);
		  new_error_flag = FALSE;
		  node_ptr = type ();
		  if (declaration_ptr != NULL && !new_error_flag)
		    {
		      TYPE_DECLARATION_NODE (declaration_ptr)->type_definition
			= node_ptr;
		      if (MODE (node_ptr) == ICNM_RECORD)
			RECORD_NODE (node_ptr)->record_type_identifier
			  = declaration_ptr;
		    }
		  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
		}
	      else if (!test_and_process_C_text ())
		break;
	    }
	}
      else if (current_symbol == SYMBOL_VAR)
	{
	  next_symbol ();
	  variable_declaration_list ();
	}
      else if (current_symbol == SYMBOL_PROCEDURE)
	{
	  procedure_heading ();
	  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
	  block ();
	  test_identifier_after_block (ERR_procedure_name_after_end);
	  current_scope = PROCEDURE_NODE (current_scope)->scope;
	  current_scope_is_open = MODE (current_scope) == ICNM_PROCEDURE;
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
	}
      else if (current_symbol == SYMBOL_MODULE)
	{
	  next_symbol ();
	  declaration_ptr
	    = create_and_include_declaration_node (ICNM_MODULE, FALSE, NULL);
	  if (declaration_ptr == NULL)
	    {
	      declaration_ptr = create_node (ICNM_MODULE);
	      MODULE_NODE (declaration_ptr)->scope = current_scope;
	      if (current_symbol == SYMBOL_IDENTIFIER)
		MODULE_NODE (declaration_ptr)->identifier
		  = icode_symbol_presentation;
	    }
	  if (first_local_module_ptr == NULL)
	    first_local_module_ptr = declaration_ptr;
	  if (pred_local_module_ptr != NULL)
	    MODULE_NODE (pred_local_module_ptr)->module_brother
	      = declaration_ptr;
	  pred_local_module_ptr = declaration_ptr;
	  MODULE_NODE (declaration_ptr)->module_mode = (char) MM_LOCAL_MODULE;
	  current_scope = declaration_ptr;
	  current_scope_is_open = FALSE;
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	  optional_priority ();
	  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
	  ADD_STOP_SYMBOL (SYMBOL_EXPORT);
	  import_list ();
	  REMOVE_STOP_SYMBOL (SYMBOL_EXPORT);
	  TEST_CURRENT_SYMBOL
	    (SYMBOL_EXPORT OR SYMBOL_C_TEXT OR SYMBOL_CONST OR SYMBOL_TYPE
	     OR SYMBOL_VAR OR SYMBOL_PROCEDURE OR SYMBOL_MODULE
	     OR SYMBOL_BEGIN OR SYMBOL_END, ERR_in_block);
	  if (current_symbol == SYMBOL_EXPORT)
	    {
	      next_symbol ();
	      TEST_CURRENT_SYMBOL (SYMBOL_QUALIFIED OR SYMBOL_IDENTIFIER,
				   ERR_in_export_list);
	      if (current_symbol == SYMBOL_QUALIFIED)
		{
		  next_symbol ();
		  it_is_qualified_export = TRUE;
		}
	      else
		it_is_qualified_export = FALSE;
	      for (pred_export_ptr = current_scope;;)
		{
		  if (current_symbol == SYMBOL_IDENTIFIER)
		    {
		      if (it_is_qualified_export)
			create_and_include_qualified_export_node (FALSE);
		      else
			{
			  /* It is may be procedure which is also declared
			     in corresponding definition module. */
			  current_scope = MODULE_NODE (current_scope)->scope;
			  current_scope_is_open
			    = MODE (current_scope) == ICNM_PROCEDURE;
			  if (current_symbol == SYMBOL_IDENTIFIER
			      && !it_is_definition_module
			      && SCOPE (current_scope) == NULL)
			    node_ptr
			      = find_denotation (current_scope,
						 icode_symbol_presentation);
			  if (current_symbol == SYMBOL_IDENTIFIER
			      && !it_is_definition_module
			      && SCOPE (current_scope) == NULL
			      && node_ptr != NULL
			      && MODE (node_ptr) == ICNM_PROCEDURE
			      && IN_DEFINITION_MODULE (node_ptr))
			    {
			      node_ptr
				= create_and_initiate_declaration_node
				  (ICNM_EXPORT, FALSE);
			      include_to_table (node_ptr);
			      add_declaration_to_list (node_ptr);
			    }
			  else
			    node_ptr
			      = create_and_include_declaration_node
				(ICNM_EXPORT, FALSE, declaration_ptr);
			  if (node_ptr != NULL)
			    {
			      ADD_EXPORT_TO_LIST (node_ptr);
			      EXPORT_NODE (node_ptr)->module_exporter
				= declaration_ptr;
			    }
			  current_scope = declaration_ptr;
			  current_scope_is_open = FALSE;
			}
		      match_symbol (ERR_in_export_list, SYMBOL_IDENTIFIER);
		      TEST_CURRENT_SYMBOL (SYMBOL_COMMA OR SYMBOL_SEMICOLON,
					   ERR_in_export_list);
		      if (current_symbol == SYMBOL_COMMA)
			next_symbol ();
		      else if (current_symbol == SYMBOL_SEMICOLON ||
			       it_is_stop_symbol[(int) current_symbol])
			break;
		    }
		}
	      match_symbol (ERR_in_export_list, SYMBOL_SEMICOLON);
	    }
	  block ();
	  test_identifier_after_block (ERR_module_name_after_end);
	  current_scope = MODULE_NODE (current_scope)->scope;
	  current_scope_is_open = MODE (current_scope) == ICNM_PROCEDURE;
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
	}
      else if (!test_and_process_C_text ()
	       && (current_symbol == SYMBOL_BEGIN || current_symbol == SYMBOL_END
		   || it_is_stop_symbol[(int) current_symbol]))
	break;
    }
  LOCAL_MODULE_LIST (current_scope) = first_local_module_ptr;
  node_ptr = create_node (ICNM_BLOCK_BEGIN);
  BLOCK_BEGIN_NODE (node_ptr)->block_scope = current_scope;
  if (last_block_begin_node_ptr != NULL)
    BLOCK_BEGIN_NODE (last_block_begin_node_ptr)->next_block_begin = node_ptr;
  last_block_begin_node_ptr = node_ptr;
  REMOVE_STOP_SYMBOL (SYMBOL_BEGIN);
  REMOVE_STOP_SYMBOL (SYMBOL_CONST);
  REMOVE_STOP_SYMBOL (SYMBOL_TYPE);
  REMOVE_STOP_SYMBOL (SYMBOL_VAR);
  REMOVE_STOP_SYMBOL (SYMBOL_PROCEDURE);
  REMOVE_STOP_SYMBOL (SYMBOL_MODULE);
  REMOVE_STOP_SYMBOL (SYMBOL_C_TEXT);
  if (current_symbol == SYMBOL_BEGIN)
    {
      next_symbol ();
      BLOCK_BEGIN_NODE (node_ptr)->next_statement = statement_sequence ();
    }
  REMOVE_STOP_SYMBOL (SYMBOL_END);
  match_symbol (ERR_in_block, SYMBOL_END);
}


/* optional_priority = [ ["const_expression"]" ] */

static void
optional_priority ()
{
  register ICN_pointer expression_ptr;

  TEST_CURRENT_SYMBOL (SYMBOL_LEFT_BRACKET OR SYMBOL_SEMICOLON,
		       ERR_in_priority);
  if (current_symbol == SYMBOL_LEFT_BRACKET)
    {
      next_symbol ();
      new_error_flag = FALSE;
      expression_ptr = expression ();
      if (!new_error_flag)
	MODULE_NODE (current_scope)->module_priority = expression_ptr;
      match_symbol (ERR_in_priority, SYMBOL_RIGHT_BRACKET);
    }
}


/* procedure_heading =
      "PROCEDURE" IDENTIFIER
          [ "(" [ { ["VAR"] {IDENTIFIER/","} ":"
                     [ "ARRAY" "OF" ] qualident/";"}
		]
	     ")" [ ":" qualident ]
	  ]
 */

static void
procedure_heading ()
{
  int it_is_var_parameter, it_is_array_parameter;
  register ICN_pointer node_ptr, list_of_parameters_with_the_same_type;
  register ICN_pointer formal_parameter_list;
  ICN_pointer procedure_ptr, procedure_type_ptr;
  struct stdid *stdid_ptr;

  match_symbol (ERR_in_declaration_part, SYMBOL_PROCEDURE);
  procedure_ptr
    = create_and_initiate_declaration_node (ICNM_PROCEDURE,
					    it_is_definition_module);
  if (current_symbol != SYMBOL_IDENTIFIER)
    {
      add_declaration_to_list (procedure_ptr);
    }
  else
    {
      if (!current_scope_is_open)
	stdid_ptr
	  = find_stdid
	  (IDENTIFIER_NODE (icode_symbol_presentation)->identifier_string,
	   strlen (IDENTIFIER_NODE (icode_symbol_presentation)
		   ->identifier_string));
      if (!current_scope_is_open && stdid_ptr != 0
	  && (!stdid_ptr->lower_case_stdid || !only_upper_case_flag))
	{
	  add_declaration_to_list (procedure_ptr);
	  error (ERR_standard_indentifier_declaration);
	}
      else
	{
	  node_ptr = find_in_or_else_include_to_table (procedure_ptr);
	  if (node_ptr == procedure_ptr)
	    {
	      add_declaration_to_list (procedure_ptr);
	    }
	  /* Corresponding procedure from definition module may be in the
	     hash table. */
	  else if (MODE (node_ptr) == ICNM_PROCEDURE
		   && !it_is_definition_module
		   && SCOPE (current_scope) == NULL
		   && IN_DEFINITION_MODULE (node_ptr))
	    {
	      procedure_ptr
		= create_and_initiate_declaration_node (ICNM_PROCEDURE, FALSE);
	      include_to_table (procedure_ptr);
	      add_declaration_to_list (procedure_ptr);
	    }
	  else
	    {
	      error_with_parameter (ERR_repeated_identifier_declaration,
				    IDENTIFIER_NODE (icode_symbol_presentation)
				    ->identifier_string);
	      procedure_ptr
		= create_and_initiate_declaration_node (ICNM_PROCEDURE, FALSE);
	      add_declaration_to_list (procedure_ptr);
	    }
	}
    }
  procedure_type_ptr = create_node (ICNM_PROCEDURE_TYPE);
  formal_parameter_list = NULL;
  current_scope = procedure_ptr;
  current_scope_is_open = TRUE;
  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
  TEST_CURRENT_SYMBOL (SYMBOL_LEFT_PARENTHESIS OR SYMBOL_SEMICOLON,
		       ERR_in_procedure_head);
  if (current_symbol == SYMBOL_LEFT_PARENTHESIS)
    {
      next_symbol ();
      ADD_STOP_SYMBOL (SYMBOL_RIGHT_PARENTHESIS);
      if (current_symbol != SYMBOL_RIGHT_PARENTHESIS)
	for (;;)
	  {
	    TEST_CURRENT_SYMBOL (SYMBOL_VAR OR SYMBOL_IDENTIFIER,
				 ERR_in_declaration_of_formal_parameters);
	    if (current_symbol == SYMBOL_VAR)
	      {
		next_symbol ();
		it_is_var_parameter = TRUE;
	      }
	    else
	      it_is_var_parameter = FALSE;
	    for (list_of_parameters_with_the_same_type = NULL;;)
	      {
		node_ptr
		  = create_and_include_declaration_node (ICNM_FORMAL_PARAMETER,
							 FALSE, NULL);
		if (formal_parameter_list == NULL)
		  formal_parameter_list = node_ptr;
		if (list_of_parameters_with_the_same_type == NULL)
		  list_of_parameters_with_the_same_type = node_ptr;
		match_symbol (ERR_in_formal_parameter_list, SYMBOL_IDENTIFIER);
		TEST_CURRENT_SYMBOL (SYMBOL_COMMA OR SYMBOL_COLON,
				     ERR_in_declaration_of_formal_parameters);
		if (current_symbol == SYMBOL_COMMA)
		  next_symbol ();
		else if (current_symbol == SYMBOL_COLON
			 || it_is_stop_symbol[(int) current_symbol])
		  break;
	      }
	    match_symbol (ERR_in_formal_parameter_list, SYMBOL_COLON);
	    TEST_CURRENT_SYMBOL (SYMBOL_ARRAY OR SYMBOL_IDENTIFIER,
				 ERR_in_declaration_of_formal_parameters);
	    if (current_symbol == SYMBOL_ARRAY)
	      {
		next_symbol ();
		match_symbol (ERR_keyword_of_expectation, SYMBOL_OF);
		it_is_array_parameter = TRUE;
	      }
	    else
	      it_is_array_parameter = FALSE;
	    new_error_flag = FALSE;
	    node_ptr = qualident ();
	    for (; list_of_parameters_with_the_same_type != NULL;
		 list_of_parameters_with_the_same_type
		 = NEXT_DECLARATION_ELEMENT
		 (list_of_parameters_with_the_same_type))
	      {
		if (!new_error_flag)
		  FORMAL_PARAMETER_TYPE (list_of_parameters_with_the_same_type)
		    = node_ptr;
		VAR_FORMAL_PARAMETER (list_of_parameters_with_the_same_type)
		  = it_is_var_parameter;
		ARRAY_FORMAL_PARAMETER (list_of_parameters_with_the_same_type)
		  = it_is_array_parameter;
	      }
	    TEST_CURRENT_SYMBOL (SYMBOL_SEMICOLON OR SYMBOL_RIGHT_PARENTHESIS,
				 ERR_in_declaration_of_formal_parameters);
	    if (current_symbol == SYMBOL_SEMICOLON)
	      next_symbol ();
	    else if (current_symbol == SYMBOL_RIGHT_PARENTHESIS
		     || it_is_stop_symbol[(int) current_symbol])
	      break;
	  }
      REMOVE_STOP_SYMBOL (SYMBOL_RIGHT_PARENTHESIS);
      match_symbol (ERR_in_declaration_of_formal_parameters,
		    SYMBOL_RIGHT_PARENTHESIS);
      TEST_CURRENT_SYMBOL (SYMBOL_COLON OR SYMBOL_SEMICOLON,
			   ERR_in_procedure_head);
      if (current_symbol == SYMBOL_COLON)
	{
	  next_symbol ();
	  new_error_flag = FALSE;
	  node_ptr = qualident ();
	  if (!new_error_flag)
	    PROCEDURE_TYPE_NODE (procedure_type_ptr)->procedure_result_type
	      = node_ptr;
	}
    }
  NEXT_DECLARATION_ELEMENT (procedure_type_ptr) = formal_parameter_list;
  PROCEDURE_NODE (procedure_ptr)->procedure_type = procedure_type_ptr;
}


/* The following function is used in function import_list for forming
   list of import nodes (on member next_declaration).  The function sets up
   variable *FIRST_IMPORT_PTR if it is needed and initiates member
   module_exporter in ADDED_NODE by IDENTIFIER_AFTER_FROM. */

static void
new_import_node (first_import_ptr, added_node, identifier_after_from)
     register ICN_pointer *first_import_ptr;
     ICN_pointer added_node, identifier_after_from;
{
  if (*first_import_ptr == NULL)
    *first_import_ptr = added_node;
  IMPORT_NODE (added_node)->module_exporter = identifier_after_from;
}


/* import_list = { [ "FROM" IDENTIFIER] "IMPORT" [ {IDENTIFIER/","} ";" ] } */

static void
import_list ()
{
  register ICN_pointer import_ptr, from_identifier_ptr;
  register int flag_presence;
  ICN_pointer first_import_ptr;

  for (first_import_ptr = NULL;;)
    {
      from_identifier_ptr = NULL;
      SET_PRESENCE_FLAG_AND_TEST_CURRENT_SYMBOL
	(flag_presence, SYMBOL_FROM OR SYMBOL_IMPORT OR SYMBOL_EXPORT
	 OR SYMBOL_CONST OR SYMBOL_TYPE OR SYMBOL_VAR OR SYMBOL_C_TEXT
	 OR SYMBOL_PROCEDURE OR SYMBOL_MODULE OR SYMBOL_BEGIN OR SYMBOL_END,
	 ERR_in_declaration_part);
      if (current_symbol == SYMBOL_FROM)
	{
	  next_symbol ();
	  if (current_symbol == SYMBOL_IDENTIFIER)
	    from_identifier_ptr = icode_symbol_presentation;
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	}
      else if (current_symbol != SYMBOL_IMPORT
	       && (flag_presence || it_is_stop_symbol[(int) current_symbol]))
	break;
      match_symbol (ERR_keyword_import_expectation, SYMBOL_IMPORT);
      TEST_CURRENT_SYMBOL (SYMBOL_IDENTIFIER OR SYMBOL_SEMICOLON,
			   ERR_in_import_list);
      if (current_symbol == SYMBOL_IDENTIFIER)
	for (;;)
	  {
	    import_ptr
	      = create_and_include_declaration_node
		(ICNM_IMPORT, FALSE, from_identifier_ptr);
	    if (import_ptr != NULL)
	      new_import_node (&first_import_ptr, import_ptr,
			       from_identifier_ptr);
	    match_symbol (ERR_in_import_list, SYMBOL_IDENTIFIER);
	    TEST_CURRENT_SYMBOL (SYMBOL_COMMA OR SYMBOL_SEMICOLON,
				 ERR_in_import_list);
	    if (current_symbol == SYMBOL_COMMA)
	      next_symbol ();
	    else if (current_symbol == SYMBOL_SEMICOLON
		     || it_is_stop_symbol[(int) current_symbol])
	      break;
	  }
      else
	{
	  import_ptr
	    = create_and_initiate_declaration_node (ICNM_IMPORT, FALSE);
	  add_declaration_to_list (import_ptr);
	  new_import_node (&first_import_ptr, import_ptr, from_identifier_ptr);
	}
      match_symbol (ERR_in_import_list, SYMBOL_SEMICOLON);
    }
  MODULE_NODE (current_scope)->import_list = first_import_ptr;
}


/* compilation_unit =
      "DEFINITION" "MODULE" IDENTIFIER ";" import_list
         { "CONST" constant_declaration_list |
           "TYPE" { IDENTIFIER[ "=" type ]; | CTEXT } |
           CTEXT |
           "VAR" variable_declaration_list |
           procedure_heading ";"
	 }"END" IDENTIFIER "." |
      [ "IMPLEMENTATION" ] "MODULE" IDENTIFIER optional_priority ";"
           import_list block IDENTIFIER "."
  
   The function returns TRUE if processed file has suffix of implementation
   module, and is an implementation module itself, and corresponding
   definition module has not been processed yet.  It means that only first
   keyword in the source file was processed and that it is necessary to
   process corresponding definition module and after that to repeat
   processing the source file. */

int
compilation_unit ()
{
  enum symbol saved_current_symbol;
  register ICN_pointer identifier_icode_presentation, type_declaration_ptr;
  register ICN_pointer type_definition_ptr, saved_icode_symbol_presentation;
  register int presence_flag;
  enum module_mode module_mode;

  start_scanner ();
  current_scope = last_block_begin_node_ptr = NULL;
  current_scope_is_open = FALSE;
  ADD_STOP_SYMBOL (SYMBOL_END_OF_FILE);
  TEST_CURRENT_SYMBOL (SYMBOL_DEFINITION OR SYMBOL_IMPLEMENTATION
		       OR SYMBOL_MODULE, ERR_in_compilation_unit_begin);
  it_is_main_module = (current_symbol == SYMBOL_MODULE);
  it_is_definition_module = (current_symbol == SYMBOL_DEFINITION);
  if (!testing_main_module)
    {
      if (current_symbol == SYMBOL_DEFINITION)
	{
	  next_symbol ();
	  match_symbol (ERR_in_compilation_unit_begin, SYMBOL_MODULE);
	  current_compilation_unit
	    = create_and_initiate_declaration_node (ICNM_MODULE, FALSE);
	  if (current_symbol == SYMBOL_IDENTIFIER
	      && (current_compilation_unit
	    != find_in_or_else_include_to_table (current_compilation_unit)))
	    {
	      current_compilation_unit
		= create_and_initiate_declaration_node (ICNM_MODULE, FALSE);
	      error_with_parameter
		(ERR_repeated_identifier_declaration,
		 IDENTIFIER_NODE (icode_symbol_presentation)
		 ->identifier_string);
	    }
	  MODULE_NODE (current_compilation_unit)->module_mode
	    = (char) MM_DEFINITION_MODULE;
	  pred_declaration_ptr
	    = pred_export_ptr = current_scope = current_compilation_unit;
	  test_input_file_name (current_compilation_unit);
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
	  ADD_STOP_SYMBOL (SYMBOL_CONST);
	  ADD_STOP_SYMBOL (SYMBOL_TYPE);
	  ADD_STOP_SYMBOL (SYMBOL_VAR);
	  ADD_STOP_SYMBOL (SYMBOL_PROCEDURE);
	  ADD_STOP_SYMBOL (SYMBOL_C_TEXT);
	  ADD_STOP_SYMBOL (SYMBOL_END);
	  import_list ();
	  if (!picking_names_of_imported_modules)
	    for (;;)
	      {
		TEST_CURRENT_SYMBOL
		  (SYMBOL_CONST OR SYMBOL_TYPE OR SYMBOL_C_TEXT OR SYMBOL_VAR
		OR SYMBOL_PROCEDURE OR SYMBOL_END, ERR_in_declaration_part);
		if (current_symbol == SYMBOL_CONST)
		  {
		    next_symbol ();
		    constant_declaration_list ();
		  }
		else if (current_symbol == SYMBOL_TYPE)
		  {
		    next_symbol ();
		    for (;;)
		      {
			TEST_CURRENT_SYMBOL
			  (SYMBOL_IDENTIFIER OR SYMBOL_CONST OR SYMBOL_TYPE
			   OR SYMBOL_C_TEXT OR SYMBOL_VAR OR SYMBOL_PROCEDURE
			   OR SYMBOL_END, ERR_in_type_declaration_part);
			if (current_symbol == SYMBOL_IDENTIFIER)
			  {
			    identifier_icode_presentation
			      = icode_symbol_presentation;
			    next_symbol ();
			    TEST_CURRENT_SYMBOL (SYMBOL_EQUAL
						 OR SYMBOL_SEMICOLON,
						 ERR_in_type_declaration_part);
			    saved_current_symbol = current_symbol;
			    saved_icode_symbol_presentation
			      = icode_symbol_presentation;
			    current_symbol = SYMBOL_IDENTIFIER;
			    icode_symbol_presentation
			      = identifier_icode_presentation;
			    if (saved_current_symbol != SYMBOL_EQUAL)
			      {
				create_and_include_qualified_export_node
				  (TRUE);
				current_symbol = saved_current_symbol;
				icode_symbol_presentation
				  = saved_icode_symbol_presentation;
			      }
			    else
			      {
				type_declaration_ptr
				  = create_and_include_declaration_node
				  (ICNM_TYPE_DECLARATION,
				   it_is_definition_module, NULL);
				next_symbol ();
				new_error_flag = FALSE;
				type_definition_ptr = type ();
				if (!new_error_flag
				    && type_declaration_ptr != NULL)
				  {
				    TYPE_DECLARATION_NODE
				      (type_declaration_ptr)
					->type_definition
					  = type_definition_ptr;
				    if (MODE (type_definition_ptr)
					== ICNM_RECORD)
				      RECORD_NODE (type_definition_ptr)
					->record_type_identifier
					= type_declaration_ptr;
				  }
			      }
			    match_symbol (ERR_in_type, SYMBOL_SEMICOLON);
			  }
			else if (!test_and_process_C_text ())
			  break;
		      }
		  }
		else if (current_symbol == SYMBOL_VAR)
		  {
		    next_symbol ();
		    variable_declaration_list ();
		  }
		else if (current_symbol == SYMBOL_PROCEDURE)
		  {
		    procedure_heading ();
		    current_scope = PROCEDURE_NODE (current_scope)->scope;
		    current_scope_is_open = FALSE;
		    match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
		  }
		else if (!test_and_process_C_text ()
			 && (current_symbol == SYMBOL_END
			     || it_is_stop_symbol[(int) current_symbol]))
		  {
		    match_symbol (ERR_in_declaration_part, SYMBOL_END);
		    test_identifier_after_block (ERR_module_name_after_end);
		    match_symbol (ERR_identifier_expectation,
				  SYMBOL_IDENTIFIER);
		    match_symbol (ERR_symbol_colon_expectation, SYMBOL_PERIOD);
		    break;
		  }
	      }
	  REMOVE_STOP_SYMBOL (SYMBOL_CONST);
	  REMOVE_STOP_SYMBOL (SYMBOL_TYPE);
	  REMOVE_STOP_SYMBOL (SYMBOL_VAR);
	  REMOVE_STOP_SYMBOL (SYMBOL_PROCEDURE);
	  REMOVE_STOP_SYMBOL (SYMBOL_C_TEXT);
	  REMOVE_STOP_SYMBOL (SYMBOL_END);
	}
      else
	{
	  if (current_symbol == SYMBOL_IMPLEMENTATION)
	    {
	      register char *name_without_suffix;

	      name_without_suffix = file_name_without_suffix (input_file_name);
	      if (strcmp (file_name_suffix (input_file_name),
			  IMPLEMENTATION_MODULE_SUFFIX) == 0
		  && (find_denotation
		      (ICN_POINTER (NULL),
		       create_identifier_node (base_file_name
					       (name_without_suffix)))
		      == NULL))
		{
		  M2C_FREE (name_without_suffix);
		  return TRUE;
		}
	      M2C_FREE (name_without_suffix);
	      next_symbol ();
	      module_mode = MM_DEFINITION_WITH_IMPLEMENTATION_MODULE;
	    }
	  else
	    module_mode = MM_MAIN_MODULE;
	  match_symbol (ERR_in_compilation_unit_begin, SYMBOL_MODULE);
	  current_compilation_unit
	    = create_and_initiate_declaration_node (ICNM_MODULE, FALSE);
	  if (current_symbol == SYMBOL_IDENTIFIER
	      && module_mode == MM_DEFINITION_WITH_IMPLEMENTATION_MODULE)
	    {
	      current_compilation_unit
		= find_in_or_else_include_to_table (current_compilation_unit);
	      for (pred_declaration_ptr = current_compilation_unit;
		   NEXT_DECLARATION_ELEMENT (pred_declaration_ptr) != NULL;
		   pred_declaration_ptr
		   = NEXT_DECLARATION_ELEMENT (pred_declaration_ptr));
	    }
	  else
	    {
	      if (current_symbol == SYMBOL_IDENTIFIER
	      && (find_in_or_else_include_to_table (current_compilation_unit)
		  != current_compilation_unit))
		{
		  current_compilation_unit
		    = create_and_initiate_declaration_node (ICNM_MODULE,
							    FALSE);
		  error_with_parameter (ERR_repeated_identifier_declaration,
				 IDENTIFIER_NODE (icode_symbol_presentation)
					->identifier_string);
		}
	      pred_declaration_ptr = current_compilation_unit;
	    }
	  MODULE_NODE (current_compilation_unit)->module_mode
	    = (char) module_mode;
	  current_scope = current_compilation_unit;
	  test_input_file_name (current_compilation_unit);
	  match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	  optional_priority ();
	  match_symbol (ERR_semicolon_expectation, SYMBOL_SEMICOLON);
	  import_list ();
	  if (!picking_names_of_imported_modules)
	    {
	      block ();
	      test_identifier_after_block (ERR_module_name_after_end);
	      match_symbol (ERR_identifier_expectation, SYMBOL_IDENTIFIER);
	      match_symbol (ERR_symbol_colon_expectation, SYMBOL_PERIOD);
	    }
	}
    }
  REMOVE_STOP_SYMBOL (SYMBOL_END_OF_FILE);
  if (!testing_main_module && !picking_names_of_imported_modules)
    match_symbol (ERR_end_of_file_expectation, SYMBOL_END_OF_FILE);
  return FALSE;
}


/* The following function is to be called only one for the translator
   run time. */

void
initiate_analyzer_data ()
{
  initiate_scanner_data ();
}
