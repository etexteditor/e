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

#include "DocHistory.h"
#include "EditorCtrl.h"
#include "VersionTree.h"
#include "Timeline.h"

enum {
	ID_TOOLTIP_TIMER
};

BEGIN_EVENT_TABLE(DocHistory, wxControl)
	EVT_PAINT(DocHistory::OnPaint)
	EVT_SIZE(DocHistory::OnSize)
	EVT_IDLE(DocHistory::OnIdle)
	EVT_ERASE_BACKGROUND(DocHistory::OnEraseBackground)
	EVT_SCROLLWIN(DocHistory::OnScroll)
	EVT_MOUSEWHEEL(DocHistory::OnMouseWheel)
	EVT_VERSIONTREE_SEL_CHANGED(1, DocHistory::OnVersionTreeSel)
	EVT_VERSIONTREE_TOOLTIP(1, DocHistory::OnVersionTreeTooltip)
	EVT_VERSIONTREE_DRAWITEM(1, DocHistory::OnVersionTreeDrawitem)
	EVT_MOTION(DocHistory::OnMotion)
	EVT_LEFT_DOWN(DocHistory::OnMouseLeftDown)
	EVT_ENTER_WINDOW(DocHistory::OnMotion)
	EVT_LEAVE_WINDOW(DocHistory::OnLeaveWindow)
	EVT_TIMER(ID_TOOLTIP_TIMER, DocHistory::OnTooltipTimer)
END_EVENT_TABLE()

DocHistory::DocHistory(CatalystWrapper& cw, int win_id, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxControl(parent, id, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE),
	  m_catalyst(cw), m_dispatcher(cw.GetDispatcher()), m_doc(cw), m_mdc(), m_bitmap(1,1), m_cell(m_mdc, m_doc), m_hotNode(-1),
	  m_revTooltip(cw), m_editorCtrl(NULL), m_source_win_id(win_id)
{
	m_revTooltip.Create(this);
	m_tooltipTimer.SetOwner(this, ID_TOOLTIP_TIMER);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM); // Avoid flicker

	// Initialize variables
	m_needRedrawing = true; // Make sure the ctrl gets drawn on first idle event
	m_document_id = -1;
	m_scrollPos = 0;
	m_isScrolling = false;
	m_lineHeight = 18;

	bgBrush = *wxWHITE_BRUSH;
	linePen = *wxGREY_PEN;

	m_pTimeline = new Timeline(this, 2);
	m_pTree = new VersionTree(this, 1);

	// Initialize the memoryDC for dubblebuffering
	m_mdc.SelectObject(m_bitmap);
	m_mdc.SetFont(wxFont(9, wxMODERN, wxNORMAL, wxNORMAL, false));

	// Make sure we recieve notifications of new versions and updates
	m_dispatcher.SubscribeC(wxT("WIN_CLOSEPAGE"), (CALL_BACK)OnClosePage, this);
	m_dispatcher.SubscribeC(wxT("WIN_CHANGEDOC"), (CALL_BACK)OnChangeDoc, this);
	m_dispatcher.SubscribeC(wxT("DOC_UPDATED"), (CALL_BACK)OnDocUpdated, this);
	m_dispatcher.SubscribeC(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);
	m_dispatcher.SubscribeC(wxT("DOC_COMMITED"), (CALL_BACK)OnDocCommited, this);
	m_dispatcher.SubscribeC(wxT("DOC_NEWREVISION"), (CALL_BACK)OnNewRevision, this);
	m_dispatcher.SubscribeC(wxT("DOC_UPDATEREVISION"), (CALL_BACK)OnUpdateRevision, this);
}

DocHistory::~DocHistory() {
	m_dispatcher.UnSubscribe(wxT("WIN_CLOSEPAGE"), (CALL_BACK)OnClosePage, this);
	m_dispatcher.UnSubscribe(wxT("WIN_CHANGEDOC"), (CALL_BACK)OnChangeDoc, this);
	m_dispatcher.UnSubscribe(wxT("DOC_UPDATED"), (CALL_BACK)OnDocUpdated, this);
	m_dispatcher.UnSubscribe(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);
	m_dispatcher.UnSubscribe(wxT("DOC_COMMITED"), (CALL_BACK)OnDocCommited, this);
	m_dispatcher.UnSubscribe(wxT("DOC_NEWREVISION"), (CALL_BACK)OnNewRevision, this);
	m_dispatcher.UnSubscribe(wxT("DOC_UPDATEREVISION"), (CALL_BACK)OnUpdateRevision, this);
}

