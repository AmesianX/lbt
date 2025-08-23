Disassemble hex input to Cpu0 assembly.
e.g. 
  input: 29 20 20 00
  output: sltu	$2, $zero, $2

hw-dis % pwd
~/git/lbt/exlbt/hw-dis
hw-dis % ~/llvm/test/build/bin/dishex --endian=big 1.hex 2.hex


input % pwd
~/git/lbd/lbdex/input

input % bash build-run_backend.sh cpu032II eb

input % ls -l ch_run_backend.cpu0.o
-rw-r--r--  1 chungshu  staff  20176 Aug  2 22:17 ch_run_backend.cpu0.o

input % ~/llvm/test/build/bin/llvm-objdump -d ch_run_backend.cpu0.o | ~/llvm/test/llvm/tools/dishex/filterhex | ~/llvm/test/build/bin/dishex --endian=big
