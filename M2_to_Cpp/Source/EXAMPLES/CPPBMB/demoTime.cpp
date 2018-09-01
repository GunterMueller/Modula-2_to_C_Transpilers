/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Time.m2h>
#include <Wr.m2h>
#include <Time.cpp>
#include <Wr.cpp>

Time::T t, t1, t2;
ARRAY<_SubrangeType<CARDINAL, 1, 100>, CHAR> str;

int main() {
  Time::GetSystemTime(t);

  Time::ToString(str, t);
  Wr::Str("Current time is ");
  Wr::Str(str);
  Wr::Ln();

  Wr::Str("That is, the hour is ");
  Wr::Card(Time::Hour(t), 0);
  Wr::Str(", the minute is ");
  Wr::Card(Time::Min(t), 0);
  Wr::Str(", the second is ");
  Wr::Card(Time::Sec(t), 0);
  Wr::Str(", the hundred is ");
  Wr::Card(Time::Hund(t), 0);
  Wr::Ln();

  Time::Inc(t, 6000);

  Time::ToString(str, t);
  Wr::Str("In a minute it will be ");
  Wr::Str(str);
  Wr::Ln();

  Time::Copy(t1, t);

  Time::Dec(t, 12000);

  Time::ToString(str, t);
  Wr::Str("The minute before it was ");
  Wr::Str(str);
  Wr::Ln();

  Time::Sub(t2, t1, t);

  Time::ToString(str, t2);
  Wr::Str("minute after - minute before == ");
  Wr::Str(str);
  Wr::Ln();

  return 0;
}
