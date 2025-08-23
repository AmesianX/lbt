#ifndef LLVM_TOOLS_DIS_HEX_DIS_HEX_H
#define LLVM_TOOLS_DIS_HEX_DIS_HEX_H

#include "disas.h"

#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

using namespace llvm;

using namespace llvm::object;

enum Endian {
  Big,
  Little
};

class HexToAsm {
public:
  HexToAsm(int Cpu0Endian);
  void run(std::istream &Is) const;
  void reportError(const Twine &Message) const;

private:
  const Target *getTarget(Triple &TheTriple) const;
  bool checkInput(std::string &Line) const;
  void hexToBytes(const std::string &Hex, std::vector<uint8_t> &Byte) const;
  bool disassemble(std::string &Line, llvm::raw_string_ostream &RSO) const;

  const DisAs *Disas;
};
#endif // LIVM_TOOLS_DIS_HEX_DIS_HEX_H
