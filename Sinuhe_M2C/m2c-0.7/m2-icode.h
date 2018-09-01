/* Header file for work with intermediata code.
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

/* Intermediate code (icode) of Modula-2 program is a directed graph
   (possibly cyclic).  A graph node is implemented by C structure.
   Node represents Modula-2 construction of program which is divided on
   the constructions according to Modula-2 abstract syntax.  For example,
   node less or equal operation represents as numeric operation as set
   inclusion operation.

   Value of node pointer to erroneous (syntactically or semantically)
   constructions is NULL.  But this value may be also used for representation
   of correct program.

   After semantic analysis identifiers are changed by corresponding
   declaration and constant expressions are folded. */


/* First member of structure representing any intermediate code
   node is its mode.  ICNM is abbreviation of icode node mode.  The order of
   enumeration constants is important.  It is used in
   characteristics_node_table and ICPN_in_node_table. */

enum icode_node_mode
{
  ICNM_INTEGER,
  ICNM_CARDINAL,
  ICNM_REAL,
  ICNM_STRING,
  ICNM_IDENTIFIER,
  ICNM_SET,
  ICNM_EQUAL,
  ICNM_UNEQUAL,
  ICNM_LESS_THAN,
  ICNM_GREATER_THAN,
  ICNM_LESS_OR_EQUAL,
  ICNM_GREATER_OR_EQUAL,
  ICNM_IN,
  ICNM_IDENTITY,
  ICNM_SIGN_CONVERSION,
  ICNM_ADDITION,
  ICNM_SUBTRACTION,
  ICNM_OR,
  ICNM_AND,
  ICNM_MULT,
  ICNM_REAL_DIVISION,
  ICNM_DIV,
  ICNM_MOD,
  ICNM_NOT,
  ICNM_QUALIFICATION,
  ICNM_INDEXING,
  ICNM_DEREFERENCING,
  ICNM_SET_VALUE_START,
  ICNM_ELEMENT_IN_SET_VALUE,
  ICNM_RANGE_IN_SET_VALUE,
  ICNM_FUNCTION_CALL,
  ICNM_ASSIGNMENT,
  ICNM_PROCEDURE_CALL,
  ICNM_ACTUAL_PARAMETER,
  ICNM_BLOCK_BEGIN,
  ICNM_RETURN_WITHOUT_RESULT,
  ICNM_RETURN_WITH_RESULT,
  ICNM_LOOP_STATEMENT,
  ICNM_EXIT_STATEMENT,
  ICNM_WHILE_STATEMENT,
  ICNM_REPEAT_STATEMENT,
  ICNM_FOR_STATEMENT,
  ICNM_IF_STATEMENT,
  ICNM_ELSIF_CLAUSE,
  ICNM_CASE_STATEMENT,
  ICNM_WITH_STATEMENT,
  ICNM_CASE_STATEMENT_VARIANT,
  ICNM_CASE_STATEMENT_LABEL_ELEMENT,
  ICNM_CASE_STATEMENT_LABEL_RANGE,
  ICNM_C_CODE_IN_STATEMENTS,
  ICNM_C_CODE_IN_DECLARATIONS,
  ICNM_EXPORT,
  ICNM_QUALIFIED_EXPORT,
  ICNM_IMPORT,
  ICNM_FIELD,
  ICNM_FORMAL_PARAMETER,
  ICNM_MODULE,
  ICNM_PROCEDURE,
  ICNM_VARIABLE_DECLARATION,
  ICNM_CONSTANT_DECLARATION,
  ICNM_TYPE_DECLARATION,
  ICNM_C_FUNCTION,
  ICNM_ENUMERATION_TYPE,
  ICNM_RANGE_TYPE,
  ICNM_SET_TYPE,
  ICNM_POINTER_TYPE,
  ICNM_ARRAY_TYPE,
  ICNM_PROCEDURE_TYPE,
  ICNM_PROCEDURE_PARAMETER_TYPE,
  ICNM_RECORD,
  ICNM_RECORD_CASE_PART,
  ICNM_RECORD_CASE_VARIANT,
  ICNM_RECORD_CASE_LABEL_ELEMENT,
  ICNM_RECORD_CASE_LABEL_RANGE,
  ICNM_BASIC_TYPE
};


/* Icode node ICNM_BASIC_TYPE has member of given enumeration type.
   BTM is abbreviation of basic type mode. */

