/* Scanner of GNU Modula-2 translator.
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


/* Structure and function for recognizing keyword in source program. */


/* Characteristics of Modula-2 keyword in source program. */

struct keyword_characteristics
{
  enum symbol symbol;
  /* All letters of keyword may be only in lower case or in upper case. */
  int lower_case_keyword;
};


/* If string STR (with end marker '\0') with length LENGTH is a keyword
   then return its characteristics else return 0.

   Fast search is implemented by minimal pruned O-trie forest. */

#ifdef __GNUC__
__inline__
#endif
static struct keyword_characteristics *
find_keyword (str, length)
     register char *str;
     register int length;
{
  switch (length)
    {
    case 2:
      switch (*str)
	{
	case 'B':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_BY, FALSE};
	    return (str[1] == 'Y' ? &kw : 0);
	  }
	case 'b':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_BY, TRUE};
	    return (str[1] == 'y' ? &kw : 0);
	  }
	case 'D':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_DO, FALSE};
	    return (str[1] == 'O' ? &kw : 0);
	  }
	case 'd':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_DO, TRUE};
	    return (str[1] == 'o' ? &kw : 0);
	  }
	case 'I':
	  if (str[1] == 'F')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_IF, FALSE};
	      return (&kw);
	    }
	  else if (str[1] == 'N')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_IN, FALSE};
	      return (&kw);
	    }
	  else
	    return 0;
	case 'i':
	  if (str[1] == 'f')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_IF, TRUE};
	      return (&kw);
	    }
	  else if (str[1] == 'n')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_IN, TRUE};
	      return (&kw);
	    }
	  else
	    return 0;
	case 'O':
	  if (str[1] == 'F')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_OF, FALSE};
	      return (&kw);
	    }
	  else if (str[1] == 'R')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_OR, FALSE};
	      return (&kw);
	    }
	  else
	    return 0;
	case 'o':
	  if (str[1] == 'f')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_OF, TRUE};
	      return (&kw);
	    }
	  else if (str[1] == 'r')
	    {
	      static struct keyword_characteristics kw =
	      {SYMBOL_OR, TRUE};
	      return (&kw);
	    }
	  else
	    return 0;
	case 'T':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_TO, FALSE};
	    return (str[1] == 'O' ? &kw : 0);
	  }
	case 't':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_TO, TRUE};
	    return (str[1] == 'o' ? &kw : 0);
	  }
	default:
	  return 0;
	}
    case 3:
      switch (*str)
	{
	case 'A':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_AND, FALSE};
	    return (strcmp (str, "AND") == 0 ? &kw : 0);
	  }
	case 'a':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_AND, TRUE};
	    return (strcmp (str, "and") == 0 ? &kw : 0);
	  }
	case 'D':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_DIV, FALSE};
	    return (strcmp (str, "DIV") == 0 ? &kw : 0);
	  }
	case 'd':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_DIV, TRUE};
	    return (strcmp (str, "div") == 0 ? &kw : 0);
	  }
	case 'E':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_END, FALSE};
	    return (strcmp (str, "END") == 0 ? &kw : 0);
	  }
	case 'e':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_END, TRUE};
	    return (strcmp (str, "end") == 0 ? &kw : 0);
	  }
	case 'F':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_FOR, FALSE};
	    return (strcmp (str, "FOR") == 0 ? &kw : 0);
	  }
	case 'f':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_FOR, TRUE};
	    return (strcmp (str, "for") == 0 ? &kw : 0);
	  }
	case 'M':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_MOD, FALSE};
	    return (strcmp (str, "MOD") == 0 ? &kw : 0);
	  }
	case 'm':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_MOD, TRUE};
	    return (strcmp (str, "mod") == 0 ? &kw : 0);
	  }
	case 'N':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_NOT, FALSE};
	    return (strcmp (str, "NOT") == 0 ? &kw : 0);
	  }
	case 'n':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_NOT, TRUE};
	    return (strcmp (str, "not") == 0 ? &kw : 0);
	  }
	case 'S':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_SET, FALSE};
	    return (strcmp (str, "SET") == 0 ? &kw : 0);
	  }
	case 's':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_SET, TRUE};
	    return (strcmp (str, "set") == 0 ? &kw : 0);
	  }
	case 'V':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_VAR, FALSE};
	    return (strcmp (str, "VAR") == 0 ? &kw : 0);
	  }
	case 'v':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_VAR, TRUE};
	    return (strcmp (str, "var") == 0 ? &kw : 0);
	  }
	default:
	  return 0;
	}
    case 4:
      switch (str[1])
	{
	case 'A':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_CASE, FALSE};
	    return (strcmp (str, "CASE") == 0 ? &kw : 0);
	  }
	case 'a':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_CASE, TRUE};
	    return (strcmp (str, "case") == 0 ? &kw : 0);
	  }
	case 'H':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_THEN, FALSE};
	    return (strcmp (str, "THEN") == 0 ? &kw : 0);
	  }
	case 'h':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_THEN, TRUE};
	    return (strcmp (str, "then") == 0 ? &kw : 0);
	  }
	case 'I':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_WITH, FALSE};
	    return (strcmp (str, "WITH") == 0 ? &kw : 0);
	  }
	case 'i':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_WITH, TRUE};
	    return (strcmp (str, "with") == 0 ? &kw : 0);
	  }
	case 'L':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_ELSE, FALSE};
	    return (strcmp (str, "ELSE") == 0 ? &kw : 0);
	  }
	case 'l':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_ELSE, TRUE};
	    return (strcmp (str, "else") == 0 ? &kw : 0);
	  }
	case 'O':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_LOOP, FALSE};
	    return (strcmp (str, "LOOP") == 0 ? &kw : 0);
	  }
	case 'o':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_LOOP, TRUE};
	    return (strcmp (str, "loop") == 0 ? &kw : 0);
	  }
	case 'R':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_FROM, FALSE};
	    return (strcmp (str, "FROM") == 0 ? &kw : 0);
	  }
	case 'r':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_FROM, TRUE};
	    return (strcmp (str, "from") == 0 ? &kw : 0);
	  }
	case 'X':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_EXIT, FALSE};
	    return (strcmp (str, "EXIT") == 0 ? &kw : 0);
	  }
	case 'x':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_EXIT, TRUE};
	    return (strcmp (str, "exit") == 0 ? &kw : 0);
	  }
	case 'Y':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_TYPE, FALSE};
	    return (strcmp (str, "TYPE") == 0 ? &kw : 0);
	  }
	case 'y':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_TYPE, TRUE};
	    return (strcmp (str, "type") == 0 ? &kw : 0);
	  }
	default:
	  return 0;
	}
    case 5:
      switch (*str)
	{
	case 'A':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_ARRAY, FALSE};
	    return (strcmp (str, "ARRAY") == 0 ? &kw : 0);
	  }
	case 'a':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_ARRAY, TRUE};
	    return (strcmp (str, "array") == 0 ? &kw : 0);
	  }
	case 'B':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_BEGIN, FALSE};
	    return (strcmp (str, "BEGIN") == 0 ? &kw : 0);
	  }
	case 'b':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_BEGIN, TRUE};
	    return (strcmp (str, "begin") == 0 ? &kw : 0);
	  }
	case 'C':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_CONST, FALSE};
	    return (strcmp (str, "CONST") == 0 ? &kw : 0);
	  }
	case 'c':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_CONST, TRUE};
	    return (strcmp (str, "const") == 0 ? &kw : 0);
	  }
	case 'E':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_ELSIF, FALSE};
	    return (strcmp (str, "ELSIF") == 0 ? &kw : 0);
	  }
	case 'e':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_ELSIF, TRUE};
	    return (strcmp (str, "elsif") == 0 ? &kw : 0);
	  }
	case 'U':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_UNTIL, FALSE};
	    return (strcmp (str, "UNTIL") == 0 ? &kw : 0);
	  }
	case 'u':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_UNTIL, TRUE};
	    return (strcmp (str, "until") == 0 ? &kw : 0);
	  }
	case 'W':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_WHILE, FALSE};
	    return (strcmp (str, "WHILE") == 0 ? &kw : 0);
	  }
	case 'w':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_WHILE, TRUE};
	    return (strcmp (str, "while") == 0 ? &kw : 0);
	  }
	default:
	  return 0;
	}
    case 6:
      switch (str[5])
	{
	case 'E':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_MODULE, FALSE};
	    return (strcmp (str, "MODULE") == 0 ? &kw : 0);
	  }
	case 'e':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_MODULE, TRUE};
	    return (strcmp (str, "module") == 0 ? &kw : 0);
	  }
	case 'D':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_RECORD, FALSE};
	    return (strcmp (str, "RECORD") == 0 ? &kw : 0);
	  }
	case 'd':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_RECORD, TRUE};
	    return (strcmp (str, "record") == 0 ? &kw : 0);
	  }
	case 'N':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_RETURN, FALSE};
	    return (strcmp (str, "RETURN") == 0 ? &kw : 0);
	  }
	case 'n':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_RETURN, TRUE};
	    return (strcmp (str, "return") == 0 ? &kw : 0);
	  }
	case 'T':
	  switch (*str)
	    {
	    case 'E':
	      {
		static struct keyword_characteristics kw =
		{SYMBOL_EXPORT, FALSE};
		return (strcmp (str, "EXPORT") == 0 ? &kw : 0);
	      }
	    case 'I':
	      {
		static struct keyword_characteristics kw =
		{SYMBOL_IMPORT, FALSE};
		return (strcmp (str, "IMPORT") == 0 ? &kw : 0);
	      }
	    case 'R':
	      {
		static struct keyword_characteristics kw =
		{SYMBOL_REPEAT, FALSE};
		return (strcmp (str, "REPEAT") == 0 ? &kw : 0);
	      }
	    default:
	      return 0;
	    }
	case 't':
	  switch (*str)
	    {
	    case 'e':
	      {
		static struct keyword_characteristics kw =
		{SYMBOL_EXPORT, TRUE};
		return (strcmp (str, "export") == 0 ? &kw : 0);
	      }
	    case 'i':
	      {
		static struct keyword_characteristics kw =
		{SYMBOL_IMPORT, TRUE};
		return (strcmp (str, "import") == 0 ? &kw : 0);
	      }
	    case 'r':
	      {
		static struct keyword_characteristics kw =
		{SYMBOL_REPEAT, TRUE};
		return (strcmp (str, "repeat") == 0 ? &kw : 0);
	      }
	    default:
	      return 0;
	    }
	default:
	  return 0;
	}
    case 7:
      if (*str == 'P')
	{
	  static struct keyword_characteristics kw =
	  {SYMBOL_POINTER, FALSE};
	  return (strcmp (str, "POINTER") == 0 ? &kw : 0);
	}
      else
	{
	  static struct keyword_characteristics kw =
	  {SYMBOL_POINTER, TRUE};
	  return (strcmp (str, "pointer") == 0 ? &kw : 0);
	}
    case 9:
      switch (*str)
	{
	case 'P':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_PROCEDURE, FALSE};
	    return (strcmp (str, "PROCEDURE") == 0 ? &kw : 0);
	  }
	case 'p':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_PROCEDURE, TRUE};
	    return (strcmp (str, "procedure") == 0 ? &kw : 0);
	  }
	case 'Q':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_QUALIFIED, FALSE};
	    return (strcmp (str, "QUALIFIED") == 0 ? &kw : 0);
	  }
	case 'q':
	  {
	    static struct keyword_characteristics kw =
	    {SYMBOL_QUALIFIED, TRUE};
	    return (strcmp (str, "qualified") == 0 ? &kw : 0);
	  }
	default:
	  return 0;
	}
    case 10:
      if (*str == 'D')
	{
	  static struct keyword_characteristics kw =
	  {SYMBOL_DEFINITION, FALSE};
	  return (strcmp (str, "DEFINITION") == 0 ? &kw : 0);
	}
      else
	{
	  static struct keyword_characteristics kw =
	  {SYMBOL_DEFINITION, TRUE};
	  return (strcmp (str, "definition") == 0 ? &kw : 0);
	}
    case 14:
      if (*str == 'I')
	{
	  static struct keyword_characteristics kw =
	  {SYMBOL_IMPLEMENTATION, FALSE};
	  return (strcmp (str, "IMPLEMENTATION") == 0 ? &kw : 0);
	}
      else
	{
	  static struct keyword_characteristics kw =
	  {SYMBOL_IMPLEMENTATION, TRUE};
	  return (strcmp (str, "implementation") == 0 ? &kw : 0);
	}
    default:
      return 0;
    }
}



