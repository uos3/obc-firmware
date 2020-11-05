target extended-remote :3333

# print demangled symbols
set print asm-demangle on

# set backtrace limit to not have infinite backtrace loops
set backtrace limit 32

# detect unhandled exceptions, hard faults and panics
break DefaultHandler
break HardFault
break rust_begin_unwind
# # run the next few lines so the panic message is printed immediately
# # the number needs to be adjusted for your panic handler
# commands $bpnum
# next 4
# end

# Do not break on entering main, instead just run. Uncommenting this line will
# add a breakpoint on the main function, allowing steps through.
# break main

monitor arm semihosting enable

# # enable ITM port 0
# monitor itm port 0 on

# Load the software onto the launchpad
load

# Begin execution
continue
