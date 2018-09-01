MODULE TestReal

IMPORT m2, math

VAR err: INTEGER

FUNCTION do(s: STRING, expected: REAL)
VAR
	r: REAL
BEGIN
	r = stor(s)
	print("stor(" + StringToLiteral(s) + "): expected " + expected
	+ ", got " + r + ": ")
	IF is_nan(r) AND is_nan(expected)
	OR NOT is_nan(r) AND (r = expected) THEN
		print("ok\n")
	ELSE
		print("FAILED\n")
		inc(err, 1)
	END
END

BEGIN
	do("NAN", NAN())
	do("nan", NAN())
	do("inf", INF())
	do("INF", INF())
	do("-inf", -INF())
	do("-INF", -INF())
	do("1e12", 1e12)
	print("Total errors: " + err + "\n")
END