/* Function and corresponding structures for recognizing symbols from
   input file (sourcefile). */


/* Defined only for the following symbols: SYMBOL_IDENTIFIER, SYMBOL_STRING,
   SYMBOL_CHAR, SYMBOL_INTEGER, SYMBOL_REAL. */

ICN_pointer icode_symbol_presentation;


/* Modula-2 real number may have not fractional part and scale factor,
   e.g. `123.'.  But the character sequence `123..' is treated as integer
   `123' and range `..'.  In this case, when number `123' is returned
   by the scanner the input stream does not contain the periods, but
   FLAG_OF_RANGE is TRUE. */

static int flag_of_range;


/* Variable length string used by function next_file_symbol for text
   presentation of the symbol. */

static VLS symbol_text;


/* The following function recognizes next Modula-2 symbol from the input file,
   returns its code, modifies variable current_line_number so that its value
   is equal to line number of the current character in the input file and sets
   up variable source_line_number so that its value is equal to line number of
   the returned symbol start, creates corresponding icode node (if it is
   needed) and sets up icode_symbol_presentation to the node.  The function
   skips all white spaces and commentaries and fixes all lexical errors. */

static enum symbol
next_file_symbol ()
{
  register int input_char;
  register int number_of_successive_error_characters;

  VLS_NULLIFY (symbol_text);
  if (flag_of_range)
    {
      flag_of_range = FALSE;
      return SYMBOL_RANGE;
    }
  for (number_of_successive_error_characters = 0;;)
    {
      source_line_number = current_line_number;
      input_char = getc (current_input_file);
      switch (input_char)
	{
	  /* Break results in skipping all white spaces. */
	case ' ':
	case '\t':
	case '\f':
	  break;
	case '\n':
	  current_line_number++;
	  break;
	case '&':
	  return SYMBOL_AND;
	case '~':
	  return SYMBOL_NOT;
	case '+':
	  return SYMBOL_ADDITION;
	case '-':
	  return SYMBOL_SUBTRACTION;
	case '=':
	  return SYMBOL_EQUAL;
	case '#':
	  return SYMBOL_UNEQUAL;
	case '<':
	  input_char = getc (current_input_file);
	  if (input_char == '>')
	    return SYMBOL_UNEQUAL;
	  else if (input_char == '=')
	    return SYMBOL_LESS_THAN_OR_EQUAL;
	  else
	    {
	      ungetc (input_char, current_input_file);
	      return SYMBOL_LESS_THAN;
	    }
	case '>':
	  input_char = getc (current_input_file);
	  if (input_char == '=')
	    return SYMBOL_GREATER_THAN_OR_EQUAL;
	  else
	    {
	      ungetc (input_char, current_input_file);
	      return SYMBOL_GREATER_THAN;
	    }
	case '*':
	  return SYMBOL_MULTIPLICATION;
	case '/':
	  return SYMBOL_DIVISION;
	case ':':
	  input_char = getc (current_input_file);
	  if (input_char == '=')
	    return SYMBOL_ASSIGNMENT;
	  else
	    {
	      ungetc (input_char, current_input_file);
	      return SYMBOL_COLON;
	    }
	case ',':
	  return SYMBOL_COMMA;
	case '.':
	  input_char = getc (current_input_file);
	  if (input_char == '.')
	    return SYMBOL_RANGE;
	  else
	    {
	      ungetc (input_char, current_input_file);
	      return SYMBOL_PERIOD;
	    }
	case ';':
	  return SYMBOL_SEMICOLON;
	case '^':
	  return SYMBOL_DEREFERENCING;
	case '|':
	  return SYMBOL_BAR;
	case '(':
	  input_char = getc (current_input_file);
	  if (input_char != '*')
	    {
	      ungetc (input_char, current_input_file);
	      return SYMBOL_LEFT_PARENTHESIS;
	    }
	  else
	    {
	      /* Commentary recognition.  Remember that Modula-2 commentaries
		 may be nested. */
	      int commentary_bracket_level = 1;
	      do
		{
		  do
		    {
		      if (input_char == '\n')
			{
			  current_line_number++;
			}
		      input_char = getc (current_input_file);
		    }
		  while (input_char != '*' && input_char != '('
			 && input_char != EOF);
		  if (input_char == '(')
		    {
		      input_char = getc (current_input_file);
		      if (input_char == '*')
			commentary_bracket_level++;
		    }
		  else if (input_char == '*')
		    {
		      input_char = getc (current_input_file);
		      if (input_char == ')')
			commentary_bracket_level--;
		      else if (input_char == '*')
			ungetc (input_char, current_input_file);
		    }
		  else
		    {
		      error (ERR_comment_end_absence);
		      ungetc (input_char, current_input_file);
		      break;
		    }
	      } while (commentary_bracket_level != 0);
	      break;		/* Break results in skipping all white spaces. */
	    }
	case ')':
	  return SYMBOL_RIGHT_PARENTHESIS;
	case '[':
	  return SYMBOL_LEFT_BRACKET;
	case ']':
	  return SYMBOL_RIGHT_BRACKET;
	case '{':
	  return SYMBOL_LEFT_SET_BRACKET;
	case '}':
	  return SYMBOL_RIGHT_SET_BRACKET;
	case EOF:
	  return SYMBOL_END_OF_FILE;
	case '\"':
	case '\'':
	  /* String recognition. */
	  {
	    int expected_string_end_mark;
	    register ICN_pointer string_node_ptr;
	    char *string_value_in_icode_memory;
	    for (expected_string_end_mark = input_char;;)
	      {
		input_char = getc (current_input_file);
		if (input_char == expected_string_end_mark)
		  break;
		if (input_char == EOF || input_char == '\n')
		  {
		    ungetc (input_char, current_input_file);
		    error (ERR_string_end_absence);
		    break;
		  }
		VLS_ADD_CHAR (symbol_text, input_char);
	      }
	    VLS_ADD_CHAR (symbol_text, '\0');
	    string_node_ptr
	      = create_node_with_string (ICNM_STRING, VLS_BEGIN (symbol_text),
					 &string_value_in_icode_memory);
	    STRING_NODE (string_node_ptr)->string_value
	      = string_value_in_icode_memory;
	    STRING_NODE (string_node_ptr)->value_type = create_node (ICNM_BASIC_TYPE);
	    BASIC_TYPE_NODE (STRING_NODE (string_node_ptr)->value_type)->basic_type
	      = (enum basic_type) ((unsigned) BTM_EMPTY_STRING
				   + VLS_LENGTH (symbol_text)
				   - (third_edition_flag ? 1 : 0));
	    STRING_NODE (string_node_ptr)->value_type
	      = find_in_or_else_include_to_table
		(STRING_NODE (string_node_ptr)->value_type);
	    icode_symbol_presentation
	      = find_in_or_else_include_to_table (string_node_ptr);
	    return SYMBOL_STRING;
	  }
	  break;
	case '`':
	  /* C string recognition. */
	  if (strict_flag)
	    {
	      number_of_successive_error_characters++;
	      if (number_of_successive_error_characters == 1)
		error (ERR_invalid_input_char);
	    }
	  else
	    {
	      int ignore_current_char, character_code;
	      register ICN_pointer string_node_ptr;
	      char *string_value_in_icode_memory;
	      for (;;)
		{
		  input_char = getc (current_input_file);
		  if (input_char == '\'')
		    break;
		  if (input_char == EOF || input_char == '\n')
		    {
		      ungetc (input_char, current_input_file);
		      error (ERR_string_end_absence);
		      break;
		    }
		  ignore_current_char = FALSE;
		  if (input_char == '\\')
		    {
		      input_char = getc (current_input_file);
		      if (input_char == 'n')
			input_char = '\n';
		      else if (input_char == 't')
			input_char = '\t';
		      else if (input_char == 'v')
			input_char = '\v';
		      else if (input_char == 'b')
			input_char = '\b';
		      else if (input_char == 'r')
			input_char = '\r';
		      else if (input_char == 'f')
			input_char = '\f';
		      else if (input_char == '\\' || input_char == '\''
			       || input_char == '\"') ;
		      else if (input_char == '\n')
			{
			  current_line_number++;
			  ignore_current_char = TRUE;
			}
		      else if (isdigit (input_char) && input_char != '8'
			       && input_char != '9')
			{
			  character_code = value_of_hexadecimal_digit (input_char);
			  input_char = getc (current_input_file);
			  if (!isdigit (input_char) || input_char == '8'
			      || input_char == '9')
			    ungetc (input_char, current_input_file);
			  else
			    {
			      character_code
				= (character_code * 8
				   + value_of_hexadecimal_digit (input_char));
			      input_char = getc (current_input_file);
			      if (!isdigit (input_char) || input_char == '8'
				  || input_char == '9')
				ungetc (input_char, current_input_file);
			      else
				character_code
				  = (character_code * 8
				     + value_of_hexadecimal_digit (input_char));
			    }
			  input_char = character_code;
			}
		    }
		  else
		    ignore_current_char = TRUE;
		  if (!ignore_current_char)
		    VLS_ADD_CHAR (symbol_text, input_char);
		}
	      VLS_ADD_CHAR (symbol_text, '\0');
	      string_node_ptr
		= create_node_with_string
		  (ICNM_STRING, VLS_BEGIN (symbol_text),
		   &string_value_in_icode_memory);
	      STRING_NODE (string_node_ptr)->string_value
		= string_value_in_icode_memory;
	      STRING_NODE (string_node_ptr)->value_type
		= create_node (ICNM_BASIC_TYPE);
	      BASIC_TYPE_NODE (STRING_NODE (string_node_ptr)->value_type)
		->basic_type
		  = (enum basic_type) ((unsigned) BTM_EMPTY_STRING
				       + VLS_LENGTH (symbol_text) - 1);
	      STRING_NODE (string_node_ptr)->value_type
		= find_in_or_else_include_to_table
		  (STRING_NODE (string_node_ptr)->value_type);
	      icode_symbol_presentation
		= find_in_or_else_include_to_table (string_node_ptr);
	      return SYMBOL_STRING;
	    }
	  break;
	case '%':
	  input_char = getc (current_input_file);
	  if (strict_flag || input_char != '{')
	    {
	      ungetc (input_char, current_input_file);
	      number_of_successive_error_characters++;
	      if (number_of_successive_error_characters == 1)
		error (ERR_invalid_input_char);
	    }
	  else
	    /* C text recognition. */
	    {
	      register ICN_pointer C_code_node_ptr;
	      char *s;
	      for (;;)
		{
		  for (;;)
		    {
		      input_char = getc (current_input_file);
		      if (input_char == '%' || input_char == EOF)
			break;
		      VLS_ADD_CHAR (symbol_text, input_char);
		      if (input_char == '\n')
			current_line_number++;
		    }
		  if (input_char == EOF)
		    {
		      error (ERR_C_code_end_absence);
		      ungetc (input_char, current_input_file);
		      break;
		    }
		  else
		    {
		      input_char = getc (current_input_file);
		      if (input_char == '}')
			break;
		      else
			{
			  ungetc (input_char, current_input_file);
			  VLS_ADD_CHAR (symbol_text, '%');
			}
		    }
		}
	      VLS_ADD_CHAR (symbol_text, '\0');
	      /* C code node in declarations may be changed on C code node
		 in statements afterwards. */
	      C_code_node_ptr
		= create_node_with_string
		  (ICNM_C_CODE_IN_DECLARATIONS, VLS_BEGIN (symbol_text), &s);
	      C_CODE_IN_DECLARATIONS_NODE (C_code_node_ptr)->C_text = s;
	      icode_symbol_presentation = C_code_node_ptr;
	      return SYMBOL_C_TEXT;
	    }
	  break;
	default:
	  if (isalpha (input_char) || (input_char == '_' && !strict_flag))
	    {
	      /* Identifier recognition. */
	      register struct keyword_characteristics *keyword_characteristics;
	      do
		{
		  VLS_ADD_CHAR (symbol_text, input_char);
		  input_char = getc (current_input_file);
		}
	      while (isalpha (input_char) || isdigit (input_char)
		     || (input_char == '_' && !strict_flag));
	      ungetc (input_char, current_input_file);
	      VLS_ADD_CHAR (symbol_text, '\0');
	      keyword_characteristics = find_keyword (VLS_BEGIN (symbol_text),
						      VLS_LENGTH (symbol_text) - 1);
	      if (keyword_characteristics != 0
		  && (!keyword_characteristics->lower_case_keyword || !only_upper_case_flag))
		return keyword_characteristics->symbol;
	      else
		{
		  icode_symbol_presentation
		    = create_identifier_node (VLS_BEGIN (symbol_text));
		  return SYMBOL_IDENTIFIER;
		}
	    }
	  else if (isdigit (input_char))
	    {
	      /* Recognition numbers. */
	      register ICN_pointer number_value_node;
	      int i, scale_factor, error_flag, scale_factor_is_negative;
	      enum symbol recognized_symbol;
	      char *current_symbol_char;
	      Tcard integer_number_value, integer_number_base;
	      Treal real_number_value;
	      Treal factor_for_real_number = 10.0;
	      do
		{
		  VLS_ADD_CHAR (symbol_text, input_char);
		  input_char = getc (current_input_file);
		}
	      while (isxdigit (input_char)
		     && (!only_upper_case_flag || isdigit (input_char)
			 || isupper (input_char)));
	      recognized_symbol = SYMBOL_INTEGER;
	      integer_number_value = 0;
	      real_number_value = 0.0;
	      error_flag = FALSE;
	      if (input_char == 'H' || (!only_upper_case_flag && input_char == 'h'))
		integer_number_base = 16;
	      else if (*VLS_END (symbol_text) == 'B'
		       || *VLS_END (symbol_text) == 'C'
		       || *VLS_END (symbol_text) == 'b'
		       || *VLS_END (symbol_text) == 'c')
		{
		  if (*VLS_END (symbol_text) == 'C'
		      || *VLS_END (symbol_text) == 'c')
		    recognized_symbol = SYMBOL_CHAR;
		  VLS_SHORTEN (symbol_text, 1);
		  integer_number_base = 8;
		  ungetc (input_char, current_input_file);
		  for (current_symbol_char = VLS_BEGIN (symbol_text);
		       current_symbol_char <= VLS_END (symbol_text); current_symbol_char++)
		    if (!isdigit (CHAR_TO_INT (*current_symbol_char))
			|| input_char == '8' || input_char == '9')
		      {
			error_flag = TRUE;
			error (ERR_octal_number);
			break;
		      }
		}
	      else
		{
		  integer_number_base = 10;
		  ungetc (input_char, current_input_file);
		  for (current_symbol_char = VLS_BEGIN (symbol_text);
		       current_symbol_char <= VLS_END (symbol_text); current_symbol_char++)
		    if (!isdigit (CHAR_TO_INT (*current_symbol_char)))
		      {
			error_flag = TRUE;
			error (ERR_decimal_number);
			break;
		      }
		}
	      if (!error_flag)
		{
		  if (integer_number_base == 10)
		    {
		      input_char = getc (current_input_file);
		      if (input_char == '.')
			{
			  input_char = getc (current_input_file);
			  if (input_char == '.')
			    flag_of_range = TRUE;
			  else
			    recognized_symbol = SYMBOL_REAL;
			}
		      if (!flag_of_range)
			ungetc (input_char, current_input_file);
		    }
		  if (recognized_symbol != SYMBOL_REAL)
		    {
		      for (current_symbol_char = VLS_BEGIN (symbol_text);
			   current_symbol_char <= VLS_END (symbol_text); current_symbol_char++)
			integer_number_value
			  = (integer_number_value * integer_number_base
			     + value_of_hexadecimal_digit (*current_symbol_char));
		      if (recognized_symbol == SYMBOL_CHAR
			  && integer_number_value > CHARACTER_MAX)
			{
			  error (ERR_big_char_code);
			  error_flag = TRUE;
			  integer_number_value = 0;
			}
		    }
		  else
		    {
		      scale_factor = 0;
		      for (current_symbol_char = VLS_BEGIN (symbol_text);
			   current_symbol_char <= VLS_END (symbol_text); current_symbol_char++)
			if (real_number_value >= LONGREAL_MAX / factor_for_real_number)
			  scale_factor++;
			else
			  real_number_value
			    = (real_number_value * factor_for_real_number
			       + (value_of_hexadecimal_digit
				  (*current_symbol_char)));
		      for (;;)
			{
			  input_char = getc (current_input_file);
			  if (!isdigit (input_char))
			    break;
			  else if (real_number_value < LONGREAL_MAX / factor_for_real_number)
			    {
			      scale_factor--;
			      real_number_value
				= (real_number_value * factor_for_real_number
				   + value_of_hexadecimal_digit (input_char));
			    }
			}
		      if (input_char == 'E' || (!only_upper_case_flag && input_char == 'e'))
			{
			  input_char = getc (current_input_file);
			  if (input_char == '+' || input_char == '-')
			    {
			      scale_factor_is_negative = input_char == '-';
			      input_char = getc (current_input_file);
			    }
			  else
			    scale_factor_is_negative = FALSE;
			  if (!isdigit (input_char))
			    {
			      error_flag = TRUE;
			      error (ERR_order_absence);
			    }
			  else
			    {
			      for (i = 0; isdigit (input_char);
				   input_char = getc (current_input_file))
				i = (i * 10
				     + (value_of_hexadecimal_digit
					(input_char)));
			      if (scale_factor_is_negative)
				scale_factor -= i;
			      else
				scale_factor += i;
			    }
			}
		      ungetc (input_char, current_input_file);
		      if (!error_flag)
			{
			  if (scale_factor >= 0)
			    scale_factor_is_negative = FALSE;
			  else
			    {
			      scale_factor = (-scale_factor);
			      scale_factor_is_negative = TRUE;
			    }
			  for (i = 1; i <= scale_factor; i++)
			    real_number_value
			      = (scale_factor_is_negative
				 ? real_number_value / factor_for_real_number
				 : real_number_value * factor_for_real_number);
			}
		    }
		}
	      if (recognized_symbol == SYMBOL_REAL)
		{
		  number_value_node = create_node (ICNM_REAL);
		  REAL_NODE (number_value_node)->real_value = real_number_value;
		  REAL_NODE (number_value_node)->value_type
		    = type_of_real_value (real_number_value);
		}
	      else
		{
		  number_value_node = create_node (ICNM_CARDINAL);
		  CARDINAL_NODE (number_value_node)->cardinal_value = integer_number_value;
		  CARDINAL_NODE (number_value_node)->value_type
		    = (recognized_symbol == SYMBOL_CHAR
		       ? ICN_POINTER (&ICN_TD_char)
		       : (integer_cardinal_type_of_cardinal_value
			  (integer_number_value)));
		  if (CARDINAL_NODE (number_value_node)->value_type == NULL)
		    CARDINAL_NODE (number_value_node)->value_type
		      = type_of_cardinal_value (integer_number_value);
		}
	      icode_symbol_presentation
		= find_in_or_else_include_to_table (number_value_node);
	      return recognized_symbol;
	    }
	  else
	    {
	      number_of_successive_error_characters++;
	      if (number_of_successive_error_characters == 1)
		error (ERR_invalid_input_char);
	    }
	}
    }
}



