IMPLEMENTATION MODULE Write;

  FROM Defs IMPORT IdStr, BigStr;
  FROM Printer IMPORT Append, NewLine, BlankLine,
    IndentNextLine, ExdentNextLine;

  IMPORT Fmt, String; (* BMB *)
  IMPORT Import, Ids, Procs;

  VAR
    tmpId, tmpId2: IdStr;
    tmpBig: BigStr;

  PROCEDURE AppendCardToStr(VAR s: ARRAY OF CHAR; n: CARDINAL);
    VAR
      nStr: ARRAY [0..15] OF CHAR;
  BEGIN
    Fmt.Card(nStr, n);
    String.Append(s, "_");
    String.Append(s, nStr);
  END AppendCardToStr;

  PROCEDURE WriteIfDefs(modName: IdStr; ext: ARRAY OF CHAR);
  BEGIN
    Append("#ifndef _M2_");
    Append(modName);
    Append("_");
    Append(ext);
    Append("_"); NewLine;
    Append("#define _M2_");
    Append(modName);
    Append("_");
    Append(ext);
    Append("_"); NewLine;
    BlankLine;
  END WriteIfDefs;

  PROCEDURE WriteIncludes;
    VAR
      written: BOOLEAN;
  BEGIN
    written := FALSE;
    Import.NextModule(tmpId);
    WHILE NOT String.Equal(tmpId, "") DO
      Append("#include <");
      Append(tmpId);
      Append(".m2h>");
      NewLine;
      written := TRUE;
      Import.NextModule(tmpId);
    END;
    Import.NextModule(tmpId);
    WHILE NOT String.Equal(tmpId, "") DO
      Append("#include <");
      Append(tmpId);
      Append(".cpp>");
      NewLine;
      Import.NextModule(tmpId);
    END;
    IF written THEN BlankLine END;
  END WriteIncludes;

  PROCEDURE WriteImportUsings;
    VAR
      written: BOOLEAN;
  BEGIN
    written := FALSE;
    Import.NextModule(tmpId);
    WHILE NOT String.Equal(tmpId, "") DO
      IF NOT Import.IsQualified() THEN
        Import.NextImported(tmpId2);
        WHILE NOT String.Equal(tmpId2, "") DO
          Append("using ");
          Append(tmpId);
          Append("::");
          Append(tmpId2);
          Append(";");
          NewLine;
          written := TRUE;
          Import.NextImported(tmpId2);
        END;
      END;
      Import.NextModule(tmpId);
    END;
    IF written THEN BlankLine END;
  END WriteImportUsings;

  PROCEDURE WriteExportUsings(modName: IdStr; eList: Ids.List);
    VAR
      name: IdStr;
  BEGIN
    Ids.GetNext(eList, name);
    WHILE NOT String.Equal(name, "") DO
      NewLine;
      Append("using ");
      Append(modName);
      Append("::");
      Append(name);
      Append(";");
      Ids.GetNext(eList, name);
    END;
  END WriteExportUsings;

  PROCEDURE WriteTemplateParams(procDesc: Procs.ProcDesc);
    VAR
      arrayLvl, i, templateCount: CARDINAL;
      isVar: BOOLEAN;
      countStr: ARRAY [0..15] OF CHAR;
  BEGIN
    IF Procs.IsTemplate(procDesc) THEN
      templateCount := 1;
      Append("template < ");
      Procs.GetNextTemplateParam(procDesc, tmpId, tmpBig, arrayLvl, isVar);
      IF NOT String.Equal(tmpId, "") THEN
        Append("class _");
        FOR i := 1 TO arrayLvl DO Append("ARRAYOF_") END;
        Append(tmpBig);
        Fmt.Card(countStr, templateCount);
        Append(countStr);
        Procs.GetNextTemplateParam(procDesc, tmpId, tmpBig, arrayLvl, isVar);
        WHILE NOT String.Equal(tmpId, "") DO
          INC(templateCount);
          Append(", class _");
          FOR i := 1 TO arrayLvl DO Append("ARRAYOF_") END;
          Append(tmpBig);
          Fmt.Card(countStr, templateCount);
          Append(countStr);
          Procs.GetNextTemplateParam(procDesc, tmpId, tmpBig, arrayLvl, isVar);
        END;
      END;
      Append(" >");
      NewLine;
    END;
  END WriteTemplateParams;

  PROCEDURE AppendParams(procDesc: Procs.ProcDesc);
    VAR
      arrayLvl, i, templateCount: CARDINAL;
      isVar, isTemplate: BOOLEAN;
      countStr: ARRAY [0..15] OF CHAR;
  BEGIN
    templateCount := 0;
    Procs.GetNextParam(procDesc, tmpId, tmpBig, arrayLvl, isVar, isTemplate);
    IF NOT String.Equal(tmpId, "") THEN
      IF isTemplate AND (Procs.level = 1) THEN
        INC(templateCount);
        Append("_");
        FOR i := 1 TO arrayLvl DO Append("ARRAYOF_") END;
        Fmt.Card(countStr, templateCount);
      ELSE
        FOR i := 1 TO arrayLvl DO Append("_ARRAYOF< ") END;
      END;
      Append(tmpBig);
      IF isTemplate AND (Procs.level = 1) THEN
        Append(countStr);
      ELSE
        FOR i := 1 TO arrayLvl DO Append(" >") END;
      END;
      IF isVar THEN Append("&") END;
      Append(" ");
      IF isTemplate THEN Append("_") END;
      Append(tmpId);
      Procs.GetNextParam(procDesc, tmpId, tmpBig, arrayLvl, isVar, isTemplate);
      WHILE NOT String.Equal(tmpId, "") DO
        Append(", ");
        IF isTemplate AND (Procs.level = 1) THEN
          INC(templateCount);
          Append("_");
          FOR i := 1 TO arrayLvl DO Append("ARRAYOF_") END;
          Fmt.Card(countStr, templateCount);
        ELSE
          FOR i := 1 TO arrayLvl DO Append("_ARRAYOF< ") END;
        END;
        Append(tmpBig);
        IF isTemplate AND (Procs.level = 1) THEN
          Append(countStr);
        ELSE
          FOR i := 1 TO arrayLvl DO Append(" >") END;
        END;
        IF isVar THEN Append("&") END;
        Append(" ");
        IF isTemplate THEN Append("_") END;
        Append(tmpId);
        Procs.GetNextParam(procDesc, tmpId, tmpBig, arrayLvl, isVar, isTemplate);
      END;
    END;
  END AppendParams;

  PROCEDURE WriteParamReferences(procDesc: Procs.ProcDesc);
    VAR
      arrayLvl, i: CARDINAL;
      isVar, isTemplate: BOOLEAN;
  BEGIN
    Procs.GetNextParam(procDesc, tmpId, tmpBig, arrayLvl, isVar, isTemplate);
    WHILE NOT String.Equal(tmpId, "") DO
      IF isTemplate AND (Procs.level = 1) THEN
        IF arrayLvl = 0 THEN Append("_SINGLEOF< ") END;
        FOR i := 1 TO arrayLvl DO Append("_ARRAYOF< ") END;
        Append(tmpBig);
        IF arrayLvl = 0 THEN Append(" >") END;
        FOR i := 1 TO arrayLvl DO Append(" >") END;
        Append(" ");
        Append(tmpId);
        Append("(_DATAADR(_");
        Append(tmpId);
        Append("), _DATASIZE(_");
        Append(tmpId);
        Append("))");
        Append(";");
        NewLine;
