/* ===-- assembly.h - libUnwind assembler support macros -------------------===
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 * ===----------------------------------------------------------------------===
 *
 * This file defines macros for use in libUnwind assembler source.
 * This file is not part of the interface of this library.
 *
 * ===----------------------------------------------------------------------===
 */

#ifndef UNWIND_ASSEMBLY_H
#define UNWIND_ASSEMBLY_H

#if defined(__powerpc64__)
#define SEPARATOR ;
#define PPC64_OFFS_SRR0   0
#define PPC64_OFFS_CR     272
#define PPC64_OFFS_XER    280
#define PPC64_OFFS_LR     288
#define PPC64_OFFS_CTR    296
#define PPC64_OFFS_VRSAVE 304
#define PPC64_OFFS_FP     312
#define PPC64_OFFS_V      824
#ifdef _ARCH_PWR8
#define PPC64_HAS_VMX
#endif
#elif defined(__POWERPC__) || defined(__powerpc__) || defined(__ppc__)
#define SEPARATOR @
#elif defined(__arm64__)
#define SEPARATOR %%
#else
#define SEPARATOR ;
#endif

#define STR2(s) #s
#define STR(s) STR2(s)

#define GLUE2(a, b) a ## b
#define GLUE(a, b) GLUE2(a, b)
#define SYMBOL_NAME(name) GLUE(__USER_LABEL_PREFIX__, name)

#if defined(__APPLE__)

#define SYMBOL_IS_FUNC(name)
#define HIDDEN_SYMBOL(name) ".private_extern " STR(name)
#define NO_EXEC_STACK_DIRECTIVE

#elif defined(__ELF__)

#if defined(__arm__)
#define SYMBOL_IS_FUNC(name) ".type " STR(name) ",%function"
#else
#define SYMBOL_IS_FUNC(name) ".type " STR(name) ",@function"
#endif
#define HIDDEN_SYMBOL(name) ".hidden " STR(name)

#if defined(__GNU__) || defined(__FreeBSD__) || defined(__Fuchsia__) || \
    defined(__linux__)
#define NO_EXEC_STACK_DIRECTIVE asm(R"( .section .note.GNU-stack,"",%progbits)");
#else
#define NO_EXEC_STACK_DIRECTIVE
#endif

#elif defined(_WIN32)

#define SYMBOL_IS_FUNC(name)                                                   \
asm("\
  .def " STR(name) " " STR(SEPARATOR) "                                                          \
    .scl 2 " STR(SEPARATOR) "                                                           \
    .type 32 " STR(SEPARATOR) "                                                         \
  .endef\
");
#define HIDDEN_SYMBOL(name)

#define NO_EXEC_STACK_DIRECTIVE

#else

#error Unsupported target

#endif

#define DEFINE_LIBUNWIND_FUNCTION(name)                   \
asm("\
  .globl " STR(SYMBOL_NAME(name)) " " STR(SEPARATOR) "                      \
  " SYMBOL_IS_FUNC(SYMBOL_NAME(name)) " " STR(SEPARATOR) "             \
  " STR(SYMBOL_NAME(name)) ":\
");

#define DEFINE_LIBUNWIND_PRIVATE_FUNCTION(name)           \
asm("\
  .globl " STR(SYMBOL_NAME(name)) " " STR(SEPARATOR) "                      \
  " HIDDEN_SYMBOL(SYMBOL_NAME(name)) " " STR(SEPARATOR) "              \
  " SYMBOL_IS_FUNC(SYMBOL_NAME(name)) " " STR(SEPARATOR) "             \
  " STR(SYMBOL_NAME(name)) ":\
");

#if defined(__arm__)
#if !defined(__ARM_ARCH)
#define __ARM_ARCH 4
#endif

#if defined(__ARM_ARCH_4T__) || __ARM_ARCH >= 5
#define ARM_HAS_BX
#endif

#ifdef ARM_HAS_BX
#define JMP(r) asm("bx r");
#else
#define JMP(r) asm("mov pc, r");
#endif
#endif /* __arm__ */

#endif /* UNWIND_ASSEMBLY_H */