enum basic_type
{
  BTM_SHORT_CARDINAL,
  BTM_CARDINAL,
  BTM_LONG_CARDINAL,
  BTM_SHORT_INTEGER,
  BTM_INTEGER,
  BTM_LONG_INTEGER,
  BTM_SHORT_REAL,
  BTM_REAL,
  BTM_LONG_REAL,
  /* It is important that the following enumeration constant represents
     last basic type for which standard Modula-2 MIN and MAX functions are
     defined (see function min_or_max). */
  BTM_CHAR,
  BTM_BYTE,
  BTM_WORD,
  /* BTM_NIL type is compatible with any pointer type. */
  BTM_NIL,
  /* These types are used only for integer numbers
     which may be considered as of integer or cardinal types. */
  BTM_SHORT_CARDINAL_OR_INTEGER,
  BTM_CARDINAL_OR_INTEGER,
  BTM_LONG_CARDINAL_OR_INTEGER,
  /* Type of empty string.  Another string has type with mode
     BTM_EMPTY_STRING + length of this string. */
  BTM_EMPTY_STRING
};


/* Icode node ICNM_MODULE has member of given enumeration type.
   MM is abbreviation of module mode. */

enum module_mode
{
  MM_LOCAL_MODULE,
  MM_MAIN_MODULE,
  MM_DEFINITION_MODULE,
  /* The following is used for implementation module.
     Corresponding definition module is to be compiled. */
  MM_DEFINITION_WITH_IMPLEMENTATION_MODULE
};


/* Type representing pointer to any icode node.  All icode node members
   representing icode node reference is of this type. */

typedef struct generic_node *ICN_pointer;


/* Macros for transformation of given node reference to previously
   mentioned type. */

#define ICN_POINTER(v) ((ICN_pointer)(v))


/* Enumeration constants of icode_node_mode are stored in icode nodes
   as the following type for memory economy.  ICN is abbreviation
   of icode node. */

typedef char ICN_mode;


/* Type representing flag of declaration presence (except for opaque type)
   in definition module. */

typedef char ICN_in_definition_module;


/* Type representing line number of Modula-2 construction corresponding to
   given icode node.  For generation time icode node member of this type
   also stores current block number in procedure type node (if procedure
   with given type is called in the block) or string constant number
   used in object file. */

typedef int ICN_line;


/* Members in different structures but with the same name have common sense
   and must have the same displacement in the structures.  According to this
   the translator code is simpler and more economic (see commentaries for
   the member access macros).

   Structure of nodes is common members (mode and etc.), pointers to
   icode nodes and after all miscellaneous members.  If the number of
   icode pointers is changed than the corresponding element in characteristics
   node table must be changed. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  Tint integer_value;
} integer_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  Tcard cardinal_value;
} cardinal_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  Treal real_value;
} real_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  char *string_value;
  /* The following member is used for generation time. */
  char string_declaration_was_generated;
} string_node;


/* After semantic analysis reference from any icode node to the following node
   is changed on reference to node representing the corresponding object
   (e.g. type definition or variable). */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  char *identifier_string;
} identifier_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  Tset set_value;
} set_node;


/* The following structure is representing all binary operation nodes
   in Modula-2. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  ICN_pointer left_operand, right_operand;
} binary_operation_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type, operand;
} unary_operation_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer value_type;
  /* Reference to designator that defines the called function. */
  ICN_pointer function_designator;
  /* The following member refers to actual parameter node. */
  ICN_pointer function_actual_parameter_list;
} function_call_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement, assignment_variable, assignment_expression;
} assignment_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  /* See commentaries for function call node. */
  ICN_pointer procedure_designator, procedure_actual_parameter_list;
} procedure_call_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer actual_parameter_expression, next_actual_parameter;
} actual_parameter_node;


/* Block is a procedure or module statement body. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  /* Reference to corresponding module or procedure node. */
  ICN_pointer block_scope;
  /* Reference to block begin node representing textually next block. */
  ICN_pointer next_block_begin;
} block_begin_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
} return_without_result_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  /* Reference to expression after keyword return. */
  ICN_pointer returned_expression;
} return_with_result_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  ICN_pointer loop_statements;
} loop_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
} exit_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  ICN_pointer while_expression;
  /* Reference to statements in given while-statement. */
  ICN_pointer while_statements;
} while_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  ICN_pointer until_expression;
  /* Reference to statements in given repeat-statement. */
  ICN_pointer repeat_statements;
} repeat_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  /* Reference to identifier node (afterwards to variable declaration node). */
  ICN_pointer for_control_variable;
  ICN_pointer for_starting_value, for_limit, for_increment;
  /* Reference to statements in given for-statement. */
  ICN_pointer for_statements;
} for_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  ICN_pointer if_expression;
  /* Reference to elsif clause node or to
     any statement node (if only else-part is present). */
  ICN_pointer if_continuation;
  /* Reference to statements after corresponding keyword then. */
  ICN_pointer if_statements;
} if_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer elsif_expression;
  /* Reference to elsif clause node or to
     any statement node (if only else-part is present). */
  ICN_pointer elsif_continuation;
  /* Reference to statements after corresponding keyword then. */
  ICN_pointer elsif_statements;
} elsif_clause_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  ICN_pointer case_expression;
  /* List of case variant nodes. */
  ICN_pointer case_statement_variant_list;
} case_statement_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  /* Reference to subgraph representing variable after keyword with. */
  ICN_pointer with_designator;
  /* Reference to statements in given with-statement. */
  ICN_pointer with_statements;
} with_statement_node;


