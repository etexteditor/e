////////////////////////////////////////////////////////////////////////////////////////////////
//
// stdint.h - Based on ISO C99 7.18 and ISO/IEC SC22/WG14 9899 draft (SC22 N2794). 
//
// Copyright (c) 1995-2002 - NetWand Solutions, Inc.  All Rights Reserved.
//
#ifndef  _INC_STDINT
#define  _INC_STDINT

#if !defined(WIN32) && !defined(_WIN64)
#error   Only Win32 and Win64 targets are supported.
#endif

//
// Compiler specific 64-bit type:
//
#ifndef  quad
#define  quad  __int64
#endif

//
// 7.18.1.1 - Exact width integer types:
//
typedef  signed   char  int8_t;
typedef  signed   short int16_t;
typedef  signed   int   int32_t;
typedef  signed   quad  int64_t;

typedef  unsigned char  uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int   uint32_t;
typedef  unsigned quad  uint64_t;

//
// 7.18.1.2 - Minimum width integer types:
//
typedef  signed   char  int_least8_t;
typedef  signed   short int_least16_t;
typedef  signed   int   int_least32_t;
typedef  signed   quad  int_least64_t;

typedef  unsigned char  uint_least8_t;
typedef  unsigned short uint_least16_t;
typedef  unsigned int   uint_least32_t;
typedef  unsigned quad  uint_least64_t;

//
// 7.18.1.3 - Fastest minimum width integer types:
//
typedef  signed   char  int_fast8_t;
typedef  signed   short int_fast16_t;
typedef  signed   int   int_fast32_t;
typedef  signed   quad  int_fast64_t;

typedef  unsigned char  uint_fast8_t;
typedef  unsigned short uint_fast16_t;
typedef  unsigned int   uint_fast32_t;
typedef  unsigned quad  uint_fast64_t;

//
// 7.18.1.4 - Object pointer containers (offsets):
//
#if defined(_WIN64)
typedef  signed   quad  intptr_t;
typedef  unsigned quad  uintptr_t;
#else
typedef  signed   int   intptr_t;
typedef  unsigned int   uintptr_t;
#endif

//
// 7.18.1.5 - Largest width integer types:
//
typedef  signed   quad  intmax_t;
typedef  unsigned quad  uintmax_t;

//
// 7.18.2 - Specific type limits (use climits for C++):
//
#if !defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

//
// 7.18.2.1 - Exact width integer type limits:
//
#define  INT8_MIN           (-128) 
#define  INT16_MIN          (-32768)
#define  INT32_MIN          (-2147483647L - 1)
#define  INT64_MIN          (-9223372036854775807i64 - 1)

#define  INT8_MAX           (127)
#define  INT16_MAX          (32767)
#define  INT32_MAX          (2147483647L)
#define  INT64_MAX          (9223372036854775807i64)

#define  UINT8_MAX          (255)
#define  UINT16_MAX         (65535)
#define  UINT32_MAX         (4294967295U)
#define  UINT64_MAX         (18446744073709551615ui64)

//
// 7.18.2.2 - Minimum width integer type limits:
//
#define  INT_LEAST8_MIN     INT8_MIN
#define  INT_LEAST16_MIN    INT16_MIN
#define  INT_LEAST32_MIN    INT32_MIN
#define  INT_LEAST64_MIN    INT64_MIN

#define  INT_LEAST8_MAX     INT8_MAX
#define  INT_LEAST16_MAX    INT16_MAX
#define  INT_LEAST32_MAX    INT32_MAX
#define  INT_LEAST64_MAX    INT64_MAX

#define  UINT_LEAST8_MAX    UINT8_MAX
#define  UINT_LEAST16_MAX   UINT16_MAX
#define  UINT_LEAST32_MAX   UINT32_MAX
#define  UINT_LEAST64_MAX   UINT64_MAX

//
// 7.18.2.3 - Fastest minimum width integer type limits:
//
#define  INT_FAST8_MIN      INT8_MIN
#define  INT_FAST16_MIN     INT16_MIN
#define  INT_FAST32_MIN     INT32_MIN
#define  INT_FAST64_MIN     INT64_MIN

#define  INT_FAST8_MAX      INT8_MAX
#define  INT_FAST16_MAX     INT16_MAX
#define  INT_FAST32_MAX     INT32_MAX
#define  INT_FAST64_MAX     INT64_MAX

#define  UINT_FAST8_MAX     UINT8_MAX
#define  UINT_FAST16_MAX    UINT16_MAX
#define  UINT_FAST32_MAX    UINT32_MAX
#define  UINT_FAST64_MAX    UINT64_MAX

