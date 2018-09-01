/* Modula-2 standard enviroment and functions for work with intermidiate code.
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


/* Table of icode node characteristics.  The order of the table elements is
   the same as in enumeration icode_node_mode.

   The element has three members
     - size of corresponding node;
     - number of icode pointers in the node;
     - flag of that the node is denotation, i.e. the node has members
       identifier and scope. */

int characteristics_node_table[] =
{
  CHARACTERISTICS_NODE (sizeof (integer_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (cardinal_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (real_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (string_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (identifier_node), 0, FALSE),
  CHARACTERISTICS_NODE (sizeof (set_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (unary_operation_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (unary_operation_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (unary_operation_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (unary_operation_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (unary_operation_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (binary_operation_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (function_call_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (assignment_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (procedure_call_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (actual_parameter_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (block_begin_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (return_without_result_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (return_with_result_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (loop_statement_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (exit_statement_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (while_statement_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (repeat_statement_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (for_statement_node), 6, FALSE),
  CHARACTERISTICS_NODE (sizeof (if_statement_node), 4, FALSE),
  CHARACTERISTICS_NODE (sizeof (elsif_clause_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_statement_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (with_statement_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_variant_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_label_element_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_label_range_node), 4, FALSE),
  CHARACTERISTICS_NODE (sizeof (C_code_in_statements_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (C_code_in_declarations_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (export_node), 5, TRUE),
  CHARACTERISTICS_NODE (sizeof (qualified_export_node), 3, TRUE),
  CHARACTERISTICS_NODE (sizeof (import_node), 4, TRUE),
  CHARACTERISTICS_NODE (sizeof (field_node), 5, TRUE),
  CHARACTERISTICS_NODE (sizeof (formal_parameter_node), 4, TRUE),
  CHARACTERISTICS_NODE (sizeof (module_node), 9, TRUE),
  CHARACTERISTICS_NODE (sizeof (procedure_node), 6, TRUE),
  CHARACTERISTICS_NODE (sizeof (variable_declaration_node), 4, TRUE),
  CHARACTERISTICS_NODE (sizeof (constant_declaration_node), 4, TRUE),
  CHARACTERISTICS_NODE (sizeof (type_declaration_node), 4, TRUE),
  CHARACTERISTICS_NODE (sizeof (C_function_node), 3, TRUE),
  CHARACTERISTICS_NODE (sizeof (enumeration_type_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (range_type_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (set_type_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (pointer_type_node), 1, FALSE),
  CHARACTERISTICS_NODE (sizeof (array_type_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (procedure_type_node), 2, FALSE),
  CHARACTERISTICS_NODE (sizeof (procedure_parameter_type_node), 4, FALSE),
  CHARACTERISTICS_NODE (sizeof (record_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (record_case_part_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_variant_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_label_element_node), 3, FALSE),
  CHARACTERISTICS_NODE (sizeof (case_label_range_node), 4, FALSE),
  CHARACTERISTICS_NODE (sizeof (basic_type_node), 0, FALSE)
};


/* Icode nodes representing all Modula-2 standard environment objects. */


/* The following nodes fully describe standard identifiers.  ICN_ID is
   abbreviation of icode node identifier. */

static identifier_node ICN_ID_abs =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "ABS"};
static identifier_node ICN_ID_bitset =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "BITSET"};
static identifier_node ICN_ID_boolean =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "BOOLEAN"};
static identifier_node ICN_ID_cap =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "CAP"};
static identifier_node ICN_ID_cardinal =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "CARDINAL"};
static identifier_node ICN_ID_char =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "CHAR"};
static identifier_node ICN_ID_chr =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "CHR"};
static identifier_node ICN_ID_dec =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "DEC"};
static identifier_node ICN_ID_excl =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "EXCL"};
static identifier_node ICN_ID_false =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "FALSE"};
static identifier_node ICN_ID_float =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "FLOAT"};
static identifier_node ICN_ID_halt =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "HALT"};
static identifier_node ICN_ID_high =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "HIGH"};
static identifier_node ICN_ID_inc =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "INC"};
static identifier_node ICN_ID_incl =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "INCL"};
static identifier_node ICN_ID_integer =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "INTEGER"};
static identifier_node ICN_ID_longcard =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "LONGCARD"};
static identifier_node ICN_ID_longint =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "LONGINT"};
static identifier_node ICN_ID_longreal =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "LONGREAL"};
static identifier_node ICN_ID_max =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "MAX"};
static identifier_node ICN_ID_min =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "MIN"};
static identifier_node ICN_ID_nil =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "NIL"};
static identifier_node ICN_ID_odd =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "ODD"};
static identifier_node ICN_ID_ord =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "ORD"};
static identifier_node ICN_ID_proc =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "PROC"};
static identifier_node ICN_ID_real =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "REAL"};
static identifier_node ICN_ID_shrtcard =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "SHORTCARD"};
static identifier_node ICN_ID_shrtint =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "SHORTINT"};
static identifier_node ICN_ID_shrtreal =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "SHORTREAL"};
static identifier_node ICN_ID_size =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "SIZE"};
static identifier_node ICN_ID_true =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "TRUE"};
static identifier_node ICN_ID_trunc =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "TRUNC"};
static identifier_node ICN_ID_val =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "VAL"};
static identifier_node ICN_ID_system =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "SYSTEM"};
static identifier_node ICN_ID_address =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "ADDRESS"};
static identifier_node ICN_ID_adr =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "ADR"};
static identifier_node ICN_ID_byte =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "BYTE"};
static identifier_node ICN_ID_iotransfer =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "IOTRANSFER"};
static identifier_node ICN_ID_transfer =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "TRANSFER"};
static identifier_node ICN_ID_newprocess =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "NEWPROCESS"};
static identifier_node ICN_ID_tsize =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "TSIZE"};
static identifier_node ICN_ID_word =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "WORD"};
static identifier_node ICN_ID_cproc =
{(ICN_mode) ICNM_IDENTIFIER, FALSE, 0, "CPROC"};


/* The following nodes fully describes basic types.  ICN_TD is abbreviation of
   icode node type definition.  See also commentaries for enumeration
   basic_type. */

basic_type_node ICN_TD_short_cardinal =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_SHORT_CARDINAL};
basic_type_node ICN_TD_cardinal =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_CARDINAL};
basic_type_node ICN_TD_long_cardinal =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_LONG_CARDINAL};
basic_type_node ICN_TD_short_integer =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_SHORT_INTEGER};
basic_type_node ICN_TD_integer =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_INTEGER};
basic_type_node ICN_TD_long_integer =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_LONG_INTEGER};
basic_type_node ICN_TD_short_real =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_SHORT_REAL};
basic_type_node ICN_TD_real =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_REAL};
basic_type_node ICN_TD_long_real =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_LONG_REAL};
basic_type_node ICN_TD_char =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_CHAR};
basic_type_node ICN_TD_byte =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_BYTE};
basic_type_node ICN_TD_word =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_WORD};
basic_type_node ICN_TD_nil =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_NIL};
basic_type_node ICN_TD_short_cardinal_or_integer =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_SHORT_CARDINAL_OR_INTEGER};
basic_type_node ICN_TD_cardinal_or_integer =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_CARDINAL_OR_INTEGER};
basic_type_node ICN_TD_long_cardinal_or_integer =
{(ICN_mode) ICNM_BASIC_TYPE, FALSE, 0, BTM_LONG_CARDINAL_OR_INTEGER};


/* The following node fully describes type definition for ADDRESS. */

pointer_type_node ICN_TD_address =
{(ICN_mode) ICNM_POINTER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_TD_word)};


/* The following nodes fully describe type definition for BITSET and
   some type definitions and constant values corresponding to it.
   ICN_V is abbreviation of icode node value. */

cardinal_node ICN_V_min_short_cardinal =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_short_cardinal), 0};
static cardinal_node ICN_V_bitset_bound =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_short_cardinal),
   BITS_IN_SET - 1};
static range_type_node ICN_TD_bitset_range =
{(ICN_mode) ICNM_RANGE_TYPE, FALSE, 0, ICN_POINTER (&ICN_TD_short_cardinal),
   ICN_POINTER (&ICN_V_min_short_cardinal), ICN_POINTER (&ICN_V_bitset_bound)};
set_type_node ICN_TD_bitset =
{(ICN_mode) ICNM_SET_TYPE, FALSE, 0, ICN_POINTER (&ICN_TD_bitset_range)};


/* The following node fully describes type definition for PROC. */

static procedure_type_node ICN_TD_proc =
{(ICN_mode) ICNM_PROCEDURE_TYPE, FALSE, 0};


/* The following nodes fully describe type definitions and constant declaration
   corresponding to type BOOLEAN.  ICN_V is abbreviation of icode node
   value. */

cardinal_node ICN_V_false =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_boolean), 0};
cardinal_node ICN_V_true =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_boolean), 1};
static constant_declaration_node ICN_D_true =
{(ICN_mode) ICNM_CONSTANT_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_true), ICN_POINTER (&ICN_V_true)};
static constant_declaration_node ICN_D_false =
{(ICN_mode) ICNM_CONSTANT_DECLARATION, FALSE, 0, ICN_POINTER (&ICN_D_true),
   NULL, ICN_POINTER (&ICN_ID_false), ICN_POINTER (&ICN_V_false)};
enumeration_type_node ICN_TD_boolean =
{(ICN_mode) ICNM_ENUMERATION_TYPE, FALSE, 0, ICN_POINTER (&ICN_D_false)};


/* The following nodes fully describes the rest of constant values from
   language standard environment. */

static cardinal_node ICN_V_nil =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_nil), 0};
cardinal_node ICN_V_min_cardinal =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_cardinal), 0};
cardinal_node ICN_V_min_long_cardinal =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_long_cardinal), 0};
cardinal_node ICN_V_max_short_cardinal =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_short_cardinal),
   SHORTCARD_MAX};
cardinal_node ICN_V_max_cardinal =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_cardinal),
   CARDINAL_MAX};
cardinal_node ICN_V_max_long_cardinal =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_long_cardinal),
   LONGCARD_MAX};
integer_node ICN_V_min_short_integer =
{(ICN_mode) ICNM_INTEGER, FALSE, 0, ICN_POINTER (&ICN_TD_short_integer),
   SHORTINT_MIN};
integer_node ICN_V_min_integer =
{(ICN_mode) ICNM_INTEGER, FALSE, 0, ICN_POINTER (&ICN_TD_integer),
   INTEGER_MIN};
integer_node ICN_V_min_long_integer =
{(ICN_mode) ICNM_INTEGER, FALSE, 0, ICN_POINTER (&ICN_TD_long_integer),
   LONGINT_MIN};
integer_node ICN_V_max_short_integer =
{(ICN_mode) ICNM_INTEGER, FALSE, 0, ICN_POINTER (&ICN_TD_short_integer),
   SHORTINT_MAX};
integer_node ICN_V_max_integer =
{(ICN_mode) ICNM_INTEGER, FALSE, 0, ICN_POINTER (&ICN_TD_integer),
   INTEGER_MAX};
integer_node ICN_V_max_long_integer =
{(ICN_mode) ICNM_INTEGER, FALSE, 0, ICN_POINTER (&ICN_TD_long_integer),
   LONGINT_MAX};


/* The following icode real nodes are not initiated because maximum and minimum
   real values in the file float.h may be not C constant expression (see
   ANSI standard of C language). */

real_node ICN_V_min_short_real;
real_node ICN_V_min_real;
real_node ICN_V_min_long_real;
real_node ICN_V_max_short_real;
real_node ICN_V_max_real;
real_node ICN_V_max_long_real;

cardinal_node ICN_V_min_char =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_char), 0};
cardinal_node ICN_V_max_char =
{(ICN_mode) ICNM_CARDINAL, FALSE, 0, ICN_POINTER (&ICN_TD_char),
   CHARACTER_MAX};


/* The following node partially describes declaration of ABS. */

procedure_node ICN_D_abs =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_abs)};


/* The following node fully describes declaration of BITSET. */

static type_declaration_node ICN_D_bitset =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_bitset), ICN_POINTER (&ICN_TD_bitset)};


/* The following node fully describes declaration of BOOLEAN. */

static type_declaration_node ICN_D_boolean =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, ICN_POINTER (&ICN_V_false), NULL,
   ICN_POINTER (&ICN_ID_boolean), ICN_POINTER (&ICN_TD_boolean)};


/* The following node partially describes declaration of CAP. */

procedure_node ICN_D_cap =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_cap)};


/* The following node fully describes declaration of CARDINAL. */

static type_declaration_node ICN_D_cardinal =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_cardinal), ICN_POINTER (&ICN_TD_cardinal)};


/* The following node fully describes declaration of CHAR. */

static type_declaration_node ICN_D_char =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_char), ICN_POINTER (&ICN_TD_char)};


/* The following node partially describes declaration of CHR. */

procedure_node ICN_D_chr =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_chr)};


/* The following node partially describes declaration of DEC. */

procedure_node ICN_D_dec =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_dec)};


/* The following node partially describes declaration of EXCL. */

procedure_node ICN_D_excl =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_excl)};


/* The following node fully describes declaration of FLOAT. */