/* The following structure representing case statement variant nodes
   and record case variant nodes. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* The following member is not used for clause else. */
  ICN_pointer case_label_list;
  /* Reference to field node or statements node. */
  ICN_pointer case_variant_elements;
  /* Reference to next (record or statement) case variant node.
     The last node refers to corresponding case statement node or record case
     part node (i.e. the list is cyclic). */
  ICN_pointer next_case_variant;
} case_variant_node;


/* The following structure representing case statement label element nodes
   and record case label element nodes. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer case_label_list, case_label_type;
  /* Reference to subgraph representing case label value. */
  ICN_pointer case_label_value;
} case_label_element_node;


/* The following structure representing case statement label range nodes
   and record case label range nodes. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer case_label_list, case_label_type;
  /* References to subgraphs representing left and right bounds of
     case label range. */
  ICN_pointer case_label_range_left_bound, case_label_range_right_bound;
} case_label_range_node;


/* The following node represents fragment of C language text in
   statement parts of the program. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_statement;
  char *C_text;
} C_code_in_statements_node;


/* The following node represents fragment of C language text in
   definition parts of the program. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element;
  char *C_text;
} C_code_in_declarations_node;


/* The following node represents nonqualified export of object in scope
   to which the object is exported, i.e. the object self and corresponding
   export node are in different scopes. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  ICN_pointer module_exporter, export_list;
} export_node;


/* The following node represents qualified export of object in scope
   in which the object is declared, i.e. the object self (or other object
   export) and corresponding export node are in the same scopes.
   But the qualified export node is accessible through second name space.

   After definition module compilation but after implementation module
   compilation for opaque type only qualified export node exists.
   Also For economy memory qualified export node is absent for objects
   (different from opaque type) declared in definition module.
   But they are accessible through two name spaces in any case.  The member
   in_definition_module is equal to TRUE for these nodes. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  /* The following member is equal to TRUE
     for node representing opaque type. */
  char opaque_type;
} qualified_export_node;


/* The following node represents import of object.  This node is in scope
   in which import clause is present.  The node is accessible through
   main name space. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier, module_exporter;
} import_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  /* Reference to field node or record case part node. */
  ICN_pointer next_field;
  /* References to subgraphs representing field type. */
  ICN_pointer field_type;
} field_node;


 /* For economy memory the following node represents formal parameter and
    its type.  I.e. parameter type node is used only for describing
    parameter characteristics in procedure type definition. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  ICN_pointer formal_parameter_type;
  char var_formal_parameter;
  char array_formal_parameter;
} formal_parameter_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  ICN_pointer local_module_list, export_list, block_begin;
  /* Reference to first import node representing first import element
     in given module. */
  ICN_pointer import_list;
  /* The following member is defined only for local module and representing
     next local module in the same scope
     (and at the same declaration level). */
  ICN_pointer module_brother;
  /* Reference to subgraph representing module priority. */
  ICN_pointer module_priority;
  /* See enumeration module_mode. */
  char module_mode;
  /* The following member is used for generation time. */
  char module_declaration_was_generated;
} module_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier, local_module_list;
  /* Reference to corresponding procedure type node. */
  ICN_pointer procedure_type;
  ICN_pointer block_begin;
  /* The following member is used for generation time.  If the member value
     is TRUE then given procedure may be called implicitly or explicitly
     from main module body. */
  char procedure_is_used;
} procedure_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  /* Reference to subgraph representing variable type. */
  ICN_pointer variable_type;
  /* The following members are used for semantic analysis time for
   search of declarations cycle.  Remember that in given Modula-2
   implementation any identifier may be used before its declaration. */
  char variable_is_being_processed, variable_was_processed;
} variable_declaration_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  /* Reference to subgraph representing constant expression.  After semantic
     analysis the subgraph is folded to single node representing
     expression value. */
  ICN_pointer constant_expression;
  /* See commentaries for corresponding members
     in variable declaration node. */
  char constant_is_being_processed, constant_was_processed;
} constant_declaration_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element, scope, identifier;
  /* Reference to subgraph representing given type definition. */
  ICN_pointer type_definition;
  /* See commentaries for corresponding members
     in variable declaration node. */
  char type_is_being_processed, type_was_processed;
} type_declaration_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* Scope of this node is module CPROC. */
  ICN_pointer next_declaration_element, scope, identifier;
} C_function_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* Reference to constant declaration node representing
     first enumeration literal.  Next enumeration literals are
     accessible through member next_declaration_element. */
  ICN_pointer first_enumeration_constant;
  /* The following member is used for generation time if option `-test' is
     given  in command line. */
  char enumeration_value_test_function_was_generated;
} enumeration_type_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer base_type;
  /* */
  ICN_pointer left_range_bound, right_range_bound;
  /* The following member is used for generation time if option `-test' is
     given  in command line. */
  char range_test_function_was_generated;
} range_type_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer base_type;
} set_type_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer base_type;
  /* The following members are used for generation time and  if Modula-2
     pointer type may be represented in C only as a structure.  For example,
       TYPE
         p1: POINTER TO p2;
         p2: POINTER TO p1;
   */
  char corresponding_C_structure_was_generated, pointer_is_being_processed;
} pointer_type_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer base_type;
  /* Reference to subgraph representing array index type. */
  ICN_pointer array_index_type;
} array_type_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element;
  /* Reference to subgraph representing type of procedure-function result. */
  ICN_pointer procedure_result_type;
  /* The following member is used for generation time.  Modula-2 procedure
     parameters are passed as a single C structure. */
  char corresponding_C_parameter_structure_was_generated;
} procedure_type_node;


