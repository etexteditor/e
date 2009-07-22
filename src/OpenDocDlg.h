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

#ifndef __OPENDOCDLG_H__
#define __OPENDOCDLG_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "Catalyst.h"
#include <wx/listctrl.h>

class OpenDocDlg : public wxDialog {
public:
	OpenDocDlg(wxWindow *parent, const CatalystWrapper cw);

	bool HasSelection() const;
	doc_id GetSelectedDoc() const;

private:
	class CmpName {
	public:
		CmpName(const Catalyst& catalyst) : m_catalyst(catalyst) {};
		bool operator()(const doc_id& x, const doc_id& y) const;
	private:
		const Catalyst& m_catalyst;
	};

	class CmpDate {
	public:
		CmpDate(const Catalyst& catalyst) : m_catalyst(catalyst) {};
		bool operator()(const doc_id& x, const doc_id& y) const;
	private:
		const Catalyst& m_catalyst;
	};

	class CmpLabel {
	public:
		CmpLabel(const Catalyst& catalyst) : m_catalyst(catalyst) {};
		bool operator()(const doc_id& x, const doc_id& y) const;
	private:
		const Catalyst& m_catalyst;
	};

	class CmpDesc {
	public:
		CmpDesc(const Catalyst& catalyst) : m_catalyst(catalyst) {};
		bool operator()(const doc_id& x, const doc_id& y) const;
	private:
		const Catalyst& m_catalyst;
	};

	class DocList : public wxListCtrl {
	public:
		DocList(wxWindow *parent, int id, const CatalystWrapper cw);

		doc_id GetSelectedDoc();
	protected:
		wxString OnGetItemText(long item, long column) const;
	private:
		void ResizeColumns();

		// Event handlers
		void OnListColClick(wxListEvent& event);
		void OnListColDrag(wxListEvent& event);
		void OnSize(wxSizeEvent& event);
		DECLARE_EVENT_TABLE();

		const CatalystWrapper m_catalyst;
		vector<doc_id> m_docs;
	};

	// Event handlers
	void OnListItemActivate(wxListEvent& event);
	DECLARE_EVENT_TABLE();

	const CatalystWrapper m_catalyst;
	DocList* m_docListCtrl;
};

#endif // __OPENDOCDLG_H_
