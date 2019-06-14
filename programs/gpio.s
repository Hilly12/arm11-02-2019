ldr r0, =0x20200004
mov r1, #0x40000
str, r1, [r0]
mov r2, #0x10000
loop:
str r2, [r0, #36]
ldr r1,=0xFFFFFF
first_delay:
sub r1, r1, #1
cmp r1, #0
bne first_delay
str r2, [r0, #24]
ldr r1,=0xFFFFFF
second_delay:
sub r1, r1, #1
cmp r1, #0
bne second_delay
b loop
