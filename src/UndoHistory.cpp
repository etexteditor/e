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

#include "UndoHistory.h"
#include <algorithm>
#include "IFrameUndoPane.h"
#include "EditorCtrl.h"
#include "VersionTree.h"

enum UndoHistory_IDs {
	CTRL_VERSIONTREE = 1,
	MENU_DIFF_TO_CURRENT
};

BEGIN_EVENT_TABLE(UndoHistory, wxControl)
	EVT_PAINT(UndoHistory::OnPaint)
	EVT_SIZE(UndoHistory::OnSize)
	EVT_IDLE(UndoHistory::OnIdle)
	EVT_CHAR(UndoHistory::OnChar)
	EVT_ERASE_BACKGROUND(UndoHistory::OnEraseBackground)
	EVT_SCROLLWIN(UndoHistory::OnScroll)
	EVT_MOUSEWHEEL(UndoHistory::OnMouseWheel)
	EVT_VERSIONTREE_SEL_CHANGED(CTRL_VERSIONTREE, UndoHistory::OnVersionTreeSel)
	EVT_VERSIONTREE_CONTEXTMENU(CTRL_VERSIONTREE, UndoHistory::OnVersionTreeContextMenu)
	EVT_VERSIONTREE_TOOLTIP(CTRL_VERSIONTREE, UndoHistory::OnVersionTreeTooltip)
	EVT_VERSIONTREE_DRAWITEM(CTRL_VERSIONTREE, UndoHistory::OnVersionTreeDrawitem)
	EVT_MENU(MENU_DIFF_TO_CURRENT, UndoHistory::OnMenuDiffToCurrent)
END_EVENT_TABLE()

UndoHistory::UndoHistory(CatalystWrapper& cw, IFrameUndoPane* parentFrame, int win_id, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size):
	wxControl(parent, id, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE|wxHSCROLL),
	m_catalyst(cw), m_doc(cw), m_dispatcher(cw.GetDispatcher()), m_mdc(), m_bitmap(1,1), m_cell(m_mdc, m_doc), 
	m_ignoreUpdates(false), m_editorCtrl(NULL), m_parentFrame(parentFrame), m_source_win_id(win_id)
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Avoid flicker

	// Initialize variables
	m_needRedrawing = true; // Make sure the ctrl gets drawn on first idle event
	m_verticalScrollPos = 0;
	m_horizontalScrollPos = 0;
	m_isScrolling = false;
	m_lineHeight = 18;

	bgBrush = *wxWHITE_BRUSH;
	linePen = *wxGREY_PEN;

	m_pTree = new VersionTree(this, CTRL_VERSIONTREE);

	// Initialize the memoryDC for dubblebuffering
	m_mdc.SelectObject(m_bitmap);
	m_mdc.SetFont(wxFont(9, wxMODERN, wxNORMAL, wxNORMAL, false));

	// Make sure we recieve notifications of new versions and updates
	m_dispatcher.SubscribeC(wxT("WIN_CLOSEPAGE"), (CALL_BACK)OnClosePage, this);
	m_dispatcher.SubscribeC(wxT("WIN_CHANGEDOC"), (CALL_BACK)OnChangeDoc, this);
	m_dispatcher.SubscribeC(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);
	m_dispatcher.SubscribeC(wxT("DOC_NEWREVISION"), (CALL_BACK)OnNewRevision, this);
	m_dispatcher.SubscribeC(wxT("DOC_UPDATEREVISION"), (CALL_BACK)OnUpdateRevision, this);
	m_dispatcher.SubscribeC(wxT("DOC_UPDATED"), (CALL_BACK)OnDocUpdated, this);
	m_dispatcher.SubscribeC(wxT("DOC_COMMITED"), (CALL_BACK)OnDocCommited, this);
}

void UndoHistory::SetEditorCtrl(EditorCtrl* ctrl) {
	m_editorCtrl = ctrl;
}

