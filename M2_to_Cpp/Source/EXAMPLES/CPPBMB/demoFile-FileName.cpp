/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <File.m2h>
#include <FileName.m2h>
#include <Rd.m2h>
#include <Wr.m2h>
#include <File.cpp>
#include <FileName.cpp>
#include <Rd.cpp>
#include <Wr.cpp>

File::T f;
ARRAY <_SubrangeType<CARDINAL, 0, 260>, CHAR> fname, str1, str2;
BOOLEAN fileExists;

int main() {
  do {
    Wr::Str("Enter name of file to create: ");
    Rd::Line(fname);
    fileExists = FileName::Exists(fname);
    if (fileExists) {
      Wr::Str("A file with that name already exists!");
      Wr::Ln();
    }
  } while (fileExists);

  f = File::Create(fname);

  Wr::Str("Enter a string to write to file '");
  Wr::Str(fname);
  Wr::Str("': ");

  Rd::Line(str1);
  Wr::Ln();
  File::WrStr(f, str1);
  File::Close(f);

  f = File::Open(fname);
  File::RdBin(f, str2, File::Size(f));
  str2[File::Size(f)] = '\0';
  Wr::Str("This string was written into the file: '");
  Wr::Str(str2);
  Wr::Str("'");
  Wr::Ln();
  File::Close(f);

  Wr::Str("Do you wish to delete file '");
  Wr::Str(fname);
  Wr::Str("' (y/n)? ");

  if (CAP(Rd::Char()) == 'Y') {
    FileName::Delete(fname);
    Wr::Str("File deleted.");
  }
  else {
    Wr::Str("File not deleted.");
  }

  return 0;
}
