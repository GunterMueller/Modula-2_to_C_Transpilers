/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Dir.m2h>
#include <Wr.m2h>
#include <Rd.m2h>
#include <Dir.cpp>
#include <Wr.cpp>
#include <Rd.cpp>

int main() {
  ARRAY<_SubrangeType<CARDINAL, 0, 260>, CHAR> str;

  Dir::GetCurrent(str);
  Wr::Str("Current directory: ");
  Wr::Str(str);
  Wr::Ln();

  Dir::GetParent(str);
  Wr::Str("Parent directory: ");
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Change to: ");
  Rd::Line(str);
  Dir::ChangeTo(str);
  Wr::Ln();

  Dir::GetCurrent(str);
  Wr::Str("Current directory: ");
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("Make directory: ");
  Rd::Line(str);
  Dir::Make(str);
  Wr::Ln();

  Wr::Str("Remove directory: ");
  Rd::Line(str);
  Dir::Remove(str);
  Wr::Ln();

  return 0;
}
