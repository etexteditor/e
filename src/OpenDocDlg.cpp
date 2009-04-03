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

#include "OpenDocDlg.h"
#include <algorithm>

// Ctrl id's
enum {
	CTRL_DOCLIST = 100
};

BEGIN_EVENT_TABLE(OpenDocDlg, wxDialog)
	EVT_LIST_ITEM_ACTIVATED(CTRL_DOCLIST, OpenDocDlg::OnListItemActivate)
END_EVENT_TABLE()

OpenDocDlg::OpenDocDlg(wxWindow *parent, const CatalystWrapper cw)
: wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
  m_catalyst(cw), m_docListCtrl(0)
{
	SetTitle (_("Retrieve Document"));

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	// List of documents
	m_docListCtrl = new DocList(this, CTRL_DOCLIST, m_catalyst);
	sizer->Add(m_docListCtrl, 1, wxEXPAND|wxTOP|wxLEFT|wxRIGHT, 5);

	// Buttons
	sizer->Add(CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND|wxALL, 5);

	// Add columns to listCtrl
	wxListItem itemCol;
	itemCol.SetText(_("Name"));
	m_docListCtrl->InsertColumn(0, itemCol);

	itemCol.SetText(_("Date"));
	m_docListCtrl->InsertColumn(1, itemCol);

	itemCol.SetText(_("Label"));
	m_docListCtrl->InsertColumn(2, itemCol);

	itemCol.SetText(_("Comment"));
	m_docListCtrl->InsertColumn(3, itemCol);

	m_docListCtrl->SetFocus();
	SetSizerAndFit(sizer);
	SetSize(400, 450);
	Centre();
}

bool OpenDocDlg::HasSelection() const {
	return m_docListCtrl->GetSelectedItemCount() == 1;
}

doc_id OpenDocDlg::GetSelectedDoc() const {
	wxASSERT(m_docListCtrl);
	return m_docListCtrl->GetSelectedDoc();
}

void OpenDocDlg::OnListItemActivate(wxListEvent& WXUNUSED(event)) {
	EndModal(wxID_OK);
}


BEGIN_EVENT_TABLE(OpenDocDlg::DocList, wxListCtrl)
	EVT_LIST_COL_CLICK(100, OpenDocDlg::DocList::OnListColClick)
	EVT_LIST_COL_DRAGGING(100, OpenDocDlg::DocList::OnListColDrag)
	EVT_SIZE(OpenDocDlg::DocList::OnSize)
END_EVENT_TABLE()

OpenDocDlg::DocList::DocList(wxWindow *parent, int id, const CatalystWrapper cw)
: wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VIRTUAL),
  m_catalyst(cw)
{
	cxLOCK_READ(m_catalyst)
		catalyst.GetDocList(m_docs);
		sort(m_docs.begin(), m_docs.end(), CmpName(catalyst));
	cxENDLOCK

	SetItemCount(m_docs.size());
}

doc_id OpenDocDlg::DocList::GetSelectedDoc() {
	wxASSERT(GetSelectedItemCount() == 1);

	const long item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

	if (item != -1) return m_docs[item];
	else {
		wxASSERT(false);
		return doc_id();
	}
}

wxString OpenDocDlg::DocList::OnGetItemText(long item, long column) const {
	wxASSERT(item >= 0 && item < (long)m_docs.size());

	cxLOCK_READ(m_catalyst)
		switch (column) {
		case 0: // Name
			{
				const wxString name = catalyst.GetDocName(m_docs[item]);
				if (name.empty()) return _("(untitled)");
				else return name;
			}
		case 1: // Date
			{
				const wxDateTime date = catalyst.GetDocDate(m_docs[item]);
				return date.Format();
			}
		case 2:
			return catalyst.GetLabel(m_docs[item]);
		case 3:
			return catalyst.GetDescription(m_docs[item]);
		default:
			wxASSERT(false);
			return wxString();
		}
	cxENDLOCK
}

void OpenDocDlg::DocList::OnListColClick(wxListEvent& event) {
	const int column = event.GetColumn();

	cxLOCK_READ(m_catalyst)
		switch (column) {
		case 0: // Name
			sort(m_docs.begin(), m_docs.end(), CmpName(catalyst));
			break;
		case 1: // Date
			sort(m_docs.begin(), m_docs.end(), CmpDate(catalyst));
			break;
		case 2: // Label
			sort(m_docs.begin(), m_docs.end(), CmpLabel(catalyst));
			break;
		case 3: // Comment
			sort(m_docs.begin(), m_docs.end(), CmpDesc(catalyst));
			break;
		case -1: // Clicked outside columns
			return;
		}
	cxENDLOCK

	if (!m_docs.empty()) RefreshItems(0, m_docs.size()-1);
}

void OpenDocDlg::DocList::ResizeColumns() {
	// Add up sizes of all columns before comment
	int comment_xpos = 0;
	for (unsigned int i = 0; i < 3; ++i) {
		comment_xpos += GetColumnWidth(i);
	}

	const wxSize size = GetClientSize();
	const int comment_width = wxMax(size.x - comment_xpos, 0);
	SetColumnWidth(3, comment_width);
}

void OpenDocDlg::DocList::OnListColDrag(wxListEvent& WXUNUSED(event)) {
	ResizeColumns();
}

void OpenDocDlg::DocList::OnSize(wxSizeEvent& WXUNUSED(event)) {
	ResizeColumns();
}

bool OpenDocDlg::CmpName::operator()(const doc_id& x, const doc_id& y) const {
	const wxString name1 = m_catalyst.GetDocName(x);
	const wxString name2 = m_catalyst.GetDocName(y);

	return name1 < name2;
}

bool OpenDocDlg::CmpDate::operator()(const doc_id& x, const doc_id& y) const {
	const wxDateTime date1 = m_catalyst.GetDocDate(x);
	const wxDateTime date2 = m_catalyst.GetDocDate(y);

	return date1 < date2;
}

bool OpenDocDlg::CmpLabel::operator()(const doc_id& x, const doc_id& y) const {
	const wxString name1 = m_catalyst.GetLabel(x);
	const wxString name2 = m_catalyst.GetLabel(y);

	return name1 < name2;
}

bool OpenDocDlg::CmpDesc::operator()(const doc_id& x, const doc_id& y) const {
	const wxString name1 = m_catalyst.GetDescription(x);
	const wxString name2 = m_catalyst.GetDescription(y);

	return name1 < name2;
}
