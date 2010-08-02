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

#include "plistHandler.h"

#include <wx/dir.h>
#include <wx/ffile.h>

#include "FileActionThread.h"
#include "Catalyst.h"
#include "jsonwriter.h"
#include "jsonreader.h"

// tinyxml includes unused vars so it can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <tinyxml.h>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

enum {
	REF_STRING = 0,
	REF_DICT,
	REF_ARRAY,
	REF_INTEGER,
	REF_BOOL
};

enum {
	PLIST_PRISTINE = 1,
	PLIST_LOCAL = 2,
	PLIST_DISABLED = 4,
	PLIST_DELETED = 8,
	PLIST_INSTALLED = 16
};

// event id's
enum {
	ID_COMMITTIMER
};

const char* PListHandler::DB_THEMES_FORMAT =
	"themes["
		"uuid:S,"
		"loc:I,"
		"mod:I,"
		"prisref:I,"
		"localref:I]";

const char* PListHandler::DB_BUNDLES_FORMAT =
	"bundles["
		"uuid:S,"
		"name:S,"
		"path:S,"
		"loc:I,"       // info.plist
		"mod:I,"       // info.plist
		"prisref:I,"   // info.plist
		"localref:I,"  // info.plist
		"commands["
			"uuid:S,"
			"loc:I,"
			"mod:I,"
			"prisref:I,"
			"localref:I],"
		"dragcommands["
			"uuid:S,"
			"loc:I,"
			"mod:I,"
			"prisref:I,"
			"localref:I],"
		"snippets["
			"uuid:S,"
			"loc:I,"
			"mod:I,"
			"prisref:I,"
			"localref:I],"
		"syntaxes["
			"uuid:S,"
			"loc:I,"
			"mod:I,"
			"prisref:I,"
			"localref:I],"
		"prefs["
			"uuid:S,"
			"loc:I,"
			"mod:I,"
			"prisref:I,"
			"localref:I],"
		"macros["
			"uuid:S,"
			"loc:I,"
			"mod:I,"
			"prisref:I,"
			"localref:I]]";

const char* PListHandler::DB_PLISTS_FORMAT =
	"plists["
		"filename:S,"
		"moddate:L,"
		"dicts["
			"dict["
				"key:S,"
				"type:I,"
				"ref:I]],"
			"dict_H[_H:I,_R:I],"
		"arrays["
			"array["
				"type:I,"
				"ref:I]],"
		"strings["
			"string:S],"
		"freedicts["
			"ref:I],"
		"freearrays["
			"ref:I],"
		"freestrings["
			"ref:I]]";

const char* PListHandler::DB_FREEPLISTS_FORMAT =
	"freeplists["
		"ref:I]";

static const c4_ViewProp   pArray("array");
static const c4_ViewProp   pArrays("arrays");
static const c4_StringProp pBundleName("name");
static const c4_StringProp pBundlePath("path");
static const c4_ViewProp   pCommands("commands");
static const c4_ViewProp   pDict("dict");
static const c4_ViewProp   pDictH("dict_H");
static const c4_ViewProp   pDicts("dicts");
static const c4_ViewProp   pDragCommands("dragcommands");
static const c4_StringProp pExt("ext");
static const c4_StringProp pFilename("filename");
static const c4_ViewProp   pFreeDicts("freedicts");
static const c4_ViewProp   pFreeArrays("freearrays");
static const c4_ViewProp   pFreeStrings("freestrings");
static const c4_StringProp pKey("key");
static const c4_IntProp    pLocality("loc");
static const c4_IntProp    pLocalRef("localref");
static const c4_ViewProp   pMacros("macros");
static const c4_LongProp   pModDate("moddate");
static const c4_IntProp    pModified("mod");
static const c4_IntProp    pPristineRef("prisref");
static const c4_ViewProp   pPrefs("prefs");
static const c4_IntProp    pRef("ref");
static const c4_IntProp    pRefType("type");
static const c4_ViewProp   pSnippets("snippets");
static const c4_ViewProp   pStrings("strings");
static const c4_StringProp pString("string");
static const c4_StringProp pSynId("syntax");
static const c4_ViewProp   pSyntaxes("syntaxes");
static const c4_StringProp pUuid("uuid");
static const c4_IntProp    pDbInteger("int");

BEGIN_EVENT_TABLE(PListHandler, wxEvtHandler)
	EVT_TIMER(ID_COMMITTIMER, PListHandler::OnCommitTimer)
END_EVENT_TABLE()

PListHandler::PListHandler(const wxString& appPath, const wxString& appDataPath, bool rebuildDb)
: m_dbChanged(false), m_allBundlesUpdated(false), m_appPath(appPath, wxEmptyString), m_appDataPath(appDataPath, wxEmptyString) {
	wxFileName dbPath = m_appDataPath;
	dbPath.SetFullName(wxT("config.db"));
	m_dbPath = dbPath.GetFullPath();

	m_storage = c4_Storage(m_dbPath.mb_str(), true);

	// Check db version
	const int DB_VERSION = 6;
	c4_View vDbinfo = m_storage.GetAs("db[int:I]");
	if (vDbinfo.GetSize()) {
		const int dbver = pDbInteger(vDbinfo[0]);
		if (dbver < DB_VERSION || rebuildDb) {
			wxLogDebug(wxT("Old format config db (%d), deleting and recreating"), dbver);

			// Close and delete old db
			vDbinfo = c4_View(); // disconnect from storage
			m_storage = c4_Storage();
			wxRemoveFile(m_dbPath);

			// Start again from a fresh db
			m_storage = c4_Storage(m_dbPath.mb_str(), true);
			vDbinfo = m_storage.GetAs("db[int:I]");
			vDbinfo.Add(pDbInteger[DB_VERSION]); // set db version id
			m_dbChanged = true; // make sure new db layout is commited
		}
	}
	else vDbinfo.Add(pDbInteger[DB_VERSION]); // set db version id

	m_vThemes = m_storage.GetAs(DB_THEMES_FORMAT);
	m_vBundles = m_storage.GetAs(DB_BUNDLES_FORMAT);
	m_vPlists = m_storage.GetAs(DB_PLISTS_FORMAT);
	m_vFreePlists = m_storage.GetAs(DB_FREEPLISTS_FORMAT);

	// Manual file extension to syntax associations
	const c4_View assocs = m_storage.GetAs("assocs[ext:S,syntax:S]");
	const c4_View assocsh = m_storage.GetAs("assocs_H[_H:I,_R:I]");
	m_vSyntaxAssocs = assocs.Hash(assocsh);

	// Initialize TinyXml
	TiXmlBase::SetCondenseWhiteSpace(false);
#ifdef __WXMSW__
	TiXmlBase::SetStrictEntityEncoding(false);
	TiXmlBase::AllowBlankValues(true);
#endif

	// Set paths
	m_bundleDir = m_appPath;
	m_bundleDir.AppendDir(wxT("Bundles"));
	m_installedBundleDir = m_appDataPath;
	m_installedBundleDir.AppendDir(wxT("InstalledBundles"));
	m_localBundleDir = m_appDataPath;
	m_localBundleDir.AppendDir(wxT("Bundles"));

	Update(UPDATE_SYNTAXONLY);

}

PListHandler::~PListHandler() {
	if (m_dbChanged) {
		m_storage.Commit();
	}
}

wxString PListHandler::GetSyntaxAssoc(const wxString& ext) const {
	const int index = m_vSyntaxAssocs.Find(pExt[ext.mb_str(wxConvUTF8)]);
	return index == -1 ? wxString(wxEmptyString) : wxString(pSynId(m_vSyntaxAssocs[index]), wxConvUTF8);
}

void PListHandler::SetSyntaxAssoc(const wxString& ext, const wxString& syntaxId) {
	if (syntaxId.empty()) {
		// Empty id removes entry
		const int index = m_vSyntaxAssocs.Find(pExt[ext.mb_str(wxConvUTF8)]);
		if (index != -1) m_vSyntaxAssocs.RemoveAt(index);
	}
	else {
		m_vSyntaxAssocs.Add(pExt[ext.mb_str(wxConvUTF8)] + pSynId[syntaxId.mb_str(wxConvUTF8)]);
	}

	// Mark for commit in next idle time
	m_dbChanged = true;
}

void PListHandler::OnCommitTimer(wxTimerEvent& WXUNUSED(event)) {
	if (m_dbChanged) {
		wxLogDebug(wxT("Commiting config.db"));
		m_storage.Commit();
		m_dbChanged = false;
	}
}

void PListHandler::Update(cxUpdateMode mode) {
	m_allBundlesUpdated = false;

	if (mode != UPDATE_REST) {
		// Update Themes
		wxFileName themePath = m_appPath;
		themePath.AppendDir(wxT("Themes"));
		if (themePath.DirExists()) {
			// Update Pristine Themes
			wxSortedArrayString themeFiles;
			wxDir::GetAllFiles(themePath.GetPath(), &themeFiles, wxT("*.tmTheme"), wxDIR_FILES);

			UpdatePlists(themePath, themeFiles, PLIST_PRISTINE, m_vThemes);
		}
		else DeleteAllItems(PLIST_PRISTINE, m_vThemes);

		// Update Local Themes
		wxFileName localThemePath = m_appDataPath;
		localThemePath.AppendDir(wxT("Themes"));
		if (localThemePath.DirExists()) {
			wxSortedArrayString localFiles;
			wxDir::GetAllFiles(localThemePath.GetPath(), &localFiles, wxT("*.tmTheme"), wxDIR_FILES);

			UpdatePlists(localThemePath, localFiles, PLIST_LOCAL, m_vThemes);
		}
		else DeleteAllItems(PLIST_LOCAL, m_vThemes);
	}


	// Update Installed Bundles
	if (m_installedBundleDir.DirExists()) {
		// Update Pristine Bundles
		UpdateBundles(m_installedBundleDir, PLIST_INSTALLED, mode);
	}
	else DeleteAllItems(PLIST_INSTALLED, m_vBundles);

	// Update Pristine Bundles
	if (m_bundleDir.DirExists()) {
		// Update Pristine Bundles
		UpdateBundles(m_bundleDir, PLIST_PRISTINE, mode);
	}
	else DeleteAllItems(PLIST_PRISTINE, m_vBundles);

	// Update Local bundles
	if (m_localBundleDir.DirExists()) {
		UpdateBundles(m_localBundleDir, PLIST_LOCAL, mode);
	}
	else DeleteAllItems(PLIST_LOCAL, m_vBundles);

	// Mark for commit in next idle time
	m_dbChanged = true;
	if (mode != UPDATE_SYNTAXONLY) m_allBundlesUpdated = true;
}

void PListHandler::UpdatePlists(const wxFileName& path, wxArrayString& filePaths, int loc, c4_View vList) {
	wxASSERT(path.IsOk() && path.IsDir());

	// Check if files have been deleted or modified
	for (int i = 0; i < vList.GetSize(); ++i) {
		c4_RowRef rPlistItem = vList[i];
		const int locality = pLocality(rPlistItem);
		if (!(locality & loc)) continue;

		// Get the referenced plist
		int plistRef;
		if (loc == PLIST_PRISTINE || loc == PLIST_INSTALLED) plistRef = pPristineRef(rPlistItem);
		else {
			wxASSERT(loc == PLIST_LOCAL);
			plistRef = pLocalRef(rPlistItem);
		}
		c4_RowRef rPlist = m_vPlists[plistRef];

		// Get the path
		wxFileName dbPath = path;
		const wxString filename(pFilename(rPlist), wxConvUTF8);
		dbPath.SetFullName(filename);
		const wxString filepath = dbPath.GetFullPath();

		// Check if file has been deleted
		const int ndx = filePaths.Index(filepath);
		if (ndx == wxNOT_FOUND) {
			DeletePlistItem(i, loc, vList);
			continue;
		}

		// Get modification times
		const wxDateTime modDateDb((const wxLongLong)pModDate(rPlist));
		//const wxDateTime modDateFile(wxFileModificationTime(filePaths[ndx]));
		wxDateTime modDateFile = dbPath.GetModificationTime();
		modDateFile.SetMillisecond(0); // we cannot be sure milliseconds are stable between checks

		// Check if file has been modified
		if (modDateDb != modDateFile) {
			const int ref = LoadPList(filepath);
			if (ref != -1) {
				UpdatePlistItem(ref, loc, vList, i);
			}
			else if (DeletePlistItem(i, loc, vList)) --i;
		}

		// Remove file from file list when it has been processed
		filePaths.RemoveAt(ndx);
	}

	// Any files left in themeFiles are new
	for (unsigned int i2 = 0; i2 < filePaths.GetCount(); ++i2) {
		const wxString& path = filePaths[i2];
		const int ref = LoadPList(path);

		if (ref != -1) {
			// Get the uuid
			const PListDict plist = GetPlist(ref);
			const char* uuid = plist.GetString("uuid");
			wxASSERT(uuid);

			// Check if we have a matching plist in another locality
			const int plistId = uuid ? vList.Find(pUuid[uuid]) : -1;

			if (plistId != -1) {
#ifdef __WXDEBUG__
				const c4_RowRef rPlistItem = vList[plistId];
				const int id = (pLocality(rPlistItem) == PLIST_PRISTINE) ? pPristineRef(rPlistItem) : pLocalRef(rPlistItem);
				const wxString plistName(pFilename(m_vPlists[id]), wxConvUTF8);
				wxLogDebug(wxT("WARNING: plist '%s' has same uuid as '%s'"), path.c_str(), plistName.c_str());
#endif
				UpdatePlistItem(ref, loc, vList, plistId);
			}
			else NewPlistItem(ref, loc, vList);
		}
	}
}

