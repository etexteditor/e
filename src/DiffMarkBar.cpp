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
	const unsigned int scrollArrowY = m_editor->HasScrollbar() ? wxSystemSettings::GetMetric(wxSYS_VSCROLL_ARROW_Y) : 0;
	const unsigned int sizeY = size.y - (scrollArrowY * 2);
	const unsigned int docY = lines.GetHeight();

	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	
	// Draw the markers
	for (std::vector<DiffBar::LineMatch>::const_iterator p = m_lineMatches.begin(); p != m_lineMatches.end(); ++p) {
		const unsigned int leftTop = lines.GetYPosFromLine(m_isLeft ? p->left_start : p->right_start);
		const unsigned int leftBottom = lines.GetBottomYPosFromLine(m_isLeft ? p->left_end-1 : p->right_end-1);

		const unsigned int y = scrollArrowY + ((leftTop * sizeY) / docY);
		const unsigned int linesHeight = leftBottom - leftTop;
		const unsigned int height = linesHeight ? sizeY / ( docY / linesHeight) : 1;

		const cxChangeType type = m_isLeft ? p->left_type : p->right_type;
		dc.SetBrush(type == cxDELETION ? m_delColor : m_insColor);
		dc.SetPen(type == cxDELETION ? m_delBorder : m_insBorder);

		dc.DrawRectangle(0, y, size.x, height);
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
