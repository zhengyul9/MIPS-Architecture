LUI $v0, 0x1001
ADDIU $a0, $zero, 0xA
SW $a0, 0($v0)

ADDIU $t0, $zero, 0
ADDIU $t1, $zero, 0x1
ADDIU $t2, $zero, 0
ADDU $a1, $zero, $a0
ADDIU $a2, $zero, 0
L1: ADDIU $t0, $t1, 0
ADDIU $t1, $t2, 0
ADDU $t2, $t0, $t1
ADDIU $a2, $a2, 0x1
BNE $a1, $a2, L1
ADDIU $v0, $zero, 0xA
syscall