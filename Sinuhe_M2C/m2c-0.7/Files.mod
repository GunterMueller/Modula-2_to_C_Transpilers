IMPLEMENTATION MODULE Files;
FROM SYSTEM IMPORT ADR;
FROM ASCII IMPORT nul;
FROM CPROC IMPORT mfopen,mfclose,munlink,mfseek,mftell,mfeof,mferror,
		  mclearerr,mfwrite,mfread;


TYPE
 File=ADDRESS;


CONST CFNamelong=80;
TYPE CFName=ARRAY [0..CFNamelong] OF CHAR;

%{

#include <stdio.h>

extern FILE *fopen();
extern long ftell();

#define mfopen(f,name,mode) (f=(void *)fopen((char *)name,(char *)mode))
#define mfclose(f) fclose((FILE *)f)
#define munlink(name) unlink((char *)name)
#define mftell(f,hp,lp) (hp=(unsigned)(ftell((FILE *)f)>>16),\
			 lp=ftell((FILE *)f)&0177777l)
#define mfseek(f,hp,lp,wh) fseek((FILE *)f,((long)hp)<<16|lp,wh)
#define mfeof(res,f) res=feof((FILE *)f)
#define mferror(res,f) res=ferror((FILE *)f)
#define mclearerr(f) clearerr(((FILE *)f))
#define mfread(ptr,size,nel,f) fread(ptr,size,nel,(FILE *)f)
#define mfwrite(ptr,size,nel,f) fwrite(ptr,size,nel,(FILE *)f)
%}

PROCEDURE Create(filename:ARRAY OF CHAR; m:Mode):File;
 VAR f:File;i:INTEGER;cfn:CFName;
 BEGIN
  GetCFName(cfn,filename);
  IF m=RWmode THEN mfopen(f,cfn,"w+");
  ELSE
   mfopen(f,cfn,"w");
   IF (f#NIL) & (m=Rmode) THEN
    i:=mfclose(f);
    IF i=-1 THEN f:=NIL;ELSE mfopen(f,cfn,"r");END;
   END;
  END;
  status:=Done;
  IF f=NIL THEN status:=NotDone;END;
  RETURN f;
 END Create;

PROCEDURE Open(filename:ARRAY OF CHAR; m:Mode):File;
 VAR f:File;i:INTEGER;cfn:CFName;h,l:CARDINAL;
 BEGIN
  GetCFName(cfn,filename);
  IF m=Rmode THEN mfopen(f,cfn,"r");
  ELSIF m=Wmode THEN
   mfopen(f,cfn,"a");IF f#NIL THEN h:=0;l:=0;i:=mfseek(f,h,l,0);END;
  ELSE mfopen(f,cfn,"r+");END;
  status:=Done;
  IF f=NIL THEN status:=NotDone;END;
  RETURN f;
 END Open;

PROCEDURE Close(f:File);
 BEGIN
  IF mfclose(f)#-1 THEN status:=Done;
  ELSE status:=NotDone;END;
 END Close;

PROCEDURE Delete(filename:ARRAY OF CHAR);
 VAR cfn:CFName;
 BEGIN
  GetCFName(cfn,filename);
  IF munlink(cfn)#-1 THEN status:=Done;
  ELSE status:=NotDone;END;
 END Delete;

PROCEDURE SetPos(f:File;highpos,lowpos:CARDINAL);
 BEGIN
  IF mfseek(f,highpos,lowpos,0)#-1 THEN status:=Done;
  ELSE status:=NotDone;END;
 END SetPos;

PROCEDURE GetPos(f:File;VAR highpos,lowpos:CARDINAL);
 BEGIN 
  mftell(f,highpos,lowpos);
 END GetPos;

PROCEDURE Length(f:File;VAR highpos,lowpos:CARDINAL);
 VAR h,l:CARDINAL;i:INTEGER;
 BEGIN
  mftell(f,h,l);highpos:=0;lowpos:=0;i:=mfseek(f,highpos,lowpos,2);
  mftell(f,highpos,lowpos);i:=mfseek(f,h,l,0);
 END Length;

PROCEDURE IsEof(f:File):BOOLEAN;
 VAR i:INTEGER;
 BEGIN
  mfeof(i,f);RETURN i#0;
 END IsEof;

PROCEDURE GetNErr(f:File):CARDINAL;
 VAR c:CARDINAL;
 BEGIN 
  mferror(c,f);RETURN c;
 END GetNErr;

PROCEDURE ClearErr(f:File);
 BEGIN 
  mclearerr(f);
 END ClearErr;

PROCEDURE ReadNBytes(f:File;p:ADDRESS;nbytes:CARDINAL):CARDINAL;
 VAR rnb:CARDINAL;
 BEGIN
  rnb:=mfread(p,1,nbytes,f);
  IF rnb#nbytes THEN status:=NotDone;ELSE status:=Done; END;
  RETURN rnb;
 END ReadNBytes;

PROCEDURE WriteNBytes(f:File;p:ADDRESS;nbytes:CARDINAL):CARDINAL;
 VAR wnb:CARDINAL;
 BEGIN
  wnb:=mfwrite(p,1,nbytes,f);
  IF wnb#nbytes THEN status:=NotDone;ELSE status:=Done; END;
  RETURN wnb;
 END WriteNBytes;

PROCEDURE ReadChar(f:File;VAR ch:CHAR);
 BEGIN
  IF mfread(ADR(ch),1,1,f)#1 THEN status:=NotDone;
  ELSE status:=Done; END;
 END ReadChar;

PROCEDURE WriteChar(f:File;ch:CHAR);
 BEGIN
  IF mfwrite(ADR(ch),1,1,f)#1 THEN status:=NotDone;
  ELSE status:=Done; END;
 END WriteChar;

PROCEDURE ReadWord(f:File;VAR w:WORD);
 BEGIN
  IF mfread(ADR(w),1,SIZE(WORD),f)#SIZE(WORD) THEN status:=NotDone;
  ELSE status:=Done; END;
 END ReadWord;

PROCEDURE WriteWord(f:File;w:WORD);
 BEGIN
  IF mfwrite(ADR(w),1,SIZE(WORD),f)#SIZE(WORD) THEN
   status:=NotDone;
  ELSE
   status:=Done;
  END;
 END WriteWord;

 (* Internal procedure: transform filename to null-ended string cfn *)
PROCEDURE GetCFName(VAR cfn:CFName;VAR filename:ARRAY OF CHAR);
 VAR i:CARDINAL;
 BEGIN
  i:=0;
  WHILE (i<=CARDINAL(HIGH(cfn))) & (i<=CARDINAL(HIGH(filename)))
         & (filename[i]#nul) DO
   cfn[i]:=filename[i];INC(i);
  END;
  IF i>CARDINAL(HIGH(cfn)) THEN i:=CARDINAL(HIGH(cfn));END;
  cfn[i]:=nul;
 END GetCFName;

BEGIN
END Files.

