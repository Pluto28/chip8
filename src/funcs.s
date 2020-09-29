    .text
    .global flags
    .type flags, @function



flags:
        pushq   %rbp
        nop
        lahf
        sar     $8, %rax
        movq    %rax,%rbp
        popq    %rbp
        ret
