#include "dishex.h"

#include "llvm/Support/InitLLVM.h"

#include <fstream>
#include <iostream>

extern StringRef ToolName;

static void reportCommandUsage() {
  outs() << "Usage:\n"
         << "  dishex, or"
         << "  dishex hex-input-file\n";
  outs().flush();
}

// Usage:
//   "dishex" or
//   "dishex < input.hex" or
//   "dishex input.hex
int main (int argc, char **argv) {
  using namespace llvm;
  InitLLVM X(argc, argv);

  ToolName = argv[0];
  std::string InputFileName;

  if (argc > 2) {
    reportCommandUsage();
  } else if (argc == 2) {
    InputFileName = argv[1];
  } else {
    InputFileName = "";
  }

  HexToAsm DisAsm;
  if (InputFileName == "") {
    // Input from the keyboard ("dishex") or
    // redirect from a file ("dishex < input.hex")
    DisAsm.run(std::cin);
  } else {
    // Input from a file ("dishex input.hex")
    std::ifstream Infile(InputFileName);
    DisAsm.run(Infile);
    Infile.close();
  }

  return EXIT_SUCCESS;
}

