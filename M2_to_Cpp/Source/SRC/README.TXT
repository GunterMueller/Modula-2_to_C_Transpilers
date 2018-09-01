In order to compile the sources, make sure of the following:

0. You have COCO/R 1.50 compiler generator installed and in your PATH
   (this is not obligatory, I left all sources it generates here)
   
1. You have JPI Modula-2 1.x installed and in your PATH, with the complete
   path of M2BMB directory added to the *.DEF and *.MOD sections
   of file M2.RED in JPI Modula-2 home directory.

   Now, run 1.BAT, and COCO/R will generate all necessary code for the
translator. I already did this step.

   Then, run 2.BAT, and you will get C2.EXE.

   If you wish to translate the sources to C++, run 3.BAT. In order to compile
these, make sure of the following:

2. You have DJGPP v2 installed and in your PATH, and g++ is version 2.95.2.
   The complete path of directories CPPLIB and CPPBMB needs to be added to
   the C++ include path section of file djgpp.env in DJGPP home directory.

   Then, run 4.BAT, and you will get c2dj.exe.

   If you wish to translate the sources to C++ with c2dj.exe, run 5.BAT.

   To compile these, run 6.BAT, and you will get c2dj2.exe. By renaming
c2dj2.exe to C2.EXE, and compressing it with UPX 1.04 executable packer,
I made the executable in C2 home directory.

   If you wish to translate the sources to C++ with c2dj2.exe, (just to
make sure) run 7.BAT.


Note: Modula-2 sources can also be compiled with TopSpeed Modula-2 3.x, but
since v3.2 locks up my computer ocasionally at link time (or aborts with
"Unexpected loader error"), I chose to stick with v1.