void PListHandler::UpdateBundles(const wxFileName& path, int loc, cxUpdateMode mode) {
	wxASSERT(path.IsOk() && path.IsDir());
	wxASSERT(loc == PLIST_PRISTINE || loc == PLIST_LOCAL || loc == PLIST_INSTALLED);

	// Get all bundle dirs
	wxArrayString bundlePaths;
	wxDir d(path.GetPath());
	if (d.IsOpened()) {
		wxString dirname;
        for ( bool cont = d.GetFirst(&dirname, wxT("*.tmbundle"), wxDIR_DIRS); cont; cont = d.GetNext(&dirname)) {
			bundlePaths.Add(dirname);
		}
	}

	// Check if bundles have been deleted or modified
	for (int i = 0; i < m_vBundles.GetSize(); ++i) {
		c4_RowRef rBundle = m_vBundles[i];

		// Only deal with bundles with current locality
		const int locality = pLocality(rBundle);
		if (!(locality & loc)) continue;

		// Get the bundle dir name
		const wxString dirName(pBundlePath(rBundle), wxConvUTF8);

		// Check if bundle has been deleted
		const int ndx = bundlePaths.Index(dirName);
		if (ndx == wxNOT_FOUND) {
			if (DeletePlistItem(i, loc, m_vBundles)) --i; // adjust i for item removal
			continue;
		}

		// Check if we should ignore this bundle
		if ((loc & (PLIST_DISABLED|PLIST_DELETED))                      // ignore disabled
			|| (loc == PLIST_PRISTINE && locality & PLIST_INSTALLED)) { // installed overrides pristine
			bundlePaths.RemoveAt(ndx);
			continue;
		}

		wxFileName bundlePath = path;
		bundlePath.AppendDir(dirName);

		if (mode != UPDATE_REST) {
			// Check if 'info.plist' has been deleted
			wxFileName infoPath = bundlePath;
			infoPath.SetFullName(wxT("info.plist"));
			if (infoPath.FileExists()) {
				// Get the referenced plist
				int plistRef;
				if (loc == PLIST_PRISTINE || loc == PLIST_INSTALLED) plistRef = pPristineRef(rBundle);
				else {
					wxASSERT(loc == PLIST_LOCAL);
					plistRef = pLocalRef(rBundle);
				}
				c4_RowRef rPlist = m_vPlists[plistRef];

				// Get modification times for 'info.plist'
				const wxDateTime modDateDb((const wxLongLong)pModDate(rPlist));
				//const wxDateTime modDateFile(wxFileModificationTime(filepath));
				wxDateTime modDateFile = infoPath.GetModificationTime();
				modDateFile.SetMillisecond(0); // milliseconds are not stable between checks

				// Check if file has been modified
				if (modDateDb != modDateFile) {
					const wxString filepath = infoPath.GetFullPath();
					const int ref = LoadPList(filepath);
					if (ref != -1) {
						UpdatePlistItem(ref, loc, m_vBundles, i);
					}
					else {
						if (DeletePlistItem(i, loc, m_vBundles)) --i;
						bundlePaths.RemoveAt(ndx);
						continue;
					}
				}
			}
			else {
				if (loc == PLIST_PRISTINE || loc == PLIST_INSTALLED) {
					// A pristine bundle is only valid if it has 'info.plist'
					DeletePlistItem(i, loc, m_vBundles);
					continue;
				}
				else {
					int ref = -1;

					// See if we can copy manifest from pristine
					if (locality & PLIST_PRISTINE || locality & PLIST_INSTALLED) {
						unsigned int prisRef = pPristineRef(rBundle);
						ref = CopyPlist(prisRef);
					}
					else {
						// Create a new manifest
						const wxString name = dirName.BeforeLast(wxT('.'));
						ref = NewManifest(name);
					}

					UpdatePlistItem(ref, loc, m_vBundles, i);
					SavePList(ref, bundlePath);
				}
			}
		}

		// Update all bundle items
		UpdateBundleSubDirs(bundlePath, loc, i, mode);

		// Remove file from file list when it has been processed
		bundlePaths.RemoveAt(ndx);
	}

	// Any bundles left in bundlePaths are new
	for (unsigned int i2 = 0; i2 < bundlePaths.GetCount(); ++i2) {
		const wxString& dirName = bundlePaths[i2];
		int ref = -1;

		// Check if we have a matching bundle in another locality
		int bundleId = m_vBundles.Find(pBundlePath[dirName.mb_str(wxConvUTF8)]);

		// Check if we should ignore this bundle
		if (bundleId != -1 && loc == PLIST_PRISTINE) {
			const int locality = pLocality(m_vBundles[bundleId]);
			if (locality & PLIST_INSTALLED) continue; // installed overrides pristine
		}

		// Get paths
		wxFileName bundlePath = path;
		bundlePath.AppendDir(dirName);

		if (mode != UPDATE_REST) {
			wxFileName infoPath = bundlePath;
			infoPath.SetFullName(wxT("info.plist"));

			// Check if we have 'info.plist'
			if (infoPath.FileExists()) {
				const wxString filepath = infoPath.GetFullPath();
				ref = LoadPList(filepath);
			}
			else if (loc == PLIST_LOCAL) {
				// See if we can copy manifest from pristine
				if (bundleId != -1) {
					const int locality = pLocality(m_vBundles[bundleId]);
					if (locality & PLIST_PRISTINE || locality & PLIST_INSTALLED) {
						unsigned int prisRef = pPristineRef(m_vBundles[bundleId]);
						ref = CopyPlist(prisRef);
					}
				}
				else {
					// Create a new manifest
					const wxString name = dirName.BeforeLast(wxT('.'));
					ref = NewManifest(name);
				}
				SavePList(ref, bundlePath);
			}
			else {
				// A pristine bundle is only valid if it has 'info.plist'
				continue;
			}
		}

		if (ref != -1) {
			if (bundleId != -1) UpdatePlistItem(ref, loc, m_vBundles, bundleId);
			else {
				bundleId = NewPlistItem(ref, loc, m_vBundles);

				// Set the path
				c4_RowRef rBundle = m_vBundles[bundleId];
				pPath(rBundle) = dirName.mb_str(wxConvUTF8);
			}

			UpdateBundleSubDirs(bundlePath, loc, bundleId, mode);
		}
	}
}

void PListHandler::UpdateBundleSubDirs(const wxFileName& path, int loc, unsigned int bundleId, cxUpdateMode mode) {
	wxASSERT(path.IsOk() && path.IsDir());
	wxASSERT(loc == PLIST_PRISTINE || loc == PLIST_LOCAL || loc == PLIST_INSTALLED);
	wxASSERT((int)bundleId < m_vBundles.GetSize());

	c4_RowRef rBundle = m_vBundles[bundleId];

	if (mode != UPDATE_REST) {
		// Update Syntaxes
		wxFileName syntaxDir = path;
		syntaxDir.AppendDir(wxT("Syntaxes"));
		c4_View vSyntaxes = pSyntaxes(rBundle);
		if (syntaxDir.DirExists()) {
			wxSortedArrayString syntaxFiles;
			wxDir::GetAllFiles(syntaxDir.GetPath(), &syntaxFiles, wxT("*.plist"), wxDIR_FILES);
			wxDir::GetAllFiles(syntaxDir.GetPath(), &syntaxFiles, wxT("*.tmLanguage"), wxDIR_FILES);

			UpdatePlists(syntaxDir, syntaxFiles, loc, vSyntaxes);
		}
		else DeleteAllItems(loc, vSyntaxes);
	}

	if (mode != UPDATE_SYNTAXONLY) {
		// Update Commands
		wxFileName commandsDir = path;
		commandsDir.AppendDir(wxT("Commands"));
		c4_View vCommands = pCommands(rBundle);
		if (commandsDir.DirExists()) {
			wxSortedArrayString files;
			wxDir::GetAllFiles(commandsDir.GetPath(), &files, wxT("*.plist"), wxDIR_FILES);
			wxDir::GetAllFiles(commandsDir.GetPath(), &files, wxT("*.tmCommand"), wxDIR_FILES);

			UpdatePlists(commandsDir, files, loc, vCommands);
		}
		else DeleteAllItems(loc, vCommands);

		// Update Snippets
		wxFileName snippetsDir = path;
		snippetsDir.AppendDir(wxT("Snippets"));
		c4_View vSnippets = pSnippets(rBundle);
		if (snippetsDir.DirExists()) {
			wxSortedArrayString files;
			wxDir::GetAllFiles(snippetsDir.GetPath(), &files, wxT("*.plist"), wxDIR_FILES);
			wxDir::GetAllFiles(snippetsDir.GetPath(), &files, wxT("*.tmSnippet"), wxDIR_FILES);

			UpdatePlists(snippetsDir, files, loc, vSnippets);
		}
		else DeleteAllItems(loc, vSnippets);

		// Update DragCommands
		wxFileName dragCommandsDir = path;
		dragCommandsDir.AppendDir(wxT("DragCommands"));
		c4_View vDragCommands = pDragCommands(rBundle);
		if (dragCommandsDir.DirExists()) {
			wxSortedArrayString files;
			wxDir::GetAllFiles(dragCommandsDir.GetPath(), &files, wxT("*.plist"), wxDIR_FILES);
			wxDir::GetAllFiles(dragCommandsDir.GetPath(), &files, wxT("*.tmDragCommand"), wxDIR_FILES);

			UpdatePlists(dragCommandsDir, files, loc, vDragCommands);
		}
		else DeleteAllItems(loc, vDragCommands);

		// Update Preferences
		wxFileName prefsDir = path;
		prefsDir.AppendDir(wxT("Preferences"));
		c4_View vPrefs= pPrefs(rBundle);
		if (prefsDir.DirExists()) {
			wxSortedArrayString files;
			wxDir::GetAllFiles(prefsDir.GetPath(), &files, wxT("*.plist"), wxDIR_FILES);
			wxDir::GetAllFiles(prefsDir.GetPath(), &files, wxT("*.tmPreferences"), wxDIR_FILES);

			UpdatePlists(prefsDir, files, loc, vPrefs);
		}
		else DeleteAllItems(loc, vPrefs);

		// Update Macros
		wxFileName macrosDir = path;
		macrosDir.AppendDir(wxT("Macros"));
		c4_View vMacros = pMacros(rBundle);
		if (macrosDir.DirExists()) {
			wxSortedArrayString files;
			wxDir::GetAllFiles(macrosDir.GetPath(), &files, wxT("*.plist"), wxDIR_FILES);
			wxDir::GetAllFiles(macrosDir.GetPath(), &files, wxT("*.tmMacro"), wxDIR_FILES);

			UpdatePlists(macrosDir, files, loc, vMacros);
		}
		else DeleteAllItems(loc, vMacros);

	}
}

wxDateTime PListHandler::GetBundleModDate(unsigned int bundleId) const {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());

	const c4_RowRef rBundle = m_vBundles[bundleId];
	const int loc = pLocality(rBundle);
	const wxString dirName(pBundlePath(rBundle), wxConvUTF8);

	// Only get modDate from pristine or installed bundles
	wxASSERT(loc & (PLIST_PRISTINE|PLIST_INSTALLED));

	// The date of the bundle is set on the info.plist file
	// as windows does not support setting dates on dirs on FAT
	wxFileName path = (loc & PLIST_INSTALLED) ? m_installedBundleDir : m_bundleDir; // installed overrides pristine
	path.AppendDir(dirName);
	path.SetFullName(wxT("info.plist"));
	return path.GetModificationTime();
}

vector<cxBundleInfo> PListHandler::GetInstalledBundlesInfo() const {
	vector<cxBundleInfo> bInfo;

	for (unsigned int i = 0; (int)i < m_vBundles.GetSize(); ++i) {
		const c4_RowRef rBundle = m_vBundles[i];
		const int loc = pLocality(rBundle);

		if (loc & (PLIST_PRISTINE|PLIST_INSTALLED)) {
			cxBundleInfo bi;
			bi.id = i;
			bi.dirName = wxString(pBundlePath(rBundle), wxConvUTF8);

			if (loc & (PLIST_DISABLED|PLIST_DELETED)) bi.isDisabled = true;
			else {
				bi.isDisabled = false;

				// The date of the bundle is set on the info.plist file
				// as windows does not support setting dates on dirs on FAT
				wxFileName path = (loc & PLIST_INSTALLED) ? m_installedBundleDir : m_bundleDir; // installed overrides pristine
				path.AppendDir(bi.dirName);
				path.SetFullName(wxT("info.plist"));
				bi.modDate = path.GetModificationTime();
			}

			bInfo.push_back(bi);
		}
	}

	return bInfo;
}

vector<unsigned int> PListHandler::GetBundles() const {
	vector<unsigned int> bundles;

	for (unsigned int i = 0; (int)i < m_vBundles.GetSize(); ++i) {
		const c4_RowRef rBundle = m_vBundles[i];
		const int loc = pLocality(rBundle);

		if ((loc & (PLIST_DISABLED|PLIST_DELETED)) == 0) {
			bundles.push_back(i);
		}
	}

	// Sort list based on bundle names
	BundleCmp cmp(m_vBundles);
	sort(bundles.begin(), bundles.end(), cmp);

	return bundles;
}

