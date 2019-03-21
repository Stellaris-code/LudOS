/*
signal.h

Copyright (c) 15 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef LUDOS_SIGNAL_H
#define LUDOS_SIGNAL_H

#include <sys/types.h>
#include <limits.h>

#define SIGHUP		 1
#define SIGINT		 2
#define SIGQUIT		 3
#define SIGILL		 4
#define SIGTRAP		 5
#define SIGABRT		 6
#define SIGIOT		 6
#define SIGBUS		 7
#define SIGFPE		 8
#define SIGKILL		 9
#define SIGUSR1		10
#define SIGSEGV		11
#define SIGUSR2		12
#define SIGPIPE		13
#define SIGALRM		14
#define SIGTERM		15
#define SIGSTKFLT	16
#define SIGCHLD		17
#define SIGCONT		18
#define SIGSTOP		19
#define SIGTSTP		20
#define SIGTTIN		21
#define SIGTTOU		22
#define SIGURG		23
#define SIGXCPU		24
#define SIGXFSZ		25
#define SIGVTALRM	26
#define SIGPROF		27
#define SIGWINCH	28
#define SIGIO		29
#define SIGPOLL		SIGIO
/*
#define SIGLOST		29
*/
#define SIGPWR		30
#define SIGSYS		31
#define	SIGUNUSED	31

/* These should not be considered constants from userland.  */
#define SIGRTMIN	32
#ifndef SIGRTMAX
#define SIGRTMAX	64
#endif

/*
 * SA_FLAGS values:
 *
 * SA_ONSTACK indicates that a registered stack_t will be used.
 * SA_RESTART flag to get restarting signals (which were the default long ago)
 * SA_NOCLDSTOP flag to turn off SIGCHLD when children stop.
 * SA_RESETHAND clears the handler when the signal is delivered.
 * SA_NOCLDWAIT flag on SIGCHLD to inhibit zombies.
 * SA_NODEFER prevents the current signal from being masked in the handler.
 *
 * SA_ONESHOT and SA_NOMASK are the historical Linux names for the Single
 * Unix names RESETHAND and NODEFER respectively.
 */
#define SA_NOCLDSTOP	0x00000001
#define SA_NOCLDWAIT	0x00000002
#define SA_SIGINFO	0x00000004
#define SA_ONSTACK	0x08000000
#define SA_RESTART	0x10000000
#define SA_NODEFER	0x40000000
#define SA_RESETHAND	0x80000000

#define SA_NOMASK	SA_NODEFER
#define SA_ONESHOT	SA_RESETHAND

/*
 * New architectures should not define the obsolete
 *	SA_RESTORER	0x04000000
 */

/*
 * sigaltstack controls
 */
#define SS_ONSTACK	1
#define SS_DISABLE	2

#define MINSIGSTKSZ	2048
#define SIGSTKSZ	8192

typedef struct {
    unsigned long sig[SIGRTMAX/(sizeof(unsigned long)*CHAR_BIT)];
} sigset_t;

typedef union {
    /* Members as suggested by Annex C of POSIX 1003.1b. */
    int	sival_int;
    void	*sival_ptr;
    /* 6.0 compatibility */
    int     sigval_int;
    void    *sigval_ptr;
} sigval_t;

typedef struct siginfo_t {
    int     si_signo;       /* Numéro de signal         */
    int     si_errno;       /* Numéro d'erreur          */
    int     si_code;        /* Code du signal           */
    pid_t   si_pid;         /* PID de l'émetteur        */
    uid_t   si_uid;         /* UID réel de l'émetteur   */
    int     si_status;      /* Valeur de sortie         */
    clock_t si_utime;       /* Temps utilisateur écoulé */
    clock_t si_stime;       /* Temps système écoulé     */
    sigval_t si_value;      /* Valeur de signal         */
    int     si_int;         /* Signal POSIX.1b          */
    void   *si_ptr;         /* Signal POSIX.1b          */
    void   *si_addr;        /* Emplacement d'erreur     */
    int     si_band;        /* Band event               */
    int     si_fd;          /* Descripteur de fichier   */
} siginfo_t;

typedef void (*sighandler_t)(int);
typedef void (sighandler_noptr_t)(int);
typedef void (*sigaction_handler_t)(int, siginfo_t*, void*);

struct sigaction {
    union
    {
        sighandler_t sa_handler;
        sigaction_handler_t sa_sigaction;
    };
    unsigned long sa_flags;
    sigset_t sa_mask;		/* mask last for extensibility */
};

typedef int sig_atomic_t;

#define SIG_DFL	((sighandler_t)0)	/* default signal handling */
#define SIG_IGN	((sighandler_t)1)	/* ignore signal */
#define SIG_ERR	((sighandler_t)-1) /* error return from signal */

#define SIG_ACTION_TERM (0)
#define SIG_ACTION_CORE (1)
#define SIG_ACTION_IGN  (2)
#define SIG_ACTION_CONT (3)
#define SIG_ACTION_STOP (4)

#endif // LUDOS_SIGNAL_H