/* See commentaries for formal parameter node. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  ICN_pointer next_declaration_element;
  /* Unused members is used for the following members to have the same
     displacement as in other structures. */
  ICN_pointer empty1, empty2;
  ICN_pointer formal_parameter_type;
  char var_formal_parameter;
  char array_formal_parameter;
} procedure_parameter_type_node;


/* The following node determines new scope, i.e. members scope may refer
   to this node. */

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* Reference to filed node or record case part node. */
  ICN_pointer record_element_list;
  ICN_pointer scope;
  /* The following node refers to record type identifier
     (if it is defined). */
  ICN_pointer record_type_identifier;
  /* The following member is used only for generation time. */
  char corresponding_C_structure_was_generated;
} record_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* Reference to subgraph representing type given after keyword case.
     Corresponding case tag field is predecessor of given node in list on
     record_element_after_case_part, next_field. */
  ICN_pointer case_tag_type;
  /* Reference to first record case variant node.  The last list node refers
     to given node (i.e. the list is cyclic). */
  ICN_pointer record_case_variant_list;
  /* Reference to field node or record case part node after given case part. */
  ICN_pointer record_element_after_case_part;
} record_case_part_node;

typedef struct
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* See commentaries for enumeration basic_type. */
  enum basic_type basic_type;
  /* For alignment on 64-bit machines. */
  ICN_pointer empty;
} basic_type_node;

struct generic_node
{
  ICN_mode mode;
  ICN_in_definition_module in_definition_module;
  ICN_line line;
  /* The length of the following array is equal to maximum number of ICN
     pointers in icode nodes. */
  ICN_pointer array_of_all_node_pointers[10];
};

typedef struct generic_node generic_node;


/* Macros for transformation generic type node pointer type (ICN_pointer)
   to pointer to structure representing concrete icode node. */

#define INTEGER_NODE(v) ((integer_node *)(v))
#define CARDINAL_NODE(v) ((cardinal_node *)(v))
#define REAL_NODE(v) ((real_node *)(v))
#define STRING_NODE(v) ((string_node *)(v))
#define IDENTIFIER_NODE(v) ((identifier_node *)(v))
#define SET_NODE(v) ((set_node *)(v))