(*    ELSE
        FOR i := 1 TO arrayLvl DO Append("_ARRAYOF< ") END;
        Append(tmpBig);
        FOR i := 1 TO arrayLvl DO Append(" >") END;
        Append("& ");
        Append(tmpId);
        Append(" = _");
        Append(tmpId);
        Append(";");
        NewLine;         *)
      END;
      Procs.GetNextParam(procDesc, tmpId, tmpBig, arrayLvl, isVar, isTemplate);
    END;
  END WriteParamReferences;

  PROCEDURE WriteModuleBodyStruct;
  BEGIN
    Append("struct _ModuleBody {");
    IndentNextLine;
    Append("_ModuleBody()  { BEGIN(); }");
    NewLine;
    Append("~_ModuleBody() { FINALLY(); }");
    NewLine; BlankLine;
    Append("void BEGIN();");
    NewLine;
    Append("void FINALLY();");
    ExdentNextLine;
    Append("};");
    NewLine; BlankLine;
  END WriteModuleBodyStruct;

  PROCEDURE WriteModuleBodyVar;
  BEGIN
    Append("_ModuleBody _body;");
    NewLine; BlankLine;
  END WriteModuleBodyVar;

  PROCEDURE WriteMain(modName: IdStr);
  BEGIN
    BlankLine;
    Append("int main(int argc, char* argv[]) {");
    IndentNextLine;
    Append("Env::_init(argc, argv);"); NewLine;
    Append("Env::_ModuleBody _Env_body;"); NewLine;
    Append(modName);
    Append("::_ModuleBody _body;"); NewLine;
    Append("return 0;");
    ExdentNextLine;
    Append("}");
    NewLine;
  END WriteMain;

END Write.
