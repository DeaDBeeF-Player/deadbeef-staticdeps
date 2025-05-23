/* DO NOT EDIT! GENERATED AUTOMATICALLY! */
/* A GNU-like <string.h>.

   Copyright (C) 1995-1996, 2001-2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef _GL_STRING_H

#if __GNUC__ >= 3
#pragma GCC system_header
#endif

/* The include_next requires a split double-inclusion guard.  */
#include_next <string.h>

#ifndef _GL_STRING_H
#define _GL_STRING_H

/* NetBSD 5.0 mis-defines NULL.  */
#include <stddef.h>

/* MirBSD defines mbslen as a macro.  */
#if 0 && defined __MirBSD__
# include <wchar.h>
#endif

#ifndef __attribute__
/* This feature is available in gcc versions 2.5 and later.  */
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 5)
#  define __attribute__(Spec) /* empty */
# endif
/* The attribute __pure__ was added in gcc 2.96.  */
# if __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 96)
#  define __pure__ /* empty */
# endif
#endif


/* The definitions of _GL_FUNCDECL_RPL etc. are copied here.  */
#ifndef _GL_CXXDEFS_H
#define _GL_CXXDEFS_H

/* The three most frequent use cases of these macros are:

   * For providing a substitute for a function that is missing on some
     platforms, but is declared and works fine on the platforms on which
     it exists:

       #if @GNULIB_FOO@
       # if !@HAVE_FOO@
       _GL_FUNCDECL_SYS (foo, ...);
       # endif
       _GL_CXXALIAS_SYS (foo, ...);
       _GL_CXXALIASWARN (foo);
       #elif defined GNULIB_POSIXCHECK
       ...
       #endif

   * For providing a replacement for a function that exists on all platforms,
     but is broken/insufficient and needs to be replaced on some platforms:

       #if @GNULIB_FOO@
       # if @REPLACE_FOO@
       #  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
       #   undef foo
       #   define foo rpl_foo
       #  endif
       _GL_FUNCDECL_RPL (foo, ...);
       _GL_CXXALIAS_RPL (foo, ...);
       # else
       _GL_CXXALIAS_SYS (foo, ...);
       # endif
       _GL_CXXALIASWARN (foo);
       #elif defined GNULIB_POSIXCHECK
       ...
       #endif

   * For providing a replacement for a function that exists on some platforms
     but is broken/insufficient and needs to be replaced on some of them and
     is additionally either missing or undeclared on some other platforms:

       #if @GNULIB_FOO@
       # if @REPLACE_FOO@
       #  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
       #   undef foo
       #   define foo rpl_foo
       #  endif
       _GL_FUNCDECL_RPL (foo, ...);
       _GL_CXXALIAS_RPL (foo, ...);
       # else
       #  if !@HAVE_FOO@   or   if !@HAVE_DECL_FOO@
       _GL_FUNCDECL_SYS (foo, ...);
       #  endif
       _GL_CXXALIAS_SYS (foo, ...);
       # endif
       _GL_CXXALIASWARN (foo);
       #elif defined GNULIB_POSIXCHECK
       ...
       #endif
*/

/* _GL_EXTERN_C declaration;
   performs the declaration with C linkage.  */
#if defined __cplusplus
# define _GL_EXTERN_C extern "C"
#else
# define _GL_EXTERN_C extern
#endif

/* _GL_FUNCDECL_RPL (func, rettype, parameters_and_attributes);
   declares a replacement function, named rpl_func, with the given prototype,
   consisting of return type, parameters, and attributes.
   Example:
     _GL_FUNCDECL_RPL (open, int, (const char *filename, int flags, ...)
                                  _GL_ARG_NONNULL ((1)));
 */
