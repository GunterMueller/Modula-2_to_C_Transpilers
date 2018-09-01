/* Common definitions (work with errors) for all GNU Modula-2 translator files.
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

/* Fatal errors, i.e. the errors result in immediate translator finish. */

extern char ERR_no_memory[];
extern char ERR_many_errors[];
extern char ERR_input_file_opening[];
extern char ERR_object_file_opening[];
extern char ERR_output_to_object_file[];
extern char ERR_file_closing[];
extern char ERR_output_to_stdout[];
extern char ERR_invalid_file_name[];
extern char ERR_definition_module_absence[];
extern char ERR_floating_point_exception[];


/* Lexical errors. */

extern char ERR_octal_number[];
extern char ERR_decimal_number[];
extern char ERR_big_char_code[];
extern char ERR_order_absence[];
extern char ERR_comment_end_absence[];
extern char ERR_string_end_absence[];
extern char ERR_invalid_input_char[];
extern char ERR_C_code_end_absence[];


/* Semantic errors which are searched on syntax analysis pass. */

extern char ERR_standard_indentifier_declaration[];
extern char ERR_repeated_identifier_declaration[];


/* Syntactic errors. */

extern char ERR_symbol_equal_expectation[];
extern char ERR_in_constant_expression[];
extern char ERR_keyword_to_expectation[];
extern char ERR_in_index_type[];
extern char ERR_in_field_declaration[];
extern char ERR_keyword_of_expectation[];
extern char ERR_in_formal_parameter_declaration[];
extern char ERR_in_case_tag_type[];
extern char ERR_in_case_label_list[];
extern char ERR_in_record_case_part[];
extern char ERR_in_field_identifier_list[];
extern char ERR_in_enumeration_type[];
extern char ERR_in_simple_type[];
extern char ERR_in_range_type[];
extern char ERR_in_qualified_identifier[];
extern char ERR_in_variable_list[];
extern char ERR_in_type[];
extern char ERR_in_index_value[];
extern char ERR_in_expression[];
extern char ERR_in_set[];
extern char ERR_in_actual_parameters[];
extern char ERR_in_if_statement[];
extern char ERR_in_if_condition[];
extern char ERR_in_statement[];
extern char ERR_in_case_statement[];
extern char ERR_in_case_value[];
extern char ERR_in_while_condition[];
extern char ERR_identifier_expectation[];
extern char ERR_symbol_assignment_expectation[];
extern char ERR_in_for_statement_start_value[];
extern char ERR_in_for_statement_limit_or_increment[];
extern char ERR_in_with_statement_variable[];
extern char ERR_semicolon_expectation[];
extern char ERR_procedure_name_after_end[];
extern char ERR_in_export_list[];
extern char ERR_module_name_after_end[];
extern char ERR_in_block[];
extern char ERR_in_priority[];
extern char ERR_in_constant_declaration_part[];
extern char ERR_in_variable_declaration_part[];
extern char ERR_in_type_declaration_part[];
extern char ERR_in_procedure_head[];
extern char ERR_in_declaration_part[];
extern char ERR_in_formal_parameter_list[];
extern char ERR_in_declaration_of_formal_parameters[];
extern char ERR_keyword_import_expectation[];
extern char ERR_in_import_list[];
extern char ERR_in_compilation_unit_begin[];
extern char ERR_symbol_colon_expectation[];
extern char ERR_end_of_file_expectation[];


/* Semantic errors. */

