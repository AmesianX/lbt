#ifndef LLVM_TOOLS_DISAS_DISAS_H
#define LLVM_TOOLS_DISAS_DISAS_H

#include "llvm/DebugInfo/DIContext.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/Object/MachO.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Object/Archive.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

using namespace llvm::object;

class DisAs {
public:
  DisAs();
  bool disassemble(const ArrayRef<uint8_t> Bytes, llvm::raw_string_ostream &RSO);
  bool getInstruction(const ArrayRef<uint8_t> Bytes, MCInst &Inst);
  const Target *getTarget();
  void reportError(const Twine &Message);

private:
  void initLLVM();
  void create();
  const MCSubtargetInfo *createSTI(const Target *TheTarget);

  std::unique_ptr<MCDisassembler> DisAsm;
  std::unique_ptr<MCInstPrinter> IP;
  const MCSubtargetInfo *STI;

  const Target *TheTarget;
  const MCRegisterInfo *MRI;
  const MCAsmInfo *AsmInfo;
  MCContext *Ctx;
};
#endif // LIVM_TOOLS_DISAS_DISAS_H
