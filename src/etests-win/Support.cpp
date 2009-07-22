#include "Support.h"
#include <wx/string.h>
#include <wx/filefn.h>

bool RequireEdb(wxString& path) {
	path = wxGetCwd();
	path += wxFILE_SEP_PATH; 
	path += wxT("e.db");

	return wxFileExists(path);
}
