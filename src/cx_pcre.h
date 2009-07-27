#ifndef _CX_PCRE_H
#define _CX_PCRE_H

/* e text editor: use smart pointer */
#include "doc_byte_iter.h"
#define CUSTOM_SUBJECT_PTR doc_byte_iter

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "cx_pcre_internal.h"

int cx_pcre_exec(const pcre *, const pcre_extra *, PCRE_SPTR,
                   int, int, int, int *, int);

#endif // _CX_PCRE_H