/* Functions used by syntax analyzer.  Functions implement input symbol
   stream.  Current symbol and symbols in buffer are in the beginning of
   the stream.  This buffer is needed for syntactic error recovery and
   it is empty in normal state. */


/* Length of the buffer used for looking up in syntactic error recover. */

#define SYMBOL_BUFFER_LENGTH 10


/* The following structure contains all symbol characteristics needed
   for syntax analyzer. */

struct symbol_buffer_entry
{
  enum symbol symbol;
  int number_of_symbol_line;	/* Source line on which the symbol starts. */
  ICN_pointer icode_symbol_presentation;
};


/* Buffer used for looking up in syntactic error recover.  The buffer is
   cyclic. */

static struct symbol_buffer_entry symbol_buffer[SYMBOL_BUFFER_LENGTH];


/* Number of symbols which are currently in the buffer. */

static int number_of_buffer_elements;


/* Number of the buffer entry in which the first stream symbol (after
   current symbol) is stored.  Remember that the buffer is cyclic. */

static int first_buffer_element_index;


/* The first symbol of the input symbol stream. */

enum symbol current_symbol;


/* The following functions returns next symbol from the input stream.
   After its call the variables current_symbol, source_line_number and
   icode_symbol_presentation are set up correspondingly. */

enum symbol
next_symbol ()
{
  register struct symbol_buffer_entry *buffer_entry_ptr;

  if (number_of_buffer_elements != 0)
    {
      buffer_entry_ptr = (&symbol_buffer[first_buffer_element_index]);
      current_symbol = buffer_entry_ptr->symbol;
      source_line_number = buffer_entry_ptr->number_of_symbol_line;
      icode_symbol_presentation = buffer_entry_ptr->icode_symbol_presentation;
      first_buffer_element_index
	= (first_buffer_element_index + 1) % SYMBOL_BUFFER_LENGTH;
      number_of_buffer_elements--;
    }
  else
    current_symbol = next_file_symbol ();
  return current_symbol;
}


