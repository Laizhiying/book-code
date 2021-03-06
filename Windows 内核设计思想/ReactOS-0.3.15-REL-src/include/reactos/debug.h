/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS kernel
 * FILE:            include/internal/debug.h
 * PURPOSE:         Useful debugging macros
 * PROGRAMMERS:     David Welch (welch@mcmail.com)
 *                  Hermes Belusca-Maito (hermes.belusca@sfr.fr)
 */

/*
 * NOTE: Define NDEBUG before including this header
 * to disable debugging macros.
 */

#ifndef __INTERNAL_DEBUG
#define __INTERNAL_DEBUG

/* Define DbgPrint/DbgPrintEx/RtlAssert unless the NDK is used */
#if !defined(_RTLFUNCS_H) && !defined(_NTDDK_)

/* Make sure we have basic types (some people include us *before* SDK)... */
#if !defined(_NTDEF_) && !defined(_NTDEF_H) && !defined(_WINDEF_) && !defined(_WINDEF_H)
    #error Please include SDK first.
#endif

ULONG
__cdecl
DbgPrint(
    _In_z_ _Printf_format_string_ PCSTR Format,
    ...
);

NTSYSAPI
ULONG
__cdecl
DbgPrintEx(
    _In_ ULONG ComponentId,
    _In_ ULONG Level,
    _In_z_ _Printf_format_string_ PCCH Format,
    ...
);

__analysis_noreturn
NTSYSAPI
VOID
NTAPI
RtlAssert(
    PVOID FailedAssertion,
    PVOID FileName,
    ULONG LineNumber,
    PCHAR Message
);

#endif /* !defined(_RTLFUNCS_H) && !defined(_NTDDK_) */

#ifndef assert
    #ifndef NASSERT
        #define assert(x) if (!(x)) { RtlAssert((PVOID)#x, (PVOID)__FILE__, __LINE__, ""); }
    #else
        #define assert(x)
    #endif
#endif

#ifndef ASSERT
    #ifndef NASSERT
        #define ASSERT(x) if (!(x)) { RtlAssert((PVOID)#x, (PVOID)__FILE__, __LINE__, ""); }
    #else
        #define ASSERT(x)
    #endif
#endif

#ifndef ASSERTMSG
    #ifndef NASSERT
        #define ASSERTMSG(x,m) if (!(x)) { RtlAssert((PVOID)#x, __FILE__, __LINE__, m); }
    #else
        #define ASSERTMSG(x)
    #endif
#endif

/* For internal purposes only */
#define __NOTICE(level, fmt, ...)   DbgPrint(#level ":  %s at %s:%d " fmt, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)

/* Print stuff only on Debug Builds*/
#define DPFLTR_DEFAULT_ID -1
//#if DBG