// 
// 7.18.2.4 - Object pointer container type limits:
//
#if defined(_WIN64)
#define  INTPTR_MIN         (-9223372036854775807i64 - 1)
#define  INTPTR_MAX         (9223372036854775807i64)
#define  UINTPTR_MAX        (18446744073709551615ui64)
#else
#define  INTPTR_MIN         (-2147483647L - 1)
#define  INTPTR_MAX         (2147483647L)
#define  UINTPTR_MAX        (4294967295U)
#endif

//
// 7.18.2.5 - Largest width integer type limits:
//
#define  INTMAX_MIN         INT64_MIN
#define  INTMAX_MAX         INT64_MAX
#define  UINTMAX_MAX        UINT64_MAX

//
// 7.18.3 - Other integer type limits:
//
#define  PTRDIFF_MIN        INTPTR_MIN
#define  PTRDIFF_MAX        INTPTR_MAX
#define  SSIZE_MIN          INTPTR_MIN
#define  SSIZE_MAX          INTPTR_MAX
#define  SIZE_MAX           UINTPTR_MAX
#define  SIG_ATOMIC_MIN     INTPTR_MIN
#define  SIG_ATOMIC_MAX     INTPTR_MAX

#ifndef  WCHAR_MIN
#define  WCHAR_MIN          0
#endif
#ifndef  WCHAR_MAX
#define  WCHAR_MAX          UINT16_MAX
#endif

#ifndef  WINT_MIN
#define  WINT_MIN           0
#endif
#ifndef  WINT_MAX
#define  WINT_MAX           UINT32_MAX
#endif

#endif//!defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

//
// 7.18.4 - Macros for integer type constants:
//
#if !defined(__cplusplus) || defined (__STDC_CONSTANT_MACROS)

// 
// 7.18.4.1 - Macros for minimum width integer type constants:
//
#define  INT8_C(expr)   (INT_LEAST8_MAX  - INT_LEAST8_MAX  + (expr))
#define  INT16_C(expr)  (INT_LEAST16_MAX - INT_LEAST16_MAX + (expr))
#define  INT32_C(expr)  (INT_LEAST32_MAX - INT_LEAST32_MAX + (expr))
#define  INT64_C(expr)  (INT_LEAST64_MAX - INT_LEAST64_MAX + (expr))

#define  UINT8_C(expr)  (UINT_LEAST8_MAX  - UINT_LEAST8_MAX  + (expr))
#define  UINT16_C(expr) (UINT_LEAST16_MAX - UINT_LEAST16_MAX + (expr))
#define  UINT32_C(expr) (UINT_LEAST32_MAX - UINT_LEAST32_MAX + (expr))
#define  UINT64_C(expr) (UINT_LEAST64_MAX - UINT_LEAST64_MAX + (expr))

//
// 7.18.4.2 - Macros for largest width integer type constants:
//
#define  INTMAX_C(expr)  (INTMAX_MAX  - INTMAX_MAX  + (expr))
#define  UINTMAX_C(expr) (UINTMAX_MAX - UINTMAX_MAX + (expr))

#endif//!defined(__cplusplus) || defined(__STDC_CONSTANT_MACROS)

//
// Unsigned integer aliases:
//
typedef  unsigned char  uchar;
typedef  unsigned short ushort;
typedef  unsigned int   uint;
typedef  unsigned long  ulong;
typedef  unsigned quad  uquad;

//
// Exact width integer aliases:
//
typedef  unsigned char  byte;
typedef  unsigned short word;
typedef  unsigned long  dword;
typedef  unsigned quad  qword;

//
// Array indexing and size integer types:
//
#if defined(_WIN64)
typedef  signed   quad  int_t;
typedef  unsigned quad  uint_t;
typedef  signed   quad  ssize_t;
typedef  unsigned quad  size_t;
#else
typedef  signed   int   int_t;
typedef  unsigned int   uint_t;
typedef  signed   int   ssize_t;
typedef  unsigned int   size_t;
#endif

//
// Array indexing and size integer type limits:
//
#define  INT_T_MIN      INTPTR_MIN
#define  INT_T_MAX      INTPTR_MAX
#define  UINT_T_MAX     UINTPTR_MAX

//
// Character model specific character types:
//
#if defined(_UNICODE)
typedef  wchar_t        char_t;
#else
typedef  char           char_t;
#endif

//
// Character model specific character type limits:
//
#if defined(_UNICODE)
#define  CHAR_T_MIN     UINT16_MIN
#define  CHAR_T_MAX     UINT16_MAX
#elif defined(_CHAR_UNSIGNED)
#define  CHAR_T_MIN     UINT8_MIN
#define  CHAR_T_MAX     UINT8_MAX
#else
#define  CHAR_T_MIN     INT8_MIN
#define  CHAR_T_MAX     INT8_MAX
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
//
// EOF
//
#endif//!_INC_STDINT