/* The following function reads symbol from the file and fill the whole buffer.
   It is needed for syntactic error recovery. */

static void
fill_symbol_buffer ()
{
  register struct symbol_buffer_entry *buffer_entry_ptr;
  register int saved_source_line_number;
  enum symbol saved_current_symbol;
  ICN_pointer saved_icode_symbol_presentation;

  saved_current_symbol = current_symbol;
  saved_source_line_number = source_line_number;
  saved_icode_symbol_presentation = icode_symbol_presentation;
  while (number_of_buffer_elements < SYMBOL_BUFFER_LENGTH
	 && current_symbol != SYMBOL_END_OF_FILE)
    {
      buffer_entry_ptr
	= (&symbol_buffer[(first_buffer_element_index
			   + number_of_buffer_elements)
			  % SYMBOL_BUFFER_LENGTH]);
      number_of_buffer_elements++;
      buffer_entry_ptr->symbol = next_file_symbol ();
      buffer_entry_ptr->number_of_symbol_line = source_line_number;
      buffer_entry_ptr->icode_symbol_presentation = icode_symbol_presentation;
    }
  current_symbol = saved_current_symbol;
  source_line_number = saved_source_line_number;
  icode_symbol_presentation = saved_icode_symbol_presentation;
}


/* The function skips all nonstop stream symbol up to the first
   SEARCHED_SYMBOL (this symbol will be current symbol).  Symbol is stop
   if the corresponding element of IT_IS_STOP_SYMBOL is not zero.
   Search are executed only up to the last buffer symbol.  If the symbol
   is not found then the function does nothing and returns FALSE.
   Otherwise the function returns TRUE. */