extern char ERR_implicitly_exported_identifier_declaration[];
extern char ERR_implicitly_imported_identifier_declaration[];
extern char ERR_undeclared_identifier_export[];
extern char ERR_undeclared_identifier_import[];
extern char ERR_unexported_identifier_import[];
extern char ERR_import_from_undeclared_module[];
extern char ERR_no_module_after_from[];
extern char ERR_no_type_in_type_definition[];
extern char ERR_no_constant_expression_in_constant_definition[];
extern char ERR_type_in_variable_declaration[];
extern char ERR_actual_parameter_is_not_expression[];
extern char ERR_nonvariable_for_var_formal_parameter[];
extern char ERR_invalid_actual_parameter[];
extern char ERR_local_procedure_as_parameter[];
extern char ERR_nonconstant_case_label[];
extern char ERR_case_label_type[];
extern char ERR_case_label_types_inequality[];
extern char ERR_case_label_value_bounds[];
extern char ERR_case_label_and_tag_types_inequality[];
extern char ERR_repeated_case_label[];
extern char ERR_field_type[];
extern char ERR_case_tag_is_not_type[];
extern char ERR_record_case_tag_type[];
extern char ERR_undeclared_identifier[];
extern char ERR_declaration_cycle[];
extern char ERR_set_type[];
extern char ERR_set_base_type[];
extern char ERR_set_base_type_value[];
extern char ERR_no_type_in_pointer_type[];
extern char ERR_no_type_in_index_type[];
extern char ERR_no_type_in_array_element[];
extern char ERR_array_index_type[];
extern char ERR_range_base_type[];
extern char ERR_no_constant_in_range_bound[];
extern char ERR_range_bound_type[];
extern char ERR_range_bound_value[];
extern char ERR_no_type_in_procedure_result_type[];
extern char ERR_no_type_in_formal_parameter_type[];
extern char ERR_equal_operands[];
extern char ERR_unequal_operands[];
extern char ERR_less_than_operands[];
extern char ERR_greater_than_operands[];
extern char ERR_less_or_equal_or_set_inclusion_operands[];
extern char ERR_greater_or_equal_or_set_inclusion_operands[];
extern char ERR_in_operands[];
extern char ERR_identity_operand[];
extern char ERR_sign_inversion_operand[];
extern char ERR_or_operands[];
extern char ERR_and_operands[];
extern char ERR_not_operand[];
extern char ERR_real_division_operands[];
extern char ERR_addition_operands[];
extern char ERR_subtraction_operands[];
extern char ERR_mult_operands[];
extern char ERR_div_operands[];
extern char ERR_mod_operands[];
extern char ERR_dereferencing_operands[];
extern char ERR_unexported_identifier[];
extern char ERR_record_variable[];
extern char ERR_undeclared_field[];
extern char ERR_vnoarr[];
extern char ERR_index_expression[];
extern char ERR_index_expression_type[];
extern char ERR_index_value[];
extern char ERR_set_constructor_base_type[];
extern char ERR_set_element_expression[];
extern char ERR_set_element_expression_type[];
extern char ERR_set_element_value[];
extern char ERR_coroutines[];
extern char ERR_iotransfer[];
extern char ERR_number_of_actual_parameters[];
extern char ERR_actual_parameter_is_not_type[];
extern char ERR_actual_parameter_type[];
extern char ERR_actual_parameter_value[];
extern char ERR_actual_parameter_is_not_variable[];
extern char ERR_actual_parameter_is_not_variable_or_type[];
extern char ERR_actual_parameter_is_not_array[];
extern char ERR_function_call[];
extern char ERR_return_without_expression[];
extern char ERR_return_with_expression[];
extern char ERR_expression_after_return[];
extern char ERR_expression_type_after_return[];
extern char ERR_expression_value_after_return[];
extern char ERR_exit_statement[];
extern char ERR_expression_after_while[];
extern char ERR_expression_after_until[];
extern char ERR_expression_after_if[];
extern char ERR_expression_after_elsif[];
extern char ERR_loop_control_variable[];
extern char ERR_start_value_or_limit_is_not_expression[];
extern char ERR_types_of_loop_bounds[];
extern char ERR_types_of_control_variable_and_bounds[];
extern char ERR_loop_start_value[];
extern char ERR_nonconstant_loop_increment[];
extern char ERR_loop_increment_type[];
extern char ERR_variable_after_with[];
extern char ERR_expression_after_case[];
extern char ERR_assignment_variable[];
extern char ERR_assignment_expression[];
extern char ERR_types_in_assignment[];
extern char ERR_local_procedure_assignment[];
extern char ERR_assignment_expression_value[];
extern char ERR_function_used_as_procedure[];
extern char ERR_call_statement[];
extern char ERR_qualified_export_of_undeclared_identifier[];
extern char ERR_qualified_export_of_imported_identifier[];
extern char ERR_opaque_type_is_not_correct[];
extern char ERR_nonconstant_priority[];
extern char ERR_priority_type[];
extern char ERR_negative_priority[];
extern char ERR_procedure_absent_in_implementation_module[];
extern char ERR_declarations_difference_in_corresponding_modules[];

extern void (*finish_procedure) ();
extern int number_of_errors;
extern int new_error_flag;
extern void error_with_parameter ();

/* Macros is defined for convenience of fixing errors without parameters. */
#define error(err) error_with_parameter(err,0)