#define _GL_FUNCDECL_RPL(func,rettype,parameters_and_attributes) \
  _GL_FUNCDECL_RPL_1 (rpl_##func, rettype, parameters_and_attributes)
#define _GL_FUNCDECL_RPL_1(rpl_func,rettype,parameters_and_attributes) \
  _GL_EXTERN_C rettype rpl_func parameters_and_attributes

/* _GL_FUNCDECL_SYS (func, rettype, parameters_and_attributes);
   declares the system function, named func, with the given prototype,
   consisting of return type, parameters, and attributes.
   Example:
     _GL_FUNCDECL_SYS (open, int, (const char *filename, int flags, ...)
                                  _GL_ARG_NONNULL ((1)));
 */
#define _GL_FUNCDECL_SYS(func,rettype,parameters_and_attributes) \
  _GL_EXTERN_C rettype func parameters_and_attributes

/* _GL_CXXALIAS_RPL (func, rettype, parameters);
   declares a C++ alias called GNULIB_NAMESPACE::func
   that redirects to rpl_func, if GNULIB_NAMESPACE is defined.
   Example:
     _GL_CXXALIAS_RPL (open, int, (const char *filename, int flags, ...));
 */
#define _GL_CXXALIAS_RPL(func,rettype,parameters) \
  _GL_CXXALIAS_RPL_1 (func, rpl_##func, rettype, parameters)
#if defined __cplusplus && defined GNULIB_NAMESPACE
# define _GL_CXXALIAS_RPL_1(func,rpl_func,rettype,parameters) \
    namespace GNULIB_NAMESPACE                                \
    {                                                         \
      rettype (*const func) parameters = ::rpl_func;          \
    }                                                         \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#else
# define _GL_CXXALIAS_RPL_1(func,rpl_func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

/* _GL_CXXALIAS_RPL_CAST_1 (func, rpl_func, rettype, parameters);
   is like  _GL_CXXALIAS_RPL_1 (func, rpl_func, rettype, parameters);
   except that the C function rpl_func may have a slightly different
   declaration.  A cast is used to silence the "invalid conversion" error
   that would otherwise occur.  */
#if defined __cplusplus && defined GNULIB_NAMESPACE
# define _GL_CXXALIAS_RPL_CAST_1(func,rpl_func,rettype,parameters) \
    namespace GNULIB_NAMESPACE                                     \
    {                                                              \
      rettype (*const func) parameters =                           \
        reinterpret_cast<rettype(*)parameters>(::rpl_func);        \
    }                                                              \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#else
# define _GL_CXXALIAS_RPL_CAST_1(func,rpl_func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

/* _GL_CXXALIAS_SYS (func, rettype, parameters);
   declares a C++ alias called GNULIB_NAMESPACE::func
   that redirects to the system provided function func, if GNULIB_NAMESPACE
   is defined.
   Example:
     _GL_CXXALIAS_SYS (open, int, (const char *filename, int flags, ...));
 */
#if defined __cplusplus && defined GNULIB_NAMESPACE
  /* If we were to write
       rettype (*const func) parameters = ::func;
     like above in _GL_CXXALIAS_RPL_1, the compiler could optimize calls
     better (remove an indirection through a 'static' pointer variable),
     but then the _GL_CXXALIASWARN macro below would cause a warning not only
     for uses of ::func but also for uses of GNULIB_NAMESPACE::func.  */
# define _GL_CXXALIAS_SYS(func,rettype,parameters) \
    namespace GNULIB_NAMESPACE                     \
    {                                              \
      static rettype (*func) parameters = ::func;  \
    }                                              \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#else
# define _GL_CXXALIAS_SYS(func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

/* _GL_CXXALIAS_SYS_CAST (func, rettype, parameters);
   is like  _GL_CXXALIAS_SYS (func, rettype, parameters);
   except that the C function func may have a slightly different declaration.
   A cast is used to silence the "invalid conversion" error that would
   otherwise occur.  */
#if defined __cplusplus && defined GNULIB_NAMESPACE
# define _GL_CXXALIAS_SYS_CAST(func,rettype,parameters) \
    namespace GNULIB_NAMESPACE                          \
    {                                                   \
      static rettype (*func) parameters =               \
        reinterpret_cast<rettype(*)parameters>(::func); \
    }                                                   \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#else
# define _GL_CXXALIAS_SYS_CAST(func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

/* _GL_CXXALIAS_SYS_CAST2 (func, rettype, parameters, rettype2, parameters2);
   is like  _GL_CXXALIAS_SYS (func, rettype, parameters);
   except that the C function is picked among a set of overloaded functions,
   namely the one with rettype2 and parameters2.  Two consecutive casts
   are used to silence the "cannot find a match" and "invalid conversion"
   errors that would otherwise occur.  */
#if defined __cplusplus && defined GNULIB_NAMESPACE
  /* The outer cast must be a reinterpret_cast.
     The inner cast: When the function is defined as a set of overloaded
     functions, it works as a static_cast<>, choosing the designated variant.
     When the function is defined as a single variant, it works as a
     reinterpret_cast<>. The parenthesized cast syntax works both ways.  */
# define _GL_CXXALIAS_SYS_CAST2(func,rettype,parameters,rettype2,parameters2) \
    namespace GNULIB_NAMESPACE                                                \
    {                                                                         \
      static rettype (*func) parameters =                                     \
        reinterpret_cast<rettype(*)parameters>(                               \
          (rettype2(*)parameters2)(::func));                                  \
    }                                                                         \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#else
# define _GL_CXXALIAS_SYS_CAST2(func,rettype,parameters,rettype2,parameters2) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

/* _GL_CXXALIASWARN (func);
   causes a warning to be emitted when ::func is used but not when
   GNULIB_NAMESPACE::func is used.  func must be defined without overloaded
   variants.  */
#if defined __cplusplus && defined GNULIB_NAMESPACE
# define _GL_CXXALIASWARN(func) \
   _GL_CXXALIASWARN_1 (func, GNULIB_NAMESPACE)
# define _GL_CXXALIASWARN_1(func,namespace) \
   _GL_CXXALIASWARN_2 (func, namespace)
/* To work around GCC bug <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43881>,
   we enable the warning only when not optimizing.  */
# if !__OPTIMIZE__
#  define _GL_CXXALIASWARN_2(func,namespace) \
    _GL_WARN_ON_USE (func, \
                     "The symbol ::" #func " refers to the system function. " \
                     "Use " #namespace "::" #func " instead.")
# elif __GNUC__ >= 3 && GNULIB_STRICT_CHECKING
#  define _GL_CXXALIASWARN_2(func,namespace) \
     extern __typeof__ (func) func
# else
#  define _GL_CXXALIASWARN_2(func,namespace) \
     _GL_EXTERN_C int _gl_cxxalias_dummy
# endif
#else
# define _GL_CXXALIASWARN(func) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

/* _GL_CXXALIASWARN1 (func, rettype, parameters_and_attributes);
   causes a warning to be emitted when the given overloaded variant of ::func
   is used but not when GNULIB_NAMESPACE::func is used.  */
#if defined __cplusplus && defined GNULIB_NAMESPACE
# define _GL_CXXALIASWARN1(func,rettype,parameters_and_attributes) \
   _GL_CXXALIASWARN1_1 (func, rettype, parameters_and_attributes, \
                        GNULIB_NAMESPACE)
# define _GL_CXXALIASWARN1_1(func,rettype,parameters_and_attributes,namespace) \
   _GL_CXXALIASWARN1_2 (func, rettype, parameters_and_attributes, namespace)
/* To work around GCC bug <http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43881>,
   we enable the warning only when not optimizing.  */
# if !__OPTIMIZE__
#  define _GL_CXXALIASWARN1_2(func,rettype,parameters_and_attributes,namespace) \
    _GL_WARN_ON_USE_CXX (func, rettype, parameters_and_attributes, \
                         "The symbol ::" #func " refers to the system function. " \
                         "Use " #namespace "::" #func " instead.")
# elif __GNUC__ >= 3 && GNULIB_STRICT_CHECKING
#  define _GL_CXXALIASWARN1_2(func,rettype,parameters_and_attributes,namespace) \
     extern __typeof__ (func) func
# else
#  define _GL_CXXALIASWARN1_2(func,rettype,parameters_and_attributes,namespace) \
     _GL_EXTERN_C int _gl_cxxalias_dummy
# endif
#else
# define _GL_CXXALIASWARN1(func,rettype,parameters_and_attributes) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
#endif

#endif /* _GL_CXXDEFS_H */

/* The definition of _GL_ARG_NONNULL is copied here.  */
/* _GL_ARG_NONNULL((n,...,m)) tells the compiler and static analyzer tools
   that the values passed as arguments n, ..., m must be non-NULL pointers.
   n = 1 stands for the first argument, n = 2 for the second argument etc.  */
#ifndef _GL_ARG_NONNULL
# if (__GNUC__ == 3 && __GNUC_MINOR__ >= 3) || __GNUC__ > 3
#  define _GL_ARG_NONNULL(params) __attribute__ ((__nonnull__ params))
# else
#  define _GL_ARG_NONNULL(params)
# endif
#endif

/* The definition of _GL_WARN_ON_USE is copied here.  */
#ifndef _GL_WARN_ON_USE

# if 4 < __GNUC__ || (__GNUC__ == 4 && 3 <= __GNUC_MINOR__)
/* A compiler attribute is available in gcc versions 4.3.0 and later.  */
#  define _GL_WARN_ON_USE(function, message) \
extern __typeof__ (function) function __attribute__ ((__warning__ (message)))
# elif __GNUC__ >= 3 && GNULIB_STRICT_CHECKING
/* Verify the existence of the function.  */
#  define _GL_WARN_ON_USE(function, message) \
extern __typeof__ (function) function
# else /* Unsupported.  */
#  define _GL_WARN_ON_USE(function, message) \
_GL_WARN_EXTERN_C int _gl_warn_on_use
# endif
#endif

/* _GL_WARN_ON_USE_CXX (function, rettype, parameters_and_attributes, "string")
   is like _GL_WARN_ON_USE (function, "string"), except that the function is
   declared with the given prototype, consisting of return type, parameters,
   and attributes.
   This variant is useful for overloaded functions in C++. _GL_WARN_ON_USE does
   not work in this case.  */
#ifndef _GL_WARN_ON_USE_CXX
# if 4 < __GNUC__ || (__GNUC__ == 4 && 3 <= __GNUC_MINOR__)
#  define _GL_WARN_ON_USE_CXX(function,rettype,parameters_and_attributes,msg) \
extern rettype function parameters_and_attributes \
     __attribute__ ((__warning__ (msg)))
# elif __GNUC__ >= 3 && GNULIB_STRICT_CHECKING
/* Verify the existence of the function.  */
#  define _GL_WARN_ON_USE_CXX(function,rettype,parameters_and_attributes,msg) \
extern rettype function parameters_and_attributes
# else /* Unsupported.  */
#  define _GL_WARN_ON_USE_CXX(function,rettype,parameters_and_attributes,msg) \
_GL_WARN_EXTERN_C int _gl_warn_on_use
# endif
#endif

/* _GL_WARN_EXTERN_C declaration;
   performs the declaration with C linkage.  */
#ifndef _GL_WARN_EXTERN_C
# if defined __cplusplus
#  define _GL_WARN_EXTERN_C extern "C"
# else
#  define _GL_WARN_EXTERN_C extern
# endif
#endif


/* Return the first instance of C within N bytes of S, or NULL.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define memchr rpl_memchr
#  endif
_GL_FUNCDECL_RPL (memchr, void *, (void const *__s, int __c, size_t __n)
                                  __attribute__ ((__pure__))
                                  _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (memchr, void *, (void const *__s, int __c, size_t __n));
# else
#  if ! 1
_GL_FUNCDECL_SYS (memchr, void *, (void const *__s, int __c, size_t __n)
                                  __attribute__ ((__pure__))
                                  _GL_ARG_NONNULL ((1)));
#  endif
  /* On some systems, this function is defined as an overloaded function:
       extern "C" { const void * std::memchr (const void *, int, size_t); }
       extern "C++" { void * std::memchr (void *, int, size_t); }  */
_GL_CXXALIAS_SYS_CAST2 (memchr,
                        void *, (void const *__s, int __c, size_t __n),
                        void const *, (void const *__s, int __c, size_t __n));
# endif
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (memchr, void *, (void *__s, int __c, size_t __n));
_GL_CXXALIASWARN1 (memchr, void const *,
                   (void const *__s, int __c, size_t __n));
# else
_GL_CXXALIASWARN (memchr);
# endif
#elif defined GNULIB_POSIXCHECK
# undef memchr
/* Assume memchr is always declared.  */
_GL_WARN_ON_USE (memchr, "memchr has platform-specific bugs - "
                 "use gnulib module memchr for portability" );
#endif

/* Return the first occurrence of NEEDLE in HAYSTACK.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define memmem rpl_memmem
#  endif
_GL_FUNCDECL_RPL (memmem, void *,
                  (void const *__haystack, size_t __haystack_len,
                   void const *__needle, size_t __needle_len)
                  __attribute__ ((__pure__)) _GL_ARG_NONNULL ((1, 3)));
_GL_CXXALIAS_RPL (memmem, void *,
                  (void const *__haystack, size_t __haystack_len,
                   void const *__needle, size_t __needle_len));
# else
#  if ! 1
_GL_FUNCDECL_SYS (memmem, void *,
                  (void const *__haystack, size_t __haystack_len,
                   void const *__needle, size_t __needle_len)
                  __attribute__ ((__pure__)) _GL_ARG_NONNULL ((1, 3)));
#  endif
_GL_CXXALIAS_SYS (memmem, void *,
                  (void const *__haystack, size_t __haystack_len,
                   void const *__needle, size_t __needle_len));
# endif
_GL_CXXALIASWARN (memmem);
#elif defined GNULIB_POSIXCHECK
# undef memmem
# if HAVE_RAW_DECL_MEMMEM
_GL_WARN_ON_USE (memmem, "memmem is unportable and often quadratic - "
                 "use gnulib module memmem-simple for portability, "
                 "and module memmem for speed" );
# endif
#endif

/* Copy N bytes of SRC to DEST, return pointer to bytes after the
   last written byte.  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (mempcpy, void *,
                  (void *restrict __dest, void const *restrict __src,
                   size_t __n)
                  _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS (mempcpy, void *,
                  (void *restrict __dest, void const *restrict __src,
                   size_t __n));
_GL_CXXALIASWARN (mempcpy);
#elif defined GNULIB_POSIXCHECK
# undef mempcpy
# if HAVE_RAW_DECL_MEMPCPY
_GL_WARN_ON_USE (mempcpy, "mempcpy is unportable - "
                 "use gnulib module mempcpy for portability");
# endif
#endif

/* Search backwards through a block for a byte (specified as an int).  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (memrchr, void *, (void const *, int, size_t)
                                   __attribute__ ((__pure__))
                                   _GL_ARG_NONNULL ((1)));
# endif
  /* On some systems, this function is defined as an overloaded function:
       extern "C++" { const void * std::memrchr (const void *, int, size_t); }
       extern "C++" { void * std::memrchr (void *, int, size_t); }  */
_GL_CXXALIAS_SYS_CAST2 (memrchr,
                        void *, (void const *, int, size_t),
                        void const *, (void const *, int, size_t));
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (memrchr, void *, (void *, int, size_t));
_GL_CXXALIASWARN1 (memrchr, void const *, (void const *, int, size_t));
# else
_GL_CXXALIASWARN (memrchr);
# endif
#elif defined GNULIB_POSIXCHECK
# undef memrchr
# if HAVE_RAW_DECL_MEMRCHR
_GL_WARN_ON_USE (memrchr, "memrchr is unportable - "
                 "use gnulib module memrchr for portability");
# endif
#endif

/* Find the first occurrence of C in S.  More efficient than
   memchr(S,C,N), at the expense of undefined behavior if C does not
   occur within N bytes.  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (rawmemchr, void *, (void const *__s, int __c_in)
                                     __attribute__ ((__pure__))
                                     _GL_ARG_NONNULL ((1)));
# endif
  /* On some systems, this function is defined as an overloaded function:
       extern "C++" { const void * std::rawmemchr (const void *, int); }
       extern "C++" { void * std::rawmemchr (void *, int); }  */
_GL_CXXALIAS_SYS_CAST2 (rawmemchr,
                        void *, (void const *__s, int __c_in),
                        void const *, (void const *__s, int __c_in));
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (rawmemchr, void *, (void *__s, int __c_in));
_GL_CXXALIASWARN1 (rawmemchr, void const *, (void const *__s, int __c_in));
# else
_GL_CXXALIASWARN (rawmemchr);
# endif
#elif defined GNULIB_POSIXCHECK
# undef rawmemchr
# if HAVE_RAW_DECL_RAWMEMCHR
_GL_WARN_ON_USE (rawmemchr, "rawmemchr is unportable - "
                 "use gnulib module rawmemchr for portability");
# endif
#endif

/* Copy SRC to DST, returning the address of the terminating '\0' in DST.  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (stpcpy, char *,
                  (char *restrict __dst, char const *restrict __src)
                  _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS (stpcpy, char *,
                  (char *restrict __dst, char const *restrict __src));
_GL_CXXALIASWARN (stpcpy);
#elif defined GNULIB_POSIXCHECK
# undef stpcpy
# if HAVE_RAW_DECL_STPCPY
_GL_WARN_ON_USE (stpcpy, "stpcpy is unportable - "
                 "use gnulib module stpcpy for portability");
# endif
#endif

/* Copy no more than N bytes of SRC to DST, returning a pointer past the
   last non-NUL byte written into DST.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define stpncpy rpl_stpncpy
#  endif
_GL_FUNCDECL_RPL (stpncpy, char *,
                  (char *restrict __dst, char const *restrict __src,
                   size_t __n)
                  _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (stpncpy, char *,
                  (char *restrict __dst, char const *restrict __src,
                   size_t __n));
# else
#  if ! 1
_GL_FUNCDECL_SYS (stpncpy, char *,
                  (char *restrict __dst, char const *restrict __src,
                   size_t __n)
                  _GL_ARG_NONNULL ((1, 2)));
#  endif
_GL_CXXALIAS_SYS (stpncpy, char *,
                  (char *restrict __dst, char const *restrict __src,
                   size_t __n));
# endif
_GL_CXXALIASWARN (stpncpy);
#elif defined GNULIB_POSIXCHECK
# undef stpncpy
# if HAVE_RAW_DECL_STPNCPY
_GL_WARN_ON_USE (stpncpy, "stpncpy is unportable - "
                 "use gnulib module stpncpy for portability");
# endif
#endif

#if defined GNULIB_POSIXCHECK
/* strchr() does not work with multibyte strings if the locale encoding is
   GB18030 and the character to be searched is a digit.  */
# undef strchr
/* Assume strchr is always declared.  */
_GL_WARN_ON_USE (strchr, "strchr cannot work correctly on character strings "
                 "in some multibyte locales - "
                 "use mbschr if you care about internationalization");
#endif

/* Find the first occurrence of C in S or the final NUL byte.  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (strchrnul, char *, (char const *__s, int __c_in)
                                     __attribute__ ((__pure__))
                                     _GL_ARG_NONNULL ((1)));
# endif
  /* On some systems, this function is defined as an overloaded function:
       extern "C++" { const char * std::strchrnul (const char *, int); }
       extern "C++" { char * std::strchrnul (char *, int); }  */
_GL_CXXALIAS_SYS_CAST2 (strchrnul,
                        char *, (char const *__s, int __c_in),
                        char const *, (char const *__s, int __c_in));
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (strchrnul, char *, (char *__s, int __c_in));
_GL_CXXALIASWARN1 (strchrnul, char const *, (char const *__s, int __c_in));
# else
_GL_CXXALIASWARN (strchrnul);
# endif
#elif defined GNULIB_POSIXCHECK
# undef strchrnul
# if HAVE_RAW_DECL_STRCHRNUL
_GL_WARN_ON_USE (strchrnul, "strchrnul is unportable - "
                 "use gnulib module strchrnul for portability");
# endif
#endif

/* Duplicate S, returning an identical malloc'd string.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef strdup
#   define strdup rpl_strdup
#  endif
_GL_FUNCDECL_RPL (strdup, char *, (char const *__s) _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (strdup, char *, (char const *__s));
# else
#  if !(1 || defined strdup)
_GL_FUNCDECL_SYS (strdup, char *, (char const *__s) _GL_ARG_NONNULL ((1)));
#  endif
_GL_CXXALIAS_SYS (strdup, char *, (char const *__s));
# endif
_GL_CXXALIASWARN (strdup);
#elif defined GNULIB_POSIXCHECK
# undef strdup
# if HAVE_RAW_DECL_STRDUP
_GL_WARN_ON_USE (strdup, "strdup is unportable - "
                 "use gnulib module strdup for portability");
# endif
#endif

/* Append no more than N characters from SRC onto DEST.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef strncat
#   define strncat rpl_strncat
#  endif
_GL_FUNCDECL_RPL (strncat, char *, (char *dest, const char *src, size_t n)
                                   _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (strncat, char *, (char *dest, const char *src, size_t n));
# else
_GL_CXXALIAS_SYS (strncat, char *, (char *dest, const char *src, size_t n));
# endif
_GL_CXXALIASWARN (strncat);
#elif defined GNULIB_POSIXCHECK
# undef strncat
# if HAVE_RAW_DECL_STRNCAT
_GL_WARN_ON_USE (strncat, "strncat is unportable - "
                 "use gnulib module strncat for portability");
# endif
#endif

/* Return a newly allocated copy of at most N bytes of STRING.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef strndup
#   define strndup rpl_strndup
#  endif
_GL_FUNCDECL_RPL (strndup, char *, (char const *__string, size_t __n)
                                   _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (strndup, char *, (char const *__string, size_t __n));
# else
#  if ! 1
_GL_FUNCDECL_SYS (strndup, char *, (char const *__string, size_t __n)
                                   _GL_ARG_NONNULL ((1)));
#  endif
_GL_CXXALIAS_SYS (strndup, char *, (char const *__string, size_t __n));
# endif
_GL_CXXALIASWARN (strndup);
#elif defined GNULIB_POSIXCHECK
# undef strndup
# if HAVE_RAW_DECL_STRNDUP
_GL_WARN_ON_USE (strndup, "strndup is unportable - "
                 "use gnulib module strndup for portability");
# endif
#endif

/* Find the length (number of bytes) of STRING, but scan at most
   MAXLEN bytes.  If no '\0' terminator is found in that many bytes,
   return MAXLEN.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef strnlen
#   define strnlen rpl_strnlen
#  endif
_GL_FUNCDECL_RPL (strnlen, size_t, (char const *__string, size_t __maxlen)
                                   __attribute__ ((__pure__))
                                   _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (strnlen, size_t, (char const *__string, size_t __maxlen));
# else
#  if ! 1
_GL_FUNCDECL_SYS (strnlen, size_t, (char const *__string, size_t __maxlen)
                                   __attribute__ ((__pure__))
                                   _GL_ARG_NONNULL ((1)));
#  endif
_GL_CXXALIAS_SYS (strnlen, size_t, (char const *__string, size_t __maxlen));
# endif
_GL_CXXALIASWARN (strnlen);
#elif defined GNULIB_POSIXCHECK
# undef strnlen
# if HAVE_RAW_DECL_STRNLEN
_GL_WARN_ON_USE (strnlen, "strnlen is unportable - "
                 "use gnulib module strnlen for portability");
# endif
#endif

#if defined GNULIB_POSIXCHECK
/* strcspn() assumes the second argument is a list of single-byte characters.
   Even in this simple case, it does not work with multibyte strings if the
   locale encoding is GB18030 and one of the characters to be searched is a
   digit.  */
# undef strcspn
/* Assume strcspn is always declared.  */
_GL_WARN_ON_USE (strcspn, "strcspn cannot work correctly on character strings "
                 "in multibyte locales - "
                 "use mbscspn if you care about internationalization");
#endif

/* Find the first occurrence in S of any character in ACCEPT.  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (strpbrk, char *, (char const *__s, char const *__accept)
                                   __attribute__ ((__pure__))
                                   _GL_ARG_NONNULL ((1, 2)));
# endif
  /* On some systems, this function is defined as an overloaded function:
       extern "C" { const char * strpbrk (const char *, const char *); }
       extern "C++" { char * strpbrk (char *, const char *); }  */
_GL_CXXALIAS_SYS_CAST2 (strpbrk,
                        char *, (char const *__s, char const *__accept),
                        const char *, (char const *__s, char const *__accept));
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (strpbrk, char *, (char *__s, char const *__accept));
_GL_CXXALIASWARN1 (strpbrk, char const *,
                   (char const *__s, char const *__accept));
# else
_GL_CXXALIASWARN (strpbrk);
# endif
# if defined GNULIB_POSIXCHECK
/* strpbrk() assumes the second argument is a list of single-byte characters.
   Even in this simple case, it does not work with multibyte strings if the
   locale encoding is GB18030 and one of the characters to be searched is a
   digit.  */
#  undef strpbrk
_GL_WARN_ON_USE (strpbrk, "strpbrk cannot work correctly on character strings "
                 "in multibyte locales - "
                 "use mbspbrk if you care about internationalization");
# endif
#elif defined GNULIB_POSIXCHECK
# undef strpbrk
# if HAVE_RAW_DECL_STRPBRK
_GL_WARN_ON_USE (strpbrk, "strpbrk is unportable - "
                 "use gnulib module strpbrk for portability");
# endif
#endif

#if defined GNULIB_POSIXCHECK
/* strspn() assumes the second argument is a list of single-byte characters.
   Even in this simple case, it cannot work with multibyte strings.  */
# undef strspn
/* Assume strspn is always declared.  */
_GL_WARN_ON_USE (strspn, "strspn cannot work correctly on character strings "
                 "in multibyte locales - "
                 "use mbsspn if you care about internationalization");
#endif

#if defined GNULIB_POSIXCHECK
/* strrchr() does not work with multibyte strings if the locale encoding is
   GB18030 and the character to be searched is a digit.  */
# undef strrchr
/* Assume strrchr is always declared.  */
_GL_WARN_ON_USE (strrchr, "strrchr cannot work correctly on character strings "
                 "in some multibyte locales - "
                 "use mbsrchr if you care about internationalization");
#endif

/* Search the next delimiter (char listed in DELIM) starting at *STRINGP.
   If one is found, overwrite it with a NUL, and advance *STRINGP
   to point to the next char after it.  Otherwise, set *STRINGP to NULL.
   If *STRINGP was already NULL, nothing happens.
   Return the old value of *STRINGP.

   This is a variant of strtok() that is multithread-safe and supports
   empty fields.

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.
   Caveat: It doesn't work with multibyte strings unless all of the delimiter
           characters are ASCII characters < 0x30.

   See also strtok_r().  */
#if 0
# if ! 1
_GL_FUNCDECL_SYS (strsep, char *,
                  (char **restrict __stringp, char const *restrict __delim)
                  _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS (strsep, char *,
                  (char **restrict __stringp, char const *restrict __delim));
_GL_CXXALIASWARN (strsep);
# if defined GNULIB_POSIXCHECK
#  undef strsep
_GL_WARN_ON_USE (strsep, "strsep cannot work correctly on character strings "
                 "in multibyte locales - "
                 "use mbssep if you care about internationalization");
# endif
#elif defined GNULIB_POSIXCHECK
# undef strsep
# if HAVE_RAW_DECL_STRSEP
_GL_WARN_ON_USE (strsep, "strsep is unportable - "
                 "use gnulib module strsep for portability");
# endif
#endif

#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define strstr rpl_strstr
#  endif
_GL_FUNCDECL_RPL (strstr, char *, (const char *haystack, const char *needle)
                                  __attribute__ ((__pure__))
                                  _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (strstr, char *, (const char *haystack, const char *needle));
# else
  /* On some systems, this function is defined as an overloaded function:
       extern "C++" { const char * strstr (const char *, const char *); }
       extern "C++" { char * strstr (char *, const char *); }  */
_GL_CXXALIAS_SYS_CAST2 (strstr,
                        char *, (const char *haystack, const char *needle),
                        const char *, (const char *haystack, const char *needle));
# endif
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (strstr, char *, (char *haystack, const char *needle));
_GL_CXXALIASWARN1 (strstr, const char *,
                   (const char *haystack, const char *needle));
# else
_GL_CXXALIASWARN (strstr);
# endif
#elif defined GNULIB_POSIXCHECK
/* strstr() does not work with multibyte strings if the locale encoding is
   different from UTF-8:
   POSIX says that it operates on "strings", and "string" in POSIX is defined
   as a sequence of bytes, not of characters.  */
# undef strstr
/* Assume strstr is always declared.  */
_GL_WARN_ON_USE (strstr, "strstr is quadratic on many systems, and cannot "
                 "work correctly on character strings in most "
                 "multibyte locales - "
                 "use mbsstr if you care about internationalization, "
                 "or use strstr if you care about speed");
#endif

/* Find the first occurrence of NEEDLE in HAYSTACK, using case-insensitive
   comparison.  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define strcasestr rpl_strcasestr
#  endif
_GL_FUNCDECL_RPL (strcasestr, char *,
                  (const char *haystack, const char *needle)
                  __attribute__ ((__pure__)) _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (strcasestr, char *,
                  (const char *haystack, const char *needle));
# else
#  if ! 1
_GL_FUNCDECL_SYS (strcasestr, char *,
                  (const char *haystack, const char *needle)
                  __attribute__ ((__pure__)) _GL_ARG_NONNULL ((1, 2)));
#  endif
  /* On some systems, this function is defined as an overloaded function:
       extern "C++" { const char * strcasestr (const char *, const char *); }
       extern "C++" { char * strcasestr (char *, const char *); }  */
_GL_CXXALIAS_SYS_CAST2 (strcasestr,
                        char *, (const char *haystack, const char *needle),
                        const char *, (const char *haystack, const char *needle));
# endif
# if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 10 \
     && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))
_GL_CXXALIASWARN1 (strcasestr, char *, (char *haystack, const char *needle));
_GL_CXXALIASWARN1 (strcasestr, const char *,
                   (const char *haystack, const char *needle));
# else
_GL_CXXALIASWARN (strcasestr);
# endif
#elif defined GNULIB_POSIXCHECK
/* strcasestr() does not work with multibyte strings:
   It is a glibc extension, and glibc implements it only for unibyte
   locales.  */
# undef strcasestr
# if HAVE_RAW_DECL_STRCASESTR
_GL_WARN_ON_USE (strcasestr, "strcasestr does work correctly on character "
                 "strings in multibyte locales - "
                 "use mbscasestr if you care about "
                 "internationalization, or use c-strcasestr if you want "
                 "a locale independent function");
# endif
#endif

/* Parse S into tokens separated by characters in DELIM.
   If S is NULL, the saved pointer in SAVE_PTR is used as
   the next starting point.  For example:
        char s[] = "-abc-=-def";
        char *sp;
        x = strtok_r(s, "-", &sp);      // x = "abc", sp = "=-def"
        x = strtok_r(NULL, "-=", &sp);  // x = "def", sp = NULL
        x = strtok_r(NULL, "=", &sp);   // x = NULL
                // s = "abc\0-def\0"

   This is a variant of strtok() that is multithread-safe.

   For the POSIX documentation for this function, see:
   http://www.opengroup.org/susv3xsh/strtok.html

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.
   Caveat: It doesn't work with multibyte strings unless all of the delimiter
           characters are ASCII characters < 0x30.

   See also strsep().  */
#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef strtok_r
#   define strtok_r rpl_strtok_r
#  endif
_GL_FUNCDECL_RPL (strtok_r, char *,
                  (char *restrict s, char const *restrict delim,
                   char **restrict save_ptr)
                  _GL_ARG_NONNULL ((2, 3)));
_GL_CXXALIAS_RPL (strtok_r, char *,
                  (char *restrict s, char const *restrict delim,
                   char **restrict save_ptr));
# else
#  if 0 || defined GNULIB_POSIXCHECK
#   undef strtok_r
#  endif
#  if ! 1
_GL_FUNCDECL_SYS (strtok_r, char *,
                  (char *restrict s, char const *restrict delim,
                   char **restrict save_ptr)
                  _GL_ARG_NONNULL ((2, 3)));
#  endif
_GL_CXXALIAS_SYS (strtok_r, char *,
                  (char *restrict s, char const *restrict delim,
                   char **restrict save_ptr));
# endif
_GL_CXXALIASWARN (strtok_r);
# if defined GNULIB_POSIXCHECK
_GL_WARN_ON_USE (strtok_r, "strtok_r cannot work correctly on character "
                 "strings in multibyte locales - "
                 "use mbstok_r if you care about internationalization");
# endif
#elif defined GNULIB_POSIXCHECK
# undef strtok_r
# if HAVE_RAW_DECL_STRTOK_R
_GL_WARN_ON_USE (strtok_r, "strtok_r is unportable - "
                 "use gnulib module strtok_r for portability");
# endif
#endif


/* The following functions are not specified by POSIX.  They are gnulib
   extensions.  */

#if 0
/* Return the number of multibyte characters in the character string STRING.
   This considers multibyte characters, unlike strlen, which counts bytes.  */
# ifdef __MirBSD__  /* MirBSD defines mbslen as a macro.  Override it.  */
#  undef mbslen
# endif
# if 0  /* AIX, OSF/1, MirBSD define mbslen already in libc.  */
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define mbslen rpl_mbslen
#  endif
_GL_FUNCDECL_RPL (mbslen, size_t, (const char *string) _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (mbslen, size_t, (const char *string));
# else
_GL_FUNCDECL_SYS (mbslen, size_t, (const char *string) _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_SYS (mbslen, size_t, (const char *string));
# endif
_GL_CXXALIASWARN (mbslen);
#endif

#if 0
/* Return the number of multibyte characters in the character string starting
   at STRING and ending at STRING + LEN.  */
_GL_EXTERN_C size_t mbsnlen (const char *string, size_t len)
     _GL_ARG_NONNULL ((1));
#endif

#if 0
/* Locate the first single-byte character C in the character string STRING,
   and return a pointer to it.  Return NULL if C is not found in STRING.
   Unlike strchr(), this function works correctly in multibyte locales with
   encodings such as GB18030.  */
# if defined __hpux
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define mbschr rpl_mbschr /* avoid collision with HP-UX function */
#  endif
_GL_FUNCDECL_RPL (mbschr, char *, (const char *string, int c)
                                  _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (mbschr, char *, (const char *string, int c));
# else
_GL_FUNCDECL_SYS (mbschr, char *, (const char *string, int c)
                                  _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_SYS (mbschr, char *, (const char *string, int c));
# endif
_GL_CXXALIASWARN (mbschr);
#endif

#if 0
/* Locate the last single-byte character C in the character string STRING,
   and return a pointer to it.  Return NULL if C is not found in STRING.
   Unlike strrchr(), this function works correctly in multibyte locales with
   encodings such as GB18030.  */
# if defined __hpux
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define mbsrchr rpl_mbsrchr /* avoid collision with HP-UX function */
#  endif
_GL_FUNCDECL_RPL (mbsrchr, char *, (const char *string, int c)
                                   _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_RPL (mbsrchr, char *, (const char *string, int c));
# else
_GL_FUNCDECL_SYS (mbsrchr, char *, (const char *string, int c)
                                   _GL_ARG_NONNULL ((1)));
_GL_CXXALIAS_SYS (mbsrchr, char *, (const char *string, int c));
# endif
_GL_CXXALIASWARN (mbsrchr);
#endif

#if 0
/* Find the first occurrence of the character string NEEDLE in the character
   string HAYSTACK.  Return NULL if NEEDLE is not found in HAYSTACK.
   Unlike strstr(), this function works correctly in multibyte locales with
   encodings different from UTF-8.  */
_GL_EXTERN_C char * mbsstr (const char *haystack, const char *needle)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Compare the character strings S1 and S2, ignoring case, returning less than,
   equal to or greater than zero if S1 is lexicographically less than, equal to
   or greater than S2.
   Note: This function may, in multibyte locales, return 0 for strings of
   different lengths!
   Unlike strcasecmp(), this function works correctly in multibyte locales.  */
_GL_EXTERN_C int mbscasecmp (const char *s1, const char *s2)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Compare the initial segment of the character string S1 consisting of at most
   N characters with the initial segment of the character string S2 consisting
   of at most N characters, ignoring case, returning less than, equal to or
   greater than zero if the initial segment of S1 is lexicographically less
   than, equal to or greater than the initial segment of S2.
   Note: This function may, in multibyte locales, return 0 for initial segments
   of different lengths!
   Unlike strncasecmp(), this function works correctly in multibyte locales.
   But beware that N is not a byte count but a character count!  */
_GL_EXTERN_C int mbsncasecmp (const char *s1, const char *s2, size_t n)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Compare the initial segment of the character string STRING consisting of
   at most mbslen (PREFIX) characters with the character string PREFIX,
   ignoring case, returning less than, equal to or greater than zero if this
   initial segment is lexicographically less than, equal to or greater than
   PREFIX.
   Note: This function may, in multibyte locales, return 0 if STRING is of
   smaller length than PREFIX!
   Unlike strncasecmp(), this function works correctly in multibyte
   locales.  */
_GL_EXTERN_C char * mbspcasecmp (const char *string, const char *prefix)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Find the first occurrence of the character string NEEDLE in the character
   string HAYSTACK, using case-insensitive comparison.
   Note: This function may, in multibyte locales, return success even if
   strlen (haystack) < strlen (needle) !
   Unlike strcasestr(), this function works correctly in multibyte locales.  */
_GL_EXTERN_C char * mbscasestr (const char *haystack, const char *needle)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Find the first occurrence in the character string STRING of any character
   in the character string ACCEPT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.
   Unlike strcspn(), this function works correctly in multibyte locales.  */
_GL_EXTERN_C size_t mbscspn (const char *string, const char *accept)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Find the first occurrence in the character string STRING of any character
   in the character string ACCEPT.  Return the pointer to it, or NULL if none
   exists.
   Unlike strpbrk(), this function works correctly in multibyte locales.  */
# if defined __hpux
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define mbspbrk rpl_mbspbrk /* avoid collision with HP-UX function */
#  endif
_GL_FUNCDECL_RPL (mbspbrk, char *, (const char *string, const char *accept)
                                   _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_RPL (mbspbrk, char *, (const char *string, const char *accept));
# else
_GL_FUNCDECL_SYS (mbspbrk, char *, (const char *string, const char *accept)
                                   _GL_ARG_NONNULL ((1, 2)));
_GL_CXXALIAS_SYS (mbspbrk, char *, (const char *string, const char *accept));
# endif
_GL_CXXALIASWARN (mbspbrk);
#endif

#if 0
/* Find the first occurrence in the character string STRING of any character
   not in the character string REJECT.  Return the number of bytes from the
   beginning of the string to this occurrence, or to the end of the string
   if none exists.
   Unlike strspn(), this function works correctly in multibyte locales.  */
_GL_EXTERN_C size_t mbsspn (const char *string, const char *reject)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Search the next delimiter (multibyte character listed in the character
   string DELIM) starting at the character string *STRINGP.
   If one is found, overwrite it with a NUL, and advance *STRINGP to point
   to the next multibyte character after it.  Otherwise, set *STRINGP to NULL.
   If *STRINGP was already NULL, nothing happens.
   Return the old value of *STRINGP.

   This is a variant of mbstok_r() that supports empty fields.

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.

   See also mbstok_r().  */
_GL_EXTERN_C char * mbssep (char **stringp, const char *delim)
     _GL_ARG_NONNULL ((1, 2));
#endif

#if 0
/* Parse the character string STRING into tokens separated by characters in
   the character string DELIM.
   If STRING is NULL, the saved pointer in SAVE_PTR is used as
   the next starting point.  For example:
        char s[] = "-abc-=-def";
        char *sp;
        x = mbstok_r(s, "-", &sp);      // x = "abc", sp = "=-def"
        x = mbstok_r(NULL, "-=", &sp);  // x = "def", sp = NULL
        x = mbstok_r(NULL, "=", &sp);   // x = NULL
                // s = "abc\0-def\0"

   Caveat: It modifies the original string.
   Caveat: These functions cannot be used on constant strings.
   Caveat: The identity of the delimiting character is lost.

   See also mbssep().  */
_GL_EXTERN_C char * mbstok_r (char *string, const char *delim, char **save_ptr)
     _GL_ARG_NONNULL ((2, 3));
#endif

/* Map any int, typically from errno, into an error message.  */
#if 1
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   undef strerror
#   define strerror rpl_strerror
#  endif
_GL_FUNCDECL_RPL (strerror, char *, (int));
_GL_CXXALIAS_RPL (strerror, char *, (int));
# else
_GL_CXXALIAS_SYS (strerror, char *, (int));
# endif
_GL_CXXALIASWARN (strerror);
#elif defined GNULIB_POSIXCHECK
# undef strerror
/* Assume strerror is always declared.  */
_GL_WARN_ON_USE (strerror, "strerror is unportable - "
                 "use gnulib module strerror to guarantee non-NULL result");
#endif

#if 0
# if 0
#  if !(defined __cplusplus && defined GNULIB_NAMESPACE)
#   define strsignal rpl_strsignal
#  endif
_GL_FUNCDECL_RPL (strsignal, char *, (int __sig));
_GL_CXXALIAS_RPL (strsignal, char *, (int __sig));
# else
#  if ! 1
_GL_FUNCDECL_SYS (strsignal, char *, (int __sig));
#  endif
/* Need to cast, because on Cygwin 1.5.x systems, the return type is
   'const char *'.  */
_GL_CXXALIAS_SYS_CAST (strsignal, char *, (int __sig));
# endif
_GL_CXXALIASWARN (strsignal);
#elif defined GNULIB_POSIXCHECK
# undef strsignal
# if HAVE_RAW_DECL_STRSIGNAL
_GL_WARN_ON_USE (strsignal, "strsignal is unportable - "
                 "use gnulib module strsignal for portability");
# endif
#endif

#if 1
# if !1
_GL_FUNCDECL_SYS (strverscmp, int, (const char *, const char *)
                                   _GL_ARG_NONNULL ((1, 2)));
# endif
_GL_CXXALIAS_SYS (strverscmp, int, (const char *, const char *));
_GL_CXXALIASWARN (strverscmp);
#elif defined GNULIB_POSIXCHECK
# undef strverscmp
# if HAVE_RAW_DECL_STRVERSCMP
_GL_WARN_ON_USE (strverscmp, "strverscmp is unportable - "
                 "use gnulib module strverscmp for portability");
# endif
#endif


#endif /* _GL_STRING_H */
#endif /* _GL_STRING_H */
