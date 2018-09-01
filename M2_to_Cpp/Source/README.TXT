===============================================
C2 - a Modula-2 to C++ translator, version 0.90
===============================================

Welcome to C2, a Modula-2 to C++ translator. What it does is take a single
Modula-2 source program, check its lexis and syntax, and output a
(hopefully) correct C++ program. It is probably not something
every Modula-2 programmer ever dreamed about, but more a collection of
ideas I had and my determination to get them to compile right. It also a
result of my determination to do a project for the Compiler Construction
course in 2000/2001, which made the exam a whole lot easier.

Archive C2-090.ZIP should contain the following:

  CPPBMB        - directory with BMB M2 Cross Platform Library, ported to C++
  CPPLIB        - directory with other C++ libraries
  EXAMPLES      - directory with some examples for C2 and BMB C++ library
  M2BMB         - directory with BMB M2 Cross Platform Library
  SRC           - directory with source code of C2
  C2.EXE        - main (and only) executable
  README.TXT    - this file

Installation is simple - just unzip the archive into an empty directory,
and put it in your PATH. To compile the resulting C++ programs, paths of
subdirectories CPPLIB and CPPBMB need to be added to the include path of
your C++ compiler. For DJGPP v2, it is section CPLUS_INCLUDE_PATH in
djgpp.env file.

Usage is also simple: C2 <input file> [-s]

For example, from this directory, type: C2 EXAMPLES\Ex1.mod
and you will get Ex1.cpp. The name of the output file is determined by
the name of the module in the input file, with the extension ".m2h", if it
is a definition module, or ".cpp" otherwise.

The optional switch -s, if specified, means that every string literal in
the output C++ source will be surrounded by "_ARRAYOF<CHAR>(" and ")". I
will explain later.

The C++ compiler used for developing and testing was DJGPP v2, with g++
version 2.95.2. Although I did a lot to make the generated C++ as portable
as possible, I don't expect it to work "out of the box" with any other
C++ compiler. BMB Modula-2 Cross Platform Library C++ Port works, however,
with Borland C++ 5.5 (free compiler).

C2 was written in Modula-2, using COCO/R to generate the compiler, scanner
and parser modules. The C2.ATG file I constructed from the M2 syntax in EBNF
I found on the ISO WG13 site, although I took a peek at the ATGs that come
with COCO/R. So, the input language is ISO Modula-2 '96 base language. C2 has
come to the stage where it can translate its own source, and the output
can cleanly be compiled with the DJGPP C++ compiler under DOS. (Not very
spectacular, but it took me a lot to achieve this.) See the SRC subdirectory
for details. Semantic analysis (type and scope checking) is not done (to make
things simpler), but in some situations it proved to be more of a limitation
than a simplification. Without it, there is still some room for improvement,
but, great improvement would only be possible by adding semantic analysis and
radically changing the concept. In other words, by rewriting C2. Furthermore,
without semantic analysis, an infinite number of incorrect Modula-2 programs
can be translated.

This concept I mentioned is to, basically, implement Modula-2 language
features in C++ using classes and templates (see CPPLIB\_m2sys.h for
details). This makes C++ code more readable and looking more like Modula-2.
On the other hand, it imposes some limitations, particularly with low-level
Modula-2 features, some of which I didn't forsee (most of them I did, though).
Also, the resulting C++ executable can be slower than the equivalent
Modula-2 one.

I take pretty great pride in many Modula-2 constructs C2 successfully
translates. I will mention several I had to give some extra thought:
  - open array formal parameters of procedures
  - LOC, BYTE, WORD fomal parameters
  - open array of LOC, BYTE or WORD formal parameters
  - subranges, and their compatibility with their base types
  - ARRAY type, and its compatibility with open arrays
  - ADDRESS and POINTER types, and their compatibility
  - SETs and PACKEDSETs, BITSET
  - constructed values of sets
  - variant records
  - local procedures
  - IMPORTing
  - EXPORTing from local modules
  - etc.

