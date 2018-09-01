(* ---------------------------------------------------------------- *
 *                BMB Modula-2 Cross-Platform Library               *
 *                                                                  *
 * Implementation of:     Env (1.00)                                *
 * Version:               1.0                                       *
 * Date:                  January, 1997                             *
 * Conformance Test Passed: -                                       *
 * Platform:              80x86 / MS-DOS                            *
 * Requires:              TopSpeed Modula-2 ver. 1.x                *
 *                                                                  *
 * Author:                    Daniel, Sendula                       *
 *                            University of Novi Sad, Yugoslavia    *
 *                            e-mail: isendula@unsim.ns.ac.yu       *
 * ---------------------------------------------------------------- *)

IMPLEMENTATION MODULE Env;
IMPORT Wr, String, Storage, Lib, FileName;
FROM SYSTEM IMPORT Seg,Ofs,Registers,CarryFlag;
VAR errorCode: CARDINAL;

VAR
  comspec: CARDINAL;
  env: POINTER TO ARRAY [ 0 .. 65535 ] OF CHAR;
  envptr: CARDINAL;
  command: ARRAY [ 0 .. 12 ] OF CHAR;
  path: ARRAY [ 0 .. 255 ] OF CHAR;

PROCEDURE Error() : CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c;
END Error;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** Env.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    unknownError    : Wr.Str('Unknown error');
  | noRoomError     : Wr.Str('No room error');
  | wrongArgError   : Wr.Str('Wrong argument supplied')
  END;
  HALT;
END PrintError;

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg);
END SetError;

(* ----------------------------------------------------------------- *)

   PROCEDURE GetParam(VAR s: ARRAY OF CHAR; n: CARDINAL);
   BEGIN
     IF n > 0 THEN
       Lib.ParamStr(s, n)
     ELSE
       SetError(wrongArgError, "GetParam")
     END;
   END GetParam;

   PROCEDURE Var(VAR Value: ARRAY OF CHAR; variable: ARRAY OF CHAR): BOOLEAN;
   CONST
     MAXSTR = 256;
   TYPE
     str = ARRAY [0 .. MAXSTR-1] OF CHAR;
   VAR
     env, i, j, len: CARDINAL;
     p: POINTER TO str;
     var, val: str;
   BEGIN
     env := 0;
     LOOP
       p := ADDRESS(Lib.Environment(env)); INC(env);
       len := String.Length(p^);
       IF String.Length(p^) = 0 THEN
         RETURN FALSE
       END;
       i := 0;
       REPEAT
         IF p^[i] # '=' THEN
           var[i] := p^[i];
           INC(i)
         END;
       UNTIL (i >= len) OR (p^[i] = '=');
       IF i < len THEN
         var[i] := 0C;
         j := 0;
         INC(i);
         REPEAT
           IF i < len THEN
             IF j > MAXSTR THEN
               SetError(noRoomError, "Var");
             END;
             val[j] := p^[i];
             INC(i);
             INC(j);
           END;
         UNTIL i >= len;
         IF j < MAXSTR-1 THEN
           val[j] := 0C;
         END;
         IF String.Equal(variable, var) THEN
           IF HIGH(Value) <= MAXSTR-1 THEN
             String.Copy(Value, val)
           ELSE
             SetError(noRoomError, "GetVar");
           END;
           RETURN TRUE;
         END;
       END;
     END;
   END Var;

   PROCEDURE GetProgramName(VAR s: ARRAY OF CHAR);
   BEGIN
     String.Copy(s, command);
   END GetProgramName;

   PROCEDURE GetProgramPath(VAR s: ARRAY OF CHAR);
   BEGIN
     String.Copy(s, path);
   END GetProgramPath;

VAR
  i: CARDINAL;

BEGIN
  errorCode := noError;
  errorHandler := PrintError;

  env := [ [ Lib.PSP : 2CH ] ^ : 0 ];

  envptr := 0;
  REPEAT
    INC(envptr);
  UNTIL (env^[envptr] = 0C) AND (env^[envptr-1] = 0C);

  comspec := ORD(env^[envptr+1])+ORD(env^[envptr+2])*256;
  INC(envptr);
  INC(envptr);
  INC(envptr);
  i := 0;
  REPEAT
    path[i] := env^[envptr+i];
    INC(i);
  UNTIL (i > 255) OR (env^[envptr+i-1] = 0C);
  FileName.Split(path, path, command);
  i := 0;
  WHILE (command[i] # '.') AND (i < 12) DO
    INC(i);
  END;
  IF i < 12 THEN
    command[i] := 0C
  END;
END Env.