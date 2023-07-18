#include <p32xxxx.h>
.equ IOPORT_BIT_0, 1 << 0 /* Define constant symbols */
.equ IOPORT_BIT_1, 1 << 1
.equ IOPORT_BIT_2, 1 << 2
.global main /* define all global symbols here */
/***************************************************************************
* main() function. This is where the PIC32 start-up code will jump to after 
* initial set-up.
****************************************************************************/
.text /* program section of the memory */
.ent main /* Assembler directive that marks symbol 'main' as a function */
    
main:
ori $a0, $zero, IOPORT_BIT_1
la $s0, TRISDCLR /* get address of a memory-mapped I/O register */
sw $a0, 0($s0) /* write value into the I/O register */
addiu $s0, $zero, 100 /* create a counter */
toggle_bits:
jal mPORTDSetBits /* function call to turn on an LED (port) */
li $a1, 0x10000 /* prepare parameter for another function call */
jal delay /* function call to insert some delay */
nop
jal mPORTDClearBits /* function call to turn off the LED */
li $a1, 0x30000
jal delay
nop
addiu $s0, $s0, -1
bnez $s0, toggle_bits
endless:
j endless /* trap the program here */
nop
.end main /* Assembler directive that marks end of a function */
.ent mPORTDSetBits
mPORTDSetBits:
addiu $sp, $sp, -4
sw $s0, 0($sp)
la $s0, LATDSET /* set memory-mapped I/O register */
sw $a0, 0($s0)
lw $s0, 0($sp)
addiu $sp, $sp, 4
jr $ra 
nop
.end mPORTDSetBits
.ent mPORTDClearBits
mPORTDClearBits:
addiu $sp, $sp, -4
sw $s0, 0($sp)
la $s0, LATDCLR /* clear memory-mapped I/O register */
sw $a0, 0($s0)
lw $s0, 0($sp)
addiu $sp, $sp, 4
jr $ra 
nop
.end mPORTDClearBits
.ent delay
delay: addiu $a1, $a1, -1
bnez $a1, delay
jr $ra /*return to caller*/
nop
.end delay