UndoHistory::~UndoHistory() {
	m_dispatcher.UnSubscribe(wxT("WIN_CLOSEPAGE"), (CALL_BACK)OnClosePage, this);
	m_dispatcher.UnSubscribe(wxT("WIN_CHANGEDOC"), (CALL_BACK)OnChangeDoc, this);
	m_dispatcher.UnSubscribe(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);
	m_dispatcher.UnSubscribe(wxT("DOC_NEWREVISION"), (CALL_BACK)OnNewRevision, this);
	m_dispatcher.UnSubscribe(wxT("DOC_UPDATEREVISION"), (CALL_BACK)OnUpdateRevision, this);
	m_dispatcher.UnSubscribe(wxT("DOC_UPDATED"), (CALL_BACK)OnDocUpdated, this);
	m_dispatcher.UnSubscribe(wxT("DOC_COMMITED"), (CALL_BACK)OnDocCommited, this);
}

void UndoHistory::Clear() {
	m_editorCtrl = NULL;
	m_sourceDoc.Invalidate();
	m_pTree->Clear();
	m_verticalScrollPos = 0;
	m_horizontalScrollPos = 0;
	m_needRedrawing = true;
}

void UndoHistory::SetDocument(const doc_id& di, bool doCenter) {
	if (!di.IsOk()) {
		wxFAIL_MSG(wxT("UndoHistory::SetDocument - invalid docid"));
		Clear();
		return;
	}

	m_needRedrawing = true;
	if (m_sourceDoc == di) return;

	cxLOCKDOC_WRITE(m_doc)
		doc.SetDocument(di);
	cxENDLOCK
	m_sourceDoc = di;

	// If we are doing selective undo, the we do not
	// want the tree to be updated until after we are done.
	if (m_ignoreUpdates) return;
	
	// Clear selection info
	m_range.Set(0, 0);
	m_rangeHistory.clear();
	
	UpdateTree(doCenter);
}

bool UndoHistory::Show(bool show) {
	bool result = wxControl::Show(show);
	if (show) UpdateTree(); // not updated when hidden

	return result;
}

void UndoHistory::UpdateTree(bool doCenter) {
	if (!IsShown()) return;

	// Update the VersionTree
	m_pTree->Clear();
	if (m_sourceDoc.IsDraft()) {
		cxLOCKDOC_READ(m_doc)
			const int nodecount = doc.GetVersionCount();
			if (nodecount) m_pTree->AddRoot();

			for(int i = 1; i < nodecount; ++i)
				m_pTree->AddNode(doc.GetDraftParent(i).version_id);
		cxENDLOCK
		m_selectedNode = m_sourceDoc.version_id;
	}
	else {
		m_pTree->AddRoot(); // document has no undo history
		m_selectedNode = 0;
	}
	
	// Make the version visible
	m_pTree->Select(m_selectedNode);
	m_pTree->MakeNodeVisible(m_selectedNode, doCenter);
	m_verticalScrollPos = m_pTree->GetVerticalScrollPos();
	m_horizontalScrollPos = m_pTree->GetHorizontalScrollPos();

	m_needRedrawing = true;
}

