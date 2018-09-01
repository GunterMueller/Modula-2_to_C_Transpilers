/* By Milos Radovanovic, 2000, 2001 */

#ifndef _m2sys_h_
#define _m2sys_h_

#include <limits.h> // INT_MIN, INT_MAX...

  /* numeric_limits<> wouldn't be better because it's a
     *function*, therefore doesn't return true constants
     (at least with the compilers I used), so it can't
     be passed as a template parameter */

#include <float.h>  // FLT_MIN, FLT_MAX...
#include <stdlib.h> // exit()

// using namespace std;


/**************************************************************************
 * Modula-2 built-in types                                                *
 **************************************************************************/
typedef /*unsigned*/ char CHAR; // needs to be compiled with default char unsigned option
typedef int           INTEGER;
typedef unsigned      CARDINAL;
typedef bool          BOOLEAN;
typedef float         REAL;
typedef double        LONGREAL;

// BEGIN extensions
typedef long           LONGINT;
typedef unsigned long  LONGCARD;
typedef short          SHORTINT;
typedef unsigned short SHORTCARD;
// END extensions

typedef void (* PROC)(void);


/**************************************************************************
 * Stuff from Modula-2 SYSTEM module. It is not a module here             *
 **************************************************************************/
// namespace SYSTEM {

typedef /*unsigned*/ char LOC;
typedef unsigned      WORD;
typedef /*unsigned*/ char BYTE;

#define BITSPERLOC    CHAR_BIT
#define LOCSPERWORD   sizeof(WORD)
#define LOCSPERBYTE   1

typedef void* _VoidP;
typedef LOC*  _LOCP;


/**************************************************************************
 * Modula-2 SYSTEM.ADDRESS type. Compatible with void* and POINTER<...>   *
 **************************************************************************/
class ADDRESS {
protected:
  _VoidP p;
public:
  ADDRESS() {}
  ADDRESS(_VoidP vp) { p = vp; }
  //ADDRESS& operator= (_VoidP vp) { p = vp; return *this; }
  operator _VoidP() const { return p; }

  friend _VoidP  ADDADR(ADDRESS a, CARDINAL o);
  friend _VoidP  SUBADR(ADDRESS a, CARDINAL o);
  friend INTEGER DIFADR(ADDRESS a1, ADDRESS a2);
};


/**************************************************************************
 * Operations with ADDRESS type                                           *
 **************************************************************************/
inline _VoidP  ADDADR(ADDRESS a, CARDINAL o)  { return _VoidP(_LOCP(a.p)+o); }
inline _VoidP  SUBADR(ADDRESS a, CARDINAL o)  { return _VoidP(_LOCP(a.p)-o); }
inline INTEGER DIFADR(ADDRESS a1, ADDRESS a2) { return INTEGER(_LOCP(a1.p)-_LOCP(a2.p)); }

#define ADR(x)         _VoidP(&(x))

//#define MAKEADR // ToDo


/**************************************************************************
 * Modula-2 POINTER type (not from SYSTEM, but nice to be next to ADDRESS)*
 **************************************************************************/
template <class T> class POINTER : public ADDRESS {
protected:
  typedef T* _TP;
public:
  POINTER() : ADDRESS() {}
  POINTER(_VoidP vp) : ADDRESS(vp) {}
  POINTER& operator= (_VoidP vp) { p = vp; return *this; }
  POINTER& operator= (_TP tp)    { p = _VoidP(tp); return *this; }
  T&       operator* ()          { return *_TP(p); }
  //operator _TP() const { return (_TP)p; }
};

#define SHIFT(ps,n)  (ps).shift(n)
#define ROTATE(ps,n) (ps).rotate(n)

#define CAST(T,v)     (T(v))
// or                 static_cast<T>(i)

#define TSIZE(T)       CARDINAL(sizeof(T))

// } /* SYSTEM */


/**************************************************************************
 * Modula-2 NEW procedure. Does NOT call Storage.ALLOCATE                 *
 **************************************************************************/