void DocHistory::Clear() {
	// Invalidate doc ref
	m_sourceDoc.Invalidate();
	m_editorCtrl = NULL;

	// Clear data structures
	m_items.clear();

	// Clear sub-ctrls
	m_pTimeline->Clear();
	m_pTree->Clear();

	m_needRedrawing = true;
}

void DocHistory::SetDocument(const doc_id& di) {
	//wxASSERT(m_catalyst.IsOk(di));

	// Check if we are invalidating the view
	if (!di.IsOk()) {
		Clear();
		return;
	}

	if (m_sourceDoc != di) {
		cxLOCKDOC_WRITE(m_doc)
			doc.SetDocument(di);

			// Get the document
			m_document_id = di.IsDraft() ? doc.GetParent().document_id : di.document_id;
		cxENDLOCK

		m_sourceDoc = di;

		// Check if we need to rebuild the tree
		ReBuildTree();
	}

	Select(m_selectedNode);
	MakeItemVisible(m_selectedNode);
}

void DocHistory::ReBuildTree() {
	if (m_document_id == -1) {
		wxASSERT(m_sourceDoc.IsDraft());
		// The draft has no parent (and can't have children)
		// So just add a single entry.
		m_items.clear();
		m_positions.clear();

		item new_item;
		new_item.doc = m_sourceDoc;
		cxLOCKDOC_READ(m_doc)
			new_item.date = doc.GetDate();
		cxENDLOCK
		new_item.parent = 0; // root is it's own parent
		new_item.ypos = 0;

		m_items.push_back(new_item);
		m_positions.push_back(0);
		m_selectedNode = 0;

		// Update timeline
		m_pTimeline->Clear();
		m_pTimeline->AddItem(new_item.date);

		// Update the VersionTree
		m_pTree->Clear();
		m_pTree->AddRoot();
	}
	else
	{
		m_items.clear();
		m_positions.clear();

		// Add the root
		const doc_id root(DOCUMENT, m_document_id, 0); // root is always zero
		item new_item;
		new_item.doc = root;
		cxLOCK_READ(m_catalyst)
			new_item.date = catalyst.GetDocDate(root);
			new_item.parent = 0; // root is it's own parent
			new_item.ypos = 0;
			m_items.push_back(new_item);
			if (m_sourceDoc == root) m_selectedNode = 0;
			else m_selectedNode = -1; // not yet set

			// Build the tree
			AddChildren(0, root, m_sourceDoc, catalyst);
		cxENDLOCK
		wxASSERT(m_selectedNode >= 0 && m_selectedNode < (int)m_items.size());

		// Update Timeline & VersionTree
		m_pTimeline->Clear();
		m_pTree->Clear();
		if (!m_items.empty()) {
			m_pTimeline->AddItem(new_item.date);
			m_pTree->AddRoot();
		}
		for(unsigned int i = 1; i < m_items.size(); ++i) {
			const int ypos = m_pTimeline->AddItem(m_items[i].date);
			m_pTree->AddNode(m_items[i].parent, ypos);
			m_items[i].ypos = ypos;
		}
	}

	m_pTree->CalculateLayout();

	m_treeHeight = m_items.empty() ? 0 : m_items.back().ypos + m_lineHeight;
}