wxString PListHandler::GetBundleItemUri(BundleItemType type, unsigned int bundleId, unsigned int itemId) const {
	wxASSERT((int)bundleId < m_vBundles.GetSize());
	if ((int)bundleId >= m_vBundles.GetSize()) return wxEmptyString;

	wxString uri = wxT("bundle://");

	// Get the bundle dir name
	const c4_RowRef rBundle = m_vBundles[bundleId];
	const wxString dirName(pBundlePath(rBundle), wxConvUTF8);
	uri += dirName + wxT("/");

	// Get type
	c4_View vItems;
	switch(type) {
	case BUNDLE_SNIPPET:
		uri += wxT("Snippets");
		vItems = pSnippets(rBundle);
		break;
	case BUNDLE_COMMAND:
		uri += wxT("Commands");
		vItems = pCommands(rBundle);
		break;
	case BUNDLE_DRAGCMD:
		uri += wxT("DragCommands");
		vItems = pDragCommands(rBundle);
		break;
	case BUNDLE_PREF:
		uri += wxT("Preferences");
		vItems = pPrefs(rBundle);
		break;
	case BUNDLE_LANGUAGE:
		uri += wxT("Syntaxes");
		vItems = pSyntaxes(rBundle);
		break;
	case BUNDLE_MACRO:
		uri += wxT("Macros");
		vItems = pMacros(rBundle);
		break;
	default:
		wxASSERT(false);
		return wxEmptyString;
	}
	uri += wxT("/");

	// Get ref to plist
	wxASSERT((int)itemId < vItems.GetSize());
	const c4_RowRef rItem = vItems[itemId];
	const int locality = pLocality(rItem);
	int ref = -1;
	if (locality & PLIST_DISABLED) return wxEmptyString;
	else if (locality & PLIST_LOCAL) ref = pLocalRef(rItem);
	else if (locality & PLIST_PRISTINE || locality & PLIST_INSTALLED) ref = pPristineRef(rItem);
	else wxASSERT(false);

	// Get item name
	const wxString itemName(pFilename(m_vPlists[ref]), wxConvUTF8);
	uri += itemName;

	return uri;
}

BundleItemType PListHandler::GetBundleTypeFromUri(const wxString& uri) const {
	// Uri format is "bundle://bundlename/type/itemname"
	wxString uri_part;
	if (!uri.StartsWith(wxT("bundle://"), &uri_part)) return BUNDLE_NONE;

	// Parse uri parts
	const wxString bundlename = uri_part.BeforeFirst(wxT('/'));
	uri_part = uri_part.AfterFirst(wxT('/'));
	const wxString itemtype = uri_part.BeforeFirst(wxT('/'));

	if (itemtype == wxT("Snippets")) return BUNDLE_SNIPPET;
	if (itemtype == wxT("Commands")) return BUNDLE_COMMAND;
	if (itemtype == wxT("DragCommands")) return BUNDLE_DRAGCMD;
	if (itemtype == wxT("Preferences")) return BUNDLE_PREF;
	if (itemtype == wxT("Syntaxes")) return BUNDLE_LANGUAGE;
	if (itemtype == wxT("Macros")) return BUNDLE_MACRO;
	return BUNDLE_NONE;
}

bool PListHandler::GetBundleItemFromUri(const wxString& uri, BundleItemType& type, unsigned int& bundleId, unsigned int& itemId) const {
	// Uri format is "bundle://bundlename/type/itemname"
	wxString uri_part;
	if (!uri.StartsWith(wxT("bundle://"), &uri_part)) return false;

	// Parse uri parts
	const wxString bundlename = uri_part.BeforeFirst(wxT('/'));
	uri_part = uri_part.AfterFirst(wxT('/'));
	const wxString itemtype = uri_part.BeforeFirst(wxT('/'));
	const wxString itemName = uri_part.AfterFirst(wxT('/'));
	if (bundlename.empty() || itemtype.empty() || itemName.empty()) return false;

	// Find bundle
	const c4_Row rBundleName = pBundlePath[bundlename.ToUTF8()];
	const int bundleNdx = m_vBundles.Find(rBundleName);
	if (bundleNdx == -1) return false;
	bundleId = bundleNdx;
	const c4_RowRef rBundle = m_vBundles[bundleId];

	// Find type
	c4_View vItems;
	if (itemtype == wxT("Snippets")) {
		type = BUNDLE_SNIPPET;
		vItems = pSnippets(rBundle);
	}
	else if (itemtype == wxT("Commands")) {
		type = BUNDLE_COMMAND;
		vItems = pCommands(rBundle);
	}
	else if (itemtype == wxT("DragCommands")) {
		type = BUNDLE_DRAGCMD;
		vItems = pDragCommands(rBundle);
	}
	else if (itemtype == wxT("Preferences")) {
		type = BUNDLE_PREF;
		vItems = pPrefs(rBundle);
	}
	else if (itemtype == wxT("Syntaxes")) {
		type = BUNDLE_LANGUAGE;
		vItems = pSyntaxes(rBundle);
	}
	else if (itemtype == wxT("Macros")) {
		type = BUNDLE_MACRO;
		vItems = pMacros(rBundle);
	}
	else return false;

	// Find item with matching name
	const wxCharBuffer item = itemName.ToUTF8();
	int itemNdx = -1;
	for (int i = 0; i < vItems.GetSize(); ++i) {
		const c4_RowRef rItem = vItems[i];
		const int locality = pLocality(rItem);

		int ref;
		if (locality & PLIST_DISABLED) continue;
		else if (locality & PLIST_LOCAL) ref = pLocalRef(rItem);
		else if (locality & PLIST_PRISTINE || locality & PLIST_INSTALLED) ref = pPristineRef(rItem);
		else continue;

		if (strcmp(item, (const char*)pFilename(m_vPlists[ref])) == 0) {
			itemNdx = i;
			break;
		}
	}

	if (itemNdx == -1) return false;
	itemId = itemNdx;

	return true;
}


bool PListHandler::GetBundleItemFromUuid(const wxString& uuid, BundleItemType& type, unsigned int& bundleId, unsigned int& itemId) const {
	const wxCharBuffer str = uuid.ToUTF8();
	const c4_Row rUuid = pUuid[str.data()];

	for (int b = 0; b < m_vBundles.GetSize(); ++b) {
		const c4_RowRef rBundle = m_vBundles[b];

		const c4_View vSnippets = pSnippets(rBundle);
		int ndx = vSnippets.Find(rUuid);
		if (ndx != -1) {
			type = BUNDLE_SNIPPET;
			bundleId = b;
			itemId = ndx;
			return true;
		}

		const c4_View vCommands = pCommands(rBundle);
		ndx = vCommands.Find(rUuid);
		if (ndx != -1) {
			type = BUNDLE_COMMAND;
			bundleId = b;
			itemId = ndx;
			return true;
		}

		const c4_View vDragCommands = pDragCommands(rBundle);
		ndx = vDragCommands.Find(rUuid);
		if (ndx != -1) {
			type = BUNDLE_DRAGCMD;
			bundleId = b;
			itemId = ndx;
			return true;
		}

		const c4_View vPrefs = pPrefs(rBundle);
		ndx = vPrefs.Find(rUuid);
		if (ndx != -1) {
			type = BUNDLE_PREF;
			bundleId = b;
			itemId = ndx;
			return true;
		}

		const c4_View vSyntaxes = pSyntaxes(rBundle);
		ndx = vSyntaxes.Find(rUuid);
		if (ndx != -1) {
			type = BUNDLE_LANGUAGE;
			bundleId = b;
			itemId = ndx;
			return true;
		}

		const c4_View vMacros = pMacros(rBundle);
		ndx = vMacros.Find(rUuid);
		if (ndx != -1) {
			type = BUNDLE_MACRO;
			bundleId = b;
			itemId = ndx;
			return true;
		}
	}

	return false;
}

vector<unsigned int> PListHandler::GetList(BundleItemType type, unsigned int bundleId) const {
	wxASSERT((int)bundleId < m_vBundles.GetSize());

	vector<unsigned int> items;
	const c4_RowRef rBundle = m_vBundles[bundleId];
	c4_View vItems;

	switch(type) {
	case BUNDLE_SNIPPET:
		vItems = pSnippets(rBundle);
		break;
	case BUNDLE_COMMAND:
		vItems = pCommands(rBundle);
		break;
	case BUNDLE_DRAGCMD:
		vItems = pDragCommands(rBundle);
		break;
	case BUNDLE_PREF:
		vItems = pPrefs(rBundle);
		break;
	case BUNDLE_LANGUAGE:
		vItems = pSyntaxes(rBundle);
		break;
	case BUNDLE_MACRO:
		vItems = pMacros(rBundle);
		break;
	default:
		wxASSERT(false);
	}

	for (int i = 0; i < vItems.GetSize(); ++i) {
		const c4_RowRef rItem = vItems[i];
		if ((pLocality(rItem) & (PLIST_DISABLED|PLIST_DELETED)) == 0) {
			items.push_back(i);
		}
	}

	return items;
}

PListDict PListHandler::Get(BundleItemType type, unsigned int bundleId, unsigned int itemId) const {
	wxASSERT((int)bundleId < m_vBundles.GetSize());

	const c4_RowRef rBundle = m_vBundles[bundleId];
	c4_View vItems;

	switch(type) {
	case BUNDLE_SNIPPET:
		vItems = pSnippets(rBundle);
		break;
	case BUNDLE_COMMAND:
		vItems = pCommands(rBundle);
		break;
	case BUNDLE_DRAGCMD:
		vItems = pDragCommands(rBundle);
		break;
	case BUNDLE_PREF:
		vItems = pPrefs(rBundle);
		break;
	case BUNDLE_LANGUAGE:
		vItems = pSyntaxes(rBundle);
		break;
	case BUNDLE_MACRO:
		vItems = pMacros(rBundle);
		break;
	default:
		wxASSERT(false);
	}

	wxASSERT((int)itemId < vItems.GetSize());

	return GetPlistItem(itemId, vItems);
}

PListDict PListHandler::GetEditable(BundleItemType type, unsigned int bundleId, unsigned int itemId) {
	wxASSERT((int)bundleId < m_vBundles.GetSize());

	const c4_RowRef rBundle = m_vBundles[bundleId];
	c4_View vItems;

	switch(type) {
	case BUNDLE_SNIPPET:
		vItems = pSnippets(rBundle);
		break;
	case BUNDLE_COMMAND:
		vItems = pCommands(rBundle);
		break;
	case BUNDLE_DRAGCMD:
		vItems = pDragCommands(rBundle);
		break;
	case BUNDLE_PREF:
		vItems = pPrefs(rBundle);
		break;
	case BUNDLE_LANGUAGE:
		vItems = pSyntaxes(rBundle);
		break;
	case BUNDLE_MACRO:
		vItems = pMacros(rBundle);
		break;
	default:
		wxASSERT(false);
	}

	wxASSERT((int)itemId < vItems.GetSize());

	return GetEditablePlistItem(itemId, vItems);
}

unsigned int PListHandler::New(BundleItemType type, unsigned int bundleId, const wxString& name) {
	wxASSERT((int)bundleId < m_vBundles.GetSize());
	wxASSERT(!name.empty());

	const int ref = CreateNewPlist(name);
	const c4_RowRef rBundle = m_vBundles[bundleId];
	c4_View vItems;

	switch(type) {
	case BUNDLE_SNIPPET:
		vItems = pSnippets(rBundle);
		break;
	case BUNDLE_COMMAND:
		vItems = pCommands(rBundle);
		break;
	case BUNDLE_DRAGCMD:
		vItems = pDragCommands(rBundle);
		break;
	case BUNDLE_PREF:
		vItems = pPrefs(rBundle);
		break;
	case BUNDLE_LANGUAGE:
		vItems = pSyntaxes(rBundle);
		break;
	case BUNDLE_MACRO:
		vItems = pMacros(rBundle);
		break;
	default:
		wxASSERT(false);
	}

	// NOTE: The plist is not saved yet. Remember to do that later.
	return NewPlistItem(ref, PLIST_LOCAL, vItems);
}

void PListHandler::Delete(BundleItemType type, unsigned int bundleId, unsigned int itemId) {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());

	const c4_RowRef rBundle = m_vBundles[bundleId];
	wxFileName path = GetLocalBundlePath(bundleId);
	c4_View vPlists;

	switch(type) {
	case BUNDLE_SNIPPET:
		path.AppendDir(wxT("Snippets"));
		vPlists = pSnippets(rBundle);
		break;
	case BUNDLE_COMMAND:
		path.AppendDir(wxT("Commands"));
		vPlists = pCommands(rBundle);
		break;
	case BUNDLE_DRAGCMD:
		path.AppendDir(wxT("DragCommands"));
		vPlists = pDragCommands(rBundle);
		break;
	case BUNDLE_PREF:
		path.AppendDir(wxT("Preferences"));
		vPlists = pPrefs(rBundle);
		break;
	case BUNDLE_LANGUAGE:
		path.AppendDir(wxT("Syntaxes"));
		vPlists = pSyntaxes(rBundle);
		break;
	case BUNDLE_MACRO:
		path.AppendDir(wxT("Macros"));
		vPlists = pMacros(rBundle);
		break;
	default:
		wxASSERT(false);
	}

	wxASSERT((int)itemId < vPlists.GetSize());

	SafeDeletePlistItem(itemId, vPlists, path);
	m_dbChanged = true; // Mark for commit in next idle time
}


void PListHandler::GetThemes(vector<cxItemRef>& themes) const {
	themes.clear();

	for (unsigned int i = 0; (int)i < m_vThemes.GetSize(); ++i) {
		const c4_RowRef rTheme = m_vThemes[i];
		if ((pLocality(rTheme) & (PLIST_DISABLED|PLIST_DELETED)) == 0) {
			const PListDict theme = GetTheme(i);

			themes.push_back(cxItemRef(theme.wxGetString("name"), i));
		}
	}

	sort(themes.begin(), themes.end(), ItemRefCmp());
}

