/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Rd.m2h>
#include <Wr.m2h>
#include <Rd.cpp>
#include <Wr.cpp>

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
  Wr::Str("Enter a BOOLEAN: ");
  b = Rd::Bool();
  Wr::Str("You entered: ");
  Wr::Bool(b, 0);
  Wr::Ln();

  Wr::Str("Enter an INTEGER: ");
  i = Rd::Int();
  Wr::Str("You entered: ");
  Wr::Int(i, 0);
  Wr::Ln();

  Wr::Str("Enter a LONGINT: ");
  li = Rd::LongInt();
  Wr::Str("You entered: ");
  Wr::LongInt(li, 0);
  Wr::Ln();

  Wr::Str("Enter a CARDINAL: ");
  c = Rd::Card();
  Wr::Str("You entered: ");
  Wr::Card(c, 0);
  Wr::Ln();

  Wr::Str("Enter a LONGCARD: ");
  lc = Rd::LongCard();
  Wr::Str("You entered: ");
  Wr::LongCard(lc, 0);
  Wr::Ln();

  Wr::Str("Enter a CHAR: ");
  ch = Rd::Char();
  Wr::Str("You entered: '");
  Wr::Char(ch);
  Wr::Str("'");
  Wr::Ln();

  Wr::Str("Enter a string: ");
  Rd::Line(str);
  Wr::Str("You entered: '");
  Wr::Str(str);
  Wr::Str("'");
  Wr::Ln();

  Wr::Str("Enter an REAL: ");
  r = Rd::Real();
  Wr::Str("You entered: ");
  Wr::Real(r, 0);
  Wr::Ln();

  Wr::Str("Enter a LONGREAL: ");
  lr = Rd::LongReal();
  Wr::Str("You entered: ");
  Wr::LongReal(lr, 0);
  Wr::Ln();

  return 0;
}