void UndoHistory::DrawLayout(wxDC& dc) {
	if (!IsShown()) {
		// Sometimes OnSize() might get called while control is hidden
		m_needRedrawing = true;
		return;
	}

	const wxSize size = GetClientSize();
	const size_t lineCount = m_pTree->GetNodeCount();
	m_treeHeight = (int)lineCount * m_lineHeight;
	m_treeWidth = m_pTree->GetWidth();

	wxRect rect;
	//If an image is moved both horizontally and vertically, then there is no way to capture the area to be redrawn in a single rectangle.
	//So, this optimization is only used if the vertical scrollbar is used.
	if (m_isScrolling) {
		// If there is overlap, then move the image
		if (m_verticalScrollPos + size.y > m_oldVerticalScrollPos && m_verticalScrollPos < m_oldVerticalScrollPos + size.y) {
			const int top = wxMax(m_verticalScrollPos, m_oldVerticalScrollPos);
			const int bottom = wxMin(m_verticalScrollPos, m_oldVerticalScrollPos) + size.y;
			const int overlap_height = bottom - top;
			m_mdc.Blit(m_horizontalScrollPos, (top - m_oldVerticalScrollPos) + (m_oldVerticalScrollPos - m_verticalScrollPos),  size.x, overlap_height, &m_mdc, m_horizontalScrollPos, top - m_oldVerticalScrollPos);

			const int new_height = size.y - overlap_height;
			const int newtop = (top == m_verticalScrollPos) ? bottom : m_verticalScrollPos;
			rect = wxRect(m_horizontalScrollPos,newtop,size.x,new_height); // Just redraw the revealed part
		}
		else {
			rect = wxRect(m_horizontalScrollPos,m_verticalScrollPos,size.x,size.y);
		}

		SetScrollPos(wxVERTICAL, m_verticalScrollPos);
		SetScrollPos(wxHORIZONTAL, m_horizontalScrollPos);
		m_isScrolling = false;
	}
	else {
		// Check if we need a scrollbar
		bool hasScroll = (GetScrollThumb(wxVERTICAL) != 0);
		if (m_treeHeight > size.y) {
			SetScrollbar(wxVERTICAL, m_verticalScrollPos, size.y, m_treeHeight);
			if (!hasScroll) return; // have sent a size event
		}
		else {
			SetScrollbar(wxVERTICAL, 0, 0, 0);
			if (hasScroll) return; // have sent a size event
		}
		
		bool hasHorizontalScroll = (GetScrollThumb(wxHORIZONTAL) != 0);
		if (m_treeWidth > size.x) {
			SetScrollbar(wxHORIZONTAL, m_horizontalScrollPos, size.x, m_treeWidth);
			if (!hasHorizontalScroll) return; // have sent a size event
		}
		else {
			SetScrollbar(wxHORIZONTAL, 0, 0, 0);
			if (hasHorizontalScroll) return; // have sent a size event
		}

		// Can we make room for more of the history?
		if (m_verticalScrollPos + size.y > m_treeHeight) {
			m_verticalScrollPos = wxMax(0, m_treeHeight-size.y);
		}
		if (m_horizontalScrollPos + size.x > m_treeWidth) {
			m_horizontalScrollPos = wxMax(0, m_treeWidth-size.x);
		}
		rect = wxRect(m_horizontalScrollPos, m_verticalScrollPos, size.x, size.y);
	}

	// Resize & scroll the versiontree
	const wxSize treesize = m_pTree->GetBestSize();
	m_pTree->SetSize(wxSize(treesize.x, size.y));
	m_pTree->Scroll(m_horizontalScrollPos, m_verticalScrollPos);

	// Calculate visible node-range
	const int topnode = rect.y ? (rect.y-1) / (m_lineHeight) : 0;
	size_t endnode = (rect.GetBottom() / (m_lineHeight)) + 1;
	endnode = wxMin(endnode, lineCount);

	const int xpos = treesize.x;
	const int ypos = m_lineHeight * (int)lineCount;
	const int textpos = 3; // indent the text a bit
	const int textwidth = size.x - xpos;
	rect = wxRect(xpos, rect.y, textwidth, rect.height);

	// resize the bitmap used for doublebuffering
	if (m_bitmap.GetWidth() < textwidth || m_bitmap.GetHeight() < size.y) {
		m_bitmap = wxBitmap(textwidth, size.y);
		m_mdc.SelectObject(m_bitmap);
	}
	wxASSERT(m_mdc.Ok());

	// Clear the background
	m_mdc.SetBrush(bgBrush);
	m_mdc.SetPen(*wxWHITE_PEN);
	m_mdc.DrawRectangle(0, rect.y - m_verticalScrollPos, textwidth, rect.height);

	if (lineCount) {
		// Draw vertical seperator
		m_mdc.SetPen(linePen);
		m_mdc.DrawLine(0, 0, 0, ypos - m_verticalScrollPos);

		// Draw the diffs
		if (textwidth >= 0) m_cell.SetWidth(textwidth);
		for (int i = (int)topnode; i < (int)endnode; ++i) {
			if (textwidth > 0) {
				// Draw text colums
				m_mdc.SetPen(linePen);
				const int bottom = m_lineHeight * (i+1) - m_verticalScrollPos;
				m_mdc.DrawLine(0, bottom, textwidth, bottom);

				// Draw the diff
				const int top = 1 + m_lineHeight*i - m_verticalScrollPos;
				if (m_rangeHistory.empty()) {
					if (m_sourceDoc.IsDraft()) {
						doc_id di;
						cxLOCKDOC_READ(m_doc)
							di = doc.GetDraftParent(i);
						cxENDLOCK
						m_cell.CalcLayout(di, doc_id(DRAFT, m_sourceDoc.document_id, i));
					}
					else m_cell.CalcLayout(m_sourceDoc, m_sourceDoc);
				}
				else {
					const cxDiffEntry& de = m_rangeHistory[i];
					const doc_id child(DRAFT, m_sourceDoc.document_id, de.version);
					doc_id parent;
					cxLOCKDOC_READ(m_doc)
						parent = doc.GetDraftParent(de.version);
					cxENDLOCK
					m_cell.CalcLayoutRange(parent, child, de.range);
				}
				const wxRect rect(textpos, top, textwidth-textpos, m_lineHeight);
				m_cell.DrawCell(textpos, top, rect);
			}
		}
	}

	// Copy MemoryDC to Display
	dc.Blit(xpos, 0, textwidth, size.y, &m_mdc, 0, 0);

	m_needRedrawing = false;
}

