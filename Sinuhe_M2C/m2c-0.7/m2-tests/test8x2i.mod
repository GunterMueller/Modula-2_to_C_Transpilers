(* p.143 5t-7t *)

(* Relations *)

(* The ordering relations apply to the basic types  *)
(*   INTEGER, CARDINAL, BOOLEAN, CHAR, REAL,        *)
(*   to enumerations, and to sybrange types.        *)

(* < operation is not applied to set *)

MODULE test8x2i;
 VAR
    s1,s2: BITSET;
    r: BOOLEAN;
 BEGIN
  s1:={0,1,2};
  s2:={2,1};
  r:=(s1<s2);
 END test8x2i.