void DocHistory::AddChildren(int parent_pos, const doc_id& di, const doc_id& sel_doc, const Catalyst& catalyst) {
	wxASSERT(di.IsDocument());

	const int childCount = catalyst.GetChildCount(di);
	for (int i = 0; i < childCount; ++i) {
		const doc_id child = catalyst.GetChildID(di, i);
		item new_item;
		new_item.doc = child;
		new_item.date = catalyst.GetDocDate(child);
		new_item.parent = parent_pos;

		// Find the insert pos
		size_t n;
		for (n = parent_pos+1; n < m_items.size(); ++n) {
			if (new_item.date < m_items[n].date) {
				m_items.insert(m_items.begin()+n, new_item);

				// The indexes under the insertion has moved, so
				// we also have to update any references to this range
				if (m_selectedNode >= (int)n) ++m_selectedNode;
				for (size_t n2 = n+1; n2 < m_items.size(); ++n2) {
					int& parent = m_items[n2].parent;
					if (parent >= (int)n) ++parent;
				}
				break;
			}
		}
		if (n == m_items.size()) {
			m_items.push_back(new_item);
		}

		// Check if this is the selected node
		if (child.IsDocument()) {
			if (child == sel_doc) m_selectedNode = n;

			AddChildren(n, child, sel_doc, catalyst);
		}
		else if (child.SameDoc(sel_doc)) m_selectedNode = n;
	}
}

void DocHistory::MakeItemVisible(unsigned int item_id) {
	wxASSERT(item_id < m_items.size());

	const wxSize size = GetClientSize();
	const int ypos = m_items[item_id].ypos;
	const int ybottom = ypos + m_lineHeight;

	if (m_scrollPos > ypos) m_scrollPos = ypos;
	else if (m_scrollPos + size.y < ybottom) m_scrollPos = ybottom - size.y;

	m_needRedrawing = true;
}

void DocHistory::Select(unsigned int node_id) {
	wxASSERT(node_id < m_items.size());

	// Select the node in the tree
	m_pTree->Select(node_id);
	m_pTree->MakeNodeVisible(node_id);

	m_selectedNode = node_id;
	m_needRedrawing = true;
}

void DocHistory::DrawLayout(wxDC& dc) {
	if (!IsShown()) {
		// Sometimes OnSize() might get called while control is hidden
		m_needRedrawing = true;
		return;
	}

	const wxSize size = GetClientSize();

	wxRect rect;
	if (m_isScrolling) {
		// If there is overlap, then move the image
		if (m_scrollPos + size.y > m_oldScrollPos && m_scrollPos < m_oldScrollPos + size.y) {
			const int top = wxMax(m_scrollPos, m_oldScrollPos);
			const int bottom = wxMin(m_scrollPos, m_oldScrollPos) + size.y;
			const int overlap_height = bottom - top;
			m_mdc.Blit(0, (top - m_oldScrollPos) + (m_oldScrollPos - m_scrollPos),  size.x, overlap_height, &m_mdc, 0, top - m_oldScrollPos);

			const int new_height = size.y - overlap_height;
			const int newtop = (top == m_scrollPos) ? bottom : m_scrollPos;
			rect = wxRect(0,newtop,size.x,new_height); // Just redraw the revealed part
		}
		else {
			rect = wxRect(0,m_scrollPos,size.x,size.y);
		}

		SetScrollPos(wxVERTICAL, m_scrollPos);
		m_isScrolling = false;
	}
	else {
		// Check if we need a scrollbar
		bool hasScroll = (GetScrollThumb(wxVERTICAL) != 0);
		if (m_treeHeight > size.y) {
			SetScrollbar(wxVERTICAL, m_scrollPos, size.y, m_treeHeight);
			if (!hasScroll) return; // have sent a size event
		}
		else {
			SetScrollbar(wxVERTICAL, 0, 0, 0);
			if (hasScroll) return; // have sent a size event
		}

		// Can we make room for more of the history?
		if (m_scrollPos + size.y > m_treeHeight) {
			m_scrollPos = wxMax(0, m_treeHeight-size.y);
		}
		rect = wxRect(0, m_scrollPos, size.x, size.y);
	}

	// Resize & scroll the versiontree
	wxSize timelinesize = m_pTimeline->GetBestSize();
	wxSize treesize = m_pTree->GetBestSize();

	if (timelinesize.x + treesize.x >= size.x) {
		timelinesize.x = treesize.x = size.x / 3;
	}

	m_pTimeline->Move(0, 0);
	m_pTimeline->SetSize(wxSize(timelinesize.x, size.y));
	m_pTimeline->Scroll(m_scrollPos);
	m_pTimeline->ReDraw();

	// Resize & scroll the versiontree (positioned right of Timeline)
	m_pTree->Move(timelinesize.x, 0);
	m_pTree->SetSize(wxSize(treesize.x, size.y));
	m_pTree->ScrollVertical(m_scrollPos);

	m_xposDesc = timelinesize.x + treesize.x;
	const int textwidth = size.x - m_xposDesc;
	rect = wxRect(m_xposDesc, rect.y, textwidth, rect.height);

	// resize the bitmap used for doublebuffering
	if (m_bitmap.GetWidth() < textwidth || m_bitmap.GetHeight() < size.y) {
		m_bitmap = wxBitmap(textwidth, size.y);
		m_mdc.SelectObject(m_bitmap);
	}

	// Draw the description area
	// Notice that the memDc is moved so that m_xposDesc=0
	{
		// Clear the background
		m_mdc.SetBrush(bgBrush);
		m_mdc.SetPen(*wxWHITE_PEN);
		m_mdc.DrawRectangle(0, rect.y - m_scrollPos, textwidth, rect.height);

		// Find the first visible node
		unsigned int i;
		for (i = 0; i < m_items.size(); ++i) {
			if (m_items[i].ypos+m_lineHeight >= rect.y) break;
		}

		const int ybottom = m_scrollPos + size.y;

		// Draw Label & Description
		m_mdc.SetTextForeground(*wxBLACK);
		const int textindent = 3;
		cxLOCK_READ(m_catalyst)
			while (i < m_items.size() && m_items[i].ypos < ybottom) {
				wxString label = catalyst.GetLabel(m_items[i].doc);
				if (!label.empty()) {
					label = label.BeforeFirst('\n');
					label += wxT(": ");
				}
				int x; int y;
				m_mdc.GetTextExtent(label, &x, &y);
				m_mdc.DrawText(label, textindent, m_items[i].ypos-m_scrollPos);

				wxString desc = catalyst.GetDescription(m_items[i].doc);
				desc = desc.BeforeFirst('\n');
				m_mdc.DrawText(desc, textindent+x, m_items[i].ypos-m_scrollPos);
				++i;
			}
		cxENDLOCK
	}

	// Copy MemoryDC to Display
	dc.Blit(m_xposDesc, 0, textwidth, size.y, &m_mdc, 0, 0);

	m_needRedrawing = false;
}

