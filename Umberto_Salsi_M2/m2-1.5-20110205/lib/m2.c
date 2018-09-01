// IMPLEMENTATION MODULE m2
#define M2_IMPORT_m2

/*
	Copyright 2002 by Umberto Salsi <salsi@icosaedro.it>
*/


int m2_min(int a, int b)
{
	if( a <= b )
		return a;
	else
		return b;
}


int m2_max(int a, int b)
{
	if( a >= b )
		return a;
	else
		return b;
}


#define m2_abs(i)  abs(i)


void m2_print(STRING *s)
{
	if( s == NULL )  return;
	fwrite(s->s, s->len, 1, stdout);
}


void m2_error(STRING *s)
{
	if( s == NULL )  return;
	fwrite(s->s, s->len, 1, stderr);
}


STRING* m2_input()
{
	int len, c;
#define LINE_SIZE 1000
	char line[LINE_SIZE];
	char *b;
	int b_size, b_len;
	STRING * res;
	
	len = 0;

	do {
		c = getchar();
		if( c == EOF || c == '\n'){
			if( len == 0 && c == EOF )
				return NULL;
			return new_STRING((char *) line, len);
		}
		line[len] = c;
		len++;
	} while( len < LINE_SIZE );
	
	b_size = LINE_SIZE + len;
	b = M2_MALLOC_ATOMIC(b_size);
	memcpy(b, line, len);
	b_len = len;

	do {
		c = getchar();
		if( c == EOF || c == '\n'){
			res = new_STRING(b, b_len);
			M2_FREE(b);
			return res;
		}
		if( b_len >= b_size ){
			b_size = 2 * b_len;
			b = M2_REALLOC(b, b_size);
		}
		b[b_len] = c;
		b_len++;
	} while( 1 );
}


STRING *m2_readchar()
{
	int ch;
	char s[2];

	ch = getchar();
	if( ch == EOF )
		return NULL;
	else {
		s[0] = ch;
		s[1] = 0;
		return new_STRING(s, 1);
	}
}


int m2_stoi(STRING *s)
{
	char *b;

	if( s == NULL )  return 0;
	b = alloca(s->len + 1);
	memcpy(b, s->s, s->len);
	b[s->len] = 0;
	return atoi(b);
}


double m2_stor(STRING *s)
{
	char *b, *end;

	if( s == NULL )  return 0.0;
	b = alloca(s->len + 1);
	memcpy(b, s->s, s->len);
	b[s->len] = 0;
	return strtod(b, (char **)&end);
}


#define m2_itor(i) (double)(i)
#define m2_rtoi(r) (int)(r)


#define m2_inc(ip, d) *(ip)+=d


#define m2_time() time(NULL)


/****************************************************************************

	(Modified) Regular Expressions

****************************************************************************/

typedef enum {
	 circsym, dollarsym, barsym, lroundsym, rroundsym, lsquaresym,
	 rsquaresym, lbracesym, rbracesym, questionsym, asteriscsym,
	 plussym, minussym, periodsym, charsym, endsym
} resymbol;

typedef unsigned char uchar;

uchar *re_begin; /* points to the first byte of the RE */
uchar *re_end;   /* points to the the byte after the end of the RE */
uchar *s_begin;  /* points to the first byte of the RE */
uchar *s_end;    /* points to the the byte after the end of the RE */
int match_array; /* TRUE=match array, FALSE=no match array */
ARRAY **m2_a;    /* stores matching subex */
uchar *re;       /* next char to be examined */
int cc;         /* current char */
resymbol m2_sym; /* current symbol type */


void __re_err(char *fmt, ...)
// FIXME: use die() or HALT().
{
	va_list ap;
	char *s;

	fflush(stdout);
	fprintf(stderr, "\n**** M2 RUNTIME ERROR:\n");
	fprintf(stderr, "     Regular expression '"); \
	fwrite(re_begin, re_end - re_begin, 1, stderr); \
	fprintf(stderr, "',\n     at char no. %d: ", (int) (re - re_begin) ); \
	va_start(ap, fmt);
	vasprintf((char **)&s, fmt, ap);
	va_end(ap);
	fprintf(stderr, "%s\n", s);
	exit(1);
}


void next()
/*
	Get next symbol from RE
*/
{
	if( re >= re_end ){
		m2_sym = endsym;
		cc = 0;
		re++; /* per il rewind, anche se vado "fuori" */
		return;
	}
	if( *re == '\\' ){
		re++;
		if( re >= re_end )
			__re_err("expected char after `\'");
		m2_sym = charsym;
		cc = *re;
		re++;
		return;
	}
	cc = *re;
	re++;
	switch( cc ){
		case '^': m2_sym = circsym; break;
		case '$': m2_sym = dollarsym; break;
		case '|': m2_sym = barsym; break;
		case '(': m2_sym = lroundsym; break;
		case ')': m2_sym = rroundsym; break;
		case '[': m2_sym = lsquaresym; break;
		case ']': m2_sym = rsquaresym; break;
		case '{': m2_sym = lbracesym; break;
		case '}': m2_sym = rbracesym; break;
		case '?': m2_sym = questionsym; break;
		case '*': m2_sym = asteriscsym; break;
		case '+': m2_sym = plussym; break;
		case '-': m2_sym = minussym; break;
		case '.': m2_sym = periodsym; break;
		default: m2_sym = charsym; break;
	}
}


