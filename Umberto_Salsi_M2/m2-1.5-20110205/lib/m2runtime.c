/* IMPLEMENTATION MODULE m2runtime */
#define M2_IMPORT_m2runtime

/*
	M2 Runtime Library

	Copyright 2003 by Umberto Salsi <salsi@icosaedro.it>
	http://www.icosaedro.it/m2/
*/


#define _GNU_SOURCE
#include <alloca.h>
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

/* The flag USE_GC get set in include/missing.c by 'configure': */
#ifdef USE_GC
#    include <gc/gc.h>
#    define m2runtime_GC() GC_gcollect()
#else
#    define GC_INIT()
#    define m2runtime_GC()
#endif


//#undef QN
//#define QN(n)  m2runtime_##n

/*
	2006-12-15 "Markus Kuehni" <markus.kuehni@trilab.ch>
	+ Add the TEMP_FAILURE_RETRY macro to support cygwin:
*/
#ifndef TEMP_FAILURE_RETRY

#define TEMP_FAILURE_RETRY(expression) \
(__extension__ \
	({ long int __result; \
		do __result = (long int) (expression); \
		while (__result == -1L && errno == EINTR); \
		__result; }))

#endif /* TEMP_FAILURE_RETRY */


#define FALSE 0
#define TRUE !FALSE

#define m2runtime_mark(v)


static void die(char *fmt, ...)
/*
	Fatal run-time error.
*/
{
	va_list ap;
	char *s;

	va_start(ap, fmt);
	vasprintf((char **) &s, fmt, ap);
	va_end(ap);
	fprintf(stderr, "**** M2 RUNTIME ERROR: %s\n", s);
	abort();
	//exit(EXIT_FAILURE);
}


static void OOM()
/*
	Out of memory error.
*/
{
	fprintf(stderr, "OUT OF MEMORY\n");
	abort();
	//exit(EXIT_FAILURE);
}


void * M2_MALLOC(size_t s)
{
	void * p;

#ifdef USE_GC
	p = GC_MALLOC(s);
#else
	p = malloc(s);
#endif
	//printf("M2_MALLOC(%d) -> %x\n", s, p);
	if( p == NULL )  OOM();
	return p;
}


void * M2_MALLOC_ATOMIC(size_t s)
{
	void * p;

#ifdef USE_GC
	p = GC_MALLOC_ATOMIC(s);
#else
	p = malloc(s);
#endif
	//printf("M2_MALLOC_ATOMIC(%d) -> %x\n", s, p);
	if( p == NULL )  OOM();
	return p;
}


void * M2_REALLOC(void * p, size_t s)
{
	void * q;

#ifdef USE_GC
	q = GC_REALLOC(p, s);
#else
	q = realloc(p, s);
#endif
	//printf("M2_REALLOC(%x, %d) -> %x\n", p, s, q);
	if( q == NULL && s > 0 )  OOM();
	return q;
}


#ifdef USE_GC
#    define M2_FREE  GC_free
#else
#    define M2_FREE  free
#endif


/*
	M2 BASIC TYPES
*/

#define BASE_TYPE_ID_MASK     0xff
/*
	The 4 low order bits of the ID field is the type of the datum,
	STRING, ARRAY or RECORD.
*/

#define BASE_TYPE_LOCK_FLAG  0x100
/*
	The bit no. 4 of the ID field is the lock flag. ARRAYs and RECORDs
	locked cannot be modified.
*/


typedef enum {
	STRING_type, ARRAY_type, RECORD_type
} base_type;

typedef struct {
	base_type type; /* == STRING_type */
	int len;
	char s[0];
} STRING;


typedef struct {
	base_type type;       /* == ARRAY_type possibly with lock flag */
	int n_dyn_fields;     /* 0=no dyn; 1=address dynamic type */
	int elem_size;        /* n. of B per element */
	int allocated;        /* n. of elements allocated for 'p' */
	int n_elems;          /* n. of elements assigned */
	void *p;              /* address of the first element */
} ARRAY;