#define EQUAL_NODE(v) ((binary_operation_node *)(v))
#define UNEQUAL_NODE(v) ((binary_operation_node *)(v))
#define LESS_THAN_NODE(v) ((binary_operation_node *)(v))
#define GREATER_THAN_NODE(v) ((binary_operation_node *)(v))
#define LESS_OR_EQUAL_NODE(v) ((binary_operation_node *)(v))
#define GREATER_OR_EQUAL_NODE(v) ((binary_operation_node *)(v))
#define IN_NODE(v) ((binary_operation_node *)(v))
#define IDENTITY_NODE(v) ((unary_operation_node *)(v))
#define SIGN_CONVERSION_NODE(v) ((unary_operation_node *)(v))
#define ADDITION_NODE(v) ((binary_operation_node *)(v))
#define SUBTRACTION_NODE(v) ((binary_operation_node *)(v))
#define OR_NODE(v) ((binary_operation_node *)(v))
#define AND_NODE(v) ((binary_operation_node *)(v))
#define MULT_NODE(v) ((binary_operation_node *)(v))
#define REAL_DIVISION_NODE(v) ((binary_operation_node *)(v))
#define DIV_NODE(v) ((binary_operation_node *)(v))
#define MOD_NODE(v) ((binary_operation_node *)(v))
#define NOT_NODE(v) ((unary_operation_node *)(v))
#define QUALIFICATION_NODE(v) ((binary_operation_node *)(v))
#define INDEXING_NODE(v) ((binary_operation_node *)(v))
#define DEREFERENCING_NODE(v) ((unary_operation_node *)(v))
#define SET_VALUE_START_NODE(v) ((binary_operation_node *)(v))
#define ELEMENT_IN_SET_VALUE_NODE(v) ((unary_operation_node *)(v))
#define RANGE_IN_SET_VALUE_NODE(v) ((binary_operation_node *)(v))

#define FUNCTION_CALL_NODE(v) ((function_call_node *)(v))
#define ASSIGNMENT_NODE(v) ((assignment_node *)(v))
#define PROCEDURE_CALL_NODE(v) ((procedure_call_node *)(v))
#define ACTUAL_PARAMETER_NODE(v) ((actual_parameter_node *)(v))
#define BLOCK_BEGIN_NODE(v) ((block_begin_node *)(v))
#define RETURN_WITHOUT_RESULT_NODE(v) ((return_without_result_node *)(v))
#define RETURN_WITH_RESULT_NODE(v) ((return_with_result_node *)(v))
#define LOOP_STATEMENT_NODE(v) ((loop_statement_node *)(v))
#define EXIT_STATEMENT_NODE(v) ((exit_statement_node *)(v))
#define WHILE_STATEMENT_NODE(v) ((while_statement_node *)(v))
#define REPEAT_STATEMENT_NODE(v) ((repeat_statement_node *)(v))
#define FOR_STATEMENT_NODE(v) ((for_statement_node *)(v))
#define IF_STATEMENT_NODE(v) ((if_statement_node *)(v))
#define ELSIF_CLAUSE_NODE(v) ((elsif_clause_node *)(v))
#define CASE_STATEMENT_NODE(v) ((case_statement_node *)(v))
#define WITH_STATEMENT_NODE(v) ((with_statement_node *)(v))

#define CASE_VARIANT_NODE(v) ((case_variant_node *)(v))
#define CASE_LABEL_ELEMENT_NODE(v) ((case_label_element_node *)(v))
#define CASE_LABEL_RANGE_NODE(v) ((case_label_range_node *)(v))
#define CASE_STATEMENT_VARIANT_NODE(v) CASE_VARIANT_NODE(v)
#define CASE_STATEMENT_LABEL_ELEMENT_NODE(v) CASE_LABEL_ELEMENT_NODE(v)
#define CASE_STATEMENT_LABEL_RANGE_NODE(v) CASE_LABEL_RANGE_NODE(v)

#define C_CODE_IN_STATEMENTS_NODE(v) ((C_code_in_statements_node *)(v))
#define C_CODE_IN_DECLARATIONS_NODE(v) ((C_code_in_declarations_node *)(v))
#define EXPORT_NODE(v) ((export_node *)(v))
#define QUALIFIED_EXPORT_NODE(v) ((qualified_export_node *)(v))
#define IMPORT_NODE(v) ((import_node *)(v))
#define FIELD_NODE(v) ((field_node *)(v))
#define MODULE_NODE(v) ((module_node *)(v))
#define PROCEDURE_NODE(v) ((procedure_node *)(v))
#define VARIABLE_DECLARATION_NODE(v) ((variable_declaration_node *)(v))
#define CONSTANT_DECLARATION_NODE(v) ((constant_declaration_node *)(v))
#define TYPE_DECLARATION_NODE(v) ((type_declaration_node *)(v))

#define C_FUNCTION_NODE(v) ((C_function_node *)(v))

#define ENUMERATION_TYPE_NODE(v) ((enumeration_type_node *)(v))
#define RANGE_TYPE_NODE(v) ((range_type_node *)(v))
#define SET_TYPE_NODE(v) ((set_type_node *)(v))
#define POINTER_TYPE_NODE(v) ((pointer_type_node *)(v))
#define ARRAY_TYPE_NODE(v) ((array_type_node *)(v))
#define PROCEDURE_TYPE_NODE(v) ((procedure_type_node *)(v))
#define PROCEDURE_PARAMETER_TYPE_NODE(v) ((procedure_parameter_type_node *)(v))
#define RECORD_NODE(v) ((record_node *)(v))
#define RECORD_CASE_PART_NODE(v) ((record_case_part_node *)(v))
#define RECORD_CASE_VARIANT_NODE(v) CASE_VARIANT_NODE(v)
#define RECORD_CASE_LABEL_ELEMENT_NODE(v) CASE_LABEL_ELEMENT_NODE(v)
#define RECORD_CASE_LABEL_RANGE_NODE(v) CASE_LABEL_RANGE_NODE(v)
#define BASIC_TYPE_NODE(v) ((basic_type_node *)(v))
#define GENERIC_NODE(v) ((generic_node *)(v))


