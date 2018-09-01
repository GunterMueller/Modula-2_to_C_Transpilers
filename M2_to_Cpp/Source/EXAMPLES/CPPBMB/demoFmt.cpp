/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Fmt.m2h>
#include <Rd.m2h>
#include <Wr.m2h>
#include <Fmt.cpp>
#include <Rd.cpp>
#include <Wr.cpp>

BOOLEAN b;
INTEGER i;
LONGINT li;
CARDINAL c;
LONGCARD lc;
REAL r;
LONGREAL lr;
ARRAY <_SubrangeType<CARDINAL, 1, 100>, CHAR> str, strp;

int main() {
  Wr::Str("Enter a BOOLEAN: ");
  b = Rd::Bool();
  Wr::Str("You entered: ");
  Fmt::Bool(str, b);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter an INTEGER: ");
  i = Rd::Int();
  Wr::Str("You entered: ");
  Fmt::Int(str, i);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter a LONGINT: ");
  li = Rd::LongInt();
  Wr::Str("You entered: ");
  Fmt::LongInt(str, li);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter a CARDINAL: ");
  c = Rd::Card();
  Wr::Str("You entered: ");
  Fmt::Card(str, c);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter a LONGCARD: ");
  lc = Rd::LongCard();
  Wr::Str("You entered: ");
  Fmt::LongCard(str, lc);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter an REAL: ");
  r = Rd::Real();
  Wr::Str("You entered: ");
  Fmt::Real(str, r);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter a LONGREAL: ");
  lr = Rd::LongReal();
  Wr::Str("You entered: ");
  Fmt::LongReal(str, lr);
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Enter a string: ");
  Rd::Line(str);
  Wr::Str("You entered: '");
  Wr::Str(str);
  Wr::Str("'");
  Wr::Ln();

  Wr::Str("Left adjusted, padded with '*' : '");
  Fmt::PadL(strp, str, 40, '*');
  Wr::Str(strp);
  Wr::Str("'");
  Wr::Ln();

  Wr::Str("Right adjusted, padded with '*': '");
  Fmt::PadR(strp, str, 40, '*');
  Wr::Str(strp);
  Wr::Str("'");
  Wr::Ln();

  Wr::Str("Centered, padded with '*'      : '");
  Fmt::PadC(strp, str, 40, '*');
  Wr::Str(strp);
  Wr::Str("'");
  Wr::Ln();

  return 0;
}