void DocHistory::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	if (m_needRedrawing) {
		//wxLogDebug(wxT("OnPaint Redraw"));
		DrawLayout(dc);
	}
	else {
		//wxLogDebug(wxT("OnPaint Blit"));
		// Re-Blit MemoryDC to Display
		wxSize size = GetClientSize();
		dc.Blit(m_xposDesc, 0,  size.x-m_xposDesc, size.y, &m_mdc, 0, 0);
		//wxLogDebug(wxT(" Blit %d,%d-%d-%d"), treesize.x, 0,  size.x-treesize.x, size.y);
	}
}

void DocHistory::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	if (IsShown() && m_needRedrawing) {
		wxClientDC dc(this);
		DrawLayout(dc);
	}
}

void DocHistory::OnSize(wxSizeEvent& WXUNUSED(event)) {
	wxClientDC dc(this);
	DrawLayout(dc);
}

void DocHistory::OnMotion(wxMouseEvent& event) {
	const int y = event.GetY() + m_scrollPos;
	if (y < 0 || y >= m_treeHeight) {
		m_hotNode = -1;
		m_tooltipTimer.Stop();
		m_revTooltip.Hide();
		return;
	}

	// Which node are we over?
	int node_id = -1;
	for (unsigned int i = 0; i < m_items.size(); ++i) {
		const int node_ypos = m_items[i].ypos;
		if (y >= node_ypos && y < (node_ypos + m_lineHeight)) {
			node_id = i;
			break;
		}
	}
	if (node_id == -1) {
		m_hotNode = -1;
		m_tooltipTimer.Stop();
		m_revTooltip.Hide();
		return; // clicked outside nodes
	}

	//wxLogDebug(wxT("Over node %d"), i);

	if (node_id != m_hotNode) {
		if (m_hotNode == -1) {
			// Wait for the user to hover a bit before showing tip
			m_hotNode = node_id;
			m_tooltipTimer.Start(500, true);
		}
		else {
			// If we are already showing tooltip, just update
			// it to the new node
			m_hotNode = node_id;
			wxPoint tipPos = wxGetMousePosition();
			m_revTooltip.SetDocument(m_items[m_hotNode].doc, tipPos);
		}
	}
}