template <class T>
void NEW(POINTER<T>& p) {
  p = new T;
}


/**************************************************************************
 * Modula-2 DISPOSE procedure. Does NOT call Storage.DEALLOCATE           *
 **************************************************************************/
template <class T>
void DISPOSE(POINTER<T>& p) {
  delete (T*)_VoidP(p); // works!
}


/**************************************************************************
 * TRUE and FALSE                                                         *
 **************************************************************************/
#define TRUE      true
#define FALSE     false


/**************************************************************************
 * Modula-2 subrange type. Compatible with the type it's taken from       *
 **************************************************************************/
template <class RangeType, RangeType _from, RangeType _to>
class _SubrangeType {
private:
  RangeType val;
public:
  static const RangeType from = _from;
  static const RangeType to   = _to;

  _SubrangeType() {} // has to be specified to satisfy DJGPP

  _SubrangeType(RangeType c) { val = c; }
  _SubrangeType& operator= (RangeType c) { val = c; return *this; }
  // ToDo: range checking, when exceptions are implemented

  operator RangeType() const { return val; }
};


/**************************************************************************
 * Modula-2 MIN and MAX procedures                                        *
 **************************************************************************/
#define MIN(U) (_MinMax<U>::T::from)
#define MAX(U) (_MinMax<U>::T::to)

// This is used when U is a _SubrangeType<...>
template <class U> struct _MinMax {
  typedef U T;
};

template<> struct _MinMax <CHAR> {
  typedef _SubrangeType<CHAR, 0, UCHAR_MAX> T;
};
template<> struct _MinMax <INTEGER> {
  typedef _SubrangeType<INTEGER, INT_MIN, INT_MAX> T;
};
template<> struct _MinMax <CARDINAL> {
  typedef _SubrangeType<CARDINAL, 0, UINT_MAX> T;
};
template<> struct _MinMax <BOOLEAN> {
  typedef _SubrangeType<BOOLEAN, FALSE, TRUE> T;
};

// BEGIN extensions
template<> struct _MinMax <LONGINT> {
  typedef _SubrangeType<LONGINT, LONG_MIN, LONG_MAX> T;
};
template<> struct _MinMax <LONGCARD> {
  typedef _SubrangeType<LONGCARD, 0, ULONG_MAX> T;
};
template<> struct _MinMax <SHORTINT> {
  typedef _SubrangeType<SHORTINT, SHRT_MIN, SHRT_MAX> T;
};
template<> struct _MinMax <SHORTCARD> {
  typedef _SubrangeType<SHORTCARD, 0, USHRT_MAX> T;
};
// END extensions

// Some "constants" in float.h are not really constants in DJGPP 2.95.2,
// so they cannot be used where true constants are required. Furthermore,
// those "constants" cannot be used to initialize static const class members.
// If necessary, change the following values to match your g++ implementation.
// (These values are for DJGPP 2.95.2 under DOS)

// DJGPP note: the comment above would make sense if the linker would not
// complain about undefined references to `_MinMax<float>::T::from' and the
// like. I guess this is a DJGPP bug. There is no way I could think of to
// initialize `from' and `to' here...

#ifdef __DJGPP__
/*
template<> struct _MinMax <REAL> {
  struct T {
    static REAL from;
    static REAL to;
  };
  _MinMax() { T::from = 1.175494E-38; T::to = 3.402823E+38; }
} _djgppTrickREAL;
template<> struct _MinMax <LONGREAL> {
  struct T {
    static LONGREAL from;
    static LONGREAL to;
  };
  _MinMax() { T::from = 2.225074E-308; T::to = 1.797693E+308; }
} _djgppTrickLONGREAL;
*/
#else
template<> struct _MinMax <REAL> {
  struct T {
    static REAL from = FLT_MIN;
    static REAL to   = FLT_MAX;
  };
};
template<> struct _MinMax <LONGREAL> {
  struct T {
    static LONGREAL from = DBL_MIN;
    static LONGREAL to   = DBL_MAX;
  };
};
#endif


