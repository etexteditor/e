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

#include "eSettings.h"
#include "jsonreader.h"
#include "jsonwriter.h"
#include <wx/wfstream.h>
#include "RemoteThread.h"
#include <wx/regex.h>

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif

eSettings::eSettings() {
}

void eSettings::Load(const wxString& appDataPath) {
	m_path = appDataPath + wxT("e.cfg");

	// Open the settings file
	if (!wxFileExists(m_path)) return;
	wxFileInputStream fstream(m_path);
	if (!fstream.IsOk()) {
		wxMessageBox(_("Could not open settings file."), _("File error"), wxICON_ERROR|wxOK);
		return;
	}

	// Parse the JSON contents
	wxJSONReader reader;
	const int numErrors = reader.Parse(fstream, &m_jsonRoot);
	if ( numErrors > 0 )  {
		// if there are errors in the JSON document, print the errors
		wxString msg = _("Invalid JSON in settings:\n");
		const wxArrayString& errors = reader.GetErrors();
		for ( int i = 0; i < numErrors; i++ )  {
			msg += wxT('\n');
			msg += errors[i];
		}

		wxMessageBox(msg, _("Syntax error"), wxICON_ERROR|wxOK);
		return;
	}
}

bool eSettings::Save() {
	wxASSERT(!m_path.empty());

	// Open (or create) the settings file
	wxFileOutputStream fstream(m_path);
	if (!fstream.IsOk()) {
		wxMessageBox(_("Could not open settings file."), _("File error"), wxICON_ERROR|wxOK);
		return false;
	}

	// Write settings
	wxJSONWriter writer(wxJSONWRITER_STYLED);
	writer.Write( m_jsonRoot, fstream );
	return true;
}

bool eSettings::IsEmpty() const {
	return !m_jsonRoot.IsObject();
}

bool eSettings::GetSettingBool(const wxString& name, bool& value) const {
	if (!m_jsonRoot.HasMember(wxT("settings"))) return false;

	const wxJSONValue settings = m_jsonRoot.ItemAt(wxT("settings"));
	if (!settings.HasMember(name)) return false;

	// old bool values may have been stored as ints
	const wxJSONValue val = settings.ItemAt(name);
	if (val.IsInt()) return (val.AsInt() > 0);

	if (!val.IsBool()) return false;

	value = val.AsBool();
	return true;
}

void eSettings::SetSettingBool(const wxString& name, bool value) {
	wxJSONValue& settings = m_jsonRoot[wxT("settings")];
	settings[name] = value;
}

bool eSettings::GetSettingInt(const wxString& name, int& value) const {
	if (!m_jsonRoot.HasMember(wxT("settings"))) return false;

	const wxJSONValue settings = m_jsonRoot.ItemAt(wxT("settings"));
	if (!settings.HasMember(name)) return false;

	const wxJSONValue val = settings.ItemAt(name);
	if (!val.IsInt()) return false;

	value = val.AsInt();
	return true;
}

void eSettings::SetSettingInt(const wxString& name, int value) {
	wxJSONValue& settings = m_jsonRoot[wxT("settings")];
	settings[name] = value;
}

bool eSettings::GetSettingLong(const wxString& name, wxLongLong& value) const {
	if (!m_jsonRoot.HasMember(wxT("settings"))) return false;

	const wxJSONValue settings = m_jsonRoot.ItemAt(wxT("settings"));
	if (!settings.HasMember(name)) return false;

	const wxJSONValue val = settings.ItemAt(name);
	if (!val.IsInt64()) return false;

	value = val.AsInt64();
	return true;
}

void eSettings::SetSettingLong(const wxString& name, const wxLongLong& value) {
	wxJSONValue& settings = m_jsonRoot[wxT("settings")];
	settings[name] = value.GetValue();
}

