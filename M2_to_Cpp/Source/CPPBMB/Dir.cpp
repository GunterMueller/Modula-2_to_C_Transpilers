/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       Dir (1.1)                                   *
 * Version:                 1.0                                         *
 * Date:                    June 10, 2000                               *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Borland C++ 5.5 or                          *
 *                          any other POSIX copliant ISO C++ compiler   *
 * Portability:             This module relies on C functions           *
 *                          'mkdir', 'rmdir', 'chdir' and 'getcwd',     *
 *                          which are POSIX.                            *
 * Author:                  Daniel Sendula                              *
 *                          (for 'Black Mailbox' Group)                 *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: isendula@unsim.ns.ac.yu         *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_Dir_cpp_
#define _M2_Dir_cpp_

#ifdef __BORLANDC__
#include <direct.h>
#else
#include <unistd.h>
#include <sys\stat.h>
#endif

#include <_m2sys.h>

#include <Dir.m2h>

#include <FileName.m2h>
#include <String.m2h>
#include <Wr.m2h>
#include <FileName.cpp>
#include <String.cpp>
#include <Wr.cpp>

namespace Dir {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** Dir::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case unknownError:
            Wr::Str("Unknown error");
            break;
         case diskFullError:
            Wr::Str("Disk is full");
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

   void Make(_ARRAYOF<CHAR> dir) {
      if (mkdir(_CSTR(dir)
#ifndef __BORLANDC__
          , S_IWUSR
#endif
         ) != 0) {
         switch (errno) {
         case EACCES: case EPERM:
            SetError(priviledgeError, "Make");
            break;
         case ENOENT:
            SetError(pathNotFoundError, "Make");
            break;
         case ENOSPC:
            SetError(diskFullError, "Make");
            break;
         default:
            SetError(unknownError, "Make");
         }
      }
   } // Make

   void Remove(_ARRAYOF<CHAR> dir) {
      if (rmdir(_CSTR(dir)) != 0) {
         switch (errno) {
         case EACCES: case EPERM:
            SetError(priviledgeError, "Remove");
            break;
         case ENOENT:
            SetError(pathNotFoundError, "Remove");
            break;
         default:
            SetError(unknownError, "Remove");
         }
      }
   } // Remove

   void ChangeTo(_ARRAYOF<CHAR> dir) {
      if (chdir(_CSTR(dir)) != 0) {
         switch (errno) {
         case EACCES: case EPERM:
            SetError(priviledgeError, "ChangeTo");
            break;
         case ENOENT:
            SetError(pathNotFoundError, "ChangeTo");
            break;
         default:
            SetError(unknownError, "ChangeTo");
         }
      }
   } // ChangeTo

   void GetCurrent(_ARRAYOF<CHAR>& dir) {
      char *buf = getcwd(_CSTR(dir), HIGH(dir)+1);

      if (buf == 0) SetError(noRoomError, "GetCurrent");
      else          String::Copy(dir, buf);
   } // GetCurrent

   void GetParent(_ARRAYOF<CHAR>& dir) {
      ARRAY<_SubrangeType<CARDINAL, 0, 260>, CHAR> full, name;

      GetCurrent(full);
      FileName::Split(full, dir, name);
      if (dir[String::Length(dir)-1] == ':') {
         String::Append(dir, FileName::delim);
         SetError(pathNotFoundError, "GetParent");
      }
   } // GetParent

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // Dir

#endif
