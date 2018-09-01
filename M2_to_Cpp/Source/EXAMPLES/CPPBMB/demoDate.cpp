/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Date.m2h>
#include <Wr.m2h>
#include <Date.cpp>
#include <Wr.cpp>

Date::T d, d1, d2;
ARRAY<_SubrangeType<CARDINAL, 1, 100>, CHAR> str;

int main() {
  Date::GetSystemDate(d);

  Date::ToString(str, d);
  Wr::Str("Current date is ");
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("That is, the day is ");
  Wr::Card(Date::Day(d), 0);
  Wr::Str(", the month is ");
  Wr::Card(Date::Month(d), 0);
  Wr::Str(", and the year is ");
  Wr::Card(Date::Year(d), 0);
  Wr::Ln();

  Date::Inc(d, 1);

  Date::ToString(str, d);
  Wr::Str("Tomorrow is ");
  Wr::Str(str);
  Wr::Ln();

  d1 = d;

  Date::Dec(d, 2);

  Date::ToString(str, d);
  Wr::Str("Yesterday was ");
  Wr::Str(str);
  Wr::Ln();

  Date::Sub(d2, d1, d);

  Date::ToString(str, d2);
  Wr::Str("tomorrow - yesterday == ");
  Wr::Str(str);
  Wr::Ln();

  return 0;
}
