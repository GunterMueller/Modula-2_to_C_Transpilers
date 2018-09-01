IMPLEMENTATION MODULE TS1RLibs;
FROM MATHLIB IMPORT MathError;  (* TopSpeed error handling *)
FROM

PROCEDURE MyMathError(x: LONGREAL; s: ARRAY OF CHAR);
   (* Exception handling procedure for dealing with floating point errors *)
BEGIN
  errorCode := arithmeticError;
END MyMathError;


BEGIN
  MathError := MyMathError;
      (* Initialzaition of exception handling procedure *)
END TS1RLibs;