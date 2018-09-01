#define DEFINITION_Errors

#ifndef DEFINITION_Positions
#include "Positions.h"
#endif

#ifndef DEFINITION_IO
#include "IO.h"
#endif

#define Errors_NoText	0
#define Errors_SyntaxError	1
#define Errors_ExpectedTokens	2
#define Errors_RestartPoint	3
#define Errors_TokenInserted	4
#define Errors_WrongParseTable	5
#define Errors_OpenParseTable	6
#define Errors_ReadParseTable	7
#define Errors_TooManyErrors	8
#define Errors_Fatal	1
#define Errors_Restriction	2
#define Errors_Error	3
#define Errors_Warning	4
#define Errors_Repair	5
#define Errors_Note	6
#define Errors_Information	7
#define Errors_None	0
#define Errors_Integer	1
#define Errors_Short	2
#define Errors_Long	3
#define Errors_Real	4
#define Errors_Boolean	5
#define Errors_Character	6
#define Errors_String	7
#define Errors_Array	8
#define Errors_Set	9
#define Errors_Ident	10
extern PROC Errors_Exit;
extern void Errors_StoreMessages ARGS((BOOLEAN Store));
extern void Errors_ErrorMessage ARGS((CARDINAL ErrorCode, CARDINAL ErrorClass, Positions_tPosition Position));
extern void Errors_ErrorMessageI ARGS((CARDINAL ErrorCode, CARDINAL ErrorClass, Positions_tPosition Position, CARDINAL InfoClass, ADDRESS Info));
extern void Errors_Message ARGS((CHAR ErrorText[], LONGCARD , CARDINAL ErrorClass, Positions_tPosition Position));
extern void Errors_MessageI ARGS((CHAR ErrorText[], LONGCARD , CARDINAL ErrorClass, Positions_tPosition Position, CARDINAL InfoClass, ADDRESS Info));
extern void Errors_WriteMessages ARGS((IO_tFile File));
extern void BEGIN_Errors();