typedef struct {
	base_type type;       /* == RECORD_type possibly with lock flag */
	int n_dyn_fields;     /* n. of dyn fields */
	/* ... */             /* proper fields here - pointers first */
} RECORD;


/*
	STRING HANDLING
*/


STRING *EMPTY_STRING = (STRING *) INTSTR(\0,\0,\0,\0)INTSTR(\0,\0,\0,\0);

/*
	The compiler uses this constant (actually a variable) for every
	empty string. NIL strings are simply NULL. For strings of length
	1 see below.
*/


#define MK_ZSTRING(z, S) \
	z = alloca(S->len + 1); \
	if( z == NULL )  OOM(); \
	memcpy(z, S->s, S->len); \
	z[S->len] = 0;
/*
	This macro convert the STRING * S to a ZSTRING char *s
	allocated in the stack with alloca(). So, the string s is
	automatically released once the function terminate. Note that NUL
	characters in S cause a premature termination of the ZSTRING,
	and NULL strings cause a segmentation fault: use the function
	m2runtime_InvalidZString() to do the check these case.

	FIXME: how to check for OOM condition using alloca()?
*/


char *zstring;


#define StringToZstring(S) \
	( \
		zstring = (char * ) alloca(S->len + 1), \
		memcpy(zstring, S->s, S->len), \
		zstring[S->len] = 0, \
		zstring \
	)
/*
	This macro convert the STRING * S to a ZSTRING char *s
	allocated in the stack with alloca(). So, the string s is
	automatically released once the function terminate. Note that NUL
	characters in S cause a premature termination of the ZSTRING,
	and NULL strings cause a segmentation fault: use the function
	m2runtime_InvalidZString() to do the check these case.

	FIXME: how to check for OOM condition using alloca()?
*/


int m2runtime_length(STRING *s)
{
	if( s == NULL )  return 0;
	return s->len;
}


STRING * unconditionally_allocate_STRING(char *z, int len)
{
	STRING *s;
	s = M2_MALLOC_ATOMIC( sizeof(STRING) + len );
	s->type = STRING_type;
	s->len = len;
	memcpy(s->s, z, len);
	return s;
}


STRING *new_STRING(char *z, int len)
/*
	Allocate a STRING. The empty string is statically allocated as
	EMPTY_STRING; strings whose length is exactly 1 are allocated in a
	table once for all.
*/
{
	static STRING **chars = NULL; /* Array of single-char strings. */
	int c;

	if( len <= 0 )  return EMPTY_STRING;

	if( len == 1 ){
		if( chars == NULL ){
			chars = M2_MALLOC( 256 * sizeof(STRING*) );
			memset(chars, 0, 256 * sizeof(STRING*));
		}
		c = *(unsigned char *)z;
		if( chars[c] == NULL ){
			chars[c] = unconditionally_allocate_STRING(z, 1);
		}
		return chars[c];
	}

	return unconditionally_allocate_STRING(z, len);
}


STRING *new_STRING2(char *z)
{
	return new_STRING(z, strlen(z));
}


