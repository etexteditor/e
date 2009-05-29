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

#ifndef __THEMEEDITOR_H__
#define __THEMEEDITOR_H__

#ifdef __WXMSW__
    #pragma warning(disable: 4786)
#endif

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/grid.h>

#include "plistHandler.h"

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

class ITmThemeHandler;

class ThemeEditor : public wxDialog {
public:
	ThemeEditor(wxWindow *parent, ITmThemeHandler& syntaxHandler);
	~ThemeEditor();

	static bool ParseColour(const char* text, wxColour& colour, unsigned int& alpha);
	static vector<char> WriteColour(const wxColour& colour, unsigned int& alpha);

	void OnSelectorKillFocus(); // called by selectorCtrl

private:
	void Clear();
	void SetTheme(const PListHandler::cxItemRef& themeRef, bool init=false);
	void SizeGrid();
	void EnableCtrls();

	void SetSettingColour(unsigned int ndx, const char* id, const wxColour& colour, unsigned int alpha);
	void SetSettingFontStyle(unsigned int ndx, bool bold, bool italic, bool underline);
	void SetSettingName(unsigned int ndx, const wxString& name);
	void SetSettingScope(unsigned int ndx, const wxString& scope);

	bool AskForColour(wxColour& colour, unsigned int& alpha);
	void NotifyThemeChanged();

	// Event handlers
	void OnFontSelect(wxCommandEvent& event);
	void OnFontQuality(wxCommandEvent& event);
	void OnThemeSelected(wxCommandEvent& event);
	void OnNewTheme(wxCommandEvent& event);
	void OnDelTheme(wxCommandEvent& event);
	void OnButtonFg(wxCommandEvent& event);
	void OnButtonBg(wxCommandEvent& event);
	void OnButtonSel(wxCommandEvent& event);
	void OnButtonInv(wxCommandEvent& event);
	void OnButtonLine(wxCommandEvent& event);
	//void OnButtonCaret(wxCommandEvent& event);
	void OnButtonSearchHL(wxCommandEvent& event);
	void OnButtonBracketHL(wxCommandEvent& event);
	void OnButtonGutter(wxCommandEvent& event);
	void OnButtonMulti(wxCommandEvent& event);
	void OnNewSetting(wxCommandEvent& event);
	void OnDelSetting(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnGridSelect(wxGridEvent& event);
	void OnGridLeftDClick(wxGridEvent& event);
	void OnGridCellChange(wxGridEvent& event);
	DECLARE_EVENT_TABLE();

	class ColourButton : public wxBitmapButton {
	public:
		ColourButton(wxWindow* parent, wxWindowID id);
		void Clear();
		void SetColour(const wxColour& colour, unsigned int alpha);
		void SetColour(const wxColour& colour);
		void SetAlpha(unsigned int alpha);
		unsigned int GetAlpha() const {return m_alpha;};
		const wxColour& GetColour() const {return m_bgColour;};
	private:
		void DrawButton();
		wxColour m_bgColour;
		unsigned int m_alpha;
	};

	class TransparencyDlg : public wxDialog {
	public:
		TransparencyDlg(wxWindow *parent, unsigned int alpha);
		unsigned int GetAlpha() const {return m_slider->GetValue();};
	private:
		wxSlider* m_slider;
	};

	class ColourCellRenderer : public wxGridCellRenderer {
	public:
		ColourCellRenderer(const PListDict& themeDict) : m_themeDict(themeDict) {};
		void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
		wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col);
		wxGridCellRenderer* Clone() const;
	private:
		const PListDict& m_themeDict;
	};

	class FocusTextCtrl : public wxTextCtrl {
	public:
		FocusTextCtrl(ThemeEditor& parent, wxWindowID id);
	private:
		void OnKillFocus(wxFocusEvent& event);
		DECLARE_EVENT_TABLE();
		ThemeEditor& m_parentDlg;
	};

	// Member variables
	ITmThemeHandler& m_syntaxHandler;
	PListHandler& m_plistHandler;
	PListDict m_themeDict;
	vector<PListHandler::cxItemRef> m_themes;
	int m_themeNdx;
	int m_currentRow;

	// Member Ctrls
	wxTextCtrl* m_fontDesc;
	wxListBox* m_themeList;
	wxGrid* m_grid;
	FocusTextCtrl* m_selectorCtrl;
	ColourButton* m_fgButton;
	ColourButton* m_bgButton;
	ColourButton* m_selButton;
	ColourButton* m_invButton;
	ColourButton* m_lineButton;
	//ColourButton* m_caretButton;
	ColourButton* m_gutterButton;
	ColourButton* m_searchButton;
	ColourButton* m_multiButton;
	ColourButton* m_bracketButton;
	wxButton* m_gridPlus;
	wxButton* m_gridMinus;
	wxButton* m_themePlus;
	wxButton* m_themeMinus;
	wxComboBox* m_qualityCombo;
};

#endif // __THEMEEDITOR_H__