/**************************************************************************
 * Some other Modula-2 built in procedures                                *
 **************************************************************************/
inline CHAR CAP(CHAR c) { return ((c>='a')&&(c<='z')) ? c-'a'+'A' : c; }

template <class T> inline T       ABS(T x) { return ( (x<0) ? -x : x ); }
template <class T> inline CHAR    CHR(T x) { return CHAR(x); }
template <class T> inline BOOLEAN ODD(T x) { return (x%2 == 1); }

template <class T> inline REAL      FLOAT(T x)   { return REAL(x); }
template <class T> inline LONGREAL LFLOAT(T x)   { return LONGREAL(x); }

template <class T> inline void INC(T& x)             { x = x + T(1); } // This way it works with _SubrangeType
template <class T> inline void DEC(T& x)             { x = x - T(1); }
template <class T> inline void INC(T& x, CARDINAL k) { x = x + T(k); }
template <class T> inline void DEC(T& x, CARDINAL k) { x = x - T(k); }

template <class T> inline LONGINT  INT  (T x)      { return LONGINT(x); }
template <class T> inline LONGINT  TRUNC(T x)      { return LONGINT(x); }

#define ORD(x)    CARDINAL(x)
#define VAL(T, x) T(x)

#define SIZE(x)   CARDINAL(sizeof(x))


/**************************************************************************
 * _SINGLEOF is analogue to _ARRAYOF, only it's used to store single      *
 * elements. It's purpose is to refer to procedure parameters of type     *
 * LOC, BYTE or WORD, which are template in C++                           *
 **************************************************************************/
template <class ComponentType>
class _SINGLEOF {
private:
  ComponentType* p;
public:
  _SINGLEOF(_VoidP where) {
    p = (ComponentType*)where;
  }
  operator ComponentType() const { return *p; }
};


/**************************************************************************
 * Modula-2 open array type. Compatible with all "closed" arrays of       *
 * the same ComponentType. Also used for referring to procedure parameters*
 * of type ARRAY OF {ARRAY OF} LOC, BYTE or WORD                          *
 **************************************************************************/
template <class ComponentType>
class _ARRAYOF {
protected:
  CARDINAL h;
  ComponentType* p;
  BOOLEAN doDelete;
  static const CARDINAL oneIfCHAR = 0;

public:
  _ARRAYOF() { h = 0; p = 0; doDelete = FALSE; }

  /* This is used for referring to template parameters */
  _ARRAYOF(_VoidP where, CARDINAL size) {
    size == 0 ? h = 0 : h = size - 1; // size == 0 should never happen
    p = (ComponentType*)where;
    doDelete = FALSE;
  }

  _ARRAYOF(const _ARRAYOF& a);

  ~_ARRAYOF() {
    if (doDelete) {
      delete[] p;
      p = 0;
    }
  }

  ComponentType& operator[] (CARDINAL i) { return p[i]; } // ToDo: range checking

  CARDINAL high() const { return h; }
};

template <class ComponentType>
_ARRAYOF<ComponentType>::_ARRAYOF(const _ARRAYOF& a) {
  h = a.h;
  p = new ComponentType[h+1];
  for (CARDINAL i = 0; i <= h; INC(i)) p[i] = a.p[i];
  doDelete = TRUE;
}


/**************************************************************************
 * ARRAY OF CHAR is a special case (string)                               *
 **************************************************************************/