wxString PListHandler::MakeValidDir(const wxFileName& path, const wxString& name, const wxString& ext) { // static
	wxASSERT(path.IsOk() && path.IsDir());

	// Remove invalid chars from name
	wxString dirName = name;
	unsigned int i = 0;
	while (i < dirName.size()) {
		switch (dirName[i]) {
		case wxT('\\'):
		case wxT('/'):
		case wxT(':'):
		case wxT('*'):
		case wxT('?'):
		case wxT('"'):
		case wxT('<'):
		case wxT('>'):
		case wxT('|'):
			dirName.Remove(i, 1);
			break;
		default:
			++i;
		}
	}
	if (dirName.empty()) dirName = wxT("New");

	// Check if file already exists
	wxString fullName;
	wxFileName newpath;
	unsigned int count = 0;
	do {
		fullName = dirName;
		if (count) fullName += wxString::Format(wxT("%u"), count);
		fullName += ext;
		newpath = path;
		newpath.AppendDir(fullName);
		++count;
	} while (newpath.DirExists());

	return fullName;
}

wxString PListHandler::MakeValidFilename(const wxFileName& path, const wxString& name, const wxString& ext) { // static
	wxASSERT(path.IsOk() && path.IsDir());

	// Remove invalid chars from name
	wxString fileName = name;
	unsigned int i = 0;
	while (i < fileName.size()) {
		switch (fileName[i]) {
		case wxT('\\'):
		case wxT('/'):
		case wxT(':'):
		case wxT('*'):
		case wxT('?'):
		case wxT('"'):
		case wxT('<'):
		case wxT('>'):
		case wxT('|'):
			fileName.Remove(i, 1);
			break;
		default:
			++i;
		}
	}
	if (fileName.empty()) fileName = wxT("New");

	// Check if file already exists
	wxString fullName;
	wxFileName newpath;
	unsigned int count = 0;
	do {
		fullName = fileName;
		if (count) fullName += wxString::Format(wxT("%u"), count);
		fullName += ext;
		newpath = path;
		newpath.SetFullName(fullName);
		++count;
	} while (newpath.FileExists());

	return fullName;
}

unsigned int PListHandler::NewBundle(const wxString& name) {
	// Create new bundle 'info.plist' manifest
	const unsigned int manifestRef = NewManifest(name);

	// Create the bundle item
	const unsigned int bundleId = NewPlistItem(manifestRef, PLIST_LOCAL, m_vBundles);

	// NOTE: The manifest plist is not saved yet. Remember to do that later.
	return bundleId;
}

unsigned int PListHandler::NewManifest(const wxString& name) {
	wxASSERT(!name.empty());

	// Return reference to the new manifest plist
	// NOTE: The plist is not saved yet. Remember to do that later.
	return CreateNewPlist(name, wxT("info.plist"));
}

unsigned int PListHandler::CreateNewPlist(const wxString& name, const wxString& filename) {
	// Create the plist
	int ref;
	if(m_vFreePlists.GetSize()) {
		ref = pRef(m_vFreePlists[0]);
		m_vFreePlists.RemoveAt(0);
	}
	else ref = m_vPlists.Add(c4_Row());
	c4_RowRef rPlist = m_vPlists[ref];

	// Set filename
	if (!filename.empty()) {
		pFilename(rPlist) = filename.mb_str(wxConvUTF8);
	}

	// Create the root dict
	c4_View vDicts = pDicts(rPlist);
	const int vref = vDicts.Add(c4_Row());

	// Get the hash
	c4_RowRef rDict = vDicts[vref];
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);
	PListDict rootDict(vDict, rPlist);

	// Set name
	rootDict.SetString("name", name.mb_str(wxConvUTF8));

	// Create and set new uuid
	const wxString uuid = GetNewUuid();
	rootDict.SetString("uuid", uuid.mb_str(wxConvUTF8));

	// Return reference to the new plist
	return ref;
}

unsigned int PListHandler::NewPlistItem(unsigned int ref, int loc, c4_View vList) {
	wxASSERT((int)ref < m_vPlists.GetSize());
	wxASSERT(loc == PLIST_PRISTINE || loc == PLIST_LOCAL || loc == PLIST_INSTALLED);

	// Get uuid
	const PListDict plist = GetPlist(ref);
	const char* uuid = plist.GetString("uuid");
	wxASSERT(uuid);

	// Check if we can reuse a previously deleted item
	int reuse_ndx = -1;
	for (int i = 0; i < vList.GetSize(); ++i) {
		if (pLocality(vList[i]) == PLIST_DELETED) {
			reuse_ndx = i;
			break;
		}
	}

	// Add new plist
	c4_Row rPlist;
	if (loc == PLIST_PRISTINE || loc == PLIST_INSTALLED) pPristineRef(rPlist) = ref;
	else if (loc == PLIST_LOCAL) pLocalRef(rPlist) = ref;
	pLocality(rPlist) = loc;
	pUuid(rPlist) = uuid;

	// Check if we should set name
	if (vList.FindPropIndexByName("name") != -1) {
		const char* name = plist.GetString("name");
		wxASSERT_MSG(name, wxT("Plist does not have a name"));

		if (name) pBundleName(rPlist) = name;
	}

	if (reuse_ndx == -1) return vList.Add(rPlist);
	else {
		vList[reuse_ndx] = rPlist;
		return reuse_ndx;
	}
}

void PListHandler::UpdatePlistItem(unsigned int ref, int loc, c4_View vList, unsigned int ndx) {
	wxASSERT((int)ref < m_vPlists.GetSize());
	wxASSERT(loc == PLIST_PRISTINE || loc == PLIST_LOCAL || loc == PLIST_INSTALLED);
	wxASSERT((int)ndx < vList.GetSize());

	// Get uuid
	const PListDict plist = GetPlist(ref);
	const char* newUuid = plist.GetString("uuid");

	// Get item
	c4_RowRef rPlist = vList[ndx];
	int locality = pLocality(rPlist);
	const char* oldUuid = pUuid(rPlist);

	// Delete the old plist
	if (loc == PLIST_PRISTINE && locality & PLIST_PRISTINE) DeletePList(pPristineRef(rPlist));
	else if (loc == PLIST_INSTALLED && locality & PLIST_INSTALLED) DeletePList(pPristineRef(rPlist));
	else if (loc == PLIST_LOCAL && locality & PLIST_LOCAL) DeletePList(pLocalRef(rPlist));

	// Update item
	pLocality(rPlist) = locality | loc;
	if (loc == PLIST_PRISTINE || loc == PLIST_INSTALLED) pPristineRef(rPlist) = ref;
	else if (loc == PLIST_LOCAL) pLocalRef(rPlist) = ref;

	if (!newUuid) {
		wxFAIL_MSG(wxT("No uuid in plist"));
		pUuid(rPlist) = "";
	}
	else if(strcmp(newUuid, oldUuid) != 0) pUuid(rPlist) = newUuid;

	// Check if we should update name
	if (vList.FindPropIndexByName("name") != -1) {
		const char* newName = plist.GetString("name");
		const char* oldName = pBundleName(rPlist);
		if (strcmp(newName, oldName) != 0) pBundleName(rPlist) = newName;
	}
}

bool PListHandler::DeletePlistItem(unsigned int ndx, int loc, c4_View vList) {
	wxASSERT((int)ndx < vList.GetSize());

	c4_RowRef rPlistItem = vList[ndx];
	int locality = pLocality(rPlistItem);

	// Delete the plist(s)
	if (loc & PLIST_INSTALLED && locality & PLIST_INSTALLED) {
		loc |= PLIST_PRISTINE; // Installed overrides pristine, so make sure we don't leave a dangling pristine ref
		DeletePList(pPristineRef(rPlistItem));
	}
	else if (loc & PLIST_PRISTINE && locality & PLIST_PRISTINE) DeletePList(pPristineRef(rPlistItem));
	if (loc & PLIST_LOCAL && locality & PLIST_LOCAL) DeletePList(pLocalRef(rPlistItem));

	// Disable references
	locality &= ~loc;

	// Check if we are in a bundle and need to delete subitems
	if (vList.FindPropIndexByName("syntaxes") != -1) {
		DeleteBundleSubItems(ndx, loc);
	}

	// Check if we have to delete the entire item
	if ((locality & (PLIST_PRISTINE|PLIST_LOCAL|PLIST_INSTALLED)) == 0) {
		vList.RemoveAt(ndx);
		return true;
	}

	// Just update locality
	pLocality(rPlistItem) = locality;
	return false;
}

void PListHandler::DeleteBundleSubItems(unsigned int bundleId, int loc) {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());

	c4_RowRef rBundle = m_vBundles[bundleId];
	c4_View vSyntaxes = pSyntaxes(rBundle);
	c4_View vCommands = pCommands(rBundle);
	c4_View vSnippets = pSnippets(rBundle);
	c4_View vPrefs= pPrefs(rBundle);

	// Delete subitems
	DeleteAllItems(loc, vSyntaxes);
	DeleteAllItems(loc, vCommands);
	DeleteAllItems(loc, vSnippets);
	DeleteAllItems(loc, vPrefs);
}

void PListHandler::DeleteAllItems(int loc, c4_View vList) {
	for (int i = 0; i < vList.GetSize(); ++i) {
		if (DeletePlistItem(i, loc, vList)) --i;
	}
}

unsigned int PListHandler::NewTheme(const wxString& name) {
	wxASSERT(!name.empty());

	// Create the plist
	int ref;
	if(m_vFreePlists.GetSize()) {
		ref = pRef(m_vFreePlists[0]);
		m_vFreePlists.RemoveAt(0);
	}
	else ref = m_vPlists.Add(c4_Row());
	c4_RowRef rPlist = m_vPlists[ref];

	// Set filename
	const wxString filename = name + wxT(".tmTheme");
	pFilename(rPlist) = filename.mb_str(wxConvUTF8);

	// Create the root dict & array
	c4_View vDicts = pDicts(rPlist);
	const int vref = vDicts.Add(c4_Row());

	// Get the hash
	c4_RowRef rDict = vDicts[vref];
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	PListDict rootDict(vDict, rPlist);
	PListArray rootArray = rootDict.NewArray("settings");

	// Set name
	rootDict.SetString("name", name.mb_str(wxConvUTF8));

	// Create the general settings
	PListDict generalDict = rootArray.InsertNewDict(0);
	PListDict settings = generalDict.NewDict("settings");

	// Set default values
	settings.SetString("background", "#FFFFFF");
	settings.SetString("caret", "#000000");
	settings.SetString("foreground", "#000000");
	settings.SetString("invisibles", "#BFBFBF");
	settings.SetString("lineHighlight", "#00000012");
	settings.SetString("selection", "#C3DCFF");

	// Create and set new uuid
	const wxString uuid = GetNewUuid();
	rootDict.SetString("uuid", uuid.mb_str(wxConvUTF8));

	// Create the theme
	const unsigned int ndx = NewPlistItem(ref, PLIST_LOCAL, m_vThemes);
	MarkThemeAsModified(ndx);

	return ndx;
}

wxString PListHandler::GetNewUuid() { // static
#ifdef __WXMSW__
	GUID guid;
	BYTE* str;
	CoCreateGuid(&guid);
	UuidToStringA((UUID*)&guid, &str);

	wxString uuid((char*)str, wxConvUTF8);
	uuid.MakeUpper();

	RpcStringFreeA(&str);

	return uuid;
#else
    wxASSERT(false); // not implemented
    return wxEmptyString;
#endif
}

void PListHandler::Commit() {
	wxFileName themePath(m_appDataPath);
	themePath.AppendDir(wxT("Themes"));

	// Save any changed themes
	for (unsigned int i = 0; (int)i < m_vThemes.GetSize(); ++i) {
		const c4_RowRef rTheme = m_vThemes[i];
		if (pModified(rTheme) == 1) {
			wxASSERT(pLocality(rTheme) & PLIST_LOCAL);

			const unsigned int ref = pLocalRef(rTheme);
			if (SavePList(ref, themePath)) {
				pModified(rTheme) = 0;
			}
		}
	}

	// Mark for commit in next idle time
	m_dbChanged = true;
}

void PListHandler::Flush() {
	wxLogDebug(wxT("Flushing PList Storage"));

	// Write any changes to disk
	if (m_dbChanged) {
		m_storage.Commit();
		m_dbChanged = false;
	}
	
	// Release cached memory and rebind storage
	m_storage = c4_View();
	m_storage = c4_Storage(m_dbPath.mb_str(), true);

	// Rebind views
	m_vThemes = m_storage.GetAs(DB_THEMES_FORMAT);
	m_vBundles = m_storage.GetAs(DB_BUNDLES_FORMAT);
	m_vPlists = m_storage.GetAs(DB_PLISTS_FORMAT);
	m_vFreePlists = m_storage.GetAs(DB_FREEPLISTS_FORMAT);
	const c4_View assocs = m_storage.GetAs("assocs[ext:S,syntax:S]");
	const c4_View assocsh = m_storage.GetAs("assocs_H[_H:I,_R:I]");
	m_vSyntaxAssocs = assocs.Hash(assocsh);
}


PListDict PListHandler::GetBundleInfo(unsigned int ndx) const {
	return GetPlistItem(ndx, m_vBundles);
}

PListDict PListHandler::GetEditableBundleInfo(unsigned int bundleId) {
	return GetEditablePlistItem(bundleId, m_vBundles);
}

bool PListHandler::SaveBundle(unsigned int bundleId) {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());

	const c4_RowRef rBundle = m_vBundles[bundleId];

	// First time we save a bundle we have to create the dirname
	wxString dirName(pBundlePath(rBundle), wxConvUTF8);
	if (dirName.empty()) {
		// Get bundle name from local manifest
		const unsigned int ref = pLocalRef(rBundle);
		PListDict dict = GetPlist(ref);
		const wxString name = dict.wxGetString("name");

		// Create valid and unused dirname
		dirName = MakeValidDir(m_localBundleDir, name, wxT(".tmbundle"));
		pBundlePath(rBundle) = dirName.mb_str(wxConvUTF8);
	}

	// Build path
	wxFileName path = m_localBundleDir;
	path.AppendDir(dirName);

	// Save the manifest file
	m_dbChanged = true; // Mark for commit in next idle time
	return SavePListItem(bundleId, m_vBundles, path, wxEmptyString);
}

