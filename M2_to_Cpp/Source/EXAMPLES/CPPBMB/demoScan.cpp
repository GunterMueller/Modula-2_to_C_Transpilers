/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Scan.m2h>
#include <Rd.m2h>
#include <Wr.m2h>
#include <Scan.cpp>
#include <Rd.cpp>
#include <Wr.cpp>

BOOLEAN b;
INTEGER i;
LONGINT li;
CARDINAL c;
LONGCARD lc;
REAL r;
LONGREAL lr;
ARRAY <_SubrangeType<CARDINAL, 1, 100>, CHAR> str;

int main() {
  Wr::Str("Enter a BOOLEAN: ");
  Rd::Line(str);
  b = Scan::Bool(str);
  Wr::Str("You entered: ");
  Wr::Bool(b, 0);
  Wr::Ln();

  Wr::Str("Enter an INTEGER: ");
  Rd::Line(str);
  i = Scan::Int(str);
  Wr::Str("You entered: ");
  Wr::Int(i, 0);
  Wr::Ln();

  Wr::Str("Enter a LONGINT: ");
  Rd::Line(str);
  li = Scan::LongInt(str);
  Wr::Str("You entered: ");
  Wr::LongInt(li, 0);
  Wr::Ln();

  Wr::Str("Enter a CARDINAL: ");
  Rd::Line(str);
  c = Scan::Card(str);
  Wr::Str("You entered: ");
  Wr::Card(c, 0);
  Wr::Ln();

  Wr::Str("Enter a LONGCARD: ");
  Rd::Line(str);
  lc = Scan::LongCard(str);
  Wr::Str("You entered: ");
  Wr::LongCard(lc, 0);
  Wr::Ln();

  Wr::Str("Enter an REAL: ");
  Rd::Line(str);
  r = Scan::Real(str);
  Wr::Str("You entered: ");
  Wr::Real(r, 0);
  Wr::Ln();

  Wr::Str("Enter a LONGREAL: ");
  Rd::Line(str);
  lr = Scan::LongReal(str);
  Wr::Str("You entered: ");
  Wr::LongReal(lr, 0);
  Wr::Ln();

  return 0;
}