Now, for the missing features and limitations. I will try to group them
somehow:

  1. Limitations that cannot be overcome without _radical_ changes
     - no support for coroutines, threads, monitors etc. since ISO C++
       doesn't support equivalent concepts, and I wanted to make everything
       as portable as possible
     - pointers to undefined types that are not records are not supported,
       and pointers to undefined records are supported only in
       _definitions_ and _declarations_ of a pointer type, not when
       declaring variables etc.
     - no local modules declared in procedures
     - no opaque types (I tried and tried, but could not come up with
       something satisfying)
     - no procedure type parameter types {ARRAY OF} LOC, BYTE, WORD
     - ALLOCATE and DEALLOCATE have a whole story behind them. Read on...
     - no support for variant record fields of ARRAY, POINTER, SET, PACKEDSET,
       BITSET, COMPLEX, LONGCOMPLEX type
     - procedures declared in other procedures have severe limitations:
        * cannot have parameters of type {ARRAY OF} LOC, BYTE, WORD
        * cannot access parameters or local variables of "host" procedures

  2. Limitations that can be overcome with some extra effort (like adding
     semantic analysis, but not changing the concept)
     - WITH statement is commented
     - constants are all type INTEGER
     - subranges are all taken from INTEGER
     - EXCEPT and following statement sequence are commented
     - RETRY is commented (this might go to category 1.)
     - no ISO Modula-2 library. Instead, there is the BMB Modula-2 Cross
       Platform Library, developed at my faculty
     - no libraries from PIM (except Storage)
     - no constructed values of records
     - no constructed values of arrays
     - no IMPORT/EXPORT of enumeration constants along with the
       enumeration type
     - no specifying machine addresses for variables (not sure...)
     - procedures declared in other procedures cannot recursively call
       themselves
     - no automatic detection should a C++ string literal be placed
       within "_ARRAYOF<CHAR>(" and ")"

  3. Limitations that can be comfortably overcome within the current concept
     - comments are stripped
     - no warning/renaming of C++ identifiers in a Modula-2 program
     - no warnings/errors about unsupported M2 features
     - FOR might not work in some *very* special cases
       (haven't tested it that thoroughly)

  4. Limitations I don't think really matter
     - no redefinition of pervasive identifiers
     - no redefinition of identifiers from module SYSTEM

  5. Limitations that are probably not my fault
     - MIN(REAL), MAX(REAL), MIN(LONGREAL), MAX(LONGREAL) don't work with
       DJGPP (I think there is a bug in DJGPP)

The -s switch should be used if a string literal is passed as a parameter
to a procedure, and the formal parameter is of type ARRAY OF LOC, BYTE or
WORD. This is because string literals in C++ are of type 'const char*',
and without -s, the open array would take the pointer, not the string itself,
which was the obvious intention. To be on the safe side, you can always
specify -s. The resulting code is simply more readable without it.

The story behind ALLOCATE and DEALLOCATE is long and painful. I will try to
be brief. The whole problem is that ARRAYs and other classes from _m2sys.h
all have their constructors and destructors, which have to be called in
order for the program to work properly. C++ operators 'new' and 'delete'
ensure that. Modula-2 NEW and DISPOSE and implemented using them. But,
sometimes there is a need to (DE)ALLOCATE more or less memory than
the type pointed to requires. This is done in C++ using _functions_
operator new() and operator delete(), which do not call constructors or
destructors. In short these are general guidelines for M2 programs:
  1. Use NEW and DISPOSE wherever possible
  2. Do NOT use (DE)ALLOCATE on pointers to arrays, variant records, sets,
     packed sets, bitsets, and complex types, with the parameter
     size # TSIZE(type)

Please feel free to copy, distribute and use this software in any way 
compliant with the Creative Commons Attribution-ShareAlike 4.0 International
License. Please bear in mind that COCO/R is not completely free software.

One more thing. This archive and its contents come without any warranty of
any kind. If anything should happen to anything or anyone because of this
archive or its contents, the author can not be held responsible in any way.
I hope this covers it.

In the end, here are some links:

  http://www.dmi.uns.ac.rs                    - my department home page
  http://sc22wg13.twi.tudelft.nl              - Modula-2 ISO work group
  http://www.arjay.bc.ca/Modula-2/m2faq.html  - Modula-2 FAQ
  http://upx.sourceforge.net/                 - UPX executable packer


        Milos Radovanovic

            University of Novi Sad, Serbia
            e-mail: radacha@dmi.uns.ac.rs