static int
empty_stream_up_to_symbol (searched_symbol, it_is_stop_symbol)
     register enum symbol searched_symbol;
     register int it_is_stop_symbol[];
{
  register int i, current_index_in_buffer;

  if (current_symbol == searched_symbol)
    return TRUE;
  for (i = 0; i < number_of_buffer_elements; i++)
    {
      current_index_in_buffer
	= (i + first_buffer_element_index) % SYMBOL_BUFFER_LENGTH;
      if (!it_is_stop_symbol[(int) symbol_buffer[current_index_in_buffer]
			     .symbol]
	  && (symbol_buffer[current_index_in_buffer].symbol
	      == searched_symbol))
	{
	  first_buffer_element_index = current_index_in_buffer;
	  number_of_buffer_elements -= i;
	  next_symbol ();
	  return TRUE;
	}
    }
  return FALSE;
}


/* Return number of the symbol table entry in which the first symbol matched
   with current symbol is placed. If the current symbol is not found return
   -1. */

int
number_of_current_symbol_in_table (symbol_table)
     register enum symbol *symbol_table;
{
  register int i;

  for (i = 0; *symbol_table != SYMBOL_END_MARK; i++, symbol_table++)
    if (*symbol_table == current_symbol)
      return i;
  return -1;
}


/* Allocate new symbol set string and return it.  The set string is created for
   symbols in SYMBOL_TABLE.  Symbol set string contains one bit for any symbol.
   The zero bit means that corresponding symbol is absent in the set. */

