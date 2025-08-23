#include "dishex.h"

#include "llvm/Support/InitLLVM.h"

#include <string.h>
#include <fstream>
#include <iostream>
#include <algorithm>

// https://llvm.org/docs/CommandLine.html
static cl::OptionCategory DishexCat("dishex Options");

static cl::opt<Endian>
    EndianOpt("endian",
        cl::Required,
        cl::desc("Choose Endian"),
        cl::values(
            clEnumValN(Big, "big", "Big Endian"),
            clEnumValN(Little, "little", "Litte Endian")),
        cl::cat(DishexCat));

static cl::list<std::string> InputFilenames(cl::Positional,
                                           cl::desc("<input object files>"),
                                           cl::ZeroOrMore,
                                           cl::cat(DishexCat));

extern StringRef ToolName;

HexToAsm *DisAsm;

static void disassembleFile(std::string Filename) {
  std::ifstream Infile(Filename);
  DisAsm->run(Infile);
  Infile.close();
}

// Usage: See help as follows:
// ~/llvm/test/build/bin/dishex --help
int main (int argc, char **argv) {
  using namespace llvm;
  InitLLVM X(argc, argv);

  cl::HideUnrelatedOptions(DishexCat);
  cl::ParseCommandLineOptions(argc, argv, "disassemble hex input\n", nullptr,
                              /*EnvVar=*/nullptr,
                              /*LongOptionsUseDoubleDash=*/true);

  ToolName = argv[0];

  DisAsm = new HexToAsm(EndianOpt);
  if (InputFilenames.size() == 0) {
    // Input from the keyboard ("dishex") or
    // redirect from a file ("dishex < input.hex")
    DisAsm->run(std::cin);
  } else {
    // Input from files (e.g. "dishex 1.hex 2.hex")
    std::for_each(InputFilenames.begin(), InputFilenames.end(),
                  disassembleFile);
    //std::for_each(InputFilenames, disassembleFile);
  }

  return EXIT_SUCCESS;
}