bool PListHandler::RestoreBundle(unsigned int bundleId) {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());
	m_dbChanged = true; // Mark for commit in next idle time

	const c4_RowRef rBundle = m_vBundles[bundleId];
	wxASSERT(pLocality(rBundle) & (PLIST_PRISTINE|PLIST_DISABLED));

	// Get paths
	wxFileName bundlePath = m_bundleDir;
	const wxString bundleName(pBundlePath(rBundle), wxConvUTF8);
	bundlePath.AppendDir(bundleName);
	wxFileName infoPath = bundlePath;
	infoPath.SetFullName(wxT("info.plist"));

	// Check if we have 'info.plist'
	int ref = -1;
	if (infoPath.FileExists()) {
		const wxString filepath = infoPath.GetFullPath();
		ref = LoadPList(filepath);
	}
	else return false; // bundle without info.plist is invalid

	pLocality(rBundle) = PLIST_PRISTINE;
	UpdatePlistItem(ref, PLIST_PRISTINE, m_vBundles, bundleId);
	UpdateBundleSubDirs(bundlePath, PLIST_PRISTINE, bundleId, UPDATE_FULL);
	return true;
}


bool PListHandler::DeleteBundle(unsigned int bundleId) {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());
	m_dbChanged = true; // Mark for commit in next idle time

	const c4_RowRef rBundle = m_vBundles[bundleId];
	int locality = pLocality(rBundle);

	// Delete all local subitems
	// Pristine are ignored so that they are still there if we
	// enable the bundle again
	DeleteBundleSubItems(bundleId, PLIST_LOCAL|PLIST_INSTALLED);

	// Mark the entire local dir for deletion
	wxArrayString delPaths;
	const wxFileName path = GetLocalBundlePath(bundleId);
	if (path.DirExists()) delPaths.Add(path.GetPath());

	// Mark the entire installed dir for deletion
	if (locality & PLIST_INSTALLED) {
		const wxString bundleName(pBundlePath(rBundle), wxConvUTF8);
		wxFileName installPath = m_installedBundleDir;
		installPath.AppendDir(bundleName);

		if (installPath.DirExists()) delPaths.Add(installPath.GetPath());
	}

	// Delete the files/dirs (thread will delete itself when done)
	if (!delPaths.IsEmpty()) {
		new FileActionThread(FileActionThread::FILEACTION_DELETE_SILENT, delPaths, false);
	}

	// Returns true if bundle was fully deleted, false if just disabled
	if (locality & PLIST_PRISTINE) {
		pLocality(rBundle) = PLIST_PRISTINE|PLIST_DISABLED;
		return false;
	}
	else {
		// Remove installed ref
		if (locality & PLIST_INSTALLED) {
			DeletePList(pPristineRef(rBundle));
			locality &= ~PLIST_INSTALLED;
			pLocality(rBundle) = locality;
		}

		SafeDeletePlistItem(bundleId, m_vBundles, path);
		return true;
	}
}

bool PListHandler::Save(BundleItemType type, unsigned int bundleId, unsigned int itemId) {
	wxASSERT((int)bundleId < m_vBundles.GetSize());

	const c4_RowRef rBundle = m_vBundles[bundleId];
	wxFileName path = GetLocalBundlePath(bundleId);
	wxString ext;
	c4_View vPlists;

	switch(type) {
	case BUNDLE_SNIPPET:
		path.AppendDir(wxT("Snippets"));
		vPlists = pSnippets(rBundle);
		ext = wxT(".tmSnippet");
		break;
	case BUNDLE_COMMAND:
		path.AppendDir(wxT("Commands"));
		vPlists = pCommands(rBundle);
		ext = wxT(".tmCommand");
		break;
	case BUNDLE_DRAGCMD:
		path.AppendDir(wxT("DragCommands"));
		vPlists = pDragCommands(rBundle);
		ext = wxT(".tmDragCommand");
		break;
	case BUNDLE_PREF:
		path.AppendDir(wxT("Preferences"));
		vPlists = pPrefs(rBundle);
		ext = wxT(".tmPreferences");
		break;
	case BUNDLE_LANGUAGE:
		path.AppendDir(wxT("Syntaxes"));
		vPlists = pSyntaxes(rBundle);
		ext = wxT(".tmLanguage");
		break;
	case BUNDLE_MACRO:
		path.AppendDir(wxT("Macros"));
		vPlists = pMacros(rBundle);
		ext = wxT(".tmMacro");
		break;
	default:
		wxASSERT(false);
	}

	wxASSERT((int)itemId < vPlists.GetSize());

	m_dbChanged = true; // Mark for commit in next idle time
	return SavePListItem(itemId, vPlists, path, ext);
}

wxFileName PListHandler::GetBundlePath(unsigned int ndx) const {
	wxASSERT((int)ndx >= 0 && (int)ndx < m_vBundles.GetSize());

	// Get the dir name
	const c4_RowRef rBundle = m_vBundles[ndx];
	const wxString dirName(pBundlePath(rBundle), wxConvUTF8);

	// If we have pristine send that, otherwise local
	wxFileName bundleDir;
	const int locality = pLocality(rBundle);
	if (locality & PLIST_INSTALLED) bundleDir = m_installedBundleDir;
	else if (locality & PLIST_PRISTINE) bundleDir = m_bundleDir;
	else bundleDir = m_localBundleDir;

	bundleDir.AppendDir(dirName);

	return bundleDir;
}

wxFileName PListHandler::GetBundleItemPath(BundleItemType type, unsigned int bundleId, unsigned int itemId) const {
	// Get base path
	wxFileName path;
	const c4_RowRef rBundle = m_vBundles[bundleId];
	const int locality = pLocality(rBundle);
	if (locality & PLIST_INSTALLED) path = m_installedBundleDir;
	else if (locality & PLIST_PRISTINE) path = m_bundleDir;
	else path = m_localBundleDir;

	// Append bundle dir name
	const wxString dirName(pBundlePath(rBundle), wxConvUTF8);
	path.AppendDir(dirName);

	// Append subdir
	c4_View vItems;
	switch(type) {
		case BUNDLE_SNIPPET:
			path.AppendDir(wxT("Snippets"));
			vItems = pSnippets(rBundle);
			break;
		case BUNDLE_COMMAND:
			path.AppendDir(wxT("Commands"));
			vItems = pCommands(rBundle);
			break;
		case BUNDLE_DRAGCMD:
			path.AppendDir(wxT("DragCommands"));
			vItems = pDragCommands(rBundle);
			break;
		case BUNDLE_PREF:
			path.AppendDir(wxT("Preferences"));
			vItems = pPrefs(rBundle);
			break;
		case BUNDLE_LANGUAGE:
			path.AppendDir(wxT("Syntaxes"));
			vItems = pSyntaxes(rBundle);
			break;
		case BUNDLE_MACRO:
			path.AppendDir(wxT("Macros"));
			vItems = pMacros(rBundle);
			break;
		default: wxASSERT(false);
	}

	// Get ref to plist
	wxASSERT((int)itemId < vItems.GetSize());
	const c4_RowRef rItem = vItems[itemId];
	const int itemLocality = pLocality(rItem);
	int ref = -1;
	if (itemLocality & PLIST_DISABLED) wxASSERT(false);
	else if (itemLocality & PLIST_LOCAL) ref = pLocalRef(rItem);
	else if (itemLocality & PLIST_PRISTINE || itemLocality & PLIST_INSTALLED) ref = pPristineRef(rItem);
	else wxASSERT(false);

	// Append item filename
	const wxString itemName(pFilename(m_vPlists[ref]), wxConvUTF8);
	path.SetFullName(itemName);

	return path;
}

bool PListHandler::ExportBundle(const wxFileName& dstPath, unsigned int bundleId) const {
	wxASSERT(dstPath.IsDir());

	// Create the bundle dir
	if (!wxMkdir(dstPath.GetFullPath())) return true;

	// Get the bundle dirName
	const c4_RowRef rItem = m_vBundles[bundleId];
	const wxString dirName(pBundlePath(rItem), wxConvUTF8);

	// Get the source path
	const int locality = pLocality(rItem);
	wxFileName infoPath;
	if (locality & PLIST_INSTALLED) infoPath = m_installedBundleDir;
	else if (locality & PLIST_PRISTINE) infoPath = m_bundleDir;
	else infoPath = m_localBundleDir;
	infoPath.AppendDir(dirName);
	infoPath.SetFullName(wxT("info.plist"));

	// Copy the info.plist
	wxFileName infoDst = dstPath;
	infoDst.SetFullName(wxT("info.plist"));
	if (!wxCopyFile(infoPath.GetFullPath(), infoDst.GetFullPath())) return false;

	map<BundleItemType, wxString> bundleTypes;
	bundleTypes[BUNDLE_SNIPPET]  = wxT("Snippets");
	bundleTypes[BUNDLE_COMMAND]  = wxT("Commands");
	bundleTypes[BUNDLE_DRAGCMD]  = wxT("DragCommands");
	bundleTypes[BUNDLE_PREF]     = wxT("Preferences");
	bundleTypes[BUNDLE_LANGUAGE] = wxT("Syntaxes");
	bundleTypes[BUNDLE_MACRO]    = wxT("Macros");

	// Copy subdirs
	for (map<BundleItemType, wxString>::const_iterator p = bundleTypes.begin(); p != bundleTypes.end(); ++p) {
		const vector<unsigned int> items = GetList(p->first, bundleId);
		if (items.empty()) continue;

		// Create the subdir
		wxFileName path = dstPath;
		path.AppendDir(p->second);
		if (!wxMkdir(path.GetFullPath())) return false;

		// Copy items
		for (unsigned int i = 0; i < items.size(); ++i) {
			const unsigned int itemId = items[i];
			const wxFileName itemPath = GetBundleItemPath(p->first, bundleId, itemId);
			path.SetFullName(itemPath.GetFullName());
			if (!ExportBundleItem(path, p->first, bundleId, itemId)) return false;
		}
	}

	// Copy support dir
	const wxFileName supportDir = GetBundleSupportPath(bundleId);
	if (supportDir.IsOk()) {
		wxArrayString paths;
		paths.Add(supportDir.GetPath());
		wxFileName target = dstPath;
		target.AppendDir(wxT("Support"));
		FileActionThread::DoFileAction(FileActionThread::FILEACTION_COPY, paths, target.GetPath());
	}

	return true;
}

bool PListHandler::ExportBundleItem(const wxFileName& dstPath, BundleItemType type, unsigned int bundleId, unsigned int itemId) const {
	wxASSERT(!dstPath.IsDir());

	const wxFileName srcPath = GetBundleItemPath(type, bundleId, itemId);
	if (!srcPath.IsOk()) return false;

	return wxCopyFile(srcPath.GetFullPath(), dstPath.GetFullPath());
}

wxFileName PListHandler::GetBundleSupportPath(unsigned int bundleId) const {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());

	// Get the dir name
	const c4_RowRef rBundle = m_vBundles[bundleId];
	const wxString dirName(pBundlePath(rBundle), wxConvUTF8);

	wxFileName supportDir;

	// Local support dir has highest priority
	const int locality = pLocality(rBundle);
	if (locality & PLIST_LOCAL) {
		supportDir = m_localBundleDir;
		supportDir.AppendDir(dirName);
		supportDir.AppendDir(wxT("Support"));
		if (supportDir.DirExists()) return supportDir;
	}

	// Else send pristine (if available)
	supportDir = (locality & PLIST_INSTALLED) ? m_installedBundleDir : m_bundleDir; // installed overrides pristine
	supportDir.AppendDir(dirName);
	supportDir.AppendDir(wxT("Support"));
	if (supportDir.DirExists()) return supportDir;

	// No support dir
	return wxFileName();
}

wxFileName PListHandler::GetLocalBundlePath(unsigned int bundleId) {
	wxASSERT((int)bundleId >= 0 && (int)bundleId < m_vBundles.GetSize());

	// Get the bundle dir name
	const c4_RowRef rBundle = m_vBundles[bundleId];
	const wxString bundleName(pBundlePath(rBundle), wxConvUTF8);

	// Build path
	wxFileName path = m_localBundleDir;
	path.AppendDir(bundleName);

	return path;
}

PListDict PListHandler::GetTheme(unsigned int ndx) const {
	return GetPlistItem(ndx, m_vThemes);
}

PListDict PListHandler::GetPlistItem(unsigned int ndx, c4_View vList) const {
	wxASSERT((int)ndx >= 0 && (int)ndx < vList.GetSize());

	const c4_RowRef rItem = vList[ndx];
	const int locality = pLocality(rItem);

	if (locality & PLIST_DISABLED) return PListDict();

	if (locality & PLIST_LOCAL) {
		const unsigned int ref = pLocalRef(rItem);
		return GetPlist(ref);
	}
	
	if (locality & PLIST_PRISTINE || locality & PLIST_INSTALLED) {
		const unsigned int ref = pPristineRef(rItem);
		return GetPlist(ref);
	}

	wxASSERT(false);
	return PListDict();
}

PListDict PListHandler::GetEditablePlistItem(unsigned int ndx, c4_View vList) {
	wxASSERT((int)ndx >= 0 && (int)ndx < vList.GetSize());

	const c4_RowRef rItem = vList[ndx];
	const int locality = pLocality(rItem);

	if (locality & PLIST_LOCAL) return GetPlist(pLocalRef(rItem));

	wxASSERT(locality & PLIST_PRISTINE || locality & PLIST_INSTALLED);

	// We have to make an editable copy
	const unsigned int prisRef = pPristineRef(rItem);
	const unsigned int locRef = CopyPlist(prisRef);

	// Update theme entry
	pLocalRef(rItem) = locRef;
	pLocality(rItem) = locality | PLIST_LOCAL;

	return GetPlist(locRef);
}

