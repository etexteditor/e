#include "eApp.h"

#ifdef __WXMSW__
#include <wx/msw/registry.h>
#include "ExceptionHandler.h"

// We need our own WinMain to implement crash handling
extern "C" int WINAPI WinMain(HINSTANCE hInstance,
							HINSTANCE hPrevInstance,
							wxCmdLineArgType lpCmdLine,
							int nCmdShow)
{
	int nResult = -1;
    __try
    {

		  nResult = wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
    __except(RecordExceptionInfo(GetExceptionInformation(), "WinMain"))
    {
        // Do nothing here - RecordExceptionInfo() has already done
        // everything that is needed. Actually this code won't even
        // get called unless you return EXCEPTION_EXECUTE_HANDLER from
        // the __except clause.
    }
    return nResult;
}

IMPLEMENT_APP_NO_MAIN(eApp)
#else
IMPLEMENT_APP(eApp)
#endif