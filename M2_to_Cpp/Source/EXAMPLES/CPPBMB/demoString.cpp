/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <String.m2h>
#include <Rd.m2h>
#include <Wr.m2h>
#include <String.cpp>
#include <Rd.cpp>
#include <Wr.cpp>

ARRAY<_SubrangeType<CARDINAL, 0, 100>, CHAR> s1, s2, s3;
CARDINAL pos;

int main() {
  Wr::Str("Enter string s1: ");
  Rd::Line(s1);
  Wr::Str("Enter string s2: ");
  Rd::Line(s2);

  if (String::SubString(s1, s2, pos)) {
    Wr::Str("s2 is a substring of s1 from position ");
    Wr::Card(pos, 0);
  }
  else {
    Wr::Str("s2 is not a substring of s1");
  }
  Wr::Ln();

  if (String::Equal(s1, s2)) {
    Wr::Str("s1 and s2 are equal");
  }
  else {
    Wr::Str("s1 and s2 are not equal");
  }
  Wr::Ln();

  if (String::LEq(s1, s2)) {
    Wr::Str("s1 is less than or equal to s2");
  }
  else {
    Wr::Str("s1 is not less than or equal to s2");
  }
  Wr::Ln();

  if (String::GEq(s1, s2)) {
    Wr::Str("s1 is greater than or equal to s2");
  }
  else {
    Wr::Str("s1 is not greater than or equal to s2");
  }
  Wr::Ln();

  Wr::Str("Length of s1 is ");
  Wr::Card(String::Length(s1), 0);
  Wr::Ln();
  Wr::Str("Length of s2 is ");
  Wr::Card(String::Length(s2), 0);
  Wr::Ln();

  Wr::Str("String::Caps(s1);");
  Wr::Ln();
  String::Caps(s1);
  Wr::Str("s1 == "); Wr::Str(s1);
  Wr::Ln();

  Wr::Str("String::Append(s1, s2);");
  Wr::Ln();
  String::Append(s1, s2);
  Wr::Str("s1 == "); Wr::Str(s1);
  Wr::Ln();

  Wr::Str("String::Copy(s3, s2);");
  Wr::Ln();
  String::Copy(s3, s2);
  Wr::Str("s3 == "); Wr::Str(s3);
  Wr::Ln();

  Wr::Str("String::Concat(s3, s1, s2);");
  Wr::Ln();
  String::Concat(s3, s1, s2);
  Wr::Str("s3 == "); Wr::Str(s3);
  Wr::Ln();

  Wr::Str("String::Insert(s3, s2, String::Length(s1) / 2);");
  Wr::Ln();
  String::Insert(s3, s2, String::Length(s1) / 2);
  Wr::Str("s3 == "); Wr::Str(s3);
  Wr::Ln();

  Wr::Str("String::Delete(s3, String::Length(s1) / 2, String::Length(s1));");
  Wr::Ln();
  String::Delete(s3, String::Length(s1) / 2, String::Length(s1));
  Wr::Str("s3 == "); Wr::Str(s3);
  Wr::Ln();

  Wr::Str("String::Slice(s3, s2, String::Length(s2) / 4, String::Length(s2) / 2);");
  Wr::Ln();
  String::Slice(s3, s2, String::Length(s2) / 4, String::Length(s2) / 2);
  Wr::Str("s3 == "); Wr::Str(s3);
  Wr::Ln();

  return 0;
}
