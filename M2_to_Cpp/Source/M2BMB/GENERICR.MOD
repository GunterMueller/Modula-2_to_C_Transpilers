(* -------------------------------------------------------------------
    This module is internally needed by the implementation of BMB
    Cross Platform 'Fmt' module. It contains operations for conversion
    of real numbers. Operations are general and should compile on
    every Modula-2 compiler and execute on every platform.

    by Zoran Budimac, July 11, 1996.
    ------------------------------------------------------------------ *)
IMPLEMENTATION MODULE GenericRealConvs;
IMPORT String;

PROCEDURE Pow10(n: LONGINT; VAR overflow: BOOLEAN): LONGREAL;
(* Returns 10 to the 'n'-th. *)
  VAR
    expNeg: BOOLEAN;
    b, x: LONGREAL;
BEGIN
  overflow := FALSE;
  expNeg := n<0;
  n := ABS(n);
  b := 10.0;
  x := 1.0;
  WHILE n#0 DO
    IF ODD(n) THEN
       IF x <= MAX(REAL)/b THEN
          x := x*b
       ELSE
          overflow := TRUE
       END;
    END;
    IF b <= MAX(REAL)/b THEN
       b := b*b
    ELSE
       overflow := TRUE
    END;
    n := n DIV 2;
  END;
  IF expNeg THEN
    RETURN 1.0/x
  ELSE
    RETURN x;
  END;
END Pow10;

PROCEDURE Digit(ch: CHAR): BOOLEAN;
BEGIN
  RETURN (ch >= '0') & (ch <= '9')
END Digit;

PROCEDURE StrToReal(s: ARRAY OF CHAR;
                    VAR done: BOOLEAN; VAR overflow: BOOLEAN): REAL;
   (* Converts the string 's' to real number. 'done' is TRUE if conversion
      was succesful. 'overflow' is TRUE if overflow occured during
      conversion. *)
   (* by Zoran Budimac, July, 1996 *)
VAR
  len, i, fracDigits: CARDINAL;
  neg, negExp: BOOLEAN;
  pow: LONGREAL;
  mantissa: REAL;
  val: CARDINAL;
  exp: LONGINT;

BEGIN
  done := TRUE;
  overflow := FALSE;
  IF String.Length(s) = 0 THEN
     done := FALSE;
     RETURN 0.0
  END;
  len := String.Length(s);
  i := 0;
  neg := FALSE;
  negExp := FALSE;
  mantissa := 0.0;
  exp := 0;
  fracDigits := 0; (* digits behind decimal point *)

  (* ['-'] *)
  IF s[i] = '-' THEN
     neg := TRUE;
     INC(i)
  ELSIF NOT Digit(s[i]) THEN
     done := FALSE;
     RETURN 0.0
  END;
  (* digit(s) *)
  WHILE Digit(s[i]) DO
    val := ORD(s[i]) - ORD('0');
    IF mantissa <= MAX(REAL) / 10.0 - FLOAT(val) THEN
       mantissa := mantissa * 10.0 + FLOAT(val);
    ELSE
       overflow := TRUE;
       RETURN 0.0
    END;
    INC(i);
  END;
  (* decimal point *)
  IF s[i] = '.' THEN
     INC(i);
     (* digit(s) *)
     WHILE (i<len) & Digit(s[i]) DO
       val := ORD(s[i]) - ORD('0');
       IF mantissa <= MAX(REAL) / 10.0 - FLOAT(val) THEN
          mantissa := mantissa * 10.0 + FLOAT(val)
       ELSE
          overflow := TRUE;
          RETURN 0.0
       END;
       INC(i);
       INC(fracDigits)
     END;
  END;
  (* exponent *)
  IF (CAP(s[i]) = 'E') THEN
     INC(i);
     IF s[i] = '-' THEN
        negExp := TRUE;
        INC(i)
     ELSIF s[i] = '+' THEN
        INC(i)
     END;
     IF (i<len) & Digit(s[i]) THEN
        WHILE (i<len) & Digit(s[i]) DO
           val := ORD(s[i]) - ORD('0');
           IF (exp DIV 10) - LONGINT(val) <= MAX(LONGINT) THEN
              exp := 8*exp + 2*exp + LONGINT(val);
           ELSE
              overflow := TRUE;
              RETURN 0.0
           END;
           INC(i)
        END
     ELSE
        done := FALSE;
        RETURN 0.0
     END
  END;
  IF negExp THEN
     exp := -exp
  END;
  pow := Pow10(exp-LONGINT(fracDigits), overflow);
  IF (NOT overflow) AND (mantissa <= MAX(REAL) / REAL(pow)) THEN
     IF neg THEN
        mantissa := -mantissa
     END;
     RETURN mantissa * REAL(pow)
  ELSE
     overflow := TRUE;
     RETURN 0.0
  END;
