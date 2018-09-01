/*************************************************************************

m2c.c - cross compiler from M2 to C

Copyright 2002-2005 by Umberto Salsi <salsi@icosaedro.it>
http://www.icosaedro.it/m2/

*************************************************************************/

#define _GNU_SOURCE /* required by vasprintf() */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/missing.c"

#include "version.h"

/* #define DEBUG
*/

#define FALSE 0
#define TRUE !FALSE

/*
	Command line options:
*/

int debug = 0;
int warn = 1;
int tabsize = 8;


#define FIXME  fixme(__LINE__)

static void fixme(int line)
{
	fprintf(stderr, "m2c " VERSION ": FIXME in line %d\n", line);
	exit(EXIT_FAILURE);
}


#define ERROR  internal_error(__LINE__)

static void internal_error(int line)
{
	fprintf(stderr, "m2c " VERSION ": INTERNAL ERROR in line %d\n", line);
	exit(EXIT_FAILURE);
}


static void OOM(void)
{
	fprintf(stderr, "MEMORY EXAUSTED.\n");
	exit(EXIT_FAILURE);
}


static void * xmalloc(int s)
{
	void *p;

	p = malloc(s);
	if( p == NULL )  OOM();
	return p;
}


static void * xrealloc(void *p, int size)
{
	p = realloc(p, size);
	if( p == NULL )  OOM();
	return p;
}


static char *xstrdup(char *s)
{
	char *t;

	t = strdup(s);
	if( t == NULL )  OOM();
	return t;
}


/* Lexical elements: */
typedef enum {

	/* keywords: */
	andsym, arraysym, beginsym, booleansym, bysym, casesym, catchsym,
	constsym, definitionsym, divsym, dosym, elsesym, elsifsym, endsym,
	errorsym, exitsym, falsesym, finalizersym, forsym, forwardsym,
	functionsym, ifsym, implementationsym, importsym, integersym, loopsym,
	modulesym, nilsym, notsym, ofsym, orsym, raisesym, realsym, recordsym,
	modsym, repeatsym, returnsym, stringsym, switchsym, thensym, tosym,
	truesym, trysym, typesym, untilsym, staticsym, varsym, voidsym, haltsym,
	whilesym,

	/* other symbols: */
	integerlitsym = 1000, reallitsym, plussym, minussym, timessym,
	lroundsym, rroundsym, lsquaresym, rsquaresym, lbracesym,
	rbracesym, commasym, namesym, idsym, colonsym, periodsym, ltsym,
	lesym, gtsym, gesym, eqsym, slashsym, nesym, stringlitsym,
	inlinecodesym, vbarsym, xorsym, ampsym, tildesym, rshiftsym,
	lshiftsym, questionsym, sendsym, arrowsym, eofsym

} symtype;


/* Keywords: */
struct {
	char *name;
	symtype type;
} keywords[] = {
	{"AND",      andsym},
	{"ARRAY",    arraysym},
	{"BEGIN",    beginsym},
	{"BOOLEAN",  booleansym},
	{"BY",       bysym},
	{"CASE",     casesym},
	{"CATCH",    catchsym},
	{"CONST",    constsym},
	{"DEFINITION", definitionsym},
	{"DIV",      divsym},
	{"DO",       dosym},
	{"ELSE",     elsesym},
	{"ELSIF",    elsifsym},
	{"END",      endsym},
	{"ERROR",    errorsym},
	{"EXIT",     exitsym},
	{"FALSE",    falsesym},
	{"FINALIZER", finalizersym},
	{"FOR",      forsym},
	{"FORWARD",  forwardsym},
	{"FUNCTION", functionsym},
	{"HALT",     haltsym},
	{"IF",       ifsym},
	{"IMPLEMENTATION", implementationsym},
	{"IMPORT",   importsym},
	{"INTEGER",  integersym},
	{"LOOP",     loopsym},
	{"MOD",      modsym},
	{"MODULE",   modulesym},
	{"NIL",      nilsym},
	{"NOT",      notsym},
	{"OF",       ofsym},
	{"OR",       orsym},
	{"RAISE",    raisesym},
	{"REAL",     realsym},
	{"RECORD",   recordsym},
	{"REPEAT",   repeatsym},
	{"RETURN",   returnsym},
	{"STATIC",   staticsym},
	{"STRING",   stringsym},
	{"SWITCH",   switchsym},
	{"THEN",     thensym},
	{"TO",       tosym},
	{"TRUE",     truesym},
	{"TRY",      trysym},
	{"TYPE",     typesym},
	{"UNTIL",    untilsym},
	{"VAR",      varsym},
	{"VOID",     voidsym},
	{"WHILE",    whilesym},
};


/*
	Modules:
*/

typedef enum { def, impl, mod } moduletype;

typedef struct {
	moduletype type;
} module;


/*
	Constants:
*/

typedef struct {
	symtype symtype;
	int i;
	char *r; /* REAL literal constant */
	char *s;  int s_len;  /* string literal */
} xconst;


/*
	Types

	The "NIL" type is the type of the NIL literal constant. The type
	"VOID" is not fully implemented; it is useful just to define
	the function m2runtime.count() and lock().
*/

typedef enum{
	NIL, VOID, BOOLEAN, INTEGER, REAL, STRING, ARRAY, RECORD,
	FUNCTION, FORWARD
} base_type;

typedef struct type {
	base_type bt;
	/* if bt==ARRAY: */
	struct type *elem_type;
	/* if bt==RECORD: */
	int n_fields;
	char **field_names;
	struct type **field_types;
	/* if bt==FUNCTION: */
	void *func;  /* forward declared */
} type;


/*
	Variables and function args:
*/

typedef enum {byvalue, byaddress} method;

typedef struct {
	struct type *type;
	method method;
} var;


/*
	Functions:
*/

#define maxargs 10
/* max. no. of arguments. */
/* FIXME: use a dynamic structure. It would be really useful to have
   functions with more than 10 arguments? */

typedef struct {

	int raise_error;
	/* the func. may raise errors? if yes, the func. has a first argument,
	hidden to the programmer, telling if the caller want to handle the error
	by itself using TRY or not. */

	type *type;
	/* type of the result - NULL if it is a procedure */

	method return_method;
	/* return the value or the address? */

	int nargs;
	/* no. of args. */

	type *argtype[maxargs];
	/* type of each arg. */

	method argmethod[maxargs];
	/* method byvalue/byaddress of each arg. */

} function;


/*
	Symbol table
*/

typedef enum {is_module, is_const, is_type, is_var, is_func} id_type;

typedef struct identifier {

	struct identifier * next;
	/* next ID of the linked list */

	char * name;
	/* its name */

	int scope;
	/* lexical scope - needed to detect collisions at the same scope level */

	id_type t;
	/* type of the ID */

	struct identifier * m;
	/* module where the ID is defined - NULL for module names */

	int line;
	/* line of the source where the ID is defined */

	int used;
	/* how many times has been used? */

	/* specific data of each type: */
	union {
		module * m;
		xconst * c;
		type * t;
		var * v;
		function * f;
	} u;

} identifier;

identifier * ids = NULL;
/* linked list of IDs, last inserted first */



/*
	Status of the parser
*/

FILE *src;         /* source file ptr */
char *fn;          /* source file name */
int line, pos;     /* current line no. (>=1), current column no. (>=1) */
int sym_start_line, sym_start_pos;
                   /* where the last symbol fetched started */
int symc;          /* next char */
symtype sym;       /* type of the last symbol found */
int symi;          /* last INTEGER found (if sym == integerlitsym) */

char *syms = NULL; /* last name (if sym==namesym) or literal string found */
                   /* (if sym==stringlitsym) or last literal REAL (if */
                   /* sym==reallitsym) */
int syms_size = 0; /* current allocated size for syms */
int syms_len = 0;  /* current length of syms */

identifier * symid; /* the ID (if sym == qnamesym) */

identifier * ori_module = NULL;
/* the original module for which the compiler was invoked */

identifier * curr_module = NULL;
/* the module we are parsing now */

int scope = 0;
/* current lexical scope */

int nest;          /* indentation level of the formatted C output (>=0) */


typedef struct {
	identifier *curr_module;
	int scope;
	FILE *src;
	char *fn;
	int line, pos;
	int sym_start_line, sym_start_pos;
	int nest;
	symtype sym;
	int symc;
	int symi;
	char *syms;
	int syms_size, syms_len;
} parser_status;


static void suspend_parser(parser_status * ps)
{
	ps->curr_module = curr_module;
	ps->scope = scope;
	ps->src = src;
	ps->fn = fn;
	ps->line = line;
	ps->pos = pos;
	ps->sym_start_line = sym_start_line;
	ps->sym_start_pos = sym_start_pos;
	ps->nest = nest;
	ps->sym = sym;
	ps->symc = symc;
	ps->symi = symi;
	ps->syms = syms;
	ps->syms_size = syms_size;
	ps->syms_len = syms_len;

	syms = NULL;
	syms_size = 0;
	syms_len = 0;
}


static void resume_parser(parser_status * ps)
{
	curr_module = ps->curr_module;
	scope = ps->scope;
	src = ps->src;
	fn = ps->fn;
	line = ps->line;
	pos = ps->pos;
	sym_start_line = ps->sym_start_line;
	sym_start_pos = ps->sym_start_pos;
	nest = ps->nest;
	sym = ps->sym;
	symc = ps->symc;
	symi = ps->symi;
	syms = ps->syms;
	syms_size = ps->syms_size;
	syms_len = ps->syms_len;
}


#ifdef DEBUG

static void print_id(identifier *id)
{
	identifier *m_id;

	if( id == NULL ){
		fprintf(stderr, "<NULL ID>");
	} else {

		/* print module name: */
		m_id = id->m;
		if( m_id == NULL )
			fprintf(stderr, "<NULL>");
		else if( m_id->name == NULL )
			fprintf(stderr, "<NULL*>");
		else
			fprintf(stderr, "%s", m_id->name);

		/* print ID name: */
		if( id->name == NULL )
			fprintf(stderr, ".<NULL>");
		else
			fprintf(stderr, ".%s", id->name);
	}
}


static void print_symbol_table(void)
{
	identifier *id;
	module *m;
	xconst *c;
	type *t;
	var *v;
	function *f;

	id = ids;

	fprintf(stderr, "Symbol table:\n");
	while( id != NULL ){

		print_id(id);
		fprintf(stderr, " used=%d", id->used);
		fprintf(stderr, " is ");

		switch( id->t ){

		case is_module:
			m = id->u.m;
			if( m == NULL ){
				fprintf(stderr, "NULL MODULE");
			} else {
				switch( m->type ){
				case mod: fprintf(stderr, "MODULE"); break;
				case def: fprintf(stderr, "DEFINITION MODULE"); break;
				case impl: fprintf(stderr, "IMPLEMENTATION MODULE"); break;
				default: ERROR;
				}
			}
			break;

		case is_const:
			c = id->u.c;
			fprintf(stderr, "CONST");
			break;

		case is_type:
			t = id->u.t;
			fprintf(stderr, "TYPE");
			break;

		case is_var:
			v = id->u.v;
			fprintf(stderr, "VAR");
			break;

		case is_func:
			f = id->u.f;
			fprintf(stderr, "FUNCTION");
			break;

		default:  ERROR;

		}
		fprintf(stderr, " in line %d\n", id->line);

		id = id->next;

	}
}

#endif


int error_count;
int warning_count;


static void error(char *fmt, ...)
/* Fatal error in syntax. */
{
	va_list ap;
	char *s;

	va_start(ap, fmt);
	vasprintf((char **)&s, fmt, ap);
	va_end(ap);
	fflush(stdout);
	fprintf(stderr, "%s:%d:%d: ERROR: %s\n",
		fn, sym_start_line, sym_start_pos, s);
	free(s);
	error_count++;
	exit(EXIT_FAILURE);
}


static void error_non_fatal(char *fmt, ...)
/* Non-fatal error in syntax. */
{
	va_list ap;
	char *s;

	error_count++;
	va_start(ap, fmt);
	vasprintf((char **)&s, fmt, ap);
	va_end(ap);
	fflush(stdout);
	fprintf(stderr, "%s:%d:%d: ERROR: %s\n",
		fn, sym_start_line, sym_start_pos, s);
	free(s);
}


static void warning(char *fmt, ...)
/* Warning on the syntax/semantic. */
{
	va_list ap;
	char *s;

	warning_count++;
	if( ! warn )  return;
	va_start(ap, fmt);
	vasprintf((char **)&s, fmt, ap);
	va_end(ap);
	fflush(stdout);
	fprintf(stderr, "%s:%d:%d: warning: %s\n",
		fn, sym_start_line, sym_start_pos, s);
	free(s);
}


static void add_id(char * name, id_type t)
/*
	Add a new ID to the symbol table. Use the current module.
	The caller must ensure the ID is not duplicated in current
	scope, either using an appropriate argument for get_sym(),
	or using check_name_in_curr_module_same_scope(name).
*/
{
	identifier * id;
	module * m;
	xconst *c;
	var *v;
	function *f;

	id = xmalloc( sizeof(identifier) );
	id->next = ids;
	id->name = xstrdup(name);
	id->scope = scope;
	id->t = t;
	id->m = curr_module;
	id->line = sym_start_line;
	id->used = 0;
	switch( t ){
	
		case is_module:
			id->m = NULL;
			m = xmalloc( sizeof(module) );
			id->u.m = m;
			/*id->u.m->type = ...; */
			break;

		case is_const:
			c = xmalloc( sizeof(xconst) );
			c->symtype = 0; /* invalid */
			c->i = 0;
			c->r = NULL;
			c->s = NULL;
			id->u.c = c;
			break;

		case is_type:
			id->u.t = NULL;
			break;

		case is_var:
			v = xmalloc( sizeof(var) );
			v->type = NULL;
			v->method = 0; /* invalid */
			id->u.v = v;
			break;

		case is_func:
			f = xmalloc( sizeof(function) );
			f->raise_error = 0;
			f->type = NULL;
			f->nargs = 0;
			/* f->argtype[] = ... */
			/* f->argmethod[] = ... */
			id->u.f = f;
			break;

		default: ERROR;
	
	}
	ids = id;
}


/*
	Simple types are allocated once for all.
*/
type *NIL_type;
type *VOID_type;
type *BOOLEAN_type;
type *INTEGER_type;
type *REAL_type;
type *STRING_type;


static void add_simple_types(void)
{
	type * add_type(char *name, base_type bt)
	{
		type * t;

		t = xmalloc( sizeof(type) );
		t->bt = bt;
		t->elem_type = NULL;
		t->n_fields = 0;
		t->func = NULL;
		return t;
	}

	NIL_type     = add_type("NIL", NIL);
	VOID_type    = add_type("VOID", VOID);
	BOOLEAN_type = add_type("BOOLEAN", BOOLEAN);
	INTEGER_type = add_type("INTEGER", INTEGER);
	REAL_type    = add_type("REAL", REAL);
	STRING_type  = add_type("STRING", STRING);
}


static void rm_type(type * t)
{

#ifdef NOT_USED_NOT_USED
	/*
		FIXME: I can't release types because VAR, TYPE and FUNCTION
		may share the data structures.
	*/

	int i;

	if( t == VOID_type
	|| t == INTEGER_type
	|| t == REAL_type
	|| t == STRING_type ){
		return;
	} else if( t->bt == ARRAY ){
		rm_type(t->elem_type);
		free(t);
	} else if( t->bt == RECORD ){
		for( i = t->n_fields; i >= 0; i-- ){
			free(t->field_names[i]);
			rm_type(t->field_types[i]);
		}
		free(t->field_names);
		free(t->field_types);
		free(t);
	} else if( t->bt == FUNCTION ){
		FIXME;
	} else if( t == NULL ){
		FIXME;
	} else {
		printf("bt=%d\n", t->bt);
		FIXME;
	}
#endif

}


static void rm_id(void)
/*
	Remove the first ID from the symbol table.
*/
{
	identifier * id;

	if( ids == NULL )  ERROR;
	id = ids;
	ids = id->next;

	switch( id->t ){
	
		case is_module: free(id->u.m);    break;

		case is_const:
			free(id->u.c->s);
			free(id->u.c->r);
			free(id->u.c);
			break;

		case is_type:   rm_type(id->u.t); break;

		case is_var:    free(id->u.v);    break;

		case is_func:   free(id->u.f);    break;
	}

	free(id->name);
	free(id);
}


static void symbol_table_rollback(identifier *upto)
/*
	Release all the symbols up to `upto' (excluding this latter).
*/
{
	static char *is[] = {"module", "constant", "type", "variable/argument",
		"function"};

	while( ids != upto ){
		if( warn && ! ids->used ){
			if( ids->m == curr_module && ids->t != is_module ){
				fprintf(stderr, "%s:%d: warning: %s `%s' not used\n",
					fn, ids->line, is[ids->t], ids->name);
			} else if( ids->m != curr_module && ids->t == is_module ){
				fprintf(stderr, "%s:%d: warning: module `%s' "
					"imported but not used\n",
					fn, ids->line, ids->name);
			}
		}
		rm_id();
	}
}


static int next_char(void)
{
	int c;

	c = fgetc(src);
	pos++;
	if( c == '\n' ){
		line++;
		pos = 0;
	} else if( c == '\t' ){
		pos += tabsize - 1;
	}
	return c;
}


static int is_letter(int c)
{
	return ('a'<=c && c<='z') || ('A'<=c && c<='Z');
}


static void skip_comment(void)
{
	int c, prec, l;

	l = line;
	prec = 'x';
	do{
		c = next_char();
		if( c == EOF ){ 
			error("unclosed comment opened in line %d", l);
		}
		if( c == ')' && prec == '*' ){
			symc = next_char();
			return;
		} else if( c == '*' && prec == '(' ){
			skip_comment();
			c = symc;
			prec = 'x';
		} else {
			prec = c;
		}
	} while( 1 );
}