procedure_node ICN_D_float =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_float)};


/* The following node partially describes declaration of HALT. */

procedure_node ICN_D_halt =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_halt)};


/* The following node partially describes declaration of HIGH. */

procedure_node ICN_D_high =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_high)};


/* The following node partially describes declaration of INC. */

procedure_node ICN_D_inc =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_inc)};


/* The following node partially describes declaration of INCL. */

procedure_node ICN_D_incl =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_incl)};


/* The following node fully describes declaration of INTEGER. */

static type_declaration_node ICN_D_integer =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_integer), ICN_POINTER (&ICN_TD_integer)};


/* The following node fully describes declaration of LONGCARD. */

static type_declaration_node ICN_D_longcard =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_longcard), ICN_POINTER (&ICN_TD_long_cardinal)};


/* The following node fully describes declaration of LONGINT. */

static type_declaration_node ICN_D_longint =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_longint), ICN_POINTER (&ICN_TD_long_integer)};


/* The following node fully describes declaration of LONGREAL. */

static type_declaration_node ICN_D_longreal =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_longreal), ICN_POINTER (&ICN_TD_long_real)};


/* The following node partially describes declaration of MAX. */

procedure_node ICN_D_max =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_max)};


/* The following node partially describes declaration of MIN. */

procedure_node ICN_D_min =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_min)};


/* The following node fully describes declaration of NIL. */

static constant_declaration_node ICN_D_nil =
{(ICN_mode) ICNM_CONSTANT_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_nil), ICN_POINTER (&ICN_V_nil)};


/* The following node partially describes declaration of ODD. */

procedure_node ICN_D_odd =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_odd)};


/* The following node partially describes declaration of ORD. */

procedure_node ICN_D_ord =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_ord)};


/* The following node partially describes declaration of PROC. */

static type_declaration_node ICN_D_proc =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_proc), ICN_POINTER (&ICN_TD_proc)};


/* The following node fully describes declaration of REAL. */

static type_declaration_node ICN_D_real =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_real), ICN_POINTER (&ICN_TD_real)};


/* The following node fully describes declaration of SHORTCARD. */

static type_declaration_node ICN_D_shortcard =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_shrtcard), ICN_POINTER (&ICN_TD_short_cardinal)};


/* The following node fully describes declaration of SHORTINT. */

static type_declaration_node ICN_D_shortint =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_shrtint), ICN_POINTER (&ICN_TD_short_integer)};


/* The following node fully describes declaration of SHORTREAL. */

static type_declaration_node ICN_D_shortreal =
{(ICN_mode) ICNM_TYPE_DECLARATION, FALSE, 0, NULL, NULL,
   ICN_POINTER (&ICN_ID_shrtreal), ICN_POINTER (&ICN_TD_short_real)};


/* The following node partially describes declaration of SIZE. */

procedure_node ICN_D_size =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_size)};


/* The following node partially describes declaration of TRUNC. */

procedure_node ICN_D_trunc =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_trunc)};


/* The following node partially describes declaration of VAL. */

procedure_node ICN_D_val =
{(ICN_mode) ICNM_PROCEDURE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_val)};



/* The following nodes fully describe nodes used by declaration of
   IOTRANSFER from module SYSTEM.  ICN_PT is abbreviation of icode node
   for parameter type. */

static procedure_parameter_type_node ICN_PT_iotransfer_3 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0, NULL, NULL, NULL,
   ICN_POINTER (&ICN_TD_cardinal)};
static procedure_parameter_type_node ICN_PT_iotransfer_2 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_PT_iotransfer_3), NULL, NULL,
   ICN_POINTER (&ICN_TD_address), TRUE};
static procedure_parameter_type_node ICN_PT_iotransfer_1 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_PT_iotransfer_2), NULL, NULL,
   ICN_POINTER (&ICN_TD_address), TRUE};
static procedure_type_node ICN_TD_iotransfer =
{(ICN_mode) ICNM_PROCEDURE_TYPE, FALSE, 0, ICN_POINTER (&ICN_PT_iotransfer_1)};


/* The following nodes fully describe nodes used by declaration of
   TRANSFER from module SYSTEM. */

static procedure_parameter_type_node ICN_PT_transfer_2 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0, NULL, NULL, NULL,
   ICN_POINTER (&ICN_TD_address), TRUE};
static procedure_parameter_type_node ICN_PT_transfer_1 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_PT_transfer_2), NULL, NULL, ICN_POINTER (&ICN_TD_address),
   TRUE};
static procedure_type_node ICN_TD_transfer =
{(ICN_mode) ICNM_PROCEDURE_TYPE, FALSE, 0, ICN_POINTER (&ICN_PT_transfer_1)};


/* The following nodes fully describe nodes used by declaration of
   NEWPROCESS from module SYSTEM. */

static procedure_parameter_type_node ICN_PT_newprocess_4 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0, NULL, NULL, NULL,
   ICN_POINTER (&ICN_TD_address), TRUE};
static procedure_parameter_type_node ICN_PT_newprocess_3 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_PT_newprocess_4), NULL, NULL,
   ICN_POINTER (&ICN_TD_cardinal)};
static procedure_parameter_type_node ICN_PT_newprocess_2 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_PT_newprocess_3), NULL, NULL,
   ICN_POINTER (&ICN_TD_address)};
static procedure_parameter_type_node ICN_PT_newprocess_1 =
{(ICN_mode) ICNM_PROCEDURE_PARAMETER_TYPE, FALSE, 0,
   ICN_POINTER (&ICN_PT_newprocess_2), NULL, NULL, ICN_POINTER (&ICN_TD_proc)};
static procedure_type_node ICN_TD_newprocessr =
{(ICN_mode) ICNM_PROCEDURE_TYPE, FALSE, 0, ICN_POINTER (&ICN_PT_newprocess_1)};


/* The following nodes fully describe module SYSTEM and declaration nodes
   in it. */

module_node ICN_D_system =
{(ICN_mode) ICNM_MODULE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_system),
   NULL, NULL, NULL, NULL, NULL, NULL,
   (char) MM_DEFINITION_WITH_IMPLEMENTATION_MODULE};
static type_declaration_node ICN_D_address =
{(ICN_mode) ICNM_TYPE_DECLARATION, TRUE, 0,
   NULL, ICN_POINTER (&ICN_D_system), ICN_POINTER (&ICN_ID_address), 
   ICN_POINTER (&ICN_TD_address)};


/* The following node partially describes declaration of ADR. */

procedure_node ICN_D_adr =
{(ICN_mode) ICNM_PROCEDURE, TRUE, 0, NULL, ICN_POINTER (&ICN_D_system),
   ICN_POINTER (&ICN_ID_adr)};


/* The following node fully describes declaration of BYTE. */

static type_declaration_node ICN_D_byte =
{(ICN_mode) ICNM_TYPE_DECLARATION, TRUE, 0, NULL, ICN_POINTER (&ICN_D_system),
   ICN_POINTER (&ICN_ID_byte), ICN_POINTER (&ICN_TD_byte)};
procedure_node ICN_D_iotransfer =
{(ICN_mode) ICNM_PROCEDURE, TRUE, 0,
   NULL, ICN_POINTER (&ICN_D_system), ICN_POINTER (&ICN_ID_iotransfer), NULL, 
   ICN_POINTER (&ICN_TD_iotransfer)};
procedure_node ICN_D_transfer =
{(ICN_mode) ICNM_PROCEDURE, TRUE, 0,
   NULL, ICN_POINTER (&ICN_D_system), ICN_POINTER (&ICN_ID_transfer), NULL, 
   ICN_POINTER (&ICN_TD_transfer)};
procedure_node ICN_D_newprocess =
{(ICN_mode) ICNM_PROCEDURE, TRUE, 0,
   NULL, ICN_POINTER (&ICN_D_system), ICN_POINTER (&ICN_ID_newprocess), NULL, 
   ICN_POINTER (&ICN_TD_newprocessr)};


/* The following node partially describes declaration of TSIZE. */

procedure_node ICN_D_tsize =
{(ICN_mode) ICNM_PROCEDURE, TRUE, 0, NULL, ICN_POINTER (&ICN_D_system),
   ICN_POINTER (&ICN_ID_tsize)};


/* The following node fully describes declaration of WORD. */

static type_declaration_node ICN_D_word =
{(ICN_mode) ICNM_TYPE_DECLARATION, TRUE, 0, NULL, ICN_POINTER (&ICN_D_system),
   ICN_POINTER (&ICN_ID_word), ICN_POINTER (&ICN_TD_word)};


/* The following node partially describe module CPROC.  Nodes representing
   C functions will be create in it. */

module_node ICN_D_cproc =
{(ICN_mode) ICNM_MODULE, FALSE, 0, NULL, NULL, ICN_POINTER (&ICN_ID_cproc),
   NULL, NULL, NULL, NULL, NULL, NULL,
   (char) MM_DEFINITION_WITH_IMPLEMENTATION_MODULE};


/* The following table contains pointers to nodes which will be included in
   the hash table. */

static ICN_pointer nodes_for_hash_table[] =
{
  ICN_POINTER (&ICN_ID_abs), ICN_POINTER (&ICN_ID_bitset),
  ICN_POINTER (&ICN_ID_boolean), ICN_POINTER (&ICN_ID_cap),
  ICN_POINTER (&ICN_ID_cardinal), ICN_POINTER (&ICN_ID_char),
  ICN_POINTER (&ICN_ID_chr), ICN_POINTER (&ICN_ID_dec),
  ICN_POINTER (&ICN_ID_excl),
  ICN_POINTER (&ICN_ID_false), ICN_POINTER (&ICN_ID_float),
  ICN_POINTER (&ICN_ID_halt), ICN_POINTER (&ICN_ID_high),
  ICN_POINTER (&ICN_ID_inc), ICN_POINTER (&ICN_ID_incl),
  ICN_POINTER (&ICN_ID_integer), ICN_POINTER (&ICN_ID_longcard),
  ICN_POINTER (&ICN_ID_longint), ICN_POINTER (&ICN_ID_longreal),
  ICN_POINTER (&ICN_ID_max), ICN_POINTER (&ICN_ID_min),
  ICN_POINTER (&ICN_ID_nil), ICN_POINTER (&ICN_ID_odd),
  ICN_POINTER (&ICN_ID_ord), ICN_POINTER (&ICN_ID_proc),
  ICN_POINTER (&ICN_ID_real),
  ICN_POINTER (&ICN_ID_shrtcard), ICN_POINTER (&ICN_ID_shrtint),
  ICN_POINTER (&ICN_ID_shrtreal),
  ICN_POINTER (&ICN_ID_true), ICN_POINTER (&ICN_ID_trunc),
  ICN_POINTER (&ICN_ID_size), ICN_POINTER (&ICN_ID_val),
  ICN_POINTER (&ICN_ID_system), ICN_POINTER (&ICN_ID_address),
  ICN_POINTER (&ICN_ID_adr), ICN_POINTER (&ICN_ID_byte),
  ICN_POINTER (&ICN_ID_iotransfer), ICN_POINTER (&ICN_ID_transfer),
  ICN_POINTER (&ICN_ID_newprocess),
  ICN_POINTER (&ICN_ID_tsize), ICN_POINTER (&ICN_ID_word),
  ICN_POINTER (&ICN_ID_cproc),
  ICN_POINTER (&ICN_TD_short_cardinal), ICN_POINTER (&ICN_TD_cardinal),
  ICN_POINTER (&ICN_TD_long_cardinal), ICN_POINTER (&ICN_TD_short_integer),
  ICN_POINTER (&ICN_TD_integer), ICN_POINTER (&ICN_TD_long_integer),
  ICN_POINTER (&ICN_TD_short_real),
  ICN_POINTER (&ICN_TD_real), ICN_POINTER (&ICN_TD_long_real),
  ICN_POINTER (&ICN_TD_char), ICN_POINTER (&ICN_TD_byte),
  ICN_POINTER (&ICN_TD_word),
  ICN_POINTER (&ICN_TD_nil), ICN_POINTER (&ICN_TD_short_cardinal_or_integer),
  ICN_POINTER (&ICN_TD_cardinal_or_integer),
  ICN_POINTER (&ICN_TD_long_cardinal_or_integer),
  ICN_POINTER (&ICN_V_min_short_cardinal), ICN_POINTER (&ICN_V_bitset_bound),
  ICN_POINTER (&ICN_V_false),
  ICN_POINTER (&ICN_V_true), ICN_POINTER (&ICN_D_true),
  ICN_POINTER (&ICN_D_false), ICN_POINTER (&ICN_V_nil),
  ICN_POINTER (&ICN_V_min_cardinal), ICN_POINTER (&ICN_V_min_long_cardinal),
  ICN_POINTER (&ICN_V_max_short_cardinal),
  ICN_POINTER (&ICN_V_max_cardinal), ICN_POINTER (&ICN_V_max_long_cardinal),
  ICN_POINTER (&ICN_V_min_short_integer),
  ICN_POINTER (&ICN_V_min_integer), ICN_POINTER (&ICN_V_min_long_integer),
  ICN_POINTER (&ICN_V_max_short_integer),
  ICN_POINTER (&ICN_V_max_integer), ICN_POINTER (&ICN_V_max_long_integer),
  ICN_POINTER (&ICN_V_min_short_real), ICN_POINTER (&ICN_V_min_real),
  ICN_POINTER (&ICN_V_min_long_real), ICN_POINTER (&ICN_V_max_short_real),
  ICN_POINTER (&ICN_V_max_real),
  ICN_POINTER (&ICN_V_max_long_real), ICN_POINTER (&ICN_V_min_char),
  ICN_POINTER (&ICN_V_max_char),
  ICN_POINTER (&ICN_D_abs), ICN_POINTER (&ICN_D_bitset),
  ICN_POINTER (&ICN_D_boolean), ICN_POINTER (&ICN_D_cap),
  ICN_POINTER (&ICN_D_cardinal), ICN_POINTER (&ICN_D_char),
  ICN_POINTER (&ICN_D_chr),
  ICN_POINTER (&ICN_D_dec), ICN_POINTER (&ICN_D_excl),
  ICN_POINTER (&ICN_D_float), ICN_POINTER (&ICN_D_halt),
  ICN_POINTER (&ICN_D_high), ICN_POINTER (&ICN_D_inc),
  ICN_POINTER (&ICN_D_incl), ICN_POINTER (&ICN_D_integer),
  ICN_POINTER (&ICN_D_longcard),
  ICN_POINTER (&ICN_D_longint), ICN_POINTER (&ICN_D_longreal),
  ICN_POINTER (&ICN_D_max), ICN_POINTER (&ICN_D_min),
  ICN_POINTER (&ICN_D_nil), ICN_POINTER (&ICN_D_ord),
  ICN_POINTER (&ICN_D_odd), ICN_POINTER (&ICN_D_proc),
  ICN_POINTER (&ICN_D_real),
  ICN_POINTER (&ICN_D_shortcard), ICN_POINTER (&ICN_D_shortint),
  ICN_POINTER (&ICN_D_shortreal),
  ICN_POINTER (&ICN_D_size), ICN_POINTER (&ICN_D_trunc),
  ICN_POINTER (&ICN_D_val),
  ICN_POINTER (&ICN_D_system), ICN_POINTER (&ICN_D_address),
  ICN_POINTER (&ICN_D_adr),
  ICN_POINTER (&ICN_D_byte), ICN_POINTER (&ICN_D_iotransfer),
  ICN_POINTER (&ICN_D_transfer), ICN_POINTER (&ICN_D_newprocess),
  ICN_POINTER (&ICN_D_tsize), ICN_POINTER (&ICN_D_word),
  ICN_POINTER (&ICN_D_cproc),
  NULL			/* end marker for elements */
};



