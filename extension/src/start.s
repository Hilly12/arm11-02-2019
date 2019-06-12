.section .init

.global _start

_start:
    // mov     sp, #0x8000 need lower stack ptr for more mallocs
    ldr sp, =(64 * 1024 * 1024)
    b       _cstartup

_inf_loop:
    b       _inf_loop