template<> class _ARRAYOF<CHAR> {
protected:
  CARDINAL h;
  CHAR* p;
  BOOLEAN doDelete;

  static const CARDINAL oneIfCHAR = 1;
    // for use of _CSTR with ARRAY (guarantees a '\0' after every string)
    // this way I don't have to use RTTI

public:
  _ARRAYOF() { h = 0; p = 0; doDelete = FALSE; }

  _ARRAYOF(_VoidP where, CARDINAL size) { // zbog ARRAY OF LOC
    size == 0 ? h = 0 : h = size - 1; // size == 0 should never happen
    p = (CHAR*)where;
    doDelete = FALSE;
  }

  _ARRAYOF(const char*);
  _ARRAYOF(const _ARRAYOF&);

  _ARRAYOF(CHAR ch) {
    h = 0;
    p = new CHAR[2];
    p[0] = ch;
    p[1] = '\0';
    doDelete = TRUE;
  }

  ~_ARRAYOF() {
    if (doDelete) {
      delete[] p;
      p = 0;
    }
  }

  CHAR& operator[] (CARDINAL i) { return p[i]; } // ToDo: range checking

  CARDINAL high() const { return h; }

  friend char* _CSTR(const _ARRAYOF<CHAR>&);
};

_ARRAYOF<CHAR>::_ARRAYOF(const char* s) {
  CARDINAL i;
  h = 0;
  while (s[h] != '\0') INC(h);
  p = new CHAR[h+1];
  for (i = 0; i <= h; INC(i)) p[i] = (CHAR)s[i];
  doDelete = TRUE;
}

_ARRAYOF<CHAR>::_ARRAYOF(const _ARRAYOF<CHAR>& a) {
  h = a.h;
  p = new CHAR[h+1];
  for (CARDINAL i = 0; i <= h; INC(i)) p[i] = a.p[i];
  doDelete = TRUE;
}


/**************************************************************************
 * HIGH procedure                                                         *
 **************************************************************************/
#define HIGH(a) ((a).high())


/**************************************************************************
 * Operations with strings                                                *
 **************************************************************************/
CARDINAL LENGTH(_ARRAYOF<CHAR> s) {
  CARDINAL l = 0;
  while ((l <= HIGH(s)) && (s[l] != '\0')) INC(l);
  return l;
}

inline char* _CSTR(const _ARRAYOF<CHAR>& s) {
  return (char*)s.p;
}


/**************************************************************************
 * Modula-2 ARRAY                                                         *
 **************************************************************************/
template <class IndexType, class ComponentType>
class ARRAY : public _ARRAYOF<ComponentType> {
public:
  ARRAY() {
    h = MAX(IndexType) - MIN(IndexType);
    p = new ComponentType[h+1+oneIfCHAR]; // one more for '\0' if ComponentType is CHAR
    if (oneIfCHAR == 1) p[h+1] = ComponentType('\0');
    doDelete = TRUE;
  }

  ARRAY(const ComponentType* s);
  ARRAY(const ARRAY& a);

  ARRAY(ComponentType c) { // ComponentType is CHAR
    h = MAX(IndexType) - MIN(IndexType);
    p = new ComponentType[h+1+oneIfCHAR]; // should always be oneIfCHAR == 1
    p[0] = c;
    if (h > 0) p[1] = ComponentType('\0');
    if (oneIfCHAR == 1) p[h+1] = ComponentType('\0');
    doDelete = TRUE;
  }

  ~ARRAY() {
    if (doDelete) {
      delete[] p;
      p = 0;
    }
  }

  ComponentType& operator[] (IndexType i) { return p[i-MIN(IndexType)]; }
  // ToDo: range checking

  // extension
  IndexType high() const { return MAX(IndexType); }

  ARRAY& operator= (const ARRAY&);
  ARRAY& operator= (const ComponentType*);
  ARRAY& operator= (ComponentType c) { // ComponentType is CHAR
    p[0] = c;
    if (oneIfCHAR == 1) p[1] = ComponentType('\0');
  }
};