/* Function for search of standard identifier and geting its characteristics.

   If string STR (with end marker '\0') with length LENGTH is
   a standard identifier then return its characteristic else return 0.

   Fast search is implemented by minimal pruned O-trie forest. */

struct stdid *
find_stdid (str, length)
     register char *str;
     register int length;
{
  switch (length)
    {
    case 3:
      switch (*str)
	{
	case 'A':
	  {
	    static struct stdid std =
	    {STDID_ABS, FALSE, FALSE};
	    return (strcmp (str, "ABS") == 0 ? &std : 0);
	  }
	case 'a':
	  {
	    static struct stdid std =
	    {STDID_ABS, TRUE, FALSE};
	    return (strcmp (str, "abs") == 0 ? &std : 0);
	  }
	case 'C':
	  if (str[1] == 'A')
	    {
	      static struct stdid std =
	      {STDID_CAP, FALSE, FALSE};
	      return (strcmp (str, "CAP") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_CHR, FALSE, FALSE};
	      return (strcmp (str, "CHR") == 0 ? &std : 0);
	    }
	case 'c':
	  if (str[1] == 'a')
	    {
	      static struct stdid std =
	      {STDID_CAP, TRUE, FALSE};
	      return (strcmp (str, "cap") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_CHR, TRUE, FALSE};
	      return (strcmp (str, "chr") == 0 ? &std : 0);
	    }
	case 'D':
	  {
	    static struct stdid std =
	    {STDID_DEC, FALSE, FALSE};
	    return (strcmp (str, "DEC") == 0 ? &std : 0);
	  }
	case 'd':
	  {
	    static struct stdid std =
	    {STDID_DEC, TRUE, FALSE};
	    return (strcmp (str, "dec") == 0 ? &std : 0);
	  }
	case 'I':
	  {
	    static struct stdid std =
	    {STDID_INC, FALSE, FALSE};
	    return (strcmp (str, "INC") == 0 ? &std : 0);
	  }
	case 'i':
	  {
	    static struct stdid std =
	    {STDID_INC, TRUE, FALSE};
	    return (strcmp (str, "inc") == 0 ? &std : 0);
	  }
	case 'M':
	  if (str[1] == 'A')
	    {
	      static struct stdid std =
	      {STDID_MAX, FALSE, FALSE};
	      return (strcmp (str, "MAX") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_MIN, FALSE, FALSE};
	      return (strcmp (str, "MIN") == 0 ? &std : 0);
	    }
	case 'm':
	  if (str[1] == 'a')
	    {
	      static struct stdid std =
	      {STDID_MAX, TRUE, FALSE};
	      return (strcmp (str, "max") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_MIN, TRUE, FALSE};
	      return (strcmp (str, "min") == 0 ? &std : 0);
	    }
	case 'N':
	  {
	    static struct stdid std =
	    {STDID_NIL, FALSE, FALSE};
	    return (strcmp (str, "NIL") == 0 ? &std : 0);
	  }
	case 'n':
	  {
	    static struct stdid std =
	    {STDID_NIL, TRUE, FALSE};
	    return (strcmp (str, "nil") == 0 ? &std : 0);
	  }
	case 'O':
	  if (str[1] == 'D')
	    {
	      static struct stdid std =
	      {STDID_ODD, FALSE, FALSE};
	      return (strcmp (str, "ODD") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_ORD, FALSE, FALSE};
	      return (strcmp (str, "ORD") == 0 ? &std : 0);
	    }
	case 'o':
	  if (str[1] == 'd')
	    {
	      static struct stdid std =
	      {STDID_ODD, TRUE, FALSE};
	      return (strcmp (str, "odd") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_ORD, TRUE, FALSE};
	      return (strcmp (str, "ord") == 0 ? &std : 0);
	    }
	case 'V':
	  {
	    static struct stdid std =
	    {STDID_VAL, FALSE, FALSE};
	    return (strcmp (str, "VAL") == 0 ? &std : 0);
	  }
	case 'v':
	  {
	    static struct stdid std =
	    {STDID_VAL, TRUE, FALSE};
	    return (strcmp (str, "val") == 0 ? &std : 0);
	  }
	default:
	  return 0;
	}
    case 4:
      switch (*str)
	{
	case 'C':
	  {
	    static struct stdid std =
	    {STDID_CHAR, FALSE, FALSE};
	    return (strcmp (str, "CHAR") == 0 ? &std : 0);
	  }
	case 'c':
	  {
	    static struct stdid std =
	    {STDID_CHAR, TRUE, FALSE};
	    return (strcmp (str, "char") == 0 ? &std : 0);
	  }
	case 'E':
	  {
	    static struct stdid std =
	    {STDID_EXCL, FALSE, FALSE};
	    return (strcmp (str, "EXCL") == 0 ? &std : 0);
	  }
	case 'e':
	  {
	    static struct stdid std =
	    {STDID_EXCL, TRUE, FALSE};
	    return (strcmp (str, "excl") == 0 ? &std : 0);
	  }
	case 'H':
	  if (str[1] == 'A')
	    {
	      static struct stdid std =
	      {STDID_HALT, FALSE, FALSE};
	      return (strcmp (str, "HALT") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_HIGH, FALSE, FALSE};
	      return (strcmp (str, "HIGH") == 0 ? &std : 0);
	    }
	case 'h':
	  if (str[1] == 'a')
	    {
	      static struct stdid std =
	      {STDID_HALT, TRUE, FALSE};
	      return (strcmp (str, "halt") == 0 ? &std : 0);
	    }
	  else
	    {
	      static struct stdid std =
	      {STDID_HIGH, TRUE, FALSE};
	      return (strcmp (str, "high") == 0 ? &std : 0);
	    }
	case 'I':
	  {
	    static struct stdid std =
	    {STDID_INCL, FALSE, FALSE};
	    return (strcmp (str, "INCL") == 0 ? &std : 0);
	  }
	case 'i':
	  {
	    static struct stdid std =
	    {STDID_INCL, TRUE, FALSE};
	    return (strcmp (str, "incl") == 0 ? &std : 0);
	  }
	case 'P':
	  {
	    static struct stdid std =
	    {STDID_PROC, FALSE, FALSE};
	    return (strcmp (str, "PROC") == 0 ? &std : 0);
	  }
	case 'p':
	  {
	    static struct stdid std =
	    {STDID_PROC, TRUE, FALSE};
	    return (strcmp (str, "proc") == 0 ? &std : 0);
	  }
	case 'R':
	  {
	    static struct stdid std =
	    {STDID_REAL, FALSE, FALSE};
	    return (strcmp (str, "REAL") == 0 ? &std : 0);
	  }
	case 'r':
	  {
	    static struct stdid std =
	    {STDID_REAL, TRUE, FALSE};
	    return (strcmp (str, "real") == 0 ? &std : 0);
	  }
	case 'S':
	  {
	    static struct stdid std =
	    {STDID_SIZE, FALSE, FALSE};
	    return (strcmp (str, "SIZE") == 0 ? &std : 0);
	  }
	case 's':
	  {
	    static struct stdid std =
	    {STDID_SIZE, TRUE, FALSE};
	    return (strcmp (str, "size") == 0 ? &std : 0);
	  }
	case 'T':
	  {
	    static struct stdid std =
	    {STDID_TRUE, FALSE, FALSE};
	    return (strcmp (str, "TRUE") == 0 ? &std : 0);
	  }
	case 't':
	  {
	    static struct stdid std =
	    {STDID_TRUE, TRUE, FALSE};
	    return (strcmp (str, "true") == 0 ? &std : 0);
	  }
	default:
	  return 0;
	}
    case 5:
      switch (str[1])
	{
	case 'A':
	  {
	    static struct stdid std =
	    {STDID_FALSE, FALSE, FALSE};
	    return (strcmp (str, "FALSE") == 0 ? &std : 0);
	  }
	case 'a':
	  {
	    static struct stdid std =
	    {STDID_FALSE, TRUE, FALSE};
	    return (strcmp (str, "false") == 0 ? &std : 0);
	  }
	case 'L':
	  {
	    static struct stdid std =
	    {STDID_FLOAT, FALSE, FALSE};
	    return (strcmp (str, "FLOAT") == 0 ? &std : 0);
	  }
	case 'l':
	  {
	    static struct stdid std =
	    {STDID_FLOAT, TRUE, FALSE};
	    return (strcmp (str, "float") == 0 ? &std : 0);
	  }
	case 'R':
	  {
	    static struct stdid std =
	    {STDID_TRUNC, FALSE, FALSE};
	    return (strcmp (str, "TRUNC") == 0 ? &std : 0);
	  }
	case 'r':
	  {
	    static struct stdid std =
	    {STDID_TRUNC, TRUE, FALSE};
	    return (strcmp (str, "trunc") == 0 ? &std : 0);
	  }
	default:
	  return 0;
	}
    case 6:
      if (*str == 'B')
	{
	  static struct stdid std =
	  {STDID_BITSET, FALSE, FALSE};
	  return (strcmp (str, "BITSET") == 0 ? &std : 0);
	}
      else
	{
	  static struct stdid std =
	  {STDID_BITSET, TRUE, FALSE};
	  return (strcmp (str, "bitset") == 0 ? &std : 0);
	}
    case 7:
      switch (*str)
	{
	case 'B':
	  {
	    static struct stdid std =
	    {STDID_BOOLEAN, FALSE, FALSE};
	    return (strcmp (str, "BOOLEAN") == 0 ? &std : 0);
	  }
	case 'b':
	  {
	    static struct stdid std =
	    {STDID_BOOLEAN, TRUE, FALSE};
	    return (strcmp (str, "boolean") == 0 ? &std : 0);
	  }
	case 'I':
	  {
	    static struct stdid std =
	    {STDID_INTEGER, FALSE, FALSE};
	    return (strcmp (str, "INTEGER") == 0 ? &std : 0);
	  }
	case 'i':
	  {
	    static struct stdid std =
	    {STDID_INTEGER, TRUE, FALSE};
	    return (strcmp (str, "integer") == 0 ? &std : 0);
	  }
	case 'L':
	  {
	    static struct stdid std =
	    {STDID_LONGINT, FALSE, FALSE};
	    return (strcmp (str, "LONGINT") == 0 ? &std : 0);
	  }
	case 'l':
	  {
	    static struct stdid std =
	    {STDID_LONGINT, TRUE, FALSE};
	    return (strcmp (str, "longint") == 0 ? &std : 0);
	  }
	default:
	  return 0;
	}
    case 8:
      switch (str[4])
	{
	case 'C':
	  {
	    static struct stdid std =
	    {STDID_LONGCARD, FALSE, TRUE};
	    return (strcmp (str, "LONGCARD") == 0 ? &std : 0);
	  }
	case 'c':
	  {
	    static struct stdid std =
	    {STDID_LONGCARD, TRUE, TRUE};
	    return (strcmp (str, "longcard") == 0 ? &std : 0);
	  }
	case 'I':
	  {
	    static struct stdid std =
	    {STDID_CARDINAL, FALSE, FALSE};
	    return (strcmp (str, "CARDINAL") == 0 ? &std : 0);
	  }
	case 'i':
	  {
	    static struct stdid std =
	    {STDID_CARDINAL, TRUE, FALSE};
	    return (strcmp (str, "cardinal") == 0 ? &std : 0);
	  }
	case 'R':
	  {
	    static struct stdid std =
	    {STDID_LONGREAL, FALSE, FALSE};
	    return (strcmp (str, "LONGREAL") == 0 ? &std : 0);
	  }
	case 'r':
	  {
	    static struct stdid std =
	    {STDID_LONGREAL, TRUE, FALSE};
	    return (strcmp (str, "longreal") == 0 ? &std : 0);
	  }
	case 'T':
	  {
	    static struct stdid std =
	    {STDID_SHORTINT, FALSE, TRUE};
	    return (strcmp (str, "SHORTINT") == 0 ? &std : 0);
	  }
	case 't':
	  {
	    static struct stdid std =
	    {STDID_SHORTINT, TRUE, TRUE};
	    return (strcmp (str, "shortint") == 0 ? &std : 0);
	  }
	default:
	  return 0;
	}
    case 9:
      switch (str[5])
	{
	case 'C':
	  {
	    static struct stdid std =
	    {STDID_SHORTCARD, FALSE, TRUE};
	    return (strcmp (str, "SHORTCARD") == 0 ? &std : 0);
	  }
	case 'c':
	  {
	    static struct stdid std =
	    {STDID_SHORTCARD, TRUE, TRUE};
	    return (strcmp (str, "shortcard") == 0 ? &std : 0);
	  }
	case 'R':
	  {
	    static struct stdid std =
	    {STDID_SHORTREAL, FALSE, TRUE};
	    return (strcmp (str, "SHORTREAL") == 0 ? &std : 0);
	  }
	case 'r':
	  {
	    static struct stdid std =
	    {STDID_SHORTREAL, TRUE, TRUE};
	    return (strcmp (str, "shortreal") == 0 ? &std : 0);
	  }
	default:
	  return 0;
	}
    default:
      return 0;
    }
}



