/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       FileName (1.1)                              *
 * Version:                 1.0                                         *
 * Date:                    June 10, 2000                               *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module highly portable. The only issue *
 *                          are 'root' and 'delim' constants in         *
 *                          FileName.m2h.                               *
 *                                                                      *
 * Author:                  Daniel Sendula                              *
 *                          (for 'Black Mailbox' Group)                 *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: isendula@unsim.ns.ac.yu         *
 *                                                                      *
 * C++ Port Author:         Milos Radovanovic                           *
 *                              University of Novi Sad, Yugoslavia      *
 *                              e-mail: radacha@eunet.yu                *
 * -------------------------------------------------------------------- */

#ifndef _M2_FileName_cpp_
#define _M2_FileName_cpp_

#include <stdio.h>
#include <errno.h>

#include <_m2sys.h>

#include <FileName.m2h>

#include <String.m2h>
#include <DirEntry.m2h>
#include <Wr.m2h>
#include <String.cpp>
#include <DirEntry.cpp>
#include <Wr.cpp>

namespace FileName {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** FileName::");
      Wr::Str(msg); Wr::Str(": ");
      switch (errorCode) {
         case unknownError:
            Wr::Str("Unknown error");
            break;
         case noRoomError:
            Wr::Str("Buffer exceeded");
            break;
         case notFileError:
            Wr::Str("It is not a file");
            break;
         case fileNotFoundError:
            Wr::Str("File not found");
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

   // ToDo: errors ?
   BOOLEAN Exists(_ARRAYOF<CHAR> name) {
      DirEntry::T e;

      DirEntry::SetPath(name);
      return DirEntry::Found(e);
   } // Exists

   void Delete(_ARRAYOF<CHAR> name) {
      ARRAY <_SubrangeType<CARDINAL, 0, 260>, CHAR> dirName, fileName;

      if (remove(_CSTR(name)) != 0) {
         switch (errno) {
         case EACCES: case EPERM:
            SetError(priviledgeError, "Delete");
            break;
         case ENOENT:
            Split(name, dirName, fileName);
            if (String::Length(dirName) != 0) {
              if (Exists(dirName)) SetError(fileNotFoundError, "Delete");
              else                 SetError(pathNotFoundError, "Delete");
            }
            else {
              SetError(fileNotFoundError, "Delete");
            }
            break;
         case EISDIR:
            SetError(notFileError, "Delete");
            break;
         default:
            SetError(unknownError, "Delete");
         }
      }
   } // Delete

   void Rename(_ARRAYOF<CHAR> from, _ARRAYOF<CHAR> to) {
      ARRAY <_SubrangeType<CARDINAL, 0, 260>, CHAR> dirName, fileName;

      if (rename(_CSTR(from), _CSTR(to)) != 0) {
         switch (errno) {
         case EACCES: case EPERM:
            SetError(priviledgeError, "Rename");
            break;
         case ENOENT:
            Split(from, dirName, fileName);
            if (String::Length(dirName) != 0) {
              if (Exists(dirName)) SetError(fileNotFoundError, "Rename");
              else                 SetError(pathNotFoundError, "Rename");
            }
            else {
              SetError(fileNotFoundError, "Rename");
            }
            break;
         case EISDIR: case ENOTDIR: // 'from' and 'to' both exist and
                                    // one is a file, the other a directory
            SetError(notFileError, "Rename");
            break;
         default:
            SetError(unknownError, "Rename");
         }
      }
   } // Rename

   void Split(_ARRAYOF<CHAR> fullName,
              _ARRAYOF<CHAR>& dir, _ARRAYOF<CHAR>& name) {

      CARDINAL p, sp, len, i;

      len = String::Length(fullName);
      p   = len + 1;
      sp  = 0;
      for(;;) {
         if (p == 0) break;
         DEC(p);
         if (fullName[p] == root) {
            if (p > 0) sp = p;
            break;
         }
      }
      if (sp > 0) {
         DEC(sp);
         if (HIGH(dir) >= sp) {
            for (i = 0; i <= sp; INC(i))
               dir[i] = fullName[i];

            if (HIGH(dir) > sp)
               dir[sp+1] = '\0';
         }
         else
            SetError(noRoomError, "Split");

         INC(sp);
      }
      else
         dir[0] = '\0';

      if (sp > 0) INC(sp);

      if (sp < len)
         if (HIGH(name) >= len-sp) {
            for (i = sp; i <= len-1; INC(i))
               name[i-sp] = fullName[i];

            if (HIGH(name) > len-sp)
               name[len-sp] = '\0';
         }
      else
         name[0] = '\0';
   } // Split

   void Concat(_ARRAYOF<CHAR>& fullName,
               _ARRAYOF<CHAR> dir, _ARRAYOF<CHAR> name) {
      String::Concat(fullName, dir, delim);
      String::Append(fullName, name);
   } // Concat

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;
   }

   void _ModuleBody::FINALLY() {
   }

} // FileName

#endif