void parser_init(uchar *re_start)
/*
	Init the scanner/parser of the RE
*/
{
	re = re_start;
	next();
}


void match_repeater(int *a, int *b)
/*
	Check if there is a repeater {a,b}

	Set *a,*b accordingly. If no repeater, simply sets a=1,b=1.
*/
{
	switch( m2_sym ){

	case questionsym:
		*a = 0; *b = 1; next(); return;
	
	case plussym:
		*a = 1; *b = 9999; next(); return;
	
	case asteriscsym:
		*a = 0; *b = 9999; next(); return;
	
	case lbracesym:
		next();
		*a = 0;
		if( isdigit(cc) ){
			do {
				*a = 10*(*a) + cc - '0';
				if( *a > 9999 )
					__re_err("expected number <= 9999");
				next();
			} while( isdigit(cc) );
		}
		if( cc == ',' ){
			next();
			if( isdigit(cc) ){
				*b = 0;
				do {
					*b = 10*(*b) + cc - '0';
					if( *b > 9999 ) __re_err("expected number <= 9999");
					next();
				} while( isdigit(cc) );
			} else {
				*b = 9999;
			}
		} else {
			*b = *a;
		}
		if( cc != '}' ) __re_err("expected `}'");
		next();
		if( *a > *b ) __re_err("unordered range {%d,%d}", *a, *b);
		return;
	
	default:
		*a = 1; *b = 1; return;
	}
}


int match_continue(uchar *s, int *ks, int toplevel);

int match_set(uchar *s, int *ks)
/*
	Match a set [...]

	Entering with m2_sym == lsquaresym.
	Returns in *ks the number of bytes that matched.
	Returns 1 on success (at least a byte matched) or 0.
*/
{
	int i;
	int x;
	int a, b;
	uchar set[256];

	//printf("match_set(%s, %s)\n", s, re);

	memset(set, 0, 256);

	next();
	if( m2_sym == circsym ){
		x = 0;
		next();
	} else {
		x = 1;
	}
	do {
		if( m2_sym != charsym ) __re_err("expected literal char in set");
		a = cc;
		set[a] = 1;
		next();
		if( m2_sym == minussym ){ /* range: */
			next();
			if( m2_sym != charsym )
				__re_err("expected upper limit in range");
			b = cc;
			if( a > b )
				__re_err("reversed range [%c-%c]", a, b);
			for( i=a; i<=b; i++ )  set[i] = 1;
			next();
		}
	} while( m2_sym != rsquaresym );
	next();
	
	match_repeater(&a, &b);

	/* count how many bytes from *s are in the set: */
	i = 0;
	*ks = 0;
	while( s < s_end && set[(int)*s] == x && i < b ){
		i++;
		s++; (*ks)++;
	}

	if( i >= a )
		return TRUE;
	else
		return FALSE;
}


void match_skip_subex(int *a, int *b)
/*
	Entering with m2_sym == lroundsym.
	Search for the end of the subex then return in *a,*b the repeater.
*/
{
	int level;

	level = 0;
	do {
		if( m2_sym == endsym )
			__re_err("missing teminating subexpression");
		if( m2_sym == lroundsym )
			level++;
		else if ( m2_sym == rroundsym ){
			level--;
		}
		next();
	} while( level > 0 );
	
	match_repeater(a, b);
}


void match_skip_set()
/*
	m2_sym == lsquaresym
	Skip a set and repeater.
*/
{
	int a, b;

	do {
		next();
	} while( m2_sym != rsquaresym && m2_sym != endsym );
	if( m2_sym == endsym )
		__re_err("missing set ending `]'");
	match_repeater(&a, &b);
}
	


void match_skip_branch()
{
	int a, b;

	switch( m2_sym ){

	case lroundsym:
		match_skip_subex(&a, &b);
		return;
	
	case lsquaresym:
		match_skip_set();
		return;
	
	case periodsym:
		next();
		return;
	
	case dollarsym:
		if( re < re_end )
			__re_err("`$' allowed at the end of the RE only");
		return;
	
	case charsym:
		next();
		match_repeater(&a, &b);
		return;
	
	default:
		__re_err("illegal char %c at branch start", cc); /* cc? */
		
	}
}