STRING * m2runtime_concat_STRING(int dummy, ...)
/*
	Returns the STRING resulting concatenating the given STRINGs.
	The last pointer MUST be "(STRING *)1", marking the end of the list.
	The 'dummy' argument is ignored, and it is required by va_arg().

	NOTE. If all the strings are NIL, the result must be NIL.
*/
{
	va_list ap;
	int n = 0;           // no. of strings - the compiler ensure that n >= 2
	int len = 0;         // total length of the resulting string
	int nil_count = 0;   // no. of NIL strings
	int empty_count = 0; // no. of strings = ""
	STRING *q;           // last not-NIL, not-empty string found (if len > 0)
	STRING *s, *t;

	/*
		Collect data about the strings:
	*/

	va_start(ap, dummy);
	do{
		s = va_arg(ap, STRING *);
		if( s == (STRING *) 1 )  break;
		n++;
		if( s == NULL ){
			nil_count++;
		} else if( s->len == 0 ){
			empty_count++;
		} else {
			len += s->len;
			q = s;
		}
	} while( 1 );
	va_end(ap);

	/*
		Optimize particular cases:
	*/

	if( nil_count == n )
		return NULL;

	if( len == 0 )
		return EMPTY_STRING;

	if( n - nil_count - empty_count == 1 )
		return q;

	/*
		Concatenate the strings:
	*/

	t = M2_MALLOC_ATOMIC(sizeof(STRING) + len);
	t->type = STRING_type;
	t->len = len;

	len = 0;
	va_start(ap, dummy);
	do{
		s = va_arg(ap, STRING *);
		if( s == (STRING *) 1 )  break;
		if( s != NULL && s->len > 0 ){
			memcpy(&t->s[len], s->s, s->len);
			len += s->len;
		}
	} while( 1 );
	va_end(ap);

	return t;
}


/*
	ERROR HANDLING
*/

#define m2runtime_errno()  errno


STRING *m2runtime_strerror()
{
	char *s;

	s = (char *) strerror(errno);
	return new_STRING(s, strlen(s));
}


int m2runtime_ERROR_CODE = 0;
STRING *m2runtime_ERROR_MESSAGE = NULL;

typedef void (*err_entry_get_prototype)(int i, char **m, char **f, int *l);
/*
	Every M2 module defines a function that maps an error to the
	corresponding function and line number. This is its proto.
*/


void m2runtime_HALT(err_entry_get_prototype err_entry_get, int i, STRING *msg)
/*
	FIXME: translate control chars to something readable
*/
{
	char *m, *f;
	int l;

	err_entry_get(i, &m, &f, &l);

	fflush(stdout);
	fprintf(stderr, "%s.%s(), line %d: ", m, f, l);
	if( msg == NULL )
		fwrite("HALT", 4, 1, stderr);
	else
		fwrite(msg->s, msg->len, 1, stderr);
	fprintf(stderr, "\n");
	abort();
	//exit(EXIT_FAILURE);
}


void m2runtime_RAISE_ERROR(int try_section,
	err_entry_get_prototype err_entry_get, int i, int code, STRING *msg)
/*
	FIXME: prevent the final string to become too long
*/
{
	char *m, *f;
	int l;
	char *s;

	if( code == 0 ){
		m2runtime_ERROR_CODE = 0;
		return;
	}

	err_entry_get(i, &m, &f, &l);

	if( try_section ){
		asprintf((char **) &s, "%s.%s(), line %d, code %d: ", m, f, l, code);
		msg = m2runtime_concat_STRING(0, new_STRING2(s), msg, (STRING *)1);
		m2runtime_ERROR_CODE = code;
		m2runtime_ERROR_MESSAGE = msg;
	} else {
		asprintf((char **) &s, " (code %d)", code);
		msg = m2runtime_concat_STRING(0, msg, new_STRING2(s), (STRING *)1);
		m2runtime_HALT(err_entry_get, i, msg);
	}
}


void m2runtime_missing_case_in_switch(err_entry_get_prototype err_entry_get,
	int i)
/*
	The compiler always include an implicit "default:" branch in "switch()".
	If the SWITCH statement lacks the ELSE branch, and none of the CASE
	braches matches, this function is called.
*/
{
	m2runtime_HALT(err_entry_get, i, new_STRING2("unexpected case in SWITCH"));
}


void m2runtime_missing_return(err_entry_get_prototype err_entry_get, int i)
/*
	At the end of every M2 function that should return a value, the
	compiler always adds this function, that catches the missing
	RETURN expr.
*/
{
	m2runtime_HALT(err_entry_get, i, new_STRING2("missing RETURN <expr>"));
}




/*
	EVALUATION STACK

	The evaluation stack is used only to build arrays and records with
	the {...} constructor. It is not used elsewere, and probably should be
	removed in a future, improved version of the compiler.
*/