bool eSettings::GetSettingString(const wxString& name, wxString& value) const {
	if (!m_jsonRoot.HasMember(wxT("settings"))) return false;

	const wxJSONValue settings = m_jsonRoot.ItemAt(wxT("settings"));
	if (!settings.HasMember(name)) return false;

	const wxJSONValue val = settings.ItemAt(name);
	if (!val.IsString()) return false;

	value = val.AsString();
	return true;
}

void eSettings::SetSettingString(const wxString& name, const wxString& value) {
	wxJSONValue& settings = m_jsonRoot[wxT("settings")];
	settings[name] = value;
}

void eSettings::RemoveSetting(const wxString& name) {
	wxJSONValue& settings = m_jsonRoot[wxT("settings")];
	settings.Remove(name);
}

void eSettings::AddRecentFile(const wxString& path) {
	wxJSONValue& recentFiles = m_jsonRoot[wxT("recentFiles")];
	AddToRecent(path, recentFiles, 10);
}

void eSettings::AddRecentProject(const wxString& path) {
	wxJSONValue& recentProjects = m_jsonRoot[wxT("recentProjects")];
	AddToRecent(path, recentProjects, 10);
}

void eSettings::GetRecentFiles(wxArrayString& recentfiles) const {
	const wxJSONValue recents = m_jsonRoot.ItemAt(wxT("recentFiles"));
	GetRecents(recents, recentfiles);
}

void eSettings::GetRecentProjects(wxArrayString& recentprojects) const {
	const wxJSONValue recents = m_jsonRoot.ItemAt(wxT("recentProjects"));
	GetRecents(recents, recentprojects);
}

void eSettings::AddToRecent(const wxString& key, wxJSONValue& jsonArray, size_t max) {
	const wxJSONValue value(key);

	// Check if value is already in array
	int ndx = -1;
	for (int i = 0; i < jsonArray.Size(); ++i) {
		if (value.IsSameAs(jsonArray.ItemAt(i))) {
			ndx = i;
			break;
		}
	}

	if (ndx == 0) return; // No need to do anything if path is already top
	
	if (ndx > 0) jsonArray.Remove(ndx); // remove duplicate entry

	// Insert value at top
	jsonArray.Insert(0, value);

	// Make sure we have no more than max entries
	if (jsonArray.Size() > (int)max) jsonArray.Remove(max);
}

void eSettings::GetRecents(const wxJSONValue& jarray, wxArrayString& recents) const {
	for (int i = 0; i < jarray.Size(); ++i) {
		recents.Add(jarray.ItemAt(i).AsString());
	}
}

size_t eSettings::GetPageCount() const {
	if (!m_jsonRoot.HasMember(wxT("pages"))) return 0;

	const wxJSONValue pages = m_jsonRoot.ItemAt(wxT("pages"));
	return pages.Size();
}

void eSettings::SetPageSettings(size_t page_id, const wxString& path, doc_id di, int pos, int topline, const wxString& syntax, const vector<unsigned int>& folds, const vector<cxBookmark>& bookmarks) {
	wxJSONValue& pages = m_jsonRoot.Item(wxT("pages"));
	if (!pages.IsArray()) pages.SetType(wxJSONTYPE_ARRAY);

	wxASSERT((int)page_id <= pages.Size());
	wxJSONValue& page = ((int)page_id == pages.Size()) ? pages.Append(wxJSONValue(wxJSONTYPE_OBJECT)) : pages[page_id];
	page.RemoveAll();

	page[wxT("path")] = path;
	page[wxT("pos")] = pos;
	page[wxT("topline")] = topline;
	page[wxT("syntax")] = syntax;

	// doc_id
	page[wxT("doc_type")] = di.type;
	page[wxT("doc_doc")] = di.document_id;
	page[wxT("doc_version")] = di.version_id;

	// Set folds
	wxJSONValue& foldsArray = page[wxT("folds")];
	if (!foldsArray.IsArray()) foldsArray.SetType(wxJSONTYPE_ARRAY);
	for (vector<unsigned int>::const_iterator p = folds.begin(); p != folds.end(); ++p) {
		foldsArray.Append(*p);
	}

	// Set bookmarks
	wxJSONValue& bookmarksArray = page[wxT("bookmarks")];
	if (!bookmarksArray.IsArray()) bookmarksArray.SetType(wxJSONTYPE_ARRAY);
	for (vector<cxBookmark>::const_iterator b = bookmarks.begin(); b != bookmarks.end(); ++b) {
		bookmarksArray.Append(b->line_id);
	}
}

