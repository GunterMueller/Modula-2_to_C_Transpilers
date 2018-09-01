/* Definitions for syntax analyze pass of GNU Modula-2 translator.
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

/* Symbols of Modula-2 vocabulary. */

enum symbol
{
  SYMBOL_AND,			/* Modula-2 keyword `AND' and operator `&'. */
  SYMBOL_ARRAY,
  SYMBOL_BEGIN,
  SYMBOL_BY,
  SYMBOL_CASE,
  SYMBOL_CONST,
  SYMBOL_DEFINITION,
  SYMBOL_DIV,
  SYMBOL_DO,
  SYMBOL_ELSE,
  SYMBOL_ELSIF,
  SYMBOL_END,
  SYMBOL_EXIT,
  SYMBOL_EXPORT,
  SYMBOL_FOR,
  SYMBOL_FROM,
  SYMBOL_IF,
  SYMBOL_IMPLEMENTATION,
  SYMBOL_IMPORT,
  SYMBOL_IN,
  SYMBOL_LOOP,
  SYMBOL_MOD,
  SYMBOL_MODULE,
  SYMBOL_NOT,			/* Modula-2 keyword `NOT' and operator `~'. */
  SYMBOL_OF,
  SYMBOL_OR,
  SYMBOL_POINTER,
  SYMBOL_PROCEDURE,
  SYMBOL_QUALIFIED,
  SYMBOL_RECORD,
  SYMBOL_REPEAT,
  SYMBOL_RETURN,
  SYMBOL_SET,
  SYMBOL_THEN,
  SYMBOL_TO,
  SYMBOL_TYPE,
  SYMBOL_UNTIL,
  SYMBOL_VAR,
  SYMBOL_WHILE,
  SYMBOL_WITH,

  SYMBOL_IDENTIFIER,
  SYMBOL_STRING,
  SYMBOL_INTEGER,
  SYMBOL_REAL,
  SYMBOL_CHAR,
  SYMBOL_C_TEXT,
  SYMBOL_ADDITION,
  SYMBOL_SUBTRACTION,
  SYMBOL_EQUAL,
  SYMBOL_UNEQUAL,		/* Modula-2 operators `#' and `<>'. */
  SYMBOL_LESS_THAN,
  SYMBOL_LESS_THAN_OR_EQUAL,
  SYMBOL_GREATER_THAN,
  SYMBOL_GREATER_THAN_OR_EQUAL,
  SYMBOL_MULTIPLICATION,
  SYMBOL_DIVISION,
  SYMBOL_ASSIGNMENT,
  SYMBOL_COMMA,
  SYMBOL_PERIOD,
  SYMBOL_COLON,
  SYMBOL_SEMICOLON,
  SYMBOL_RANGE,
  SYMBOL_DEREFERENCING,
  SYMBOL_BAR,
  SYMBOL_LEFT_PARENTHESIS,
  SYMBOL_RIGHT_PARENTHESIS,
  SYMBOL_LEFT_BRACKET,
  SYMBOL_RIGHT_BRACKET,
  SYMBOL_LEFT_SET_BRACKET,
  SYMBOL_RIGHT_SET_BRACKET,
  SYMBOL_END_OF_FILE,
  SYMBOL_END_MARK		/* This symbol is to be last enumeration constant. */
};

extern ICN_pointer icode_symbol_presentation;
extern enum symbol current_symbol;

extern void initiate_scanner_data ();
extern void start_scanner ();
extern enum symbol next_symbol ();
extern enum symbol match_symbol ();
extern char *new_symbol_set_string ();
extern int number_of_current_symbol_in_table ();
extern int symbol_is_in_set_string ();
extern int test_current_symbol ();

extern int it_is_stop_symbol[];


/* SYMBOL will be stop symbol.  Stop symbols are skipped in error recovery
   regime only by matching with expected symbol. */

#define ADD_STOP_SYMBOL(symbol)    it_is_stop_symbol[(int) symbol]++


/* SYMBOL will be nonstop symbol if number of removing is equal to
   number of adding the symbol. */

#define REMOVE_STOP_SYMBOL(symbol) (--it_is_stop_symbol[(int) symbol]<0\
				    ? (abort(), 0) : 0)