/* Functions for allocation and deallocation of icode nodes. */



/* Standard size of segment of memory allocated for icode nodes. */

#define STANDARD_SEGMENT_SIZE 0100000


/* Align of icode nodes.  It is important for hash table that
   node address will be even (see commentaries for the symbol hash table). */

#define REAL_ALIGN  ((MAX_ALIGNMENT+1)/2*2)


/* Size of memory allocated for the node with size s. */

#define AllocSize(s) ((s)+MAX_ALIGNMENT-1)/MAX_ALIGNMENT*MAX_ALIGNMENT

/* All memory used for icode nodes is divided on segments.  All existing
   segments are named by pool.  If no free memory is in current segment
   than new segment is allocated. */

struct segment
{
  struct segment *next, *pred;	/* for two-link list of segments */
  unsigned segment_length;	/* real length of the following array */
  char *free;			/* first free byte in given segment */
  char segment_memory[1];	/* Memory of icode nodes starts here. */
};


/* Pointer to segment in which memory for icode nodes is allocated now.
   It always refers to the last segment in list of the segments.  If
   CURRENT_SEGMENT is NULL than no segments exist. */

static struct segment *current_segment;


/* Maximum number of nodes creating after the last call of initiate_pool. */

unsigned int max_number_of_nodes;


/* Current number of nodes in segments. */

unsigned int current_number_of_nodes;


/* Maximum number of bytes of memory for nodes creating after the last call
   of initiate_pool. */

unsigned int max_number_of_bytes_for_nodes;


/* Current number of bytes of memory for nodes. */

unsigned int current_number_of_bytes_for_nodes;


/* Initiate pool without segments. */

void
initiate_pool ()
{
  max_number_of_nodes = current_number_of_nodes = 0;
  max_number_of_bytes_for_nodes = current_number_of_bytes_for_nodes = 0;
  current_segment = NULL;
}


/* Free all of all pool segments. */

void
free_pool ()
{
  register struct segment *next_segment;

  while (current_segment != NULL)
    {
      next_segment = current_segment;
      current_segment = current_segment->pred;
      free ((char *) next_segment);
    }
}


/* Allocate memory in pool for node with given size.  If it is necessary
   a new segment may be created in the pool.  The new segment size may
   be greater than standard segment size if the node is very big.

   It is very important that the node memory is cleared.  Values of many
   members in nodes are set up by this way. */

static ICN_pointer
allocate_icode (size)
     register unsigned int size;
{
  register char *str;
  register ICN_pointer ref;
  register char *new_segment;
  register unsigned int new_segment_length;

  size = AllocSize (size);
  if (current_segment == NULL
      || (current_segment->segment_memory + current_segment->segment_length
	  < current_segment->free + size))
    {
      new_segment_length = (size > STANDARD_SEGMENT_SIZE
			    ? size : STANDARD_SEGMENT_SIZE);
      M2C_ALLOC (new_segment, sizeof (struct segment) + new_segment_length);
      if (current_segment != NULL)
	current_segment->next = (struct segment *) new_segment;
      ((struct segment *) new_segment)->pred = current_segment;
      ((struct segment *) new_segment)->next = NULL;
      ((struct segment *) new_segment)->free
	= ((struct segment *) new_segment)->segment_memory;
      ((struct segment *) new_segment)->segment_length = new_segment_length;
      current_segment = (struct segment *) new_segment;
    }
  ref = ICN_POINTER (current_segment->free);
  current_segment->free += size;
  current_number_of_bytes_for_nodes += size;
  if (current_number_of_bytes_for_nodes > max_number_of_bytes_for_nodes)
    max_number_of_bytes_for_nodes = current_number_of_bytes_for_nodes;
  for (str = (char *) ref; size-- != 0;)
    *str++ = '\0';
  return ref;
}


/* Free memory for node starting with REF and having given size.  The memory
   is really freed if the node is most recently allocated.  If current segment
   does not contain other nodes it is freed. */

static void
icode_free (ref, size)
     register char *ref;
     register unsigned int size;
{
  register struct segment *next_segment;

  size = AllocSize (size);
  if (ref + size == current_segment->free)
    {
      current_segment->free = ref;
      current_number_of_bytes_for_nodes -= size;
      if (ref == current_segment->segment_memory)
	{
	  next_segment = current_segment;
	  current_segment = current_segment->pred;
	  current_segment->next = NULL;
	  free ((char *) next_segment);
	}
    }
}


/* Allocate memory for node with given mode and initiate its common members
   mode and line. */

ICN_pointer
create_node (mode)
     register enum icode_node_mode mode;
{
  register ICN_pointer ref;

  ref = allocate_icode ((unsigned int) NODE_SIZE (mode));
  SET_MODE (ref, mode);
  LINE (ref) = source_line_number;
  ++current_number_of_nodes;
  if (current_number_of_nodes > max_number_of_nodes)
    max_number_of_nodes = current_number_of_nodes;
  return ref;
}


/* Allocate memory for node with given mode and string attribute and
   initiate its common members mode and line.  Return pointer to the string
   attribute STR through parameter string_in_pool_ptr.  Nodes with string
   attribute are string, identifier an C code. */

ICN_pointer
create_node_with_string (m, str, string_in_pool_ptr)
     register enum icode_node_mode m;
     char *str, **string_in_pool_ptr;
{
  register ICN_pointer ref;

  ref = allocate_icode ((unsigned int) NODE_SIZE (m)
			+ (unsigned) strlen (str) + 1);
  SET_MODE (ref, m);
  LINE (ref) = source_line_number;
  ++current_number_of_nodes;
  if (current_number_of_nodes > max_number_of_nodes)
    max_number_of_nodes = current_number_of_nodes;
  *string_in_pool_ptr = (char *) ref + (unsigned int) NODE_SIZE (m);
  strcpy (*string_in_pool_ptr, str);
  return ref;
}


/* Create identifier with name STR (allocate memory, copy attribute in pool,
   initiate members and include to the hash table if the identifier does not
   exists). */

ICN_pointer
create_identifier_node (str)
     char *str;
{
  ICN_pointer ref;
  char *string_in_pool;

  ref = create_node_with_string (ICNM_IDENTIFIER, str, &string_in_pool);
  IDENTIFIER_NODE (ref)->identifier_string = string_in_pool;
  return find_in_or_else_include_to_table (ref);
}


/* Return string attribute in pool for given node. */

static char *
get_string_attribute (ref)
     register ICN_pointer ref;
{
  register enum icode_node_mode mode;

  mode = MODE (ref);
  if (mode == ICNM_C_CODE_IN_STATEMENTS || mode == ICNM_C_CODE_IN_DECLARATIONS)
    return C_TEXT (ref);
  else if (mode == ICNM_STRING)
    return STRING_NODE (ref)->string_value;
  else if (mode == ICNM_IDENTIFIER)
    return IDENTIFIER_NODE (ref)->identifier_string;
  return NULL;
}


/* Delete given node. */

void
delete_node (ref)
     register ICN_pointer ref;
{
  register char *str;

  str = get_string_attribute (ref);
  icode_free ((char *) ref, (unsigned int) NODE_SIZE (MODE (ref))
	      + (str != NULL ? (unsigned int) strlen (str) + 1 : 0));
  current_number_of_nodes--;
}

/* Return pointer to first block_begin_node in the pool. */

ICN_pointer
first_block_begin ()
{
  register ICN_pointer ref;
  register unsigned l;
  register char *str;
  register struct segment *seg;

  for (seg = current_segment; seg != NULL; seg = seg->pred)
    if (seg->pred == NULL)
      break;
  while (seg != NULL)
    {
      for (ref = ICN_POINTER (seg->segment_memory); (char *) ref < seg->free;)
	{
	  if (MODE (ref) == ICNM_BLOCK_BEGIN)
	    return ref;
	  str = get_string_attribute (ref);
	  l = (NODE_SIZE (MODE (ref))
	       + (str == NULL ? 0 : strlen (str) + 1));
	  l = AllocSize (l);
	  ref = ICN_POINTER ((char *) ref + l);
	}
      seg = seg->next;
    }
  return NULL;
}



/* Functions for work with icode node hash table.  The table contains pointers
   to icode nodes representing identifiers, values (cardinal, integer
   and etc.), basic types and denotations (i.e. nodes having members
   identifier and scope).

   The table has two name spaces.  The most of nodes are accessible from major
   name space.  Part of nodes (denotations) are accessible from two name
   spaces.  Qualified export nodes are accessible only from the second
   name space.  The table element is pointer to node (if the element used for
   major name space) or pointer to corresponding node incremented by one (if
   the element used for the second name space). */


/* Value of the following macros is TRUE if the table element REF represents
   node from the second name space. */

#define IT_IS_FROM_SECOND_NAME_SPACE(ref) ((unsigned long int)ref%2==1)


/* The following macros transforms the table element REF to
   the corresponding table element which represents icode node from the
   major name space. */

#define MAJOR_NAME_SPACE_REFERENCE(ref) (IT_IS_FROM_SECOND_NAME_SPACE(ref)\
					 ?ICN_POINTER ((char *)ref-1):ref)


/* The following macros transforms the table element REF to
   the corresponding table element which represents icode node from the
   second name space. */

#define SECOND_NAME_SPACE_REFERENCE(ref)  (IT_IS_FROM_SECOND_NAME_SPACE(ref)\
					   ?ref:ICN_POINTER ((char *)ref+1))


/* The node hash table. */

static hash_table node_hash_table;


/* Function for evaluation of hash value of icode node represented by EL_PTR.
   It may represent node in the second or major name spaces. */

static unsigned
hash_function (el_ptr)
     hash_table_entry el_ptr;
{
  register ICN_pointer node_ref;
  register enum icode_node_mode mode;
  register char *chp;
  register unsigned int i, size, hash_value;

  node_ref = ICN_POINTER (el_ptr);
  node_ref = MAJOR_NAME_SPACE_REFERENCE (node_ref); /* pointer to icode node.*/
  mode = MODE (node_ref);
  if (NODE_IS_DENOTATION (mode))
    hash_value = ((unsigned long int) SCOPE (node_ref)
		  + (unsigned long int) IDENTIFIER (node_ref));
  else if (mode == ICNM_BASIC_TYPE)
    hash_value = (unsigned int) BASIC_TYPE_NODE (node_ref)->basic_type;
  else
    {
      if (mode == ICNM_INTEGER)
	{
	  size = sizeof (Tint);
	  chp = (char *) &INTEGER_NODE (node_ref)->integer_value;
	}
      else if (mode == ICNM_CARDINAL)
	{
	  size = sizeof (Tcard);
	  chp = (char *) &CARDINAL_NODE (node_ref)->cardinal_value;
	}
      else if (mode == ICNM_REAL)
	{
	  size = sizeof (Treal);
	  chp = (char *) &REAL_NODE (node_ref)->real_value;
	}
      else if (mode == ICNM_SET)
	{
	  size = sizeof (Tset);
	  chp = (char *) &SET_NODE (node_ref)->set_value;
	}
      else if (mode == ICNM_STRING)
	{
	  chp = STRING_NODE (node_ref)->string_value;
	  size = strlen (chp);
	}
      else if (mode == ICNM_IDENTIFIER)
	{
	  chp = IDENTIFIER_NODE (node_ref)->identifier_string;
	  size = strlen (chp);
	}
      else
	abort ();
      for (hash_value = 0, i = 0; i < size; i++, chp++)
	hash_value += (*chp);
      if (mode != ICNM_IDENTIFIER)
	hash_value += (unsigned long int) VALUE_TYPE (node_ref);
    }
  return hash_value;
}


