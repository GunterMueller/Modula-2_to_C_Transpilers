// IMPLEMENTATION MODULE buffer
#define M2_IMPORT_buffer

typedef struct {
	int s;  // size of the buffer m
	int n;  // number of bytes of m[] effectively used, 0<=n<=s
	int is_nil;  // an empty buffer must be interpreted as NIL string?
	char * m;  // the buffer, possibly NULL when s==0
} mybuffer;


/*
	TODO:
	- BufferPrependString(VAR b: BUFFER, s: STRING)
	- BufferAddSubString(VAR b: BUFFER, s: STRING, i: INTEGER, j: INTEGER)
*/


mybuffer * buffer_alloc_buffer(int size)
/*
	Allocate a new buffer with at least `size' bytes of space.
*/
{
	mybuffer *b;

	if( size < 0 )
		size = 0;
	b = M2_MALLOC(sizeof(mybuffer));
	b->n = 0;
	b->s = size;
	if( size <= 0 )
		b->m = NULL;
	else
		b->m = M2_MALLOC(size);
	return b;
}


void buffer_realloc_buffer(mybuffer *b, int size)
{
	if( b->s >= size )
		return;
	b->m = M2_REALLOC(b->m, size);
	b->s = size;
}


void buffer_AddChar(void **b, unsigned int c)
{
	mybuffer *bl;
	int size;

	if( *b == NULL ){
		bl = buffer_alloc_buffer(1);
		*b = (void *) bl;
	} else {
		bl = (mybuffer *) *b;
		size = bl->s;
		if( bl->n + 1 > size ){
			size = size * 2 + 1;
			buffer_realloc_buffer(bl, size);
		}
	}
	bl->m[ bl->n++ ] = c;
}


void buffer_AddString(void **b, STRING *s)
{
	mybuffer *bl;
	int len, size;

	if( s == NULL )  return;
	if( *b == NULL ){
		bl = buffer_alloc_buffer(s->len);
		*b = (void *) bl;
		bl->n = s->len;
		memcpy(bl->m, s->s, s->len);
	} else {
		bl = (mybuffer *) *b;
		len = bl->n + s->len;
		size = bl->s;
		if( len > size ){
			size = len + len/2 + 5;  // FIXME: ?
			buffer_realloc_buffer(bl, size);
		}
		memcpy(bl->m + bl->n, s->s, s->len);
		bl->n = len;
	}
	bl->is_nil = 0;
}


STRING * buffer_ToString(void *b)
{
	mybuffer *bl;

	if( b == NULL )
		return NULL;
	bl = (mybuffer *) b;
	if( bl->n == 0 ){
		if( bl->is_nil )
			return NULL;
		else
			return EMPTY_STRING;
	}
 	return new_STRING(bl->m, bl->n);
}


int buffer_Length(void *b)
{
	mybuffer *bl;

	if( b == NULL )
		return 0;
	bl = (mybuffer *) b;
	return bl->n;
}


int buffer_CharAt(STRING *s, int i)
{
	if( s == NULL || i < 0 || i > s->len )
		die("buffer.CharAt(): string NIL or index out of range");
	return (unsigned char) s->s[i];
}


void buffer_Set(void **b, STRING *s)
{
	mybuffer *bl;

	if( *b == NULL ){
		if( s == NULL )  return;
		buffer_AddString(b, s);
	} else if( s == NULL ){
		bl = *(mybuffer **)b;
		bl->n = 0;
		bl->is_nil = 1;
	} else {
		bl = *(mybuffer **)b;
		bl->n = 0;
		bl->is_nil = 0;
		buffer_AddString(b, s);
	}
}


void buffer_Reset(void *b)
{
	if( b == NULL )  return;
	((mybuffer *)b)->n = 0;
	((mybuffer *)b)->is_nil = 1;
}


void buffer_Empty(void **b)
{
	mybuffer *bl;

	if( *b == NULL ){
		bl = buffer_alloc_buffer(0);
		*b = (void *) bl;
	} else {
		bl = *(mybuffer **)b;
	}
	bl->is_nil = 0;
	bl->n = 0;
}


// END