bool PListHandler::GetTheme(unsigned int ndx, PListDict& theme) const {
	wxASSERT((int)ndx >= 0 && (int)ndx < m_vThemes.GetSize());

	const c4_RowRef rTheme = m_vThemes[ndx];
	const int locality = pLocality(rTheme);

	if (locality & PLIST_DISABLED) return false;

	if (locality & PLIST_LOCAL) GetPlist(pLocalRef(rTheme), theme);
	else if (locality & PLIST_PRISTINE) GetPlist(pPristineRef(rTheme), theme);
	else return false;

	return true;
}

int PListHandler::GetThemeFromUuid(const char* key) const {
	return m_vThemes.Find(pUuid[key]);
}

void PListHandler::GetEditableTheme(unsigned int ndx, PListDict& themeDict) {
	wxASSERT((int)ndx < m_vThemes.GetSize());

	const c4_RowRef rTheme = m_vThemes[ndx];
	const int locality = pLocality(rTheme);

	if (locality & PLIST_LOCAL) GetPlist(pLocalRef(rTheme), themeDict);
	else {
		wxASSERT(locality & PLIST_PRISTINE);

		// We have to make an editable copy
		const unsigned int prisRef = pPristineRef(rTheme);
		const unsigned int locRef = CopyPlist(prisRef);

		// Update theme entry
		pLocalRef(rTheme) = locRef;
		pLocality(rTheme) = locality | PLIST_LOCAL;

		GetPlist(locRef, themeDict);
	}
}

void PListHandler::DeleteTheme(unsigned int ndx) {
	wxASSERT((int)ndx < m_vThemes.GetSize());

	// Get the local path
	wxFileName path = m_appDataPath;
	path.AppendDir(wxT("Themes"));

	SafeDeletePlistItem(ndx, m_vThemes, path);
}

// Deletes local plist if available, but pristine is just marked as disabled
void PListHandler::SafeDeletePlistItem(unsigned int ndx, c4_View vList, const wxFileName localPath) {
	wxASSERT((int)ndx < vList.GetSize());

	c4_RowRef rItem = vList[ndx];
	int locality = pLocality(rItem);

	// See if we should delete the local copy
	if (locality & PLIST_LOCAL) {
		const unsigned int ref = pLocalRef(rItem);

		// Delete plist file on disk
		const char* filename = pFilename(m_vPlists[ref]);
		wxFileName path = localPath;
		path.SetFullName(wxString(filename, wxConvUTF8));
		if (path.FileExists()) wxRemoveFile(path.GetFullPath());

		// Delete plist file in db
		DeletePList(ref);
		locality &= ~PLIST_LOCAL;
	}

	// We can not delete the pristine plist
	// but we can mark it as disabled
	if (locality & PLIST_PRISTINE || locality & PLIST_INSTALLED) {
		pLocality(rItem) = locality|PLIST_DISABLED;
	}
	else {
		if ((int)ndx == vList.GetSize()-1) {
			// Last item can just be deleted, it won't cause any reordering
			vList.RemoveAt(ndx);

			// Any deleted items above last can now be removed as well
			while (ndx > 0) {
				--ndx;
				if (pLocality(vList[ndx]) == PLIST_DELETED) vList.RemoveAt(ndx);
				else break;
			}
		}
		else {
			rItem = c4_Row(); // clear entry
			pLocality(rItem) = PLIST_DELETED; // Mark as deleted
		}
	}
}

bool PListHandler::IsThemeEditable(unsigned int ndx) const {
	wxASSERT((int)ndx < m_vThemes.GetSize());

	const c4_RowRef rTheme = m_vThemes[ndx];
	const int locality = pLocality(rTheme);

	if (locality & PLIST_LOCAL) return true;
	else return false;
}

void PListHandler::SaveTheme(unsigned int ndx) {
	wxASSERT((int)ndx < m_vThemes.GetSize());

	const c4_RowRef rTheme = m_vThemes[ndx];
	const int locality = pLocality(rTheme);

	// Only local themes can be saved
	if (locality & PLIST_LOCAL) {
		const int plistNdx = pLocalRef(rTheme);

		wxFileName path(m_appDataPath);
		path.AppendDir(wxT("Themes"));

		SavePList(plistNdx, path);
	}

	m_dbChanged = true; // Mark for commit in next idle time
}

void PListHandler::MarkThemeAsModified(unsigned int ndx) {
	MarkPlistItemAsModified(ndx, m_vThemes);
}

void PListHandler::MarkPlistItemAsModified(unsigned int ndx, c4_View vList) {
	wxASSERT((int)ndx < vList.GetSize());

	const c4_RowRef rPlistItem = vList[ndx];
	wxASSERT(pLocality(rPlistItem) & PLIST_LOCAL);

	pModified(rPlistItem) = 1;
}

PListDict PListHandler::GetPlist(unsigned int ndx) const {
	wxASSERT((int)ndx < m_vPlists.GetSize());

	c4_RowRef rPlist = m_vPlists[ndx];

	// First dict is root
	c4_View vDicts = pDicts(rPlist);
	c4_RowRef rDict = vDicts[0];

	// Get the hash
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	return PListDict(vDict, rPlist);
}

void PListHandler::GetPlist(unsigned int ndx, PListDict& plist) const {
	wxASSERT((int)ndx < m_vPlists.GetSize());

	c4_RowRef rPlist = m_vPlists[ndx];

	// First dict is root
	c4_View vDicts = pDicts(rPlist);
	c4_RowRef rDict = vDicts[0];

	// Get the hash
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	plist.SetDict(vDict, rPlist);
}

PListArray PListHandler::GetPlistArray(unsigned int plistId, unsigned int arrayId) const {
	wxASSERT((int)plistId < m_vPlists.GetSize());

	c4_RowRef rPlist = m_vPlists[plistId];

	c4_View vArrays = pArrays(rPlist);
	c4_View vArray = pArray(vArrays[arrayId]);

	return PListArray(vArray, rPlist);
}

unsigned int PListHandler::CopyPlist(unsigned int ndx) {
	wxASSERT((int)ndx < m_vPlists.GetSize());

	// Get the new plist index
	unsigned int ref;
	if(m_vFreePlists.GetSize()) {
		ref = pRef(m_vFreePlists[0]);
		m_vFreePlists.RemoveAt(0);
	}
	else ref = m_vPlists.Add(c4_Row());
	c4_RowRef rPlist = m_vPlists[ref];

	// Copy the plist
	rPlist = m_vPlists[ndx];

	// Set moddate the zero to indicate that it is changed
	pModDate(rPlist) = 0;

	return ref;
}

int PListHandler::LoadPList(const wxString& path) {
	// We have to open the file manually to allow
	// filenames with unicode chars
	wxFFile file(path, wxT("rb"));
	if (!file.IsOpened()) return -1;

	// Load xml document
	TiXmlDocument doc;
	if (!doc.LoadFile(file.fp())) {
#ifdef __WXDEBUG__
		const char * error = doc.ErrorDesc();
		const int row = doc.ErrorRow();
		const int col = doc.ErrorCol();
		const wxString errorStr(error, wxConvUTF8);
		wxLogDebug(wxT("Load of plist failed: %s"), path.c_str());
		wxLogDebug(wxT("%d,%d - %s"), row, col, errorStr.c_str());
#endif
		return -1;
	}

	const TiXmlHandle docHandle(&doc);

	// TODO: Verify that this is a valid plist

	// Create the plist
	int ref;
	if(m_vFreePlists.GetSize()) {
		ref = pRef(m_vFreePlists[0]);
		m_vFreePlists.RemoveAt(0);
	}
	else ref = m_vPlists.Add(c4_Row());
	c4_RowRef rPlist = m_vPlists[ref];

	// Set path and modDate
	const wxString filename = path.AfterLast(wxFileName::GetPathSeparator());
	wxDateTime modDate = wxFileName(path).GetModificationTime();
	modDate.SetMillisecond(0); // milliseconds are not stable between checks
	pFilename(rPlist) = filename.mb_str(wxConvUTF8);
	pModDate(rPlist) = modDate.GetValue().GetValue();

	// Parse sub-elements
	const TiXmlElement* child = docHandle.FirstChildElement("plist").FirstChildElement().Element();
	if (!child) goto error; // empty plist

	if (strcmp(child->Value(), "dict") == 0) {
		if (child->NoChildren()) return -1;

		const int dictRef = LoadDict(child, rPlist);
		if (dictRef == -1) goto error;

		return ref;
	}

	wxASSERT(false); // we only support toplevel dicts
	return -1;

error:
	DeletePList(ref);
	return -1;
}

bool PListHandler::SavePListItem(unsigned int ndx, c4_View vList, const wxFileName& path, const wxString& ext) {
	wxASSERT((int)ndx >= 0 && (int)ndx < vList.GetSize());

	const c4_RowRef rItem = vList[ndx];

	// Only local items can be saved
	if (!(pLocality(rItem) & PLIST_LOCAL)) {
		wxFAIL_MSG(wxT("Trying to save pristine plist"));
		return false;
	}

	// First time we save we need to make a filename for the plist
	const unsigned int ref = pLocalRef(rItem);
	c4_RowRef rPlist = m_vPlists[ref];
	if (pFilename(rPlist).GetSize() == 0) {
		// Get name from plist
		PListDict dict = GetPlist(ref);
		const wxString name = dict.wxGetString("name");

		// Create valid and unused filename
		const wxString filename = MakeValidFilename(path, name, ext);
		pFilename(rPlist) = filename.mb_str(wxConvUTF8);
	}

	return SavePList(ref, path);
}

