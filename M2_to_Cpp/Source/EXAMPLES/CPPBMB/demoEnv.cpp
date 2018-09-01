/* By Milos Radovanovic, 2000, 2001 */

#include <_m2sys.h>

#include <Env.m2h>
#include <Wr.m2h>
#include <Env.cpp>
#include <Wr.cpp>

ARRAY<_SubrangeType<INTEGER, 0, 255>, CHAR> progName, progPath, param;
ARRAY<_SubrangeType<INTEGER, 0, 1023>, CHAR> var;

int main(int argc, char* argv[]) {
  Env::_init(argc, argv);
  Env::_ModuleBody Env_body;

  Env::Var(var, "PATH");
  Env::GetProgramName(progName);
  Env::GetProgramPath(progPath);

  Wr::Str("PATH=");
  Wr::Str(var);
  Wr::Ln();
  Wr::Str("Program name: ");
  Wr::Str(progName);
  Wr::Ln();
  Wr::Str("Program path: ");
  Wr::Str(progPath);
  Wr::Ln();

  Wr::Str("Program parameters:");
  Wr::Ln();
  for (CARDINAL i = 1; i <= Env::Params(); INC(i)) {
    Env::GetParam(param, i);
    Wr::Str(param);
    Wr::Ln();
  }

  return 0;
}
