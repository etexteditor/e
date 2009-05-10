#include "ProjectInfo.h"
#include <wx/ffile.h>

#include "tinyxml.h"
#include "EasyPlistWriter.h"

cxProjectInfo::cxProjectInfo(): isRoot(false), hasFilters(false) {};

cxProjectInfo::cxProjectInfo(const cxProjectInfo& info) {
	isRoot = info.isRoot;
	hasFilters = info.hasFilters;
	path = info.path;
	includeDirs = info.includeDirs;
	excludeDirs = info.excludeDirs;
	includeFiles = info.includeFiles;
	excludeFiles = info.excludeFiles;
	env = info.env;
}

void cxProjectInfo::Clear() {
	isRoot = false;
	hasFilters = false;
	path.clear();
	includeDirs.Empty();
	excludeDirs.Empty();
	includeFiles.Empty();
	excludeFiles.Empty();
	env.clear();
}

void cxProjectInfo::ClearFilters() {
	includeDirs.Empty();
	excludeDirs.Empty();
	includeFiles.Empty();
	excludeFiles.Empty();
	hasFilters = false;
}

void cxProjectInfo::SetFilters(const wxArrayString& ind, const wxArrayString& exd, const wxArrayString& inf, const wxArrayString& exf) {
	ClearFilters();

	includeDirs = ind;
	excludeDirs = exd;
	includeFiles = inf;
	excludeFiles = exf;

	hasFilters = true;
}

cxProjectInfo::cxProjectInfo(const wxFileName &rootPath, const wxString& path, bool onlyFilters) {
	this->Load(rootPath, path, onlyFilters);
}

bool cxProjectInfo::IsEmpty() const {
	return !hasFilters && env.empty() && triggers.empty();
}

static bool projectinfo_match(const wxString& path, const wxArrayString& includes, const wxArrayString& excludes) {
	if (!includes.IsEmpty()) {
		bool doInclude = false;
		for (unsigned int i = 0; i < includes.GetCount(); ++i) {
			if (wxMatchWild(includes[i], path, false)) {
				doInclude = true;
				break;
			}
		}

		if (!doInclude) return false;
	}

	for (unsigned int i = 0; i < excludes.GetCount(); ++i) {
		if (wxMatchWild(excludes[i], path, false)) {
			return false;
		}
	}

	return true;
}

bool cxProjectInfo::IsFileIncluded(const wxString& file_name) const {
	return projectinfo_match(file_name, includeFiles, excludeFiles);
}

bool cxProjectInfo::IsDirectoryIncluded(const wxString& dir_name) const {
	return projectinfo_match(dir_name, includeDirs, excludeDirs);
}


