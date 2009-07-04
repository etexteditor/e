#include "Env.h"

#include "eDocumentPath.h"

#include <wx/filename.h>
#include <wx/file.h>
#include <wx/utils.h>

void cxEnv::SetEnv(const wxString& key, const wxString& value) {
	wxASSERT(!key.empty());
	m_env[key] = value;
}

void cxEnv::SetIfValue(const wxString& key,  const wxString& value) {
	if (!value.IsEmpty()) SetEnv(key,value);
}

void cxEnv::SetEnv(const std::map<wxString, wxString>& env) {
	for (std::map<wxString, wxString>::const_iterator r = env.begin(); r != env.end(); ++r) {
		m_env.insert(*r);
	}
}

void cxEnv::SetToCurrent() {
#ifdef __WXMSW__
	LPWCH envStr = ::GetEnvironmentStrings();
	if (!envStr) return;
	const wxChar* env = envStr;

	while (*env != wxT('\0')) {
		const wxString envStr(env);
		if (envStr.StartsWith(wxT("=C:"))) { // special case that holds cwd for C:
			const wxString value = envStr.substr(4);
			m_env[wxT("=C:")] = value;
		}
		else {
			const int equalNdx = envStr.Find(wxT('='));
			if (equalNdx != -1) {
				// NOTE: cygwin uses all env keys in uppercase, so we
				// convert here. But if used native we might want to avoid this
				wxString key = envStr.substr(0, equalNdx);
				key.MakeUpper();
				const wxString value = envStr.substr(equalNdx+1);
				m_env[key] = value;
			}
		}

		env += envStr.size()+1;
	}

	::FreeEnvironmentStrings(envStr);
#else
	char **env = environ;
	while(*env != NULL)
	{
		const wxString envStr(*env, wxConvUTF8);
		const int equalNdx = envStr.Find(wxT('='));
		if (equalNdx != -1) {
			m_env[envStr.substr(0, equalNdx)] = envStr.substr(equalNdx+1);
		}
		env++;
	}
#endif //__WXMSW__
}

bool cxEnv::GetEnv(const wxString& key, wxString& value) {
	std::map<wxString, wxString>::const_iterator p = m_env.find(key);

	if (p == m_env.end()) return false;

	value = p->second;
	return true;
}

const char* cxEnv::GetEnvBlock() const {
	// Add all lines together to a single string
	m_envStr.clear();
	m_envStr.reserve(m_env.size() * 40);
	for (std::map<wxString, wxString>::const_iterator p = m_env.begin(); p != m_env.end(); ++p) {
		wxString line = p->first;
		line += wxT('=');
		line += p->second;

		//wxLogDebug(wxT("%d: %s"), i, line);

		// Convert
		const wxCharBuffer buf = line.mb_str(wxConvUTF8);
		const char* str = buf.data();

		m_envStr.insert(m_envStr.end(), str, str+strlen(str));
		m_envStr.push_back('\0');
	}

	// Add end marker
	m_envStr.push_back('\0');

	return &*m_envStr.begin();
}

void cxEnv::GetEnvBlock(wxString& env) const {
	for (std::map<wxString, wxString>::const_iterator p = m_env.begin(); p != m_env.end(); ++p) {
		wxString line = p->first;
		line += wxT('=');
		line += p->second;

		//wxLogDebug(wxT("%d: %s"), i, line);

#ifdef __WXMSW__
		// Convert to utf8
		const wxCharBuffer buf = line.mb_str(wxConvUTF8);

		// to avoid unicode chars being mangled by windows automatic
		// conversion to oem, we first convert the utf8 text to unicode
		// as if it was oem. Windows will convert it back to oem, which
		// will then preserve the utf8 encoding.
		// We use native MultiByteToWideChar to ensure we get the right
		// codepage no matter the current locale.
		const size_t len = ::MultiByteToWideChar(CP_OEMCP, 0, buf.data(), -1, NULL, 0);
		if (len) {
			wxWCharBuffer wBuf(len);
			::MultiByteToWideChar(CP_OEMCP, 0, buf.data(), -1, wBuf.data(), len);
			env += wBuf;
		}
		else wxASSERT(false);

		//const wxString oemEnv(buf, wxCSConv(wxFONTENCODING_CP437));

#else
		env += line;
#endif

		env += wxT('\0');
	}

	// Add end marker
	env += wxT('\0');
}

void cxEnv::WriteEnvToFile(wxFile& file) const {
	wxASSERT(file.IsOpened());

	for (std::map<wxString, wxString>::const_iterator p = m_env.begin(); p != m_env.end(); ++p) {
		wxString line = p->first;
		line += wxT('=');
		line += p->second;
		line += wxT('\n');

		file.Write(line);
	}

	file.Write(wxT("\n"));
}

void cxEnv::AddSystemVars(const bool isUnix, const wxString &baseAppPath) {
	// TM_SUPPORT_PATH
	wxFileName supportPath = baseAppPath;
	supportPath.AppendDir(wxT("Support"));
	const bool supportPathExists = supportPath.DirExists();
	if (supportPathExists) {
		const wxString tmSupportPath = isUnix ? eDocumentPath::WinPathToCygwin(supportPath) : supportPath.GetPath();
		this->SetEnv(wxT("TM_SUPPORT_PATH"), tmSupportPath);

		// TM_BASH_INIT
		if (isUnix) {
			wxFileName bashInit = supportPath;
			bashInit.AppendDir(wxT("lib"));
			bashInit.SetFullName(wxT("cygwin_bash_init.sh"));
			if (bashInit.FileExists()) {
				wxString s_tmBashInit = eDocumentPath::WinPathToCygwin(bashInit);
				this->SetEnv(wxT("TM_BASH_INIT"), s_tmBashInit);
			}
		}
	}

	// PATH
	wxString envPath;
	if (wxGetEnv(wxT("PATH"), &envPath)) {
#ifdef __WXMSW__
		// Check if cygwin is on the path
		if (!eDocumentPath::CygwinPath().empty()) {
			if (!envPath.Contains(eDocumentPath::CygwinPath())) {
				const wxString binPath = eDocumentPath::CygwinPath() + wxT("\\bin");
				const wxString x11Path = eDocumentPath::CygwinPath() + wxT("\\usr\\X11R6\\bin");

				if (!envPath.empty()) {
					envPath = binPath + wxT(";") + x11Path + wxT(";") + envPath;
				}
			}
		}
#endif // __WXMSW__

		// Add TM_SUPPORT_PATH/bin to the PATH
		if (supportPathExists) {
			wxFileName supportBinPath = supportPath;
			supportBinPath.AppendDir(wxT("bin"));
			if (supportBinPath.DirExists()) {
				envPath = supportBinPath.GetPath() + wxT(";") + envPath;
			}
		}

		this->SetEnv(wxT("PATH"), envPath);
	}

	// TM_APPPATH
	wxString appPath = baseAppPath;
	if (isUnix) appPath = eDocumentPath::WinPathToCygwin(appPath);
	this->SetEnv(wxT("TM_APPPATH"), appPath);

	// TM_FULLNAME
	this->SetEnv(wxT("TM_FULLNAME"), wxGetUserName());
}