char **es = NULL;
int sp = 0; /* stack pointer */
int m2runtime_ssize = 0; /* n. of elements allocated */


void push(char * p)
{
	if( sp >= m2runtime_ssize ){
		m2runtime_ssize = 2*m2runtime_ssize + 10;
		es = M2_REALLOC(es, m2runtime_ssize * sizeof(char *));
		if( es == NULL )  OOM();
	}
	es[sp++] = p;
}


char * pop(void)
{
	char * p;

	sp--;
	if( sp < 0 )
		die("underflow of the evaluation stack");
	/* Reset unused entries for the GC: */
	p = es[sp];
	es[sp] = NULL;  /* reset unused entry for memory recover by GC */
	return p;
}


char * tos(void)
{
	if( sp <= 0 )
		die("underflow of the evaluation stack");
	return es[sp-1];
}


char * tosn(int offset)
{
	if( sp-1-offset < 0 )
		die("underflow of the evaluation stack");
	return es[sp-1-offset];
}

/***** END of the evaluation stack functions. *****/



static ARRAY * alloc_ARRAY(
	int elem_size,     /* size of the element */
	int n_dyn_fields  /* 0=element is a simple data; 1=elem. is an addr. */
)
{
	ARRAY *a;

	a = M2_MALLOC(sizeof(ARRAY));
	a->type = ARRAY_type;
	a->n_dyn_fields = n_dyn_fields;
	a->elem_size = elem_size;
	a->allocated = 0;
	a->n_elems = 0;
	a->p = NULL;
	return a;
}


static void * element_ARRAY(ARRAY * a, int index)
/*
	Return the address of the element.
	Allocate new elements as needed.  New elements are set to zero.
	The array MUST be already allocated.
*/
{
	int new_allocated, elem_size;

	elem_size = a->elem_size;
	if( index >= a->n_elems ){
		if( index >= a->allocated ){
			/* allocate or resize the array: */
			new_allocated = a->allocated*3/2+10;
			if( new_allocated <= index )
				new_allocated = index+1;
			if( a->allocated == 0 ){
				if( a->n_dyn_fields == 0 )
					a->p = M2_MALLOC_ATOMIC(elem_size * new_allocated);
				else
					a->p = M2_MALLOC(elem_size * new_allocated);
			} else {
				a->p = M2_REALLOC(a->p, elem_size * new_allocated);
			}
			memset( (char *) a->p + elem_size * a->allocated, 0,
				elem_size * (new_allocated - a->allocated));
			a->allocated = new_allocated;
		}
		a->n_elems = index + 1;
	}

	return (void *) ((char *) a->p + index * elem_size);
}


void * m2runtime_dereference_lhs_ARRAY(
	ARRAY **ap,
	int elem_size,     /* size of the element of the ARRAY */
	int n_dyn_fields,  /* 0=the element is the data; 1=is an address */
	int index,         /* the index of the element, >= 0 */
	err_entry_get_prototype err_entry_get,
	int i
)
/*
	anArray[index] = ...

	Dereference element of array as a left-hand side.  Return its address.
	The array is allocated and expanded as necessary.
*/
{
	ARRAY *a;

	if( ap == NULL )
		m2runtime_HALT(err_entry_get, i, new_STRING2("INTERNAL ERROR (NIL array)"));

	if( *ap == NULL ){
		a = alloc_ARRAY(elem_size, n_dyn_fields);
		*ap = a;
	} else {
		a = *ap;
		if( (a->type & BASE_TYPE_ID_MASK) != ARRAY_type )
			m2runtime_HALT(err_entry_get, i,
				new_STRING2("INTERNAL ERROR (not an ARRAY)"));
		if( a->type & BASE_TYPE_LOCK_FLAG )
			m2runtime_HALT(err_entry_get, i,
				new_STRING2("writing to a locked ARRAY"));
	}

	if( index < 0 )
		m2runtime_HALT(err_entry_get, i,
			new_STRING2("array index is negative"));

	return (void *) element_ARRAY(a, index);
}


