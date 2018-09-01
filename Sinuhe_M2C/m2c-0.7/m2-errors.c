/* Error messages and function of work with errors for GNU Modula-2 translator.
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

/* Any error may be contain a parameter.  The parameter value is given
   for error_with_parameter call and printed according to
   conversion specification as in fprintf format. */


/* Fatal errors, i.e. the errors result in immediate translator finish. */

char ERR_no_memory[]
= "no memory";
char ERR_many_errors[]
= "too many errors";
char ERR_input_file_opening[]
= "source file %s is not opened";
char ERR_object_file_opening[]
= "object file %s is not opened";
char ERR_output_to_object_file[]
= "error for output to object file %s";
char ERR_file_closing[]
= "error for closing file %s";
char ERR_output_to_stdout[]
= "error for output to standard output file";
char ERR_invalid_file_name[]
= "file must be named as %s";
char ERR_definition_module_absence[]
= "definition module file %s is not opened";
char ERR_floating_point_exception[]
= "floating point exception is fixed";


/* Lexical errors. */

char ERR_octal_number[]
= "it\'s not octal number";
char ERR_decimal_number[]
= "it\'s not decimal number";
char ERR_big_char_code[]
= "too big character ordinal number";
char ERR_order_absence[]
= "order of real number is absent";
char ERR_comment_end_absence[]
= "comment end is absent";
char ERR_string_end_absence[]
= "string end is absent";
char ERR_invalid_input_char[]
= "invalid symbol";
char ERR_C_code_end_absence[]
= "C code end is absent";


/* Semantic errors which are searched on syntax analysis pass. */

char ERR_standard_indentifier_declaration[]
= "standard identifier redeclaration in module or in record";
char ERR_repeated_identifier_declaration[]
= "repeated declaration of identifier %s";


/* Syntactic errors. */

char ERR_symbol_equal_expectation[]
= "symbol \'=\' is expected";
char ERR_in_constant_expression[]
= "syntax error in constant expression";
char ERR_keyword_to_expectation[]
= "word TO is expected";
char ERR_in_index_type[]
= "syntax error in index type of array";
char ERR_in_field_declaration[]
= "syntax error in declaration of record fields";
char ERR_keyword_of_expectation[]
= "word OF is expected";
char ERR_in_formal_parameter_declaration[]
= "syntax error in declaration of formal parameters";
char ERR_in_case_tag_type[]
= "syntax error in case tag type";
char ERR_in_case_label_list[]
= "syntax error in case label lists";
char ERR_in_record_case_part[]
= "syntax error in case part of record";
char ERR_in_field_identifier_list[]
= "syntax error in field identifier list";
char ERR_in_enumeration_type[]
= "syntax error in enumeration type";
char ERR_in_simple_type[]
= "syntax error in simple type";
char ERR_in_range_type[]
= "syntax error in range type";
char ERR_in_qualified_identifier[]
= "syntax error in qualified identifier";
char ERR_in_variable_list[]
= "syntax error in variable identifier list";
char ERR_in_type[]
= "syntax error in type";
char ERR_in_index_value[]
= "syntax error in index value";
char ERR_in_expression[]
= "syntax error in expression";
char ERR_in_set[]
= "syntax error in set";
char ERR_in_actual_parameters[]
= "syntax error in actual parameter list";
char ERR_in_if_statement[]
= "syntax error in IF-statement";
char ERR_in_if_condition[]
= "syntax error in condition of IF-statement";
char ERR_in_statement[]
= "invalid statement";
char ERR_in_case_statement[]
= "syntax error in CASE-statement";
char ERR_in_case_value[]
= "invalid CASE-value";
char ERR_in_while_condition[]
= "syntax error in condition of WHILE-statement";
char ERR_identifier_expectation[]
= "identifier is expected";
char ERR_symbol_assignment_expectation[]
= "symbol \':=\' is expected";
char ERR_in_for_statement_start_value[]
= "sytax error in start value of FOR-statement";
char ERR_in_for_statement_limit_or_increment[]
= "syntax error in limit or increment of FOR-statement";
char ERR_in_with_statement_variable[]
= "syntax error in WITH-variable";
char ERR_semicolon_expectation[]
= "symbol \';\' is expected";
char ERR_procedure_name_after_end[]
= "identifier after END is not current procedure identifier";
char ERR_in_export_list[]
= "syntax error in export list";
char ERR_module_name_after_end[]
= "identifier after END is not current module identifier";
char ERR_in_block[]
= "syntax error in block";
char ERR_in_priority[]
= "syntax error in module priority";
char ERR_in_constant_declaration_part[]
= "syntax error in part of constant declarations";
char ERR_in_variable_declaration_part[]
= "syntax error in part of variable declarations";
char ERR_in_type_declaration_part[]
= "syntax error in part of type declarations";
char ERR_in_procedure_head[]
= "syntax error in procedure head";
char ERR_in_declaration_part[]
= "syntax error in part of declarations";
char ERR_in_formal_parameter_list[]
= "syntax error in list of formal parameters";
char ERR_in_declaration_of_formal_parameters[]
= "syntax error in declarations of formal parameters";
char ERR_keyword_import_expectation[]
= "word IMPORT is expected";
char ERR_in_import_list[]
= "syntax error in import list";
char ERR_in_compilation_unit_begin[]
= "syntax error in compilation unit begin";
char ERR_symbol_colon_expectation[]
= "symbol \':\' is expected";
char ERR_end_of_file_expectation[]
= "end of file is expected";