/* Macros for access to common members of nodes (with the same name).
   These members have the same displacement from the corresponding structure
   begin. */


/* See commentaries for enumeration icode_node_mode. */

#define MODE(v) ((enum icode_node_mode) GENERIC_NODE(v)->mode)


/* The following macros is used for denotation (it has members scope and
   identifier). See commentaries for export qualified node. */

#define IN_DEFINITION_MODULE(v) GENERIC_NODE(v)->in_definition_module


/* See commentaries for typedef ICN_line. */

#define LINE(v) GENERIC_NODE(v)->line


/* The following macros is used for nodes representing operations and
   constants. The macro call value is reference to type of
   operation result or constant. */

#define VALUE_TYPE(v) INTEGER_NODE(v)->value_type


/* The following macros is used for nodes representing binary operations. */

#define LEFT_OPERAND(v) EQUAL_NODE(v)->left_operand


/* The following macros is used for nodes representing binary operations. */

#define RIGHT_OPERAND(v) EQUAL_NODE(v)->right_operand


/* The following macros is used for nodes representing unary operations. */

#define OPERAND(v) IDENTITY_NODE(v)->operand


/* The following macros is used for nodes representing statements.
   The macro call value is pointer to node representing next statement on
   the same nest level. */

#define NEXT_STATEMENT(v) ASSIGNMENT_NODE(v)->next_statement


/* The following macros is used for access to list of record case label
   element, record case label range, case statement label element or
   case statement label range nodes.  The list start with corresponding
   case statement variant or record case variant node. */

#define CASE_LABEL_LIST(v) CASE_LABEL_ELEMENT_NODE(v)->case_label_list

/* The following macros is used for record case label element, record case
   label range, case statement label element or case statement label range
   nodes.  The macro call value is pointer to node representing
   type corresponding labels. */

#define CASE_LABEL_TYPE(v) CASE_LABEL_ELEMENT_NODE(v)->case_label_type


/* The following macros is used for C code in declarations node or
   C code in statements node. */

#define C_TEXT(v) C_CODE_IN_STATEMENTS_NODE(v)->C_text


/* The following macros is used for denotations.  The macro call value is
   pointer to nodes representing module, procedure or record in which the
   denotation is immediately placed.  Such nodes open new scope. */

#define SCOPE(v) EXPORT_NODE(v)->scope


/* The following macros is used for denotations.  The macro call value is
   pointer to identifier node representing name of the denotation. */

#define IDENTIFIER(v) EXPORT_NODE(v)->identifier


/* The following macros is used for export or import node.  The macro call
   value is pointer to module-exporter. The value (for import node) may be
   also pointer to identifier node representing name of the module-exporter.
   But the pointer is changed on pointer to the module for semantic
   analysis time. */

#define MODULE_EXPORTER(v) EXPORT_NODE(v)->module_exporter


/* The following macros is used for access to list of export nodes
   from the module.  The macro parameter may be module node (exporter)
   or export node. */

#define EXPORT_LIST(v) EXPORT_NODE(v)->export_list


/* The following macros is used for access to one level list of all compilation
   unit declaration nodes.

   It is important that nodes representing enumeration constants, formal
   parameters and procedure parameter types are placed successively in the
   list.  One qualified export node may be between successive enumeration
   constants (in definition module). */

#define NEXT_DECLARATION_ELEMENT(v) \
  C_CODE_IN_DECLARATIONS_NODE(v)->next_declaration_element


/* The following macros is used for access to first element in list of
   local modules which is immediately places in given module procedure node.
   See also commentaries for member module_brother in module node. */

#define LOCAL_MODULE_LIST(v) MODULE_NODE(v)->local_module_list


/* The following macros is used for module or procedure node.  The macro call
   value is pointer to block begin node representing corresponding module or
   procedure body. */

#define BLOCK_BEGIN(v) MODULE_NODE(v)->block_begin


/* The following macros is used for range type node, pointer type node or
   set type node. */

#define BASE_TYPE(v) RANGE_TYPE_NODE(v)->base_type


