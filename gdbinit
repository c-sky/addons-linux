tar jtag jtag://127.0.0.1:1025
reset

# setup CCR (Cache Config Reg)
# 0-1:MP,2:IE,3:DE,4:WB,5:RS,6:Z,7:BE 
set $cr18 = 0x7d

# vendor custom setup
set $cr30 = 0xc

# pass devicetree blob
# r2 is magic
# r3 is address
set $r2 = 0x20150401
set $r3 = 0x91000000
restore /tmp/kernel_build/arch/csky/boot/dts/gx6605s.dtb binary 0x91000000

# flush cache
set $cr17 = 0x33

# pin mux for serial8250
set *(unsigned int *) 0xa030a14c |= (1 << 22) | (1 << 23)
load

