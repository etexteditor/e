#ifndef __FILEINFO_H__
#define __FILEINFO_H__

class cxFileInfo {
public:
	cxFileInfo() : m_isDir(false) {};
	cxFileInfo(const cxFileInfo& fi) {
		m_isDir = fi.m_isDir;
		m_name = fi.m_name.c_str();
		m_modDate = fi.m_modDate;
		m_size = fi.m_size;
	};

	bool m_isDir;
	wxString m_name;
	wxDateTime m_modDate;
	size_t m_size;
};

#endif
