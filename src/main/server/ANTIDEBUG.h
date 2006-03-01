/** Fonctions d'anti debuggage.
@file ANTIDEBUG.h
   @author Christophe Romain
   @date 2005

Sont disponibles les macros suivantes:

FALSE_BREAKPOINT : ajoute un point d'arrêt
   ceci est à inclure dans les boucles et tous les endroits
   génants en cas d'optention du debuggage.
   attention, ceci provoque un appel de fonction. il conviens donc
   de ne pas l'utiliser dans les algorithmes consomateurs de
   ressources pour le service. préférer les boucles d'initialisation
   ou les zones non consomatrices de puissance.

QUIT_IF_BREAKPOINT(fonction) : quitte si un point d'arrêt est posé sur
   la fonction passé en paramètre. à parsemer dans l'initialisation
   du programme pour toute fonction importante du programme:
   algorithme, mécanismes de protection, lecture des données, etc...

QUIT_IF_DEBUG : quitte si le programme est debuggé ou tracé
   ceci est à parsemer dans l'initialisation et cause la fin de
   l'execution si le processus est en cours de debug ou de trace.

FALSE_DISASM : trompe les outils de désassemblage comme objdump.
   ceci est à mêttre à quelques endroits répartis du code.
   
 Ces dispositifs ne sont installés que dans le binaire Linux
 en version non DEBUG.
 
*/

#include <sys/types.h>
#include <sys/ptrace.h>
#include <signal.h>

/** @addtogroup m01
 @{
*/


#if defined(__linux__) && !defined(DEBUG)
#define ANTIDEBUG
#define FALSE_BREAKPOINT __asm__("int3");
#define QUIT_IF_BREAKPOINT(foo) if((*(volatile unsigned*)((unsigned)foo+3)&0xff)==0xcc) exit(0)
#define QUIT_IF_DEBUG if(ptrace(PTRACE_TRACEME,0,1,0)<0) exit(0)
#define FALSE_DISASM \
asm(" \
    jmp l1 + 2 \
l1: \
.short 0xc606 \
    call l2 \
l2: \
    popl %esi \
    jmp l3 \
l3: \
    addl $(l4-l2),%esi \
    movl 0(%esi),%edi \
    pushl %esi \
    jmp *%edi \
l4: \
    .long 0 \
")
#else
#define FALSE_BREAKPOINT
#define QUIT_IF_BREAKPOINT(foo)
#define QUIT_IF_DEBUG
#define FALSE_DISASM
#endif

#ifdef ANTIDEBUG
void handler( int sig ) {}
// appeler signal(handler, SIGTRAP);
// puis FALSE_BREAKPOINT
#endif

/** @} */
