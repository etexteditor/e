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

#ifndef __PLISTHANDLER_H__
#define __PLISTHANDLER_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif
#include "mk4.h"
#include <wx/filename.h>

#include "BundleItemType.h"
#include "BundleInfo.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#include <algorithm>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

// Pre-definitions
class TiXmlElement;
class PListDict;
class PListArray;
class wxJSONValue;

class PListHandler : public wxEvtHandler {
public:
	PListHandler(const wxString& appPath, const wxString& appDataPath, bool rebuildDb);
	~PListHandler();

	enum cxUpdateMode {
		UPDATE_FULL,
		UPDATE_SYNTAXONLY,
		UPDATE_REST
	};

	bool AllBundlesUpdated() const {return m_allBundlesUpdated;};
	void Update(cxUpdateMode mode = UPDATE_FULL);
	void Commit();

	class cxItemRef {
	public:
		cxItemRef(wxString n, unsigned int r) : name(n), ref(r) {};
		wxString name;
		unsigned int ref;
	};

	// Theme lists
	void GetThemes(vector<cxItemRef>& themes) const;

	// Themes
	bool IsThemeEditable(unsigned int ndx) const;
	PListDict GetTheme(unsigned int ndx) const;
	bool GetTheme(unsigned int ndx, PListDict& theme) const;
	void GetEditableTheme(unsigned int ndx, PListDict& themeDict);
	int GetThemeFromUuid(const char* key) const;
	void SaveTheme(unsigned int ndx);
	void MarkThemeAsModified(unsigned int ndx);
	void DeleteTheme(unsigned int ndx);
	unsigned int NewTheme(const wxString& name);

	// Bundles (General)
	vector<unsigned int> GetBundles() const;
	vector<cxBundleInfo> GetInstalledBundlesInfo() const;
	PListDict GetBundleInfo(unsigned int ndx) const;
	unsigned int NewBundle(const wxString& name);
	PListDict GetEditableBundleInfo(unsigned int bundleId);
	bool DeleteBundle(unsigned int bundleId);
	bool RestoreBundle(unsigned int bundleId);
	bool SaveBundle(unsigned int bundleId);

	// Paths for bundles
	wxFileName GetBundlePath(unsigned int ndx) const;
	wxFileName GetBundleItemPath(BundleItemType type, unsigned int bundleId, unsigned int itemId) const;
	wxFileName GetBundleSupportPath(unsigned int bundleId) const;
	wxDateTime GetBundleModDate(unsigned int bundleId) const;

	// Exports
	bool ExportBundle(const wxFileName& dstPath, unsigned int bundleId) const;
	bool ExportBundleItem(const wxFileName& dstPath, BundleItemType type, unsigned int bundleId, unsigned int itemId) const;

	// Bundle items
	vector<unsigned int> GetList(BundleItemType type, unsigned int bundleId) const;
	PListDict Get(BundleItemType type, unsigned int bundleId, unsigned int itemId) const;
	PListDict GetEditable(BundleItemType type, unsigned int bundleId, unsigned int itemId);
	unsigned int New(BundleItemType type, unsigned int bundleId, const wxString& name);
	bool Save(BundleItemType type, unsigned int bundleId, unsigned int itemId);
	void Delete(BundleItemType type, unsigned int bundleId, unsigned int itemId);
	wxDateTime GetModDate(BundleItemType type, unsigned int bundleId, unsigned int itemId) const;

	// Bundle id's
	BundleItemType GetBundleTypeFromUri(const wxString& uri) const;
	wxString GetBundleItemUri(BundleItemType type, unsigned int bundleId, unsigned int itemId) const;
	bool GetBundleItemFromUri(const wxString& uuid, BundleItemType& type, unsigned int& bundleId, unsigned int& itemId) const;
	bool GetBundleItemFromUuid(const wxString& uuid, BundleItemType& type, unsigned int& bundleId, unsigned int& itemId) const;

	// Plist Items
	//PlistDict GetPlistDict(unsigned int plistId, unsigned int dictId) const;
	PListArray GetPlistArray(unsigned int plistId, unsigned int arrayId) const;
	//const char* GetPlistString(unsigned int plistId, unsigned int stringId) const;

	// Utility functions
	static wxString GetNewUuid();

	// Manual file extension to syntax associations
	wxString GetSyntaxAssoc(const wxString& ext) const;
	void SetSyntaxAssoc(const wxString& ext, const wxString& syntax);

private:

	void UpdatePlists(const wxFileName& path, wxArrayString& filePaths, int loc, c4_View vList);
	void UpdateBundles(const wxFileName& path, int loc, cxUpdateMode mode);
	void UpdateBundleSubDirs(const wxFileName& path, int loc, unsigned int bundleId, cxUpdateMode mode);

	// Bundle handling
	unsigned int NewManifest(const wxString& name);
	wxFileName GetLocalBundlePath(unsigned int bundleId);
	void DeleteBundleSubItems(unsigned int bundleId, int loc);

	// PlistItem handling
	unsigned int NewPlistItem(unsigned int ref, int loc, c4_View vList);
	void UpdatePlistItem(unsigned int ref, int loc, c4_View vList, unsigned int ndx);
	bool DeletePlistItem(unsigned int ndx, int loc, c4_View vList);
	void SafeDeletePlistItem(unsigned int ndx, c4_View vList, const wxFileName localPath);
	void DeleteAllItems(int loc, c4_View vList);
	PListDict GetPlistItem(unsigned int ndx, c4_View vList) const;
	PListDict GetEditablePlistItem(unsigned int ndx, c4_View vList);
	void MarkPlistItemAsModified(unsigned int ndx, c4_View vList);
	bool SavePListItem(unsigned int ndx, c4_View vList, const wxFileName& path, const wxString& ext);