template <class IndexType, class ComponentType>
ARRAY<IndexType, ComponentType>::ARRAY(const ComponentType* s) {
  IndexType i = MIN(IndexType);
  h = MAX(IndexType) - MIN(IndexType);
  p = new ComponentType[h+1+oneIfCHAR]; // should always be oneIfCHAR == 1
  while ((i <= MAX(IndexType)) && (ComponentType('\0') != (*s))) {
    p[i-MIN(IndexType)] = (*s);
    INC(i);
    s++;
  }
  if (i <= MAX(IndexType)) {
    p[i-MIN(IndexType)] = ComponentType('\0');
  }
  /*if (oneIfCHAR == 1)*/ p[h+1] = ComponentType('\0');
  doDelete = TRUE;
}
// ToDo: range checking

template <class IndexType, class ComponentType>
ARRAY<IndexType, ComponentType>::ARRAY(const ARRAY<IndexType, ComponentType>& a) {
  h = a.h;
  p = new ComponentType[h+1+oneIfCHAR];
  for (CARDINAL i = 0; i <= h+oneIfCHAR; INC(i)) p[i] = a.p[i];
  doDelete = TRUE;
}

template <class IndexType, class ComponentType>
ARRAY<IndexType, ComponentType>& ARRAY<IndexType, ComponentType>::operator= (const ARRAY<IndexType, ComponentType>& a) {
  for (CARDINAL i = 0; i <= h; INC(i)) p[i] = a.p[i];
  return *this;
}

template <class IndexType, class ComponentType>
ARRAY<IndexType, ComponentType>& ARRAY<IndexType, ComponentType>::operator= (const ComponentType* s) { // samo zbog stringova (ComponentType = CHAR)
  IndexType i = MIN(IndexType);
  while ((i <= MAX(IndexType)) && ((*s) != ComponentType('\0'))) {
    p[i-MIN(IndexType)] = (*s);
    INC(i);
    s++;
  }
  if (i <= MAX(IndexType)) {
    p[i-MIN(IndexType)] = ComponentType('\0');
  }
  return *this;
}
// ToDo: range checking (in M2 it's done at compile time)


/**************************************************************************
 * Some internally used operations                                        *
 **************************************************************************/
template <class T>
inline _VoidP _DATAADR(_ARRAYOF<T>& a) { return &(a[0]); }

template <class T, class U>
inline _VoidP _DATAADR(ARRAY<T, U>& a) { return &(a[MIN(T)]); }

template <class T>
inline _VoidP _DATAADR(T& a) { return &a; }

template <class T>
inline CARDINAL _DATASIZE(_ARRAYOF<T>& a) { return HIGH(a)+1; }

template <class T, class U>
inline CARDINAL _DATASIZE(ARRAY<T, U>& a) { return CARDINAL(MAX(T)-MIN(T)+1); }

template <class T>
inline CARDINAL _DATASIZE(T& a) { return sizeof(a); }

template <class T>
inline CARDINAL _DATATSIZE_fun() {
  T a;
  return _DATASIZE(a);
}

#define _DATATSIZE(T) _DATATSIZE_fun<T>()


/**************************************************************************
 * Modula-2 PACKEDSET type                                                *
 **************************************************************************/
