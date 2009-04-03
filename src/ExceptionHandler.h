// ExceptionHandler.h  Version 1.1
//
// Copyright © 1998 Bruce Dawson
//
// Author:       Bruce Dawson
//               brucedawson@cygnus-software.com
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// A paper by the original author can be found at:
//     http://www.cygnus-software.com/papers/release_debugging.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	EXCEPTIONHANDLER_H
#define	EXCEPTIONHANDLER_H

// We forward declare PEXCEPTION_POINTERS so that the function
// prototype doesn't needlessly require windows.h.

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS data, const char *Message);


/*
// Sample usage - put the code that used to be in main into HandledMain.
// To hook it in to an MFC app add ExceptionAttacher.cpp from the mfctest
// application into your project.
int main(int argc, char *argv[])
{
	int Result = -1;
	__try
	{
		Result = HandledMain(argc, argv);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), "main thread"))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return Result;
}
*/

#endif