void UndoHistory::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	if (m_needRedrawing) {
		//wxLogDebug(wxT("OnPaint Redraw"));
		DrawLayout(dc);
	}
	else {
		//wxLogDebug(wxT("OnPaint Blit"));
		// Re-Blit MemoryDC to Display
		wxSize size = GetClientSize();
		const wxSize treesize = m_pTree->GetSize();
		dc.Blit(treesize.x, 0,  size.x-treesize.x, size.y, &m_mdc, 0, 0);
	}
}

void UndoHistory::UpdateCaption(const wxChar* caption) {
	if (m_parentFrame) m_parentFrame->SetUndoPaneCaption(caption);
}

void UndoHistory::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	if (!IsShown() || !m_editorCtrl) return;

	// Get current selection(s)
	const vector<interval>& selections = m_editorCtrl->GetSelections();
	interval newrange(0,0);
	if (!selections.empty()) newrange = selections[0];

	// Check if selections have changed
	if (m_range != newrange) {
		m_rangeHistory.clear();
		m_pTree->Clear();

		if (selections.empty()) {
			UpdateTree();
			UpdateCaption(_("Undo"));
		}
		else {
			cxLOCKDOC_READ(m_doc)
				doc.PartialDiff(newrange, m_rangeHistory);
			cxENDLOCK

			// Create the version tree
			if (!m_rangeHistory.empty()) {
				m_pTree->AddRoot();
				for(size_t i = 1; i < m_rangeHistory.size(); ++i) {
					m_pTree->AddNode(m_rangeHistory[i].parent);
				}

				// Select last node
				m_selectedNode = m_rangeHistory.size()-1;
				m_pTree->Select(m_selectedNode);
			}
			UpdateCaption(_("Undo (selection)"));
		}

		m_range = newrange;
		m_needRedrawing = true;
	}

	if (m_needRedrawing) {
		wxClientDC dc(this);
		DrawLayout(dc);
	}
}

void UndoHistory::OnSize(wxSizeEvent& WXUNUSED(event)) {
	wxClientDC dc(this);
	DrawLayout(dc);
}

