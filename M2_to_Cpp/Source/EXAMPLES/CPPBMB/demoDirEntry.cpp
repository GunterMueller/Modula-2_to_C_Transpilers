/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <DirEntry.m2h>
#include <Wr.m2h>
#include <Rd.m2h>
#include <DirEntry.cpp>
#include <Wr.cpp>
#include <Rd.cpp>

DirEntry::T e;
BOOLEAN found = TRUE;
ARRAY<_SubrangeType<CARDINAL, 0, 260>, CHAR> nameStr, pathStr;

int main() {
  Wr::Str("Enter path (wildcards excepted): ");
  Rd::Line(pathStr);
  Wr::Ln();

  DirEntry::SetPath(pathStr);

  Wr::Str("Directury entries corresponding to the path:");
  Wr::Ln();

  while (found) {
    found = DirEntry::Found(e);
    if (found) {
      DirEntry::Name(nameStr, e);
      Wr::Ln();
      Wr::Str(nameStr);
    }
  }

  return 0;
}