void eSettings::GetPageSettings(size_t page_id, wxString& path, doc_id& di, int& pos, int& topline, wxString& syntax, vector<unsigned int>& folds, vector<unsigned int>& bookmarks) const {
	const wxJSONValue pages = m_jsonRoot.ItemAt(wxT("pages"));
	wxASSERT((int)page_id < pages.Size());
	const wxJSONValue page = pages.ItemAt(page_id);

	path = page.ItemAt(wxT("path")).AsString();
	pos = page.ItemAt(wxT("pos")).AsInt();
	topline = page.ItemAt(wxT("topline")).AsInt();
	syntax = page.ItemAt(wxT("syntax")).AsString();

	// doc_id
	di.type = (doc_type)page.ItemAt(wxT("doc_type")).AsInt();
	di.document_id = page.ItemAt(wxT("doc_doc")).AsInt();
	di.version_id = page.ItemAt(wxT("doc_version")).AsInt();

	// Set folds
	const wxJSONValue foldsArray = page.ItemAt(wxT("folds"));
	for (int f = 0; f < foldsArray.Size(); ++f) {
		folds.push_back(foldsArray.ItemAt(f).AsInt());
	}

	// Set bookmarks
	const wxJSONValue bookmarksArray = page.ItemAt(wxT("bookmarks"));
	for (int b = 0; b < bookmarksArray.Size(); ++b) {
		bookmarks.push_back(bookmarksArray.ItemAt(b).AsInt());
	}
}

wxString eSettings::GetPagePath(size_t page_id) const {
	const wxJSONValue pages = m_jsonRoot.ItemAt(wxT("pages"));
	wxASSERT((int)page_id < pages.Size());
	const wxJSONValue page = pages.ItemAt(page_id);

	return page.ItemAt(wxT("path")).AsString();
}

doc_id eSettings::GetPageDoc(size_t page_id) const {
	const wxJSONValue pages = m_jsonRoot.ItemAt(wxT("pages"));
	wxASSERT((int)page_id < pages.Size());
	const wxJSONValue page = pages.ItemAt(page_id);

	doc_id di;
	di.type = (doc_type)page.ItemAt(wxT("doc_type")).AsInt();
	di.document_id = page.ItemAt(wxT("doc_doc")).AsInt();
	di.version_id = page.ItemAt(wxT("doc_version")).AsInt();

	return di;
}

void eSettings::DeleteAllPageSettings() {
	m_jsonRoot.Remove(wxT("pages"));
}

void eSettings::DeletePageSettings(size_t page_id) {
	wxJSONValue& pages = m_jsonRoot.Item(wxT("pages"));
	wxASSERT((int)page_id < pages.Size());

	pages.Remove(page_id);
}

RemoteProfile* eSettings::DoGetRemoteProfile(size_t profile_id)  {
	// Check if profile is already in cache
	for (auto_vector<RemoteProfile>::iterator p = m_tempRemotes.begin(); p != m_tempRemotes.end(); ++p) {
		if ((*p)->m_id == (int)profile_id) return *p;
	}

	// Get the profile
	const wxJSONValue remotes = m_jsonRoot.ItemAt(wxT("remoteProfiles"));
	wxASSERT((int)profile_id < remotes.Size());
	const wxJSONValue profile = remotes.ItemAt(profile_id);
	
	// Add profile to cache
	auto_ptr<RemoteProfile> rp(new RemoteProfile());
	rp->m_id = profile_id;
	rp->m_protocol = profile.ItemAt(wxT("protocol")).AsString();
	rp->m_name = profile.ItemAt(wxT("name")).AsString();
	rp->m_address = profile.ItemAt(wxT("address")).AsString();
	rp->m_dir = profile.ItemAt(wxT("dir")).AsString();
	rp->m_username = profile.ItemAt(wxT("username")).AsString();
	rp->m_pwd = profile.ItemAt(wxT("pwd")).AsString();
	m_tempRemotes.push_back(rp);

	return m_tempRemotes.back();
}

