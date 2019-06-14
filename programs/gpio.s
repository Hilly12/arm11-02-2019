; Accessing a GPIO pin between 10 and 19
ldr r0, =0x20200004
mov r1, #0x40000
str, r1, [r0] ; Makes GPIO pin 16 an output pin
mov r2, #0x10000
loop:
str r2, [r0, #36] ; Turns GPIO pin 16 off
ldr r2,=0xFFFFFF
; Doing many substraction operation to cause a delay between turning the GPIO pin on and off
first_delay:
sub r1, r1, #1
cmp r1, #0
bne first_delay
str r2, [r0, #24] ; Turns GPIO pin 16 on
ldr r2,=0xFFFFFF
; Doing many substraction operation to cause a delay between turning the GPIO pin on and off
second_delay:
sub r1, r1, #1
cmp r1, #0
bne second_delay
b loop
