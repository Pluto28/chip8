    .text
    .global flags
    .type flags, @function



flags:
        pushq   %rbp
        nop
        lahf
        sar     $8, %eax
        mov     %eax,%ebp
        popq    %rbp
        ret