size_t eSettings::GetRemoteProfileCount() const {
	if (!m_jsonRoot.HasMember(wxT("remoteProfiles"))) return 0;

	const wxJSONValue remotes = m_jsonRoot.ItemAt(wxT("remoteProfiles"));
	return remotes.Size();
}

wxString eSettings::GetRemoteProfileName(size_t profile_id) const {
	const wxJSONValue remotes = m_jsonRoot.ItemAt(wxT("remoteProfiles"));
	wxASSERT((int)profile_id < remotes.Size());
	const wxJSONValue profile = remotes.ItemAt(profile_id);
	
	return profile.ItemAt(wxT("name")).AsString();
}

size_t eSettings::AddRemoteProfile(const RemoteProfile& profile) {
	wxJSONValue& remotes = m_jsonRoot[wxT("remoteProfiles")];
	const size_t profile_id = remotes.Size();
	SetRemoteProfile(profile_id, profile);
	return profile_id;
}

void eSettings::SetRemoteProfile(size_t profile_id, const RemoteProfile& profile) {
	wxJSONValue& remotes = m_jsonRoot[wxT("remoteProfiles")];

	// Add new profile if needed
	wxASSERT((int)profile_id <= remotes.Size());
	wxJSONValue& jprofile = ((int)profile_id == remotes.Size()) ? remotes.Append(wxJSONValue(wxJSONTYPE_OBJECT)) : remotes[profile_id];
	jprofile.RemoveAll();

	// default protocol is ftp
	const wxString protocol = profile.m_protocol.empty() ? wxT("ftp") : profile.m_protocol;

	// Update entry
	jprofile[wxT("protocol")] = protocol;
	jprofile[wxT("name")] = profile.m_name;
	jprofile[wxT("address")] = profile.m_address;
	jprofile[wxT("dir")] = profile.m_dir;
	jprofile[wxT("username")] = profile.m_username;
	jprofile[wxT("pwd")] = profile.m_pwd;

	// If profile is in cache we have to update there as well
	for (auto_vector<RemoteProfile>::iterator p = m_tempRemotes.begin(); p != m_tempRemotes.end(); ++p) {
		if ((*p)->m_id == (int)profile_id) {
			*(*p) = profile;
			(*p)->m_id = profile_id;
			return;
		}
	}
}

const RemoteProfile* eSettings::GetRemoteProfile(size_t profile_id) {
	return DoGetRemoteProfile(profile_id);
}

