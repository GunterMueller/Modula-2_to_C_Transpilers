/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Addr.m2h>
#include <Wr.m2h>
#include <Addr.cpp>
#include <Wr.cpp>

INTEGER i[2];
POINTER<INTEGER> p;

int main() {
  i[0] = 41;
  i[1] = 42;
  p = ADR(i[0]);

  Wr::Str("i[0] == ");
  Wr::Int(i[0], 0);
  Wr::Ln();

  Wr::Str("i[1] == ");
  Wr::Int(i[1], 0);
  Wr::Ln();

  Wr::Str("p == ADR(i[0])");
  Wr::Ln();

  Wr::Str("*p == ");
  Wr::Int(*p, 0);
  Wr::Ln();

  Addr::Inc(p, TSIZE(INTEGER));

  Wr::Str("Addr::Inc(p, TSIZE(INTEGER));");
  Wr::Ln();

  Wr::Str("*p == ");
  Wr::Int(*p, 0);
  Wr::Ln();

  return 0;
}