/* These are always printed */
#define DPRINT1(fmt, ...) do { \
        if (DbgPrint("(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__))  \
            DbgPrint("(%s:%d) DbgPrint() failed!\n", __FILE__, __LINE__); \
    } while (0)
#if DBG
/* These are printed only if NDEBUG is NOT defined */
#ifndef NDEBUG

#define DPRINT(fmt, ...) do { \
        if (DbgPrint("(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__))  \
            DbgPrint("(%s:%d) DbgPrint() failed!\n", __FILE__, __LINE__); \
    } while (0)

#else

#define DPRINT(...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)

#endif

#define UNIMPLEMENTED         __NOTICE(WARNING, "is UNIMPLEMENTED!\n");

#define ERR_(ch, fmt, ...)    DbgPrintEx(DPFLTR_##ch##_ID, DPFLTR_ERROR_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN_(ch, fmt, ...)   DbgPrintEx(DPFLTR_##ch##_ID, DPFLTR_WARNING_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define TRACE_(ch, fmt, ...)  DbgPrintEx(DPFLTR_##ch##_ID, DPFLTR_TRACE_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO_(ch, fmt, ...)   DbgPrintEx(DPFLTR_##ch##_ID, DPFLTR_INFO_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#define ERR__(ch, fmt, ...)    DbgPrintEx(ch, DPFLTR_ERROR_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define WARN__(ch, fmt, ...)   DbgPrintEx(ch, DPFLTR_WARNING_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define TRACE__(ch, fmt, ...)  DbgPrintEx(ch, DPFLTR_TRACE_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define INFO__(ch, fmt, ...)   DbgPrintEx(ch, DPFLTR_INFO_LEVEL, "(%s:%d) " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#else /* not DBG */

/* On non-debug builds, we never show these */
//#define DPRINT1(...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define DPRINT(...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)

#define UNIMPLEMENTED

#define ERR_(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define WARN_(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define TRACE_(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define INFO_(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)

#define ERR__(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define WARN__(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define TRACE__(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)
#define INFO__(ch, ...) do { if(0) { DbgPrint(__VA_ARGS__); } } while(0)

#endif /* not DBG */

/******************************************************************************/
/*
 * Declare a target-dependent process termination procedure.
 */
#ifndef _NTDDK_             /* User-Mode */
#ifndef NTOS_MODE_USER  /* Should be Win32 */
#ifndef _WIN32
    #error "Unsupported target."
#else
    #define TerminateCurrentProcess(Status) TerminateProcess(GetCurrentProcess(), (Status))
#endif
#else   /* Native */
#ifndef _PSFUNCS_H
NTSYSCALLAPI
NTSTATUS
NTAPI
NtTerminateProcess(
    IN HANDLE ProcessHandle,
    IN NTSTATUS ExitStatus
);
#endif
#ifndef NtCurrentProcess
    #define NtCurrentProcess() ((HANDLE)(LONG_PTR)-1)
#endif
#define TerminateCurrentProcess(Status) NtTerminateProcess(NtCurrentProcess(), (Status))
#endif
#else   /* Kernel-Mode */
#include <bugcodes.h>
#define TerminateCurrentProcess(Status) KeBugCheckEx(CRITICAL_SERVICE_FAILED, (Status), 0, 0, 0)
#endif


/* For internal purposes only */
#define __ERROR_DBGBREAK(...)   \
    do {                            \
        DbgPrint("" __VA_ARGS__);   \
        DbgBreakPoint();            \
    } while (0)

/* For internal purposes only */
#define __ERROR_FATAL(Status, ...)      \
    do {                                    \
        DbgPrint("" __VA_ARGS__);           \
        DbgBreakPoint();                    \
        TerminateCurrentProcess(Status);    \
    } while (0)

/*
 * These macros are designed to display an optional printf-like
 * user-defined message and to break into the debugger.
 * After that they allow to continue the program execution.
 */
#define ERROR_DBGBREAK(...)         \
    do {                                \
        __NOTICE(ERROR, "\n");          \
        __ERROR_DBGBREAK(__VA_ARGS__);  \
    } while (0)

#define UNIMPLEMENTED_DBGBREAK(...)         \
    do {                                        \
        __NOTICE(ERROR, "is UNIMPLEMENTED!\n"); \
        __ERROR_DBGBREAK(__VA_ARGS__);          \
    } while (0)

/*
 * These macros are designed to display an optional printf-like
 * user-defined message and to break into the debugger.
 * After that they halt the execution of the current thread.
 */
#define ERROR_FATAL(...)                                    \
    do {                                                        \
        __NOTICE(UNRECOVERABLE ERROR, "\n");                    \
        __ERROR_FATAL(STATUS_ASSERTION_FAILURE, __VA_ARGS__);   \
    } while (0)

#define UNIMPLEMENTED_FATAL(...)                            \
    do {                                                        \
        __NOTICE(UNRECOVERABLE ERROR, "is UNIMPLEMENTED!\n");   \
        __ERROR_FATAL(STATUS_NOT_IMPLEMENTED, __VA_ARGS__);     \
    } while (0)
/******************************************************************************/

#define ASSERT_IRQL_LESS_OR_EQUAL(x) ASSERT(KeGetCurrentIrql()<=(x))
#define ASSERT_IRQL_EQUAL(x) ASSERT(KeGetCurrentIrql()==(x))
#define ASSERT_IRQL_LESS(x) ASSERT(KeGetCurrentIrql()<(x))

#endif /* __INTERNAL_DEBUG */
