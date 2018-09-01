(* ------------------------------------------------------------------ *
 *                 BMB Modula-2 Cross-Platform Library                *
 *                                                                    *
 * Implementation of:       RLib (1.1)                                *
 * Version:                 1.0                                       *
 * Date:                    February 27, 1995                         *
 * Conformance Test Passed: -                                         *
 * Platform:                DOS/80x86                                 *
 * Requires:                TopSpeed Modula-2 Compiler ver. 1.xx      *
 * Portability:             This module is highly portable. Only error*
 *                          reporting routine is taken from Top Speed *
 *                          module.
 *                                                                    *
 * Author:                      Zoran Budimac                         *
 *                              University of Novi Sad, Yugoslavia    *
 *                              e-mail: zjb@unsim.ns.ac.yu            *
 * ------------------------------------------------------------------ *)

IMPLEMENTATION MODULE RLib;
IMPORT LongRLib;                (* BMB mathematical operations *)
IMPORT Wr;
FROM MATHLIB IMPORT MathError;  (* TopSpeed error handling *)

VAR
   errorCode : CARDINAL;

PROCEDURE SetError(code: CARDINAL; msg: ARRAY OF CHAR);
BEGIN
  errorCode := code;
  errorHandler(msg);
END SetError;

PROCEDURE PrintError(msg: ARRAY OF CHAR);
BEGIN
  Wr.Str('*** RLib.');
  Wr.Str(msg); Wr.Str(": ");
  CASE errorCode OF
    arithmeticError   : Wr.Str('Floating point error')
  END;
  HALT;
END PrintError;


PROCEDURE Sin(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.Sin(LONGREAL(r)));
END Sin;

PROCEDURE Cos(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.Cos(LONGREAL(r)));
END Cos;

PROCEDURE SinH(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.SinH(LONGREAL(r)));
END SinH;

PROCEDURE CosH(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.SinH(LONGREAL(r)));
END CosH;

PROCEDURE Exp(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.Exp(LONGREAL(r)));
END Exp;

PROCEDURE Log(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.Log(LONGREAL(r)));
END Log;

PROCEDURE Log10(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.Log10(LONGREAL(r)));
END Log10;

PROCEDURE ATan(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.ATan(LONGREAL(r)));
END ATan;

PROCEDURE ATanH(r: REAL): REAL;
BEGIN
  (* ??? *)
  RETURN 0.0
END ATanH;

PROCEDURE Sqrt(r: REAL): REAL;
BEGIN
  RETURN REAL(LongRLib.Sqrt(LONGREAL(r)));
END Sqrt;

PROCEDURE MyMathError(x: LONGREAL; s: ARRAY OF CHAR);
   (* Exception handling procedure for dealing with floating point errors *)
BEGIN
  errorCode := arithmeticError;
END MyMathError;

PROCEDURE Error(): CARDINAL;
VAR c: CARDINAL;
BEGIN
  c := errorCode;
  errorCode := noError;
  RETURN c
END Error;

BEGIN
  errorCode := noError;
  errorHandler := PrintError;
  MathError := MyMathError;
      (* Initialzaition of exception handling procedure *)
END RLib.