/* Function used for comparison of hash table elements represented by EL1_PTR,
   EL2_PTR.  They may represent nodes in the second and major name spaces.
   Return TRUE if the elements represent equal nodes in the same name space. */

static int
compare_function (el1_ptr, el2_ptr)
     hash_table_entry el1_ptr, el2_ptr;
{
  register ICN_pointer node_ref1, node_ref2;
  register enum icode_node_mode mode1, mode2;
  register int el1_is_in_second_space, el2_is_in_second_space;
  register int elements_are_equal;

  node_ref1 = ICN_POINTER (el1_ptr);
  node_ref2 = ICN_POINTER (el2_ptr);
  el1_is_in_second_space = IT_IS_FROM_SECOND_NAME_SPACE (node_ref1);
  el2_is_in_second_space = IT_IS_FROM_SECOND_NAME_SPACE (node_ref2);
  node_ref1 = MAJOR_NAME_SPACE_REFERENCE (node_ref1);
  node_ref2 = MAJOR_NAME_SPACE_REFERENCE (node_ref2);
  mode1 = MODE (node_ref1);
  mode2 = MODE (node_ref2);
  if (NODE_IS_DENOTATION (mode1) && NODE_IS_DENOTATION (mode2))
    elements_are_equal = (el1_is_in_second_space == el2_is_in_second_space
			  && SCOPE (node_ref1) == SCOPE (node_ref2)
			  && IDENTIFIER (node_ref1) == IDENTIFIER (node_ref2));
  else
    {
      elements_are_equal = mode1 == mode2;
      if (elements_are_equal)
	{
	  if (mode1 == ICNM_BASIC_TYPE)
	    elements_are_equal = (BASIC_TYPE_NODE (node_ref1)->basic_type
				  == BASIC_TYPE_NODE (node_ref2)->basic_type);
	  else if (mode1 == ICNM_IDENTIFIER)
	    elements_are_equal
	      = strcmp (IDENTIFIER_NODE (node_ref1)->identifier_string,
			IDENTIFIER_NODE (node_ref2)->identifier_string) == 0;
	  else
	    {
	      elements_are_equal = VALUE_TYPE (node_ref1) == VALUE_TYPE (node_ref2);
	      if (elements_are_equal)
		{
		  if (mode1 == ICNM_INTEGER)
		    elements_are_equal
		      = (INTEGER_NODE (node_ref1)->integer_value
			 == INTEGER_NODE (node_ref2)->integer_value);
		  else if (mode1 == ICNM_CARDINAL)
		    elements_are_equal
		      = (CARDINAL_NODE (node_ref1)->cardinal_value
			 == CARDINAL_NODE (node_ref2)->cardinal_value);
		  else if (mode1 == ICNM_REAL)
		    elements_are_equal
		      = (REAL_NODE (node_ref1)->real_value
			 == REAL_NODE (node_ref2)->real_value);
		  else if (mode1 == ICNM_SET)
		    elements_are_equal = (SET_NODE (node_ref1)->set_value
					  == SET_NODE (node_ref2)->set_value);
		  else
		    elements_are_equal
		      = strcmp (STRING_NODE (node_ref1)->string_value,
				STRING_NODE (node_ref2)->string_value) == 0;
		}
	    }
	}
    }
  return elements_are_equal;
}


/* Initiate the icode node hash table (create it and initiate standard
   environment). */

void
initiate_table_and_environment ()
{
  register int i;
  register ICN_pointer ref;

  /* The following icode real nodes are initiated here because maximum and
     minimum real values in the file float.h may be not C constant expression
     (see ANSI standard of C language). */

  ICN_V_min_short_real.mode = (ICN_mode) ICNM_REAL;
  ICN_V_min_short_real.value_type = ICN_POINTER (&ICN_TD_short_real);
  ICN_V_min_short_real.real_value = SHORTREAL_MIN;

  ICN_V_min_real.mode = (ICN_mode) ICNM_REAL;
  ICN_V_min_real.value_type = ICN_POINTER (&ICN_TD_real);
  ICN_V_min_real.real_value = REAL_MIN;

  ICN_V_min_long_real.mode = (ICN_mode) ICNM_REAL;
  ICN_V_min_long_real.value_type = ICN_POINTER (&ICN_TD_long_real);
  ICN_V_min_long_real.real_value = LONGREAL_MIN;

  ICN_V_max_short_real.mode = (ICN_mode) ICNM_REAL;
  ICN_V_max_short_real.value_type = ICN_POINTER (&ICN_TD_short_real);
  ICN_V_max_short_real.real_value = SHORTREAL_MAX;

  ICN_V_max_real.mode = (ICN_mode) ICNM_REAL;
  ICN_V_max_real.value_type = ICN_POINTER (&ICN_TD_real);
  ICN_V_max_real.real_value = REAL_MAX;

  ICN_V_max_long_real.mode = (ICN_mode) ICNM_REAL;
  ICN_V_max_long_real.value_type = ICN_POINTER (&ICN_TD_long_real);
  ICN_V_max_long_real.real_value = LONGREAL_MAX;

  node_hash_table = create_hash_table (1000, hash_function, compare_function);
  for (i = 0;; i++)
    {
      ref = nodes_for_hash_table[i];
      if (ref == NULL)
	break;
      find_in_or_else_include_to_table (ref);
    }
}


/* Delete the icode node hash table. */

void
delete_table ()
{
  delete_hash_table (node_hash_table);
}


/* Find pointer to the icode node hash table entry which contains pointer to
   node with the same keys as in node starting with REF or else the empty entry
   (its value is NULL).  If RESERVE is TRUE than the empty entry is reseved
   and must be filled by node with the same keys as in node starting with
   REF. */

static ICN_pointer *
find_table_entry (ref, reserve)
     register ICN_pointer ref;
     int reserve;
{
  return
    (ICN_pointer *) find_hash_table_entry (node_hash_table,
					   (hash_table_entry) ref, reserve);
}


/* Auxiliary function used by two following functions (second parameter
   determines calling function).  */

static ICN_pointer
find_include (ref, flag_of_change)
     register ICN_pointer ref;
     int flag_of_change;
{
  register ICN_pointer *table_entry_pointer, ref_in_second_space;
  enum icode_node_mode mode;

  ref_in_second_space = SECOND_NAME_SPACE_REFERENCE (ref);
  mode = MODE (ref);
  table_entry_pointer
    = find_table_entry ((mode == ICNM_QUALIFIED_EXPORT
			 ? ref_in_second_space : ref), TRUE);
  if (*table_entry_pointer != NULL && !flag_of_change)
    delete_node (ref);
  else
    {
      *table_entry_pointer = (mode == ICNM_QUALIFIED_EXPORT
			      ? ref_in_second_space : ref);
      if (NODE_IS_DENOTATION (mode) && IN_DEFINITION_MODULE (ref))
	*find_table_entry (ref_in_second_space, TRUE) = ref_in_second_space;
    }
  return MAJOR_NAME_SPACE_REFERENCE (*table_entry_pointer);
}


/* Find node with the same keys as node starting with REF.  If the node in the
   icode node hash table does not exist than include the node starting with REF
   to the table.  Return pointer to the corresponding node which is
   in the table. */

ICN_pointer
find_in_or_else_include_to_table (ref)
     ICN_pointer ref;
{
  return find_include (ref, FALSE);
}


/* Include node starting with REF to the icode node hash table.  If node with
   the same keys is already in the table than delete it and include the node
   starting with REF to the table. */

void
include_to_table (ref)
     ICN_pointer ref;
{
  find_include (ref, TRUE);
}


/* Return pointer to identifier node with the hash table key STRING. */

ICN_pointer
find_identifier (string)
     char *string;
{
  identifier_node identifier;
  register ICN_pointer result;

  identifier.mode = (ICN_mode) ICNM_IDENTIFIER;
  identifier.identifier_string = string;
  result = (*find_table_entry (ICN_POINTER (&identifier), FALSE));
  return result;
}


/* Auxiliary function used by two following functions (third parameter
   determines calling function).  */

static ICN_pointer
find (scope, identifier, search_is_in_second_space)
     ICN_pointer scope, identifier;
     int search_is_in_second_space;
{
  variable_declaration_node var;
  register ICN_pointer ref, r;

  var.mode = (ICN_mode) ICNM_VARIABLE_DECLARATION;
  var.scope = scope;
  var.identifier = identifier;
  r = ICN_POINTER (&var);
  ref = (*find_table_entry ((search_is_in_second_space
			     ? SECOND_NAME_SPACE_REFERENCE (r) : r), FALSE));
  return MAJOR_NAME_SPACE_REFERENCE (ref);
}


/* Return pointer to denotation with the hash table keys SCOPE and IDENTIFIER.
   The search is executed in the major name space. */

ICN_pointer
find_denotation (scope, identifier)
     ICN_pointer scope, identifier;
{
  return find (scope, identifier, FALSE);
}


/* The following function is analogous to find_denotation but the search is
   executed in the second name space.  Return NULL if the declaration is
   not found. */

ICN_pointer
find_denotation_in_second_space (scope, identifier)
     ICN_pointer scope, identifier;
{
  return find (scope, identifier, TRUE);
}


/* Return pointer to corresponding declaration node for identifier starting
   with IDENTIFIER in object starting with SCOPE and opening new scope.
   Search is executed only without processing export/import.  Therefore
   function may be return export or import node.  Return NULL if the
   declaration is not found. */

ICN_pointer
find_identifier_declaration_or_its_export_import (scope, identifier)
     register ICN_pointer scope, identifier;
{
  register ICN_pointer ref, upper_identifier;
  register int flag_of_standard_identifier;
  register struct stdid *stdid_pointer;
  char *s;

  stdid_pointer
    = find_stdid (IDENTIFIER_NODE (identifier)->identifier_string,
		  strlen (IDENTIFIER_NODE (identifier)->identifier_string));
  flag_of_standard_identifier = (stdid_pointer != 0
				 && (!stdid_pointer->lower_case_stdid || !only_upper_case_flag));
  for (;;)
    {
      ref = find_denotation (scope, identifier);
      if (ref == NULL && scope != NULL
	  && (MODE (scope) == ICNM_PROCEDURE || MODE (scope) == ICNM_RECORD
	      || flag_of_standard_identifier))
	scope = SCOPE (scope);
      else if (scope == NULL && flag_of_standard_identifier)
	{
	  /* Process case when standard identifier is in lower case. */
	  upper_identifier
	    = create_node_with_string
	      (ICNM_IDENTIFIER,
	       IDENTIFIER_NODE (identifier)->identifier_string, &s);
	  IDENTIFIER_NODE (upper_identifier)->identifier_string = s;
	  for (; *s != '\0'; s++)
	    *s = (islower (*s) ? toupper (*s) : *s);
	  return
	    find_denotation
	      (scope, find_in_or_else_include_to_table (upper_identifier));
	}
      else
	return ref;
    }
}


/* Return pointer to corresponding declaration node for identifier starting
   with IDENTIFIER in object starting with SCOPE and opening new scope.
   Search is executed according to all rules of visibility.  Return NULL
   if the declaration is not found.

   According to language extensions all C functions are declared in module
   CPROC.  If searched identifier is such C function than the corresponding
   C function node may be created. */

ICN_pointer
find_identifier_declaration (scope, identifier)
     register ICN_pointer scope;
     ICN_pointer identifier;
{
  int export_was_last;
  register ICN_pointer ref, module_after_from_clause;

  for (export_was_last = FALSE;;)
    {
      ref
	= find_identifier_declaration_or_its_export_import (scope, identifier);
      if (ref == NULL)
	return NULL;
      else if (MODE (ref) == ICNM_EXPORT)
	{
	  export_was_last = TRUE;
	  scope = EXPORT_NODE (ref)->module_exporter;
	}
      else if (MODE (ref) != ICNM_IMPORT)
	return ref;
      else if (export_was_last)
	return NULL;		/* export of import */
      else if (IMPORT_NODE (ref)->module_exporter == NULL)
	scope = SCOPE (SCOPE (ref));
      else
	{
	  module_after_from_clause
	    = find_identifier_declaration (SCOPE (SCOPE (ref)),
					   IMPORT_NODE (ref)->module_exporter);
	  if (module_after_from_clause == ICN_POINTER (&ICN_D_cproc))
	    {
	      ref = create_node (ICNM_C_FUNCTION);
	      SCOPE (ref) = module_after_from_clause;
	      IDENTIFIER (ref) = identifier;
	      return find_in_or_else_include_to_table (ref);
	    }
	  else if (module_after_from_clause == NULL
		   || MODE (module_after_from_clause) != ICNM_MODULE)
	    return NULL;	/* No module is in from clause. */
	  else
	    {
	      ref = find_denotation_in_second_space (module_after_from_clause,
						     identifier);
	      if (ref == NULL)
		return NULL;
	      else if (MODE (ref) == ICNM_QUALIFIED_EXPORT
		       && QUALIFIED_EXPORT_NODE (ref)->opaque_type)
		/* Opaque type is represented by its qualified export node. */
		return ref;
	      else
		{
		  export_was_last = TRUE;
		  scope = module_after_from_clause;
		}
	    }
	}
    }
}




