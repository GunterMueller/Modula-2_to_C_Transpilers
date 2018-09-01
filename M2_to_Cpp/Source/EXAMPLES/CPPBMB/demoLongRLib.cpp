/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <LongRLib.m2h>
#include <Rd.m2h>
#include <Wr.m2h>
#include <LongRLib.cpp>
#include <Rd.cpp>
#include <Wr.cpp>

LONGREAL r;

int main() {
  Wr::Str("Enter a LONGREAL (preferably between 0 and 1): ");
  r = Rd::LongReal();
  Wr::Ln();

  Wr::Str("LongRLib::Sin(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::Sin(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::Cos(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::Cos(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::SinH(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::SinH(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::CosH(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::CosH(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::Exp(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::Exp(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::Log(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::Log(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::Log10(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::Log10(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::ATan(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::ATan(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::ATanH(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::ATanH(r), 0);
  Wr::Ln();

  Wr::Str("LongRLib::Sqrt(");
  Wr::LongReal(r, 0);
  Wr::Str(") == ");
  Wr::LongReal(LongRLib::Sqrt(r), 0);
  Wr::Ln();

  return 0;
}
