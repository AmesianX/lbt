#include "disas.h"

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Object/MachO.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"

#include <algorithm>

StringRef ToolName;
static std::string TripleName = "";
static std::string MCPU = "";

void DisAs::reportError(const Twine &Message) {
  outs().flush();
  WithColor::error(errs(), ToolName) << ": " << Message << "\n";
  exit(1);
}

// Initialize all targets, components and disassemblers.
void DisAs::initLLVM() {
  // Initialize targets and assembly printers/parsers.
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllDisassemblers();

  // Register the target printer for --version.
  cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);
}

// Outpu:
//   TheTarget
//   TripleName
// Input:
//   TheTriple
const Target *DisAs::getTarget() {
  // Figure out the target triple.
  static Triple TheTriple("cpu0--");
  // Get the target specific parser.
  std:: string Error;
  const Target *TheTarget = TargetRegistry::lookupTarget("", TheTriple, Error);
  if (!TheTarget)
    reportError("Can't find target: " + Error);

  // Update the triple name and return the found target.
  TripleName = TheTriple.getTriple();

  return TheTarget;
}

// Return/Output:
//   STI
// Input:
//   TheTarget
const MCSubtargetInfo *DisAs::createSTI(const Target *TheTarget) {
  // Package up features to be passed to target/subtarget
  SubtargetFeatures Features;
  const MCSubtargetInfo *STI =
      TheTarget->createMCSubtargetInfo(TripleName, MCPU, Features.getString());
  if (!STI)
    reportError("no subtarget info for target " + TripleName);

  return STI;
}

// Create MCDisassemble and MCInstPrinter for the "disassembler task".
// Output:
//   DisAsm, IP(Instruction Printer)
// Important! Remember that certain objects, such as Ctx, must exist outside
// the scope of the `create()` function. Do not declare them as local variables
// with `create()`, or they will be destroyed after the function exists.
void DisAs::create() {
  TheTarget = getTarget();
  MRI = TheTarget->createMCRegInfo(TripleName);
  if (!MRI)
    reportError("no register info for target " + TripleName);

  // Set up disassembler.
  MCTargetOptions MCOptions;
  AsmInfo = TheTarget->createMCAsmInfo(*MRI, TripleName, MCOptions);
  if (!AsmInfo)
    reportError("no assembly info for target " + TripleName);

  // Create member variable STI
  STI = createSTI(TheTarget);

  MCObjectFileInfo MOFI;
  Ctx = new MCContext(AsmInfo, MRI, &MOFI);
  // FIXME: for now initialize MCObjectFileInfo with default value
  MOFI.InitMCObjectFileInfo(Triple(TripleName), false, *Ctx);

  // Create member variable DisAsm
  std:: unique_ptr<MCDisassembler> DisAsm(
     TheTarget->createMCDisassembler(*STI, *Ctx));
  if (!DisAsm)
    reportError("error: no disassembler for target " + TripleName);
  this->DisAsm = std::move(DisAsm);

  int AsmPrinterVariant = AsmInfo->getAssemblerDialect();

  const MCInstrInfo *MII = TheTarget->createMCInstrInfo();
  if (!MII)
    reportError("error: no instruction info for target " + TripleName);

  // Create member variable IP
  std::unique_ptr<MCInstPrinter> IP(TheTarget->createMCInstPrinter(
     Triple(TripleName), AsmPrinterVariant, *AsmInfo, *MII, *MRI));
  if (!IP)
    reportError("error: no instruction printer for target " + TripleName);
  this->IP = std::move(IP);
}

// Hex -> MCInst
//   Convert Hex string to MCInst
// Output:
//   Inst
// Input:
//   Hex
bool DisAs::getInstruction(const ArrayRef<uint8_t> Bytes, MCInst &Inst) {
#if 0
  outs() << "Bytes size: " << Bytes.size() << "\n";
  dumpBytes(Bytes, outs());
  outs() << "\n" ;
#endif
  uint64_t Size = 0;
  SmallString<40> Comments;
  raw_svector_ostream CommentStream(Comments);
  bool Disassembled = DisAsm->getInstruction(Inst, Size, Bytes, 0, CommentStream);
  if (!Disassembled) {
    reportError("error: DisAsm->getInstruction");
  } 

  return Disassembled;
}


// Bin -> MCInst
//   Convert a Binary Instruction to MCInst.
// Output:
//   RSO
// Input:
//   Bytes   
bool DisAs::disassemble(const ArrayRef<uint8_t> Bytes, llvm::raw_string_ostream &RSO) {
  MCInst Inst;
  bool Disassembled = getInstruction(Bytes, Inst);
  if (Disassembled) {
    IP->printInst(&Inst, 0, "", *STI, RSO);
  }

  return Disassembled;
}

// Initialize LLVM targets and components, and create both MCDisassembler
// (DisAsm) and MCInstPrinter(IP) for the "disassembler task".
DisAs::DisAs() {
  initLLVM();
  create();
}
