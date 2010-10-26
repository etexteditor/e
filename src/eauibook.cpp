#include "eauibook.h"
#include <wx/log.h>

BEGIN_EVENT_TABLE(eAuiNotebook, wxAuiNotebook)
	EVT_MOUSEWHEEL(eAuiNotebook::OnMouseWheel)
END_EVENT_TABLE()

// This class redefined here as a workaround for not having
// access to the original wxTabFrame definition in auibook.cpp
// Doing it like this is a fragile hack, but it lets us
// avoid patching wxWidgets.
class wxTabFrame : public wxWindow
{
public:
    wxTabFrame();
    ~wxTabFrame();
    void SetTabCtrlHeight(int h);
    void DoSetSize(int x, int y,
                   int width, int height,
                   int WXUNUSED(sizeFlags = wxSIZE_AUTO));
    void DoGetClientSize(int* x, int* y) const;
    bool Show( bool WXUNUSED(show = true) );
    void DoSizing();
    void DoGetSize(int* x, int* y) const;
    void Update();

public:
    wxRect m_rect;
    wxRect m_tab_rect;
    wxAuiTabCtrl* m_tabs;
    int m_tab_ctrl_height;
};

// Constructor
eAuiNotebook::eAuiNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
	wxAuiNotebook(parent, id, pos, size, style) {}

// Convert page index to tab position
int eAuiNotebook::PageToTab(size_t page_idx) {
    wxWindow* page = m_tabs.GetWindowFromIdx(page_idx);

    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();
	size_t offset = 0;

    for (size_t i = 0; i < pane_count; ++i)
    {
		if (all_panes.Item(i).name == wxT("dummy"))
            continue;

        wxTabFrame* tabframe = (wxTabFrame*)all_panes.Item(i).window;
		
		const int page_idx = tabframe->m_tabs->GetIdxFromWindow(page);
        if (page_idx != -1)
			return offset + page_idx;

		offset += tabframe->m_tabs->GetPageCount();
	}

	wxASSERT(false); // invalid page_idx
	return -1;
}

// Convert tab position to page index
int eAuiNotebook::TabToPage(size_t tab_idx) {
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();
	size_t offset = 0;

    for (size_t i = 0; i < pane_count; ++i)
    {
		if (all_panes.Item(i).name == wxT("dummy"))
            continue;

        wxTabFrame* tabframe = (wxTabFrame*)all_panes.Item(i).window;
		const size_t tab_count = tabframe->m_tabs->GetPageCount();

		if (offset + tab_count <= tab_idx) offset += tab_count;
		else {
			wxWindow* page =  tabframe->m_tabs->GetWindowFromIdx(tab_idx-offset);
			return m_tabs.GetIdxFromWindow(page);
		}
	}

	wxASSERT(false); // we should never reach here
	return -1;
}

wxString eAuiNotebook::SavePerspective() {
	// Build list of panes/tabs
	wxString tabs;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();

    for (size_t i = 0; i < pane_count; ++i)
    {
		wxAuiPaneInfo& pane = all_panes.Item(i);
		if (pane.name == wxT("dummy"))
            continue;

        wxTabFrame* tabframe = (wxTabFrame*)pane.window;
		
		if (!tabs.empty()) tabs += wxT("|");
		tabs += pane.name;
		tabs += wxT("=");
		
		// add tab id's
		size_t page_count = tabframe->m_tabs->GetPageCount();
		for (size_t p = 0; p < page_count; ++p)
		{
			wxAuiNotebookPage& page = tabframe->m_tabs->GetPage(p);
			const size_t page_idx = m_tabs.GetIdxFromWindow(page.window);
			
			if (p) tabs += wxT(",");

			if ((int)page_idx == m_curpage) tabs += wxT("*");
			else if ((int)p == tabframe->m_tabs->GetActivePage()) tabs += wxT("+");
			tabs += wxString::Format(wxT("%u"), page_idx);
		}
	}
	tabs += wxT("@");

	// Add frame perspective
	tabs += m_mgr.SavePerspective();

	return tabs;
}

