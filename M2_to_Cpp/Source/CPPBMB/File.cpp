/* -------------------------------------------------------------------- *
 *                 BMB Modula-2 Cross-Platform Library                  *
 *                              C++ Port                                *
 *                                                                      *
 * Implementation of:       File (1.1)                                  *
 * Version:                 1.0                                         *
 * Date:                    February 28, 2000                           *
 * Conformance Test Passed: -                                           *
 * Platform:                Every                                       *
 * Requires:                Generic ISO C++ compiler                    *
 * Portability:             This module portable.                       *
 * Remarks:                 StdPrn() directs to StdOut() because        *
 *                          there is there is no way to implement it    *
 *                          using the standard C library.               *
 *                                                                      *
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

#ifndef _M2_File_cpp_
#define _M2_File_cpp_

#include <stdio.h>
#include <errno.h>

#include <_m2sys.h>

#include <File.m2h>

#include <Wr.m2h>
#include <Wr.cpp>

namespace File {

   CARDINAL errorCode = noError;

   void SetError(CARDINAL code, _ARRAYOF<CHAR> msg) {
      errorCode = code;
      errorHandler(msg);
   }

   void PrintError(_ARRAYOF<CHAR> msg) {
      Wr::Str("*** File::");
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
         case fileNotFoundError:
            Wr::Str("File not found");
            break;
         case pathNotFoundError:
            Wr::Str("Path not found");
            break;
         case readError:
            Wr::Str("Read error");
            break;
         case writeError:
            Wr::Str("Write error");
            break;
         case priviledgeError:
            Wr::Str("Priviledge error");
            break;
         case handleError:
            Wr::Str("Wrong handle");
            break;
      }
      HALT();
   } // PrintError

   CARDINAL Error() {
      CARDINAL c = errorCode;

      errorCode = noError;
      return c;
   } // Error

   TRec StdInR;
   TRec StdOutR;
   TRec StdErrR;
   TRec StdPrnR;

   T StdInP;
   T StdOutP;
   T StdErrP;
   T StdPrnP;

   T StdIn() {
      return StdInP;
   }

   T StdOut() {
      return StdOutP;
   }

   T StdErr() {
      return StdErrP;
   }

   T StdPrn() {
      return StdPrnP;
   }

   T Open(_ARRAYOF<CHAR> Name) {
      T f;

      NEW(f);
      (*f).LastChar = '\0';
      (*f).OneMoreChar = FALSE;
      (*f).Interactive = FALSE;
      (*f).unread = FALSE;

      errno = 0;
      (*f).F = fopen(_CSTR(Name), "r+b");

      if ((*f).F == 0) {
         switch (errno) {
         case EBADF: case EMFILE: case ENFILE: case ENODEV:
            SetError(handleError, "Open");
            break;
         case EACCES: case EPERM:
            SetError(priviledgeError, "Open");
            break;
         case ENOENT:
            SetError(fileNotFoundError, "Open");
            break;
         case EISDIR:
            SetError(notFileError, "Open");
            break;
         default:
            SetError(unknownError, "Open");
         }
      }
      return f;
   }

   T OpenRead(_ARRAYOF<CHAR> Name) {
      T f;

      NEW(f);
      (*f).LastChar = '\0';
      (*f).OneMoreChar = FALSE;
      (*f).Interactive = FALSE;
      (*f).unread = FALSE;

      errno = 0;
      (*f).F = fopen(_CSTR(Name), "rb");

      if ((*f).F == 0) {
         switch (errno) {
         case EBADF: case EMFILE: case ENFILE: case ENODEV:
            SetError(handleError, "OpenRead");
            break;
         case EACCES: case EPERM:
            SetError(priviledgeError, "OpenRead");
            break;
         case ENOENT:
            SetError(fileNotFoundError, "OpenRead");
            break;
         case EISDIR:
            SetError(notFileError, "OpenRead");
            break;
         default:
            SetError(unknownError, "OpenRead");
         }
      }
      return f;
   }

   T Create(_ARRAYOF<CHAR> Name) {
      T f;

      NEW(f);
      (*f).LastChar = '\0';
      (*f).OneMoreChar = FALSE;
      (*f).Interactive = FALSE;
      (*f).unread = FALSE;

      errno = 0;
      (*f).F = fopen(_CSTR(Name), "wb");

      if ((*f).F == 0) {
         switch (errno) {
         case EBADF: case EMFILE: case ENFILE: case ENODEV:
            SetError(handleError, "Create");
            break;
         case EACCES: case EPERM:
            SetError(priviledgeError, "Create");
            break;
         case ENOENT:
            SetError(pathNotFoundError, "Create");
            break;
         case EISDIR:
            SetError(notFileError, "Create");
            break;
         case ENOSPC:
            SetError(diskFullError, "Create");
            break;
         default:
            SetError(unknownError, "Create");
         }
      }
      return f;
   }

   T Append(_ARRAYOF<CHAR> Name) {
      T f = Open(Name);

      Seek(f, Size(f));
      return f;
   }

   void Close(T f) {

      Flush(f);
      errno = 0;
      if (fclose((*f).F) == EOF) {
         switch (errno) {
         case EBADF: case EMFILE: case ENFILE: case ENODEV:
            SetError(handleError, "Close");
            break;
         default:
            SetError(unknownError, "Close");
         }
      }
      if (!((*f).Interactive)) DISPOSE(f); // Don't DISPOSE StdIn, StdOut ...
   }

   BOOLEAN IsInteractive(T f) {
      return ((*f).F == stdin) || ((*f).F == stdout) || ((*f).F == stderr);
   }

   template <class _BYTEARRAY1>
   void WrBin(T f, _BYTEARRAY1 bin, LONGCARD size) {
      // izmena: bin nije VAR parametar
      _ARRAYOF<BYTE> _bin(_DATAADR(bin), _DATASIZE(bin));

      for (LONGCARD i = 0; i < size; INC(i)) {
         errno = 0;
         if (fputc(_bin[i], (*f).F) == EOF) {
            switch (errno) {
            case EBADF: case EMFILE: case ENFILE: case ENODEV:
               SetError(handleError, "WrBin");
               break;
            case EACCES: case EPERM:
               SetError(priviledgeError, "WrBin");
               break;
            case EIO:
               SetError(writeError, "WrBin");
               break;
            case ENOSPC:
               SetError(diskFullError, "WrBin");
               break;
            default:
               SetError(unknownError, "WrBin");
            }
            break; // from for(...)
         }
      }
   }

   void WrChar(T f, CHAR ch) {
      WrBin(f, ch, SIZE(ch));
   }

   void WrStr(T f, _ARRAYOF<CHAR> s) {
      WrBin(f, s, LENGTH(s));
   }

   void WrLn(T f) {
      WrBin(f, _ARRAYOF<CHAR>("\n"), LENGTH("\n")); // valjda ce raditi ...
   }

   template <class BYTEARRAY1>
   LONGCARD RdBin(T f, BYTEARRAY1& bin, LONGCARD size) {
      _ARRAYOF<BYTE> _bin(_DATAADR(bin), _DATASIZE(bin));

      errno = 0;
      LONGCARD actual = fread(_DATAADR(_bin), TSIZE(BYTE), size, (*f).F);
      if (actual != size) {
         switch (errno) {
         case EBADF: case EMFILE: case ENFILE: case ENODEV:
            SetError(handleError, "RdBin");
            break;
         case EACCES: case EPERM:
            SetError(priviledgeError, "RdBin");
            break;
         case EIO:
            SetError(readError, "RdBin");
            break;
         default:
            SetError(unknownError, "RdBin");
         }
      }
      return actual;
   }

   CHAR RdChar(T f) {
      CHAR c;

      if ((*f).OneMoreChar) {
         (*f).OneMoreChar = FALSE;
         (*f).unread = TRUE;
         return (*f).LastChar;
      }
      else {
         if (RdBin(f, c, SIZE(c)) != SIZE(c)) {
            (*f).unread = FALSE;
            return '\0';
         }
         else {
            (*f).LastChar = c;
            (*f).unread = TRUE;
            return c;
         }
      }
   }

   void UnRdChar(T& f) {
      if ((*f).unread) {
         (*f).OneMoreChar = TRUE;
         (*f).unread = FALSE;
      }
      else {
         SetError(unknownError, "UnRdChar");
      }
   }

   void RdLine(T f, _ARRAYOF<CHAR>& line) {
      CHAR c;
      CARDINAL p = 0;

      errorCode = 0;
      for (;;) {
         c = RdChar(f);
         if ((c == '\015') || (c == '\012') || (c == '\014')) {
            if (c == '\015') {
               RdChar(f);
            }
            if (p <= HIGH(line)) {
               line[p] = '\0';
            }
            (*f).unread = FALSE;
            return;
         }
         else {
            if (Error() > 0) {
               (*f).unread = FALSE;
               return;
            }
            else {
               if (p > HIGH(line)) {
                  SetError(noRoomError , "RdLine");
                  (*f).unread = FALSE;
                  return;
               }
               else {
                  line[p] = c;
                  INC(p);
               }
            }
         }
      }
   }

   void RdLn(T f) {
      ARRAY<_SubrangeType<CARDINAL, 0, 999> ,CHAR> s;

      RdLine(f, s);
      (*f).unread = FALSE;
   }

   BOOLEAN IsEOF(T f) {
      if (Pos(f) >= Size(f)) {
         return TRUE;
      }
      else {
         return FALSE;
      }
   }

   LONGCARD Pos(T f) {
      errno = 0;
      long ret = ftell((*f).F);
      if (ret < 0) {
         switch (errno) {
         case EBADF: case EMFILE: case ENFILE: case ENODEV:
            SetError(handleError, "Pos");
            break;
         default:
            SetError(unknownError, "Pos");
         }
         return 0;
      }
      else {
         return ret;
      }
   }

   void Seek(T f, LONGCARD Pos) {
      if (Pos > MAX(long)) {
         SetError(unknownError, "Seek");
      }
      else {
         errno = 0;
         if (fseek((*f).F, Pos, SEEK_SET) != 0) {
            switch (errno) {
            case EBADF: case EMFILE: case ENFILE: case ENODEV:
               SetError(handleError, "Seek");
               break;
            default:
               SetError(unknownError, "Seek");
            }
         }
      }
   }

   LONGCARD Size(T f) {
      LONGCARD pos, ret;

      pos = Pos(f);
      fseek((*f).F, 0L, SEEK_END);
      ret = Pos(f);
      Seek(f, pos);
      return ret;
   }

   void Flush(T f) {
      if ((*f).F == 0) {
         SetError(handleError, "Flush");
      }
      else {
         errno = 0;
         if (fflush((*f).F) != 0) {
            switch (errno) {
            case EBADF: case EMFILE: case ENFILE: case ENODEV:
               SetError(handleError, "Flush");
               break;
            default:
               SetError(unknownError, "Flush");
            }
         }
      }
   }

   void _ModuleBody::BEGIN() {
      errorHandler = PrintError;

      StdInR.F = stdin;
      StdInR.LastChar = '\0';
      StdInR.OneMoreChar = FALSE;
      StdInR.Interactive = TRUE;
      StdInR.unread = FALSE;
      StdInP = ADR(StdInR);

      StdOutR.F = stdout;
      StdOutR.LastChar = '\0';
      StdOutR.OneMoreChar = FALSE;
      StdOutR.Interactive = TRUE;
      StdOutR.unread = FALSE;
      StdOutP = ADR(StdOutR);

      StdErrR.F = stderr;
      StdErrR.LastChar = '\0';
      StdErrR.OneMoreChar = FALSE;
      StdErrR.Interactive = TRUE;
      StdErrR.unread = FALSE;
      StdErrP = ADR(StdErrR);

      //StdPrnR.F = stdprn; // ToDo: NOT ANSI
      StdPrnR.F = stdout;
      StdPrnR.LastChar = '\0';
      StdPrnR.OneMoreChar = FALSE;
      StdPrnR.Interactive = TRUE;
      StdPrnR.unread = FALSE;
      StdPrnP = ADR(StdPrnR);
   }

   void _ModuleBody::FINALLY() {
   }

} // File

#endif
