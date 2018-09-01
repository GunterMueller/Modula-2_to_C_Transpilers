/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       RLib (1.1)                                  *
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

#ifndef _M2_RLib_cpp_
#define _M2_RLib_cpp_

#include <math.h>
#include <errno.h>

#include <_m2sys.h>

#include <RLib.m2h>

#include <Wr.m2h>
#include <Wr.cpp>

namespace RLib {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg){
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** RLib::");
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

   REAL Sin(REAL r) {
      REAL res;

      errno = 0;
      res = REAL(sin(r));
      if (errno != 0) SetError(arithmeticError, "Sin");
      return res;
   } // Sin

   REAL Cos(REAL r) {
      REAL res;

      errno = 0;
      res = REAL(cos(r));
      if (errno != 0) SetError(arithmeticError, "Cos");
      return res;
   } // Cos

   REAL SinH(REAL r) {
      REAL res;

      errno = 0;
      res = REAL(sinh(r));
      if (errno != 0) SetError(arithmeticError, "SinH");
      return res;
   } // SinH

   REAL CosH(REAL r) {
      REAL res;

      errno = 0;
      res = REAL(cosh(r));
      if (errno != 0) SetError(arithmeticError, "CosH");
      return res;
   } // CosH

   REAL Exp(REAL r) {
      REAL res;

      errno = 0;
      res = REAL(exp(r));
      if (errno != 0) SetError(arithmeticError, "Exp");
      return res;
   } // Exp

   REAL Log(REAL r) {
      REAL res = 0.0;

      errno = 0;
      if (r <= 0) {
         SetError(arithmeticError, "Log");
      }
      else {
         res = REAL(log(r));
         if (errno != 0) SetError(arithmeticError, "Log");
      }
      return res;
   } // Log

   REAL Log10(REAL r) {
      REAL res = 0.0;

      errno = 0;
      if (r <= 0) {
         SetError(arithmeticError, "Log10");
      }
      else {
         res = REAL(log10(r));
         if (errno != 0) SetError(arithmeticError, "Log10");
      }
      return res;
   } // Log10

   REAL ATan(REAL r) {
      REAL res;

      errno = 0;
      res = REAL(atan(r));
      if (errno != 0) SetError(arithmeticError, "ATan");
      return res;
   } // ATan

   REAL ATanH(REAL r) {
      REAL res;

      errno = 0;
      res = REAL( Log((1.0+r) / (1.0-r)) / 2.0 );
      if (errno != 0) SetError(arithmeticError, "ATanH");
      return res;
   } // ATanH

   REAL Sqrt(REAL r) {
      REAL res = 0.0;

      errno = 0;
      if (r < 0) {
         SetError(arithmeticError, "Sqrt");
      }
      else {
         res = REAL(log10(r));
         if (errno != 0) SetError(arithmeticError, "Sqrt");
      }
      return res;
   } // Sqrt

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // RLib

#endif