void UndoHistory::OnMouseWheel(wxMouseEvent& event) {
	// Only handle scrollwheel if we have a scrollbar
	if (!GetScrollThumb(wxVERTICAL)) return;

	const int rotation = event.GetWheelRotation();
	if (rotation == 0) return; // No net rotation.

	const wxSize size = GetClientSize();
	const int linescount = (abs(rotation) / event.GetWheelDelta()) * event.GetLinesPerAction();

	int pos = m_verticalScrollPos;

	if (rotation > 0) { // up
		pos = pos - (pos % m_lineHeight) - (m_lineHeight * linescount);
		if (pos < 0) pos = 0;
	}
	else if (rotation < 0) { // down
		pos = pos - (pos % m_lineHeight) + (m_lineHeight * linescount);
		if (pos > m_treeHeight - size.y) pos = m_treeHeight - size.y;
	}

	if (pos != m_verticalScrollPos) {
		m_oldVerticalScrollPos = m_verticalScrollPos;
		m_verticalScrollPos = pos;
		m_isScrolling = true;

		wxClientDC dc(this);
		DrawLayout(dc);
	}
}

void UndoHistory::OnScroll(wxScrollWinEvent& event) {
	wxSize size = GetClientSize();

	if(event.GetOrientation() == wxVERTICAL) {
		int pos = m_verticalScrollPos;

		if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK ||
			event.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE)
		{
			pos = event.GetPosition();
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP) {
			pos -= size.y;
			if (pos < 0) pos = 0;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN) {
			pos += size.y;
			if (pos > m_treeHeight - size.y) pos = m_treeHeight - size.y;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP) {
			pos = pos - (pos % m_lineHeight) - m_lineHeight;
			if (pos < 0) pos = 0;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN) {
			pos = pos - (pos % m_lineHeight) + m_lineHeight;
			if (pos > m_treeHeight - size.y) pos = m_treeHeight - size.y;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_TOP) {
			pos = 0;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM) {
			pos = m_treeHeight - size.y;
			if (pos < 0) pos = 0;
		}

		if (pos != m_verticalScrollPos) {
			m_oldVerticalScrollPos = m_verticalScrollPos;
			m_verticalScrollPos = pos;
			m_isScrolling = true;

			wxClientDC dc(this);
			DrawLayout(dc);
		}
	} else {
		int pos = m_horizontalScrollPos;

		if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK ||
			event.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE)
		{
			pos = event.GetPosition();
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEUP) {
			pos -= size.x;
			if (pos < 0) pos = 0;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_PAGEDOWN) {
			pos += size.x;
			if (pos > m_treeWidth - size.x) pos = m_treeWidth - size.x;
		}
		/* These events don't make sense for horizontal scrolling.
		else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP) {
			pos = pos - (pos % m_lineHeight) - m_lineHeight;
			if (pos < 0) pos = 0;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN) {
			pos = pos - (pos % m_lineHeight) + m_lineHeight;
			if (pos > m_treeHeight - size.y) pos = m_treeHeight - size.y;
		}*/
		else if (event.GetEventType() == wxEVT_SCROLLWIN_TOP) {
			pos = 0;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_BOTTOM) {
			pos = m_treeWidth - size.x;
			if (pos < 0) pos = 0;
		}

		if (pos != m_horizontalScrollPos) {
			m_oldHorizontalScrollPos = m_horizontalScrollPos;
			m_horizontalScrollPos = pos;
			m_isScrolling = false;

			wxClientDC dc(this);
			DrawLayout(dc);
		}
	}
}

