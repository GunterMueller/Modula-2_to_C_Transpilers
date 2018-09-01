MODULE Test;
IMPORT Scr, Wr;
IMPORT InOut;
FROM Scr IMPORT whiteColor, grayColor, blackColor, redColor, greenColor;
VAR b: ARRAY [1..25*80*2] OF BYTE;
    ch: CHAR;
BEGIN
  Scr.FillBlock('A', 0, 0, 80, 5, greenColor, redColor);
  Scr.Save(b);
  InOut.Read(ch);
  Scr.CopyBlock(0, 0, 80, 5, 0, 5);
  InOut.Read(ch);
  Scr.Restore(b)
END Test.
