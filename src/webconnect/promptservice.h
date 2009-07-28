///////////////////////////////////////////////////////////////////////////////
// Name:        promptservice.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-07
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_PROMPTSERVICE_H
#define __WXWEBCONNECT_PROMPTSERVICE_H


#define NS_APPSHELL_CID \
 {0x2d96b3df, 0xc051, 0x11d1, {0xa8, 0x27, 0x00, 0x40, 0x95, 0x9a, 0x28, 0xc9}}
    
#define NS_PROMPTSERVICE_CID \
 {0xa2112d6a, 0x0e28, 0x421f, {0xb4, 0x6a, 0x25, 0xc0, 0xb3, 0x08, 0xcb, 0xd0}}
 
#define NS_DOWNLOAD_CID \
 {0xe3fa9d0a, 0x1dd1, 0x11b2, {0xbd, 0xef, 0x8c, 0x72, 0x0b, 0x59, 0x74, 0x45}}
            
#define NS_UNKNOWNCONTENTTYPEHANDLER_CID \
 {0x42770b50, 0x03e9, 0x11d3, {0x80, 0x68, 0x00, 0x60, 0x08, 0x11, 0xa9, 0xc3}}
 
#define NS_NSSDIALOGS_CID \
 {0x518e071f, 0x1dd2, 0x11b2, {0x93, 0x7e, 0xc4, 0x5f, 0x14, 0xde, 0xf7, 0x78}}


void CreatePromptServiceFactory(nsIFactory** result);
void CreateTransferFactory(nsIFactory** result);
void CreateUnknownContentTypeHandlerFactory(nsIFactory** result);


#endif