const RemoteProfile* eSettings::GetRemoteProfileFromUrl(const wxString& url, bool withDir) {
	// Split url up in elements
	wxRegEx reUrl(wxT("([[:alpha:]]+)://(([^:@]*):([^:@]*)@)?([^/]+)(.*)"));
	if (!reUrl.Matches(url)) return NULL; // invalid url

	const wxString protocol = reUrl.GetMatch(url, 1);
	const wxString username = reUrl.GetMatch(url, 3);
	const wxString pwd = reUrl.GetMatch(url, 4);
	const wxString address = reUrl.GetMatch(url, 5);
	const wxString dir = reUrl.GetMatch(url, 6);
	const wxString sDir = StripSlashes(dir);

	// See if we can find a matching profile in cache
	for (auto_vector<RemoteProfile>::iterator p = m_tempRemotes.begin(); p != m_tempRemotes.end(); ++p) {
		RemoteProfile* rp = (*p);
		if (rp->IsActive() && rp->m_address == address && rp->m_protocol == protocol) {
			if (!username.empty() && rp->m_username != username) continue;
			if (withDir && StripSlashes(rp->m_dir) != sDir) continue;
			if (!pwd.empty() && rp->m_pwd != pwd) {
				rp->m_pwd = pwd; // Password may have changed
				if (!rp->IsTemp()) SaveRemoteProfile(rp);
			}
			return rp;
		}
	}

	// See if we can find a matching profile in settings
	const wxJSONValue remotes = m_jsonRoot.ItemAt(wxT("remoteProfiles"));
	const int profile_count = remotes.Size();
	for (int i = 0; i < profile_count; ++i) {
		const wxJSONValue profile = remotes.ItemAt(i);
		if (profile.ItemAt(wxT("address")).AsString() != address) continue;

		// Ftp is default protocol
		wxString prot = profile.ItemAt(wxT("protocol")).AsString();
		if (prot.empty()) prot = wxT("ftp");

		if (prot != protocol) continue; // ftp is default
		if (!username.empty() && profile.ItemAt(wxT("username")).AsString() != username) continue;

		RemoteProfile* rp = DoGetRemoteProfile(i);
		if (withDir && StripSlashes(rp->m_dir) != sDir) continue;

		// Password may have changed
		if (!pwd.empty() && rp->m_pwd != pwd) {
			rp->m_pwd = pwd;
			SaveRemoteProfile(rp);
		}

		return rp;
	}

	// Add new temp profile
	auto_ptr<RemoteProfile> newRp(new RemoteProfile());
	newRp->m_protocol = protocol;
	newRp->m_name = address;
	newRp->m_address = address;
	newRp->m_dir = dir;
	newRp->m_username = username;
	newRp->m_pwd = pwd;
	m_tempRemotes.push_back(newRp);
	return m_tempRemotes.back();
}

void eSettings::SetRemoteProfileLogin(const RemoteProfile* profile, const wxString& username, const wxString& pwd, bool toDb) {
	wxASSERT(profile);

	// Get non-const pointer from cache
	RemoteProfile* rp = NULL;
	for (auto_vector<RemoteProfile>::iterator p = m_tempRemotes.begin(); p != m_tempRemotes.end(); ++p) {
		if (*p == profile) rp = *p;
	}
	wxASSERT(rp);

	// Set the new login. This means that the profile will
	// be set to correct values when this function returns
	rp->m_username = username;
	rp->m_pwd = pwd;

	// Save if profile is in db or user has indicated that it should be saved
	if (!rp->IsTemp() || toDb) {
		SaveRemoteProfile(rp); // set profile with new login
	}
}

void eSettings::DeleteRemoteProfile(size_t profile_id) {
	wxJSONValue& remotes = m_jsonRoot[wxT("remoteProfiles")];
	wxASSERT((int)profile_id < remotes.Size());

	// Remove from db
	remotes.Remove(profile_id);

	// In cache; deactivate profile and adjust id's of rest
	for (auto_vector<RemoteProfile>::iterator p = m_tempRemotes.begin(); p != m_tempRemotes.end(); ++p) {
		if ((*p)->m_id == (int)profile_id) (*p)->DisActivate();
		else if ((*p)->m_id > (int)profile_id) --((*p)->m_id);
	}
}

void eSettings::SaveRemoteProfile(RemoteProfile* rp) {
	wxJSONValue& remotes = m_jsonRoot[wxT("remoteProfiles")];

	// Add new profile if needed
	if (rp->IsTemp()) rp->m_id = remotes.Size();
	wxJSONValue& profile = (rp->m_id == remotes.Size()) ? remotes.Append(wxJSONValue(wxJSONTYPE_OBJECT)) : remotes[rp->m_id];

	// default protocol is ftp
	const wxString protocol = rp->m_protocol.empty() ? wxT("ftp") : rp->m_protocol;

	// Update settings
	profile[wxT("protocol")] = protocol;
	profile[wxT("name")] = rp->m_name;
	profile[wxT("address")] = rp->m_address;
	profile[wxT("dir")] = rp->m_dir;
	profile[wxT("username")] = rp->m_username;
	profile[wxT("pwd")] = rp->m_pwd;
}