/* Icode predicates and functions for icode navigation. */


/* Return TRUE if REF is pointer to basic type node representing
   any integer type. */

int
it_is_integer_type (ref)
     register ICN_pointer ref;
{
  return (ref == ICN_POINTER (&ICN_TD_short_integer)
	  || ref == ICN_POINTER (&ICN_TD_integer)
	  || ref == ICN_POINTER (&ICN_TD_long_integer));
}


/* Return TRUE if REF is pointer to basic type node representing
   any cardinal type. */

int
it_is_cardinal_type (ref)
     register ICN_pointer ref;
{
  return (ref == ICN_POINTER (&ICN_TD_short_cardinal)
	  || ref == ICN_POINTER (&ICN_TD_cardinal)
	  || ref == ICN_POINTER (&ICN_TD_long_cardinal));
}


/* Return TRUE if REF is pointer to basic type node representing
   any type suitable for integer and cardinal value. */

int
it_is_integer_cardinal_type (ref)
     register ICN_pointer ref;
{
  return (ref == ICN_POINTER (&ICN_TD_short_cardinal_or_integer)
	  || ref == ICN_POINTER (&ICN_TD_cardinal_or_integer)
	  || ref == ICN_POINTER (&ICN_TD_long_cardinal_or_integer));
}


/* Return TRUE if REF is pointer to basic type node representing
   any real type. */

int
it_is_real_type (ref)
     register ICN_pointer ref;
{
  return (ref == ICN_POINTER (&ICN_TD_short_real)
	  || ref == ICN_POINTER (&ICN_TD_real)
	  || ref == ICN_POINTER (&ICN_TD_long_real));
}


/* Return TRUE if REF is pointer to basic type node representing
   any string type.  See also commentaries for BTM_EMPTY_STRING. */

int
it_is_string_type (ref)
     register ICN_pointer ref;
{
  return (ref != NULL && MODE (ref) == ICNM_BASIC_TYPE
	  && (int) BASIC_TYPE_NODE (ref)->basic_type >= (int) BTM_EMPTY_STRING);
}


/* Return length of string which is represented by its type.  REF must be
   pointer to basic type node representing string type.  See also commentaries
   for BTM_EMPTY_STRING. */

int
string_length_from_its_type (ref)
     ICN_pointer ref;
{
  return (int) BASIC_TYPE_NODE (ref)->basic_type - (int) BTM_EMPTY_STRING;
}


/* Return TRUE if REF is pointer to basic type node representing character
   type.  Remember that one-character string may be interpreted
   as a character. */

int
it_is_character_type (ref)
     register ICN_pointer ref;
{
  return (ref == ICN_POINTER (&ICN_TD_char)
	  || (it_is_string_type (ref)
	      && string_length_from_its_type (ref) == (third_edition_flag ? 1 : 2)));
}


/* If TYPE is pointer to basic type node representing any type suitable for
   integer and cardinal value than return corresponding cardinal type.
   Otherwise return TYPE. */

ICN_pointer
through_integer_cardinal_type (type)
     register ICN_pointer type;
{
  if (type == ICN_POINTER (&ICN_TD_short_cardinal_or_integer))
    return ICN_POINTER (&ICN_TD_short_cardinal);
  else if (type == ICN_POINTER (&ICN_TD_cardinal_or_integer))
    return ICN_POINTER (&ICN_TD_cardinal);
  else if (type == ICN_POINTER (&ICN_TD_long_cardinal_or_integer))
    return ICN_POINTER (&ICN_TD_long_cardinal);
  return type;
}


/* Return cardinal value of character, enumeration or cardinal value.
   REF must be pointer to cardinal node or one-character string node. */

Tcard
cardinal_value (ref)
     register ICN_pointer ref;
{
  return (MODE (ref) == ICNM_STRING
	  ? CHAR_TO_INT (*STRING_NODE (ref)->string_value)
	  : CARDINAL_NODE (ref)->cardinal_value);
}


/* Return pointer to basic type node representing minimal real type
   suitable for storing R. */

ICN_pointer
type_of_real_value (r)
     Treal r;
{
  return ICN_POINTER (&ICN_TD_long_real);
}


/* Predicates for cardinal values. */

#define VALUE_IS_OF_SHORT_CARDINAL_TYPE(v) ((v)<=SHORTCARD_MAX)

#define VALUE_IS_OF_CARDINAL_TYPE(v) ((v)<=CARDINAL_MAX)


/* Return pointer to basic type node representing minimal cardinal type
   suitable for storing C. */

ICN_pointer
type_of_cardinal_value (c)
     Tcard c;
{
  return (VALUE_IS_OF_SHORT_CARDINAL_TYPE (c)
	  ? ICN_POINTER (&ICN_TD_short_cardinal)
	  : (VALUE_IS_OF_CARDINAL_TYPE (c)
	     ? ICN_POINTER (&ICN_TD_cardinal)
	     : ICN_POINTER (&ICN_TD_long_cardinal)));
}


/* Predicates for cardinal values. */

#define VALUE_IS_OF_SHORT_INTEGER_TYPE(v) (SHORTINT_MIN<=(v)\
					   && (v)<=SHORTINT_MAX)
#define VALUE_IS_OF_INTEGER_TYPE(v) (INTEGER_MIN<=(v) && (v)<=INTEGER_MAX)


/* Return pointer to basic type node representing minimal integer type
   suitable for storing I. */

ICN_pointer
type_of_integer_value (i)
     Tint i;
{
  return (VALUE_IS_OF_SHORT_INTEGER_TYPE (i)
	  ? ICN_POINTER (&ICN_TD_short_integer)
	  : (VALUE_IS_OF_INTEGER_TYPE (i)
	     ? ICN_POINTER (&ICN_TD_integer)
	     : ICN_POINTER (&ICN_TD_long_integer)));
}


/* Return pointer to basic type node representing minimal integer-cardinal
   type suitable for storing C. */

ICN_pointer
integer_cardinal_type_of_cardinal_value (c)
     Tcard c;
{
  return (c <= SHORTINT_MAX ? ICN_POINTER (&ICN_TD_short_cardinal_or_integer)
	  : (c <= INTEGER_MAX ? ICN_POINTER (&ICN_TD_cardinal_or_integer)
	     : (c <= LONGINT_MAX
		? ICN_POINTER (&ICN_TD_long_cardinal_or_integer)
		: NULL)));
}


/* If REF is a pointer to enumeration type node or constant declaration node.
   than returns pointer to constant declaration node representing first or next
   enumeration constant.  Otherwise (or if REF represents last enumeration
   constant) return NULL. */

ICN_pointer
next_enumeration_constant (ref)
     register ICN_pointer ref;
{
  register ICN_pointer r;

  if (ref == NULL)
    return ref;
  else if (MODE (ref) == ICNM_ENUMERATION_TYPE)
    return ENUMERATION_TYPE_NODE (ref)->first_enumeration_constant;
  else if (MODE (ref) != ICNM_CONSTANT_DECLARATION
	   || NEXT_DECLARATION_ELEMENT (ref) == NULL)
    return NULL;
  else
    {
      r = NEXT_DECLARATION_ELEMENT (ref);
      /* See commentaries for macro NEXT_DECLARATION_ELEMENT. */
      if (MODE (r) == ICNM_QUALIFIED_EXPORT)
	r = NEXT_DECLARATION_ELEMENT (r);
      if (r == NULL || MODE (r) != ICNM_CONSTANT_DECLARATION
	  || CONSTANT_DECLARATION_NODE (r)->constant_expression == NULL
	  || (VALUE_TYPE (CONSTANT_DECLARATION_NODE (r)->constant_expression)
	      != VALUE_TYPE (CONSTANT_DECLARATION_NODE (ref)->constant_expression)))
	return NULL;
      else
	return r;
    }
}


/* Return pointer to the nearest procedure node which contains denotation
   starting with REF.  If such procedure does not exist return NULL.
   REF must refers to denotation (node having members identifier and scope). */

ICN_pointer
procedure_over_denotation (ref)
     register ICN_pointer ref;
{
  for (; ref != NULL && MODE (ref) != ICNM_PROCEDURE; ref = SCOPE (ref)) ;
  return ref;
}


/* REF must refer to procedure type, formal parameter or procedure parameter
   type nodes.  Returns pointer to node representing first or next
   parameter type.  If REF represents procedure without parameters
   or last parameter than return NULL. */

ICN_pointer
next_parameter_type (ref)
     register ICN_pointer ref;
{
  ref = NEXT_DECLARATION_ELEMENT (ref);
  if (ref != NULL && (MODE (ref) == ICNM_FORMAL_PARAMETER
		      || MODE (ref) == ICNM_PROCEDURE_PARAMETER_TYPE))
    return ref;
  return NULL;
}


/* If TYPE is range type than return its base type definition.
   Otherwise return TYPE. */

ICN_pointer
through_range_type (type)
     register ICN_pointer type;
{
  if (type != NULL && MODE (type) == ICNM_RANGE_TYPE)
    type = RANGE_TYPE_NODE (type)->base_type;
  return type;
}


/* The following table is used by function min_or_max to determine
   icode nodes representing minimum values for basic types between short
   cardinal and char (see enumeration  basic_type). */

static ICN_pointer min_basic_type_value_table[] =
{
  ICN_POINTER (&ICN_V_min_short_cardinal), ICN_POINTER (&ICN_V_min_cardinal),
  ICN_POINTER (&ICN_V_min_long_cardinal),
  ICN_POINTER (&ICN_V_min_short_integer), ICN_POINTER (&ICN_V_min_integer),
  ICN_POINTER (&ICN_V_min_long_integer),
  ICN_POINTER (&ICN_V_min_short_real), ICN_POINTER (&ICN_V_min_real),
  ICN_POINTER (&ICN_V_min_long_real),
  ICN_POINTER (&ICN_V_min_char)};


/* The following table is used by function min_or_max to determine
   icode nodes representing maximum values for basic types between short
   cardinal and char (see enumeration  basic_type). */

static ICN_pointer max_basic_type_value_table[] =
{
  ICN_POINTER (&ICN_V_max_short_cardinal), ICN_POINTER (&ICN_V_max_cardinal),
  ICN_POINTER (&ICN_V_max_long_cardinal),
  ICN_POINTER (&ICN_V_max_short_integer), ICN_POINTER (&ICN_V_max_integer),
  ICN_POINTER (&ICN_V_max_long_integer),
  ICN_POINTER (&ICN_V_max_short_real), ICN_POINTER (&ICN_V_max_real),
  ICN_POINTER (&ICN_V_max_long_real), ICN_POINTER (&ICN_V_max_char)};


/* Return value node pointer which represents result of Modula-2 standard
   MIN (if FLMIN is TRUE) or MAX (otherwise) function with parameter
   represented by type REF.  If result of MIN or MAX function is undefined
   for language construction represented by icode subgraph REF than
   the following function returns NULL. */

ICN_pointer
min_or_max (ref, flmin)
     register ICN_pointer ref;
     register int flmin;
{
  register ICN_pointer current_enumeration_constant;

  if (ref == NULL)
    return ref;
  else if (MODE (ref) == ICNM_RANGE_TYPE)
    return (flmin
	    ? RANGE_TYPE_NODE (ref)->left_range_bound
	    : RANGE_TYPE_NODE (ref)->right_range_bound);
  else if (MODE (ref) == ICNM_ENUMERATION_TYPE)
    {
      ref = next_enumeration_constant (ref);
      if (!flmin)
	for (;;)
	  {
	    current_enumeration_constant = next_enumeration_constant (ref);
	    if (current_enumeration_constant == NULL)
	      break;
	    ref = current_enumeration_constant;
	  }
      return CONSTANT_DECLARATION_NODE (ref)->constant_expression;
    }
  else if (MODE (ref) != ICNM_BASIC_TYPE
	   || (int) BASIC_TYPE_NODE (ref)->basic_type > (int) BTM_CHAR)
    return NULL;
  else
    return
      (flmin
       ? min_basic_type_value_table[(int) BASIC_TYPE_NODE (ref)->basic_type]
       : max_basic_type_value_table[(int) BASIC_TYPE_NODE (ref)->basic_type]);
}


/* If TYPE is opaque type (this type is represented by qualified export node)
   than return its real type definition.  Otherwise return TYPE. */

