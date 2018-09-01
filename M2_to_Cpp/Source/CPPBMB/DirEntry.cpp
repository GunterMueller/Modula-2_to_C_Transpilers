/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       DirEntry                                    *
 * Version:                 1.10                                        *
 * Date:                    June 9, 2000                                *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 *                          + non standard dir.h                        *
 * Portability:             Non standard, but fairly common dir.h       *
 *                          has to be available.                        *
 *                                                                      *
 * Author:                  Zoran Budimac                               *
 *                          (for 'Black Mailbox' Group)                 *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: zjb@unsim.ns.ac.yu              *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_DirEntry_cpp_
#define _M2_DirEntry_cpp_

#include <dir.h>
#include <errno.h>

#include <_m2sys.h>

#include <DirEntry.m2h>

#include <Date.m2h>
#include <Time.m2h>
#include <String.m2h>
#include <Wr.m2h>
#include <Date.cpp>
#include <Time.cpp>
#include <String.cpp>
#include <Wr.cpp>

namespace DirEntry {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** DirEntry::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case unknownError:
            Wr::Str("Unknown error");
            break;
         case noRoomError:
            Wr::Str("Buffer exceeded");
            break;
         case pathNotFoundError:
            Wr::Str("Path not found");
            break;
         case priviledgeError:
            Wr::Str("Priviledge error");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error

   void Name(_ARRAYOF<CHAR>& name, T e) {
      String::Copy(name, e.name);
   } // Name

   void CrTime(Time::T& t, T e) {
      t = e.timeC;
   } // CrTime

   void CrDate(Date::T& d, T e) {
      d = e.dateC;
   } // CrDate

   void ModTime(Time::T& t, T e) {
      t = e.timeM;
   } // ModTime

   void ModDate(Date::T& d, T e) {
      d = e.dateM;
   } // ModDate

   LONGCARD Size(T e) {
      return e.size;
   } // Size

   void ConvertTime(Time::T& t, CARDINAL n) {
      Time::SetHour(t, n / 2048);
      n = n % 2048;
      Time::SetMin(t, n / 32);
      n = n % 32;
      Time::SetSec(t, n);
      Time::SetHund(t, 0);
   } // ConvertTime

   void ConvertDate(Date::T& d, CARDINAL n) {
      Date::SetYear(d, (n / 512)+1980);
      n = n % 512;
      Date::SetMonth(d, n / 32);
      n = n % 32;
      Date::SetDay(d, n);
   } // ConvertDate

   ARRAY<_SubrangeType<CARDINAL, 0, 125>, CHAR> currentFileName,
                                                previousFileName;
                                                /* For directory entries */

   void SetPath(_ARRAYOF<CHAR> filename) {
      String::Copy(currentFileName, filename);
   } // SetPath

   ffblk f;

   BOOLEAN Found(T& e) {
      BOOLEAN val;
      int status;

      if (String::Equal(previousFileName, currentFileName))
         status = findnext(&f);
      else {
         String::Copy(previousFileName, currentFileName);
         status = findfirst(_CSTR(currentFileName), &f,
                            FA_RDONLY | FA_HIDDEN | FA_SYSTEM |
                            /*FA_LABEL  |*/ FA_DIREC  | FA_ARCH   );
      }
      // if FA_LABEL is on, BCC 5.5 finds only the label,
      // and DJGPP 2.95.2 finds everything but the label ...

      // ToDo: better error-checking maybe
      if ((status == EACCES) || (status == EPERM))
         SetError(priviledgeError, "Found");

      if (status == 0)
         val = TRUE;
      else
         val = FALSE;

      if (val) {
         e.size = f.ff_fsize;
         ConvertTime(e.timeC, CARDINAL(f.ff_ftime));
         ConvertDate(e.dateC, CARDINAL(f.ff_fdate));
         e.timeM = e.timeC;
         e.dateM = e.dateC;
         String::Copy(e.name, f.ff_name);
      }

      return val;
   } // Found

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
      String::Copy(previousFileName, "");
   }

   void _ModuleBody::FINALLY() {
   }

} // DirEntry

#endif
