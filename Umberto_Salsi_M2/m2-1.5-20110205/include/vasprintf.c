#include <stdio.h>

int vasprintf(char **strp, const char *fmt, va_list ap)
{
	char *buf;
	int buf_s, buf_n;

	buf_s = 100;
	buf = malloc(buf_s);
	if( buf == NULL )
		exit(1);
	while( 1 ){
		buf_n = vsnprintf(buf, buf_s, fmt, ap);
		if( buf_n < buf_s ){
			*strp = buf;
			return buf_n;
		}
		buf_s = buf_s * 2;
		buf = realloc(buf, buf_s);
		if( buf == NULL )
			exit(1);
	}
}