template <class BaseType>
class PACKEDSET {
private:
  unsigned char b[(MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1];
public:
  PACKEDSET();
  PACKEDSET(CHAR);

  PACKEDSET<BaseType> operator+  (const PACKEDSET<BaseType>&);
  PACKEDSET<BaseType> operator*  (const PACKEDSET<BaseType>&);
  PACKEDSET<BaseType> operator-  (const PACKEDSET<BaseType>&);
  PACKEDSET<BaseType> operator/  (const PACKEDSET<BaseType>&);

  BOOLEAN             operator== (const PACKEDSET<BaseType>&);
  BOOLEAN             operator!= (const PACKEDSET<BaseType>&);
  BOOLEAN             operator<= (const PACKEDSET<BaseType>&);
  BOOLEAN             operator>= (const PACKEDSET<BaseType>&);

  BOOLEAN in(BaseType);

  PACKEDSET<BaseType>& incl(BaseType);
  PACKEDSET<BaseType>& incl(BaseType, BaseType);
  PACKEDSET<BaseType>& excl(BaseType);
  PACKEDSET<BaseType>& excl(BaseType, BaseType);
/*
  PACKEDSET<BaseType>& shift (INTEGER); // ToDo
  PACKEDSET<BaseType>& rotate(INTEGER); // ToDo
*/
};

template <class BaseType>
PACKEDSET<BaseType>::PACKEDSET() {
  for (CARDINAL i = 0;
       i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1;
       INC(i)) {
    b[i] = 0;
  }
}

template <class BaseType>
PACKEDSET<BaseType>::PACKEDSET(CHAR ch) {
  b[0] = (unsigned char)ch;
  for (CARDINAL i = 1;
       i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1;
       INC(i)) {
    b[i] = 0;
  }
}

template <class BaseType>
PACKEDSET<BaseType> PACKEDSET<BaseType>::operator+ (const PACKEDSET<BaseType>& ps) {
  PACKEDSET<BaseType> res;
  for (CARDINAL i = 0;
       i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1;
       INC(i)) {
    res.b[i] = b[i] | ps.b[i];
  }
  return res;
}
template <class BaseType>
PACKEDSET<BaseType> PACKEDSET<BaseType>::operator* (const PACKEDSET<BaseType>& ps) {
  PACKEDSET<BaseType> res;
  for (CARDINAL i = 0;
       i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1;
       INC(i)) {
    res.b[i] = b[i] & ps.b[i];
  }
  return res;
}
template <class BaseType>
PACKEDSET<BaseType> PACKEDSET<BaseType>::operator- (const PACKEDSET<BaseType>& ps) {
  PACKEDSET<BaseType> res;
  for (CARDINAL i = 0;
       i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1;
       INC(i)) {
    res.b[i] = b[i] & ~ps.b[i];
  }
  return res;
}
template <class BaseType>
PACKEDSET<BaseType> PACKEDSET<BaseType>::operator/ (const PACKEDSET<BaseType>& ps) {
  PACKEDSET<BaseType> res;
  for (CARDINAL i = 0;
       i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1;
       INC(i)) {
    res.b[i] = b[i] ^ ps.b[i];
  }
  return res;
}

template <class BaseType>
BOOLEAN PACKEDSET<BaseType>::operator== (const PACKEDSET<BaseType>& ps) {
  BOOLEAN stillEq = TRUE;
  CARDINAL i = 0;
  while ((i < (MAX(BaseType) - MIN(BaseType))/CHAR_BIT + 1) && stillEq) {
    stillEq = b[i] == ps.b[i];
    INC(i);
  }
  return stillEq;
}
template <class BaseType>
inline BOOLEAN PACKEDSET<BaseType>::operator!= (const PACKEDSET<BaseType>& ps) {
  return !((*this) == ps);
}
template <class BaseType>
inline BOOLEAN PACKEDSET<BaseType>::operator<= (const PACKEDSET<BaseType>& ps) {
  return (*this) + ps == ps;
}
template <class BaseType>
inline BOOLEAN PACKEDSET<BaseType>::operator>= (const PACKEDSET<BaseType>& ps) {
  return (*this) + ps == (*this);
}

template <class BaseType>
BOOLEAN PACKEDSET<BaseType>::in(BaseType i) {
  return (( b[(i-MIN(BaseType)) / CHAR_BIT] >> ((i-MIN(BaseType)) % CHAR_BIT) ) & 1) != 0;
}

template <class BaseType>
PACKEDSET<BaseType>& PACKEDSET<BaseType>::incl(BaseType i) {
  b[(i-MIN(BaseType)) / CHAR_BIT] |= (1 << ((i-MIN(BaseType)) % CHAR_BIT));
  return *this;
}
template <class BaseType>
PACKEDSET<BaseType>& PACKEDSET<BaseType>::incl(BaseType i1, BaseType i2) {
  for (BaseType i = i1; i <= i2; INC(i)) incl(i);
  return *this;
}
template <class BaseType>
PACKEDSET<BaseType>& PACKEDSET<BaseType>::excl(BaseType i) {
  b[(i-MIN(BaseType)) / CHAR_BIT] &= ~(1 << ((i-MIN(BaseType)) % CHAR_BIT));
  return *this;
}
template <class BaseType>
PACKEDSET<BaseType>& PACKEDSET<BaseType>::excl(BaseType i1, BaseType i2) {
  for (BaseType i = i1; i <= i2; INC(i)) excl(i);
  return *this;
}


/**************************************************************************
 * Operations with PACKEDSET type                                         *
 **************************************************************************/
#define IN(i,ps)   (ps).in(i)
#define INCL(ps,i) (ps).incl(i)
#define EXCL(ps,i) (ps).excl(i)


/**************************************************************************
 * Modula-2 BITSET type                                                   *
 **************************************************************************/
typedef PACKEDSET<_SubrangeType<CARDINAL, 0, sizeof(CARDINAL)*CHAR_BIT-1> > BITSET;


/**************************************************************************
 * Modula-2 SET type (the same as PACKEDSET)                              *
 **************************************************************************/
#define SET PACKEDSET


/**************************************************************************
 * Modula-2 COMPLEX and LONGCOMPLEX types                                 *
 **************************************************************************/
class LONGCOMPLEX;

class COMPLEX {
private:
  REAL re, im;

public:
  COMPLEX();
  COMPLEX(REAL, REAL);
  COMPLEX(const COMPLEX&);
  COMPLEX(const LONGCOMPLEX&);

