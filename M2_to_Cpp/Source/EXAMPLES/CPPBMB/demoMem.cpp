/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Mem.m2h>
#include <Wr.m2h>
#include <Mem.cpp>
#include <Wr.cpp>

CHAR c1[100000];
CHAR c2[100000];

ADDRESS a1;
ADDRESS a2;

int main() {
  a1 = ADR(c1[0]);
  a2 = ADR(c2[0]);

  Mem::Fill(a1, 100000*TSIZE(CHAR), 'M');
  Mem::Fill(a2, 100000*TSIZE(CHAR), 'M');

  Wr::Str("a1 == 'M' x 100000");
  Wr::Ln();
  Wr::Str("a2 == 'M' x 100000");
  Wr::Ln();

  Wr::Str("a1[6] = 'R';");
  Wr::Ln();
  c1[6] = 'R';

  Wr::Str("Mem::Equal(a1, a2, 100000*TSIZE(CHAR)) == ");
  Wr::Bool(Mem::Equal(a1, a2, 100000*TSIZE(CHAR)), 0);
  Wr::Ln();

  Wr::Str("Mem::LEq(a1, a2, 100000*TSIZE(CHAR)) == ");
  Wr::Bool(Mem::LEq(a1, a2, 100000*TSIZE(CHAR)), 0);
  Wr::Ln();

  Wr::Str("Mem::GEq(a1, a2, 100000*TSIZE(CHAR)) == ");
  Wr::Bool(Mem::GEq(a1, a2, 100000*TSIZE(CHAR)), 0);
  Wr::Ln();

  Wr::Str("Mem::Copy(a1, a2, 100000*TSIZE(CHAR));");
  Wr::Ln();
  Mem::Copy(a1, a2, 100000*TSIZE(CHAR));

  Wr::Str("Mem::Equal(a1, a2, 100000*TSIZE(CHAR)) == ");
  Wr::Bool(Mem::Equal(a1, a2, 100000*TSIZE(CHAR)), 0);
  Wr::Ln();

  return 0;
}