bool PListHandler::SavePList(unsigned int ndx, const wxFileName& path) {
	wxASSERT((int)ndx < m_vPlists.GetSize());
	wxASSERT(path.IsOk() && path.IsDir());

	// Build document
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
	doc.LinkEndChild(decl);

	TiXmlUnknown* dt = new TiXmlUnknown();
	dt->SetValue("!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
	doc.LinkEndChild(dt);
	TiXmlElement* plist = new TiXmlElement( "plist" );
	plist->SetAttribute("version", "1.0");
	doc.LinkEndChild(plist);

	// First dict is root
	c4_RowRef rPlist = m_vPlists[ndx];
	SaveDict(plist, 0, rPlist);

	// Check if the dir exists
	if (!path.DirExists()) {
		wxFileName::Mkdir(path.GetPath(), 0777, wxPATH_MKDIR_FULL);
	}

	// Get filename
	const char* fname = pFilename(rPlist);
	wxString filename(fname, wxConvUTF8);
	wxASSERT(!filename.empty());

	// Build path
	wxFileName fullpath(path);
	fullpath.SetFullName(filename);

	// Save the file
	wxFFile tempfile(fullpath.GetFullPath(), wxT("wb"));
	if (!tempfile.IsOpened() || !doc.SaveFile(tempfile.fp())) return false;

	// Update mod date
	//const wxDateTime modDate(wxFileModificationTime(pathStr));
	wxDateTime modDate = fullpath.GetModificationTime();
	modDate.SetMillisecond(0); // milliseconds are not stable between checks
	pModDate(rPlist) = modDate.GetValue().GetValue();

	return true;
}

void PListHandler::DeletePList(unsigned int ndx) {
	wxASSERT((int)ndx < m_vPlists.GetSize());

	// Clear the entry
	m_vPlists[ndx] = c4_Row();

	// Add to free list
	m_vFreePlists.Add(pRef[ndx]);
}

int PListHandler::LoadDict(const TiXmlElement* dict, c4_RowRef& rPlist) {
	wxASSERT(strcmp(dict->Value(), "dict") == 0);

	// Create the dict
	c4_View vDicts = pDicts(rPlist);
	const int vref = vDicts.Add(c4_Row());
	c4_RowRef rDict = vDicts[vref];

	// Get the hash
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	const TiXmlElement* child = dict->FirstChildElement();
	for(; child; child = child->NextSiblingElement() )
	{
		if (strcmp(child->Value(), "key") != 0) return -1; // invalid dict
		const TiXmlElement* const value = child->NextSiblingElement();
		if (!value) return -1; // invalid dict

		const char* valType = value->Value();

		c4_Row rRef;
		pKey(rRef) = child->GetText();

		if (strcmp(valType, "string") == 0) {
			pRefType(rRef) = REF_STRING;
			pRef(rRef) = LoadString(value->GetText(), rPlist);
		}
		else if (strcmp(valType, "dict") == 0) {
			const int ref = LoadDict(value, rPlist);
			if (ref == -1) return -1;

			pRefType(rRef) = REF_DICT;
			pRef(rRef) = ref;
		}
		else if (strcmp(valType, "array") == 0) {
			const int ref = LoadArray(value, rPlist);
			if (ref == -1) return -1;

			pRefType(rRef) = REF_ARRAY;
			pRef(rRef) = ref;
		}
		else if (strcmp(valType, "integer") == 0) {
			const char* val = value->GetText();
			if (!val) return -1;

			pRefType(rRef) = REF_INTEGER;
			pRef(rRef) = atoi(val);
		}
		else if (strcmp(valType, "true") == 0) {
			pRefType(rRef) = REF_BOOL;
			pRef(rRef) = 1;
		}
		else if (strcmp(valType, "false") == 0) {
			pRefType(rRef) = REF_BOOL;
			pRef(rRef) = 0;
		}
		else {
#ifdef __WXDEBUG__
			const wxString msg = wxString::Format(wxT("Invalid key: %s in plist"), wxString(valType, wxConvUTF8).c_str());
			wxFAIL_MSG(msg);
#endif  //__WXDEBUG__

			// Ignore unknown types
			child = value;
			continue;
		}

		vDict.Add(rRef);

		child = value;
	}

	return vref;
}

void PListHandler::SaveDict(TiXmlElement* parent, unsigned int ndx, const c4_RowRef& rPlist) const {
	wxASSERT(parent);

	// Build Dict
	TiXmlElement* dict = new TiXmlElement("dict");
	parent->LinkEndChild(dict);

	const c4_View vDicts = pDicts(rPlist);
	wxASSERT((int)ndx < vDicts.GetSize());
	c4_RowRef rDict = vDicts[ndx];

	// Get the hash
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	for (unsigned int i = 0; (int)i < vDict.GetSize(); ++i) {
		const c4_RowRef r = vDict[i];

		// Add key
		TiXmlElement* key= new TiXmlElement("key");
		dict->LinkEndChild(key);
		TiXmlText* text = new TiXmlText(pKey(r));
		key->LinkEndChild(text);

		// Add value
		const unsigned int ref = pRef(r);
		switch (pRefType(r)) {
		case REF_STRING:
			SaveString(dict, ref, rPlist);
			break;

		case REF_DICT:
			SaveDict(dict, ref, rPlist);
			break;

		case REF_ARRAY:
			SaveArray(dict, ref, rPlist);
			break;

		case REF_BOOL:
			if (ref == 0) {
				TiXmlElement* str = new TiXmlElement("false");
				dict->LinkEndChild(str);
			}
			else {
				wxASSERT_MSG(ref == 1, wxT("Invalid bool in plist db"));

				TiXmlElement* str = new TiXmlElement("true");
				dict->LinkEndChild(str);
			}
			break;

		case REF_INTEGER:
			{
				TiXmlElement* integer = new TiXmlElement("integer");
				dict->LinkEndChild(integer);

				const wxString strVal = wxString::Format(wxT("%d"), ref);
				TiXmlText* value = new TiXmlText(strVal.mb_str(wxConvUTF8));
				integer->LinkEndChild(value);
			}
			break;

		default:
#ifdef __WXDEBUG__
			const wxString msg = wxString::Format(wxT("Unknown type: %d in plist db"), (int)pRefType(r));
			wxFAIL_MSG(msg);
#endif  //__WXDEBUG__
			break;
		}
	}
}

int PListHandler::LoadArray(const TiXmlElement* array, c4_RowRef& rPlist) {
	wxASSERT(strcmp(array->Value(), "array") == 0);

	// Create the array
	c4_View vArrays = pArrays(rPlist);
	const int vref = vArrays.Add(c4_Row());
	c4_View vArray = pArray(vArrays[vref]);

	const TiXmlElement* child = array->FirstChildElement();
	for(; child; child = child->NextSiblingElement() )
	{
		c4_Row rRef;
		const char* valType = child->Value();

		if (strcmp(valType, "string") == 0) {
			pRefType(rRef) = REF_STRING;
			pRef(rRef) = LoadString(child->GetText(), rPlist);
		}
		else if (strcmp(valType, "dict") == 0) {
			const int ref = LoadDict(child, rPlist);
			if (ref == -1) return -1;

			pRefType(rRef) = REF_DICT;
			pRef(rRef) = ref;
		}
		else if (strcmp(valType, "array") == 0) {
			const int ref = LoadArray(child, rPlist);
			if (ref == -1) return -1;

			pRefType(rRef) = REF_ARRAY;
			pRef(rRef) = ref;
		}
		else wxASSERT(false);

		vArray.Add(rRef);
	}

	return vref;
}

void PListHandler::SaveArray(TiXmlElement* parent, unsigned int ndx, const c4_RowRef& rPlist) const {
	wxASSERT(parent);

	// Build Array
	TiXmlElement* array = new TiXmlElement("array");
	parent->LinkEndChild(array);

	const c4_View vArrays = pArrays(rPlist);
	wxASSERT((int)ndx < vArrays.GetSize());
	const c4_View vArray = pArray(vArrays[ndx]);

	for (unsigned int i = 0; (int)i < vArray.GetSize(); ++i) {
		const c4_RowRef r = vArray[i];

		// Add value
		const unsigned int ref = pRef(r);
		switch (pRefType(r)) {
		case REF_STRING:
			SaveString(array, ref, rPlist);
			break;

		case REF_DICT:
			SaveDict(array, ref, rPlist);
			break;

		case REF_ARRAY:
			SaveArray(array, ref, rPlist);
			break;

		default:
			wxASSERT(false); // unknown type
		}
	}
}

int PListHandler::LoadString(const char* str, c4_RowRef& rPlist) {
	c4_View vStrings = pStrings(rPlist);
	return vStrings.Add(pString[ str ? str : "" ]);
}

void PListHandler::SaveString(TiXmlElement* parent, unsigned int ndx, const c4_RowRef& rPlist) const {
	wxASSERT(parent);

	c4_View vStrings = pStrings(rPlist);
	wxASSERT((int)ndx < vStrings.GetSize());

	TiXmlElement* str = new TiXmlElement("string");
	parent->LinkEndChild(str);

	TiXmlText* text = new TiXmlText(pString(vStrings[ndx]));
	str->LinkEndChild(text);
}

bool PListHandler::BundleCmp::operator()(unsigned int ndx1, unsigned int ndx2) {
	return strcmp(pBundleName(m_view[ndx1]), pBundleName(m_view[ndx2])) < 0;
}

// ---- PListItem ----------------------------------------------------------------

wxDateTime PListItem::GetModDate() const {
	return wxDateTime((const wxLongLong)pModDate(*m_rPlist));
}

void PListItem::DeleteDict(unsigned int ndx) {
	wxASSERT((int)ndx < m_vDicts.GetSize());

	// Get the hash
	c4_RowRef rDict = m_vDicts[ndx];
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	// Delete items
	for (unsigned int i = 0; (int)i < vDict.GetSize(); ++i) {
		const c4_RowRef rItem = vDict[i];
		const unsigned int ref = pRef(rItem);

		// Delete the target
		switch (pType(rItem)) {
		case REF_DICT:
			DeleteDict(ref);
			break;
		case REF_ARRAY:
			DeleteArray(ref);
			break;
		case REF_STRING:
			((c4_View)pFreeStrings(*m_rPlist)).Add(pRef[ref]);
			m_vStrings[ref] = c4_Row();
			break;
		}
	}

	// Clear dict and add to free-list
	((c4_View)pFreeDicts(*m_rPlist)).Add(pRef[ndx]);
	m_vDicts[ndx] = c4_Row();
}

void PListItem::DeleteArray(unsigned int ndx) {
	wxASSERT((int)ndx < m_vArrays.GetSize());

	// Get the array
	c4_View vArray = pArray(m_vArrays[ndx]);

	// Delete items
	for (unsigned int i = 0; (int)i < vArray.GetSize(); ++i) {
		const c4_RowRef rItem = vArray[i];
		const unsigned int ref = pRef(rItem);

		// Delete the target
		switch (pType(rItem)) {
		case REF_DICT:
			DeleteDict(ref);
			break;
		case REF_ARRAY:
			DeleteArray(ref);
			break;
		case REF_STRING:
			((c4_View)pFreeStrings(*m_rPlist)).Add(pRef[ref]);
			m_vStrings[ref] = c4_Row();
			break;
		}
	}

	// Clear array and add to free-list
	((c4_View)pFreeArrays(*m_rPlist)).Add(pRef[ndx]);
	m_vArrays[ndx] = c4_Row();
}

// ---- PListDict ----------------------------------------------------------------

PListDict::PListDict(c4_View& dict, c4_RowRef& plist)
: PListItem(plist), m_vDict(dict) {
	m_vStrings = pStrings(*m_rPlist);
	m_vDicts = pDicts(*m_rPlist);
	m_vArrays = pArrays(*m_rPlist);
}

PListDict::PListDict(const PListDict& dict) {
	SetDict(dict.m_vDict, *dict.m_rPlist);
}

void PListDict::operator=(const PListDict& dict) {
	SetDict(dict.m_vDict, *dict.m_rPlist);
}

void PListDict::SetDict(const c4_View& dict, const c4_RowRef& plist) {
	m_vDict = dict;

	if (m_rPlist) delete m_rPlist;
	m_rPlist = new c4_RowRef(plist);

	m_vStrings = pStrings(*m_rPlist);
	m_vDicts = pDicts(*m_rPlist);
	m_vArrays = pArrays(*m_rPlist);
}

unsigned int PListDict::GetSize() const {
	wxASSERT(m_rPlist);
	return m_vDict.GetSize();
}

const char* PListDict::GetKey(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vDict.GetSize());

	return pKey(m_vDict[ndx]);
}

bool PListDict::HasKey(const char* key) const {
	wxASSERT(m_rPlist);

	return (m_vDict.Find(pKey[key]) != -1);
}

const char* PListDict::GetString(const char* key) const {
	wxASSERT(m_rPlist);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) return NULL;

	const c4_RowRef rItem = m_vDict[ndx];
	if (pType(rItem) != REF_STRING) return NULL;

	const int ref = pRef(rItem);
	return pString(m_vStrings[ref]);
}

wxString PListDict::wxGetString(const char* key) const {
	const char* str = GetString(key);
	if (str) return wxString(str, wxConvUTF8);
	else return wxEmptyString;
}

bool PListDict::GetDict(const char* key, PListDict& dict) const {
	wxASSERT(m_rPlist);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) return false;

	const c4_RowRef rItem = m_vDict[ndx];
	if (pType(rItem) != REF_DICT) return false;

	// Get the dict
	const int ref = pRef(rItem);
	c4_RowRef rDict = m_vDicts[ref];

	// Get the hash
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	dict.SetDict(vDict, *m_rPlist);
	return true;
}

bool PListDict::GetArray(const char* key, PListArray& array) const {
	wxASSERT(m_rPlist);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) return false;

	const c4_RowRef rItem = m_vDict[ndx];
	if (pType(rItem) != REF_ARRAY) return false;

	const int ref = pRef(rItem);
	c4_View vArray = pArray(m_vArrays[ref]);
	array.SetArray(vArray, *m_rPlist);
	return true;
}

bool PListDict::GetInteger(const char* key, int& value) const {
	wxASSERT(m_rPlist);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) return false;

	const c4_RowRef rItem = m_vDict[ndx];
	if (pType(rItem) != REF_INTEGER) return false;

	value = pRef(rItem);
	return true;
}

bool PListDict::GetBool(const char* key) const {
	wxASSERT(m_rPlist);
	// defaults to false on error

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) return false;

	const c4_RowRef rItem = m_vDict[ndx];
	if (pType(rItem) != REF_BOOL && pType(rItem) != REF_INTEGER) return false;

	const int value = pRef(rItem);
	return (value != 0);
}

void PListDict::DeleteItem(const char* key) {
	wxASSERT(m_rPlist);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) return;
	const c4_RowRef rItem = m_vDict[ndx];
	const int ref = pRef(rItem);

	// Delete the target
	switch (pType(rItem)) {
	case REF_DICT:
		DeleteDict(ref);
		break;
	case REF_ARRAY:
		DeleteArray(ref);
		break;
	case REF_STRING:
		((c4_View)pFreeStrings(*m_rPlist)).Add(pRef[ref]);
		m_vStrings[ref] = c4_Row();
		break;
	}

	// Delete the item
	m_vDict.RemoveAt(ndx);
}

void PListDict::SetBool(const char* key, bool value) {
	wxASSERT(m_rPlist);
	wxASSERT(key);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) {
		c4_Row rRef;
		pKey(rRef) = key;
		pRefType(rRef) = REF_BOOL;
		pRef(rRef) =  value ? 1 : 0;
		m_vDict.Add(rRef);
	}
	else {
		const c4_RowRef rItem = m_vDict[ndx];
		wxASSERT(pRefType(rItem) == REF_BOOL);
		pRef(rItem) = value ? 1 : 0;
	}
}

void PListDict::SetInt(const char* key, int value) {
	wxASSERT(m_rPlist);
	wxASSERT(key);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) {
		c4_Row rRef;
		pKey(rRef) = key;
		pRefType(rRef) = REF_INTEGER;
		pRef(rRef) = value;
		m_vDict.Add(rRef);
	}
	else {
		const c4_RowRef rItem = m_vDict[ndx];
		wxASSERT(pRefType(rItem) == REF_INTEGER);
		pRef(rItem) = value;
	}
}

void PListDict::SetString(const char* key, const char* text) {
	wxASSERT(m_rPlist);
	wxASSERT(key && text);

	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx == -1) {
		const int ref = m_vStrings.Add(pString[text]);

		c4_Row rRef;
		pKey(rRef) = key;
		pRefType(rRef) = REF_STRING;
		pRef(rRef) = ref;
		m_vDict.Add(rRef);
	}
	else {
		const c4_RowRef rItem = m_vDict[ndx];
		wxASSERT(pRefType(rItem) == REF_STRING);
		const int ref = pRef(rItem);

		if (strcmp(text, (const char*)pString(m_vStrings[ref])) != 0) {
			pString(m_vStrings[ref]) = text;
		}
	}
}

void PListDict::wxSetString(const char* key, const wxString& text) {
	SetString(key, text.mb_str(wxConvUTF8));
}

void PListDict::wxUpdateString(const char *key, const wxString& value) {
	if (value.empty()) this->DeleteItem(key);
	else this->wxSetString(key, value);
}

PListDict PListDict::NewDict(const char* key) {
	wxASSERT(m_rPlist);
	wxASSERT(key);

	int ref = -1;

	// Check if the dict already exists
	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx != -1) {
		const c4_RowRef rItem = m_vDict[ndx];
		wxASSERT(pType(rItem) == REF_DICT); // TODO: replace entry

		ref = pRef(rItem);
	}
	else {
		// Create a new empty dict
		ref = m_vDicts.Add(c4_Row());

		// Create the new dict entry
		c4_Row rRef;
		pKey(rRef) = key;
		pRefType(rRef) = REF_DICT;
		pRef(rRef) = ref;

		// Insert ref in dict
		m_vDict.Add(rRef);
	}

	// Get the dict
	c4_RowRef rDict = m_vDicts[ref];
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	return PListDict(vDict, *m_rPlist);
}