char *
new_symbol_set_string (symbol_table)
     register enum symbol *symbol_table;
{
  register char *symbol_set_string;
  register int i, set_string_length;

  set_string_length
    = ((int) SYMBOL_END_MARK + BITS_IN_CHARACTER) / BITS_IN_CHARACTER;
  M2C_ALLOC (symbol_set_string, set_string_length);
  for (i = 0; i < set_string_length; i++)
    symbol_set_string[i] = '\0';
  for (; *symbol_table != SYMBOL_END_MARK; symbol_table++)
    symbol_set_string[(int) *symbol_table / BITS_IN_CHARACTER] |=
      1 << (((int) *symbol_table) % BITS_IN_CHARACTER);
  return symbol_set_string;
}


/* Return TRUE if SEARCHED_SYMBOL is placed in SYMBOL_SET_STRING. */

int
symbol_is_in_set_string (searched_symbol, symbol_set_string)
     register enum symbol searched_symbol;
     register char *symbol_set_string;
{
  return (symbol_set_string[((int) searched_symbol) / BITS_IN_CHARACTER]
	  & 1 << (((int) searched_symbol) % BITS_IN_CHARACTER) ? TRUE : FALSE);
}


/* The function skips all nonstop stream symbol up to any symbol which is in
   SYMBOL_SET_STRING (this symbol will be current symbol).  Search are executed
   only up to the last buffer symbol.  If the symbol is not in set string then
   the function does nothing.  Symbol is stop symbol if the corresponding
   element of IT_IS_STOP_SYMBOL is not zero.  The function returns TRUE
   if current_symbol is contained in SYMBOL_SET_STRING after the call. */