bool cxProjectInfo::Load(const wxFileName& rootPath, const wxString& path, bool onlyFilters) {
	wxFileName projectPath(path, wxEmptyString);
	this->path = path;
	this->isRoot = (projectPath == rootPath);

	projectPath.SetFullName(wxT(".eprj"));
	if (!projectPath.FileExists()) return false;

	// Load plist file
	TiXmlDocument doc;
	wxFFile docffile(projectPath.GetFullPath(), _T("rb"));
	wxCHECK(docffile.IsOpened() && doc.LoadFile(docffile.fp()), false);

	// Get top dict
	const TiXmlHandle docHandle(&doc);
	const TiXmlElement* topDict = docHandle.FirstChildElement("plist").FirstChildElement().Element();
	if (!topDict || strcmp(topDict->Value(), "dict") != 0) return false; // empty plist

	// Parse entries
	const TiXmlElement* entry = topDict->FirstChildElement();
	for(; entry; entry = entry->NextSiblingElement() ) {
		if (strcmp(entry->Value(), "key") != 0) return false; // invalid dict
		const TiXmlElement* const value = entry->NextSiblingElement();

		if (strcmp(entry->GetText(), "filters") == 0) {
			// Load all filters
			const TiXmlElement* filter = value->FirstChildElement();
			for(; filter; filter = filter->NextSiblingElement() ) {
				if (strcmp(filter->Value(), "key") != 0) return false; // invalid dict

				// Set target array
				wxArrayString* filterArray = NULL;
				const char* filterName = filter->GetText();
				if (strcmp(filterName, "includeDirs") == 0) filterArray = &this->includeDirs;
				else if (strcmp(filterName, "excludeDirs") == 0) filterArray = &this->excludeDirs;
				else if (strcmp(filterName, "includeFiles") == 0) filterArray = &this->includeFiles;
				else if (strcmp(filterName, "excludeFiles") == 0) filterArray = &this->excludeFiles;
				else {
					wxASSERT(false);
					break;
				}

				const TiXmlElement* const array = filter->NextSiblingElement();
				if (strcmp(array->Value(), "array") != 0) return false; // invalid dict

				const TiXmlElement* child = array->FirstChildElement();
				for(; child; child = child->NextSiblingElement() ) {
					const char* valType = child->Value();

					if (strcmp(valType, "string") == 0) {
						const char* pattern = child->GetText();
						if (pattern) filterArray->Add(wxString(pattern, wxConvUTF8));
					}
					else {
						wxASSERT(false);
					}
				}

				filter = array; // jump over value
			}

			this->hasFilters = true;
			if (onlyFilters) return true;
		}
		else if (strcmp(entry->GetText(), "environment") == 0) {
			// Load all env variables
			const TiXmlElement* env = value->FirstChildElement();
			for(; env; env = env->NextSiblingElement() ) {
				// Get Key
				if (strcmp(env->Value(), "key") != 0) return false; // invalid dict
				const char* key = env->GetText();

				const TiXmlElement* const val = env->NextSiblingElement();
				if (strcmp(val->Value(), "string") != 0) return false; // invalid dict
				const char* value = val->GetText();

				if (key) {
					this->env[wxString(key, wxConvUTF8)] = value ? wxString(value, wxConvUTF8) : *wxEmptyString;
				}

				env = val; // jump over value
			}
		}
		else if (strcmp(entry->GetText(), "fileTriggers") == 0) {
			// Load all env variables
			const TiXmlElement* trigger = value->FirstChildElement();
			for(; trigger; trigger = trigger->NextSiblingElement() ) {
				// Get Key
				if (strcmp(trigger->Value(), "key") != 0) return false; // invalid dict
				const char* key = trigger->GetText();

				const TiXmlElement* const val = trigger->NextSiblingElement();
				if (strcmp(val->Value(), "string") != 0) return false; // invalid dict
				const char* value = val->GetText();

				if (key && value) {
					wxFileName path(wxString(value, wxConvUTF8));
					path.MakeAbsolute(rootPath.GetPath());

					this->triggers[wxString(key, wxConvUTF8)] = path.GetFullPath();
				}

				trigger = val; // jump over value
			}
		}

		entry = value; // jump over value
	}

	if (onlyFilters && !this->hasFilters) return false;
	return true;
}

void cxProjectInfo::Save(const wxString& rootPath) const {
	wxFileName path(this->path, wxEmptyString);
	path.SetFullName(wxT(".eprj"));
	const wxString filepath = path.GetFullPath();

	// Remove empty files
	if (path.FileExists()) wxRemoveFile(filepath);
	if (this->IsEmpty()) return;

	EasyPlistWriter eprj;

	// Filters
	if (this->hasFilters) {
		TiXmlElement *filterDict = eprj.AddDict(NULL, "filters");
		eprj.AddList(filterDict, "excludeDirs", this->excludeDirs);
		eprj.AddList(filterDict, "excludeFiles", this->excludeFiles);
		eprj.AddList(filterDict, "includeDirs", this->includeDirs);
		eprj.AddList(filterDict, "includeFiles", this->includeFiles);
	}

	// Environment variables
	if (!this->env.empty()) {
		TiXmlElement* envDict = eprj.AddDict(NULL, "environment");

		for (map<wxString,wxString>::const_iterator p = this->env.begin(); p != this->env.end(); ++p) {
			if (!p->first.empty())
				eprj.AddString(envDict, p->first.mb_str(wxConvUTF8), p->second.mb_str(wxConvUTF8));
		}
	}

	// GotoFile triggers
	if (!this->triggers.empty()) {
		TiXmlElement* trigDict = eprj.AddDict(NULL, "fileTriggers");

		for (map<wxString,wxString>::const_iterator p = this->triggers.begin(); p != this->triggers.end(); ++p) {
			if (p->first.empty()) continue;

			wxFileName path(p->second);
			path.MakeRelativeTo(rootPath);
			eprj.AddString(trigDict, p->first.mb_str(wxConvUTF8), path.GetFullPath().mb_str(wxConvUTF8));
		}
	}

	eprj.Save(filepath);

#ifdef __WXMSW__
	DWORD dwAttrs = ::GetFileAttributes(filepath.c_str());
	::SetFileAttributes(filepath.c_str(), dwAttrs | FILE_ATTRIBUTE_HIDDEN);
#endif //__WXMSW__
}
