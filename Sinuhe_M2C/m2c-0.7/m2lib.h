#ifndef M2LIB_H
#define M2LIB_H

#include <stdint.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

typedef unsigned int Tset;  /* Must match the compiler */

/* m2l_arrpar.c */
extern void m2_arrpar (unsigned char **adr, unsigned int high, unsigned int elong);
/* ml2_assarr.c */
extern void m2_assarr (char *var, char *expr, unsigned int l);
/* m2l_assstr.c */
extern void m2_assstr (char *var, char *str, unsigned int vlong, unsigned int strlong);
/* m2l_cap.c */
extern int m2_cap (unsigned int ch);
/* m2l_cor.c */
extern void m2_newprocess (int (*f) (), int *adr, unsigned int clong, int **cor);
extern void m2_transfer (int **cor1, int **cor2);
/* m2l_eq.c */
extern int m2_eq (char *str1, char *str2, unsigned int l);
/* m2l_halt.c */
extern void m2_halt (void);
/* m2l_ne.c */
extern int m2_ne (char *str1, char *str2, unsigned int l);
/* m2l_nites.c */
extern int m2_nites (Tset s1, Tset s2);
/* m2l_rngovf.c */
extern void m2_rngovf (void);
/* m2l_setin.c */
extern int m2_setin (Tset s1, Tset s2);
/* m2l_testptr.c */
extern char *m2_testptr (char *ptr);

#endif  // M2LIB_H
