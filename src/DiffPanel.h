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

#ifndef __DIFFPANEL_H__
#define __DIFFPANEL_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Catalyst.h"
#include "ITabPage.h"

// Pre-definitions
class EditorFrame;
class EditorCtrl;
class DiffBar;
class DiffMarkBar;
class wxGridBagSizer;

class DiffPanel : public wxPanel, public ITabPage {
public:
	DiffPanel(); // default const
	DiffPanel(wxWindow* parent, EditorFrame& parentFrame, CatalystWrapper& cw, wxBitmap& bitmap);
	~DiffPanel();

	bool Show(bool show=true);

	void SetDiff(const wxString& leftPath, const wxString& rightPath);
	void UpdateMarkBars();

	virtual void SaveSettings(unsigned int i, eSettings& settings);
	void RestoreSettings(unsigned int i, eSettings& settings);

	virtual EditorCtrl* GetActiveEditor();
	virtual const char** RecommendedIcon() const;

private:
	// Event handlers
	void OnButtonSwap(wxCommandEvent& event);
	void OnChildFocus(wxChildFocusEvent& event);
	DECLARE_EVENT_TABLE();

	// Member variables
	EditorFrame* m_parentFrame;
	EditorCtrl* m_leftEditor;
	EditorCtrl* m_rightEditor;
	EditorCtrl* m_currentEditor;
	DiffBar* m_diffBar;
	wxTextCtrl* m_leftTitle;
	wxTextCtrl* m_rightTitle;
	DiffMarkBar* m_leftMarkBar;
	DiffMarkBar* m_rightMarkBar;
	wxGridBagSizer* m_mainSizer;

	DECLARE_DYNAMIC_CLASS_NO_COPY(DiffPanel)
};

#endif // __DIFFPANEL_H__