END StrToReal;

PROCEDURE StrToLongReal(s: ARRAY OF CHAR;
                        VAR done: BOOLEAN; VAR overflow: BOOLEAN): LONGREAL;
   (* Converts the string 's' to long real number.
      'done' is TRUE if conversion was succesful.
      'overflow' is TRUE if overflow occured during conversion.
      This procedure is analogous to the previous one! This one
      cannot be used instaed of the previous one however, because
      greater rounding errors would be generated.
      At the moment do not conform to the BMB Conformance test
      (error < 1.E-20) *)
   (* by Zoran Budimac, July, 1996 *)
VAR
  len, i, fracDigits: CARDINAL;
  neg, negExp: BOOLEAN;
  pow, mantissa: LONGREAL;
  val: CARDINAL;
  exp: LONGINT;

BEGIN
  done := TRUE;
  overflow := FALSE;
  IF String.Length(s) = 0 THEN
     done := FALSE;
     RETURN 0.0
  END;
  len := String.Length(s);
  i := 0;
  neg := FALSE;
  negExp := FALSE;
  mantissa := 0.0;
  exp := 0;
  fracDigits := 0; (* digits behind decimal point *)

  (* ['-'] *)
  IF s[i] = '-' THEN
     neg := TRUE;
     INC(i)
  ELSIF NOT Digit(s[i]) THEN
     done := FALSE;
     RETURN 0.0
  END;
  (* digit(s) *)
  WHILE Digit(s[i]) DO
    val := ORD(s[i]) - ORD('0');
    IF mantissa <= MAX(LONGREAL) / 10.0 - LONGREAL(FLOAT(val)) THEN
       mantissa := mantissa * 10.0 + LONGREAL(FLOAT(val));
    ELSE
       overflow := TRUE;
       RETURN 0.0
    END;
    INC(i);
  END;
  (* decimal point *)
  IF s[i] = '.' THEN
     INC(i);
     (* digit(s) *)
     WHILE (i<len) & Digit(s[i]) DO
       val := ORD(s[i]) - ORD('0');
       IF mantissa <= MAX(LONGREAL) / 10.0 - LONGREAL(FLOAT(val)) THEN
          mantissa := mantissa * 10.0 + LONGREAL(FLOAT(val));
       ELSE
          overflow := TRUE;
          RETURN 0.0
       END;
       INC(i);
       INC(fracDigits)
     END;
  END;
  (* exponent *)
  IF (s[i] = 'e') OR (s[i] = 'E') THEN
     INC(i);
     IF s[i] = '-' THEN
        negExp := TRUE;
        INC(i)
     ELSIF s[i] = '+' THEN
        INC(i)
     END;
     IF (i<len) & Digit(s[i]) THEN
        WHILE (i<len) & Digit(s[i]) DO
           val := ORD(s[i]) - ORD('0');
           IF (exp DIV 10) - LONGINT(val) <= MAX(LONGINT) THEN
              exp := 10 * exp + LONGINT(val);
           ELSE
              overflow := TRUE;
              RETURN 0.0
           END;
           INC(i)
        END
     ELSE
        done := FALSE;
        RETURN 0.0
     END
  END;
  IF negExp THEN
     exp := -exp
  END;
  pow := Pow10(exp-LONGINT(fracDigits), overflow);
  IF (NOT overflow) AND (mantissa <= MAX(LONGREAL) / pow) THEN
     IF neg THEN
        mantissa := -mantissa
     END;
     RETURN mantissa * pow
  ELSE
     overflow := TRUE;
     RETURN 0.0
  END;
END StrToLongReal;

END GenericRealConvs.