wxString eSettings::StripSlashes(const wxString& path) { // static
	if (path.empty() || path == wxT('/')) return wxEmptyString;
	const size_t start = (path[0] == wxT('/')) ? 1 : 0;
	const size_t end = (path.Last() ==  wxT('/')) ? path.size()-1 : path.size();
	return path.substr(start, end-start);
}

size_t eSettings::GetSearchCount() const {
	if (!m_jsonRoot.HasMember(wxT("searchHistory"))) return 0;

	const wxJSONValue searches = m_jsonRoot.ItemAt(wxT("searchHistory"));
	return searches.Size();
}

void eSettings::GetSearch(size_t ndx, wxString& pattern, bool& isRegex, bool& matchCase) const {
	const wxJSONValue searches = m_jsonRoot.ItemAt(wxT("searchHistory"));
	wxASSERT((int)ndx < searches.Size());

	const wxJSONValue item = searches.ItemAt(ndx);
	pattern = item.ItemAt(wxT("pattern")).AsString();
	isRegex = item.ItemAt(wxT("isRegex")).AsBool();
	matchCase = item.ItemAt(wxT("matchCase")).AsBool();
}

bool eSettings::AddSearch(const wxString& pattern, bool isRegex, bool matchCase) {
	wxJSONValue& searches = m_jsonRoot[wxT("searchHistory")];

	// Don't add duplicates
	if (searches.Size() > 0) {
		wxJSONValue& last = searches[0];
		if (last.ItemAt(wxT("pattern")).AsString() == pattern) {
			// We can ignore repeated top insertions, but options may have changed
			if (last[wxT("isRegex")].AsBool() != isRegex) last[wxT("isRegex")] = isRegex;
			if (last[wxT("matchCase")].AsBool() != matchCase) last[wxT("matchCase")] = matchCase;			
			return false;
		}

		// Check if there should be a duplicate lower down
		for (int i = 0; i < searches.Size(); ++i) {
			if (searches[i][wxT("pattern")].AsString() == pattern) {
				searches.Remove(i);
				break;
			}
		}
	}
	
	// Add the new item
	wxJSONValue item;
	item[wxT("pattern")] = pattern;
	item[wxT("isRegex")] = isRegex;
	item[wxT("matchCase")] = matchCase;
	searches.Insert(0, item);

	// Limit number of items to save
	if (searches.Size() > 20) searches.Remove(searches.Size()-1);

	return true;
}

size_t eSettings::GetReplaceCount() const {
	if (!m_jsonRoot.HasMember(wxT("replaceHistory"))) return 0;

	const wxJSONValue replacements = m_jsonRoot.ItemAt(wxT("replaceHistory"));
	return replacements.Size();
}

wxString eSettings::GetReplace(size_t ndx) const {
	const wxJSONValue replacements = m_jsonRoot.ItemAt(wxT("replaceHistory"));
	wxASSERT((int)ndx < replacements.Size());

	return replacements.ItemAt(ndx).AsString();
}

bool eSettings::AddReplace(const wxString& pattern) {
	wxJSONValue& replacements = m_jsonRoot[wxT("replaceHistory")];

	// Don't add duplicates
	if (replacements.Size() > 0) {
		const wxJSONValue last = replacements.ItemAt(0);
		if (last.AsString() == pattern) return false;
		
		// Check if there should be a duplicate lower down
		for (int i = 0; i < replacements.Size(); ++i) {
			if (replacements[i].AsString() == pattern) {
				replacements.Remove(i);
				break;
			}
		}
	}
	
	// Add the new item
	replacements.Insert(0, pattern);

	// Limit number of items to save
	if (replacements.Size() > 20) replacements.Remove(replacements.Size()-1);

	return true;
}