void DocHistory::OnMouseLeftDown(wxMouseEvent& event) {
	const int y = event.GetY() + m_scrollPos;

	// Which node are we over?
	int node_id = -1;
	for (unsigned int i = 0; i < m_items.size(); ++i) {
		const int node_ypos = m_items[i].ypos;
		if (y >= node_ypos && y < (node_ypos + m_lineHeight)) {
			node_id = i;
			break;
		}
	}

	if (node_id != -1) {
		// We do not have to set the document now. We will recieve
		// a WIN_CHANGEDOC notifer if the documents actually gets
		// set on a page.

		// Do not return a reference. The events might modify
		// m_items underneath it.
		const doc_id hot_doc = m_items[node_id].doc;

		if (hot_doc.IsDraft()) {
			doc_id draft_head;
			cxLOCK_READ(m_catalyst)
				draft_head = catalyst.GetDraftHead(hot_doc.document_id);
			cxENDLOCK
			m_editorCtrl->SetDocument(draft_head);
		}
		else m_editorCtrl->SetDocument(hot_doc);
	}
}

void DocHistory::OnLeaveWindow(wxMouseEvent& WXUNUSED(event)) {
	m_hotNode = -1;
	m_revTooltip.Hide();
}

void DocHistory::OnTooltipTimer(wxTimerEvent& WXUNUSED(event)) {
	if (m_hotNode == -1) return;

	// Check if we are still over node
	const wxSize size = GetClientSize();
	const wxPoint mousePos = ScreenToClient(wxGetMousePosition());

	if (mousePos.x >= 0 && mousePos.x < size.x) {
		const int ypos = mousePos.y + m_scrollPos;
		const int node_ypos = m_items[m_hotNode].ypos;
		const int node_bottom = node_ypos + m_lineHeight;

		if (ypos >= node_ypos && ypos < node_bottom) {
			wxPoint tipPos = wxGetMousePosition();

			m_revTooltip.SetDocument(m_items[m_hotNode].doc, tipPos);
			return;
		}
	}

	// Start over if we have left node in the meantime
	m_hotNode = -1;
}

void DocHistory::OnMouseWheel(wxMouseEvent& event) {
	if (GetScrollThumb(wxVERTICAL)) { // Only handle scrollwheel if we have a scrollbar
		const wxSize size = GetClientSize();
		int pos = m_scrollPos;
		const int rotation = event.GetWheelRotation();
		const int linescount = (abs(rotation) / event.GetWheelDelta()) * event.GetLinesPerAction();

		if (rotation > 0) { // up
			pos = pos - (pos % m_lineHeight) - (m_lineHeight * linescount);
			if (pos < 0) pos = 0;
		}
		else if (rotation < 0) { // down
			pos = pos - (pos % m_lineHeight) + (m_lineHeight * linescount);
			if (pos > m_treeHeight - size.y) pos = m_treeHeight - size.y;
		}
		else return; // no rotation

		if (pos != m_scrollPos) {
			m_oldScrollPos = m_scrollPos;
			m_scrollPos = pos;
			m_isScrolling = true;

			wxClientDC dc(this);
			DrawLayout(dc);
		}
	}
}

void DocHistory::OnScroll(wxScrollWinEvent& event) {
	wxSize size = GetClientSize();
	int pos = m_scrollPos;

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

	if (pos != m_scrollPos) {
		m_oldScrollPos = m_scrollPos;
		m_scrollPos = pos;
		m_isScrolling = true;

		wxClientDC dc(this);
		DrawLayout(dc);
	}
}

void DocHistory::OnVersionTreeSel(VersionTreeEvent& event) {
	wxASSERT(event.GetItem() >= 0 && event.GetItem() < m_items.size());

	// Do not return a reference. The events might modify
	// m_items underneath it.
	const doc_id hot_doc = m_items[event.GetItem()].doc;

	// We do not have to set the document now. We will recieve
	// a WIN_CHANGEDOC notifer if the documents actually gets
	// set on a page.

	if (hot_doc.IsDraft()) {
		doc_id draft_head;
		cxLOCK_READ(m_catalyst)
			draft_head = catalyst.GetDraftHead(hot_doc.document_id);
		cxENDLOCK
		m_editorCtrl->SetDocument(draft_head);
	}
	else m_editorCtrl->SetDocument(hot_doc);
}