PListArray PListDict::NewArray(const char* key) {
	wxASSERT(m_rPlist);
	wxASSERT(key);

	int ref = -1;

	// Check if the array already exists
	const int ndx = m_vDict.Find(pKey[key]);
	if (ndx != -1) {
		const c4_RowRef rItem = m_vDict[ndx];
		wxASSERT(pType(rItem) == REF_ARRAY); // TODO: replace entry

		ref = pRef(rItem);
	}
	else {
		// Create a new empty array
		ref = m_vArrays.Add(c4_Row());

		// Create the new array entry
		c4_Row rRef;
		pKey(rRef) = key;
		pRefType(rRef) = REF_ARRAY;
		pRef(rRef) = ref;

		// Insert ref in dict
		m_vDict.Add(rRef);
	}

	c4_View vArray = pArray(m_vArrays[ref]);
	return PListArray(vArray, *m_rPlist);
}

#ifdef __WXDEBUG__
void PListDict::Print() const {
	wxJSONValue root = GetJSONDict();

	wxJSONWriter writer(wxJSONWRITER_STYLED|wxJSONWRITER_MULTILINE_STRING);
	wxString str;
	writer.Write( root, str );

	wxLogDebug(str);
}
#endif

wxString PListDict::GetJSON(bool strip) const {
	wxJSONValue root = GetJSONDict();

	if (strip) {
		root.Remove(wxT("uuid"));
		root.Remove(wxT("scope"));
		root.Remove(wxT("keyEquivalent"));
	}

	// Write JSON object to string
	// Adam V: BUG: If we are getting the JSON for a Syntax, then we really ought to be
	// ordering the keys in the dicts in a rational matter
	// Capture numbers, for instance, should be ordered by int value.
	// This order should also be in effect when serializing JSON back to PList format for
	// Syntax items, so that the diffs don't get spammed.

	wxJSONWriter writer(wxJSONWRITER_STYLED|wxJSONWRITER_MULTILINE_STRING);
	wxString str;
	writer.Write( root, str );

	return str;
}

wxJSONValue PListDict::GetJSONDict() const {
	wxJSONValue root;

	for (int i = 0; i < m_vDict.GetSize(); ++i) {
		const c4_RowRef rItem = m_vDict[i];
		const wxString key(pKey(rItem), wxConvUTF8);
		int ref = pRef(rItem);

		switch (pType(rItem)) {
		case REF_DICT:
			{
				c4_RowRef rDict = m_vDicts[ref];
				c4_View vSourceDict = pDict(rDict);
				c4_View vHashDict = pDictH(rDict);
				c4_View vDict = vSourceDict.Hash(vHashDict);
				PListDict dict(vDict, *m_rPlist);
				root[key] = dict.GetJSONDict();
			}
			break;
		case REF_ARRAY:
			{
				c4_View vArray = pArray(m_vArrays[ref]);
				PListArray array(vArray, *m_rPlist);
				root[key] = array.GetJSONArray();
			}
			break;
		case REF_STRING:
			{
				const char* str = pString(m_vStrings[ref]);
				if (str) root[key] = wxString(str, wxConvUTF8);
				else root[key] = wxEmptyString;
			}
			break;
		case REF_INTEGER:
			root[key] = ref;
			break;
		case REF_BOOL:
			root[key] = (ref != 0);
			break;
		}
	}

	return root;
}

bool PListDict::SetJSON(wxJSONValue& root, bool strip) {
	// Strip vars that would overwrite internal settings
	if (strip) {
		root.Remove(wxT("uuid"));
		root.Remove(wxT("scope"));
		root.Remove(wxT("keyEquivalent"));
	}

	// Delete old contents
	int i = 0;
	while (i < m_vDict.GetSize()) {
		const char* key = pKey(m_vDict[i]);
		if (!strip || (strcmp(key, "uuid") != 0
			        && strcmp(key, "scope") != 0
			        && strcmp(key, "keyEquivalent") != 0))
		{
			DeleteItem(key);
		}
		else ++i;
	}

	// Set values from JSON string
	InsertJSONValues(root);

	return true;
}

void PListDict::InsertJSONValues(const wxJSONValue& value) {
	wxASSERT(value.IsObject());

	// Set values from JSON string
	const wxArrayString items = value.GetMemberNames();
	for (unsigned int i = 0; i < items.GetCount(); ++i) {
		const wxString& keyStr = items[i];
		const wxJSONValue item = value.ItemAt(keyStr);

		// Get key as utf8
		const wxCharBuffer keybuf = keyStr.mb_str(wxConvUTF8);
		const char* key = keybuf.data();

		switch(item.GetType()) {
		case wxJSONTYPE_OBJECT:
			{
				PListDict dict = NewDict(key);
				dict.InsertJSONValues(item);
			}
			break;
		case wxJSONTYPE_ARRAY:
			{
				PListArray array = NewArray(key);
				array.InsertJSONValues(item);
			}
			break;
		case wxJSONTYPE_STRING:
			wxSetString(key, item.AsString());
			break;
		case wxJSONTYPE_INT:
		case wxJSONTYPE_UINT:
			m_vDict.Add(pKey[key] + pType[REF_INTEGER] + pRef[item.AsInt()]);
			break;
		case wxJSONTYPE_BOOL:
			if (item.AsBool()) m_vDict.Add(pKey[key] + pType[REF_BOOL] + pRef[1]);
			else m_vDict.Add(pKey[key] + pType[REF_BOOL] + pRef[0]);
			break;
        default:
            wxASSERT(false); // not handled
		}
	}
}

// ---- PListArray ----------------------------------------------------------------

PListArray::PListArray(c4_View& array, c4_RowRef& plist)
: PListItem(plist), m_vArray(array) {
	m_vStrings = pStrings(*m_rPlist);
	m_vDicts = pDicts(*m_rPlist);
	m_vArrays = pArrays(*m_rPlist);
}

bool PListArray::IsBool(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	return pType(rItem) == REF_BOOL;
}

bool PListArray::IsInt(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	return pType(rItem) == REF_INTEGER;
}

bool PListArray::IsString(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	return pType(rItem) == REF_STRING;
}


void PListArray::SetArray(c4_View& array, c4_RowRef& plist) {
	m_vArray = array;

	if (m_rPlist) delete m_rPlist;
	m_rPlist = new c4_RowRef(plist);

	m_vStrings = pStrings(*m_rPlist);
	m_vDicts = pDicts(*m_rPlist);
	m_vArrays = pArrays(*m_rPlist);
}

bool PListArray::GetBool(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	if (pType(rItem) != REF_BOOL) return false;

	const int ref = pRef(rItem);
	return ref != 0;
}

int PListArray::GetInt(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	if (pType(rItem) != REF_INTEGER) return false;

	const int ref = pRef(rItem);
	return ref;
}

const char* PListArray::GetString(unsigned int ndx) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	if (pType(rItem) != REF_STRING) return NULL;

	const int ref = pRef(rItem);
	return pString(m_vStrings[ref]);
}

wxString PListArray::wxGetString(unsigned int ndx) const {
	const char* str = GetString(ndx);
	if (str) return wxString(str, wxConvUTF8);
	else return wxEmptyString;
}

bool PListArray::GetDict(unsigned int ndx, PListDict& dict) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	if (pType(rItem) != REF_DICT) return false;

	// Get the dict
	const int ref = pRef(rItem);
	c4_RowRef rDict = m_vDicts[ref];

	// Get the hash
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	dict.SetDict(vDict, *m_rPlist);
	return true;
}

bool PListArray::GetArray(unsigned int ndx, PListArray& array) const {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	if (pType(rItem) != REF_ARRAY) return false;

	const int ref = pRef(rItem);
	c4_View vArray = pArray(m_vArrays[ref]);
	array.SetArray(vArray, *m_rPlist);
	return true;
}

void PListArray::DeleteItem(unsigned int ndx) {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx < m_vArray.GetSize());

	const c4_RowRef rItem = m_vArray[ndx];
	const int ref = pRef(rItem);

	// Delete the target
	switch (pType(rItem)) {
	case REF_DICT:
		DeleteDict(ref);
		break;
	case REF_ARRAY:
		DeleteArray(ref);
		break;
	case REF_STRING:
		((c4_View)pFreeStrings(*m_rPlist)).Add(pRef[ref]);
		m_vStrings[ref] = c4_Row();
		break;
	}

	// Delete the item
	m_vArray.RemoveAt(ndx);
}

void PListArray::Clear() {
	wxASSERT(m_rPlist);

	while (m_vArray.GetSize()) {
		DeleteItem(0);
	}
}


PListDict PListArray::InsertNewDict(unsigned int ndx) {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx <= m_vArray.GetSize());

	// Create a new empty dict
	const int vref = m_vDicts.Add(c4_Row());

	// Create the new array entry
	c4_Row rRef;
	pRefType(rRef) = REF_DICT;
	pRef(rRef) = vref;

	// Insert ref in array
	m_vArray.InsertAt(ndx, rRef);

	// Get the dict
	c4_RowRef rDict = m_vDicts[vref];
	c4_View vSourceDict = pDict(rDict);
	c4_View vHashDict = pDictH(rDict);
	c4_View vDict = vSourceDict.Hash(vHashDict);

	return PListDict(vDict, *m_rPlist);
}

PListArray PListArray::InsertNewArray(unsigned int ndx) {
	wxASSERT(m_rPlist);
	wxASSERT((int)ndx <= m_vArray.GetSize());

	// Create a new empty dict
	const int vref = m_vArrays.Add(c4_Row());

	// Create the new array entry
	c4_Row rRef;
	pRefType(rRef) = REF_ARRAY;
	pRef(rRef) = vref;

	// Insert ref in array
	m_vArray.InsertAt(ndx, rRef);

	c4_View vArray = pArray(m_vArrays[vref]);
	return PListArray(vArray, *m_rPlist);
}

void PListArray::InsertString(unsigned int ndx, const char* text) {
	wxASSERT((int)ndx <= m_vArray.GetSize());
	wxASSERT(text);

	const int ref = m_vStrings.Add(pString[text]);

	c4_Row rRef;
	pRefType(rRef) = REF_STRING;
	pRef(rRef) = ref;
	m_vArray.InsertAt(ndx, rRef);
}

void PListArray::AddBool(bool value) {
	c4_Row rRef;
	pRefType(rRef) = REF_BOOL;
	pRef(rRef) = value ? 1 : 0;
	m_vArray.Add(rRef);
}

void PListArray::AddInt(int value) {
	c4_Row rRef;
	pRefType(rRef) = REF_INTEGER;
	pRef(rRef) = value;
	m_vArray.Add(rRef);
}

void PListArray::AddString(const char* text) {
	wxASSERT(text);
	const int ref = m_vStrings.Add(pString[text]);

	c4_Row rRef;
	pRefType(rRef) = REF_STRING;
	pRef(rRef) = ref;
	m_vArray.Add(rRef);
}

void PListArray::AddString(const wxString& text) {
	AddString(text.ToUTF8());
}

wxJSONValue PListArray::GetJSONArray() const {
	wxJSONValue root;

	for (int i = 0; i < m_vArray.GetSize(); ++i) {
		const c4_RowRef rItem = m_vArray[i];
		int ref = pRef(rItem);

		switch (pType(rItem)) {
		case REF_DICT:
			{
				c4_RowRef rDict = m_vDicts[ref];
				c4_View vSourceDict = pDict(rDict);
				c4_View vHashDict = pDictH(rDict);
				c4_View vDict = vSourceDict.Hash(vHashDict);
				PListDict dict(vDict, *m_rPlist);
				root.Append(dict.GetJSONDict());
			}
			break;
		case REF_ARRAY:
			{
				c4_View vArray = pArray(m_vArrays[ref]);
				PListArray array(vArray, *m_rPlist);
				root.Append(array.GetJSONArray());
			}
			break;
		case REF_STRING:
			{
				const char* str = pString(m_vStrings[ref]);
				if (str) root.Append(wxString(str, wxConvUTF8));
				else root.Append(wxEmptyString);
			}
			break;
		case REF_INTEGER:
			root.Append(ref);
			break;
		case REF_BOOL:
			root.Append((ref != 0));
			break;
		}
	}

	return root;
}

void PListArray::InsertJSONValues(const wxJSONValue& value) {
	wxASSERT(value.IsArray());

	// Set values from JSON string
	for (int i = 0; i < value.Size(); ++i) {
		const wxJSONValue item = value.ItemAt(i);

		switch(item.GetType()) {
		case wxJSONTYPE_OBJECT:
			{
				PListDict dict = InsertNewDict(i);
				dict.InsertJSONValues(item);
			}
			break;
		case wxJSONTYPE_ARRAY:
			{
				PListArray array = InsertNewArray(i);
				array.InsertJSONValues(item);
			}
			break;
		case wxJSONTYPE_STRING:
			AddString(item.AsString().mb_str(wxConvUTF8));
			break;
		case wxJSONTYPE_INT:
		case wxJSONTYPE_UINT:
			m_vArray.Add(pType[REF_INTEGER] + pRef[item.AsInt()]);
			break;
		case wxJSONTYPE_BOOL:
			if (item.AsBool()) m_vArray.Add(pType[REF_BOOL] + pRef[1]);
			else m_vArray.Add(pType[REF_BOOL] + pRef[0]);
			break;
        default:
            wxASSERT(false); // not handled
		}
	}
}

wxString PListArray::GetJSON() const {
	wxJSONValue root = GetJSONArray();

	wxJSONWriter writer(wxJSONWRITER_STYLED|wxJSONWRITER_MULTILINE_STRING);
	wxString str;
	writer.Write( root, str );

	return str;
}