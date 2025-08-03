#include "dishex.h"

#include <algorithm>
#include <fstream>
#include <iostream>


HexToAsm::HexToAsm() {}

// Outpu:
//   TheTarget
// Input:
//   TheTriple
const Target *HexToAsm::getTarget() {
  // Get the target specific parser.
  const Target *TheTarget = this->Disas.getTarget();

  return TheTarget;
}

void HexToAsm::reportError(const Twine &Message) {
  Disas.reportError(Message);
}

// Hex -> Vectory<unsigned char>
//   Convert Hex to an array of unsigned char
// Output:
//   VByte
// Input:
//   Hex
void HexToAsm::hexToBytes(const std::string &Hex, std::vector<uint8_t> &VByte) {
  int Size = 8;
  for (int i = 0; i < Size; i+=2) {
    // Convert byte[i..i+1] to uint8_t and save to Byte[i/2]
    uint8_t Byte = 0x0;
    if (Hex[i] >= '0' && Hex[i] <= '9') {
      Byte = Hex[i] - '0';
    } else if (Hex[i] >= 'A' && Hex[i] <= 'F') {
      Byte = Hex[i] - 'A' + 10;
    } else if (Hex[i] >= 'a' && Hex[i] <= 'f') {
      Byte = Hex[i] - 'a' + 10;
    } else {
      reportError("error: VByte must in [0..9, A..F, a..f], Hex: " + 
         StringRef(Hex));
    }

    int j = i+1;
    if (Hex[j] >= '0' && Hex[j] <= '9') {
      Byte = (Byte << 4) + (Hex[j] - '0') ;
    } else if (Hex[j]>= 'A' && Hex[j] <= 'F') {
      Byte = (Byte << 4) + (Hex[j] - 'A' + 10);
    } else if (Hex[j] >= 'a' && Hex[j] <= 'f') {
      Byte = (Byte << 4) + (Hex[j] - 'a' + 10);
    } else {
      reportError("error: VByte must in [0..9, A..F, a..f], Hex: " + 
         StringRef(Hex));
    }

    if (i % 2 == 0) {
      VByte[i/2] = Byte;
    }
  }
}

// Return true if `Line` consists of hexadecimal numbers seperated by spaces.
//   true:  if all c in `Line` are [0..0,A..F,a..f,' ']
//   false: others
bool HexToAsm::checkInput(std::string &Line) {
  size_t Size = 8;
  size_t Length = 0;
  for (char c : Line) {
    if (c >= '0' && c <= '9') {
      Length++;
      continue;
    } else if (c >= 'A' && c <= 'F') {
      Length++;
      continue;
    } else if (c >= 'a' && c <= 'f') {
      Length++;
      continue;
    } else if (c == ' ') {
      continue;
    } else {
      reportError("error: input must in [0..9, A..F, a..f, ' '], Hex: " +
                  StringRef(Line) + "\n");
      return false;
    }
  }
  char Msg[128];
  snprintf(Msg, 127, "error: The length of input must be %d and seperated with "
               "blank, input: ", (int)Size);
  if (Length != Size) {
    reportError(Msg + StringRef(Line) + "\n");
    outs() << "Example: 09 20 00 00\n";
    return false;
  }

  return true;
}

// Hex -> MCInst
//   Convert a hex string to MCInst.
// Output:
//   RSO
// Input:
//   Line   
bool HexToAsm::disassemble(std::string &Line, llvm::raw_string_ostream &RSO) {
  Line.erase(std::remove(Line.begin(), Line.end(), ' '), Line.end());
  int Length = 4;
  std::vector<uint8_t> VByte(Length, 0x0); // Eight uint8_t with value 0x0
  hexToBytes(Line, VByte);
  ArrayRef<uint8_t> Bytes(VByte);
  bool Disassembled = Disas.disassemble(Bytes, RSO);

  return Disassembled;
}

// Read the instruction in hexidecimal format from an input stream (keyboard or 
// file) and output it to the screen.
// Output:
//   Asm
// Input:
//   Line
void HexToAsm::run(std::istream &Is) {
  std::string Line;
  std::string Asm;
  llvm::raw_string_ostream RSO(Asm);
  
  outs() << "\ninput: ";
  std::getline(Is, Line);
  outs() << Line << "\n";
  while (Line != "quit" && !Is.eof()) {
    if (Line[0] == '#') {
      outs() << "\ninput: ";
      std::getline(Is, Line);
      outs() << Line << "\n";
      continue;
    }
    bool Right = checkInput(Line);
    if (!Right) {
      outs() << "\ninput: ";
      std::getline(Is, Line);
      outs() << Line << "\n";
      continue;
    }
    bool Disassembled = disassemble(Line, RSO);
    if (Disassembled) {
      outs() << Asm << "\n";
    } else {
      outs() << Asm << ": Disassemble Error!\n";
    }
    Asm.clear();
    outs() << "\ninput: ";
    std::getline(Is, Line);
    outs() << Line << "\n";
  }
}