void UndoHistory::OnChar(wxKeyEvent& event) {
	// Keyboard navigation is disabled for selections
	if (IsSelectionMode()) return;

	switch(event.GetKeyCode()) {
	case WXK_UP:
		{
			doc_id di;
			cxLOCKDOC_READ(m_doc)
				di = doc.GetDraftParent();
			cxENDLOCK

			if (!di.IsOk()) return; // We are in top document (has no parent)
			m_editorCtrl->SetDocument(di);
		}
		break;

	case WXK_DOWN:
		{
			vector<int> childlist;
			cxLOCKDOC_READ(m_doc)
				doc.GetVersionChildren(m_sourceDoc.version_id, childlist);
			cxENDLOCK

			if (!childlist.empty()) {
				const doc_id di(DRAFT, m_sourceDoc.document_id, childlist[0]);
				m_editorCtrl->SetDocument(di);
			}
		}
		break;

	case WXK_LEFT:
		if (m_sourceDoc.version_id) {
			vector<int> childlist;
			cxLOCKDOC_READ(m_doc)
				const unsigned int parent = doc.GetDraftParent(m_sourceDoc.version_id).version_id;
				doc.GetVersionChildren(parent, childlist);
			cxENDLOCK

			vector<int>::const_iterator idx = find(childlist.begin(), childlist.end(), m_sourceDoc.version_id);
			if (idx > childlist.begin()) {
				const doc_id di(DRAFT, m_sourceDoc.document_id, *(idx-1));
				m_editorCtrl->SetDocument(di);
			}
		}
		break;

	case WXK_RIGHT:
		if (m_sourceDoc.version_id) {
			vector<int> childlist;
			cxLOCKDOC_READ(m_doc)
				const unsigned int parent = doc.GetDraftParent(m_sourceDoc.version_id).version_id;
				doc.GetVersionChildren(parent, childlist);
			cxENDLOCK

			vector<int>::const_iterator idx = find(childlist.begin(), childlist.end(), m_sourceDoc.version_id);
			if (idx < childlist.end()-1) {
				const doc_id di(DRAFT, m_sourceDoc.document_id, *(idx+1));
				m_editorCtrl->SetDocument(di);
			}
		}
	}
}

void UndoHistory::OnVersionTreeSel(VersionTreeEvent& event) {
	const size_t item_id = (int)event.GetItem();
	if ((int)item_id == m_selectedNode) return;

	if (!m_editorCtrl) return;

	if (IsSelectionMode()) { // Selective undo
		const cxDiffEntry& de = m_rangeHistory[item_id];

		// Do the undo and make sure the history does not get update
		m_ignoreUpdates = true;
		m_range = m_editorCtrl->UndoSelection(de);
		m_editorCtrl->ReDraw();
		m_ignoreUpdates = false;
	}
	else if (m_sourceDoc.IsDraft()) {
		const doc_id clicked_doc(DRAFT, m_sourceDoc.document_id, (int)item_id);

		// We do not have to set the document now. We will recieve
		// a WIN_CHANGEDOC notifer if the documents actually gets
		// set on a page.

		m_editorCtrl->SetDocument(clicked_doc);

		// Check if we were double-clicked
		if (event.GetInt() == 1) {
			// Send event so that parent ctrl can close if it want
			wxCommandEvent evt(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED);
			GetEventHandler()->ProcessEvent(evt);
		}
	}

	// Select the clicked version
	m_selectedNode = item_id;
	m_pTree->Select(m_selectedNode);
	m_needRedrawing = true;
}

void UndoHistory::OnVersionTreeContextMenu(VersionTreeEvent& event) {
#ifdef __WXDEBUG__
	// Show context menu
	m_popupItem = (int)event.GetItem();
	wxMenu contextMenu;
	contextMenu.Append(MENU_DIFF_TO_CURRENT, _("&Diff to current"), _("Diff to current"));
	PopupMenu(&contextMenu);
#endif
}

void UndoHistory::OnMenuDiffToCurrent(wxCommandEvent& WXUNUSED(event)) {
	const doc_id clicked_doc(DRAFT, m_sourceDoc.document_id, m_popupItem);
	m_dispatcher.Notify(wxT("DIFF_TOCURRENT"), &clicked_doc, m_source_win_id);
}