/* Semantic errors. */

char ERR_implicitly_exported_identifier_declaration[]
= "implicitly exported identifier %s is already declared";
char ERR_implicitly_imported_identifier_declaration[]
= "implicitly imported identifier %s is already declared";
char ERR_undeclared_identifier_export[]
= "export of undeclared identifier %s";
char ERR_undeclared_identifier_import[]
= "import of undeclared identifier %s";
char ERR_unexported_identifier_import[]
= "import of unexported identifer %s";
char ERR_import_from_undeclared_module[]
= "import from undeclared module %s";
char ERR_no_module_after_from[]
= "identifier %s after from is not module";
char ERR_no_type_in_type_definition[]
= "type definition of %s is not type";
char ERR_no_constant_expression_in_constant_definition[]
= "constant definition of %s is not constant expression";
char ERR_type_in_variable_declaration[]
= "error type in var declaration of %s";
char ERR_actual_parameter_is_not_expression[]
= "actual parameter is not expression(proc. %s)";
char ERR_nonvariable_for_var_formal_parameter[]
= "nonvariable for var formal parameter(proc. %s)";
char ERR_invalid_actual_parameter[]
= "invalid actual parameter type(proc. %s)";
char ERR_local_procedure_as_parameter[]
= "local procedure %s as actual parameter";
char ERR_nonconstant_case_label[]
= "case label is not constant expression";
char ERR_case_label_type[]
= "invalid types of case labels";
char ERR_case_label_types_inequality[]
= "different types of case labels";
char ERR_case_label_value_bounds[]
= "low bound of range in case label > upper bound";
char ERR_case_label_and_tag_types_inequality[]
= "different types of case label and discriminant";
char ERR_repeated_case_label[]
= "case label is repeated";
char ERR_field_type[]
= "error type in field declaration";
char ERR_case_tag_is_not_type[]
= "error type in case descriminant of record";
char ERR_record_case_tag_type[]
= "invalid type of case descriminant of record";
char ERR_undeclared_identifier[]
= "identifier %s is not declared";
char ERR_declaration_cycle[]
= "cycle is in declaration of %s";
char ERR_set_type[]
= "error type in set type definition";
char ERR_set_base_type[]
= "invalid range of values of base type of set";
char ERR_set_base_type_value[]
= "invalid bounds of base type of set";
char ERR_no_type_in_pointer_type[]
= "error type in pointer type definition";
char ERR_no_type_in_index_type[]
= "array index is not type";
char ERR_no_type_in_array_element[]
= "array component is not type";
char ERR_array_index_type[]
= "invalid array index type";
char ERR_range_base_type[]
= "invalid base type of range";
char ERR_no_constant_in_range_bound[]
= "range bound is not constant expression";
char ERR_range_bound_type[]
= "invalid range bound type";
char ERR_range_bound_value[]
= "low range bound > upper bound";
char ERR_no_type_in_procedure_result_type[]
= "error type in result type of function";
char ERR_no_type_in_formal_parameter_type[]
= "error type in formal parameter type definition";
char ERR_equal_operands[]
= "invalid operands of operation \'=\'";
char ERR_unequal_operands[]
= "invalid operands of operation \'#\'";
char ERR_less_than_operands[]
= "invalid operands of operation \'<\'";
char ERR_greater_than_operands[]
= "invalid operands of operation \'>\'";
char ERR_less_or_equal_or_set_inclusion_operands[]
= "invalid operands of operation \'<=\'";
char ERR_greater_or_equal_or_set_inclusion_operands[]
= "invalid operands of operation \'>=\'";
char ERR_in_operands[]
= "invalid operands of IN-operation";
char ERR_identity_operand[]
= "invalid operand of unary operation \'+\'";
char ERR_sign_inversion_operand[]
= "invalid operand of unary operation \'-\'";
char ERR_or_operands[]
= "invalid operands of OR-operation";
char ERR_and_operands[]
= "invalid operands of AND-operation";
char ERR_not_operand[]
= "invalid operand of NOT-operation";
char ERR_real_division_operands[]
= "invalid operands of operation \'/\'";
char ERR_addition_operands[]
= "invalid operands of operation \'+\'";
char ERR_subtraction_operands[]
= "invalid operands of opeartion \'-\'";
char ERR_mult_operands[]
= "invalid operands of operation \'*\'";
char ERR_div_operands[]
= "invalid operands of DIV-operation";
char ERR_mod_operands[]
= "invalid operands of MOD-operation";
char ERR_dereferencing_operands[]
= "invalid operand of operation \'^\'";
char ERR_unexported_identifier[]
= "identifier %s is not exported";
char ERR_record_variable[]
= "error variable before \'.\'";
char ERR_undeclared_field[]
= "field %s is absent in record";
char ERR_vnoarr[]
= "invalid operand of operation \'[]\'";
char ERR_index_expression[]
= "array index is not expression";
char ERR_index_expression_type[]
= "invalid type of array index";
char ERR_index_value[]
= "invalid value of array index";
char ERR_set_constructor_base_type[]
= "invalid base type of set constructor";
char ERR_set_element_expression[]
= "set element is not expression";
char ERR_set_element_expression_type[]
= "invalid type of set element";
char ERR_set_element_value[]
= "invalid value of set element";
char ERR_coroutines[]
= "coroutines on given machine was not implemented (proc. %s)";
char ERR_iotransfer[]
= "procedure IOTRANSER was not implemented";
char ERR_number_of_actual_parameters[]
= "invalid number of actual parameteres(proc. %s)";
char ERR_actual_parameter_is_not_type[]
= "actual parameter is not type(proc %s)";
char ERR_actual_parameter_type[]
= "invalid types of actual parameters of proc %s";
char ERR_actual_parameter_value[]
= "invalid value of actul parameter(proc %s)";
char ERR_actual_parameter_is_not_variable[]
= "actual parameter is not variable(proc. %s)";
char ERR_actual_parameter_is_not_variable_or_type[]
= "actual parameter is not type or variable(proc %s)";
char ERR_actual_parameter_is_not_array[]
= "actual parameter is not array(proc %s)";
char ERR_function_call[]
= "error function call(ident. %s)";
char ERR_return_without_expression[]
= "RETURN without expression in functon";
char ERR_return_with_expression[]
= "RETURN with expression is not in function";
char ERR_expression_after_return[]
= "error expression after RETURN";
char ERR_expression_type_after_return[]
= "invalid type of expression after RETURN";
char ERR_expression_value_after_return[]
= "invalid value of expression after RETURN";
char ERR_exit_statement[]
= "EXIT-statement is not in LOOP-statement";
char ERR_expression_after_while[]
= "nonboolean expression after WHILE";
char ERR_expression_after_until[]
= "nonboolean expression after UNTIL";
char ERR_expression_after_if[]
= "nonboolean expression after IF";
char ERR_expression_after_elsif[]
= "nonboolean expression after ELSIF";
char ERR_loop_control_variable[]
= "control variable of FOR-statement must be variable of simple type";
char ERR_start_value_or_limit_is_not_expression[]
= "start/limit loop values are not expression";
char ERR_types_of_loop_bounds[]
= "invalid or noncompatible types loop bounds";
char ERR_types_of_control_variable_and_bounds[]
= "noncompatible types of control variable and bounds of FOR-statement";
char ERR_loop_start_value[]
= "invalid value of loop start";
char ERR_nonconstant_loop_increment[]
= "loop increment is not constant";
char ERR_loop_increment_type[]
= "invalid type of loop increment";
char ERR_variable_after_with[]
= "error variable after WITH";
char ERR_expression_after_case[]
= "invalid expression after CASE";
char ERR_assignment_variable[]
= "error variable at left side of \':=\'";
char ERR_assignment_expression[]
= "error expression at right side of \':=\'";
char ERR_types_in_assignment[]
= "different types in assignment statement";
char ERR_local_procedure_assignment[]
= "local procedure is assigned";
char ERR_assignment_expression_value[]
= "invalid value of assignment expression";
char ERR_function_used_as_procedure[]
= "function %s in call statement";
char ERR_call_statement[]
= "invalid call statement";
char ERR_qualified_export_of_undeclared_identifier[]
= "qualified export of undeclared identifier %s";
char ERR_qualified_export_of_imported_identifier[]
= "qualified export of imported identifier %s";
char ERR_opaque_type_is_not_correct[]
= "opaque type %s is not correct type in implementation module";
char ERR_nonconstant_priority[]
= "priority of module %s is not constant";
char ERR_priority_type[]
= "invalid priority type of module %s";
char ERR_negative_priority[]
= "negative priority of module %s";
char ERR_procedure_absent_in_implementation_module[]
= "procedure %s is absent in implementation module";
char ERR_declarations_difference_in_corresponding_modules[]
= "different declarations of %s in implementation and definition module";