/*
	Utils for writing the obj file

	OutBuffer is a concatenated list of character buffers of arbitrary
	size. The last OutBuffer added is the current output buffer.

	new_out_buffer() allocate a new, empty buffer as current buffer.

	close_out_buffer(b) remove the current buffer and restore the
	previous buffer of the list as current. In effect, the argument
	`b' isn't really necessary, because when this function is called
	it MUST be b == curr_out_buffer, but it introduce a redundancy
	usefull to check unbalanced calls to open_out_buffer() and
	free_out_buffer().

	Any time we need to write code to the obj file, we call one of the
	functions of the out_*(), or the out() itself. The function out()
	concatenate the produced code into the current output buffer.

	Using the strategy of these output buffer, the parser can take a
	"look" forward to the code and resolve expressions, postfix
	notation and other issues.
*/

FILE *obj_file = NULL;  /* parse_inlinecode() need this var. */


typedef struct OutBuffer {
	struct OutBuffer * prev;  /* previous out buffer */
	int size; /* current allocated size in 'buf' */
	int n;    /* current used space in 'buf' */
	int open; /* 1=open; 0=closed (only for debugging) */
	char *p; /* the buffer */
} OutBuffer;


OutBuffer * curr_out_buffer = NULL;
/* If this var. is NULL, out*() will print directly to the file obj_file.
   If not NULL, out*() will store chars into this buffer. */

OutBuffer * unused_out_buffer = NULL;
/* List of unused output buffers. */


OutBuffer * open_out_buffer(void)
/*
	Allocate a new output buffer and set it to be the current output
	"file" for out().
*/
{
	OutBuffer *b;

	if( unused_out_buffer != NULL ){
		b = unused_out_buffer;
		unused_out_buffer = b->prev;
	} else {
		b = xmalloc( sizeof(OutBuffer) );
		b->size = 50;
		b->n = 0;
		b->p = xmalloc(b->size);
	}

	b->prev = curr_out_buffer;
	b->open = 1;
	curr_out_buffer = b;
	return b;
}


static void add_buffer(OutBuffer *b, char *s, int len)
{
	int r;

	r = b->n + len;
	if( r > b->size ){
		r = 2*r;
		b->p = xrealloc(b->p, r);
		b->size = r;
	}
	memcpy(b->p + b->n, s, len);
	b->n += len;
}


static void close_out_buffer(OutBuffer *b)
/*
	Restore the previous output buffer.
*/
{
	if( curr_out_buffer != b )  ERROR;
	if( ! b->open )  ERROR;
	b->open = 0;
	curr_out_buffer = b->prev;
	b->prev = NULL;
}


static void free_out_buffer(OutBuffer *b)
/*
	Release the output buffer. In effect, it is simply added to the
	list of the unused buffers to be reused later if necessary.
*/
{
	if( b->open )  ERROR;
	b->n = 0;
	b->prev = unused_out_buffer;
	unused_out_buffer = b;
}


static void out(char *fmt, ...)
/*
	Send data formatted as per 'fmt' to the current output buffer.
	If the current output buffer is not set, output directly to
	the obj_file.
*/
{
	va_list ap;
	char *s;
	char buf[1000];
	int n;
	OutBuffer *b;

	b = curr_out_buffer;
	if( b == NULL ){
		va_start(ap, fmt);
		vfprintf(obj_file, fmt, ap);
		va_end(ap);
	} else {
		va_start(ap, fmt);
		n = vsnprintf(buf, 1000, fmt, ap);
		va_end(ap);
		if( n < 1000 ){
			add_buffer(b, buf, n);
		} else {
			va_start(ap, fmt);
			n = vasprintf((char **)&s, fmt, ap);
			va_end(ap);
			add_buffer(b, s, n);
			free(s);
		}
	}
}


static void out_out_buffer(OutBuffer *b)
/*
	Send the buffer contents to the current output buffer.
*/
{
	if( b->open )  ERROR;
	add_buffer(b, "\0", 1);
	out("%s", b->p);
}


static void out_lit_string(char *s, int len)
/*
	Print literal STRING. Strings are translated to a sequence of bytes
	that mimic the structure of the type STRING as defined in m2runtime.c,
	with the type identifier (int, 0), the actual length (int) and then
	the chars. The INTSTR() macro, defined in include/missing.c,
	makes the C source so generated independent from the byte ordering,
	althought still dependent from sizeof(int).
*/
{
	void out_lit_int_as_string(int n)
	{
		int i;
		char *p;

		out("INTSTR(");
		p = (char *) &n;
		for( i=0; i<sizeof(int); i++ ){
			if( i > 0 )
				out(",");
			out("\\%o", (unsigned char) *p);
			p++;
		}
		out(")");
	}


	int i, c;

	if( len == 0 ){
		out("EMPTY_STRING");
		return;
	}

	if( len == 1 ){
		out("m2runtime_CHR(%d)", (unsigned char) s[0]);
		return;
	}

	out("(STRING *) ");

	/* print the type encoded as string: */
	out_lit_int_as_string( 0 /* = type is STRING */ );  /* FIXME: define a constant */

	/* print the length encoded as string: */
	out_lit_int_as_string( len );

	/* print the chars: */
	out("\"");
	for( i=0; i<len; i++ ){
		c = (unsigned char) s[i];
		if( c >= 32 && c <= 126 && c != '"' && c != '\\' )
			out("%c", c);
		else
			out("\\%.3o", c);
	}

	out("\"");
}


static void out_var_byvalue(identifier *v)
{
	out("%s%s_%s", v->u.v->method == byvalue? "":"*",
		v->m->name, v->name);
}


static void out_var_byaddress(identifier *v)
{
	/* NOTE: If v is a (void *), its address result to be (void **),
	   but this latter isn't compatible with any pointer, gcc say.
	   We need to force (void *): */
	if( v->u.v->type->bt == FORWARD )
		out("(void *)");

	out("%s%s_%s", v->u.v->method == byvalue? "&":"",
		v->m->name, v->name);
}


static void out_cast_byvalue(base_type t)
/*
	Cast for dereferenced values given by m2runtime_dereference_rhs_ARRAY()
	and m2runtime_dereference_rhs_RECORD().
*/
{
	switch(t){
	case VOID:    out("(void *)"); break;
	case BOOLEAN: out(" *(int *)"); break;
	case INTEGER: out(" *(int *)"); break;
	case REAL:    out(" *(double *)"); break;
	case STRING:  out("(STRING *)"); break;
	case ARRAY:   out("(ARRAY *)"); break;
	case RECORD:  out("(RECORD *)"); break;
	case FORWARD: out("(void *)"); break;
	case FUNCTION: out("(void *)"); break;
	default: ERROR;
	}
}


static void out_cast_byaddress(base_type t)
/*
	Cast for dereferenced values given by m2runtime_dereference_lhs_RECORD()
	and m2runtime_dereference_lhs_ARRAY().
*/
{
	switch(t){
	case VOID:    out("(void *)"); break;
	case BOOLEAN: out("(int *)"); break;
	case INTEGER: out("(int *)"); break;
	case REAL:    out("(double *)"); break;
	case STRING:  out("(STRING **)"); break;
	case ARRAY:   out("(ARRAY **)"); break;
	case RECORD:  out("(RECORD **)"); break;

	/* FIXME: this gives warning: passing arg 2 of `io_Close' from incompatible pointer type */
	/*case FORWARD: out("(void **)"); break; */
	/* ...instead this works, but FORWARD can also be ARRAY: */
	case FORWARD: out("(RECORD **)"); break;

	case FUNCTION: out("(void *)"); break;
	default: ERROR;
	}
}