/* The following macros is used for formal parameter node or
   procedure parameter type node.  The macro call value is node representing
   type of corresponding parameter. */

#define FORMAL_PARAMETER_TYPE(v) \
  PROCEDURE_PARAMETER_TYPE_NODE(v)->formal_parameter_type


/* The following macros is used for formal parameter node or
   procedure parameter type node.  See also commentaries for
   formal parameter node. */

#define VAR_FORMAL_PARAMETER(v) \
  PROCEDURE_PARAMETER_TYPE_NODE(v)->var_formal_parameter


/* The following macros is used for formal parameter node or
   procedure parameter type node.  See also commentaries for
   formal parameter node. */

#define ARRAY_FORMAL_PARAMETER(v) \
  PROCEDURE_PARAMETER_TYPE_NODE(v)->array_formal_parameter

#define SET_MODE(v, m) (GENERIC_NODE(v)->mode = (ICN_mode) (m))


/* Macros for access to virtual members of nodes. */


/* The value of the following macro call is number of last compilated block
   in which call of procedure with given type is processed (see commentaries
   for procedure type node.  The parameter of the macro is
   a procedure type node pointer. */

#define LAST_BLOCK_USING_PROCEDURE_TYPE(v) GENERIC_NODE(v)->line


/* The value of the following macro call is present in string name in
   object text (text on C language).  The parameter of the macro is
   a string node pointer. */

#define C_STRING_CONSTANT_NUMBER(v) GENERIC_NODE(v)->line


/* Macros for access to node representing parameter type of given formal
   parameter (see commentaries for formal parameter node).  The parameter of
   the macro is a formal parameter node pointer. */

#define PARAMETER_TYPE_OF_FORMAL_PARAMETER(v) (v)




/* Size of structure representing given node. */

#define NODE_SIZE(mode) (characteristics_node_table[(int) (mode)] & 0177)


/* Number of pointers to icode node in given node. Structure of
   nodes is common members (mode and etc.), pointers to icode nodes and
   after all miscellaneous members. */

#define ICN_POINTERS_IN_NODE(mode) \
  (characteristics_node_table[(int) (mode)]>>7 & 017)

/* Denotation has members scope and identifier. */
#define NODE_IS_DENOTATION(mode) \
  (characteristics_node_table[(int) (mode)]>>11 & 1)


/* Macros for forming element of characteristics node table. */

#define CHARACTERISTICS_NODE(size,nptr,denotation) \
  (size|nptr<<7|denotation<<11)

extern int characteristics_node_table[];



/* Nodes representing some object from standard Modula-2 environment.  Other
   standard environment object may be accessible by name through hash table. */


/* Nodes for basic types. */

extern basic_type_node ICN_TD_short_cardinal;
extern basic_type_node ICN_TD_cardinal;
extern basic_type_node ICN_TD_long_cardinal;
extern basic_type_node ICN_TD_short_integer;
extern basic_type_node ICN_TD_integer;
extern basic_type_node ICN_TD_long_integer;
extern basic_type_node ICN_TD_short_real;
extern basic_type_node ICN_TD_real;
extern basic_type_node ICN_TD_long_real;
extern basic_type_node ICN_TD_char;
extern basic_type_node ICN_TD_byte;
extern basic_type_node ICN_TD_word;
extern basic_type_node ICN_TD_nil;
extern basic_type_node ICN_TD_short_cardinal_or_integer;
extern basic_type_node ICN_TD_cardinal_or_integer;
extern basic_type_node ICN_TD_long_cardinal_or_integer;


/* Nodes representing other types from standard language environment. */

extern pointer_type_node ICN_TD_address;
extern set_type_node ICN_TD_bitset;
extern enumeration_type_node ICN_TD_boolean;


/* Nodes representing some constant values from
   standard language environment. */

extern cardinal_node ICN_V_false;
extern cardinal_node ICN_V_true;

extern cardinal_node ICN_V_min_short_cardinal;
extern cardinal_node ICN_V_min_cardinal;
extern cardinal_node ICN_V_min_long_cardinal;
extern cardinal_node ICN_V_max_short_cardinal;
extern cardinal_node ICN_V_max_cardinal;
extern cardinal_node ICN_V_max_long_cardinal;
extern cardinal_node ICN_V_min_char;
extern cardinal_node ICN_V_max_char;

extern integer_node ICN_V_min_short_integer;
extern integer_node ICN_V_min_integer;
extern integer_node ICN_V_min_long_integer;
extern integer_node ICN_V_max_short_integer;
extern integer_node ICN_V_max_integer;
extern integer_node ICN_V_max_long_integer;

