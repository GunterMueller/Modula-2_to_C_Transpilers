IMPLEMENTATION MODULE Storage;
FROM CPROC IMPORT malloc,free;

%{
#include <stdlib.h>
%}

 PROCEDURE ALLOCATE(VAR a:ADDRESS;size:CARDINAL);
  BEGIN
   a:=malloc(size);
   IF a=NIL THEN HALT;END;
  END ALLOCATE;

 PROCEDURE DEALLOCATE(VAR a:ADDRESS;size:CARDINAL);
  BEGIN
   free(a);
  END DEALLOCATE;

 PROCEDURE Available(size:CARDINAL):BOOLEAN;
  VAR a:ADDRESS;
  BEGIN
   a:=malloc(size);
   IF a#NIL THEN free(a); END;
   RETURN a#NIL;
  END Available;

BEGIN
END Storage.