/* All fatal errors must be in this table. */

static char *fatal_error_table[] =
{
  ERR_no_memory,
  ERR_many_errors,
  ERR_input_file_opening,
  ERR_object_file_opening,
  ERR_output_to_object_file,
  ERR_file_closing,
  ERR_output_to_stdout,
  ERR_invalid_file_name,
  ERR_definition_module_absence,
  ERR_floating_point_exception,
  NULL				/* End marker */
};


/* This table contains system errors.  Occurrence of any fatal error
   from this table results in also perror call. */

static char *system_error_table[] =
{
  ERR_input_file_opening,
  ERR_object_file_opening,
  ERR_output_to_object_file,
  ERR_file_closing,
  ERR_output_to_stdout,
  ERR_definition_module_absence,
  NULL				/* End marker */
};


/* This variable contains function
   which will be called after fatal error occurrence. */

void (*finish_procedure) ();


/* This variable contains number of all errors occurred
   for translation of current module (and all definition modules
   implicitly and explicitly imported by it). */

int number_of_errors;


/* This variable is always set up to TRUE after any error occurrence. */

int new_error_flag;


/* Fatal error ERR_many_errors will be fixed after MAXIMUM_NUMBER_OF_ERRORS-th
   error occurrence for translation of current module. */

#define MAXIMUM_NUMBER_OF_ERRORS 50