int match_branch(uchar *s, int *ks, int toplevel)
/*
	Match a branch including repeater

	About the arg. 'toplevel' see function match_continue().
*/
{
	int i, c, a, b, ks2;
	uchar *sstart;
	uchar *substart; /* byte after '(' */
	uchar *subend; /* byte after the branch */

	*ks = 0;

	switch( m2_sym ){
	
	case lsquaresym:
		if( ! match_set(s, &ks2) )  return FALSE;
		s += ks2; *ks += ks2;
		return TRUE;

	case lroundsym:
		sstart = s;
		substart = re; /* points at the char after '(' */
		match_skip_subex(&a, &b);
		subend = re; /* points at the char after the branch */
		i = 0;
		while( i < b ){
			/* back to the start of the subex */
			parser_init(substart);
			if( ! match_continue(s, &ks2, 0) ) break;
			i++;
			s += ks2; *ks += ks2;
		}
		/* sets *re properly befor return */
		if( *(subend-2) == '\\' )
			parser_init(subend-2);
		else
			parser_init(subend-1);
		if( i < a )
			return FALSE;
		else {
			if( toplevel && match_array ){
				assign_ARRAY_OF_STRING(m2_a, *m2_a==NULL? 0:(*m2_a)->n_elems,
					new_STRING((char *) sstart, s-sstart));
			}
			return TRUE;
		}

	case dollarsym:
		next();
		if( m2_sym != endsym )
			__re_err("`$' allowed at the end of the RE only");
		if( s >= s_end )
			return TRUE;
		else
			return FALSE;
	
	case periodsym:
		next();
		match_repeater(&a, &b);
		i = 0;
		while( s < s_end && i < b ){
			s++; (*ks)++;
			i++;
		}
		if( i < a )
			return FALSE;
		else
			return TRUE;

	case charsym:
		c = cc;
		next();
		match_repeater(&a, &b);
		i = 0;
		while( s < s_end && *s == c && i < b ){
			s++; (*ks)++;
			i++;
		}
		if( i < a )
			return FALSE;
		else
			return TRUE;
	
	default:
		__re_err("unexpected char %c", cc==0? ' ':cc);
	}
	return 0; /* per far contento gcc -Wall */
}


int match_continue(uchar *s, int *ks, int toplevel)
/*
	Matches zero or more branches [ branch { ["|"] branch } ]

	toplevel=1 if we are at subex of level 0. If 0 we are inside a subex
	so a ')' is expected at the end.

	Returns *ks with the no. of bytes consumed.
	Returns 1 on success, 0 on failed match, -1 on syntax err.
*/
{
	int res, ks2;

	*ks = 0;
	do {
		if( m2_sym == endsym || (toplevel == 0 && m2_sym == rroundsym ) )
			return TRUE;
		res = match_branch(s, &ks2, toplevel);
		s += ks2; *ks += ks2;

		/*
			res==1: current branch matched: skip alternatives ...|a|b
			res==0: match failed: try alternatives
		*/
		if( res == 1 ){
			while( m2_sym == barsym ){
				next();
				match_skip_branch();
			}
		} else {
			if( m2_sym == barsym ){
				next();
			} else {
				return FALSE;
			}
		}
	}while( 1 );
	return TRUE;
}


int __match(uchar *_s_begin, uchar *_s_end, uchar *_re_begin, uchar* _re_end)
/*
	Matches '_s_begin' vs. the RE 're' beginning from *_s_begin

	Returns 1 on success, 0 on failed match, -1 on syntax err.

	WARNING!
	*_s_end must be the byte AFTER the last byte of the string.
	*_re_end must be the byte AFTER the last byte of the RE.
*/
{
	uchar *s, *re;
	int ks;

	re_begin = _re_begin;
	re = _re_begin;
	re_end = _re_end;
	s_begin = _s_begin;
	s = _s_begin;
	s_end = _s_end;
	parser_init(re);
	if( m2_sym == circsym ){
		next();
		return match_continue(s, &ks, 1);
	} else {
		/* looking for the first match of *re in s: */
		while( s < s_end ){
			if( match_continue(s, &ks, 1) )  return 1;
			s++;
			parser_init(re);
		}
		return FALSE;
	}
	return 0; /* make happy gcc -Wall */
}


int m2_match(STRING *s, STRING *re)
{
	int res;

	if( s == NULL || s->s == NULL )  return FALSE;
	if( re == NULL || re->s == NULL )  return FALSE;
	match_array = 0;
	m2_a = NULL;
	res = __match((uchar *) s->s, (uchar *) (s->s + s->len), (uchar *) re->s, (uchar *) (re->s + re->len));
	return res;
}


int m2_match_array(STRING *s, STRING *re, ARRAY **a)
{
	int res;

	if( s == NULL || s->s == NULL )  return FALSE;
	if( re == NULL || re->s == NULL )  return FALSE;
	match_array = 1;
	*a = NULL;
	m2_a = a;
	res = __match((uchar *) s->s, (uchar *) (s->s + s->len), (uchar *) re->s, (uchar *) (re->s + re->len));
	return (*a == NULL)? 0:(*m2_a)->n_elems;
}