static int
empty_stream_up_to_any_set_string_symbol (symbol_set_string, it_is_stop_symbol)
     register char *symbol_set_string;
     register int it_is_stop_symbol[];
{
  register int i, current_index_in_buffer;

  if (symbol_is_in_set_string (current_symbol, symbol_set_string))
    return TRUE;
  for (i = 0; i < number_of_buffer_elements; i++)
    {
      current_index_in_buffer
 = (i + first_buffer_element_index) % SYMBOL_BUFFER_LENGTH;
      if (!it_is_stop_symbol[(int) symbol_buffer[current_index_in_buffer]
			     .symbol]
	  && (symbol_is_in_set_string
	      (symbol_buffer[current_index_in_buffer].symbol,
	       symbol_set_string)))
	{
	  first_buffer_element_index = current_index_in_buffer;
	  number_of_buffer_elements -= i;
	  next_symbol ();
	  return TRUE;
	}
    }
  return FALSE;
}



/* The following functions used for fixing syntactic error and execution
   of syntax error recovery. */


/* If the following number of successive symbols are matched in recovery
   regime then recover regime is off .*/

#define MATCHED_SYMBOLS_NUMBER_FOR_RECOVER_OFF   3


/* If the following number of successive symbols are matched in recovery
   regime then SKIPPED_SYMBOLS_NUMBER symbols are skipped in the input
   stream and the buffer is again filled.  The buffer used for looking up
   in syntactic error recover. */

