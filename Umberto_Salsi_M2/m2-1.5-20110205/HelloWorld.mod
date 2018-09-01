MODULE HelloWorld
(*
	If you have properly installed the M2 development system, you may want
	to start with this simple program. Then type this command:

	m2 -r HelloWorld.mod

	This command will invoke the bin/m2 bash script which, in turn, invoke
	the actual M2 compiler under src/m2c; once the compilation process is
	completed, the resulting program ./HelloWorld (HelloWorld.exe on
	Windows) is executed automatically. The following messages should
	appear:

	Executing HelloWorld :
	Hello, world
	Exit code: 0

	Congratulations, you have successfully compiled your first M2 program!
*)

IMPORT m2

BEGIN
	print("Hello, world\n")
END
