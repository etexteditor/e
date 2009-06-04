/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __ESETTINGS_H__
#define __ESETTINGS_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/string.h>
#endif

#include "jsonval.h"
#include "Catalyst.h"
#include "auto_vector.h"

// pre-declarations
class RemoteProfile;

class eSettings {
public:
	eSettings();
	
	void Load(const wxString& path);
	bool Save();
	bool IsEmpty() const;

	// Individual settings
	bool GetSettingBool(const wxString& name, bool& value) const;
	void SetSettingBool(const wxString& name, bool value);
	bool GetSettingInt(const wxString& name, int& value) const;
	void SetSettingInt(const wxString& name, int value);
	bool GetSettingLong(const wxString& name, wxLongLong& value) const;
	void SetSettingLong(const wxString& name, const wxLongLong& value);
	bool GetSettingString(const wxString& name, wxString& value) const;
	void SetSettingString(const wxString& name, const wxString& value);
	void RemoveSetting(const wxString& name);

	// Recent files
	void AddRecentFile(const wxString& path);
	void AddRecentProject(const wxString& path);
	void GetRecentFiles(wxArrayString& recentfiles) const;
	void GetRecentProjects(wxArrayString& recentprojects) const;

	// Pages
	size_t GetPageCount() const;
	void SetPageSettings(size_t page_id, const wxString& path, doc_id di, int pos, int topline, const wxString& syntax, const vector<unsigned int>& folds, const vector<cxBookmark>& bookmarks);
	void GetPageSettings(size_t page_id, wxString& path, doc_id& di, int& pos, int& topline, wxString& syntax, vector<unsigned int>& folds, vector<unsigned int>& bookmarks) const;
	wxString GetPagePath(size_t page_id) const;
	doc_id GetPageDoc(size_t page_id) const;
	void DeletePageSettings(size_t page_id);
	void DeleteAllPageSettings();

	// Remote profiles
	size_t GetRemoteProfileCount() const;
	wxString GetRemoteProfileName(size_t profile_id) const;
	size_t AddRemoteProfile(const RemoteProfile& profile);
	void SetRemoteProfile(size_t profile_id, const RemoteProfile& profile);
	const RemoteProfile* GetRemoteProfile(size_t profile_id);
	const RemoteProfile* GetRemoteProfileFromUrl(const wxString& url, bool withDir);
	void SetRemoteProfileLogin(const RemoteProfile* rp, const wxString& username, const wxString& pwd, bool toDb);
	void DeleteRemoteProfile(size_t profile_id);

	// Search History
	size_t GetSearchCount() const;
	void GetSearch(size_t item, wxString& pattern, bool& isRegex, bool& matchCase) const;
	bool AddSearch(const wxString& pattern, bool isRegex, bool matchCase);

	// Replace History
	size_t GetReplaceCount() const;
	wxString GetReplace(size_t ndx) const;
	bool AddReplace(const wxString& pattern);

	// Filter through command History
	size_t GetFilterCommandHistoryCount() const;
	wxString GetFilterCommand(size_t ndx) const;
	bool AddFilterCommand(const wxString& command);

	// Environmental variables
	map<wxString, wxString> env;

private:
	// Recent files (support functions)
	void AddToRecent(const wxString& key, wxJSONValue& jsonArray, size_t max);
	void GetRecents(const wxJSONValue& jarray, wxArrayString& recents) const;

	// Remote profile (support functions)
	RemoteProfile* DoGetRemoteProfile(size_t profile_id);
	void SaveRemoteProfile(RemoteProfile* rp);
	static wxString StripSlashes(const wxString& path);

	wxString m_path;
	wxJSONValue m_jsonRoot;
	auto_vector<RemoteProfile> m_tempRemotes; // cache for remote profiles
};

class IGetSettings {
public:
	virtual eSettings& GetSettings() = 0;
};

eSettings& eGetSettings(void);

#endif //__ESETTINGS_H__