void UndoHistory::OnVersionTreeTooltip(VersionTreeEvent& event) {
	const doc_id hot_doc(m_sourceDoc.type, m_sourceDoc.document_id, (int)event.GetItem());

	cxLOCK_READ(m_catalyst)
		if (catalyst.IsMirroredSpecific(hot_doc)) {
			wxArrayString paths;
			catalyst.GetMirrorPaths(hot_doc, paths);
			if (!paths.IsEmpty()) {
				wxString tip = paths[0];
				for (size_t i = 1; i < paths.GetCount(); ++i) {
					tip += wxT("\n");
					tip += paths[i];
				}

				event.SetToolTip(tip);
				event.Allow();
			}
			else wxASSERT(false);
		}
	cxENDLOCK
}

void UndoHistory::OnVersionTreeDrawitem(VersionTreeEvent& event) {
	const doc_id hot_doc(m_sourceDoc.type, m_sourceDoc.document_id, (int)event.GetItem());

	cxLOCK_READ(m_catalyst)
		if (catalyst.IsMirroredSpecific(hot_doc))
			event.SetItemStyle(event.GetItemStyle() | VTREESTYLE_SAVED);
	cxENDLOCK

	cxLOCKDOC_READ(m_doc)
		if (!(event.GetItem() == 0 && doc.GetParent().IsOk()))
			event.SetItemStyle(event.GetItemStyle() | VTREESTYLE_DRAFT);
	cxENDLOCK
}

// no evt.skip() as we don't want the control to erase the background
void UndoHistory::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

// static notification handler
void UndoHistory::OnClosePage(UndoHistory* self, void* data, int WXUNUSED(filter)) {
	if (self->m_editorCtrl == (EditorCtrl*)data) self->Clear();
}

// static notification handler
void UndoHistory::OnChangeDoc(UndoHistory* self, void* data, int filter) {
	if (filter != self->m_source_win_id) return;
	//wxLogTrace("OnChangeDoc %d %d - %d", di.document_id, di.revision_id, filter);

	self->m_editorCtrl = (EditorCtrl*)data;
	const doc_id di = self->m_editorCtrl->GetDocID();
	self->m_range.Set(0, 0); // reset to no selection (will update in idle)
	self->SetDocument(di);
}

// static notification handler
void UndoHistory::OnDocDeleted(UndoHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const doc_id* const di = (doc_id*)data;
	wxASSERT(di->IsDraft());

	if (self->m_sourceDoc.SameDoc(*di)) {
		// Invalidate doc ref
		self->m_sourceDoc.document_id = -1;
		self->m_sourceDoc.version_id = -1;

		// Schedule the tree for redrawing
		self->m_pTree->Clear();
	}
}

// static notification handler
void UndoHistory::OnNewRevision(UndoHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_editorCtrl) return; // may be called before first editor

	const doc_id& di = *(const doc_id*)data;
	wxASSERT(di.IsDraft());
	
	if (di == self->m_editorCtrl->GetDocID()) {
		self->SetDocument(di);
	}
	else if (self->m_sourceDoc.SameDoc(di)) {
		// Doc has been changed in another editor, so we just redraw
		self->UpdateTree();
	}
}

// static notification handler
void UndoHistory::OnUpdateRevision(UndoHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;
	const doc_id& di = *(const doc_id*)data;
	wxASSERT(di.IsDraft());

	if (di == self->m_editorCtrl->GetDocID()) {
		self->SetDocument(di);
	}
	else if (self->m_sourceDoc.SameDoc(di)) {
		// Doc has been changed in another editor, so we just redraw
		self->UpdateTree();
	}
}

// static notification handler
void UndoHistory::OnDocUpdated(UndoHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;
	const doc_id& di = *(const doc_id*)data;

	if (di == self->m_editorCtrl->GetDocID()) {
		self->SetDocument(di);
	}
	else if (self->m_sourceDoc.SameDoc(di)) {
		// Doc has been changed in another editor, so we just redraw
		self->UpdateTree();
	}
}

// static notification handler
void UndoHistory::OnDocCommited(UndoHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const docid_pair* const diPair = (docid_pair*)data;

	if (self->m_sourceDoc.SameDoc(diPair->doc1))
		self->SetDocument(diPair->doc2);
}