static void init_obj(char *fn)
{
	obj_file = fopen(fn, "w");
	if( obj_file == NULL ){
		fprintf(stderr, "fopen(%s, w): %s\n", fn, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if( curr_out_buffer != NULL )
		ERROR;
}


static void indent(void)
{
	int i = nest;
	out("/*%3d*/ ", line);
	while( i > 0 ){
		out("\t");
		i--;
	}
}


void syms_reset()
{
	if( syms == NULL )
		syms_size = 0;
	syms_len = 0;
}


void syms_add_char(int c)
{
	if( syms_len >= syms_size ){
		syms_size = 2*syms_size + 1000;
		syms = xrealloc(syms, syms_size);
	}
	syms[syms_len++] = c;
}


void syms_add_str(char *s)
{
	while( *s != 0 )
		syms_add_char(*s++);
}


void syms_add_mem(char *buf, int len)
{
	char *end;

	end = buf + len;
	while( buf != end ){
		syms_add_char(*buf);
		buf++;
	}
}


static void get_number(void)
/*
	Parse integerlitsym and reallitsym:

		"0x"hexdigit{hexdigit}
		digit{digit}["."{digit}]["e"|"E"["+"|"-"]digit{digit}]

	Integers are returned in symi. Reals are returned as string syms
	(so leaving to the C compiler the actual conversion to binary).
*/
{
	int c, i;

	c = symc;
	i = 0;
	syms_reset();
	syms_add_char(c);
	c = next_char();
	if( syms[0] == '0' && c == 'x' ){
		i = 0;
		c = next_char();
		if( ! isxdigit(c) )
			error_non_fatal("expected hexadecimal digit");
		/* FIXME: check overflow. */
		while( isxdigit(c) ){
			if( isdigit(c) )
				i = 16*i + (c-'0');
			else
				i = 16*i + (toupper(c)-'A'+10);
			c = next_char();
		}
		sym = integerlitsym;
		symi = i;
		symc = c;
		return;
	}
	i = 1;
	while( isdigit(c) ){
		syms_add_char(c);
		c = next_char();
	}
	if( c == '.' || c == 'e' || c == 'E' ){
		if( c == '.' ){
			syms_add_char(c);
			c = next_char();
			while( isdigit(c) ){
				syms_add_char(c);
				c = next_char();
			}
		}
		if( c == 'e' || c == 'E' ){
			syms_add_char(c);
			c = next_char();
			if( c == '+' || c == '-' ){
				syms_add_char(c);
				c = next_char();
			}
			if( ! isdigit(c) )
				error_non_fatal("missing exponent in literal REAL number");
			while( isdigit(c) ){
				syms_add_char(c);
				c = next_char();
			}
		}
		sym = reallitsym;
		symc = c;
		syms_add_char(0);
		return;
	}
	sym = integerlitsym;
	symc = c;
	/* Convert syms to int: */
	/* FIXME: parsing of "-2147483648" (-2^31) gives minussym (correct)
	   and integerlitsym with symi=2147483647 (WRONG). */
	syms_add_char(0);
	symi = atoi(syms);
	return;
}


static void get_string(void)
{
	int h;
	unsigned int c;
	int warn_ctrl = 1;


	void parse_escape()
	{
		unsigned int c;

		c = next_char();

		switch( c ){
		case '\\': c = '\\'; break;
		case '"': c = '"';  break;
		case 'a': c = '\a'; break;
		case 'b': c = '\b'; break;
		case 'n': c = '\n'; break;
		case 'r': c = '\r'; break;
		case 't': c = '\t'; break;

		case 'x':
			c = next_char();
			if( ! isxdigit(c) ){
				error_non_fatal("expected hexadecimal digit");
				return;
			}
			if( isdigit(c) )
				h = c - '0';
			else
				h = toupper(c) - 'A' + 10;
			c = next_char();
			if( ! isxdigit(c) ){
				error_non_fatal("expected hexadecimal digit");
				return;
			}
			if( isdigit(c) )
				h = 16*h + c - '0';
			else
				h = 16*h + toupper(c) - 'A' + 10;
			c = h;
			break;

		default:
			error_non_fatal("unknown escape sequence \\%c in literal "
				"string", c);
		}

		syms_add_char(c);
	}


	sym = stringlitsym;
	syms_reset();
	do{
		c = next_char();
		if( c == '"' ){
			symc = next_char();
			break;
		} else if( c == EOF ){
			error("premature end of the file");
		} else if( c == '\\' ){
			parse_escape();
		} else if( c < 32 || c == 127 ){
			if( warn_ctrl ){
				error_non_fatal("control character code %d in literal "
					"string", c);
				warn_ctrl = 0;
			}
		} else {
			syms_add_char(c);
		}
	} while( 1 );
	syms_add_char(0);
}


void check_name_in_curr_module_same_scope(char * name)
/*
	Gives an error if the name is already declared in the
	current module at the current scope level.
*/
{
	identifier * id;
	char *s;

	id = ids;
	while( id != NULL ){
		if( id->m != NULL
		&& id->scope == scope
		&& strcmp(name, id->name) == 0
		&& id->m != NULL && strcmp(id->m->name, curr_module->name) == 0 ){
			switch( id->m->u.m->type ){
			case mod: s = "MODULE"; break;
			case def: s = "DEFINITION MODULE"; break;
			case impl: s = "IMPLEMENTATION MODULE"; break;
			default: ERROR;
			}
			error_non_fatal("new identifier `%s' shadows homonymous identifier "
				"declared in %s %s, line %d",
				name,
				s,
				id->m->name,
				id->line
			);
		}
		id = id->next;
	}
}


identifier * search_name(char * name)
/*
	Search for the name not qualified. First, search in the
	current module.  Gives a fatal error if not found in any
	module or found in 2 or more modules. Used by get_sym()
	and parse_function_call().
*/
{
	identifier * id, *found;
	int n;

	/*
		Search 'name' in the current module:
	*/
	id = ids;
	while( id != NULL ){
		if( id->m != NULL
		&& strcmp(name, id->name) == 0
		&& strcmp(id->m->name, curr_module->name) == 0 ){
			return id;
		}
		id = id->next;
	}

	/*
		Search 'name' in the others modules:
	*/
	id = ids;
	found = NULL;
	n = 0;
	while( id != NULL ){
		if( id->m != NULL && strcmp(name, id->name) == 0 ){
			if( n == 0 ){
				found = id;
				n = 1;
			} else if( n == 1 /** && id->m != found->m **/ ){
				warning("please qualify `%s' already defined "
					"in modules `%s' and `%s'",
					name,
					found->m->name,
					id->m->name );
				n = 2;
			} else {
				warning("...and in module `%s'", id->m->name);
				n++;
			}
		}
		id = id->next;
	}

	if( n == 0 ){
		error("`%s' is not defined", name);
	} else if( n > 1 ){
		error_non_fatal("ambigous unqualified name %s", name);
	}

	return found;
}


static void get_sym(int enable_qname)
/*
	enable_qname == 0
	Do not allow qualified name. Gives error if the ID is already
	in use in current module, current scope. Returns sym==namesym,
	syms==the ID name.

	enable_qname == 1
	Allows qualified name. If qualified, returns sym==idsym, symid=ID.
	If not qualified, first search in the current module scanning
	scopes backward, then try the other modules: if not found or
	found in 2+ modules, gives error. Returns sym==idsym, symid==ID.

	enable_qname == 2
	Do not allow qualified name. Search in the current module scanning
	scopes backward. If found, return sym==idsym, symid==ID.  If not
	found, return sym==namesym, syms==the ID name.

	enable_qname == 3
	Do not allow qualified name. Returns sym==namesym, syms==the
	ID name.

	enable_qname == 4
	Allows qualified names. If qualified, return sym=idsym, symid = the
	ID. If not qualified, return sym==namesym leaving to the caller to
	resolve the name. Used in: resolving function f(v) in postfix notation
	v->f() using the type of the prefix variable v.
*/
{

	void get_id(void)
	{
		syms_reset();
		syms_add_char(symc);
		symc = next_char();
		while( is_letter(symc) || isdigit(symc) || symc == '_' ){
			syms_add_char(symc);
			symc = next_char();
		}
		syms_add_char(0);
	}


	identifier * search_name2(char * name)
	/*
		Search the name in the current module, from current
		scope upward. Return NULL if not found.
	*/
	{
		identifier * id;

		id = ids;
		while( id != NULL ){
			if( id->m != NULL
			&& strcmp(name, id->name) == 0
			&& strcmp(curr_module->name, id->m->name) == 0 ){
				return id;
			}
			id = id->next;
		}
		return NULL;
	}


	char * search_module(char *name)
	/*
		Search for a module named `name' and returns the pointer
		to its name as stored in the symbol table. Returns NULL
		if not found.
	*/
	{
		identifier * id;

		id = ids;
		while( id != NULL ){
			if( id->m == 0 && strcmp(name, id->name) == 0 )
				return id->name;
			id = id->next;
		}
		return NULL;
	}


	identifier * search_qname(char * m, char * name)
	/*
		Search for the qualified name. Gives error if unknow module
		or unknow ID in module.
	*/
	{
		identifier * id;
		int module_found = 0;

		id = ids;
		while( id != NULL ){
			if( id->m != 0 && strcmp(m, id->m->name) == 0 ){
				module_found = 1;
				if( strcmp(name, id->name) == 0 ){
					return id;
				}
			}
			id = id->next;
		}

		if( module_found )
			error("`%s.%s': the module does not export this identifier",
				m, name);
		else
			error("`%s.%s': unknown module", m, name);

		return 0; /* make happy gcc -Wall */
	}


	int i, j, m, r;
	unsigned int c;

get_sym_again:

	while( symc == ' ' || symc == '\t' || symc == '\n' || symc == '\r' ){
		symc = next_char();
	}

	sym_start_line = line;
	sym_start_pos  = pos;

	if ( symc == '$' && pos == 1 ){
		symc = next_char();
		sym = inlinecodesym;
		return;
	}

	if( isdigit(symc) ){
		get_number();
		return;
	}

	if( is_letter(symc) || symc == '_' ){
		get_id();

		/* is syms a keyword? */
		i = 0;
		j = whilesym;
		while( i <= j ){
			m = (i + j) / 2;
			r = strcmp(syms, keywords[m].name);
			if( r < 0 )  j = m-1;
			else if( r > 0 )  i = m+1;
			else {
				sym = keywords[m].type;
				return;
			}
		}

		/* is syms a qualified name? */
		if( symc == '.' ){
			char *m;
			if( enable_qname != 1 && enable_qname != 4 )
				error("qualification not allowed here");
			m = search_module(syms);
			if( m == NULL )
				error_non_fatal("unknown module `%s'", m);
			symc = next_char();
			if( !( is_letter(symc) || symc == '_' ) )
				error("invalid character `%c' - expected name", c);
			get_id();
			if( m == NULL )
				symid = search_name(syms);
			else
				symid = search_qname(m, syms);
		} else if( enable_qname == 0 ){
			check_name_in_curr_module_same_scope(syms);
			sym = namesym;
			return;
		} else if( enable_qname == 1 ){
			symid = search_name(syms);
		} else if( enable_qname == 2 ){
			symid = search_name2(syms);
			if( symid == NULL ){
				sym = namesym;
				return;
			} else {
				sym = idsym;
				return;
			}
		} else if( enable_qname == 3 || enable_qname == 4 ){
			sym = namesym;
			return;
		} else {
			ERROR;
		}

		sym = idsym;
		symid->used = 1;
		if( symid->m != NULL && symid->m != curr_module )
			symid->m->used = 1;

		/* check if this ID is a constant: */
		if( sym == idsym && symid->t == is_const ){
			xconst * co;
			co = symid->u.c;
			sym = co->symtype;
			if( sym == booleansym ){
				symi = co->i;
				if( symi == FALSE )
					sym = falsesym;
				else
					sym = truesym;
			} else if( sym == integerlitsym ){
				symi = co->i;
			} else if( sym == reallitsym ){
				syms_reset();
				syms_add_str(co->r);
				syms_add_char(0);
			} else if( sym == stringlitsym ){
				syms_reset();
				syms_add_mem(co->s, co->s_len);
				syms_add_char(0);
			} else {
				ERROR;
			}
			return;
		}
		return;
	}

	/* literal string */
	if( symc == '"' ){
		get_string();
		return;
	}

	if( symc == '(' ){
		symc = next_char();
		if( symc == '*' ){
			skip_comment();
			get_sym(enable_qname);
			return;
		}
		sym = lroundsym;
		return;
	}

	if( symc == '#' ){
		do{
			c = next_char();
		}while( c != '\n' && c != EOF );
		if( c == EOF ){
			symc = EOF;
			sym = eofsym;
			return;
		}
		symc = next_char();
		get_sym(enable_qname);
		return;
	}

	if( symc == '<' ){
		sym = ltsym;
		symc = next_char();
		if( symc == '=' ){
			sym = lesym;
			symc = next_char();
		} else if( symc == '>' ){
			sym = nesym;
			symc = next_char();
		} else if( symc == '<' ){
			symc = next_char();
			sym = lshiftsym;
		}
		return;
	}

	if( symc == '>' ){
		sym = gtsym;
		symc = next_char();
		if( symc == '=' ){
			sym = gesym;
			symc = next_char();
		} else if( symc == '>' ){
			sym = rshiftsym;
			symc = next_char();
		}
		return;
	}

	if( symc == '-' ){
		symc = next_char();
		if( symc == '>' ){
			sym = arrowsym;
			symc = next_char();
			return;
		} else if( symc == '-' ){
			symc = next_char();
			if( symc != '>' )
				error("unknown symbol");
			symc = next_char();
			sym = sendsym;
			return;
		} else {
			sym = minussym;
			return;
		}
	}

	switch( symc ){
	case ':':  sym = colonsym;   break;
	case '.':  sym = periodsym;  break;
	case ',':  sym = commasym;   break;
	case ')':  sym = rroundsym;  break;
	case '[':  sym = lsquaresym; break;
	case ']':  sym = rsquaresym; break;
	case '{':  sym = lbracesym;  break;
	case '}':  sym = rbracesym;  break;
	case '+':  sym = plussym;    break;
	/*case '-':  sym = minussym;   break; */
	case '*':  sym = timessym;   break;
	case '/':  sym = slashsym;   break;
	case '=':  sym = eqsym;      break;
	case '&':  sym = ampsym;     break;
	case '|':  sym = vbarsym;    break;
	case '^':  sym = xorsym;     break;
	case '~':  sym = tildesym;   break;
	case '?':  sym = questionsym;break;
	case EOF:  sym = eofsym;  return;
	default:
		if( symc >= 32 && symc <= 126 )
			error_non_fatal("invalid character `%c'", symc);
		else
			error_non_fatal("invalid character code %d", symc);
		symc = next_char();
		goto get_sym_again;
	}
	symc = next_char();

}


static void new_parser(char *file_name)
/*
	Open the given file and initialize the parser status.
*/
{
	src = fopen(file_name, "r");
	if( src == NULL ){
		fprintf(stderr, "fopen(%s, r): %s\n", file_name, strerror(errno));
		exit(EXIT_FAILURE);
	}
	fn = xstrdup(file_name);
	line = 1;
	pos = 0;
	sym_start_line = 1;
	sym_start_pos = 0;
	nest = 0;
	syms_reset();
	symc = next_char();
	symid = NULL;
	get_sym(1);
	if( debug )
		fprintf(stderr, "Begin parsing of %s\n", fn);
}


static void free_parser(void)
{
	if( debug )
		fprintf(stderr, "End parsing of %s\n", fn);
	fclose(src);
	free(fn);
	free(syms);
	syms = NULL;
	syms_size = 0;
	syms_len = 0;
}


static void parse_inlinecode(void)
/*
	"$$..." --> write to stdout
	"$SP"  --> write to obj_file
*/
{
	int c;

	if( symc == '$' ){ /* --> stdout */
		c = next_char();
		while( c != '\n' && c != EOF ){
			putchar(c);
			c = next_char();
		}
		putchar('\n');
	} else { /* --> obj */
		c = symc;
		while( c != '\n' && c != EOF ){
			if( obj_file != NULL ) out("%c", c);
			c = next_char();
		}
		if( obj_file != NULL ) out("\n");
	}
	if( c == EOF ){
		symc = EOF;
		return;
	}
	symc = next_char();
	get_sym(1); /* FIXME: 1? */
}


static void parse_CONST(void)
/*
	Parse the CONST section.

	FIXME: allow simple static expressions.
*/
{
	xconst *c;
	int sign; /* 0=no sign, +1=plus sign, -1=minus sign */
	char *s;

	do {
		get_sym(0);
		if( sym != namesym ) return;
		add_id(syms, is_const);
		c = ids->u.c;
		get_sym(0);
		if( sym != eqsym ) error("expected `='");
		get_sym(1);
		/* FIXME: minus sign should be allowed only for integers and reals */
		switch( sym ){
			case plussym: sign = +1; get_sym(1); break;
			case minussym: sign = -1; get_sym(1); break;
			default: sign = 0;
		}
		switch( sym ){
			case falsesym:
			case truesym:
				c->symtype = booleansym;
				c->i = (sym == truesym);
				break;
			case integerlitsym:
				if( sign != 0 )  symi = sign * symi;
				c->symtype = integerlitsym;
				c->i = symi;
				break;
			case reallitsym:
				s = xmalloc(syms_len + 1);
				s[0] = 0;
				if( sign < 0 )  strcpy(s, "-");
				strcat(s, syms);
				c->symtype = reallitsym;
				c->r = s;
				break;
			case stringlitsym:
				if( sign != 0 )  error("bad syntax");
				c->symtype = stringlitsym;
				c->s_len = syms_len-1;
				c->s = xmalloc(c->s_len);
				memcpy(c->s, syms, c->s_len);
				break;
			default:
				error("bad constant");
		}
	}while( 1 );
}


static int is_dynamic(type *t)
/*
	Return 1 if t is a dynamic type (aka a pointer), 0 otherwise.
	NOTE: I can't return TRUE/FALSE because the resulting value
	is used for a counter.
*/
{
	switch( t->bt ){

		case NIL:  return 1;

		case VOID:  FIXME;

		case BOOLEAN:
		case INTEGER:
		case REAL:
			return 0;

		case STRING:
		case ARRAY:
		case RECORD:
		case FUNCTION:
		case FORWARD:
			return 1;

		default: ERROR;
	}
	return 0;
}


static int n_dyn_fields(type *r)
{
	int n, i;

	n = 0;
	for( i = 0; i < r->n_fields; i++ )
		n += is_dynamic(r->field_types[i]);
	return n;
}


static int same_function_signature(function *a, function *b);


static int same_type(type *a, type *b)
/*
	Return 1 if the two types have the same structure, 0 otherwise.
	The type NIL match all the dynamic types (NIL, STRING, ARRAY,
	RECORD, FORWARD and FUNCTION).
	The VOID type match any type.
*/
{
	int i, b_bt;

	if( a == b )  return 1;
	if( a == NULL || b == NULL )  return 0;

	b_bt = b->bt;
	
	switch( a->bt ){

		case NIL:
			return (b_bt == NIL) || (b_bt == VOID) || (b_bt == STRING)
			|| (b_bt == ARRAY) || (b_bt == RECORD) || (b_bt == FUNCTION)
			|| (b_bt == FORWARD);

		case VOID:
			return 1;

		case BOOLEAN:
		case INTEGER:
		case REAL:
			return (b_bt == a->bt) || (b_bt == VOID);

		case STRING:
			return (b_bt == NIL) || (b_bt == VOID) || (b_bt == STRING);

		case ARRAY:
			return (b_bt == NIL) || (b_bt == VOID)
			|| same_type(a->elem_type, b->elem_type);

		case RECORD:
			if( b_bt == NIL || b_bt == VOID )  return 1;
			if( b_bt != RECORD )  return 0;
			if( a->n_fields != b->n_fields )  return 0;
			for( i=0; i<a->n_fields; i++ ){
				if( ! same_type(a->field_types[i], b->field_types[i]) )
					return 0;
			}
			return 1;

		case FUNCTION:
			return (b_bt == NIL) || (b_bt == VOID)
			|| (b_bt == FUNCTION && same_function_signature(a->func, b->func));

		case FORWARD:
			return (b_bt == NIL) || (b_bt == VOID);

		default: ERROR;

	}
	return 0; /* make happy gcc -Wall */
}


static void error_type_mismatch(char *msg, type *a, type *b)
/*
	a: found type
	b: expected type
*/
{
	char as[1000], bs[1000];

	void add(char *s, char *m)
	{
		if( strlen(s) + strlen(m) + 1 >= 1000 )
			return;
		if( *s != 0 )
			strcat(s, "/");
		strcat(s, m);
	}

	char * base_type_to_string(int bt)
	{
		switch( bt ){
		case NIL: return "NIL";
		case VOID: return "VOID";
		case BOOLEAN: return "BOOLEAN";
		case INTEGER: return "INTEGER";
		case REAL: return "REAL";
		case STRING: return "STRING";
		case ARRAY: return "ARRAY";
		case RECORD: return "RECORD";
		case FUNCTION: return "FUNCTION";
		case FORWARD: return "FORWARD";
		default: return "<?>";
		}
	}

	*as = 0;
	*bs = 0;

	if( a == NULL ){
		add(as, "<?>");
	} else {
		add(as, base_type_to_string(a->bt));
	}

	if( b == NULL ){
		add(bs, "<?>");
	} else {
		add(bs, base_type_to_string(b->bt));
	}

	error_non_fatal("%s: found %s, expected %s", msg, as, bs);
}


static int same_function_signature(function *a, function *b)
{
	int i;

	if( a == b )  return 1;
	if( a->nargs != b->nargs
		|| a->raise_error != b->raise_error
		|| ! same_type(a->type, b->type)
	)
		return 0;
	for( i=0; i<a->nargs; i++ ){
		if( a->argmethod[i] != b->argmethod[i]
		|| ! same_type(a->argtype[i], b->argtype[i]) )
			return 0;
	}
	return 1;
}


static type * parse_subtype(type * t)
/*
	If t!=NULL it is the preallocated space of the type to be returned,
	used ONLY by parse_TYPE().
*/
{

	type * parse_enum_type(void)
	{
		int count = 0;
		xconst *c;

		get_sym(0);
		while( 1 ){
			if( sym != namesym )
				error("expected name");
			add_id(syms, is_const);
			c = ids->u.c;
			c->symtype = integerlitsym;
			c->i = count;
			get_sym(1);
			if( sym == eqsym ){
				get_sym(1);
				if( sym != integerlitsym )
					error("expected literal integer");
				if( symi < count )
					error("expected value >= %d", count);
				count = symi;
				c->i = symi;
				get_sym(1);
			}
			count++;
			if( sym == commasym )
				get_sym(0);
			else
				break;
		}
		if( sym != rroundsym )
			error("expected `)'");
		get_sym(2);
		return INTEGER_type;
	}


	type * parse_array_type(type * t)
	{
		if( t == NULL )
			t = xmalloc(sizeof(type));
		t->bt = ARRAY;
		get_sym(1);
		if( sym == ofsym ){
			get_sym(1);
		} else {
			error("expected OF after ARRAY");
		}
		t->elem_type = parse_subtype(NULL);
		return t;
	}

/* FIXME: remove this static limit. */
#define max_fields 100

	type * parse_record_type(type * t)
	{
		int n, i, j;
		char **names;
		type **types;

		names = xmalloc(max_fields * sizeof(char *));
		types = xmalloc(max_fields * sizeof(type *));
		if( t == NULL )
			t = xmalloc(sizeof(type));
		t->bt = RECORD;
		n = 0;
		scope++; /* <-- trick: protect names of the fields agains other names */
		get_sym(2);
		while( sym != endsym ){

			/* get the name of the field: */
			if( sym != namesym && sym != idsym )
				error("expected field name");
			if( n >= max_fields ) ERROR;
			names[n] = xstrdup(syms);
			get_sym(0);

			/* is there a list of field names? ( ex: a,b,c: INTEGER ) */
			i = 0; /* = n. of "brother" fields ( ex: b,c ) */
			while( sym == commasym ){
				get_sym(0);
				if( sym != namesym )  error("expected field name");
				i++;
				if( n+1 >= max_fields ) FIXME;
				names[n+i] = xstrdup(syms);
				get_sym(0);
			}

			/* now the type: */
			if( sym != colonsym )  error("expected `:'");
			get_sym(1);
			types[n] = parse_subtype(NULL);

			for( j=1; j<=i; j++ )
				types[n+j] = types[n];

			n = n + i + 1;
		}
		scope--;
		get_sym(2);
		t->n_fields = n;
		t->field_names = names;
		t->field_types = types;
		return t;
	}


	type * parse_function_type(type *t)
	{
		function *f;

		if( t == NULL )
			t = xmalloc(sizeof(type));
		t->bt = FUNCTION;
		f = xmalloc(sizeof(function));
		f->raise_error = 0;
		t->func = f;
		f->nargs = 0;
		get_sym(0);
		if( sym != lroundsym )  error("expected `('");
		scope++;
		get_sym(0);
		while( 1 ){
			if( sym == rroundsym ){
				scope--;
				get_sym(2);
				break;
			}
			if( f->nargs > 0 ){
				if( sym != commasym )
					error("expected `,' or `)'");
				get_sym(0);
			}
			if( sym == varsym ){
				f->argmethod[f->nargs] = byaddress;
				get_sym(0);
			} else {
				f->argmethod[f->nargs] = byvalue;
			}
			if( sym != namesym )
				error("expected name of the argument");
			get_sym(0);
			if( sym != colonsym )
				error("expected `:'");
			get_sym(1);
			f->argtype[f->nargs] = parse_subtype(NULL);
			f->nargs++;
		}
		if( sym == colonsym ){
			get_sym(1);
			f->type = parse_subtype(NULL);
		} else {
			f->type = NULL;
		}
		return t;
	}


	switch( sym ){
		
		case voidsym:
			get_sym(2);
			return VOID_type;

		case booleansym:
			get_sym(2);
			return BOOLEAN_type;
		
		case integersym:
			get_sym(2);
			return INTEGER_type;

		case lroundsym:
			return parse_enum_type();

		case realsym:
			get_sym(2);
			return REAL_type;

		case stringsym:
			get_sym(2);
			return STRING_type;
		
		case arraysym:
			return parse_array_type(t);

		case recordsym:
			return parse_record_type(t);

		case functionsym:
			return parse_function_type(t);

		case forwardsym:
			get_sym(2);
			if( t == NULL )
				error_non_fatal("unnamed FORWARD type not allowed here");
			t->bt = FORWARD;
			return t;

		case idsym:
			if( symid->t != is_type )
				error("`%s' is not a type", symid->name);
			t = symid->u.t;
			get_sym(2);
			return t;

		default:
			error("invalid type declaration");
	}
	return NULL; /* make happy gcc -Wall */
}


static void parse_TYPE(void)
{
	identifier * id;

	get_sym(2);
	do {
		if( sym == namesym ){
			/*
				NEW TYPE
			*/
			add_id(syms, is_type);
			/* We need to store the id pointer to the type just created
			because, continuing parsing of the type, we may encounter
			enumeration types that add other IDs. */
			id = ids;
			get_sym(1);
			if( sym != eqsym )  error("expected `='");
			get_sym(1);
			if( sym == arraysym
			|| sym == recordsym
			|| sym == functionsym
			|| sym == forwardsym ){
				id->u.t = xmalloc(sizeof(type));
				parse_subtype(id->u.t);
			} else {
				id->u.t = parse_subtype(NULL);
			}
		} else if( sym == idsym ){
			/*
				FORWARD DECLARATION
			*/
			id = symid;
			if( id->t != is_type ){
				error("identifier `%s' already declared in line %d",
					id->name, id->line);
			} else if( id->u.t->bt != FORWARD ){
				error("type `%s' already declared in line %d",
					id->name, id->line);
			}
			get_sym(1);
			if( sym != eqsym )  error("expected `='");
			get_sym(1);
			switch( sym ){

			case voidsym:
			case booleansym:
			case integersym:
			case realsym:
			case stringsym:
			case functionsym:
				error("FORWARD declaration must be ARRAY or RECORD");

			case forwardsym:
				error("already declared FORWARD in line %d", id->line);

			case arraysym:
			case recordsym:
				break;

			default: error("invalid syntax - expected ARRAY or RECORD");
			}
			parse_subtype(id->u.t);
		} else {
			return;
		}
	} while(1);
}


static void add_var(char *name, type *type, method method)
{
	var *v;

	add_id(name, is_var);
	v = ids->u.v;
	v->type = type;
	v->method = method;
}


static void parse_VAR()
/*
	Parse the VAR section.
*/
{


	void out_var_decl(identifier * id, int is_static)
	/*
		Print declaration for the VAR
	*/
	{
		char *m;
		char *name;
		function *f;

		indent();
		if( is_static )  out("static ");
		m = id->m->name;
		name = id->name;
		switch( id->u.v->type->bt ){
			
			case VOID: /* FIXME: this should not be allowed! */
				out("void %s_%s;\n", m, name);
				break;
		
			case BOOLEAN:
			case INTEGER:
				out("int %s_%s = 0;\n", m, name);
				break;

			case REAL:
				out("double %s_%s = 0.0;\n", m, name);
				break;

			case STRING:
				out("STRING * %s_%s = NULL;\n", m, name);
				break;

			case ARRAY:
				out("ARRAY * %s_%s = NULL;\n", m, name);
				break;

			case RECORD:
				out("RECORD * %s_%s = NULL;\n", m, name);
				break;

			case FUNCTION:
				f = id->u.v->type->func;
				if( f->type == NULL ){
					out("void ");
				} else {
					switch( f->type->bt ){

					case VOID:
						out("void ");  break;

					case BOOLEAN:
					case INTEGER:
						out("int ");  break;

					case REAL:
						out("double ");  break;

					case STRING:
						out("STRING * ");  break;

					case ARRAY:
						out("ARRAY * ");  break;

					case RECORD:
						out("RECORD * ");  break;

					case FUNCTION:
						FIXME;

					case FORWARD:
						error("returned type is still FORWARD declared");

					default: ERROR;
					}
				}
				out("%s_%s = NULL;\n", m, name);
				break;

			case FORWARD:
				out("void * %s_%s = NULL;\n", m, name);
				break;

			default: ERROR;
		}
	}


	int count;
	type *t;
	identifier * id;
	int is_static = FALSE;

	/*
		Since parse_subtype() got the last symbol with get_sym(3),
		here we need to check the unicity of the first var. name.
	*/
	get_sym(2);
	if( sym == staticsym ){
		if( scope == 0 )
			error_non_fatal("STATIC attribute in global scope does not has effect");
		else
			is_static = TRUE;
		get_sym(2);
	}
	do {
		if( sym != namesym && sym != idsym )  return;
		check_name_in_curr_module_same_scope(syms);
		add_var(syms, VOID_type/*dummy*/, byvalue/*dummy*/);
		count = 1;
		get_sym(0);
		while( sym == commasym ){
			get_sym(0);
			if( sym != namesym ) error("expected variable name after `,'");
			add_var(syms, VOID_type/*dummy*/, byvalue/*dummy*/);
			count++;
			get_sym(0);
		}
		if( sym != colonsym ) error("expected `:'");
		get_sym(1);

		t = parse_subtype(NULL);

		/* Now that I know the their type, set the type of the "count"
		variables found and print them: */
		id = ids;
		do {
			if( id->t == is_var ){
				id->u.v->type = t;
				if(
					curr_module->u.m->type == mod
					/* we are compiling a MODULE: VAR declaration goes
					   to the C source */

					|| curr_module->u.m->type == impl
					/* we are compiling an IMPLEMENTATION: VAR declarations
					   goes to the C source */

					|| ( curr_module->u.m->type == def
						&& ori_module != NULL
						&& strcmp(ori_module->name, curr_module->name) == 0 )
					/* we are parsing a DEFINITION, and the original
					   module for which we are called for is just its
					   IMPLEMENTATION: VAR declaration goes to the C source
					   of the implementation */
				){
					out_var_decl(id, is_static);
				}

				count--;
			}
			id = id->next;
		} while( count > 0 );

	} while( 1 );
}


static char * size_of(type * t)
{
	switch( t->bt ){

		case NIL: return "sizeof(void *)";

		case VOID:  FIXME;

		case BOOLEAN:
		case INTEGER:  return "sizeof(int)";

		case REAL:     return "sizeof(double)";

		case STRING:
		case ARRAY:
		case RECORD:
		case FUNCTION:
		case FORWARD:  return "sizeof(void *)";

		default: ERROR;

	}
	return NULL; /* make happy gcc -Wall */
}


static char * size_of_RECORD(type *r)
{
	int i, n_int, n_ptr;
	static char *res = NULL;

	if( res == NULL )
		res = xmalloc(50);

	/* add fixed fields:

		struct{
			base_type type;
			int n_dyn_fields;
			...
		}
	*/
	n_int = 2;
	n_ptr = 0;

	for( i = 0; i < r->n_fields; i++ ){
		if( is_dynamic( r->field_types[i] ) )
			n_ptr++;
		else
			n_int++;
	}
	sprintf(res, "%d * sizeof(void*) + %d * sizeof(int)", n_ptr, n_int);
	return res;
}


static int field_index(type *r, char *field)
/*
	Return the index of the field of the given name.
*/
{
	int i;

	for( i = 0; i < r->n_fields; i++ ){
		if( strcmp(field, r->field_names[i]) == 0 )
			return i;
	}
	error("unknow field name `%s'", field);
	return 0; /* make happy gcc -Wall */
}


static char * field_offset(type *r, int i)
/*
	Return the offset in B of the field of index i.
	Dynamic fields (STRING, ARRAY, RECORD, FORWARD) MUST appear first,
	static fields (BOOLEAN, INTEGER, REAL) last.
*/
{
	int j, offset_int, offset_ptr;
	static char *res = NULL;

	if( res == NULL )
		res = xmalloc(50);

	/*
		struct RECORD {
			base_type type;
			int n_dyn_fields;
			...fields here...
		}
	*/
	offset_int = 2;
	offset_ptr = 0;

	if( is_dynamic(r->field_types[i]) ){
		j = 0;
		while( j < i ){
			if( is_dynamic(r->field_types[j]) )
				offset_ptr++;
			j++;
		}
	} else {
		for( j = 0; j < r->n_fields; j++ )
			if( is_dynamic(r->field_types[j]) )
				offset_ptr++;
		j = 0;
		while( j < i ){
			if( ! is_dynamic(r->field_types[j]) )
				offset_int++;
			j++;
		}
	}
	sprintf(res, "%d * sizeof(void*) + %d * sizeof(int)",
		offset_ptr, offset_int);
	return res;
}


static type * parse_function_call(OutBuffer *b, type *t, method m);

static void parse_expr(type *t);

/*
	Run-time error handling

	Every generated C source must declare a function
	MODULENAME_0err_entry_get() that maps module/function/line to a
	proper error message.
*/

char ** func = NULL;
int func_size = 0;
int func_n = 0;

int * err_entry = NULL;
int err_entry_size = 0;
int err_entry_n = 0;


int err_entry_new(char *f, int l)
{
	int i, j;

	i = err_entry_n;
	if( i >= err_entry_size ){
		err_entry_size = err_entry_size * 2 + 10;
		err_entry = xrealloc(err_entry, err_entry_size * 2 * sizeof(int));
	}
	err_entry_n++;
	err_entry[2*i + 1] = l;
	j = func_n;
	if( j > 0 && strcmp(f, func[j-1]) == 0 ){
		j = j - 1;
	} else {
		if( j >= func_size ){
			func_size = func_size * 2 + 10;
			func = xrealloc(func, func_size * sizeof(char *));
		}
		func_n++;
		func[j] = xstrdup(f);
	}
	err_entry[2*i] = j;
	return i;
}


void out_err_entry_get(char *f, int l)
{
	out("%s_0err_entry_get, %d", curr_module->name, err_entry_new(f, l));
}


void out_err_entry_get_prototype()
{
	out("\nvoid %s_0err_entry_get(int i, char **m, char **f, int *l);\n",
		curr_module->name);
}


void out_err_entry_get_function()
{
	char *m;
	int i;

	m = curr_module->name;

	if( err_entry_n == 0 ){
		out("\nvoid %s_0err_entry_get(int i, char **m, char **f, int *l)\n",
			m);
		out("{}\n");
		return;
	}

	out("\nchar * %s_0func[] = {\n", curr_module->name);
	for(i = 0; i < func_n; i++){
		out("    \"%s\"", func[i]);
		if( i < func_n - 1 )
			out(",\n");
		else
			out("\n");
	}
	out("};\n\n");

	out("int %s_0err_entry[] = {\n", curr_module->name);
	for(i = 0; i < err_entry_n; i++){
		out("    %d /* %s */, %d", err_entry[2*i], func[ err_entry[2*i] ],
			err_entry[2*i + 1]);
		if( i < err_entry_n - 1 )
			out(",\n");
		else
			out("\n");
	}
	out("};\n\n");
	
	out("void %s_0err_entry_get(int i, char **m, char **f, int *l)\n", m);
	out("{\n");
	out("    *m = \"%s\";\n", m);
	out("    *f = %s_0func[ %s_0err_entry[2*i] ];\n", m, m);
	out("    *l = %s_0err_entry[2*i + 1];\n", m);
	out("}\n");
}


identifier * curr_function = NULL;
/* The function we are parsing. RETURN and RAISE ERROR require to know this. */


type * dereference_rhs_sub(OutBuffer *b, type *t)
/*
	In b we have a RHS value of type t.  Resolve recursively the
	dereferencing operators "[" and "->" if any is present.
*/
{
	OutBuffer *b2;
	int i, l;
	type *t2;

	/* Procedure call? */
	if( t == NULL ){
		out_out_buffer(b);
		return NULL;
	}

	/* Postfix notation x->f()? */
	if( sym == arrowsym ){
		get_sym(4);
		b2 = open_out_buffer();
		t2 = parse_function_call(b, t, byvalue);
		close_out_buffer(b2);
		t = dereference_rhs_sub(b2, t2);
		free_out_buffer(b2);
		return t;
	}

	/* No more dereferencing operators? */
	if( sym != lsquaresym ){
		out_out_buffer(b);
		return t;
	}

	/* `[' operator: */
	switch( t->bt ){
		
	case VOID:
		FIXME;

	case BOOLEAN:
	case INTEGER:
	case REAL:
		error("dereferencing operator `[' not applicable to this type");
		break;

	case STRING:
		l = line;
		b2 = open_out_buffer();
		out("m2runtime_substr(");
		out_out_buffer(b);
		out(", ");
		get_sym(1);
		parse_expr(INTEGER_type);
		out(", ");
		switch( sym ){
			
			case rsquaresym:  /* s[i] */
				get_sym(1);
				out("0, 0, ");
				break;

			case commasym:    /* s[i,j] */
				get_sym(1);
				parse_expr(INTEGER_type);
				out(", 1, ");
				if( sym != rsquaresym )  error("expected `]'");
				get_sym(1);
				break;

			default: error("bad syntax in substring selector");
		}
		out_err_entry_get(curr_function->name, l);
		out(")");
		close_out_buffer(b2);
		b = b2;
		break;

	case ARRAY:
		get_sym(1);
		b2 = open_out_buffer();
		out_cast_byvalue(t->elem_type->bt);
		out("m2runtime_dereference_rhs_ARRAY(");
		out_out_buffer(b);
		out(", ");
		parse_expr(INTEGER_type);
		if( sym != rsquaresym ) error("expected `]'");
		get_sym(1);
		out(", ");
		out_err_entry_get(curr_function->name, sym_start_line);
		out(")");
		close_out_buffer(b2);
		b = b2;
		t = t->elem_type;
		break;
	
	case RECORD:
		get_sym(3);
		if( sym != namesym ) error("expected field name");
		i = field_index(t, syms);
		b2 = open_out_buffer();
		out_cast_byvalue(t->field_types[i]->bt);
		out("m2runtime_dereference_rhs_RECORD(");
		out_out_buffer(b);
		out(", %s, ", field_offset(t, i));
		out_err_entry_get(curr_function->name, line);
		out(")");
		close_out_buffer(b2);
		get_sym(0);
		if( sym != rsquaresym ) error("expected `]'");
		get_sym(1);
		b = b2;
		t = t->field_types[i];
		break;
	
	case FUNCTION:
		FIXME;

	case FORWARD:
		error("dereferencing operator `[' not applicable to "
			"FORWARD type");
		break;

	default: ERROR;

	}

	t = dereference_rhs_sub(b, t);
	free_out_buffer(b);
	return t;
}


static type * dereference_value()
/*
	When sym==idsym, dereference RHS value: v, v->*, v[*, f(), f()->* f()[*.
	Return the type of the dereferenced value, method "byvalue".
	If the final type is NULL, that's an error.
*/
{


	type * dereference_var_value()
	{
		identifier *v;
		type *t;
		OutBuffer *b;

		v = symid;
		t = v->u.v->type;

		get_sym(1);
		if( sym == lsquaresym || sym == arrowsym ){
			b = open_out_buffer();
			out_var_byvalue(v);
			close_out_buffer(b);
			t = dereference_rhs_sub(b, t);
			free_out_buffer(b);
		} else {
			out_var_byvalue(v);
		}

		return t;
	}


	type * dereference_func_value()
	{
		function *f;
		type *t;
		OutBuffer *b;

		f = symid->u.f;
		/* FIXME: check the return method of the func. */
		b = open_out_buffer();
		t = parse_function_call(NULL, NULL, 0);
		close_out_buffer(b);
		if( sym == lsquaresym || sym == arrowsym ){
			t = dereference_rhs_sub(b, t);
		} else {
			out_out_buffer(b);
		}
		free_out_buffer(b);

		return t;
	}


	type *t;

	if( sym != idsym )  ERROR;

	if( symid->t == is_var )
		t = dereference_var_value();
	else if( symid->t == is_func )
		t = dereference_func_value();
	else
		error("invalid identifier type `%s'", symid->name);

	if( t == NULL )
		error("call to a procedure in expression");

	return t;
}


static type * parse_function_call_x(OutBuffer *b, type *t);


type * parse_send(OutBuffer *b, type *t)
/*
	UNDOCUMENTED FEATURE OF THE "send" operator "-->"

	Dereference a chain of "send" operators:

	--> x --> y

	Return the type of the resulting value, or NULL if the last term
	is a procedure.
*/
{
	OutBuffer *b2;
	type *t2;

	if( sym != sendsym )  ERROR;

	get_sym(1);
	if( sym != idsym )
		error("expected variabile or function after `-->'%d", sym);
	if( symid->t == is_func ){
		b2 = open_out_buffer();
		t = parse_function_call_x(b, t);
		close_out_buffer(b2);
	} else if( symid->t == is_var ){
		FIXME;
	} else {
		error("expected variable or function after `-->'");
	}

	if( sym == sendsym ){
		if( t2 == NULL )
			error("can't apply `-->' operator - previuos expression does not "
				"return a value");
		t = parse_send(b2, t2);
	} else {
		out_out_buffer(b2);
	}

	
	free_out_buffer(b);
	return t;
}


type * parse_lhs(int assignment)
/*
	assignment==0: we are parsing an actual arg to be passed by address
	assignment==1: we are parsing the LHS of an assignment or prefix
		function call or postfix function call

	Depending on the nature of the var/function, we need to insert the
	operators '*' or '&':
		i = var. of type simple local or formal arg passed by value
			(VOID(?), BOOLEAN, INTEGER, REAL)
		ip = var. of simple type local or formal arg passed by address
		s = var. of type dynamic local or formal arg passed by value
			(STRING, ARRAY, RECORD, FORWARD, FUNCTION)
		sp = var of type dynamic local or passed by address
		f() = a function returning the address of a simple type or the
			address of the address of a dynamic type

	In assignment:
		i=...  s=...  *ip=...  *sp=...  *f()=...

	In actual argument to be passed by address:
		&i    &s       ip       sp       f()
*/
{

	type * dereference_lhs_sub(type *t, OutBuffer *b)
	/*
		In 'b' we have the current variable or partially dereference
		var. of type 't'. Here we continue dereferencing.
	*/
	{
		int i;
		OutBuffer *b2;
		type *t2;

		switch( t->bt ){
			
			case VOID:
				FIXME;

			case BOOLEAN:
			case INTEGER:
				if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else if( sym == lsquaresym ){
					error("`[' operator not applicable to INTEGER");
				} else {
					if( assignment )
						out("*");
					out_out_buffer(b);
					return t;
				}
				break;

			case REAL:
				if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else if( sym == lsquaresym ){
					error("`[' operator not applicable to REAL");
				} else {
					if( assignment )
						out("*");
					out_out_buffer(b);
					return t;
				}
				break;

			case STRING:
				if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else if( sym == lsquaresym ){
					FIXME; /* need to check if a "->" follow: s[i]->f() */
				} else {
					if( assignment )
						out("*");
					out_out_buffer(b);
					return t;
				}
				break;

			case ARRAY:
				if( sym == lsquaresym ){
					t2 = t->elem_type;
					b2 = open_out_buffer();
					out_cast_byaddress(t2->bt);

					get_sym(1);
					if( sym == rsquaresym ){
						/* using array[] */
						out("m2runtime_dereference_lhs_ARRAY_next(");
						out_out_buffer(b);
						out(", %s, %d",
							size_of(t2),
							is_dynamic(t2)
						);
					} else {
						/* using array[expr] */
						out("m2runtime_dereference_lhs_ARRAY(");
						out_out_buffer(b);
						out(", %s, %d, ",
							size_of(t2),
							is_dynamic(t2)
						);
						parse_expr(INTEGER_type);
						if( sym != rsquaresym ) error("expected `]'");
					}
					get_sym(1);

					out(", ");
					out_err_entry_get(curr_function->name, line);
					out(")");
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else {
					if( assignment )
						out("*");
					out_out_buffer(b);
					return t;
				}
				break;
			
			case RECORD:
				if( sym == lsquaresym ){
					get_sym(3);
					if( sym != namesym ) error("expected field name");
					i = field_index(t, syms);
					get_sym(1);
					if( sym != rsquaresym ) error("expected `]'");
					get_sym(1);
					t2 = t->field_types[i];
					b2 = open_out_buffer();
					out_cast_byaddress(t2->bt);
					out("m2runtime_dereference_lhs_RECORD(");
					out_out_buffer(b);
					out(", %s, %d, %s, ",
						size_of_RECORD(t),
						n_dyn_fields(t),
						field_offset(t, i)
					);
					out_err_entry_get(curr_function->name, line);
					out(")");
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else {
					if( assignment )
						out("*");
					out_out_buffer(b);
					return t;
				}
				break;

			case FUNCTION:
				if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else {
					out("*((void **)");
					out_out_buffer(b);
					out(")");
					return t;
				}
				break;

			case FORWARD:
				if( sym == arrowsym ){
					get_sym(4);
					b2 = open_out_buffer();
					t2 = parse_function_call(b, t, byaddress);
					close_out_buffer(b2);
					b = b2;
					t = t2;
				} else {
					out("*((void **)");
					out_out_buffer(b);
					out(")");
					return t;
				}
				break;

			default: ERROR;
		}


		if( sym == lsquaresym || sym == arrowsym ){
			t = dereference_lhs_sub(t, b);
		} else {
			if( assignment && t != NULL )
				out("*");
			out_out_buffer(b);
		}
		free_out_buffer(b);
		return t;
	}


	type * parse_lhs_var(identifier *v)
	{
		type *t;
		OutBuffer *b;

		v = symid;
		t = v->u.v->type;

		get_sym(1);

		if( ! assignment && sym == sendsym )
			error("operator `-->' used in LHS");

		if( sym == sendsym ){
			b = open_out_buffer();
			out_var_byvalue(v);
			close_out_buffer(b);
			parse_send(b, t);
			free_out_buffer(b);
			if( t != NULL )
				error_non_fatal("ignored value from `-->' chain");
			free_out_buffer(b);
			return NULL;
		}

		switch( t->bt ){

			case NIL:  ERROR;

			case VOID:
			case BOOLEAN:
			case INTEGER:
			case REAL:
			case STRING:
				if( sym == arrowsym ){
					b = open_out_buffer();
					out_var_byaddress(v);
					close_out_buffer(b);
					t = dereference_lhs_sub(t, b);
					free_out_buffer(b);
					return t;
				} else {
					if( assignment ){
						out_var_byvalue(v);
					} else {
						out_var_byaddress(v);
					}
					return t;
				}

			case ARRAY:
			case RECORD:
				if( sym == arrowsym ){
					b = open_out_buffer();
					out_var_byaddress(v);
					close_out_buffer(b);
					t = dereference_lhs_sub(t, b);
					free_out_buffer(b);
					return t;
				} else if( sym == lsquaresym ){
					/***
					if( assignment )
						out("*");
					***/
					b = open_out_buffer();
					out_var_byaddress(v);
					close_out_buffer(b);
					t = dereference_lhs_sub(t, b);
					free_out_buffer(b);
					return t;
				} else {
					if( assignment ){
						out_var_byvalue(v);
					} else {
						out_var_byaddress(v);
					}
					return t;
				}

			case FUNCTION:
				if( sym == arrowsym ){
					b = open_out_buffer();
					out_var_byaddress(v);
					close_out_buffer(b);
					t = dereference_lhs_sub(t, b);
					free_out_buffer(b);
					return t;
				} else {
					if( assignment ){
						out_var_byvalue(v);
					} else {
						out_var_byaddress(v);
					}
					return t;
				}
				break;

			case FORWARD:
				if( sym == arrowsym ){
					b = open_out_buffer();
					out_var_byaddress(v);
					close_out_buffer(b);
					t = dereference_lhs_sub(t, b);
					free_out_buffer(b);
					return t;
				} else {
					if( assignment ){
						out_var_byvalue(v);
					} else {
						out_var_byaddress(v);
					}
					return t;
				}
				break;
		}
		return NULL; /* make happy gcc -Wall */
	}


	type * parse_lhs_func(identifier *f)
	{
		OutBuffer *b;
		type *t;
		method m;

		b = open_out_buffer();
		t = parse_function_call(NULL, NULL, 0);
		close_out_buffer(b);
		m = f->u.f->return_method;
		if( t == NULL ){
			/* that's a procedure call - stop here. */
			out_out_buffer(b);
			free_out_buffer(b);
			return NULL;
		}
		if( sym == lsquaresym ){
			if( m == byvalue ){
				FIXME;
			} else {
				t = dereference_lhs_sub(t, b);
			}
		} else if( sym == arrowsym ){
			if( m == byvalue ){
				t = dereference_rhs_sub(b, t);
			} else {
				t = dereference_lhs_sub(t, b);
			}
		} else if( sym == sendsym ){
			if( m == byvalue ){
				t = parse_send(b, t);
			} else {
				error("`-->' operator in LHS");
			}
		} else {
			error_non_fatal("the function return a value "
				"- it cannot be ignored");
		}

		return NULL; /* make happy gcc -Wall */
	}


	if( sym != idsym )
		error("invalid syntax - expected LHS");

	if( symid->t == is_var )
		return parse_lhs_var(symid);
	else if( symid->t == is_func )
		return parse_lhs_func(symid);
	else
		ERROR;

	return NULL; /* make happy gcc -Wall */
}


int try_section = 0;
/* Entering a TRY...END section we set this flag. */


static void parse_expr(type * t);


static identifier * resolve_method_name(char *method_name, type *t)
/*
	Search the method given its unqualified name `method_name'
	whose first argument is of type `t'.
*/
{
	identifier *id;
	char *m;

	/*
		Search module name where type t is defined:
	*/
	id = ids;
	m = NULL;
	while( id != NULL ){
		if( id->t == is_type && id->u.t == t ){
			m = id->m->name;
			break;
		}
		id = id->next;
	}

	if( m == NULL ){
		/*
			Module name not found. Apply normal strategy for
			unqualified names.
		*/
		return search_name(method_name);
	} else {
		/*
			Module name found. Search in module m the function method_name:
		*/
		id = ids;
		while( id != NULL ){
			if( id->t == is_func
			&& strcmp(id->name, method_name) == 0
			&& strcmp(id->m->name, m) == 0 )
				return id;
			id = id->next;
		}

		/*
			Method not found in module m. Apply normal strategy for
			unqualified names.
		*/
		return search_name(method_name);
	}
}


static type * parse_function_call(OutBuffer *b, type *t, method m)
/*
	Parse function call.
	b == NULL: prefix calling form f(x, y, z); t and m are ignored.
		sym==idsym and symid is the ID.
	b != NULL: postfix calling form x->f(y, z); b contain x of
		type t passed by method m. sym==namesym if not qualified,
		or sym==idsym if qualified.

	Return the type returned by function, or NULL if procedure.
*/
{

	identifier *f_id;
	function *f;
	int i;
	type *at;

	/*
		Resolve function name:
	*/
	if( b == NULL ){
		if( sym != idsym )
			ERROR;
		f_id = symid;
	} else {
		if( sym == namesym ){
			f_id = resolve_method_name(syms, t);
			f_id->used = 1;
		} else if( sym == idsym ){
			f_id = symid;
		} else {
			error("expected function name after `->'");
		}
	}

	/*
		Print function name:
	*/
	f = f_id->u.f;
	out("%s_%s", f_id->m->name, f_id->name);
	get_sym(1);

	if( sym == lroundsym ){
		/* Function call. Parse args: */
		get_sym(1);
		out("(");
	} else {
		/* Function address. Stop Here. */
		return f->type;
	}

	if( f->raise_error ){
		if( ! try_section )
			warning("the function `%s' may RAISE ERROR - use TRY...END",
				f_id->name);
		out("%d", try_section);
		if( f->nargs > 0 )
			out(", ");
	}

	/*
		Parse actual arguments:
	*/
	for( i = 0; i < f->nargs; i++ ){

		/* Parse ',': */
		if( ( b == NULL && i >= 1 ) || ( b != NULL && i >= 2 ) ){
			if( sym == commasym ){
				get_sym(1);
			} else if( sym == rroundsym ){
				error_non_fatal("missing arguments in function `%s'",
					f_id->name);
				break;
			} else {
				error("bad syntax in arguments list");
			}
		}
		if( i > 0 )
			out(", ");

		if( i == 0 && b != NULL ){
			/* first arg in postfix form x->f() - use b: */
			if( ! same_type(t, f->argtype[i]) ){
				error_type_mismatch("in postfix function notation: "
					"type of the prefix argument does not match that of "
					"the function argument", t, f->argtype[i]);
			}
			if( f->argmethod[0] == byvalue ){
				if( m == byvalue ){
					out_out_buffer(b);
				} else {
					out("*(void **)");
					out_out_buffer(b);
				}
			} else {
				if( m == byvalue ){
					error_non_fatal("in postfix function notation: "
						"the function require a VAR value");
				} else {
					/*out("(void *)"); */
					out_out_buffer(b);
				}
			}
		} else {
			if( f->argmethod[i] == byvalue ){
				parse_expr(f->argtype[i]);
			} else {
				at = parse_lhs(0);
				if( ! same_type(at, f->argtype[i]) ){
					error_type_mismatch("actual argument type differ from the "
						"formal type", at, f->argtype[i]);
				}
			}
		}
	}

	if( sym != rroundsym ) error("expected `)'");
	get_sym(1);
	out(")");

	return f->type;
}


static type * parse_function_call_x(OutBuffer *b, type *t)
/*
	Parse function call in postfix notation using the "send" op.:

		b-->f(a, ?, c)

	The special symbol `?' mark the position where the prefix
	argument `b' of type `t' must go.
*/
{
	identifier *f_id;
	function *f;
	int i;
	type *at;
	int question_found = 0;

	/*
		Resolve function name:
	*/
	if( b == NULL ){
		if( sym != idsym )
			ERROR;
		f_id = symid;
	} else {
		if( sym == namesym ){
			f_id = resolve_method_name(syms, t);
		} else if( sym == idsym ){
			f_id = symid;
		} else {
			error("expected function name after `->'");
		}
	}

	/*
		Print function name:
	*/
	f = f_id->u.f;
	out("%s_%s", f_id->m->name, f_id->name);
	get_sym(1);

	if( sym != lroundsym )
		error("expected `(' after function name");
	get_sym(1);
	out("(");

	if( f->raise_error ){
		if( ! try_section )
			warning("the function `%s' may RAISE ERROR", f_id->name);
		out("%d", try_section);
		if( f->nargs > 0 )
			out(", ");
	}

	/*
		Parse actual arguments:
	*/
	for( i = 0; i < f->nargs; i++ ){

		/* Parse ',': */
		if( i > 0 ){
			if( sym == commasym ){
				get_sym(1);
			} else if( sym == rroundsym ){
				error_non_fatal("missing arguments in function call");
				question_found = 1;
				break;
			} else {
				error("expected `,' in argument list");
			}
			out(", ");
		}

		if( sym == questionsym ){
			if( question_found )
				error_non_fatal("two or more `?' in argument list");
			question_found = 1;
			get_sym(1);

			if( ! same_type(t, f->argtype[i]) ){
				error_type_mismatch("in postfix function notation: "
					"type of the prefix argument does not match that of "
					"the function argument", t, f->argtype[i]);
			}
			if( f->argmethod[0] == byaddress )
				error("function require an argument by address");
			out_out_buffer(b);
		} else {
			if( f->argmethod[i] == byvalue ){
				parse_expr(f->argtype[i]);
			} else {
				at = parse_lhs(0);
				if( ! same_type(at, f->argtype[i]) ){
					error_type_mismatch("invalid type of the actual argument",
						at, f->argtype[i]);
				}
			}
		}
	}

	if( sym != rroundsym )
		error("expected `)'");

	if( ! question_found )
		error_non_fatal("missing `?' in argument list");

	get_sym(1);
	out(")");

	return f->type;
}


static void parse_expr_array(type *t)
/*
	Parse ARRAY builder {elem0, elem1, ...}
*/
{
	int i;
	char *cast_;
	int is_addr;

	get_sym(1);
	out("(\n");
	nest++;
	indent();
	out("push((char*) alloc_ARRAY(%s, %d)),\n",
		size_of(t->elem_type), is_dynamic(t->elem_type));
	switch( t->elem_type->bt ){
		case NIL: ERROR;
		case VOID:    cast_ = "void*";  is_addr = TRUE; break;
		case BOOLEAN: cast_ = "int";  is_addr = FALSE; break;
		case INTEGER: cast_ = "int";  is_addr = FALSE; break;
		case REAL:    cast_ = "double";  is_addr = FALSE; break;
		case STRING:  cast_ = "STRING*";  is_addr = TRUE; break;
		case ARRAY:   cast_ = "ARRAY*";  is_addr = TRUE; break;
		case RECORD:  cast_ = "RECORD*";  is_addr = TRUE; break;
		case FUNCTION: cast_ = "void*";  is_addr = TRUE; break; /* FIXME: can't deref */
		case FORWARD: cast_ = "void**";  is_addr = TRUE; break; /* FIXME: can't deref */
		default: ERROR;
	}
	i = 0;
	if( sym != rbracesym ){
		do {

			if( is_addr ){
				/*
				 	Here we have to allocate space for the element no. i
					(LHS) and the assign its value (RHS).

					The order of evaluation of the LHS and RHS of the
					assignment LHS=RHS isn't defined, so we can't access
					to the evaluation stack both in the LHS and RHS in a
					predetermined order, so we have to calculate LHS and
					RHS separately.
				*/
				indent();
				out("push((char*) ");
				parse_expr(t->elem_type);
				out("),\n");

				indent();
				out("*(%s*) element_ARRAY((ARRAY*) tosn(1),%d) = (%s) tos(), pop(),\n",
					cast_, i, cast_);
			} else {
				indent();
				out("*(%s*) element_ARRAY((ARRAY *)tos(),%d) = ", cast_, i);
				parse_expr(t->elem_type);
				out(",\n");
			}

			if( sym == commasym )
				get_sym(1);
			else if( sym == rbracesym )
				break;
			else
				error("expected `}'");
			i++;
		} while( 1 );
	}
	get_sym(1);
	indent();
	out("(ARRAY*) pop()\n");
	nest--;
	indent();
	out(")");
}


static void parse_expr_record(type *t)
/*
	Parse RECORD builder {field0, field1, ...}
*/
{
	int i;
	char *cast_;
	int is_addr;

	out("(\n");
	nest++;
	indent();
	out("push((char*) alloc_RECORD(%s, %d)),\n",
		size_of_RECORD(t), n_dyn_fields(t));
	for( i=0; i < t->n_fields; i++ ){
		if( i > 0 && sym != commasym )  error("expected `,'");
		get_sym(1);
		switch( t->field_types[i]->bt ){
			case NIL: ERROR;
			case VOID:    cast_ = "void*";  is_addr = TRUE; break;
			case BOOLEAN: cast_ = "int";  is_addr = FALSE; break;
			case INTEGER: cast_ = "int";  is_addr = FALSE; break;
			case REAL:    cast_ = "double";  is_addr = FALSE; break;
			case STRING:  cast_ = "STRING*";  is_addr = TRUE; break;
			case ARRAY:   cast_ = "ARRAY*";  is_addr = TRUE; break;
			case RECORD:  cast_ = "RECORD*";  is_addr = TRUE; break;
			case FUNCTION: cast_ = "void*";  is_addr = TRUE; break; /* FIXME: can't deref */
			case FORWARD: cast_ = "void**";  is_addr = TRUE; break; /* FIXME: can't deref */
			default: ERROR;
		}
		if( is_addr ){
			/*
				Here we have to assign the field no. i.

				The order of evaluation of the LHS and RHS of the
				assignment LHS=RHS isn't defined, so we can't access
				to the evaluation stack both in the LHS and RHS in a
				predetermined order, so we have to calculate LHS and
				RHS separately.
			*/
			indent();
			out("push((char*) ");
			parse_expr(t->field_types[i]);
			out("),\n");
			indent();
			out("*(%s*) (tosn(1)+%s) = (%s) tos(), pop(),\n",
				cast_, field_offset(t,i), cast_);
		} else {
			indent();
			out("*(%s*) (tos()+%s) = ", cast_, field_offset(t,i));
			parse_expr(t->field_types[i]);
			out(",\n");
		}

	}
	if( sym != rbracesym )
		error("expected `}'");
	get_sym(1);
	indent(); out("(RECORD*) pop()\n");
	nest--;
	indent();
	out(")");
}


static void parse_expr_function(type * t)
{
	type *rt;

	if( sym == nilsym ){
		get_sym(1);
		out("NULL");
	} else if( sym == idsym ){
		if( symid->t == is_func ){
			/* FIXME: check the function signature */
			out("%s_%s", symid->m->name, symid->name);
			get_sym(1);
		} else {
			rt = dereference_value();
			if( ! same_type(t, rt) )
				error_non_fatal("type mismatch");
		}
	} else {
		error("invalid expression of type FUNCTION");
	}
}


/*
	EXPRESSION PARSING

	The functions of the family parse_expr*_x() parse an expression of
	unknown type and return the result. Note that we cannot resolve
	the ARRAY and RECORD build operator "{...}" because their type
	is not known.
*/

static void parse_expr_x(type **tp, OutBuffer **bp);


static void parse_expr_factor_x(type **tp, OutBuffer **bp)
{
	type *t;
	OutBuffer *b, *b2;

	if( sym == lroundsym ){
		b2 = open_out_buffer();
		out("(");
		get_sym(1);
		parse_expr_x(tp, &b);
		out_out_buffer(b);
		free_out_buffer(b);
		if( sym != rroundsym )
			error_non_fatal("missing `)'");
		else
			get_sym(1);
		out(")");
		close_out_buffer(b2);
		*bp = b2;

	} else if( sym == integerlitsym ){
		b = open_out_buffer();
		if( symi >= 0 )
			out("%d", symi);
		else
			out("(%d)", symi);
		get_sym(1);
		close_out_buffer(b);
		*tp = INTEGER_type;
		*bp = b;

	} else if( sym == reallitsym ){
		b = open_out_buffer();
		if( syms[0] == '-' )
			out("(%s)", syms);
		else
			out("%s", syms);
		get_sym(1);
		close_out_buffer(b);
		*tp = REAL_type;
		*bp = b;

	} else if( sym == notsym ){
		get_sym(1);
		b2 = open_out_buffer();
		out("!");
		parse_expr_factor_x(&t, &b);
		if( t != BOOLEAN_type )
			error_non_fatal("non-BOOLEAN type for the NOT operator");
		out_out_buffer(b);
		free_out_buffer(b);
		close_out_buffer(b2);
		*tp = BOOLEAN_type;
		*bp = b2;

	} else if( sym == tildesym ){
		get_sym(1);
		b2 = open_out_buffer();
		out("~");
		parse_expr_factor_x(&t, &b);
		if( t != INTEGER_type )
			error_non_fatal("non-INTEGER type for the `~' operator");
		out_out_buffer(b);
		free_out_buffer(b);
		close_out_buffer(b2);
		*tp = INTEGER_type;
		*bp = b2;

	} else if( sym == falsesym ){
		get_sym(1);
		b2 = open_out_buffer();
		out("FALSE");
		close_out_buffer(b2);
		*tp = BOOLEAN_type;
		*bp = b2;

	} else if( sym == truesym ){
		get_sym(1);
		b2 = open_out_buffer();
		out("TRUE");
		close_out_buffer(b2);
		*tp = BOOLEAN_type;
		*bp = b2;

	} else if( sym == stringlitsym ){
		b = open_out_buffer();
		out_lit_string(syms, syms_len-1);
		close_out_buffer(b);
		get_sym(1);
		*tp = STRING_type;
		*bp = b;

	} else if( sym == nilsym ){
		b = open_out_buffer();
		out("NULL");
		close_out_buffer(b);
		get_sym(1);
		*tp = NIL_type;
		*bp = b;

	} else if( sym == idsym ){
		*bp = open_out_buffer();
		*tp = dereference_value();
		close_out_buffer(*bp);
		if( *tp == NULL )
			error("function does not return a value to the expression");

	} else {
		error("invalid expression");
	}
}


static void parse_expr_term_x(type **tp, OutBuffer **bp)
{

	char * int_mult_op(symtype sym)
	{
		switch( sym ){
			case timessym: return "*";
			case divsym: return "/";
			case modsym: return "%";
			case lshiftsym: return "<<";
			case rshiftsym: return ">>";
			case ampsym: return "&";
			default: return NULL;
		}
	}


	char * real_mult_op(symtype sym)
	{
		switch( sym ){
			case timessym: return "*";
			case slashsym: return "/";
			default: return NULL;
		}
	}


	type *t;
	OutBuffer *b, *b2;
	char *op;

	parse_expr_factor_x(&t, &b);

	if( ! ( sym == andsym || sym == timessym || sym == divsym
		|| sym == slashsym || sym == modsym || sym == lshiftsym
		|| sym == rshiftsym || sym == ampsym
	) ){
		*tp = t;
		*bp = b;
		return;
	}

	*tp = t;

	if( t == BOOLEAN_type ){
		if( sym != andsym ){
			error_non_fatal("invalid operator for BOOLEAN factor");
		}
		b2 = open_out_buffer();
		out("(");
		out_out_buffer(b);
		free_out_buffer(b);
		do {
			out(" && ");
			get_sym(1);
			parse_expr_factor_x(&t, &b);
			if( t != BOOLEAN_type )
				error_non_fatal("invalid type in expression");
			out_out_buffer(b);
			free_out_buffer(b);
		} while( sym == andsym );
		out(")");
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else if( t == INTEGER_type ){
		op = int_mult_op(sym);
		if( op == NULL ){
			error("required parentheses: (...) AND ...");
			op = "(invalid operator)";
		}
		do {
			b2 = open_out_buffer();
			out("(");
			if( sym == lshiftsym || sym == rshiftsym )
				out("(unsigned int) ");
			out_out_buffer(b);
			free_out_buffer(b);
			out(" %s ", op);
			get_sym(1);
			parse_expr_factor_x(&t, &b);
			if( t != INTEGER_type )
				error_non_fatal("right hand side of `%s' is not INTEGER", op);
			out_out_buffer(b);
			free_out_buffer(b);
			out(")");
			close_out_buffer(b2);
			b = b2;
			op = int_mult_op(sym);
		} while( op != NULL );
		*bp = b;
		return;

	} else if( t == REAL_type ){
		op = real_mult_op(sym);
		if( op == NULL )
			error("required parentheses: (...) AND ...");
		b2 = open_out_buffer();
		out_out_buffer(b);
		free_out_buffer(b);
		do {
			out(" %s ", op);
			get_sym(1);
			parse_expr_factor_x(&t, &b);
			if( t != REAL_type )
				error_non_fatal("right hand side of `%s' is not REAL", op);
			out_out_buffer(b);
			free_out_buffer(b);
			op = real_mult_op(sym);
		} while( op != NULL );
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else {
		error("invalid expression");

	}
}


static void parse_expr_simple_x(type **tp, OutBuffer **bp)
{

	char * int_add_op(symtype sym)
	{
		switch( sym ){
			case plussym:  return "+";
			case minussym: return "-";
			case xorsym:   return "^";
			case vbarsym:  return "|";
			default: return NULL;
		}
	}


	char * real_add_op(symtype sym)
	{
		switch( sym ){
			case plussym:  return "+";
			case minussym: return "-";
			default: return NULL;
		}
	}


	type *t;
	OutBuffer *b, *b2;
	int sign;
	char *op;

	sign = 0;
	if( sym == plussym ){
		sign = +1;
		get_sym(1);
	} else if( sym == minussym ){
		sign = -1;
		get_sym(1);
	}

	parse_expr_term_x(&t, &b);
	*tp = t;

	if( sign != 0 && t != INTEGER_type && t != REAL_type )
			error_non_fatal("invalid use of the unary +|- on non-INTEGER, "
			"non-REAL term");

	if( ! ( sym == orsym || sym == plussym || sym == minussym
		|| sym == xorsym || sym == vbarsym ) ){
		if( sign >= 0 ){
			*bp = b;
		} else {
			b2 = open_out_buffer();
			out("-");
			out_out_buffer(b);
			free_out_buffer(b);
			close_out_buffer(b2);
			*bp = b2;
		}
		return;
	}

	if( t == BOOLEAN_type ){
		if( sym != orsym )
			error_non_fatal("invalid operator in BOOLEAN expression");
		b2 = open_out_buffer();
		out("(");
		out_out_buffer(b);
		free_out_buffer(b);
		do {
			get_sym(1);
			out(" || ");
			parse_expr_term_x(&t, &b);
			if( t != BOOLEAN_type )
				error_non_fatal("right side of OR is not BOOLEAN");
			out_out_buffer(b);
			free_out_buffer(b);
		} while( sym == orsym );
		out(")");
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else if( t == INTEGER_type ){
		op = int_add_op(sym);
		if( op == NULL )
			error("required parentheses: (...) OR ...");
		b2 = open_out_buffer();
		out("(");
		if( sign < 0 )
			out("-");
		do {
			out_out_buffer(b);
			free_out_buffer(b);
			out(" %s ", op);
			get_sym(1);
			parse_expr_term_x(&t, &b);
			if( t != INTEGER_type )
				error_non_fatal("right side of %s is not INTEGER", op);
			out_out_buffer(b);
			free_out_buffer(b);
			out(")");
			close_out_buffer(b2);

			op = int_add_op(sym);
			if( op == NULL )
				break;

			b = b2;
			b2 = open_out_buffer();
			out("(");
		} while( 1 );
		*bp = b2;

	} else if( t == REAL_type ){
		op = real_add_op(sym);
		if( op == NULL )
			error("invalid REAL operator");
		b2 = open_out_buffer();
		if( sign < 0 )
			out("-");
		out_out_buffer(b);
		free_out_buffer(b);
		do {
			out(" %s ", op);
			get_sym(1);
			parse_expr_term_x(&t, &b);
			if( t != REAL_type )
				error_non_fatal("right side of %s is not REAL", op);
			out_out_buffer(b);
			free_out_buffer(b);
			op = real_add_op(sym);
		} while( op != NULL );
		close_out_buffer(b2);
		*bp = b2;

	} else if( t == STRING_type ){
		if( sym != plussym )
			error_non_fatal("invalid STRING operator");
		b2 = open_out_buffer();
		out("m2runtime_concat_STRING(0, ");
		out_out_buffer(b);
		free_out_buffer(b);
		do {
			get_sym(1);
			out(", ");
			parse_expr_term_x(&t, &b);
			if( t == STRING_type ){
				out_out_buffer(b);
			} else if( t == INTEGER_type ){
				out("m2runtime_itos(");
				out_out_buffer(b);
				out(")");
			} else if( t == REAL_type ){
				out("m2runtime_rtos(");
				out_out_buffer(b);
				out(")");
			} else {
				error_type_mismatch("invalid term", t, STRING_type);
			}
			free_out_buffer(b);
		} while( sym == plussym );
		out(", (STRING *) 1)");
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else {
		error("type mismatch in operator");

	}
}


static void parse_expr_x(type **tp, OutBuffer **bp)
{

	char * cmp_op(symtype sym)
	{
		switch( sym ){
			case ltsym: return "<";
			case lesym: return "<=";
			case eqsym: return "==";
			case gesym: return ">=";
			case gtsym: return ">";
			case nesym: return "!=";
			default: ERROR;
		}
		return NULL; /* make happy gcc -Wall */
	}


	void cmp_err(type *found, type *expected)
	{
		error_type_mismatch("comparison between different types",
			found, expected);
	}


	type *t, *t2;
	OutBuffer *b, *b2, *b3;
	int op;

	parse_expr_simple_x(&t, &b);

	if( ! (sym == ltsym || sym == lesym || sym == eqsym
	|| sym == gtsym || sym == gesym || sym == nesym ) ){
		*tp = t;
		*bp = b;
		return;
	}

	*tp = BOOLEAN_type;

	if( t == NIL_type ){
		free_out_buffer(b);
		op = sym;
		get_sym(1);
		parse_expr_simple_x(&t, &b3);
		if( ! is_dynamic(t) )
			error_non_fatal("invalid type in comparison with NIL");
		if( t != STRING_type && ! (op == eqsym || op == nesym))
			error_non_fatal("NIL can be compared only using `=' or '<>'");
		if( t == STRING_type && ! (op == eqsym || op == nesym)){
			b = open_out_buffer();
			out("m2runtime_strcmp(NULL, ");
			out_out_buffer(b3);
			out(") %s 0", cmp_op(op));
			close_out_buffer(b);
			*bp = b;
		} else {
			b = open_out_buffer();
			out("NULL %s ", cmp_op(op));
			out_out_buffer(b3);
			free_out_buffer(b3);
			close_out_buffer(b);
			*bp = b;
		}
		return;

	} else if( t == VOID_type ){
		error("cannot compare VOID types");

	} else if( t == BOOLEAN_type ){
		error("cannot compare BOOLEAN types");

	} else if( t == INTEGER_type ){
		b2 = open_out_buffer();
		out("(");
		out_out_buffer(b);
		free_out_buffer(b);
		out(" %s ", cmp_op(sym));
		get_sym(1);
		parse_expr_simple_x(&t, &b);
		if( t != INTEGER_type )
			cmp_err(t, INTEGER_type);
		out_out_buffer(b);
		free_out_buffer(b);
		out(")");
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else if( t == REAL_type ){
		b2 = open_out_buffer();
		out("(");
		out_out_buffer(b);
		free_out_buffer(b);
		out(" %s ", cmp_op(sym));
		get_sym(1);
		parse_expr_simple_x(&t, &b);
		if( t != REAL_type )
			cmp_err(t, REAL_type);
		out_out_buffer(b);
		free_out_buffer(b);
		out(")");
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else if( t == STRING_type ){
		b2 = b;
		op = sym;
		get_sym(1);
		parse_expr_simple_x(&t, &b3);
		if( t != STRING_type && t != NIL_type )
			cmp_err(t, STRING_type);
		if( t == NIL_type && (op == eqsym || op == nesym) ){
			b = open_out_buffer();
			out_out_buffer(b2);
			free_out_buffer(b2);
			out(" %s NULL", cmp_op(op));
			close_out_buffer(b);
			*bp = b;
		} else {
			b = open_out_buffer();
			out("m2runtime_strcmp(");
			out_out_buffer(b2);
			free_out_buffer(b2);
			out(", ");
			out_out_buffer(b3);
			free_out_buffer(b3);
			out(") %s 0", cmp_op(op));
			close_out_buffer(b);
			*bp = b;
		}
		return;

	} else if( t->bt == ARRAY || t->bt == RECORD || t->bt == FUNCTION
	|| t->bt == FORWARD ){
		b2 = open_out_buffer();
		out_out_buffer(b);
		free_out_buffer(b);
		if( sym != eqsym && sym != nesym )
			error_non_fatal("invalid comparison operator - use `=' or `<>'");
		out(" %s ", cmp_op(sym));
		get_sym(1);
		parse_expr_simple_x(&t2, &b);
		if( ! same_type(t, t2) ){
			cmp_err(t2, t);
		}
		out_out_buffer(b);
		free_out_buffer(b);
		close_out_buffer(b2);
		*bp = b2;
		return;

	} else {
		FIXME;
	}

}


static void parse_expr(type * t)
/*
	Parse an expression of known type `t'.
*/
{
	type *t2;
	OutBuffer *b;

	/*
		Resolve the ARRAY and RECORD build operator "{...}":
	*/
	if( sym == lbracesym ){
		if( t->bt == ARRAY ){
			parse_expr_array(t);
		} else if( t->bt == RECORD ){
			parse_expr_record(t);
		} else {
			error("invalid expression");
		}
		return;
	} else if( t->bt == FUNCTION ){
		parse_expr_function(t);
		return;
	}

	/*
		Use the generic expression parser:
	*/
	parse_expr_x(&t2, &b);
	if( ! same_type(t2, t) ){
		error_type_mismatch("invalid type of the expression", t2, t);
	}
	out_out_buffer(b);
	free_out_buffer(b);
}


static void parse_instruction(void);


static void parse_TRY(void)
/*
	Parse TRY...END instruction
*/
{
	int try_line;
	int catch_success;

	indent(); out("m2runtime_ERROR_CODE = 0;\n");
	get_sym(1);

	/* Parse controlled instruction: */
	if( sym != idsym || ! (symid->t == is_var || symid->t == is_func) )
		error_non_fatal("in TRY section, expected assignment or function "
			"call - other instructions are not allowed");
	try_section = 1;
	try_line = sym_start_line;
	parse_instruction();
	try_section = 0;

	indent(); out("switch( m2runtime_ERROR_CODE ){\n\n");

	/* Parse CATCH branches: */
	catch_success = 0;
	while( sym == catchsym ){
		get_sym(1);

		/* Parse list of catched error codes by the CATCH branch: */
		do {
			if( sym != integerlitsym )
				error("expected error code as a literal INTEGER or "
					"CONST of type INTEGER");
			if( symi == 0 )  catch_success = 1;
			indent(); out("case %d:\n", symi);
			get_sym(1);
			if( sym == commasym )
				get_sym(1);
			else
				break;
		} while( 1 );
		if( sym != colonsym ) error("expected `:'");
		get_sym(1);

		/* Parse instructions of the CATCH branch: */
		nest++;
		while( sym != catchsym && sym != elsesym && sym != endsym )
			parse_instruction();
		indent(); out("break;\n\n");
		nest--;
	}

	if( sym == elsesym ){
		/* Parse ELSE branch: */
		if( ! catch_success ){
			indent();
			out("case 0:  break;\n");
		}
		get_sym(1);
		indent(); out("default:\n");
		nest++;
		/* Prevent gcc from giving "deprecated use of label at end
		   of compound statement" if the instruction block of the
		   "default" branch is empty: */
		if( sym == endsym ){
			indent(); out("break;\n");
		}
		while( sym != endsym ){
			parse_instruction();
		}
		nest--;
	} else if( sym == endsym ){
		if( ! catch_success ){
			indent();
			out("case 0:  break;\n");
		}
		indent();
		out("default:\n");
		nest++;
		indent();
		out("m2runtime_HALT(");
		out_err_entry_get(curr_function->name, try_line);
		out(", m2runtime_ERROR_MESSAGE);\n");
		nest--;
	} else {
		error("expected END");
	}

	/* END */
	get_sym(1);
	indent(); out("}\n");
}


static void parse_RAISE_ERROR(void)
/*
	Parse RAISE ERROR instruction
*/
{
	get_sym(1);
	if( sym != errorsym ) error("expected ERROR");
	get_sym(1);

	if( ! curr_function->u.f->raise_error )
		error_non_fatal("cannot RAISE ERROR in FUNCTION lacking the "
			"RAISE ERROR attribute");
	
	indent();
	out("m2runtime_RAISE_ERROR(try_section, ");
	out_err_entry_get(curr_function->name, line);
	out(", ");
	parse_expr(INTEGER_type);
	out(", ");
	parse_expr(STRING_type);
	out(");\n");
}


static void parse_IF(void)
/*
	Parse IF...THEN...ELSIF...END
*/
{
	indent(); out("if( ");
next_if_branch:
	get_sym(1);
	parse_expr(BOOLEAN_type);
	if( sym != thensym ) error("expected THEN");
	get_sym(1);
	out(" ){\n");
	nest++;
	while( ! (sym==elsifsym || sym==elsesym || sym==endsym ) )
		parse_instruction();
	if( sym == elsifsym ){
		nest--;
		indent(); out("} else if( ");
		goto next_if_branch;
	}
	if( sym == elsesym ){
		get_sym(1);
		nest--;
		indent(); out("} else {\n");
		nest++;
		while( sym != endsym )
			parse_instruction();
	}
	if( sym != endsym ) error("expected END");
	get_sym(1);
	nest--;
	indent(); out("}\n");
}


static void parse_WHILE(void)
/*
	Parse WHILE...DO...END
*/
{
	indent(); out("while( ");
	get_sym(1);
	parse_expr(BOOLEAN_type);
	if( sym != dosym ) error("expected DO");
	get_sym(1);
	out(" ){\n");
	nest++;
	while( sym!=endsym )
		parse_instruction();
	if( sym != endsym ) error("expected END");
	get_sym(1);
	nest--;
	indent(); out("}\n");
}


static void parse_REPEAT(void)
/*
	Parse REPEAT ... UNTIL
*/
{
	indent(); out("do {\n");
	nest++;
	get_sym(1);
	while( sym != untilsym )
		parse_instruction();
	get_sym(1);
	nest--;
	indent(); out("} while( !( ");
	parse_expr(BOOLEAN_type);
	out(" ));\n");
}


int for_limit_counter = 0;
/*
	Strategy: the instruction

		FOR i=1 TO 10 BY 2 DO
			...
		END

	is translated to something like this

		{
			int m2runtime_for_limit_XXX;
			i=1;
			m2runtime_for_limit_XXX = 10;
			for( ; i <= m2runtime_for_limit_XXX; i+=2 ){
				...
			}
		}

	where XXX is the value of the variable for_limit_counter. This
	counter is incremented every time we enter inside a FOR, and
	decremented on exit.

	If the increment is negative,

		FOR i=10 TO 1 BY -1 DO
			...
		END
	
	we translate to

		{
			int m2runtime_for_limit_XXX;
			i=10;
			m2runtime_for_limit_XXX = 1;
			for( ; i >= m2runtime_for_limit_XXX; i-=1 ){
				...
			}
		}

*/


static void parse_FOR(void)
/*
	Parse FOR...END
*/
{


	int const_expr_integer(void)
	{


		int const_expr_integer_factor(void)
		{
			int res;

			if( sym == integerlitsym ){
				res = symi;
				get_sym(1);
				return res;
			} else if( sym == lroundsym ){
				get_sym(1);
				res = const_expr_integer();
				if( sym != rroundsym )
					error("expected `)'");
				get_sym(1);
				return res;
			} else {
				error("unexpected symbol");
			}
			return 0; /* make happy gcc -Wall */
		}


		int const_expr_integer_term(void)
		{
			int res, q;

			res = const_expr_integer_factor();
			while( sym == timessym || sym == divsym || sym == modsym ){
				switch( sym ){
				case timessym:
					get_sym(1);
					res = res * const_expr_integer_factor();
					break;
				case divsym:
					get_sym(1);
					q = const_expr_integer_factor();
					if( q == 0 )
						error("division by zero");
					res = res / q;
					break;
				case modsym:
					get_sym(1);
					q = const_expr_integer_factor();
					if( q == 0 )
						error("division by zero");
					res = res % q;
					break;
				default: ;
				}
			}
			return res;
		}


		int res;

		if( sym == plussym ){
			get_sym(1);
			res = const_expr_integer_term();
		} else if( sym == minussym ){
			get_sym(1);
			res = - const_expr_integer_term();
		} else {
			res = const_expr_integer_term();
		}
		while( sym == plussym || sym == minussym ){
			if( sym == plussym ){
				get_sym(1);
				res = res + const_expr_integer_term();
			} else {
				get_sym(1);
				res = res - const_expr_integer_term();
			}
		}
		return res;
	}


	identifier *v;
	int step;

	for_limit_counter++;
	step = 1;
	indent(); out("{\n");
	nest++;
	indent(); out("int m2runtime_for_limit_%d;\n", for_limit_counter);
	get_sym(1);
	if( sym != idsym || symid->t != is_var )
		error("expected variable");
	v = symid;
	if( v->u.v->type->bt != INTEGER )
		error("expected variable of type INTEGER");
	if( v->u.v->method == byaddress )
		error("formal argument passed by address not allowed as index");
	get_sym(1);
	if( sym != eqsym ) error("expected `='");
	get_sym(1);
	indent();
	out("%s_%s = ", v->m->name, v->name);
	parse_expr(INTEGER_type);
	out(";\n");
	indent();  out("m2runtime_for_limit_%d = ", for_limit_counter);
	if( sym != tosym ) error("expected TO");
	get_sym(1);
	parse_expr(INTEGER_type);
	out(";\n");
	if( sym == bysym ){
		get_sym(1);
		step = const_expr_integer();
		/* FIXME: what if step==0? or step is very large 2^30? */
	}
	indent();
	if( step >= 0 ){
		out("for( ; %s_%s <= m2runtime_for_limit_%d; %s_%s += %d ){\n",
			v->m->name, v->name, for_limit_counter, v->m->name, v->name, step);
	} else {
		out("for( ; %s_%s >= m2runtime_for_limit_%d; %s_%s -= %d ){\n",
			v->m->name, v->name, for_limit_counter, v->m->name, v->name, -step);
	}
	if( sym != dosym ) error("expected DO");
	get_sym(1);
	nest++;
	while( sym != endsym )
		parse_instruction();
	get_sym(1);
	nest--;
	indent();  out("}\n");
	nest--;
	indent(); out("}\n");
	for_limit_counter--;
}


/*
	LOOP

	The instructions

		LOOP
			EXIT
			LOOP
				EXIT
			END
			LOOP
				EXIT
			END
		END

	are translated to

		{
			goto m2runtime_loop_1;
			{
				goto m2runtime_loop_2;
			}
			m2runtime_loop_2: ;
			{
				goto m2runtime_loop_3;
			}
			m2runtime_loop_3: ;
		}
		m2runtime_loop_1: ;

*/

int loop_count;
/* Incremented entering a LOOP, reset entering a function. */

int loop_curr;
/* Current LOOP label no. */

int loop_used;
/* Got at least an EXIT in the current LOOP? */

int loop_level;
/* LOOP nesting level. */


static void parse_LOOP(void)
/*
	Parse LOOP...END
*/
{
	int loop_curr_saved, loop_used_saved;
	int loop_line;

	/* Save current LOOP status to be restored on exit from LOOP: */
	loop_curr_saved = loop_curr;
	loop_used_saved = loop_used;

	/* Generate a brand new loop label and LOOP status: */
	loop_count++;
	loop_curr = loop_count;
	loop_used = 0;
	loop_level++;

	loop_line = sym_start_line;
	get_sym(1);
	indent(); out("do{\n");
	nest++;
	while( sym != endsym )
		parse_instruction();
	if( sym != endsym )
		error("expected instruction or END of the LOOP open in line %d",
		loop_line);
	get_sym(1);
	nest--;
	indent(); out("}while(TRUE);\n");

	/* Generate the "goto" label only if at least an EXIT is used,
	   otherwise gcc -Wall will complain about the unused label: */
	if( loop_used )
		out("m2runtime_loop_%d: ;\n", loop_curr);

	/* Restore previous LOOP status: */
	loop_curr = loop_curr_saved;
	loop_used = loop_used_saved;
	loop_level--;
}


static void parse_EXIT(void)
/*
	Parse EXIT (from LOOP)
*/
{
	if( loop_level == 0 )
		error_non_fatal("EXIT must be inside a LOOP");
	get_sym(1);
	indent(); out("goto m2runtime_loop_%d;\n", loop_curr);
	loop_used = 1;
}


static void parse_assign_or_proc_call(void)
{
	type *t;

	indent();
	t = parse_lhs(1);
	if( t == NULL ){
		out(";\n");
	} else {
		if( sym != eqsym )
			error("expected assignment or dereferencing operator");
		out(" = ");
		get_sym(1);
		parse_expr(t);
		out(";\n");
	}
}


static void parse_RETURN(void)
{
	type *t, *rt;

	if( curr_function == NULL )  ERROR;
	t = curr_function->u.f->type;
	indent();  out("return ");
	get_sym(1);
	if( t != NULL ){
		if( curr_function->u.f->return_method == byvalue ){
			parse_expr(t);
		} else {
			rt = parse_lhs(0);
			if( ! same_type(t, rt) ){
				error_type_mismatch("invalid return type", rt, t);
			}
		}
	}
	out(";\n");
}


static void parse_SWITCH(void)
{
	struct {
		int case_no;
		int line;
	} *cases = NULL;
	int n_cases = 0;
	int s_cases = 0;

	void add_case(int case_no)
	{
		int i;

		for( i=0; i<n_cases-1; i++ ){
			if( cases[i].case_no == case_no ){
				error_non_fatal("duplicate case value %d previously used "
					"in line %d", case_no, cases[i].line);
			}
		}
		if( n_cases + 1 > s_cases ){
			s_cases = 2*s_cases + 1; /* FIXME */
			cases = xrealloc(cases, s_cases * sizeof(*cases));
		}
		cases[n_cases].case_no = case_no;
		cases[n_cases].line = line;
		n_cases++;
	}

	indent();
	out("switch(");
	get_sym(1);
	parse_expr(INTEGER_type);
	if( sym != dosym )
		error("invalid expression or missing DO in SWITCH");
	get_sym(1);
	out("){\n");

	while( sym == casesym ){
		out("\n");
		get_sym(1);
		do {
			if( sym != integerlitsym )
				error("expected literal integer or integer constant in CASE");
			add_case(symi);
			indent();
			out("case %d:\n", symi);
			get_sym(1);
			if( sym == commasym )
				get_sym(1);
			else
				break;
		} while(1);
		if( sym != colonsym )
			error("invalid syntax in CASE - missing `:'?");
		get_sym(1);
		while( sym != casesym && sym != elsesym && sym != endsym ){
			parse_instruction();
		}
		indent();
		out("break;\n");
	}

	if( sym == elsesym ){
		get_sym(1);
		if( sym != endsym ){
			out("\n");
			indent();
			out("default:\n");
			while( sym != endsym ){
				parse_instruction();
			}
		}
	} else {
		out("\n");
		indent();
		out("default: m2runtime_missing_case_in_switch(");
		out_err_entry_get(curr_function->name, sym_start_line);
		out(");\n");
	}

	if( sym != endsym )
		error("invalid syntax in SWITCH branch - missing END?");
	get_sym(1);
	indent();
	out("}\n");
	free(cases);
}


static void parse_FINALIZER(void)
{
	identifier *f_id;
	function *f;

	get_sym(1);
	if( sym != lroundsym )
		error("expected `('");
	get_sym(1);
	if( sym != idsym || symid->t != is_func )
		error("expected finalizer procedure name");
	f_id = symid;
	f = f_id->u.f;
	if( f->type != NULL )
		error("invalid finalizer: must be a procedure");
	if( f->nargs != 1 )
		error("invalid finalizer: must take one argument");
	if( f->argtype[0]->bt != ARRAY
	&& f->argtype[0]->bt != RECORD
	&& f->argtype[0]->bt != FORWARD )
		error("invalid finalizer: argument must be ARRAY, RECORD or FORWARD");
	if( f->argmethod[0] != byvalue )
		error("invalid finalizer: argument must be passed by value");
	get_sym(1);
	if( sym != commasym )
		error("expected `,'");
	get_sym(1);
	indent();
	out("GC_REGISTER_FINALIZER(");
	parse_expr( f->argtype[0] );
	out(", (GC_finalization_proc) %s_%s, 0, 0, 0);\n", f_id->m->name,
		f_id->name);
	if( sym != rroundsym )
		error("expected `)'");
	get_sym(1);
}


static void parse_HALT(void)
{
	indent();
	out("m2runtime_HALT(");
	out_err_entry_get(curr_function->name, line);
	out(", ");
	get_sym(1);
	if( sym != lroundsym )  error("expected `('");
	get_sym(1);
	parse_expr(STRING_type);
	if( sym != rroundsym )  error("expected `)'");
	get_sym(1);
	out(");\n");
}


static void parse_instruction(void)
{
	switch( sym ){

	case idsym:
		if( symid->t == is_var || symid->t == is_func )
			 parse_assign_or_proc_call();
		else
			error("unexpected identifier `%s'", symid->name);
		break;
	
	case exitsym:   parse_EXIT();   break;
	case finalizersym: parse_FINALIZER(); break;
	case forsym:    parse_FOR();    break;
	case haltsym:   parse_HALT();   break;
	case ifsym:     parse_IF();     break;
	case inlinecodesym: parse_inlinecode(); break;
	case loopsym:   parse_LOOP();   break;
	case raisesym:  parse_RAISE_ERROR();  break;
	case repeatsym: parse_REPEAT(); break;
	case returnsym: parse_RETURN(); break;
	case switchsym: parse_SWITCH(); break;
	case trysym:    parse_TRY();    break;
	case whilesym:  parse_WHILE();  break;
	default:        error("expected instruction");
	}
}


static void parse_declarations(void);


static void parse_FUNCTION(void)
/*
	Il nome della funzione viene inserito nella symbols table, e a
	seguire vengono inseriti i nomi degli argomenti come se fossero
	variabili locali. Il tipo degli argomenti viene inserito nelle
	entrate della symbol table, ma viene copiato anche negli array
	degli argomenti della funzione.  All'END del sorgente della
	funzione eseguo il roll-back della symbols table fino all'ID
	della funzione. In questo modo, degli argomenti rimane il tipo,
	ma non il nome.
*/
{


	void out_return_type(type *t)
	{
		if( t == NULL ){
			out("void");
			return;
		}
		switch( t->bt ){
			case VOID:    out("void"); break;
			case BOOLEAN:
			case INTEGER: out("int"); break;
			case REAL:    out("double "); break;
			case STRING:  out("STRING *"); break;
			case ARRAY:   out("ARRAY *"); break;
			case RECORD:  out("RECORD *"); break;
			case FUNCTION: FIXME;
			case FORWARD: out("void *"); break;
			default: ERROR;
		}
	}


	void out_arg_decl(identifier *v)
	{
		function *f;

		switch( v->u.v->type->bt ){

			case NIL: ERROR;
			case VOID: out("void "); break;
			case BOOLEAN:
			case INTEGER: out("int "); break;
			case REAL: out("double "); break;
			case STRING: out("STRING *"); break;
			case ARRAY: out("ARRAY *"); break;
			case RECORD: out("RECORD *"); break;
			case FUNCTION:
				f = v->u.v->type->func;
				if( f->type == NULL ){
					out("void ");
				} else {
					switch( f->type->bt ){
					case NIL: ERROR;
					case VOID:  out("void"); break;

					case BOOLEAN:
					case INTEGER: out("int ");  break;

					case REAL: out("double ");  break;
					case STRING: out("STRING *");  break;
					case ARRAY: out("ARRAY *");  break;
					case RECORD: out("RECORD *");  break;
					case FUNCTION: FIXME;
					case FORWARD: FIXME;
					default: ERROR;
					}
				}
				break;

			case FORWARD: out("void *"); break;

			default: ERROR;
		}
		out_var_byvalue(v);
		if( v->u.v->type->bt == FUNCTION ){
			out("()");
		}
	}


	type *t;
	identifier *f_id, *v, *id;
	function *f;
	int arg_method, is_func_impl, arg_n, raise_error;
	method return_method;
	identifier * parent_function;
	identifier * rollback_upto;
	int last_instruction_return;

	void out_args(identifier *id, int args_left)
	{
		if( args_left == 0 )  return;
		while( id->t != is_var )
			id = id->next;
		out_args(id->next, args_left-1);
		if( args_left > 1 )  out(", ");
		out_arg_decl(id);
	}

	/* Take the function name: */
	get_sym(3);
	if( sym != namesym )  error("expected function name");

	/* Is it the implementation of a function? */
	is_func_impl = 0;
	if( scope == 0 && curr_module->u.m->type == impl ){
		f_id = NULL;
		id = ids;
		while( id != NULL ){
			if( id->t == is_func
				&& strcmp(syms, id->name) == 0
				&& id->m->u.m->type == def
				&& strcmp(curr_module->name, id->m->name) == 0
			){
				/* That's the implementation of f_id->name: */
				f_id = id;
				is_func_impl = 1;
				break;
			}
			id = id->next;
		}
	}

	if( is_func_impl ){
		rollback_upto = ids;
	} else {
		check_name_in_curr_module_same_scope(syms);
		add_id(syms, is_func);
		f_id = ids;
		rollback_upto = ids;
	}

	f_id->scope++;
	scope++;
	/* entering the declaration of the function, its name is in the same
	scope of the arguments and local objects; exiting from the function
	we will lower its scope */

	f = f_id->u.f;
	get_sym(1);

	/* Parse args: */
	if( sym != lroundsym ) error("expected `('");
	get_sym(0);
	arg_n = 0;
	while( 1 ){
		if( sym == rroundsym ){
			get_sym(0);
			break;
		}
		if( arg_n > 0 ){
			if( sym != commasym )
				error("expected `,' or `)'");
			get_sym(0);
		}

		if( sym == varsym ){
			arg_method = byaddress;
			get_sym(0);
		} else {
			arg_method = byvalue;
		}

		if( sym != namesym )
			error("expected argument name");
		if( curr_module->u.m->type == def ){
			/* DEFINITION MODULE: non serve salvare i nomi degli argomenti. */
		} else {
			/* MODULE or IMPLEMENTATION */
			add_id(syms, is_var);
			v = ids;
			v->u.v->method = arg_method;
		}

		get_sym(1);
		if( sym != colonsym )  error("expected `:'");
		get_sym(1);
		t = parse_subtype(NULL);

		/* Metti il tipo nell'ID dell'argomento: */
		if( curr_module->u.m->type != def ){
			v->u.v->type = t;
			v->u.v->method = arg_method;
		}

		if( is_func_impl ){
			if( arg_n == f->nargs )
				error_non_fatal("implementation of the function `%s': more "
					"arguments than in the declaration", f_id->name);
			if( arg_n < f->nargs ){

				if( arg_method != f->argmethod[arg_n] )
					error("implementation of the function `%s' arg no. %d: "
						"passing method VAR/value differ from the declaration",
						f_id->name, arg_n + 1);

				if( ! same_type(t, f->argtype[arg_n]) )
					error_non_fatal("implementation of the function `%s' arg no. %d: "
						"type differ from the declaration",
						f_id->name, arg_n + 1);
			}
			/* FIXME: memory leak: the type t should be deallocated */
		} else {
			/* Metti il tipo nella signature della funzione: */
			f->argtype[f->nargs] = t;
			f->argmethod[f->nargs] = arg_method;
			f->nargs++;
		}

		arg_n++;

	}

	if( is_func_impl && arg_n < f->nargs ){
		error_non_fatal("implementation of the function `%s': missing "
			"arguments", f_id->name);
	}

	/* Parse return type: */
	if( sym == colonsym ){
		get_sym(1);
		if( sym == varsym ){
			return_method = byaddress;
			get_sym(1);
		} else {
			return_method = byvalue;
		}
		t = parse_subtype(NULL);
	} else {
		t = NULL;
	}
	if( is_func_impl ){
		if( f->type != NULL && f->return_method != return_method )
			error_non_fatal("implementation of the function `%s': "
				"return method differ from the declaration", f_id->name);
		if( ! same_type(t, f->type) )
			error_non_fatal("implementation of the function `%s': "
				"return type differ from the declaration", f_id->name);
		/* FIXME: release t */
	} else {
		f->return_method = return_method;
		f->type = t;
	}

	/* Parse RAISE ERROR properties: */
	raise_error = 0;
	if( sym == raisesym ){
		get_sym(1);
		if( sym != errorsym )  error("expected ERROR after RAISE");
		get_sym(1);
		raise_error = 1;
	}
	if( is_func_impl ){
		if( f->raise_error != raise_error )
			error_non_fatal("implementation of the function `%s': "
				"mismatch in property RAISE ERROR from the declaration",
				f_id->name);
	} else {
		f->raise_error = raise_error;
	}

	/* If we are in a DEFINITION, stop here: */
	if( curr_module->u.m->type == def ){
		f_id->scope--;
		scope--;
		/* as stated before, we lower the scope of the name of the
		function */
		/* FIXME: something to roll-back? we may have enum. consts. */
		/*symbol_table_rollback(f_id); */
		return;
	}

	/*
		We are in a MODULE or IMPLEMENTATION MODULE.
		Print the function decl. and body code.
	*/

	/* Print function return type: */
	out("\n");
	indent();
	if( scope == 0 && curr_module->u.m->type == mod )  out("static ");
	out_return_type(f->type);
	if( f->type != NULL && f->return_method == byaddress )
		out(" *");
	out("\n");

	/* Print function name: */
	indent();
	out("%s_%s(", f_id->m->name, f_id->name);
	
	/* Print formal args: */
	if( f->nargs == 0 && f->raise_error == 0 ){
		out("void");
	} else {
		if( f->raise_error == 1 ){
			out("int try_section");
			if( f->nargs > 0 )
				out(", ");
		}
		/*
			Devo estrarre gli argomenti nell'ordine della dichiarazione.
			Nella symbols table gli argomenti ora si trovano in ordine
			inverso: sfrutto la ricorsione.
		*/
		out_args(ids, f->nargs);
	}
	out(")\n");
	indent();
	out("{\n");
	nest++;

	parent_function = curr_function;
	curr_function = f_id;

	parse_declarations();

	/* BEGIN */
	if( sym != beginsym )
		error("expected BEGIN");
	get_sym(1);

	last_instruction_return = 0;

	loop_count = 0;

	while( sym != endsym ){
		last_instruction_return = ( sym == returnsym );
		parse_instruction();
	}

	/* END */
	if( sym != endsym )
		error("expected instruction or END");

	/* What if missing "RETURN" in function returning a value? */
	if( ! last_instruction_return && f_id->u.f->type != NULL ){

		/* Give a fatal error: */
		indent();
		out("m2runtime_missing_return(");
		out_err_entry_get(curr_function->name, sym_start_line);
		out(");\n");

		/* Prevent warning from gcc -Wall: */
		indent();
		out("return ");
		switch( f_id->u.f->type->bt ){
			case VOID: FIXME; break;
			case BOOLEAN: out("0"); break;
			case INTEGER: out("0"); break;
			case REAL: out("0.0"); break;
			case STRING:
			case ARRAY:
			case RECORD:
			case FORWARD: out("NULL"); break;
			default: ERROR;
		}
		out(";\n");
	}

	get_sym(1);

	nest--;
	indent(); out("}\n\n");

	f_id->scope--;
	scope--;

	symbol_table_rollback(rollback_upto);

	curr_function = parent_function;
}


static void parse_declarations(void)
/*
	Parse declarations inside MODULE, DEFINITION MODULE,
	IMPLEMENTATION MODULE and FUNCTION.
*/
{
	while( 1 ){
		switch( sym ){
			case importsym: error("IMPORT not allowed here");
			case constsym:      parse_CONST();      break;
			case typesym:       parse_TYPE();       break;
			case varsym:        parse_VAR();        break;
			case functionsym:   parse_FUNCTION();   break;
			case inlinecodesym: parse_inlinecode(); break;
			default: return;
		}
	}
}


int file_readable(char *fn)
{
	int fd;

	fd = open(fn, O_RDONLY);
	if( fd >= 0 ){
		close(fd);
		return 1;
	}
	return 0;
}


void check_eof(void)
{
	if( sym != eofsym )
		error_non_fatal("trail chars at the end of the file");
}


static void parse_DEFINITION_MODULE(char * src_file_name);


static void import_module(char *module_name)
/*
	Import the specified module in the current context:
	1. Save the parser status.
	2. The content of the .lnk file, if exists, is sent to stdout.
	3. The DEFINITION MODULE is read and the exported symbols added to the
	   symbol table.
	4. Restore the initial status of the parser.
*/
{

	char * concat(char *a, char *b)
	{
		char *s;

		s = xmalloc( strlen(a) + strlen(b) + 1 );
		strcpy(s, a);
		strcat(s, b);
		return s;
	}


	void cat(char *fn)
	{
		FILE *f;
		char s[100];
		int n;

		f = fopen(fn, "r");
		if( f == NULL ){
			fprintf(stderr, "%s: %s\n", fn, strerror(errno));
			return;
		}
		while( ( n = fread(s, 1, 100, f) ) > 0 )
			fwrite(s, n, 1, stdout);
		fclose(f);
	}


	char *module_pathfile(char *m)
	/*
		Search the file m.def. Return the complete pathfile excluding
		the extension ".def". Return NULL if not found.
	*/
	{


		char * concat3(char *a, char *b, char *c)
		{
			char *s;

			s = xmalloc( strlen(a) + strlen(b) + strlen(c) + 1 );
			strcpy(s, a);
			strcat(s, b);
			strcat(s, c);
			return s;
		}


		char *libs, *d, *def, *t, *path;

		def = concat(m, ".def");
		if( file_readable(def) ){
			free(def);
			return xstrdup(m);
		}

		libs = getenv("M2_LIB");
		if( libs == NULL ){
			free(def);
			return NULL;
		}
		path = xstrdup(libs);
		d = strtok(path, ":");
		while( d != NULL ){
			t = concat3(d, "/", def);
			if( file_readable(t) ){
				free(t);
				t = concat3(d, "/", m);
				free(def);
				free(path);
				return t;
			}
			free(t);
			d = strtok(NULL, ":");
		}
		free(def);
		free(path);
		return NULL;
	}


	char *path;    /* pathfile of the module without extension */
	char *def_mod; /* file name of the DEFINITION */
	char *imp_mod; /* file name of the IMPLEMENTATION */
	char *lnk;     /* file name of the linker options */
	parser_status ps;

	if( curr_out_buffer != NULL )  ERROR;

	/*
		Guess the pathfile of the module:
	*/
	path = module_pathfile(module_name);
	if( path == NULL ){
		error_non_fatal("module %s.def not found", module_name);
		return;
	}

	/*
		Send to stdout the .lnk file, but not that of myself:
	*/
	if( curr_module != NULL
	&& strcmp(module_name, curr_module->name) != 0 ){
		lnk = concat(path, ".lnk");
		if( file_readable(lnk) ){
			cat(lnk);
		} else {
			/*warning("missing %s.lnk", path); */
		}
		free(lnk);
	}

	/*
		Generate #include:
	*/
	if(
		/* Import in a MODULE? */
		(curr_module != NULL && curr_module->u.m->type == mod )

		/* Import in an IMPLEMENTATION of a (different) module? */
		||  (ori_module != NULL && ori_module->u.m->type == impl
			&& strcmp(ori_module->name, module_name) != 0 )
	){
		imp_mod = concat(path, ".c");
		if( ! file_readable(imp_mod) ){
			error_non_fatal("missing object file `%s.c'", path);
		} else {
			out("\n#ifndef M2_IMPORT_%s\n", module_name);
			out("#    include \"%s\"\n", imp_mod);
			out("#endif\n");
		}
		free(imp_mod);
	}

	/*
		Parsing the DEFINITION of the imported module:
	*/

	suspend_parser(&ps);
	def_mod = concat(path, ".def");
	parse_DEFINITION_MODULE(def_mod);
	check_eof();
	free(def_mod);
	if( curr_out_buffer != NULL )  ERROR;
	resume_parser(&ps);
	free(path);
}


static void parse_IMPORT(void)
{

	int already_imported(char * name)
	{
		identifier *id;

		id = ids;
		while( id != NULL ){
			if( id->t == is_module
			&& id->u.m->type == def
			&& strcmp(name, id->name) == 0 )
				return 1;
			id = id->next;
		}
		return 0;
	}


	get_sym(3);

	if( sym != namesym )
		return;

	do {
		if( ! already_imported(syms) )
			import_module(syms);
		get_sym(3);
		if( sym == commasym ){
			get_sym(3);
			if( sym != namesym )
				error("expected module name after comma");
		} else if( sym == namesym ){
			error_non_fatal("missing comma in list of imported modules");
		} else {
			return;
		}
	} while(1);
}


static void parse_import_section(void)
{
	while(1){
		switch( sym ){
		case importsym:     parse_IMPORT();  break;
		case inlinecodesym: parse_inlinecode();  break;
		default: return;
		}
	}

}


static void parse_DEFINITION_MODULE(char * src_file_name)
{
	if( debug )
		printf("Parsing the DEFINITION MODULE %s:\n", src_file_name);
	new_parser(src_file_name);
	if( sym != definitionsym )  error("expected DEFINITION");
	get_sym(3);
	if( sym != modulesym )  error("expected MODULE");
	get_sym(3);
	if( sym != namesym ) error("expected module name");
	add_id(syms, is_module);
	curr_module = ids;
	curr_module->u.m->type = def;
	if( ori_module != NULL && strcmp(ori_module->name, syms) == 0 )
		/* We are importing the DEF of the corresponding IMPL: */
		curr_module->used = 1;
	get_sym(1);

	parse_import_section();

	parse_declarations();

	if( sym != endsym )
		error("bad syntax - expected declaration or END");
	get_sym(1);

	check_eof();

	free_parser();
}


static void parse_IMPLEMENTATION_MODULE(char *src_file_name, char *obj_file_name)
{
	if( debug )
		printf("Parsing the IMPLEMENTATION MODULE %s:\n", src_file_name);
	new_parser(src_file_name);
	if( sym != implementationsym )  error("expected IMPLEMENTATION");
	get_sym(1);
	if( sym != modulesym )  error("expected MODULE");
	get_sym(3);

	if( sym != namesym ) error("expected module name");
	add_id(syms, is_module);
	curr_module = ids;
	curr_module->u.m->type = impl;
	curr_module->used = 1;
	ori_module = curr_module;
	init_obj(obj_file_name);
	out("/* IMPLEMENTATION MODULE %s */\n", curr_module->name);
	out("#define M2_IMPORT_%s\n", curr_module->name);
	get_sym(1);

	import_module(curr_module->name);  /* my DEFINITION */

	parse_import_section();

	out_err_entry_get_prototype();

	parse_declarations();

	if( sym != endsym )
		error("expected declaration or END");
	get_sym(0);

	check_eof();

	out_err_entry_get_function();

	symbol_table_rollback(curr_module);
	free_parser();
}


static void parse_MODULE(char *src_file_name, char *obj_file_name)
{
	int last_instruction_return;

	if( debug )
		printf("Parsing the MODULE %s:\n", src_file_name);
	new_parser(src_file_name);
	if( sym != modulesym )  error("expected MODULE");
	get_sym(3);
	if( sym != namesym ) error("expected module name");
	add_id(syms, is_module);
	curr_module = ids;
	curr_module->u.m->type = mod;
	curr_module->used = 1;
	ori_module = curr_module;

	init_obj(obj_file_name);
	out("/* MODULE %s */\n", curr_module->name);
	out("\n#include \"m2runtime.c\"\n");

	get_sym(1);

	parse_import_section();

	out_err_entry_get_prototype();

	parse_declarations();

	if( sym != beginsym )
		error("expected declaration or BEGIN");
	get_sym(1);

	add_id("0main", is_func);
	ids->used = 1;
	ids->u.f->type = INTEGER_type;
	ids->u.f->return_method = byvalue;
	curr_function = ids;
	out("\nint %s_0main()\n{\n", curr_module->name);
	nest++;

	last_instruction_return = 0;
	loop_count = 0;
	while( sym != endsym ){
		last_instruction_return = ( sym == returnsym );
		parse_instruction();
	}
	get_sym(0);

	if( ! last_instruction_return )
		out("\treturn 0;\n");
	nest--;
	out("}\n");

	check_eof();

	out_err_entry_get_function();

	out("\nint main(int argc, char *argv[])\n"
	    "{\n"
	    "\tm2runtime_argc = argc;  m2runtime_argv = (char **)argv;\n"
	    "\tGC_INIT();\n"
		"\treturn %s_0main();\n"
		"}\n", curr_module->name);

	out("\n/* END */\n");

#ifdef DEBUG
	print_symbol_table();
#endif

	symbol_table_rollback(curr_module);
	free_parser();
}


static void version(void)
{
	printf("M2C " VERSION " - Cross compiler from M2 to C\n"
		"Copyright 2007 by Umberto Salsi\n"
		"More info: http://www.icosaedro.it/m2\n"
		"Report bugs to <m2@icosaedro.it>\n");
}


static void help(void)
{
	printf(
		"Usage: m2c <src.mod> -o <obj.c>\n"
		"       m2c <src.def>\n"
		"       m2c <src.imp>\n"
		"Options: --debug --help --no-warn --tabsize size --version\n");
}


int main(int ac, char *av[])
{
	int i;
	char *src_file_name = NULL, *obj_file_name = NULL;

	i = 1;
	while( i < ac ){
		if( strcmp(av[i], "--help") == 0 ){
			help();
			exit(0);
		} else if( strcmp(av[i], "--version") == 0 ){
			version();
			exit(0);
		} else if( strcmp(av[i], "-o") == 0 ){
			i++;
			if( i >= ac ){
				fprintf(stderr, "m2c: the option -o require an argument\n");
				exit(1);
			}
			if( obj_file_name != NULL ){
				fprintf(stderr, "m2c: only one obj file can be specified\n");
				exit(1);
			}
			obj_file_name = av[i];
			i++;
		} else if( strcmp(av[i], "--debug") == 0 ){
			debug = 1;
			i++;
		} else if( strcmp(av[i], "--tabsize") == 0 ){
			i++;
			if( i >= ac ){
				fprintf(stderr, "m2c: the option --tabsize requires an argument\n");
				exit(1);
			}
			tabsize = atoi(av[i]);
			i++;
		} else if( strcmp(av[i], "--no-warn") == 0 ){
			warn = 0;
			i++;
		} else if( av[i][0] == '-' ){
			fprintf(stderr, "m2c: unknown option %s\n", av[i]);
			exit(1);
		} else {
			if( src_file_name != NULL ){
				fprintf(stderr, "m2c: only one src file can be specified\n");
				exit(1);
			}
			src_file_name = av[i];
			i++;
		}
	}

	if( src_file_name == NULL ){
		fprintf(stderr, "m2c: missing src file\n");
		exit(1);
	}

	add_simple_types();

	import_module("m2runtime");

	new_parser(src_file_name);
	free_parser();

	switch( sym ){

	case definitionsym:
		parse_DEFINITION_MODULE(src_file_name);
		break;
	
	case implementationsym:
		if( obj_file_name == NULL ){
			fprintf(stderr, "m2c: the file appear to be an IMPLEMENTATION MODULE - "
				"Please specify the destination object file (-o <obj>)\n");
			exit(1);
		}
		parse_IMPLEMENTATION_MODULE(src_file_name, obj_file_name);
		break;
	
	case modulesym:
		if( obj_file_name == NULL ){
			fprintf(stderr, "m2c: the file appear to be a MODULE - "
				"Please specify the destination object file (-o <obj>)\n");
			exit(1);
		}
		parse_MODULE(src_file_name, obj_file_name);
		break;
	
	default:
		fprintf(stderr, "m2c: file content unknown - "
			"Expected DEFINITION, IMPLEMENTATION or MODULE\n");
		exit(1);
	
	}

	if( scope != 0 )  ERROR;

	if( curr_out_buffer != NULL )  ERROR;

#ifdef DEBUG
	print_symbol_table();
#endif

	if( error_count == 0 )
		exit(0);
	else
		exit(1);
}

/* THE END */
