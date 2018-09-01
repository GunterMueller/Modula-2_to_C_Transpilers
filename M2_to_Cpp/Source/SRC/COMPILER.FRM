(* Modified by Milos Radovanovic, 2000, 2001 *)

(* Used with COCO/R 1.50 *)
MODULE -->Grammar;
(* This is an example of a rudimentary main module for use with COCO/R.
   It assumes FileIO library will be available.
   The auxiliary modules <Grammar>S (scanner) and <Grammar>P (parser)
   are assumed to have been constructed with COCO/R compiler generator. *)

  FROM -->Scanner IMPORT lst, src, errors, Error, CharAt;
  FROM -->Parser IMPORT Parse, Successful;
  IMPORT SYSTEM, Storage, FileIO, Switches;
  FROM Printer IMPORT
    resultsName,  (* destination file name *)
    CloseResults; (* close results file *)
  FROM Defs IMPORT Version, Subversion;

  TYPE
    INT32 = FileIO.INT32;

  MODULE ListHandler;
  (* ------------------- Source Listing and Error handler -------------- *)
    IMPORT FileIO, Storage, SYSTEM;
    IMPORT lst, CharAt, errors, INT32;
    EXPORT StoreError, PrintListing;

    TYPE
      Err = POINTER TO ErrDesc;
      ErrDesc = RECORD
        nr, line, col: INTEGER;
        next: Err
      END;

    VAR
      tab: CHAR; (* 11C *)
      firstErr, lastErr: Err;
      Extra: INTEGER;

    PROCEDURE StoreError (nr, line, col: INTEGER; pos: INT32);
    (* Store an error message for later printing *)
      VAR
        nextErr: Err;
      BEGIN
        Storage.ALLOCATE(nextErr, SYSTEM.TSIZE(ErrDesc));
        nextErr^.nr := nr; nextErr^.line := line; nextErr^.col := col;
        nextErr^.next := NIL;
        IF firstErr = NIL
          THEN firstErr := nextErr
          ELSE lastErr^.next := nextErr
        END;
        lastErr := nextErr;
        INC(errors)
      END StoreError;

    PROCEDURE GetLine (VAR pos: INT32;
                       VAR line: ARRAY OF CHAR;
                       VAR eof: BOOLEAN);
    (* Read a source line. Return empty line if eof *)
      VAR
        ch: CHAR;
        i:  CARDINAL;
      BEGIN
        i := 0; eof := FALSE; ch := CharAt(pos); INC(pos);
        WHILE (ch # FileIO.CR) & (ch # FileIO.LF) & (ch # FileIO.EOFILE) DO
          line[i] := ch; INC(i); ch := CharAt(pos); INC(pos);
        END;
        eof := (i = 0) & (ch = FileIO.EOFILE); line[i] := 0C;
        IF ch = FileIO.CR THEN (* check for MsDos *)
          ch := CharAt(pos);
          IF ch = FileIO.LF THEN INC(pos); Extra := 0 END
        END
      END GetLine;

    PROCEDURE PrintErr (line: ARRAY OF CHAR; nr, col: INTEGER);
    (* Print an error message *)

      VAR
        i: INTEGER;

      PROCEDURE Msg (s: ARRAY OF CHAR);
        BEGIN
          FileIO.WriteString(lst, s)
        END Msg;

      BEGIN
        tab := 11C;

        FileIO.WriteString(lst, "*****  ");
        i := 0;
        WHILE i < col + Extra - 2 DO
          IF line[i] = tab
            THEN FileIO.Write(lst, tab)
            ELSE FileIO.Write(lst, ' ')
          END;
          INC(i)
        END;
        FileIO.WriteString(lst, "^ ");

        CASE nr OF
        -->Errors
        (* add customized cases here *)
        ELSE Msg("Error: "); FileIO.WriteInt(lst, nr, 0);
        END;
        FileIO.WriteLn(lst)
      END PrintErr;

    PROCEDURE PrintListing;
    (* Print a source listing with error messages *)
      VAR
        nextErr:   Err;
        eof:       BOOLEAN;
        lnr, errC: INTEGER;
        srcPos:    INT32;
        line:      ARRAY [0 .. 255] OF CHAR;
      BEGIN
        FileIO.WriteString(lst, "Listing:");
        FileIO.WriteLn(lst); FileIO.WriteLn(lst);
        srcPos := FileIO.Long0; nextErr := firstErr;
        GetLine(srcPos, line, eof); lnr := 1; errC := 0;
        WHILE ~ eof DO
          FileIO.WriteInt(lst, lnr, 5); FileIO.WriteString(lst, "  ");
          FileIO.WriteString(lst, line); FileIO.WriteLn(lst);
          WHILE (nextErr # NIL) & (nextErr^.line = lnr) DO
            PrintErr(line, nextErr^.nr, nextErr^.col); INC(errC);
            nextErr := nextErr^.next
          END;
          GetLine(srcPos, line, eof); INC(lnr);
        END;
        IF nextErr # NIL THEN
          FileIO.WriteInt(lst, lnr, 5); FileIO.WriteLn(lst);
          WHILE nextErr # NIL DO
            PrintErr(line, nextErr^.nr, nextErr^.col); INC(errC);
            nextErr := nextErr^.next
          END
        END;
        FileIO.WriteLn(lst);
        FileIO.WriteInt(lst, errC, 5); FileIO.WriteString(lst, " error");
        IF errC # 1 THEN FileIO.Write(lst, "s") END;
        FileIO.WriteLn(lst); FileIO.WriteLn(lst); FileIO.WriteLn(lst);
      END PrintListing;

    BEGIN
      firstErr := NIL; Extra := 1;
    END ListHandler;

  (* --------------------------- main module ------------------------------- *)

  VAR
    sourceName, listName, switch1: ARRAY [0 .. 255] OF CHAR;

  BEGIN
    FileIO.WriteString(FileIO.StdOut,
      "C2 - a Modula-2 to C++ translator, version");
    FileIO.WriteInt(FileIO.StdOut, Version, 2);
    FileIO.WriteString(FileIO.StdOut, ".");
    FileIO.WriteInt(FileIO.StdOut, Subversion, 2);
    FileIO.WriteLn(FileIO.StdOut);

    (* check on correct parameter usage *)
    FileIO.NextParameter(sourceName);
    IF FileIO.SLENGTH(sourceName) = 0 THEN
      FileIO.WriteString(FileIO.StdOut, "Usage: C2 <input file> [-s]");
      FileIO.WriteLn(FileIO.StdOut);
      FileIO.WriteString(FileIO.StdOut, "  -s   Write string_literal as _ARRAYOF<CHAR>(string_literal)");
      FileIO.WriteLn(FileIO.StdOut);
      FileIO.QuitExecution
    END;

    (* check for switches *)
    FileIO.NextParameter(switch1);
    IF FileIO.SLENGTH(switch1) # 0 THEN
      IF FileIO.Compare(switch1, "-s") = 0 THEN
        Switches.s := TRUE;
      ELSE
        FileIO.WriteString(FileIO.StdOut, "Unknown switch `");
        FileIO.WriteString(FileIO.StdOut, switch1);
        FileIO.WriteString(FileIO.StdOut, "'");
        FileIO.WriteLn(FileIO.StdOut);
        FileIO.QuitExecution
      END
    END;

    (* open the source file - Scanner.src *)
    FileIO.Open(src, sourceName, FALSE);
    IF ~ FileIO.Okay THEN
      FileIO.WriteString(FileIO.StdOut, "Could not open input file `");
      FileIO.WriteString(FileIO.StdOut, sourceName);
      FileIO.WriteString(FileIO.StdOut, "'");
      FileIO.WriteLn(FileIO.StdOut);
      FileIO.QuitExecution;
    END;

    (* install error reporting procedure - Scanner.Error *)
    Error := StoreError;

    (* instigate the compilation - Parser.Parse *)
    FileIO.WriteString(FileIO.StdOut, "Parsing"); FileIO.WriteLn(FileIO.StdOut);

    Parse; (* Parse will call OpenResults *)

    (* examine the outcome *)
    IF NOT Successful()
      THEN
        (* open the output file for the source listing - Scanner.lst *)
        FileIO.ChangeExtension(sourceName, ".LST", listName);
        FileIO.Open(lst, listName, TRUE);
        IF ~ FileIO.Okay THEN
          FileIO.WriteString(FileIO.StdOut, "Could not open listing file `");
          FileIO.WriteString(FileIO.StdOut, listName);
          FileIO.WriteString(FileIO.StdOut, "'");
          FileIO.WriteLn(FileIO.StdOut);
        ELSE
          (* generate the source listing on Scanner.lst *)
          PrintListing; FileIO.Close(lst);
          FileIO.WriteString(FileIO.StdOut, "Incorrect source, see `");
          FileIO.WriteString(FileIO.StdOut, listName);
          FileIO.WriteString(FileIO.StdOut, "' for details.");
          IF FileIO.SLENGTH(resultsName) > 0 THEN
            FileIO.WriteLn(FileIO.StdOut);
            FileIO.WriteString(FileIO.StdOut, "See `");
            FileIO.WriteString(FileIO.StdOut, resultsName);
            FileIO.WriteString(FileIO.StdOut, "' for partial results.");
          END;
        END;
      ELSE
        FileIO.WriteString(FileIO.StdOut, "Parsed correctly, see `");
        FileIO.WriteString(FileIO.StdOut, resultsName);
        FileIO.WriteString(FileIO.StdOut, "' for the results.");
        (* ++++++++ Add further activities if required ++++++++++ *)
    END;
    CloseResults;
  END -->Grammar.