  friend COMPLEX operator+(const COMPLEX&, const COMPLEX&);
  friend COMPLEX operator-(const COMPLEX&, const COMPLEX&);
  friend COMPLEX operator*(const COMPLEX&, const COMPLEX&);
  friend COMPLEX operator/(const COMPLEX&, const COMPLEX&);
  friend COMPLEX operator-(const COMPLEX&);

  friend REAL RE(const COMPLEX&);
  friend REAL IM(const COMPLEX&);
};

class LONGCOMPLEX {
private:
  LONGREAL re, im;

public:
  LONGCOMPLEX();
  LONGCOMPLEX(LONGREAL, LONGREAL);
  LONGCOMPLEX(const LONGCOMPLEX&);
  LONGCOMPLEX(const COMPLEX&);

  friend LONGCOMPLEX operator+(const LONGCOMPLEX&, const LONGCOMPLEX&);
  friend LONGCOMPLEX operator-(const LONGCOMPLEX&, const LONGCOMPLEX&);
  friend LONGCOMPLEX operator*(const LONGCOMPLEX&, const LONGCOMPLEX&);
  friend LONGCOMPLEX operator/(const LONGCOMPLEX&, const LONGCOMPLEX&);
  friend LONGCOMPLEX operator-(const LONGCOMPLEX&);

