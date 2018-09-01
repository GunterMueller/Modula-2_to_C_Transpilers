/* By Milos Radovanovic, 2000, 2001 */

#ifndef _M2_Storage_cpp_
#define _M2_Storage_cpp_

#include <_m2sys.h>
#include <Storage.m2h>

#include <new>

namespace Storage {

  void ALLOCATE(ADDRESS &p, CARDINAL size) {
    p = operator new(size, nothrow);
  }

  template <class T>
  void ALLOCATE(POINTER<T> &p, CARDINAL size) {
    if ((TSIZE(T) == size) || (_DATATSIZE(T) == size)) {
      NEW(p);
    } else {
      p = operator new(size, nothrow);
    }
  }

  void DEALLOCATE(ADDRESS &p, CARDINAL size) {
    operator delete(p);
  }

  template <class T>
  void DEALLOCATE(POINTER<T> &p, CARDINAL size) {
    if ((TSIZE(T) == size) || (_DATATSIZE(T) == size)) {
      DISPOSE(p);
    } else {
      operator delete(p);
    }
  }

  BOOLEAN Available(CARDINAL size) {
    BOOLEAN res = FALSE;
    ADDRESS p = operator new(size, nothrow);
    if (p != ADDRESS(0)) {
      res = TRUE;
      operator delete(p);
    }
    return res;
  }

}

#endif
