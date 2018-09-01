/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <ShortMem.m2h>
#include <Wr.m2h>
#include <ShortMem.cpp>
#include <Wr.cpp>

CHAR c1[1000];
CHAR c2[1000];

ADDRESS a1;
ADDRESS a2;

int main() {
  a1 = ADR(c1[0]);
  a2 = ADR(c2[0]);

  ShortMem::Fill(a1, 1000*TSIZE(CHAR), 'M');
  ShortMem::Fill(a2, 1000*TSIZE(CHAR), 'M');

  Wr::Str("a1 == 'M' x 1000");
  Wr::Ln();
  Wr::Str("a2 == 'M' x 1000");
  Wr::Ln();

  Wr::Str("a1[6] = 'R';");
  Wr::Ln();
  c1[6] = 'R';

  Wr::Str("ShortMem::Equal(a1, a2, 1000*TSIZE(CHAR)) == ");
  Wr::Bool(ShortMem::Equal(a1, a2, 1000*TSIZE(CHAR)), 0);
  Wr::Ln();

  Wr::Str("ShortMem::LEq(a1, a2, 1000*TSIZE(CHAR)) == ");
  Wr::Bool(ShortMem::LEq(a1, a2, 1000*TSIZE(CHAR)), 0);
  Wr::Ln();

  Wr::Str("ShortMem::GEq(a1, a2, 1000*TSIZE(CHAR)) == ");
  Wr::Bool(ShortMem::GEq(a1, a2, 1000*TSIZE(CHAR)), 0);
  Wr::Ln();

  Wr::Str("ShortMem::Copy(a1, a2, 1000*TSIZE(CHAR));");
  Wr::Ln();
  ShortMem::Copy(a1, a2, 1000*TSIZE(CHAR));

  Wr::Str("ShortMem::Equal(a1, a2, 1000*TSIZE(CHAR)) == ");
  Wr::Bool(ShortMem::Equal(a1, a2, 1000*TSIZE(CHAR)), 0);
  Wr::Ln();

  return 0;
}
