#ifndef LLVM_TOOLS_DIS_HEX_DIS_HEX_H
#define LLVM_TOOLS_DIS_HEX_DIS_HEX_H

#include "disas.h"

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

using namespace llvm;

using namespace llvm::object;

class HexToAsm {
public:
  HexToAsm();
  void run(std::istream &Is);
  void reportError(const Twine &Message);

private:
  const Target *getTarget();
  bool checkInput(std::string &Line);
  void hexToBytes(const std::string &Hex, std::vector<uint8_t> &Byte);
  bool disassemble(std::string &Line, llvm::raw_string_ostream &RSO);

  DisAs Disas;
};
#endif // LIVM_TOOLS_DIS_HEX_DIS_HEX_H
