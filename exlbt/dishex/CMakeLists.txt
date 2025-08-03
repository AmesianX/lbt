# dishex.cpp needs backend related functions, like 
# LLVMInitializeCpu0TargetInfo and LLVMInitializeCpu0Disassembler ... etc.
# Set LLVM_LINK_COMPONENTS then it can link them during the link stage.
set(LLVM_LINK_COMPONENTS
  AllTargetsDescs
  AllTargetsDisassemblers
  AllTargetsInfos
  CodeGen
  MCDisassembler
  Symbolize
  )

add_llvm_tool(dishex
  main.cpp
  dishex.cpp
  disas.cpp
  )

if(HAVE_LIBXAR)
  target_link_libraries(dishex PRIVATE ${XAR_LIB})
endif()

if(LLVM_INSTALL_BINUTILS_SYMLINKS)
  add_llvm_tool_symlink(dishex dishex)
endif()

