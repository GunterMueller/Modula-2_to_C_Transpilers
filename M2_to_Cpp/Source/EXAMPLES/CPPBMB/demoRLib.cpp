/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <RLib.m2h>
#include <Rd.m2h>
#include <Wr.m2h>
#include <RLib.cpp>
#include <Rd.cpp>
#include <Wr.cpp>

REAL r;

int main() {
  Wr::Str("Enter a REAL (preferably between 0 and 1): ");
  r = Rd::Real();
  Wr::Ln();

  Wr::Str("RLib::Sin(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::Sin(r), 0);
  Wr::Ln();

  Wr::Str("RLib::Cos(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::Cos(r), 0);
  Wr::Ln();

  Wr::Str("RLib::SinH(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::SinH(r), 0);
  Wr::Ln();

  Wr::Str("RLib::CosH(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::CosH(r), 0);
  Wr::Ln();

  Wr::Str("RLib::Exp(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::Exp(r), 0);
  Wr::Ln();

  Wr::Str("RLib::Log(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::Log(r), 0);
  Wr::Ln();

  Wr::Str("RLib::Log10(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::Log10(r), 0);
  Wr::Ln();

  Wr::Str("RLib::ATan(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::ATan(r), 0);
  Wr::Ln();

  Wr::Str("RLib::ATanH(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::ATanH(r), 0);
  Wr::Ln();

  Wr::Str("RLib::Sqrt(");
  Wr::Real(r, 0);
  Wr::Str(") == ");
  Wr::Real(RLib::Sqrt(r), 0);
  Wr::Ln();

  return 0;
}