bool eAuiNotebook::LoadPerspective(const wxString& layout) {
	// Remove all tab ctrls (but still keep them in main index)
	const size_t tab_count = m_tabs.GetPageCount();
	for (size_t i = 0; i < tab_count; ++i) {
		wxWindow* wnd = m_tabs.GetWindowFromIdx(i);

		// find out which onscreen tab ctrl owns this tab
		wxAuiTabCtrl* ctrl;
		int ctrl_idx;
		if (!FindTab(wnd, &ctrl, &ctrl_idx))
			return false;

		// remove the tab from ctrl
		if (!ctrl->RemovePage(wnd))
			return false;
	}
	RemoveEmptyTabFrames();

	size_t sel_page = 0;
	
	wxString tabs = layout.BeforeFirst(wxT('@'));
	while (1)
    {
		const wxString tab_part = tabs.BeforeFirst(wxT('|'));
		
		// if the string is empty, we're done parsing
        if (tab_part.empty())
            break;

		// Get pane name
		const wxString pane_name = tab_part.BeforeFirst(wxT('='));

		// create a new tab frame
		wxTabFrame* new_tabs = new wxTabFrame;
		new_tabs->m_tabs = new wxAuiTabCtrl(this,
											m_tab_id_counter++,
											wxDefaultPosition,
											wxDefaultSize,
											wxNO_BORDER|wxWANTS_CHARS);
		new_tabs->m_tabs->SetArtProvider(m_tabs.GetArtProvider()->Clone());
		new_tabs->SetTabCtrlHeight(m_tab_ctrl_height);
		new_tabs->m_tabs->SetFlags(m_flags);
		wxAuiTabCtrl *dest_tabs = new_tabs->m_tabs;

		// create a pane info structure with the information
		// about where the pane should be added
		wxAuiPaneInfo pane_info = wxAuiPaneInfo().Name(pane_name).Bottom().CaptionVisible(false);
		m_mgr.AddPane(new_tabs, pane_info);

		// Get list of tab id's and move them to pane
		wxString tab_list = tab_part.AfterFirst(wxT('='));
		while(1) {
			wxString tab = tab_list.BeforeFirst(wxT(','));
			if (tab.empty()) break;
			tab_list = tab_list.AfterFirst(wxT(','));

			// Check if this page has an 'active' marker
			const wxChar c = tab[0];
			if (c == wxT('+') || c == wxT('*')) {
				tab = tab.Mid(1); 
			}

			const size_t tab_idx = wxAtoi(tab.c_str());
			if (tab_idx >= GetPageCount()) continue;

			// Move tab to pane
			wxAuiNotebookPage& page = m_tabs.GetPage(tab_idx);
			const size_t newpage_idx = dest_tabs->GetPageCount();
			dest_tabs->InsertPage(page.window, page, newpage_idx);

			if (c == wxT('+')) dest_tabs->SetActivePage(newpage_idx);
			else if ( c == wxT('*')) sel_page = tab_idx;
		}
		dest_tabs->DoShowHide();

		tabs = tabs.AfterFirst(wxT('|'));
	}
	
	// Load the frame perspective
	const wxString frames = layout.AfterFirst(wxT('@'));
	m_mgr.LoadPerspective(frames);

	// Force refresh of selection
	m_curpage = -1;
	SetSelection(sel_page);

	return true;
}

// This code is based largely on the following patch, but modified to work with the wrap_around parameter
// http://trac.wxwidgets.org/attachment/ticket/10848/wxAuiNotebook.patch
void eAuiNotebook::ChangeTab(bool forward, bool wrap_around) {
	if (GetPageCount() <= 1) return;

	int currentSelection = this->GetSelection();
	wxAuiTabCtrl* tabCtrl = 0; 
	int idx = -1; 

	if(!FindTab(GetPage(currentSelection), &tabCtrl, &idx)) return; 
	if(!tabCtrl || idx < 0) return;

	wxWindow* page = 0; 
	int pageCount = tabCtrl->GetPageCount(); 

	forward ? idx++ : idx--;

	if(idx < 0) {
		idx = wrap_around ? pageCount - 1 : 0;
	} else if(idx >= pageCount) {
		idx = wrap_around ? 0 : pageCount-1;
	}

	page = tabCtrl->GetPage(idx).window;
	if(page) { 
		currentSelection = GetPageIndex(page); 
		SetSelection(currentSelection); 
	} 
}

void eAuiNotebook::SelectNextTab(bool wrap_around) {
	ChangeTab(true, wrap_around);
}

void eAuiNotebook::SelectPrevTab(bool wrap_around) {
	ChangeTab(false, wrap_around);
}

void eAuiNotebook::OnMouseWheel(wxMouseEvent& event) {
	const wxCoord x = event.GetX();
	const wxCoord y = event.GetY();

	bool hit = ((0 <= y) && (y < this->m_tab_ctrl_height) &&
					(0 <= x) && (x < this->GetSize().GetWidth()));

	if (!hit) {
		event.Skip(true);
		return;
	}

	const int rotation = event.GetWheelRotation();
	wxLogDebug(_("Rotation: %d"), rotation);

	// Don't use wrap-around tab selection when mouse-wheeling
	if (rotation < 0) SelectNextTab(false);
	else if (rotation > 0) SelectPrevTab(false);
}
