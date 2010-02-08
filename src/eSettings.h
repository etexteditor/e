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
#include "ISettings.h"

class eApp;
class RemoteProfile;

enum SubPage {SP_MAIN=0, SP_LEFT, SP_RIGHT};

class eFrameSettings {
public:
	eFrameSettings(wxJSONValue& framesettings);

	void AutoSave();

	// Get setting values
	bool GetSettingBool(const wxString& name, bool& value) const;
	bool GetSettingInt(const wxString& name, int& value) const;
	bool GetSettingLong(const wxString& name, wxLongLong& value) const;
	bool GetSettingString(const wxString& name, wxString& value) const;

	// Store setting values
	void SetSettingBool(const wxString& name, bool value);
	void SetSettingInt(const wxString& name, int value);
	void SetSettingString(const wxString& name, const wxString& value);
	void RemoveSetting(const wxString& name);

	// Pages
	size_t GetPageCount() const;
	void SetPageSettings(size_t page_id, const wxString& path, doc_id di, int pos, int topline, const wxString& syntax, const vector<unsigned int>& folds, const vector<cxBookmark>& bookmarks, SubPage sp=SP_MAIN);
	void GetPageSettings(size_t page_id, wxString& path, doc_id& di, int& pos, int& topline, wxString& syntax, vector<unsigned int>& folds, vector<unsigned int>& bookmarks, SubPage sp=SP_MAIN) const;
	bool IsPageDiff(size_t page_id) const;
	wxString GetPagePath(size_t page_id, SubPage sp=SP_MAIN) const;
	doc_id GetPageDoc(size_t page_id, SubPage sp=SP_MAIN) const;
	void DeletePageSettings(size_t page_id);
	void DeleteAllPageSettings();

	wxJSONRefData* GetRefData() const {return m_jsonRoot.GetRefData();};

private:
	wxJSONValue& m_jsonRoot;
};

class eSettings: public ISettings {
public:
	eSettings();
	
	void Load(const wxString& path);
	bool Save();
	bool IsEmpty() const;

	// Get setting values
	virtual bool GetSettingBool(const wxString& name, bool& value) const;
	virtual bool GetSettingInt(const wxString& name, int& value) const;
	virtual bool GetSettingLong(const wxString& name, wxLongLong& value) const;
	virtual bool GetSettingString(const wxString& name, wxString& value) const;

	// Store setting values
	void SetSettingBool(const wxString& name, bool value);
	void SetSettingInt(const wxString& name, int value);
	void SetSettingLong(const wxString& name, const wxLongLong& value);
	void SetSettingString(const wxString& name, const wxString& value);

	// Clear setting value
	void RemoveSetting(const wxString& name);

	// Frames
	unsigned int GetFrameCount() const;
	eFrameSettings GetFrameSettings(unsigned int frameId);
	unsigned int AddFrame(unsigned int top);
	void RemoveFrame(unsigned int frameId);
	void RemoveFrame(const eFrameSettings& fs);
	void DeleteAllFrameSettings(int top);
	int GetIndexFromFrameSettings(const eFrameSettings& fs) const;

	// Recent files
	void AddRecentFile(const wxString& path);
	void AddRecentProject(const wxString& path);
	void GetRecentFiles(wxArrayString& recentfiles) const;
	void GetRecentProjects(wxArrayString& recentprojects) const;

	// Recent Diffs
	void AddRecentDiff(const wxString& path, SubPage sp);
	void GetRecentDiffs(wxArrayString& recentprojectsh, SubPage sp) const;

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

	bool ShouldSave();
	void DontSave();
	void AllowSave();
	void AutoSave();
	void DoAutoSave();
	void SetApp(eApp* app);

private:
	// Recent files (support functions)
	static void AddToRecent(const wxString& key, wxJSONValue& jsonArray, size_t max);
	static void GetRecents(const wxJSONValue& jarray, wxArrayString& recents);

	// Remote profile (support functions)
	RemoteProfile* DoGetRemoteProfile(size_t profile_id);
	void SaveRemoteProfile(RemoteProfile* rp);
	static wxString StripSlashes(const wxString& path);

	wxString m_path;
	wxJSONValue m_jsonRoot;
	auto_vector<RemoteProfile> m_tempRemotes; // cache for remote profiles

	eApp* m_app;
	bool haveApp;
	bool needSave;
	int m_blockCount;
};

eSettings& eGetSettings(void);

#endif //__ESETTINGS_H__