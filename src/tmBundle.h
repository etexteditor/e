#ifndef __TMBUNDLE_H__
#define __TMBUNDLE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

#include <wx/filename.h>

struct tmBundle {
	unsigned int bundleRef;
	wxString name;
	wxString uuid;
	wxFileName path;
};

#endif // __TMBUNDLE_H__