void DocHistory::OnVersionTreeTooltip(VersionTreeEvent& event) {
	wxASSERT(event.GetItem() >= 0 && event.GetItem() < m_items.size());
	const doc_id& hot_doc = m_items[event.GetItem()].doc;

	cxLOCK_READ(m_catalyst)
		if (catalyst.IsMirrored(hot_doc)) {
			wxArrayString paths;
			if (hot_doc.IsDraft()) catalyst.GetMirrorPathsForDraft(hot_doc, paths); // gets mirrors for all draft revisions
			else catalyst.GetMirrorPaths(hot_doc, paths);

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

void DocHistory::OnVersionTreeDrawitem(VersionTreeEvent& event) {
	wxASSERT(event.GetItem() >= 0 && event.GetItem() < m_items.size());

	const doc_id& hot_doc = m_items[event.GetItem()].doc;
	int style = 0;

	cxLOCK_READ(m_catalyst)
		if (catalyst.IsMirrored(hot_doc)) style |= VTREESTYLE_SAVED;
		if (hot_doc.IsDraft()) style |= VTREESTYLE_DRAFT;
		else {
			// Only documents has these states
			if (catalyst.GetDocState(hot_doc) == cxREVSTATE_PENDING) style |= VTREESTYLE_PENDING;
			if (catalyst.IsDocUnread(hot_doc)) style |= VTREESTYLE_UNREAD;
		}
	cxENDLOCK

	event.SetItemStyle(style);
}


void DocHistory::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
	// # no evt.skip() as we don't want the control to erase the background
}

void DocHistory::HandleDocUpdate(const doc_id& di) {
	if (di == m_editorCtrl->GetDocID()) {
		SetDocument(di);
	}
	else {
		bool isSameDoc;
		cxLOCK_READ(m_catalyst)
			isSameDoc = catalyst.InSameHistory(di, m_sourceDoc);
		cxENDLOCK

		// Doc has been changed in another editor, so we just redraw
		if (isSameDoc) {
			ReBuildTree();
			m_isScrolling = false; // avoid moving old image if scrolling during update
			wxClientDC dc(this);
			DrawLayout(dc);
		}
	}
}

// static notification handler
void DocHistory::OnClosePage(DocHistory* self, void* data, int WXUNUSED(filter)) {
	if (self->m_editorCtrl == (EditorCtrl*)data) self->Clear();
}

// static notification handler
void DocHistory::OnChangeDoc(DocHistory* self, void* data, int filter) {
	if (filter != self->m_source_win_id) return;
	//wxLogTrace("OnChangeDoc %d %d - %d", di.document_id, di.revision_id, filter);

	self->m_editorCtrl = (EditorCtrl*)data;
	const doc_id di = self->m_editorCtrl->GetDocID();
	self->SetDocument(di);
}

// static notification handler
void DocHistory::OnDocUpdated(DocHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const doc_id& di = *(const doc_id*)data;
	self->HandleDocUpdate(di);
}

// static notification handler
void DocHistory::OnDocDeleted(DocHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const doc_id* const di = (doc_id*)data;
	wxASSERT(di->IsDraft());

	if (self->m_sourceDoc.SameDoc(*di)) {
		self->Clear();
	}
}

// static notification handler
void DocHistory::OnDocCommited(DocHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const docid_pair* const diPair = (docid_pair*)data;

	if (self->m_sourceDoc.SameDoc(diPair->doc1)) {
		self->m_isScrolling = false; // avoid moving old image if scrolling during update
		self->SetDocument(diPair->doc2);
	}
	else {
		self->HandleDocUpdate(diPair->doc2);
	}
}

// static notification handler
void DocHistory::OnNewRevision(DocHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const doc_id& di = *(const doc_id*)data;
	self->HandleDocUpdate(di);
}

// static notification handler
void DocHistory::OnUpdateRevision(DocHistory* self, void* data, int WXUNUSED(filter)) {
	if (!self->m_sourceDoc.IsOk()) return;

	const doc_id& di = *(const doc_id*)data;
	self->HandleDocUpdate(di);
}
