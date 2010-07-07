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

#ifndef __CLIPBOARDHISTORYPANE_H__
#define __CLIPBOARDHISTORYPANE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/panel.h>
    #include <wx/textctrl.h>
#endif

class EditorFrame;
#include "SearchListBox.h"
#include <vector>

class ClipboardHistoryPane : public wxPanel {
public:
	ClipboardHistoryPane(EditorFrame& editorFrame, bool keepOpen=true);
	bool Destroy();
	void AddCopyText(wxString copytext);

private:
	void OnAction(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();

	class ActionList : public SearchListBox {
	public:
		ActionList(wxWindow* parent, wxWindowID id, std::vector<wxString>& clipboardHistory);
		void SetAllItems();

	private:
		void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;

		class aItem {
		public:
			aItem() : id(0), text(NULL) {};
			aItem(unsigned int id, const wxString* a, const std::vector<unsigned int>& hl)
				: id(id), text(a), hlChars(hl) {};
			unsigned int id;
			const wxString* text;
			std::vector<unsigned int> hlChars;
		};

		std::vector<wxString>& m_clipboardHistory;
		std::vector<aItem> m_items;
	};


	// Member variables
	EditorFrame& m_parentFrame;
	ActionList* m_listBox;

	bool m_keepOpen;
	std::vector<wxString> m_clipboardHistory;
};

#endif // __CLIPBOARDHISTORYPANE_H__
