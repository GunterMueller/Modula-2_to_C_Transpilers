/* -------------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library                   *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Env (1.00)                                  *
 * Version:                 1.0                                         *
 * Date:                    January 11, 2001                            *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Conformance Test Passed: -                                           *
 *                                                                      *
 * Author:                  Daniel Sendula                              *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: isendula@unsim.ns.ac.yu         *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */


#ifndef _M2_Env_cpp_
#define _M2_Env_cpp_

#include <stdlib.h>

#include <_m2sys.h>

#include <Env.m2h>

#include <String.m2h>
#include <FileName.m2h>
#include <Wr.m2h>
#include <String.cpp>
#include <FileName.cpp>
#include <Wr.cpp>

namespace Env {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** Env::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case unknownError:
            Wr::Str("Unknown error");
            break;
         case noRoomError:
            Wr::Str("Buffer exceeded");
            break;
         case wrongArgError:
            Wr::Str("Wrong argument supplied");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error

   int    _argc;
   char** _argv;

   ARRAY<_SubrangeType<INTEGER, 0, 255>, CHAR> command, path;

   CARDINAL Params() {
     return CARDINAL(_argc);
   } // Params

   void GetParam(_ARRAYOF<CHAR>& s, CARDINAL n) {
     if (n > 0)
       String::Copy(s, _argv[n]);
     else
       SetError(wrongArgError, "GetParam");
   } // GetParam

   void GetProgramName(_ARRAYOF<CHAR>& s) {
     String::Copy(s, command);
   } // GetProgramName

   void GetProgramPath(_ARRAYOF<CHAR>& s) {
     String::Copy(s, path);
   } // GetProgramPath

   BOOLEAN Var(_ARRAYOF<CHAR>& value, _ARRAYOF<CHAR> variable) {
     char *tmp = getenv(_CSTR(variable));
     if (String::Length(tmp) > HIGH(value))
       SetError(noRoomError, "Var");
     else
       String::Copy(value, tmp);
   } // Var

   void _init(int argc, char* argv[]) {
     _argc = argc;
     _argv = argv;
   }

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
      FileName::Split(_argv[0], path, command);
   }

   void _ModuleBody::FINALLY() {
   }

} // Env

#endif