void * m2runtime_dereference_lhs_ARRAY_next(
	ARRAY **ap,
	int elem_size,     /* size of the element of the ARRAY */
	int n_dyn_fields,  /* 0=the element is the data; 1=is an address */
	err_entry_get_prototype err_entry_get,
	int i
)
/*
	anArray[] = ...

	Dereference next element of array.  Return its address.  The array
	is allocated and expanded as necessary.
*/
{
	if( ap == NULL || *ap == NULL )
		return m2runtime_dereference_lhs_ARRAY(ap, elem_size, n_dyn_fields,
			0, err_entry_get, i);
	else
		return m2runtime_dereference_lhs_ARRAY(ap, elem_size, n_dyn_fields,
			((ARRAY *) (*ap))->n_elems, err_entry_get, i);
}


static RECORD * alloc_RECORD(
	int record_size,   /* bytes */
	int n_dyn_fields   /* no. of fields that are addresses */
)
{
	RECORD *r;

	if( n_dyn_fields == 0 )
		r = M2_MALLOC_ATOMIC(record_size);
	else
		r = M2_MALLOC(record_size);
	r->type = RECORD_type;
	r->n_dyn_fields = n_dyn_fields;
	memset((char *)r + sizeof(RECORD), 0, record_size - sizeof(RECORD));
	return r;
}


void m2runtime_lock(void * x)
{
	ARRAY * a;
	RECORD * r;

	if( x == NULL )
		return;

	a = (ARRAY *) x;
	if( (a->type & BASE_TYPE_ID_MASK) == ARRAY_type ){
		a->type |= BASE_TYPE_LOCK_FLAG;
		return;
	}

	r = (RECORD *) x;
	if( (r->type & BASE_TYPE_ID_MASK) == RECORD_type ){
		r->type |= BASE_TYPE_LOCK_FLAG;
		return;
	}

	fprintf(stderr, "m2runtime.lock(): not an ARRAY, not a RECORD\n");
	abort();
}


void * m2runtime_dereference_lhs_RECORD(
	RECORD **rp,
	int record_size,   /* size of the RECORD */
	int n_dyn_fields,  /* no. of dynamic fiels (STRINGs, ARRAYs, etc.) */
	int field_offset,  /* field offset */
	err_entry_get_prototype err_entry_get,
	int i
)
/*
	aRec[aField] = ...

	Dereference the field of a RECORD as a left-hand side.
	The RECORD is allocated if necessary.
*/
{
	RECORD *r;

	if( rp == NULL )
		m2runtime_HALT(err_entry_get, i,
			new_STRING2("INTERNAL ERROR (NIL record)"));

	if( *rp == NULL ){
		r = alloc_RECORD(record_size, n_dyn_fields);
		*rp = r;
	} else {
		r = *rp;
		if( (r->type & BASE_TYPE_ID_MASK) != RECORD_type )
			m2runtime_HALT(err_entry_get, i, new_STRING2("INTERNAL ERROR (not a RECORD)"));
		if( r->type & BASE_TYPE_LOCK_FLAG )
			m2runtime_HALT(err_entry_get, i, new_STRING2("writing to a locked record"));

	}

	return (void *) ((char *) r + field_offset);
}