  friend LONGREAL RE(const LONGCOMPLEX&);
  friend LONGREAL IM(const LONGCOMPLEX&);
};

inline REAL     RE(const COMPLEX& c)     { return c.re; }
inline REAL     IM(const COMPLEX& c)     { return c.im; }
inline LONGREAL RE(const LONGCOMPLEX& c) { return c.re; }
inline LONGREAL IM(const LONGCOMPLEX& c) { return c.im; }

inline COMPLEX::COMPLEX()                      {}
inline COMPLEX::COMPLEX(REAL r, REAL i)        { re = r;      im = i;      }
inline COMPLEX::COMPLEX(const COMPLEX& c)      { re = RE(c);  im = IM(c);  }
inline COMPLEX::COMPLEX(const LONGCOMPLEX& lc) { re = RE(lc); im = IM(lc); }

inline COMPLEX operator+(const COMPLEX& c1, const COMPLEX& c2) {
  COMPLEX res;
  res.re = RE(c1) + RE(c2);
  res.im = IM(c1) + IM(c2);
  return res;
}

inline COMPLEX operator-(const COMPLEX& c1, const COMPLEX& c2) {
  COMPLEX res;
  res.re = RE(c1) - RE(c2);
  res.im = IM(c1) - IM(c2);
  return res;
}

inline COMPLEX operator*(const COMPLEX& c1, const COMPLEX& c2) {
  COMPLEX res;
  res.re = RE(c1) * RE(c2) - IM(c1) * IM(c2);
  res.im = RE(c1) * IM(c2) + IM(c1) * RE(c2);
  return res;
}

inline COMPLEX operator/(const COMPLEX& c1, const COMPLEX& c2) {
  COMPLEX res;
  REAL divisor = RE(c2) * RE(c2) + IM(c2) * IM(c2);
  res.re = (RE(c1) * RE(c2) + IM(c1) * IM(c2)) / divisor;
  res.im = (IM(c1) * RE(c2) + RE(c1) * IM(c2)) / divisor;
  return res;
} // ToDo: range checking

inline COMPLEX operator-(const COMPLEX& c) {
  COMPLEX res;
  res.re = -RE(c);
  res.im = -IM(c);
  return res;
}

inline LONGCOMPLEX::LONGCOMPLEX()                       {}
inline LONGCOMPLEX::LONGCOMPLEX(LONGREAL r, LONGREAL i) { re = r;      im = i;      }
inline LONGCOMPLEX::LONGCOMPLEX(const LONGCOMPLEX& lc)  { re = RE(lc); im = IM(lc); }
inline LONGCOMPLEX::LONGCOMPLEX(const COMPLEX& c)       { re = RE(c);  im = IM(c);  }

inline LONGCOMPLEX operator+(const LONGCOMPLEX& c1, const LONGCOMPLEX& c2) {
  LONGCOMPLEX res;
  res.re = RE(c1) + RE(c2);
  res.im = IM(c1) + IM(c2);
  return res;
}

inline LONGCOMPLEX operator-(const LONGCOMPLEX& c1, const LONGCOMPLEX& c2) {
  LONGCOMPLEX res;
  res.re = RE(c1) - RE(c2);
  res.im = IM(c1) - IM(c2);
  return res;
}

inline LONGCOMPLEX operator*(const LONGCOMPLEX& c1, const LONGCOMPLEX& c2) {
  LONGCOMPLEX res;
  res.re = RE(c1) * RE(c2) - IM(c1) * IM(c2);
  res.im = RE(c1) * IM(c2) + IM(c1) * RE(c2);
  return res;
}

inline LONGCOMPLEX operator/(const LONGCOMPLEX& c1, const LONGCOMPLEX& c2) {
  LONGCOMPLEX res;
  LONGREAL divisor = RE(c2) * RE(c2) + IM(c2) * IM(c2);
  res.re = (RE(c1) * RE(c2) + IM(c1) * IM(c2)) / divisor;
  res.im = (IM(c1) * RE(c2) + RE(c1) * IM(c2)) / divisor;
  return res;
} // ToDo: range checking

inline LONGCOMPLEX operator-(const LONGCOMPLEX& c) {
  LONGCOMPLEX res;
  res.re = -RE(c);
  res.im = -IM(c);
  return res;
}


/**************************************************************************
 * Modula-2 CMPLX constructor                                             *
 **************************************************************************/
#define CMPLX(r1,r2) COMPLEX(r1,r2)


/**************************************************************************
 * Modula-2 HALT procedure                                                *
 **************************************************************************/
inline void HALT() { exit(0); }


/**************************************************************************
 * Modula-2 FOR statement: FOR i := e1 TO e2 BY step DO                   *
 **************************************************************************/
#define FOR(i,e1,e2,step) if (((step>0)&&(e1<=e2))||((step<0)&&(e2<=e1))) for(i=e1;((e1<=i)&&(i<=e2))||((e2<=i)&&(i<=e1));INC(i,step))


#endif