extern real_node ICN_V_min_short_real;
extern real_node ICN_V_min_real;
extern real_node ICN_V_min_long_real;
extern real_node ICN_V_max_short_real;
extern real_node ICN_V_max_real;
extern real_node ICN_V_max_long_real;


/* Nodes representing standard procedures. */

extern procedure_node ICN_D_abs;
extern procedure_node ICN_D_cap;
extern procedure_node ICN_D_chr;
extern procedure_node ICN_D_dec;
extern procedure_node ICN_D_excl;
extern procedure_node ICN_D_float;
extern procedure_node ICN_D_halt;
extern procedure_node ICN_D_high;
extern procedure_node ICN_D_inc;
extern procedure_node ICN_D_incl;
extern procedure_node ICN_D_max;
extern procedure_node ICN_D_min;
extern procedure_node ICN_D_odd;
extern procedure_node ICN_D_ord;
extern procedure_node ICN_D_size;
extern procedure_node ICN_D_trunc;
extern procedure_node ICN_D_val;
extern procedure_node ICN_D_adr;
extern procedure_node ICN_D_iotransfer;
extern procedure_node ICN_D_transfer;
extern procedure_node ICN_D_newprocess;
extern procedure_node ICN_D_tsize;


/* Nodes representing modules from Modula-2 standard environment. */

extern module_node ICN_D_system;
extern module_node ICN_D_cproc;



/* Standard identifiers of Modula-2:  */

enum standard_identifier
{
  STDID_ABS,
  STDID_BITSET,
  STDID_BOOLEAN,
  STDID_CAP,
  STDID_CARDINAL,
  STDID_CHAR,
  STDID_CHR,
  STDID_DEC,
  STDID_EXCL,
  STDID_FALSE,
  STDID_FLOAT,
  STDID_HALT,
  STDID_HIGH,
  STDID_INC,
  STDID_INCL,
  STDID_INTEGER,
  STDID_LONGINT,
  STDID_LONGREAL,
  STDID_MAX,
  STDID_MIN,
  STDID_NIL,
  STDID_ODD,
  STDID_ORD,
  STDID_PROC,
  STDID_REAL,
  STDID_SIZE,
  STDID_TRUE,
  STDID_TRUNC,
  STDID_VAL,

  /* Additional standard identifiers which are absent in Wirth's book. */
  STDID_SHORTCARD,
  STDID_SHORTINT,
  STDID_SHORTREAL,
  STDID_LONGCARD
};

/* Characteristics of Modula-2 standard identifier in source program: */
struct stdid
{
  enum standard_identifier stdid;
  /* Flag of lower case standard identifier. */
  int lower_case_stdid;
  /* Flag of additional standard identifier. */
  int additional_stdid;
};



/* Functions for work with icode. */

extern struct stdid *find_stdid ();

extern unsigned int max_number_of_nodes;
extern unsigned int current_number_of_nodes;
extern unsigned int max_number_of_bytes_for_nodes;
extern unsigned int current_number_of_bytes_for_nodes;

extern void initiate_pool ();
extern void free_pool ();
extern void delete_node ();
extern ICN_pointer create_node ();
extern ICN_pointer create_node_with_string ();
extern ICN_pointer create_identifier_node ();
extern ICN_pointer first_block_begin ();

extern void initiate_table_and_environment ();
extern void delete_table ();
extern ICN_pointer find_in_or_else_include_to_table ();
extern ICN_pointer find_identifier ();
extern ICN_pointer find_denotation ();
extern ICN_pointer find_denotation_in_second_space ();
extern void include_to_table ();
extern ICN_pointer find_identifier_declaration_or_its_export_import ();
extern ICN_pointer find_identifier_declaration ();

extern int it_is_integer_type ();
extern int it_is_cardinal_type ();
extern int it_is_integer_cardinal_type ();
extern int it_is_real_type ();
extern int it_is_string_type ();
extern int string_length_from_its_type ();
extern int it_is_character_type ();
extern Tcard cardinal_value ();
extern ICN_pointer type_of_real_value ();
extern ICN_pointer type_of_cardinal_value ();
extern ICN_pointer type_of_integer_value ();
extern ICN_pointer integer_cardinal_type_of_cardinal_value ();
extern ICN_pointer through_integer_cardinal_type ();
extern ICN_pointer next_enumeration_constant ();
extern ICN_pointer procedure_over_denotation ();
extern ICN_pointer next_parameter_type ();
extern ICN_pointer through_range_type ();
extern ICN_pointer min_or_max ();
extern int types_are_identical ();
extern int types_are_assignment_compatible ();
extern ICN_pointer result_of_compatible_types ();
extern int subgraph_is_type ();
extern int subgraph_is_expression ();

extern void print_node ();