void * m2runtime_dereference_rhs_ARRAY(
	ARRAY *a,
	int index,
	err_entry_get_prototype err_entry_get,
	int err_i
)
/*
	Dereference the element of an ARRAY as a right-hand side value.
	Return the pointer to the element of the array, i.e. the pointer
	to BOOLEAN, INTEGER, REAL, ARRAY, RECORD, FORWARD, FUNCTION.
*/
{
	if( a == NULL )
		m2runtime_HALT(err_entry_get, err_i,
			new_STRING2("cannot dereference NIL array"));

	if( (a->type & BASE_TYPE_ID_MASK) != ARRAY_type )
		m2runtime_HALT(err_entry_get, err_i,
			new_STRING2("INTERNAL ERROR (not an ARRAY)"));

	if( index < 0 )
		m2runtime_HALT(err_entry_get, err_i,
			new_STRING2("array index is negative"));

	if( index >= a->n_elems ){
		m2runtime_HALT(err_entry_get, err_i,
			new_STRING2("array index too large"));
	}
	
	/* FIXME: forse dovrei controllare che a->p != NULL */

	char * p = a->p + index * a->elem_size;
	if( a->n_dyn_fields == 0 )
		// return the pointer to the element:
		return (void *) p;
	else
		// dereference element pointer:
		return (void *) *( (char **) p);
}


void * m2runtime_dereference_rhs_RECORD(
	RECORD *r,
	int field_offset,
	err_entry_get_prototype err_entry_get,
	int err_i
)
/*
	Dereference field of RECORD as right-hand side value.
	Return the pointer to the field of the record, i.e. the pointer
	to BOOLEAN, INTEGER, REAL, ARRAY, RECORD, FORWARD, FUNCTION.
*/
{
	char * field_addr;

	if( r == NULL )
		m2runtime_HALT(err_entry_get, err_i,
			new_STRING2("cannot dereference NIL record"));

	if( (r->type & BASE_TYPE_ID_MASK) != RECORD_type )
		m2runtime_HALT(err_entry_get, err_i, new_STRING2("INTERNAL ERROR (not a RECORD)"));

	field_addr = (void *) r + field_offset;
	if( field_offset >= 2*sizeof(int) + r->n_dyn_fields * sizeof(void *) )
		return (void *) field_addr;
	else
		return (void *)( *( (char **) field_addr) );
}


int m2runtime_eq(int a, int b)
{
	return (a && b) || (!a && !b);
}



STRING * m2runtime_CHR(int c)
{
	char s[1];

	if( c < 0 || c > 255 )
		die("CHR(%d): value out of the range [0,255]", c);
	s[0] = c;
	return new_STRING(s, 1);
}


int m2runtime_ASC(STRING *s)
{
	if( s == NULL ) die("ASC(NIL): no chars in NIL string");
	if( s->len == 0 ) die("ASC(\"\"): no chars in empty string");
	return (unsigned char) s->s[0];
}


void assign_ARRAY_OF_STRING(ARRAY **a, int i, STRING *s)
/*
	Adds the string 's' to the ARRAY OF STRING 'a'. The array is
	allocated and expanded as necessary. This function is not used
	by the compiler, but it is useful in C code.
*/
{
	*((STRING **)m2runtime_dereference_lhs_ARRAY(a, sizeof(STRING *),
		1, i, NULL, 0)) = s;
}


int m2runtime_InvalidZString(STRING *s)
{
	return (s == NULL) || (memchr(s->s, 0, s->len) != NULL);
}


int m2runtime_strcmp(STRING *a, STRING *b)
/*
	Returns:

	< 0  if a < b
	= 0  if a = b
	> 0  if a > b
*/
{
	int i, r, a_len, b_len;

	if( a == NULL ){
		if( b == NULL )  return 0;
		else return -1;
	} else if( a == EMPTY_STRING ){
		if( b == NULL )  return 1;
		else if( b == EMPTY_STRING )  return 0;
		else return -1;
	} else if( b == NULL || b == EMPTY_STRING ){
		return 1;
	} else if( a == b ){
		return 0;
	}

	/* We have 2 not NIL, not empty strings:  */

	a_len = a->len;
	b_len = b->len;

	i = ( a_len <= b_len )? a_len : b_len;

	r = (i > 0)? memcmp(a->s, b->s, i) : 0;

	if( r == 0 )
		return (a_len - b_len);
	else
		return r;
}


