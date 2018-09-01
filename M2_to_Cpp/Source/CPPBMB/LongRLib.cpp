/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       LongRLib (1.1)                              *
 * Version:                 1.0                                         *
 * Date:                    August 9, 2000                              *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module is portable.                    *
 * Remarks:                 For certain C++ compilers, the default      *
 *                          math error handling (printing an error      *
 *                          message and aborting) should be changed     *
 *                          manually (just to set errno).               *
 *                                                                      *
 * Author:                  Zoran Budimac                               *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_LongRLib_cpp_
#define _M2_LongRLib_cpp_

#include <math.h>
#include <errno.h>

#include <_m2sys.h>

#include <LongRLib.m2h>

#include <Wr.m2h>
#include <Wr.cpp>

namespace LongRLib {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg){
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** LongRLib::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case arithmeticError:
            Wr::Str("Floating point error");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode  = noError;
      return c;
   } // Error

   LONGREAL Sin(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = sin(r);
      if (errno != 0) SetError(arithmeticError, "Sin");
      return res;
   } // Sin

   LONGREAL Cos(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = cos(r);
      if (errno != 0) SetError(arithmeticError, "Cos");
      return res;
   } // Cos

   LONGREAL SinH(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = sinh(r);
      if (errno != 0) SetError(arithmeticError, "SinH");
      return res;
   } // SinH

   LONGREAL CosH(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = cosh(r);
      if (errno != 0) SetError(arithmeticError, "CosH");
      return res;
   } // CosH

   LONGREAL Exp(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = exp(r);
      if (errno != 0) SetError(arithmeticError, "Exp");
      return res;
   } // Exp

   LONGREAL Log(LONGREAL r) {
      LONGREAL res = 0.0;

      errno = 0;
      if (r <= 0) {
         SetError(arithmeticError, "Log");
      }
      else {
         res = log(r);
         if (errno != 0) SetError(arithmeticError, "Log");
      }
      return res;
   } // Log

   LONGREAL Log10(LONGREAL r) {
      LONGREAL res = 0.0;

      errno = 0;
      if (r <= 0) {
         SetError(arithmeticError, "Log10");
      }
      else {
         res = log10(r);
         if (errno != 0) SetError(arithmeticError, "Log10");
      }
      return res;
   } // Log10

   LONGREAL ATan(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = atan(r);
      if (errno != 0) SetError(arithmeticError, "ATan");
      return res;
   } // ATan

   LONGREAL ATanH(LONGREAL r) {
      LONGREAL res;

      errno = 0;
      res = Log((1.0+r) / (1.0-r)) / 2.0;
      if (errno != 0) SetError(arithmeticError, "ATanH");
      return res;
   } // ATanH

   LONGREAL Sqrt(LONGREAL r) {
      LONGREAL res = 0.0;

      errno = 0;
      if (r < 0) {
         SetError(arithmeticError, "Sqrt");
      }
      else {
         res = log10(r);
         if (errno != 0) SetError(arithmeticError, "Sqrt");
      }
      return res;
   } // Sqrt

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // LongRLib

#endif
