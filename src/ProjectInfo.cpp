#include "ProjectInfo.h"
#include "tinyxml.h"
#include <wx/ffile.h>

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
};

void cxProjectInfo::Clear() {
	isRoot = false;
	hasFilters = false;
	path.clear();
	includeDirs.Empty();
	excludeDirs.Empty();
	includeFiles.Empty();
	excludeFiles.Empty();
	env.clear();
};

bool cxProjectInfo::IsEmpty() const {
	return hasFilters || !env.empty() || !triggers.empty();
};

bool cxProjectInfo::IsFileIncluded(const wxString& file_name) const {
	if (!includeFiles.IsEmpty()) {
		bool doInclude = false;
		for (unsigned int i = 0; i < includeFiles.GetCount(); ++i) {
			if (wxMatchWild(includeFiles[i], file_name, false)) {
				doInclude = true;
				break;
			}
		}

		if (!doInclude) return false;
	}

	for (unsigned int i = 0; i < excludeFiles.GetCount(); ++i) {
		if (wxMatchWild(excludeFiles[i], file_name, false)) {
			return false;
		}
	}

	return true;
};

bool cxProjectInfo::IsDirectoryIncluded(const wxString& dir_name) const {
	if (!includeFiles.IsEmpty()) {
		bool doInclude = false;
		for (unsigned int i = 0; i < includeDirs.GetCount(); ++i) {
			if (wxMatchWild(includeDirs[i], dir_name, false)) {
				doInclude = true;
				break;
			}
		}

		if (!doInclude) return false;
	}

	for (unsigned int i = 0; i < excludeFiles.GetCount(); ++i) {
		if (wxMatchWild(excludeDirs[i], dir_name, false)) {
			return false;
		}
	}

	return true;
};


bool cxProjectInfo::Load(const wxFileName& rootPath, const wxString& path, bool onlyFilters, cxProjectInfo& projectInfo) {
	wxFileName projectPath(path, wxEmptyString);
	projectInfo.path = path;
	projectInfo.isRoot = (projectPath == rootPath);

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
				if (strcmp(filterName, "includeDirs") == 0) filterArray = &projectInfo.includeDirs;
				else if (strcmp(filterName, "excludeDirs") == 0) filterArray = &projectInfo.excludeDirs;
				else if (strcmp(filterName, "includeFiles") == 0) filterArray = &projectInfo.includeFiles;
				else if (strcmp(filterName, "excludeFiles") == 0) filterArray = &projectInfo.excludeFiles;
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

			projectInfo.hasFilters = true;
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
					projectInfo.env[wxString(key, wxConvUTF8)] = value ? wxString(value, wxConvUTF8) : *wxEmptyString;
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

					projectInfo.triggers[wxString(key, wxConvUTF8)] = path.GetFullPath();
				}

				trigger = val; // jump over value
			}
		}

		entry = value; // jump over value
	}

	if (onlyFilters && !projectInfo.hasFilters) return false;
	return true;
}