/* Print error ERR with attribute PAR and call perror if the error is
   a system error.  Function contained in variable finish_procedure are called
   for fatal errors.  Fatal error ERR_many_errors will be fixed if the error is
   MAXIMUM_NUMBER_OF_ERRORS-th error for translation of current module. */

void
error_with_parameter (err, par)
     register char *err, *par;
{
  register int fatal_error_flag, system_error_flag;
  register char **perr;
  register int error_prefix_length;

  number_of_errors++;
  new_error_flag = TRUE;
  fatal_error_flag = FALSE;
  for (perr = fatal_error_table; *perr != 0; perr++)
    if (*perr == err)
      {
	fatal_error_flag = TRUE;
	break;
      }
  system_error_flag = FALSE;
  if (fatal_error_flag)
    for (perr = system_error_table; *perr != 0; perr++)
      if (*perr == err)
	{
	  system_error_flag = TRUE;
	  break;
	}
  if (!picking_names_of_imported_modules || fatal_error_flag)
    {
      /* Calculate output length in next fprintf. */
      error_prefix_length = strlen (input_file_name) + 2;
      fprintf (stderr, "\"%s\"", input_file_name);
      fprintf (stderr, ": %d: ",
	       (source_line_number == 0 ? 1 : source_line_number));
      if (fatal_error_flag)
	fprintf (stderr, "fatal error - ");
      fprintf (stderr, err, par);
      fputc ('\n', stderr);
      if (system_error_flag)
	{
	  while (--error_prefix_length != 0)
	    putc (' ', stderr);
	  perror (" ");
	}
    }
  if (fatal_error_flag)
    (*finish_procedure) (number_of_errors);
  if (number_of_errors == MAXIMUM_NUMBER_OF_ERRORS)
    error (ERR_many_errors);
}
