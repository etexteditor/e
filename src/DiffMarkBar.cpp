#include "DiffMarkBar.h"
#include "EditorCtrl.h"
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(DiffMarkBar, wxControl)
	EVT_PAINT(DiffMarkBar::OnPaint)
	EVT_SIZE(DiffMarkBar::OnSize)
	EVT_ERASE_BACKGROUND(DiffMarkBar::OnEraseBackground)
END_EVENT_TABLE()

DiffMarkBar::DiffMarkBar(wxWindow* parent, const vector<DiffBar::LineMatch>& lineMatches, EditorCtrl* editor, bool isLeft):
	wxControl(parent, wxID_ANY, wxPoint(-100,-100), wxDefaultSize, wxNO_BORDER|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE),
	m_lineMatches(lineMatches), m_editor(editor), m_isLeft(isLeft) 
{
	// Fix width
	SetMinSize(wxSize(10, -1));
	SetMaxSize(wxSize(10, -1));

	// Marking Colors
	m_insColor.Set(192, 255, 192); // PASTEL GREEN
	m_delColor.Set(255, 192, 192); // PASTEL RED
	m_insBorder.Set(162, 225, 162);
	m_delBorder.Set(225, 162, 162);

	SetBackgroundStyle(wxBG_STYLE_CUSTOM); // For wxAutoBufferedPaintDC
}

void DiffMarkBar::DrawLayout(wxDC& dc) {
	const wxSize size = GetClientSize();
	Lines& lines = m_editor->GetLines();
	const unsigned int lineCount = lines.GetLineCount();

	// We have to adjust for the up and down buttons on the scrollbar
	const unsigned int scrollArrowY = m_editor->HasScrollbar() ? wxSystemSettings::GetMetric(wxSYS_VSCROLL_ARROW_Y) : 0;
	const unsigned int sizeY = size.y - (scrollArrowY * 2);
	
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	
	// Draw the markers
	for (std::vector<DiffBar::LineMatch>::const_iterator p = m_lineMatches.begin(); p != m_lineMatches.end(); ++p) {
		const unsigned int start = m_isLeft ? p->left_start : p->right_start;
		const unsigned int end = m_isLeft ? p->left_end : p->right_end;

		// Calculate relative positions
		const unsigned int y = (sizeY * start) / lineCount;
		const unsigned int height = (start < end) ? (((sizeY * end) / lineCount) - y) : 1;

		const cxChangeType type = m_isLeft ? p->left_type : p->right_type;
		dc.SetBrush(type == cxDELETION ? m_delColor : m_insColor);
		dc.SetPen(type == cxDELETION ? m_delBorder : m_insBorder);

		dc.DrawRectangle(0, scrollArrowY + y, size.x, height);
	}
}

void DiffMarkBar::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxAutoBufferedPaintDC dc(this);
	DrawLayout(dc);
}

void DiffMarkBar::OnSize(wxSizeEvent& WXUNUSED(event)) {
	wxClientDC dc(this);
	DrawLayout(dc);
}

// # no evt.skip() as we don't want the control to erase the background
void DiffMarkBar::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

void DiffMarkBar::SetEditor( EditorCtrl* editor )
{
	m_editor = editor;
}