STRING * m2runtime_StringToLiteral(STRING * s)
/*
	We insert a terminating '\0' too, not counted in the length:
	this is not necessary for STRINGs, but it is very useful with
	printf() in C code.
*/
{
	int i, s_len, a_len;
	unsigned char *u, *sp, *ap, *ap2;
	STRING *a;

	static STRING *nil_s = NULL;

	if( s == NULL ){
		if( nil_s == NULL ){
			nil_s = new_STRING("NIL\0", 4);
			nil_s->len = 3;
		}
		return nil_s;
	}
	u = (unsigned char *) s->s;
	s_len = s->len;

	/* Guess a prudential length of the final result:  */
	a_len = 3; // two '"' + terminating '\0'
	sp = u + s_len - 1;
	for( ; sp >= u; sp--){
		if( *sp < 32 || *sp >= 127 || *sp == '\\' || *sp == '"' )
			a_len = a_len+4; // overstimed
		else
			a_len = a_len+1;
	}

	/* Allocate the final string: */
	ap = (unsigned char *) alloca(a_len);
	if( ap == NULL )
		OOM();
	
	/* Do the conversion: */
	sp = u;
	ap2 = ap;
	*ap2++ = '"';
	for( i=0; i<s_len; i++){
		if( *sp < 32 || *sp >= 127 || *sp == '\\' || *sp == '"' ){
			*ap2++ = '\\';
			switch( *sp ){
			case '\\': *ap2++ = '\\'; break;
			case '"':  *ap2++ = '"'; break;
			case '\a': *ap2++ = 'a'; break;
			case '\b': *ap2++ = 'b'; break;
			case '\n': *ap2++ = 'n'; break;
			case '\r': *ap2++ = 'r'; break;
			case '\t': *ap2++ = 't'; break;
			default:
				*ap2++ = 'x';
				*ap2++ = "0123456789ABCDEF"[*sp/16];
				*ap2++ = "0123456789ABCDEF"[*sp%16];
			}
		} else
			*ap2++ = *sp;
		sp++;
	}
	*ap2++ = '"';
	*ap2++ = '\0';
	a_len = (int)(ap2 - ap);
	a = new_STRING((char *) ap, a_len);
	a->len = a_len - 1;
	return a;
}


STRING * m2runtime_substr(STRING *s, int i, int j, int t,
	err_entry_get_prototype err_entry_get, int err_i)
/* 
	Returns a substring of 's':

		s[i]    is translated to  m2runtime_substr(s, i, <ignored>, 0)
		s[i,j]  is translated to  m2runtime_substr(s, i, j, 1)
*/
{
	if( s == NULL )
		m2runtime_HALT(err_entry_get, err_i,
			new_STRING2("substring of a NIL string"));

	if( t == 0 )
		j = i + 1;

	if( i < 0 || j > s->len || j < i ){
		if( t == 0 )
			m2runtime_HALT(err_entry_get, err_i,
				new_STRING2("invalid substring index"));
		else
			m2runtime_HALT(err_entry_get, err_i,
				new_STRING2("invalid substring range"));
	}

	return new_STRING(&s->s[i], j-i);
}



/*
	UTILITIES
*/

STRING * m2runtime_itos(int i)
{
	char b[100];

	sprintf(b, "%d", i);
	return new_STRING(b, strlen(b));
}


STRING * m2runtime_rtos(double r)
{
	char b[100];

	sprintf(b, "%.9g", r);
	return new_STRING(b, strlen(b));
}


int m2runtime_count(ARRAY *a)
{
	if( a == NULL )  return 0;
	return a->n_elems;
}


static int m2runtime_argc;
static char **m2runtime_argv;

ARRAY * m2runtime_get_args()
{
	int i;
	STRING *s;
	ARRAY *a = NULL;

	for( i=0; i<m2runtime_argc; i++ ){
		s = new_STRING((char *) m2runtime_argv[i], strlen(m2runtime_argv[i]));
		assign_ARRAY_OF_STRING(&a, i, s);
	}
	return a;
}


#define m2runtime_exit(code)  exit(code)


/* END */
