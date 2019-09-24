LUI $v0, 0x1001
ADDIU $a0, $zero, 0x5
SW $a0, 0($v0)
ADDIU $a0, $zero, 0x3
SW $a0, 4($v0)
ADDIU $a0, $zero, 0x6
SW $a0, 8($v0)
ADDIU $a0, $zero, 0x8
SW $a0, 0xC($v0)
ADDIU $a0, $zero, 0x9
SW $a0, 0x10($v0)
ADDIU, $a0, $zero, 0x1
SW $a0, 0x14($v0)
ADDIU $a0, $zero, 0x4
SW $a0, 0x18($v0)
ADDIU $a0, $zero, 0x7
SW $a0, 0x1C($v0)
ADDIU, $a0, $zero, 0x2
SW $a0, 0x20($v0)
ADDIU $a0, $zero, 0xA
SW $a0, 0x24($v0)

LUI $t0, 0x1001
LUI $t3, 0x1001
ADDIU $a1, $zero, 0xA
ADDIU $a2, $zero, 0
SUB $s0, $a1, 0x1
ADDIU $s1, $zero, 0
L2: ADDIU $t3, $t3, 0x4
LW $t1, 0($t0)
LW $t2, 0($t3)
SUB $s2, $t1, $t2
BLTZ $s2, L3
SW $t2, 0($t0)
SW $t1, 0($t3)
L3: ADDIU $s1, $s1, 0x1
BNE $s0, $s1, L2
ADDIU $t0, $t0, 0x4
ADDIU $a2, $a2, 0x1
ADDIU $s1, $zero, 0
ADD $t3, $zero, $t0
SUB $s0, $s0, 0x1
BNE $s0, $zero, L2
ADDIU $v0, $zero, 0xA
syscall
