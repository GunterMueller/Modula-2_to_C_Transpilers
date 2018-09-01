/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <FileRd.m2h>
#include <FileWr.m2h>
#include <File.m2h>
#include <FileRd.cpp>
#include <FileWr.cpp>
#include <File.cpp>

const File::T I = File::StdIn();
const File::T O = File::StdOut();

BOOLEAN b;
INTEGER i;
LONGINT li;
CARDINAL c;
LONGCARD lc;
REAL r;
LONGREAL lr;
CHAR ch;
ARRAY <_SubrangeType<CARDINAL, 1, 100>, CHAR> str;

int main() {
  FileWr::Str(O, "Enter a BOOLEAN: ");
  b = FileRd::Bool(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::Bool(O, b, 0);
  FileWr::Ln(O);

  FileWr::Str(O, "Enter an INTEGER: ");
  i = FileRd::Int(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::Int(O, i, 0);
  FileWr::Ln(O);

  FileWr::Str(O, "Enter a LONGINT: ");
  li = FileRd::LongInt(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::LongInt(O, li, 0);
  FileWr::Ln(O);

  FileWr::Str(O, "Enter a CARDINAL: ");
  c = FileRd::Card(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::Card(O, c, 0);
  FileWr::Ln(O);

  FileWr::Str(O, "Enter a LONGCARD: ");
  lc = FileRd::LongCard(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::LongCard(O, lc, 0);
  FileWr::Ln(O);

  FileWr::Str(O, "Enter a CHAR: ");
  ch = FileRd::Char(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: '");
  FileWr::Char(O, ch);
  FileWr::Str(O, "'");
  FileWr::Ln(O);

  FileWr::Str(O, "Enter a string: ");
  FileRd::Line(I, str);
  FileWr::Str(O, "You entered: '");
  FileWr::Str(O, str);
  FileWr::Str(O, "'");
  FileWr::Ln(O);

  FileWr::Str(O, "Enter an REAL: ");
  r = FileRd::Real(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::Real(O, r, 0);
  FileWr::Ln(O);

  FileWr::Str(O, "Enter a LONGREAL: ");
  lr = FileRd::LongReal(I);
  FileRd::Ln(I);
  FileWr::Str(O, "You entered: ");
  FileWr::LongReal(O, lr, 0);
  FileWr::Ln(O);

  return 0;
}
