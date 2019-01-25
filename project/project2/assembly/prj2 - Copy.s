!=================================================================
! General conventions:
!   1) Stack grows from high addresses to low addresses, and the
!      top of the stack points to valid data
!
!   2) Register usage is as implied by assembler names and manual
!
!   3) Function Calling Convention:
!
!       Setup)
!       * Immediately upon entering a function, push the RA on the stack.
!       * Next, push all the registers used by the function on the stack.
!
!       Teardown)
!       * Load the return value in $v0.
!       * Pop any saved registers from the stack back into the registers.
!       * Pop the RA back into $ra.
!       * Return by executing jalr $zero, $ra.
!=================================================================

        ! vector table
vector0:
0        .fill 0x00000000            ! device ID 0
1        .fill 0x00000000            ! device ID 1
2        .fill 0x00000000            ! ...
3        .fill 0x00000000
4        .fill 0x00000000
5        .fill 0x00000000
6        .fill 0x00000000
7        .fill 0x00000000
8        .fill 0x00000000
9        .fill 0x00000000
a        .fill 0x00000000
b        .fill 0x00000000
c        .fill 0x00000000
d        .fill 0x00000000
e        .fill 0x00000000
f        .fill 0x00000000            ! device ID 15
        ! end vector table

10main:   lea $sp, initsp                       ! initialize the stack pointer
11        lw $sp, 0($sp)                        ! finish initialization
        
                                              ! Install timer interrupt handlers into vector table
        !noop                                  ! FIX ME
12	lea $t0, timer_handler
13	lea $t1, vector0
14	sw $t0, 0($t1)
	
        
        !noop                                  ! Don't forget to enable interrupts
15	EI

16        lea $a0, BASE                         ! load base for pow
17        lw $a0, 0($a0)
18        lea $a1, EXP                          ! load power for pow
19        lw $a1, 0($a1)
1a        lea $at, POW                          ! load address of pow
1b        jalr $ra, $at                         ! run pow
1c        lea $a0, ANS                          ! load base for pow
1d        sw $v0, 0($a0)

1e        halt

1fBASE:   .fill 2
20EXP:    .fill 3
21ANS:      .fill 0                               ! should come out to 1024

22POW:    addi $sp, $sp, -1                     ! allocate space for old frame pointer
23        sw $fp, 0($sp)
24        addi $fp, $sp, 0                      ! set new frame pinter
25        add $t1, $zero, $zero                 ! $t1 = 0
26        addi $t2, $zero, 1                    ! $t2 = 1
27        slt $t1, $a1, $t2                     ! check if $a1 is 0 (if not, $t1 = 0)
28        bne $t1, $zero, RET1                  ! if $t1 == 1, branch to RET1
29        add $t1, $zero, $zero                 ! $t1 = 0
2a        addi $t2, $zero, 1                    ! $t2 = 1
2b        slt $t1, $a0, $t2                     ! if the base is 0, $t1 = 1
2c        bne $t1, $zero, RET0                  ! if $t1 == 1, branch to RET0
2d        addi $a1, $a1, -1                     ! decrement the power
2e        lea $at, POW                          ! load the address of POW
2f        addi $sp, $sp, -2                     ! push 2 slots onto the stack
30        sw $ra, -1($fp)                       ! save RA to stack
31        sw $a0, -2($fp)                       ! save arg 0 to stack
32        jalr $ra, $at                         ! recursively call POW
33        add $a1, $v0, $zero                   ! store return value in arg 1
34        lw $a0, -2($fp)                       ! load the base into arg 0
35        lea $at, MULT                         ! load the address of MULT
36        jalr $ra, $at                         ! multiply arg 0 (base) and arg 1 (running product)
37        lw $ra, -1($fp)                       ! load RA from the stack
38        addi $sp, $sp, 2
39        addi $t1, $zero, 1                    ! $t1 = 1
3a        bne $zero, $t1, FIN                   ! unconditional branch to FIN
3bRET1:   addi $v0, $zero, 1                    ! return a value of 1
3c        addi $t1, $zero, 1                    ! $t1 = 1
3d        bne $zero, $t1, FIN                   ! unconditional branch to FIN
3eRET0:   add $v0, $zero, $zero                 ! return a value of 0
3fFIN:      lw $fp, 0($fp)                        ! restore old frame pointer
40        addi $sp, $sp, 1                      ! pop off the stack
41        jalr $zero, $ra

42MULT:   add $v0, $zero, $zero                 ! zero out return value
43        addi $t0, $zero, 1
44AGAIN:  add $v0, $v0, $a0                     ! multiply loop
45        addi $t0, $t0, 1
46        add $t1, $zero, $zero                 ! $t1 = 0
47        addi $t2, $a1, 1                      ! $t2 = $a1 + 1
48        slt $t1, $t0, $t2                     ! $t1 = 1, if $t0 < $t2 (if not, $t1 = 0)
49        bne $t1, $zero, AGAIN                 ! if $t1 = 1, loop again
4a        jalr $zero, $ra


timer_handler:
4b	addi $sp, $sp, -1
4c	sw $k0, 0($sp)
4d	ei
4e	addi $sp, $sp, -2
4f	sw $t0, 1($sp)
50	sw $t1, 2($sp)

51	lea $t0, ticks1 	!$t0 has address of ticks1
52	lw $t0, 0($t0)		!$t0 has 0xFFFFFD
53	lw $t1, 0($t0)		!$t1 has the content store in 0xFFFFFD
54	addi $t1, $t1, 1	!increment the content which is stored in 0xFFFFFD

55	lw $t0, 1($sp)
56	lw $t1, 2($sp)
57	addi $sp, $sp, 2
58	di

59	lw $k0, 0($sp)
5a	addi $sp, $sp, 1
5b	reti

5cinitsp: .fill 0xA00000

5dticks1: .fill 0xFFFFFD

!==============================

