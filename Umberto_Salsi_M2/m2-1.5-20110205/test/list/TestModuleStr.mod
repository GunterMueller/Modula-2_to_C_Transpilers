MODULE TestModuleStr

IMPORT m2, str

VAR
	err_count: INTEGER

FUNCTION btos(b: BOOLEAN): STRING
BEGIN
	IF b THEN
		RETURN "TRUE"
	ELSE
		RETURN "FALSE"
	END
END


FUNCTION do1(s: STRING, h: STRING, expected: BOOLEAN)
VAR
	b: BOOLEAN
BEGIN
	b = str.starts_with(s, h)
	print("str.starts_with(" + StringToLiteral(s)
	+ ", " + StringToLiteral(h) + "): expected "
	+ btos(expected)
	+ ", got " + btos(b))
	IF eq(b, expected) THEN
		print(": ok\n")
	ELSE
		print(": FAILED\n")
		inc(err_count, 1)
	END
END


FUNCTION do2(s: STRING, t: STRING, expected: BOOLEAN)
VAR
	b: BOOLEAN
BEGIN
	b = str.ends_with(s, t)
	print("str.ends_with(" + StringToLiteral(s)
	+ ", " + StringToLiteral(t) + "): expected "
	+ btos(expected)
	+ ", got " + btos(b))
	IF eq(b, expected) THEN
		print(": ok\n")
	ELSE
		print(": FAILED\n")
		inc(err_count, 1)
	END
END


BEGIN
	(*
		str.starts_with() test:
	*)

	do1(NIL, NIL, TRUE)
	do1(NIL, "", FALSE)
	do1(NIL, "a", FALSE)

	do1("", NIL, TRUE)
	do1("", "", TRUE)
	do1("", "a", FALSE)

	do1("a", NIL, TRUE)
	do1("a", "", TRUE)
	do1("a", "a", TRUE)
	do1("a", "abcd", FALSE)

	do1("abcd", NIL, TRUE)
	do1("abcd", "", TRUE)
	do1("abcd", "a", TRUE)
	do1("abcd", "abcd", TRUE)
	do1("abcd", "abcde", FALSE)

	do1("abc", "xyz", FALSE)


	(*
		str.ends_with() test:
	*)

	do2(NIL, NIL, TRUE)
	do2(NIL, "", FALSE)
	do2(NIL, "a", FALSE)

	do2("", NIL, TRUE)
	do2("", "", TRUE)
	do2("", "a", FALSE)

	do2("a", NIL, TRUE)
	do2("a", "", TRUE)
	do2("a", "a", TRUE)
	do2("a", "abcd", FALSE)

	do2("abcd", NIL, TRUE)
	do2("abcd", "", TRUE)
	do2("abcd", "d", TRUE)
	do2("abcd", "abcd", TRUE)
	do2("abcd", "zabcd", FALSE)

	do2("abc", "xyz", FALSE)


	print("Total errors: " + err_count + "\n")
END