#define UNMATCHED_SYMBOLS_NUMBER_TO_SKIP_SYMBOLS 2

/* See upper commentaries. */

#define SKIPPED_SYMBOLS_NUMBER                   1


/* Symbol is stop symbol when corresponding array element is not zero.
   Stop symbol may be skipped in error recovery regime only by matching with
   expected symbol. */

int it_is_stop_symbol[(int) SYMBOL_END_MARK + 1];


/* TRUE if recover regime is on.  The regime is switched on when syntactic
   error is fixed.  If the recover regime is on then the fixed syntactic errors
   are not output. */

static int syntactic_recover_is_on;


/* See commentary for macros MATCHED_SYMBOLS_NUMBER_FOR_RECOVER_OFF. */

static int number_of_matched_symbols;


/* See commentaries for macros UNMATCHED_SYMBOLS_NUMBER_TO_SKIP_SYMBOLS. */

static int number_of_unmatched_symbols;


/* The following function fixes syntactic error (output it if recover
   regime is off), switches on recover regime and fills the buffer for
   following error recovery. */

static void
syntactic_error (error_message)
     char *error_message;
{
  register int i;

  number_of_matched_symbols = 0;
  if (!syntactic_recover_is_on)
    {
      error (error_message);
      number_of_unmatched_symbols = 0;
    }
  syntactic_recover_is_on = TRUE;
  ++number_of_unmatched_symbols;
  if (number_of_unmatched_symbols >= UNMATCHED_SYMBOLS_NUMBER_TO_SKIP_SYMBOLS)
    {
      for (i = 0; i < SKIPPED_SYMBOLS_NUMBER; i++)
	next_symbol ();
      fill_symbol_buffer ();
    }
}

/* The function tests that the current symbol matches with MATCHED_SYMBOL.
   If the matching is failed the function trys to execute error recover
   by looking up in the buffer.  If the matching is successful then
   the function reads next symbol from the input stream and switches off
   recover regime (if it is needed). */

enum symbol
match_symbol (error_message, matched_symbol)
     register char *error_message;
     register enum symbol matched_symbol;
{
  if (matched_symbol != current_symbol)
    {
      syntactic_error (error_message);
      if (!empty_stream_up_to_symbol (matched_symbol, it_is_stop_symbol))
	return current_symbol;
    }
  if (syntactic_recover_is_on)
    {
      number_of_matched_symbols++;
      if (number_of_matched_symbols > MATCHED_SYMBOLS_NUMBER_FOR_RECOVER_OFF)
	syntactic_recover_is_on = FALSE;
    }
  return next_symbol ();
}


/* The function tests that the current symbol matches with any symbol
   from the set string.  If the matching is failed the function fixes error
   and trys to execute error recover by looking up in the buffer.
   If the matching is successful then the function does nothing.
   The function returns TRUE if current_symbol is contained in
   SYMBOL_SET_STRING after the call. */

int
test_current_symbol (symbol_set_string, error_message)
     register char *symbol_set_string;
     char *error_message;
{
  if (!symbol_is_in_set_string (current_symbol, symbol_set_string))
    {
      syntactic_error (error_message);
      return empty_stream_up_to_any_set_string_symbol (symbol_set_string,
						       it_is_stop_symbol);
    }
  return TRUE;
}



/* The following function initiates scanner data for processing new Modula-2
   file. */

void
start_scanner ()
{
  register int i;

  if (VLS_IS_UNDEFINED (symbol_text))
    VLS_CREATE (symbol_text, 500);
  flag_of_range = FALSE;
  number_of_buffer_elements = 0;
  first_buffer_element_index = SYMBOL_BUFFER_LENGTH - 1;
  syntactic_recover_is_on = FALSE;
  for (i = 0; i <= (int) SYMBOL_END_MARK; i++)
    it_is_stop_symbol[i] = 0;
  next_symbol ();
}


/* The following function initiates scanner data for processing all Modula-2
   files.  The function is to be called only when the translator starts. */

void
initiate_scanner_data ()
{
  VLS_UNDEFINE (symbol_text);
}