static ICN_pointer
through_opaque_type (type)
     register ICN_pointer type;
{
  register ICN_pointer r;

  if (type != NULL && MODE (type) == ICNM_QUALIFIED_EXPORT)
    {
      r = find_denotation (SCOPE (type), IDENTIFIER (type));
      if (r != NULL && MODE (r) == ICNM_TYPE_DECLARATION)
	type = TYPE_DECLARATION_NODE (r)->type_definition;
    }
  return type;
}



/* The following functions determine different type relations (identity,
   compatibility and assignment compatibility. */


/* This recursive function returns TRUE if subgraphs representing types and
   starting with TYPE1 and TYPE2 are identical types. */

int
types_are_identical (type1, type2)
     register ICN_pointer type1, type2;
{
  if (type1 != NULL && type2 != NULL)
    {
      type1 = through_opaque_type (type1);
      type2 = through_opaque_type (type2);
      if (type1 == type2)
	return TRUE;
      else if (MODE (type1) == ICNM_PROCEDURE_TYPE
	       && MODE (type2) == ICNM_PROCEDURE_TYPE)
	{
	  if (!(types_are_identical
		(PROCEDURE_TYPE_NODE (type1)->procedure_result_type,
		 PROCEDURE_TYPE_NODE (type2)->procedure_result_type))
	      && (PROCEDURE_TYPE_NODE (type1)->procedure_result_type
		  != PROCEDURE_TYPE_NODE (type2)->procedure_result_type))
	    return FALSE;
	  for (;;)
	    {
	      type1 = next_parameter_type (type1);
	      type2 = next_parameter_type (type2);
	      if (type1 == NULL || type2 == NULL)
		break;
	      if (VAR_FORMAL_PARAMETER (type1) != VAR_FORMAL_PARAMETER (type2)
		  || ARRAY_FORMAL_PARAMETER (type1) != ARRAY_FORMAL_PARAMETER (type2)
		  || !types_are_identical (FORMAL_PARAMETER_TYPE (type1),
					   FORMAL_PARAMETER_TYPE (type2)))
		return FALSE;
	    }
	  return type1 == type2;
	}
    }
  return FALSE;
}


/* This function is used by function result_of_compatible_types to decrease
   source code.  The function behaves differently after exchanging TYPE1
   and TYPE2.  The values of parameters are only subset of the values for
   function result_of_compatible_types.  The following function returns
   result analogous to function result_of_compatible_types (see commentaries
   for mentioned function). */

static ICN_pointer
auxiliary_function (type1, type2)
     register ICN_pointer type1, type2;
{
  register ICN_pointer ref;

  ref = NULL;
  if (it_is_cardinal_type (type1)
      && (it_is_cardinal_type (type2) || it_is_integer_cardinal_type (type2)))
    {
      if (type1 == ICN_POINTER (&ICN_TD_long_cardinal)
	  || type2 == ICN_POINTER (&ICN_TD_long_cardinal)
	  || type2 == ICN_POINTER (&ICN_TD_long_cardinal_or_integer))
	ref = ICN_POINTER (&ICN_TD_long_cardinal);
      else if (type1 == ICN_POINTER (&ICN_TD_cardinal)
	       || type2 == ICN_POINTER (&ICN_TD_cardinal)
	       || type2 == ICN_POINTER (&ICN_TD_cardinal_or_integer))
	ref = ICN_POINTER (&ICN_TD_cardinal);
      else
	ref = ICN_POINTER (&ICN_TD_short_cardinal);
    }
  else if (it_is_integer_type (type1)
	   && (it_is_integer_type (type2)
	       || it_is_integer_cardinal_type (type2)))
    {
      if (type1 == ICN_POINTER (&ICN_TD_long_integer)
	  || type2 == ICN_POINTER (&ICN_TD_long_integer)
	  || type2 == ICN_POINTER (&ICN_TD_long_cardinal_or_integer))
	ref = ICN_POINTER (&ICN_TD_long_integer);
      else if (type1 == ICN_POINTER (&ICN_TD_integer)
	       || type2 == ICN_POINTER (&ICN_TD_integer)
	       || type2 == ICN_POINTER (&ICN_TD_cardinal_or_integer))
	ref = ICN_POINTER (&ICN_TD_integer);
      else
	ref = ICN_POINTER (&ICN_TD_short_integer);
    }
  else if ((type1 == ICN_POINTER (&ICN_TD_nil)
	    && MODE (type2) == ICNM_POINTER_TYPE)
	   || (it_is_string_type (type1) && MODE (type2) == ICNM_ARRAY_TYPE
	       && (ARRAY_TYPE_NODE (type2)->base_type
		   == ICN_POINTER (&ICN_TD_char))
	       && (string_length_from_its_type (type1)
		   == array_index_span (ARRAY_TYPE_NODE (type2)
					->array_index_type) + 1)))
    ref = type2;
  else if (type1 == ICN_POINTER (&ICN_TD_address)
	   && (type2 == ICN_POINTER (&ICN_TD_nil)
	       || MODE (type2) == ICNM_POINTER_TYPE))
    ref = type2;
  else if (type1 == ICN_POINTER (&ICN_TD_address)
	   && (it_is_cardinal_type (type2)
	       || it_is_integer_cardinal_type (type2)))
    {
      if (type2 == ICN_POINTER (&ICN_TD_long_cardinal)
	  || type2 == ICN_POINTER (&ICN_TD_long_cardinal_or_integer))
	ref = ICN_POINTER (&ICN_TD_long_cardinal);
      else
	ref = ICN_POINTER (&ICN_TD_cardinal);
    }
  else if (it_is_string_type (type1)
	   && type2 == ICN_POINTER (&ICN_TD_char)
	   && (string_length_from_its_type (type1)
	       == (third_edition_flag ? 1 : 2)))
    ref = ICN_POINTER (&ICN_TD_char);
  return ref;
}


/* This function returns NULL if subgraphs representing types and
   starting with TYPE1 and TYPE2 are not compatible types.  Otherwise
   the function returns the nearest type which values will contain values
   of both types TYPE1 and TYPE2. */

ICN_pointer
result_of_compatible_types (type1, type2)
     register ICN_pointer type1, type2;
{
  register ICN_pointer ref;

  ref = NULL;
  type1 = through_range_type (type1);
  type2 = through_range_type (type2);
  if (type1 != NULL && type2 != NULL)
    {
      if (types_are_identical (type1, type2))
	{
	  if (type1 == ICN_POINTER (&ICN_TD_address))
	    ref = ICN_POINTER (&ICN_TD_cardinal);
	  else
	    ref = type1;
	}
      else if (it_is_integer_cardinal_type (type1)
	       && it_is_integer_cardinal_type (type2))
	{
	  if (type1 == ICN_POINTER (&ICN_TD_long_cardinal_or_integer)
	      || type2 == ICN_POINTER (&ICN_TD_long_cardinal_or_integer))
	    ref = ICN_POINTER (&ICN_TD_long_cardinal_or_integer);
	  else if (type1 == ICN_POINTER (&ICN_TD_cardinal_or_integer)
		   || type2 == ICN_POINTER (&ICN_TD_cardinal_or_integer))
	    ref = ICN_POINTER (&ICN_TD_cardinal_or_integer);
	  else
	    ref = ICN_POINTER (&ICN_TD_short_cardinal_or_integer);
	}
      else if (it_is_real_type (type1) && it_is_real_type (type2))
	{
	  if (type1 == ICN_POINTER (&ICN_TD_long_real)
	      || type2 == ICN_POINTER (&ICN_TD_long_real))
	    ref = ICN_POINTER (&ICN_TD_long_real);
	  else if (type1 == ICN_POINTER (&ICN_TD_real)
		   || type2 == ICN_POINTER (&ICN_TD_real))
	    ref = ICN_POINTER (&ICN_TD_real);
	  else
	    ref = ICN_POINTER (&ICN_TD_short_real);
	}
      else
	{
	  ref = auxiliary_function (type1, type2);
	  if (ref == NULL)
	    ref = auxiliary_function (type2, type1);
	}
    }
  return ref;
}


/* This function returns TRUE if subgraphs representing types and
   starting with TYPE1 and TYPE2 are assignment compatible types. */

int
types_are_assignment_compatible (type1, type2)
     register ICN_pointer type1, type2;
{
  type1 = through_range_type (type1);
  type2 = through_range_type (type2);
  return
    (type1 != NULL && type2 != NULL
     && (result_of_compatible_types (type1, type2) != NULL
	 || (it_is_integer_type (type1)
	     && (it_is_cardinal_type (type2)
		 || type2 == ICN_POINTER (&ICN_TD_address)))
	 || (it_is_integer_type (type2)
	     && (it_is_cardinal_type (type1)
		 || type1 == ICN_POINTER (&ICN_TD_address)))));
}



/* The following functions determine class (type or expression) and
   semantic information about the class for icode subgraph. */


/* The function determine that the icode subgraph starting with REF is a
   type.  If it is true than member type in SINF is set up by REF. */

int
subgraph_is_type (ref, sinf)
     register ICN_pointer ref;
     register semantic_information *sinf;
{
  if (ref == NULL)
    return FALSE;
  switch (MODE (ref))
    {
    case ICNM_BASIC_TYPE:
    case ICNM_ENUMERATION_TYPE:
    case ICNM_RANGE_TYPE:
    case ICNM_SET_TYPE:
    case ICNM_POINTER_TYPE:
    case ICNM_ARRAY_TYPE:
    case ICNM_PROCEDURE_TYPE:
    case ICNM_RECORD:
      sinf->type = ref;
      break;
    case ICNM_QUALIFIED_EXPORT:
      /* See commentaries for qualified export node. */
      if (QUALIFIED_EXPORT_NODE (ref)->opaque_type)
	{
	  sinf->type = ref;
	  break;
	}
    default:
      return FALSE;
    }
  return TRUE;
}


/* The function determine that the icode subgraph starting with REF is a
   expression.  If it is true than semantic information (see commentaries
   for semantic_information) about the expression is passed through SINF. */

int
subgraph_is_expression (ref, sinf)
     register ICN_pointer ref;
     register semantic_information *sinf;
{

  if (ref == NULL)
    return FALSE;
  sinf->it_is_designator = TRUE;
  sinf->it_is_constant = sinf->it_is_array_parameter = FALSE;
  switch (MODE (ref))
    {
    case ICNM_EQUAL:
    case ICNM_UNEQUAL:
    case ICNM_LESS_THAN:
    case ICNM_GREATER_THAN:
    case ICNM_LESS_OR_EQUAL:
    case ICNM_GREATER_OR_EQUAL:
    case ICNM_IN:
    case ICNM_IDENTITY:
    case ICNM_SIGN_CONVERSION:
    case ICNM_ADDITION:
    case ICNM_SUBTRACTION:
    case ICNM_OR:
    case ICNM_AND:
    case ICNM_MULT:
    case ICNM_REAL_DIVISION:
    case ICNM_DIV:
    case ICNM_MOD:
    case ICNM_NOT:
    case ICNM_SET_VALUE_START:
    case ICNM_RANGE_IN_SET_VALUE:
    case ICNM_ELEMENT_IN_SET_VALUE:
    case ICNM_FUNCTION_CALL:
      sinf->it_is_designator = FALSE;
      sinf->type = VALUE_TYPE (ref);
      break;
    case ICNM_QUALIFICATION:
    case ICNM_INDEXING:
    case ICNM_DEREFERENCING:
      sinf->type = VALUE_TYPE (ref);
      break;
    case ICNM_VARIABLE_DECLARATION:
      sinf->type = VARIABLE_DECLARATION_NODE (ref)->variable_type;
      break;
    case ICNM_FIELD:
      sinf->type = FIELD_NODE (ref)->field_type;
      break;
    case ICNM_FORMAL_PARAMETER:
      sinf->type = PARAMETER_TYPE_OF_FORMAL_PARAMETER (ref);
      if (sinf->type != NULL)
	{
	  sinf->it_is_array_parameter = ARRAY_FORMAL_PARAMETER (sinf->type);
	  sinf->type = FORMAL_PARAMETER_TYPE (sinf->type);
	}
      break;
    case ICNM_PROCEDURE:
      sinf->it_is_constant = TRUE;
      sinf->it_is_designator = FALSE;
      sinf->type = PROCEDURE_NODE (ref)->procedure_type;
      break;
    case ICNM_INTEGER:
    case ICNM_CARDINAL:
    case ICNM_REAL:
    case ICNM_STRING:
    case ICNM_SET:
      sinf->it_is_constant = TRUE;
      sinf->it_is_designator = FALSE;
      sinf->type = VALUE_TYPE (ref);
      break;
    default:
      return FALSE;
    }
  return TRUE;
}



/* Source code for icode node.  This code may be used for debugging. */


/* Table of printed names of icode nodes.  See enumeration  icode_node_mode. */

