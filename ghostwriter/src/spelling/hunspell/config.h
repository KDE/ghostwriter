#ifndef HUNSPELL_CONFIG_H
#define HUNSPELL_CONFIG_H

/* Definitions shared between Linux, Mac OS X, and Windows. */
#define HAVE_ALLOCA 1
#define HAVE_BUILTIN_EXPECT 1
#define HAVE_FCNTL_H 1
#define HAVE_FWPRINTF 1
#define HAVE_GETCWD 1
#define HAVE_GETPAGESIZE 1
#define HAVE_INTMAX_T 1
#define HAVE_INTTYPES_H 1
#define HAVE_INTTYPES_H_WITH_UINTMAX 1
#define HAVE_LIMITS_H 1
#define HAVE_LOCALE_H 1
#define HAVE_LONG_LONG_INT 1
#define HAVE_MEMCHR 1
#define HAVE_MEMORY_H 1
#define HAVE_PTHREAD_MUTEX_RECURSIVE 1
#define HAVE_PTHREAD_RWLOCK 1
#define HAVE_PUTENV 1
#define HAVE_SETLOCALE 1
#define STDC_HEADERS 1
#define HAVE_SNPRINTF 1
#define HAVE_STDDEF_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDINT_H_WITH_UINTMAX 1
#define HAVE_STDLIB_H 1
#define HAVE_STRCASECMP 1
#define HAVE_STRCHR 1
#define HAVE_STRDUP 1
#define HAVE_STRSTR 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRTOUL 1
#define HAVE_SYS_PARAM_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_TSEARCH 1
#define HAVE_UINTMAX_T 1
#define HAVE_UNISTD_H 1
#define HAVE_UNSIGNED_LONG_LONG_INT 1
#define HAVE_VISIBILITY 1
#define HAVE_WCHAR_T 1
#define HAVE_WCSLEN 1
#define HAVE_WINT_T 1
#define INTDIV0_RAISES_SIGFPE 1
#define USE_POSIX_THREADS 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif

#define __libc_lock_t                   gl_lock_t
#define __libc_lock_define              gl_lock_define
#define __libc_lock_define_initialized  gl_lock_define_initialized
#define __libc_lock_init                gl_lock_init
#define __libc_lock_lock                gl_lock_lock
#define __libc_lock_unlock              gl_lock_unlock
#define __libc_lock_recursive_t                   gl_recursive_lock_t
#define __libc_lock_define_recursive              gl_recursive_lock_define
#define __libc_lock_define_initialized_recursive  gl_recursive_lock_define_initialized
#define __libc_lock_init_recursive                gl_recursive_lock_init
#define __libc_lock_lock_recursive                gl_recursive_lock_lock
#define __libc_lock_unlock_recursive              gl_recursive_lock_unlock
#define glthread_in_use  libintl_thread_in_use
#define glthread_lock_init     libintl_lock_init
#define glthread_lock_lock     libintl_lock_lock
#define glthread_lock_unlock   libintl_lock_unlock
#define glthread_lock_destroy  libintl_lock_destroy
#define glthread_rwlock_init     libintl_rwlock_init
#define glthread_rwlock_rdlock   libintl_rwlock_rdlock
#define glthread_rwlock_wrlock   libintl_rwlock_wrlock
#define glthread_rwlock_unlock   libintl_rwlock_unlock
#define glthread_rwlock_destroy  libintl_rwlock_destroy
#define glthread_recursive_lock_init     libintl_recursive_lock_init
#define glthread_recursive_lock_lock     libintl_recursive_lock_lock
#define glthread_recursive_lock_unlock   libintl_recursive_lock_unlock
#define glthread_recursive_lock_destroy  libintl_recursive_lock_destroy
#define glthread_once                 libintl_once
#define glthread_once_call            libintl_once_call
#define glthread_once_singlethreaded  libintl_once_singlethreaded

/* Definitions shared between Linux and Mac OS X. */
#ifdef __unix__
  #define HAVE_ALLOCA_H 1
  #define HAVE_ASPRINTF 1
  #define HAVE_DECL_FEOF_UNLOCKED 1
  #define HAVE_DECL_GETC_UNLOCKED 1
  #define HAVE_DECL__SNPRINTF 0
  #define HAVE_DECL__SNWPRINTF 0
  #define HAVE_DLFCN_H 1
  #define HAVE_GETEGID 1
  #define HAVE_GETEUID 1
  #define HAVE_GETGID 1
  #define HAVE_GETUID 1
  #define HAVE_ICONV 1
  #define HAVE_LANGINFO_CODESET 1
  #define HAVE_LC_MESSAGES 1
  #define HAVE_MMAP 1
  #define HAVE_MUNMAP 1
  #define HAVE_NL_LOCALE_NAME 1
  #define HAVE_POSIX_PRINTF 1
  #define HAVE_SETENV 1
  #define HAVE_STPCPY 1
  #define ICONV_CONST 
#endif

/* Definitions shared between Linux and Windows. */
#if (defined __linux || defined _WIN32)
  #define USE_POSIX_THREADS_WEAK 1
#endif

/* Definitions specific to Linux. */
#ifdef __linux
  #define HAVE_ARGZ_COUNT 1
  #define HAVE_ARGZ_H 1
  #define HAVE_ARGZ_NEXT 1
  #define HAVE_ARGZ_STRINGIFY 1
  #define HAVE_DECL_FGETS_UNLOCKED 1
  #define HAVE_ERROR_H 1
  #define HAVE_LIBINTL_H 1
  #define HAVE_MEMPCPY 1
  #define HAVE___FSETLOCKING 1
#endif

/* Definitions specific to Mac OS X. */
#ifdef __APPLE__
  #define HAVE_CFLOCALECOPYCURRENT 1
  #define HAVE_CFPREFERENCESCOPYAPPVALUE 1
  #define HAVE_DECL_FGETS_UNLOCKED 0
#endif

/* Definitions specific to Windows. */
#ifdef _WIN32
  #define HAVE_DECL_FEOF_UNLOCKED 0
  #define HAVE_DECL_FGETS_UNLOCKED 0
  #define HAVE_DECL_GETC_UNLOCKED 0
  #define HAVE_DECL__SNPRINTF 1
  #define HAVE_DECL__SNWPRINTF 1
#endif

#endif
