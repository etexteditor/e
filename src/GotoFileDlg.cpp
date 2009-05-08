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

#include "GotoFileDlg.h"
#include <algorithm>
#include "ProjectPane.h"

// Ctrl id's
enum {
	CTRL_SEARCH,
	CTRL_ALIST
};

BEGIN_EVENT_TABLE(GotoFileDlg, wxDialog)
	EVT_TEXT(CTRL_SEARCH, GotoFileDlg::OnSearch)
	EVT_TEXT_ENTER(CTRL_SEARCH, GotoFileDlg::OnAction)
	EVT_LISTBOX_DCLICK(CTRL_ALIST, GotoFileDlg::OnAction)
	EVT_LISTBOX(CTRL_ALIST, GotoFileDlg::OnListSelection)
	EVT_SIZE(GotoFileDlg::OnSize)
	EVT_IDLE(GotoFileDlg::OnIdle)
END_EVENT_TABLE()

GotoFileDlg::GotoFileDlg(wxWindow *parent, IProjectManager& project)
:  wxDialog (parent, -1, _("Go to File"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER),
   m_project(project), m_isDone(false), m_filesLoaded(false)
{
	// Create Ctrls
	m_searchCtrl = new wxTextCtrl(this, CTRL_SEARCH, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_cmdList = new ActionList(this, CTRL_ALIST, m_files);
	m_pathStatic = new wxStaticText(this, wxID_ANY, wxEmptyString);

	// Add custom event handler
	m_searchCtrl->Connect(wxEVT_CHAR, wxKeyEventHandler(GotoFileDlg::OnSearchChar), NULL, this);

	// Create Layout
	wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
		mainSizer->Add(m_searchCtrl, 0, wxEXPAND);
		mainSizer->Add(m_cmdList, 1, wxEXPAND);
		mainSizer->Add(m_pathStatic, 0, wxEXPAND|wxALL, 5);

	SetSizer(mainSizer);
	SetSize(400, 500);
	Centre();
}

GotoFileDlg::~GotoFileDlg() {
	// Clean up allocated file entries
	for (vector<FileEntry*>::iterator p = m_files.begin(); p != m_files.end(); ++p) {
		delete *p;
	}

	// Clean up allocated dir traversing state
	for (vector<DirState*>::iterator d = m_dirStack.begin(); d != m_dirStack.end(); ++d) {
		delete *d;
	}
}

void GotoFileDlg::OnIdle(wxIdleEvent& event) {
	if (m_filesLoaded) return; // Already done loading files.
	if (!m_project.HasProject()) return; // No project, so no files to load.

	if (m_dirStack.empty()) {
		const wxString path = m_project.GetProject().GetPath();
		m_files.reserve(100);

		BuildFileList(path);
	}
	else {
		DirState& dirState = *m_dirStack.back();

		// Get next subdir
		bool cont;
		if (dirState.nextDirName.empty()) {
			cont = dirState.dir.GetFirst(&dirState.nextDirName, wxEmptyString, wxDIR_DIRS);
		}
		else cont = dirState.dir.GetNext(&dirState.nextDirName);

		// Enter subdir if it matches filter
		while (cont) {
			const wxString fulldirname = dirState.prefix + dirState.nextDirName;

			if (!dirState.filter || ProjectPane::MatchFilter(dirState.nextDirName, dirState.filter->includeDirs, dirState.filter->excludeDirs)) {
				wxLogDebug(fulldirname);
				BuildFileList(fulldirname);
				break;
			}
			else cont = dirState.dir.GetNext(&dirState.nextDirName);
		}

		if (!cont) {
			// All sub-dirs visited
			if (dirState.info) {
				m_filters.pop_back();
				delete dirState.info;
			}
			delete m_dirStack.back();
			m_dirStack.pop_back();

			if (m_dirStack.empty()) m_filesLoaded = true;
		}
	}

	m_cmdList->UpdateList();
	if (!m_dirStack.empty()) event.RequestMore();
}

void GotoFileDlg::BuildFileList(const wxString& path) {
	DirState* dirState = new DirState();

	// Open directory
	dirState->dir.Open(path);
	if (dirState->dir.IsOpened()) m_dirStack.push_back(dirState);
	else {
		delete dirState;;
		return;
	}

	// Load filters for this dir
	dirState->info = new cxProjectInfo;
	if (m_project.LoadProjectInfo(path, true, *dirState->info)) {
		m_filters.push_back(dirState->info);
	}
	else {
		delete dirState->info;
		dirState->info = NULL;
	}

	// Set active filter
	dirState->filter = m_filters.empty() ? NULL : m_filters.back();

	// the name of this dir with path delimiter at the end
    dirState->prefix = path + wxFILE_SEP_PATH;

	// Get all files
	wxString eachFilename;
	int style = wxDIR_FILES;
	bool cont = dirState->dir.GetFirst(&eachFilename, wxEmptyString, style);
	while (cont) {
		if (!dirState->filter || ProjectPane::MatchFilter(eachFilename, dirState->filter->includeFiles, dirState->filter->excludeFiles)) {
			m_files.push_back(new FileEntry(dirState->prefix, eachFilename));
		}

		cont = dirState->dir.GetNext(&eachFilename);
	}
}

void GotoFileDlg::OnSearch(wxCommandEvent& event) {
	m_cmdList->Find(event.GetString(), m_project.GetTriggers());
	UpdateStatusbar();
}

void GotoFileDlg::OnAction(wxCommandEvent& WXUNUSED(event)) {
	if(m_cmdList->GetSelectedCount() == 1) {
		m_project.SetTrigger(m_searchCtrl->GetValue(), m_cmdList->GetSelectedAction()->path);
		m_isDone = true;
		EndModal(wxID_OK);
	}
}

void GotoFileDlg::OnSearchChar(wxKeyEvent& event) {
	switch ( event.GetKeyCode() )
    {
	case WXK_UP:
		m_cmdList->SelectPrev();
		UpdateStatusbar();
		return;
	case WXK_DOWN:
		m_cmdList->SelectNext();
		UpdateStatusbar();
		return;
	case WXK_ESCAPE:
		m_isDone = true;
		EndModal(wxID_CANCEL);
		return;
	}

	// no, we didn't process it
    event.Skip();
}

void GotoFileDlg::OnSize(wxSizeEvent& event) {
	UpdateStatusbar();
	event.Skip();
}

void GotoFileDlg::OnListSelection(wxCommandEvent& WXUNUSED(event)) {
	UpdateStatusbar();
}

void GotoFileDlg::UpdateStatusbar() {
	if(m_cmdList->GetSelectedCount() == 1) {
		wxString path = GetSelection();

		// Calc extension width
		wxClientDC dc(this);
		dc.SetFont(m_pathStatic->GetFont());
		int w, h;
		static const wxString ext = wxT("...");
		dc.GetTextExtent(ext, &w, &h);
		const unsigned int extwidth = w;

		// See if we have to resize the path to fit
		unsigned int len = path.length();
		const unsigned int ctrlWidth = GetClientSize().x - 10;
		dc.GetTextExtent(path, &w, &h);
		if (len && w > (int)ctrlWidth) {
			do {
				path.erase(0, 1);
				dc.GetTextExtent(path, &w, &h);
			} while (--len > 0 && extwidth + w > ctrlWidth);
			path.Prepend(ext);
		}

		m_pathStatic->SetLabel(path);
	}
	else m_pathStatic->SetLabel(wxEmptyString);
}

// ---- FileEntry ----------------------------------------------------

GotoFileDlg::FileEntry::FileEntry(const wxString& dirpath, const wxString& filename) {
	name = filename;
	nameLower = name.Lower();
	path = dirpath + filename;
}

void GotoFileDlg::FileEntry::SetPath(const wxString& p) {
	wxFileName filepath(p);
	name = filepath.GetFullName();
	nameLower = name.Lower();
	path = p;
}

void GotoFileDlg::FileEntry::Clear() {
	name.clear();
	nameLower.clear();
	path.clear();
}

// --- ActionList --------------------------------------------------------

GotoFileDlg::ActionList::ActionList(wxWindow* parent, wxWindowID id, const vector<FileEntry*>& actions)
: SearchListBox(parent, id), m_actions(actions), m_actionCount(0) {

	UpdateList();
}

void GotoFileDlg::ActionList::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const {
	const bool isCurrent = IsCurrent(n);

	if (isCurrent) dc.SetTextForeground(m_hlTextColor);
	else dc.SetTextForeground(m_textColor);

	const FileEntry& action = *m_items[n].action;
	const vector<unsigned int>& hl = m_items[n].hlChars;
	const wxString& name = action.name;

	/*// Calc extension width
	static const wxString ext = wxT("..  ");
	dc.SetFont(m_font);
	int w, h;
	dc.GetTextExtent(ext, &w, &h);
	const unsigned int extwidth = w;

	// See if we have to resize the action name to fit
	// note that this is not 100% correct as bold chars take up a bit more space.
	unsigned int len = name.length();
	dc.GetTextExtent(name, &w, &h);
	if (w > (int)rightBorder) {
		do {
			name.resize(--len);
			dc.GetTextExtent(name, &w, &h);
		} while (len > 0 && w + extwidth > (int)rightBorder);
		name += ext;
	}*/

	// Draw action name
	DrawItemText(dc, rect, name, hl, isCurrent);
}

void GotoFileDlg::ActionList::UpdateList(bool reloadAll) {
	const FileEntry* selEntry = GetSelectedAction();
	const int topLine = GetFirstVisibleLine();
	int selection = -1;
	const unsigned int startItem = reloadAll ? 0 : m_items.size();

	// Insert new items
	if (m_searchText.empty()) {
		// Copy all actions to items
		m_items.resize(m_actions.size());
		for (unsigned int i = startItem; i < m_actions.size(); ++i) {
			m_items[i].action = m_actions[i];
			m_items[i].hlChars.clear();
		}
	}
	else {
		// Copy matching actions to items
		for (unsigned int i = m_actionCount; i < m_actions.size(); ++i) {
			if (m_tempEntry.path.empty() || m_actions[i]->path != m_tempEntry.path) {
				AddActionIfMatching(m_searchText, m_actions[i]);
			}
		}
	}

	// Sort the items
	if (startItem) {
		sort(m_items.begin() + startItem, m_items.end());
		inplace_merge(m_items.begin(), m_items.begin() + startItem, m_items.end());
	}
	else sort(m_items.begin(), m_items.end());

	// Keep same selection
	if (selEntry) {
		for (unsigned int i = 0; i < m_items.size(); ++i) {
			if (m_items[i].action == selEntry) {
				selection = i;
				break;
			}
		}
	}

	// Refresh and redraw listCtrl
	Freeze();
	SetItemCount(m_items.size());
	SetSelection(selection);
	if (selection == -1) ScrollToLine(topLine);
	else if (!IsVisible(selection)) ScrollToLine(selection);
	RefreshAll();
	Thaw();

	m_actionCount = m_actions.size();
}

void GotoFileDlg::ActionList::Find(const wxString& searchtext, const map<wxString,wxString>& triggers) {
	m_tempEntry.Clear();

	if (searchtext.empty()) {
		// Remove highlights
		m_searchText.clear();
		SetSelection(-1); // de-select
		ScrollToLine(0);
		UpdateList(true);
		return;
	}

	// Convert to lower case for case insensitive search
	m_searchText = searchtext.Lower();

	// Find all matching filenames
	m_items.clear();
	vector<unsigned int> hlChars;
	for (unsigned int i = 0; i < m_actions.size(); ++i) {
		AddActionIfMatching(m_searchText, m_actions[i]);
	}
	sort(m_items.begin(), m_items.end());

	// Check if we have a matching trigger
	int selection = wxNOT_FOUND;
	if (m_items.size()) {
		map<wxString,wxString>::const_iterator p = triggers.find(m_searchText);
		if (p != triggers.end()) {
			selection = FindPath(p->second);
		}
		if (selection == wxNOT_FOUND) {
			// Check if we have a partial match
			for (p = triggers.begin(); p != triggers.end(); ++p) {
				if (p->first.StartsWith(m_searchText)) {
					selection = FindPath(p->second);

					if (selection == wxNOT_FOUND) {
						// Since we have a trigger but it is not in list yet
						// Let's check if it exists and add it temporarily
						if (wxFileExists(p->second)) {
							m_tempEntry.SetPath(p->second);

							// Add entry with highlighted search chars
							AddActionIfMatching(m_searchText, &m_tempEntry);

							// Find position it will end up after sort
							vector<aItem>::iterator insPos = lower_bound(m_items.begin(), m_items.end()-1, m_items.back());
							selection = distance(m_items.begin(), insPos);

							// Move item to correct position
							if (m_items.size() > 1) {
								inplace_merge(m_items.begin(), m_items.end()-1, m_items.end());
							}

							break;
						}
					}
				}
			}
		}
	}

	Freeze();
	SetItemCount(m_items.size());
	if (m_items.empty()) SetSelection(-1); // deselect
	else if (selection != wxNOT_FOUND) SetSelection(selection);
	else SetSelection(0);
	RefreshAll();
	Thaw();
}

void GotoFileDlg::ActionList::AddActionIfMatching(const wxString& text, FileEntry* action) {
	wxASSERT(!text.empty());

	const wxString& name = action->nameLower;
	unsigned int charpos = 0;
	wxChar c = text[charpos];

	static vector<unsigned int> hlChars;
	hlChars.clear();

	for (unsigned int textpos = 0; textpos < name.size(); ++textpos) {
		if (name[textpos] == c) {
			hlChars.push_back(textpos);
			++charpos;
			if (charpos == text.size()) {
				// All chars found.
				m_items.push_back(aItem(action, hlChars));
				break;
			}
			else c = text[charpos];
		}
	}
}

int GotoFileDlg::ActionList::FindPath(const wxString& path) const {
	for (unsigned int i = 0; i < m_items.size(); ++i) {
		if (m_items[i].action->path == path) return i;
	}

	return wxNOT_FOUND;
}

const GotoFileDlg::FileEntry* GotoFileDlg::ActionList::GetSelectedAction() {
	const int sel = GetSelection();
	if (sel == -1) return NULL;
	else return m_items[sel].action;
}

// --- aItem --------------------------------------------------------

GotoFileDlg::ActionList::aItem::aItem(const FileEntry* a, const vector<unsigned int>& hl)
: action(a), hlChars(hl) {
	// Calculate rank (total distance between chars)
	rank = 0;
	if (hlChars.size() > 1) {
		unsigned int prev = hlChars[0]+1;
		for (vector<unsigned int>::const_iterator p = hlChars.begin()+1; p != hlChars.end(); ++p) {
			rank += *p - prev;
			prev = *p+1;
		}
	}
}

void GotoFileDlg::ActionList::aItem::swap(aItem& ai) {
	const FileEntry* const tempAction = action;
	action = ai.action;
	ai.action = tempAction;

	const unsigned int tempRank = rank;
	rank = ai.rank;
	ai.rank = tempRank;

	hlChars.swap(ai.hlChars);
}