static char *printed_icode_node_names[] =
{
  "INTEGER", "CARDINAL", "REAL", "STRING",
  "IDENTIFIER", "SET", "EQUAL", "UNEQUAL",
  "LESS_THAN", "GREATER_THAN", "LESS_OR_EQUAL", "GREATER_OR_EQUAL",
  "IN", "IDENTITY", "SIGN_CONVERSION", "ADDITION",
  "SUBTRACTION", "OR", "AND", "MULT",
  "REAL_DIVISION", "DIV", "MOD", "NOT",
  "QUALIFICATION", "INDEXING", "DEREFERENCING", "SET_VALUE_START",
  "ELEMENT_IN_SET_VALUE", "RANGE_IN_SET_VALUE", "FUNCTION_CALL", "ASSIGNMENT",
  "PROCEDURE_CALL", "ACTUAL_PARAMETER", "BLOCK_BEGIN", "RETURN_WITHOUT_RESULT",
  "RETURN_WITH_RESULT", "LOOP_STATEMENT", "EXIT_STATEMENT", "WHILE_STATEMENT",
  "REPEAT_STATEMENT", "FOR_STATEMENT", "IF_STATEMENT", "ELSIF_CLAUSE",
  "CASE_STATEMENT", "WITH_STATEMENT",
  "CASE_STATEMENT_VARIANT", "CASE_STATEMENT_LABEL_ELEMENT",
  "CASE_STATEMENT_LABEL_RANGE", "C_CODE_IN_STATEMENTS",
  "C_CODE_IN_DECLARATIONS", "EXPORT", "QUALIFIED_EXPORT", "IMPORT",
  "FIELD", "FORMAL_PARAMETER", "MODULE", "PROCEDURE",
  "VARIABLE_DECLARATION", "CONSTANT_DECLARATION",
  "TYPE_DECLARATION", "C_FUNCTION", "ENUMERATION_TYPE", "RANGE_TYPE",
  "SET_TYPE", "POINTER_TYPE", "ARRAY_TYPE", "PROCEDURE_TYPE",
  "PROCEDURE_PARAMETER_TYPE", "RECORD",
  "RECORD_CASE_PART", "RECORD_CASE_VARIANT",
  "RECORD_CASE_LABEL_ELEMENT", "RECORD_CASE_LABEL_RANGE",
  "BASIC_TYPE"
};


/* Printed names of icode pointers in icode nodes.
   See icode nodes declaration.  ICPN is abbreviation of
   icode pointer names. */


/* The following for constant values (for cardinal_node,
   integer_node and etc.) */

static char
*ICPN_in_value_nodes[] =
{"value_type"};

static char *ICPN_in_binary_operation_nodes[] =
{"value_type", "left_operand", "right_operand"};

static char *ICPN_in_unary_opertion_nodes[] =
{"value_type", "operand"};

static char *ICPN_in_case_variant_nodes[] =
{"case_label_list", "case_variant_elements", "next_case_variant"};

static char *ICPN_in_label_element_nodes[] =
{"case_label_list", "case_label_type", "case_label_value"};

static char *ICPN_in_label_range_nodes[] =
{"case_label_list", "case_label_type", "case_label_range_left_bound",
   "case_label_range_right_bound"};

static char *ICPN_in_function_call_node[] =
{"value_type", "function_designator", "function_actual_parameter_list"};

static char *ICPN_in_assignment_node[] =
{"next_statement", "assignment_variable", "assignment_expression"};

static char *ICPN_in_procedure_call_node[] =
{"next_statement", "procedure_designator", "procedure_actual_parameter_list"};

static char *ICPN_in_actual_parameter_node[] =
{"actual_parameter_expression", "next_actual_parameter"};

static char *ICPN_in_block_begin_node[] =
{"next_statement", "block_scope", "next_block_begin"};

static char *ICPN_in_return_without_result_node[] =
{"next_statement"};

static char *ICPN_in_return_with_result_node[] =
{"next_statement", "returned_expression"};

static char *ICPN_in_loop_statement_node[] =
{"next_statement", "loop_statements"};

static char *ICPN_in_exit_statement_node[] =
{"next_statement"};

static char *ICPN_in_while_statement_node[] =
{"next_statement", "while_expression", "while_statements"};

static char *ICPN_in_repeat_statement_node[] =
{"next_statement", "until_expression", "repeat_statements"};

static char *ICPN_in_for_statement_node[] =
{"next_statement", "for_control_variable", "for_starting_value", "for_limit",
   "for_increment", "for_statements"};

static char *ICPN_in_if_statement_node[] =
{"next_statement", "if_expression", "if_continuation", "if_statements"};

static char *ICPN_in_elsif_clause_node[] =
{"elsif_expression", "elsif_continuation", "elsif_statements"};

static char *ICPN_in_case_statement_node[] =
{"next_statement", "case_expression", "case_statement_variant_list"};

static char *ICPN_in_with_statement_node[] =
{"next_statement", "with_designator", "with_statements"};

static char *ICPN_in_C_code_in_statements_node[] =
{"next_statement"};

static char *ICPN_in_C_code_in_declarations_node[] =
{"next_declaration_element"};

static char *ICPN_in_export_node[] =
{"next_declaration_element", "scope", "identifier", "module_exporter",
   "export_list"};

static char *ICPN_in_qualified_export_node[] =
{"next_declaration_element", "scope", "identifier"};

static char *ICPN_in_import_node[] =
{"next_declaration_element", "scope", "identifier", "module_exporter"};

static char *ICPN_in_field_node[] =
{"next_declaration_element", "scope", "identifier", "next_field",
   "field_type"};

static char *ICPN_in_formal_parameter_node[] =
{"next_declaration_element", "scope", "identifier", "formal_parameter_type"};

static char *ICPN_in_module_node[] =
{"next_declaration_element", "scope", "identifier", "local_module_list",
   "export_list", "block_begin",
   "import_list", "module_brother", "module_priority"};

static char *ICPN_in_procedure_node[] =
{"next_declaration_element", "scope", "identifier", "local_module_list",
   "procedure_type", "block_begin"};

static char *ICPN_in_variable_declaration_node[] =
{"next_declaration_element", "scope", "identifier",
   "variable_type"};

static char *ICPN_in_constant_declaration_node[] =
{"next_declaration_element", "scope", "identifier", "constant_expression"};

static char *ICPN_in_type_declaration_node[] =
{"next_declaration_element", "scope", "identifier", "type_definition"};

static char *ICPN_in_C_function_node[] =
{"next_declaration_element", "scope", "identifier"};

static char *ICPN_in_enumeration_type_node[] =
{"first_enumeration_constant"};

static char *ICPN_in_range_type_node[] =
{"base_type", "left_range_bound", "right_range_bound"};

static char *ICPN_in_set_type_node[] =
{"base_type"};

static char *ICPN_in_pointer_type_node[] =
{"base_type"};

static char *ICPN_in_array_type_node[] =
{"base_type", "array_index_type"};

static char *ICPN_in_procedure_type_node[] =
{"partplist", "procedure_result_type"};

static char *ICPN_in_procedure_parameter_type_node[] =
{"partplist", "empty1", "empty2", "formal_parameter_type"};

static char *ICPN_in_record_node[] =
{"record_element_list", "scope", "record_type_identifier"};

static char *ICPN_in_record_case_part_node[] =
{"case_tag_type", "record_case_variant_list",
   "record_element_after_case_part"};


/* The table of icode pointer names in all icode nodes.  The table element
   order is the same as in enumeration icode_node_mode. */

static char **ICPN_in_node_table[] =
{
  ICPN_in_value_nodes, ICPN_in_value_nodes, ICPN_in_value_nodes,
  ICPN_in_value_nodes, NULL, ICPN_in_value_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_unary_opertion_nodes,
  ICPN_in_unary_opertion_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_unary_opertion_nodes,
  ICPN_in_binary_operation_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_unary_opertion_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_unary_opertion_nodes, ICPN_in_binary_operation_nodes,
  ICPN_in_function_call_node, ICPN_in_assignment_node,
  ICPN_in_procedure_call_node, ICPN_in_actual_parameter_node,
  ICPN_in_block_begin_node, ICPN_in_return_without_result_node,
  ICPN_in_return_with_result_node, ICPN_in_loop_statement_node,
  ICPN_in_exit_statement_node,
  ICPN_in_while_statement_node, ICPN_in_repeat_statement_node,
  ICPN_in_for_statement_node, ICPN_in_if_statement_node,
  ICPN_in_elsif_clause_node,
  ICPN_in_case_statement_node, ICPN_in_with_statement_node,
  ICPN_in_case_variant_nodes, ICPN_in_label_element_nodes,
  ICPN_in_label_range_nodes,
  ICPN_in_C_code_in_statements_node, ICPN_in_C_code_in_declarations_node,
  ICPN_in_export_node, ICPN_in_qualified_export_node, ICPN_in_import_node,
  ICPN_in_field_node, ICPN_in_formal_parameter_node, ICPN_in_module_node,
  ICPN_in_procedure_node, ICPN_in_variable_declaration_node,
  ICPN_in_constant_declaration_node, ICPN_in_type_declaration_node,
  ICPN_in_C_function_node,
  ICPN_in_enumeration_type_node, ICPN_in_range_type_node,
  ICPN_in_set_type_node, ICPN_in_pointer_type_node,
  ICPN_in_array_type_node, ICPN_in_procedure_type_node,
  ICPN_in_procedure_parameter_type_node, ICPN_in_record_node,
  ICPN_in_record_case_part_node, ICPN_in_case_variant_nodes,
  ICPN_in_label_element_nodes, ICPN_in_label_range_nodes, NULL
};


/* Print NPOINTER-th icode pointer with value REF in icode node with
   mode NODE_MODE.  If REF is NULL print is not executed. */

static void
print_icode_node_pointer (ref, npointer, node_mode)
     ICN_pointer ref;
     int npointer;
     enum icode_node_mode node_mode;
{
  if (ref != NULL)
    {
      if (npointer == 2 || npointer == 5 || npointer == 8)
	fprintf (stderr, "\n      ");
      fprintf (stderr, "%s=%5lu ",
	       ICPN_in_node_table[(int) node_mode][npointer - 1],
	       (unsigned long) ref);
    }
}


/* Print icode node starting with REF. */

void
print_node (ref)
     register ICN_pointer ref;
{
  register int i, npointer;
  register enum icode_node_mode node_mode;
  Treal real_value;
  ICN_pointer *pointer_to_first_icode_pointer_in_node;

  node_mode = MODE (ref);
  i = ICN_POINTERS_IN_NODE (node_mode);
  fprintf (stderr, "mode=%2d(%6s) adr=%10lu ", (int) node_mode,
	   printed_icode_node_names[(int) node_mode], (unsigned long) ref);
  fprintf (stderr, "in_definition_module=%s line=%4d nel=%5d",
	   (IN_DEFINITION_MODULE (ref) ? "TRUE" : "FALSE"), LINE (ref),
	   C_STRING_CONSTANT_NUMBER (ref));
  pointer_to_first_icode_pointer_in_node
    = GENERIC_NODE (ref)->array_of_all_node_pointers;
  for (npointer = 1; i > 0; i--, npointer++)
    print_icode_node_pointer
      (pointer_to_first_icode_pointer_in_node[npointer - 1], npointer,
       node_mode);
  switch (node_mode)
    {
    case ICNM_INTEGER:
      fprintf (stderr, "integer_value=%lld", INTEGER_NODE (ref)->integer_value);
      break;
    case ICNM_CARDINAL:
      fprintf (stderr, "cardinal_value=%llu",
	       CARDINAL_NODE (ref)->cardinal_value);
      break;
    case ICNM_REAL:
      real_value = REAL_NODE (ref)->real_value;
      fprintf (stderr, "real_value=%.20e", real_value);
      break;
    case ICNM_STRING:
      fprintf (stderr, "string_value=%s", STRING_NODE (ref)->string_value);
      break;
    case ICNM_IDENTIFIER:
      fprintf (stderr, "identifier_string=%s",
	       IDENTIFIER_NODE (ref)->identifier_string);
      break;
    case ICNM_SET:
      fprintf (stderr, "set_value=%#o", SET_NODE (ref)->set_value);
      break;
    case ICNM_C_CODE_IN_STATEMENTS:
    case ICNM_C_CODE_IN_DECLARATIONS:
      fprintf (stderr, "ctext=%s", C_TEXT (ref));
      break;
    case ICNM_QUALIFIED_EXPORT:
      fprintf (stderr, "opaque_type=%s ",
	       (QUALIFIED_EXPORT_NODE (ref)->opaque_type ? "TRUE" : "FALSE"));
      break;
    case ICNM_MODULE:
      fprintf (stderr, "module_mode=%1d", MODULE_NODE (ref)->module_mode);
      break;
    case ICNM_PROCEDURE_PARAMETER_TYPE:
    case ICNM_FORMAL_PARAMETER:
      fprintf (stderr, "var_formal_parameter=%5s array_formal_parameter=%5s",
	       (VAR_FORMAL_PARAMETER (ref) ? "TRUE" : "FALSE"),
	       (ARRAY_FORMAL_PARAMETER (ref) ? "TRUE" : "FALSE"));
      break;
    case ICNM_BASIC_TYPE:
      fprintf (stderr, "basic_type=%d", BASIC_TYPE_NODE (ref)->basic_type);
      break;
    }
  fputc ('\n', stderr);
}