	// Plist handling
	int LoadPList(const wxString& path);
	bool SavePList(unsigned int ndx, const wxFileName& path);
	unsigned int CreateNewPlist(const wxString& name, const wxString& filename = wxEmptyString);

	void DeletePList(unsigned int ndx);
	PListDict GetPlist(unsigned int ndx) const;
	void GetPlist(unsigned int ndx, PListDict& plist) const;
	unsigned int CopyPlist(unsigned int ndx);

	// Plist content handling
	int LoadDict(const TiXmlElement* dict, c4_RowRef& rPlist);
	int LoadArray(const TiXmlElement* array, c4_RowRef& rPlist);
	int LoadString(const char* str, c4_RowRef& rPlist);

	void SaveDict(TiXmlElement* parent, unsigned int ndx, const c4_RowRef& rPlist) const;
	void SaveArray(TiXmlElement* parent, unsigned int ndx, const c4_RowRef& rPlist) const;
	void SaveString(TiXmlElement* parent, unsigned int ndx, const c4_RowRef& rPlist) const;

	// Utility functions
	static wxString MakeValidDir(const wxFileName& path, const wxString& name, const wxString& ext);
	static wxString MakeValidFilename(const wxFileName& path, const wxString& name, const wxString& ext);

	// Event handlers
	void OnCommitTimer(wxTimerEvent& event);
	DECLARE_EVENT_TABLE();

	// Embedded classes
	class BundleCmp {
	public:
		BundleCmp(const c4_View& view) : m_view(view) {};
		bool operator()(unsigned int ndx1, unsigned int ndx2);
	private:
		const c4_View& m_view;
	};
	class ItemRefCmp {
	public:
		bool operator()(const cxItemRef& a, const cxItemRef& b) {
			return a.name.CmpNoCase(b.name) < 0;
		}
	};

	// Member variables
	bool m_dbChanged;
	bool m_allBundlesUpdated;
	const wxFileName m_appPath;
	const wxFileName m_appDataPath;
	c4_Storage m_storage;
	c4_View m_vThemes;
	c4_View m_vBundles;
	c4_View m_vPlists;
	c4_View m_vFreePlists;
	c4_View m_vSyntaxAssocs;
	wxTimer m_commitTimer;
	wxFileName m_bundleDir;
	wxFileName m_installedBundleDir;
	wxFileName m_localBundleDir;

	// Static constants
	static const char* DB_THEMES_FORMAT;
	static const char* DB_BUNDLES_FORMAT;
	static const char* DB_PLISTS_FORMAT;
	static const char* DB_FREEPLISTS_FORMAT;
};

class PListItem { // abstract class
public:
	PListItem() : m_rPlist(NULL) {};
	PListItem(c4_RowRef& plist) : m_rPlist(new c4_RowRef(plist)) {};
	~PListItem() {if (m_rPlist) delete m_rPlist;};
	bool IsOk() const {return (m_rPlist != NULL);};

	wxDateTime GetModDate() const;

protected:
	void DeleteDict(unsigned int ndx);
	void DeleteArray(unsigned int ndx);

	c4_RowRef* m_rPlist;
	c4_View m_vStrings;
	c4_View m_vDicts;
	c4_View m_vArrays;
};

class PListDict : public PListItem {
public:
	PListDict() {};
	PListDict(c4_View& dict, c4_RowRef& plist);
	PListDict(const PListDict& dict);
	void operator=(const PListDict& dict);

	void SetDict(const c4_View& dict, const c4_RowRef& plist);

	unsigned int GetSize() const;
	const char* GetKey(unsigned int ndx) const;

	bool HasKey(const char* key) const;
	const char* GetString(const char* key) const;
	wxString wxGetString(const char* key) const;
	bool GetDict(const char* key, PListDict& dict) const;
	bool GetArray(const char* key, PListArray& array) const;
	bool GetInteger(const char* key, int& value) const;

	void DeleteItem(const char* key);
	void SetString(const char* key, const char* text);
	void wxSetString(const char* key, const wxString& text);
	PListDict NewDict(const char* key);
	PListArray NewArray(const char* key);

	// Plist <-> JSON conversion
	wxString GetJSON(bool strip=false) const;
	bool SetJSON(wxJSONValue& root, bool strip=false);

	// support functions
	wxJSONValue GetJSONDict() const;
	void InsertJSONValues(const wxJSONValue& value);

#ifdef __WXDEBUG__
	void Print() const;
#endif

private:

	c4_View m_vDict;
};

class PListArray : public PListItem {
public:
	PListArray() {};
	PListArray(c4_View& array, c4_RowRef& plist);

	void SetArray(c4_View& array, c4_RowRef& plist);
	void Clear();

	unsigned int GetSize() const {return m_vArray.GetSize();};
	const char* GetString(unsigned int ndx) const;
	wxString wxGetString(unsigned int ndx) const;
	bool GetDict(unsigned int ndx, PListDict& dict) const;
	bool GetArray(unsigned int ndx, PListArray& array) const;

	void DeleteItem(unsigned int ndx);
	PListDict InsertNewDict(unsigned int ndx);
	PListArray InsertNewArray(unsigned int ndx);
	void InsertString(unsigned int ndx, const char* str);
	void AddString(const char* str);

	// Plist <-> JSON conversion (support functions)
	wxJSONValue GetJSONArray() const;
	void InsertJSONValues(const wxJSONValue& value);

private:
	c4_View m_vArray;
};

#endif //__PLISTHANDLER_H__
