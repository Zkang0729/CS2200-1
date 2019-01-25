!============================================================
! CS 2200 Homework 2 Part 1: mod
!
! Edit any part of this file necessary to implement the
! mod operation. Store your result in $v0.
!============================================================

# int mod (int a, int b) {
# 	int x = a;
# 	while (x >= b) {
# 		x = x - b;
# 	}
# 	return x;
# }

mod:
    addi    $a0, $zero, 28      ! $a0 = 28, the number a to compute mod(a,b)
    addi    $a1, $zero, 13      ! $a1 = 13, the number b to compute mod(a,b)

    addi 	$t0, $a0, 	0		! x = a 
    slt		$t1, $t0, 	$a1		! if x >= b, $t1 = 0
    bne		$t1, $zero,	operation
    add		$v0, $t1,	$zero
    halt
    
operation:
	nand	$t2, $a1, 	$a1		! x = x - b
	addi	$t2, $t2, 	1
	add 	$t0, $t0, 	$t2
	slt		$t1, $t0, 	$t2
	bne		$t1, $zero, operation



# mod:
#     addi    $t0, $a0, 0         ! x = a
    
# loop:                           ! while x >= b
#     slt     $t1, $t0, $a1       ! if x >= b, $t1 = 0
#     bne     $t1, $zero, rec     ! if x < b, recurse

#     nand    $t2, $a1, $a1       ! x = x - b
#     addi    $t2, $t2, 1
#     add     $t0, $t0, $t2

#     addi    $t1, $zero, 1       ! $t1 = 1
#     bne     $t1, $zero, loop
