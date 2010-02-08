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

#include "VersionTree.h"
#include "wx/tooltip.h"
#include <algorithm>
#include "images/vtree_doc.xpm"
#include "images/vtree_doc_pending.xpm"
#include "images/vtree_overlay_selected.xpm"
#include "images/vtree_overlay_unread.xpm"
#include "images/vtree_overlay_mirrored.xpm"
#include "images/vtree_draft.xpm"
#include "images/vtree_draft_selected.xpm"

using namespace std;

BEGIN_EVENT_TABLE(VersionTree, wxControl)
	EVT_PAINT(VersionTree::OnPaint)
	EVT_SIZE(VersionTree::OnSize)
	EVT_ERASE_BACKGROUND(VersionTree::OnEraseBackground)
	EVT_IDLE(VersionTree::OnIdle)
	EVT_LEFT_DOWN(VersionTree::OnMouseLeftDown)
	EVT_RIGHT_DOWN(VersionTree::OnMouseRightDown)
	EVT_LEFT_DCLICK(VersionTree::OnMouseDClick)
	EVT_MOTION(VersionTree::OnMouseMotion)
END_EVENT_TABLE()

DEFINE_EVENT_TYPE(wxEVT_VERSIONTREE_SEL_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_VERSIONTREE_CONTEXTMENU)
DEFINE_EVENT_TYPE(wxEVT_VERSIONTREE_TOOLTIP)
DEFINE_EVENT_TYPE(wxEVT_VERSIONTREE_DRAWITEM)

IMPLEMENT_DYNAMIC_CLASS(VersionTreeEvent, wxNotifyEvent)

VersionTree::VersionTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size)
	: wxControl(parent, id, pos, size, wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE),
	  bitmap(1,1), selectedNode((size_t)-1), tooltippedNode((size_t)-1),
	  m_bmDoc(vtree_doc_xpm), m_bmDocPending(vtree_doc_pending_xpm), m_bmOverlaySelected(vtree_overlay_selected_xpm),
	  m_bmOverlayUnread(vtree_overlay_unread_xpm), m_bmOverlayMirrored(vtree_overlay_mirrored_xpm),
	  m_bmDraft(vtree_draft_xpm), m_bmDraftSelected(vtree_draft_selected_xpm)
{
	// Initialize vars
	xoffset = 5;
	yoffset = 5;
	nodewidth = 8;
	nodeheight = 8;
	nodespacing = yoffset*2;
	needRecalc = false;
	needRedrawing = false;
	verticalScrollPos = 0;
	horizontalScrollPos = 0;
	lineheight = nodeheight + nodespacing;
	treeWidth = xoffset*2 + nodewidth;

	bgBrush = *wxWHITE_BRUSH;
	nodeBrush = *wxWHITE_BRUSH;
	selectBrush = *wxBLUE_BRUSH;
	savedBrush = *wxLIGHT_GREY_BRUSH;
	linePen = *wxGREY_PEN;
	nodePen = *wxBLACK_PEN;
	draftPen = *wxLIGHT_GREY_PEN;

	// Initialize the memoryDC for dubblebuffering
	mdc.SelectObject(bitmap);
}

void VersionTree::Clear() {
	parents.clear();
	node_ypos.clear();
	needRecalc = true;
}

size_t VersionTree::AddRoot() {
	wxASSERT(parents.empty());
	parents.push_back(0); // root has no parent
	node_ypos.push_back(0); // root is top

	needRecalc = true;
	return 0;
}

size_t VersionTree::AddNode(int parent, int ypos) {
	wxASSERT(!parents.empty());
	wxASSERT(parents.size() > (size_t)parent);
	wxASSERT(ypos == -1 || ypos > node_ypos.back());

	parents.push_back(parent);

	if (ypos == -1) node_ypos.push_back(node_ypos.back()+lineheight);
	else node_ypos.push_back(ypos);

	needRecalc = true;
	return parents.size() - 1;
}
/*
int VersionTree::InsertNode(int pos, int parent) {
	wxASSERT(pos >= 0 && pos <= (int)parents.size());
	wxASSERT(parents.size() > (size_t)parent);

	parents.insert(parents.begin()+pos, parent);

	needRecalc = true;
	return pos;
}
*/
void VersionTree::Select(size_t node_id) {
	wxASSERT(node_id >= 0 && (size_t)node_id < parents.size());
	selectedNode = node_id;

	needRedrawing = true;
}

void VersionTree::CalculateLayout() {
	if (!IsShown()) {
		needRedrawing = true;
		return;
	}

	const size_t nodecount = parents.size();
	int columnCount = 0;

	if (nodecount) {
		// Pre-allocate lists
		node_xpos.resize(nodecount);
		vector<int> width(nodecount);
		vector<int> columns(nodecount);

		// Calculate how much space there is under each node
		size_t i = nodecount;
		while (i) {
			--i;
			if (i != 0) { // root node has no parent
				if (width[i] == 0) width[parents[i]] += 1;
				else width[parents[i]] += width[i];
			}
		}
		columnCount = width[0];

		// Calculate the position of each node
		for(size_t n = 0; n < nodecount; ++n) {
			int nwidth = 1;
			if (width[n]) nwidth = width[n];

			if (n) {
				columns[n] = columns[parents[n]];
				columns[parents[n]] += nwidth;
			}

			node_xpos[n] = xoffset + (columns[n] * nodewidth) + (((nwidth * nodewidth) - nodewidth) / 2);
			//node_ypos[n] = yoffset + i * (nodeheight + nodespacing);
		}
	}

	treeHeight = (nodecount > 0) ? node_ypos.back() + lineheight : 0;
	treeWidth = xoffset*2 + wxMax(1, columnCount)*nodewidth;

	// Schedule the tree for redrawing
	needRecalc = false;
	needRedrawing = true;
}

void VersionTree::DrawTree(wxRect& rect) {
	// Clear the background
	mdc.SetBrush(bgBrush);
	mdc.SetPen(*wxWHITE_PEN);
	mdc.DrawRectangle(rect.x-horizontalScrollPos, rect.y - verticalScrollPos, rect.width, rect.height);

	const size_t nodecount = parents.size();
	if (!nodecount) return;

	// Calculate first visible node
	vector<int>::iterator p = lower_bound(node_ypos.begin(), node_ypos.end(), rect.y);
	size_t topnode = distance(node_ypos.begin(), p);
	if (topnode > 0) --topnode;

	// Calculate last visible node
	const int yBottom = rect.GetBottom();
	size_t endnode = node_ypos.size()-1;
	for (size_t n = topnode; n < node_ypos.size(); ++n) {
		if (node_ypos[n]+lineheight >= yBottom) {
			endnode = n;
			break;
		}
	}

	int halfwidth = nodewidth / 2;

	// Draw the nodes
	mdc.SetPen(nodePen);
	for (size_t i = topnode; i <= endnode; ++i) {
		// Give parent ctrl a chance to change the item style
		VersionTreeEvent vt_event(wxEVT_VERSIONTREE_DRAWITEM, GetId());
		vt_event.SetEventObject(this);
		vt_event.SetItem(i);
		vt_event.SetItemStyle(VTREESTYLE_NONE); // default is no tooltip
		GetEventHandler()->ProcessEvent(vt_event);

		const int style = vt_event.GetItemStyle();
		const int ypos = node_ypos[i] + yoffset;

        int x = node_xpos[i]-horizontalScrollPos;
        int y = ypos - verticalScrollPos;
        
		if (style & VTREESTYLE_DRAFT) {
			if (i == selectedNode) mdc.DrawBitmap(m_bmDraftSelected, x, y, true);
			else mdc.DrawBitmap(m_bmDraft, x, y, true);
		}
		else {
			// Draw base node
			if (style & VTREESTYLE_PENDING) mdc.DrawBitmap(m_bmDocPending, x, y, true);
			else mdc.DrawBitmap(m_bmDoc, x, y, true);

			// Draw color overlay (selected or unread)
			if (i == selectedNode) mdc.DrawBitmap(m_bmOverlaySelected, x, y, true);
			else if (style & VTREESTYLE_UNREAD) mdc.DrawBitmap(m_bmOverlayUnread, x, y, true);
		}

		// Draw mirror overlay
		if (style & VTREESTYLE_SAVED) mdc.DrawBitmap(m_bmOverlayMirrored, x, y, true);
	}

	// Draw lines between the nodes
	mdc.SetPen(linePen);
	for (size_t m = 1; m < nodecount; ++m) {
		const size_t parent = parents[m];
		if (parent <= endnode && m >= topnode) {
			const int parentypos = (node_ypos[parent] + yoffset) - verticalScrollPos;
			const int nodeypos = (node_ypos[m] + yoffset) - verticalScrollPos;
			const int parentxpos = node_xpos[parent] + halfwidth - horizontalScrollPos;
			const int nodexpos = node_xpos[m] + halfwidth - horizontalScrollPos;
			mdc.DrawLine(parentxpos, parentypos + nodeheight,
                         nodexpos, parentypos + nodeheight + nodespacing);

			mdc.DrawLine(nodexpos, parentypos + nodeheight + nodespacing,
                         nodexpos, nodeypos);
		}
	}
}

void VersionTree::UpdateTree() {
	if (!IsShown()) {
		// Sometimes OnSize() might get called while control is hidden
		needRedrawing = true;
		return;
	}

	if (needRecalc) CalculateLayout();

	wxSize size = GetClientSize();

	wxRect rect(horizontalScrollPos,verticalScrollPos,size.x,size.y);
	DrawTree(rect);

	// Copy MemoryDC to Display
	wxClientDC dc(this);
	dc.Blit(0, 0,  size.x, size.y, &mdc, 0, 0);

	needRedrawing = false;
}

void VersionTree::ScrollVertical(int pos) {
    Scroll(horizontalScrollPos, pos);
}

void VersionTree::Scroll(int xpos, int ypos) {
    if(xpos == horizontalScrollPos && ypos == verticalScrollPos) return;

	const wxSize size = GetClientSize();
	wxRect rect;

    rect = wxRect(xpos,ypos,size.x,size.y);
    //If the user scrolls horizontally, then we just do a full redraw.  There is no way to shift the image both left and up and then do a redraw on a six sided region.
	if (!needRedrawing && xpos == horizontalScrollPos) {
		// If there is overlap, then move the image
		if (ypos + size.y > verticalScrollPos && ypos < verticalScrollPos + size.y) {
			const int top = wxMax(ypos, verticalScrollPos);
			const int bottom = wxMin(ypos, verticalScrollPos) + size.y;
			const int overlap_height = bottom - top;
			mdc.Blit(0, (top - verticalScrollPos) + (verticalScrollPos - ypos),  treeWidth, overlap_height, &mdc, 0, top - verticalScrollPos);

			const int new_height = size.y - overlap_height;
			const int newtop = (top == ypos) ? bottom : ypos;
			rect = wxRect(horizontalScrollPos,newtop,size.x,new_height);
		}
		
	}
	verticalScrollPos = ypos;
	horizontalScrollPos = xpos;

	DrawTree(rect);

	// Copy updated MemoryDC to Display
	wxClientDC dc(this);
	dc.Blit(0, 0,  size.x, size.y, &mdc, 0, 0);

	needRedrawing = false;
}

void VersionTree::MakeNodeVisible(size_t node_id, bool doCenter) {
	if (!IsShown()) return;
	wxASSERT(node_id < node_ypos.size());

	if (needRecalc) CalculateLayout();

	const wxSize size = GetClientSize();
	const int ypos = node_ypos[node_id];
	const int ybottom = ypos + lineheight;
	const int xpos = node_xpos[node_id];

	if (doCenter) {
		const int centertop = ypos - (size.y / 2);
		const int maxyscrollpos = treeHeight - size.y;
		const int centerLeft = xpos - (size.x / 2);
		const int maxxscrollpos = treeWidth - size.x;

		verticalScrollPos = (centertop > 0) ? centertop : 0;
		if (maxyscrollpos >= 0 && verticalScrollPos > maxyscrollpos) verticalScrollPos = maxyscrollpos;
		
		horizontalScrollPos = (centerLeft > 0) ? centerLeft : 0;
		if (maxxscrollpos >= 0 && horizontalScrollPos > maxxscrollpos) horizontalScrollPos = maxxscrollpos;
	}
	else {
		if (verticalScrollPos > ypos) verticalScrollPos = ypos;
		else if (verticalScrollPos + size.y < ybottom) verticalScrollPos = ybottom - size.y;
		
		if(horizontalScrollPos > xpos) horizontalScrollPos = xpos;
		else if(horizontalScrollPos+size.x < xpos) horizontalScrollPos = xpos - size.x;
	}

	// Redraw the tree with new position
	needRedrawing = true;
}

void VersionTree::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);

	if (needRedrawing) {
		CalculateLayout();
		MakeNodeVisible(selectedNode);
		UpdateTree();
	}
	else {
		// Re-Blit MemoryDC to Display
		wxSize size = GetClientSize();
		dc.Blit(0, 0,  size.x, size.y, &mdc, 0, 0);
	}
}

void VersionTree::OnSize(wxSizeEvent& WXUNUSED(event)) {
	wxSize size = GetClientSize();

	if (size.x <= 0 || size.y <= 0) return; // no need to update

	// resize the bitmap used for doublebuffering
	if (bitmap.GetWidth() < size.x || bitmap.GetHeight() < size.y) {
		bitmap = wxBitmap(size.x, size.y);
		mdc.SelectObject(bitmap);
	}

	// Draw the new layout
	UpdateTree();
}

void VersionTree::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {
	// # no evt.skip() as we don't want the control to erase the background
}

void VersionTree::OnIdle(wxIdleEvent& WXUNUSED(event)) {
	if (IsShown() && (needRecalc || needRedrawing)) UpdateTree();
}

void VersionTree::OnMouseLeftDown(wxMouseEvent& event) {
	//int x = event.GetX();
	const int y = event.GetY() + verticalScrollPos;
	if (y < 0 || y >= treeHeight) return;

	// Which node was clicked on?
	int node_id = -1;
	for (unsigned int i = 0; i < node_ypos.size(); ++i) {
		if (y >= node_ypos[i] && y < (node_ypos[i] + lineheight)) {
			node_id = i;
			break;
		}
	}
	if (node_id == -1) return; // clicked outside nodes

	/*
	if (selectedNode != node_id) {
		selectedNode = node_id;
		UpdateTree();
	}*/

	VersionTreeEvent vt_event(wxEVT_VERSIONTREE_SEL_CHANGED, GetId());
	vt_event.SetEventObject(this);
	vt_event.SetItem(node_id);
	vt_event.SetInt(0); // single-click
	GetEventHandler()->ProcessEvent(vt_event);

	/*// Select only if click is inside node
	for (int i = 0; i < nodecount; ++i) {
		if (y >= node_ypos[i] && y <= (node_ypos[i] + nodeheight)) {
			if (x >= node_xpos[i] && x <= (node_xpos[i] + nodewidth)) {
				if (selectedNode != i) {
					selectedNode = i;
					UpdateTree();
				}

				doc_id currentdoc = {revision.GetDocumentID(), i};
				dispatcher.Notify("WIN_SETDOCUMENT", &currentdoc, source_win_id);
			}
		}
	}*/
}

void VersionTree::OnMouseRightDown(wxMouseEvent& event) {
	const int y = event.GetY() + verticalScrollPos;
	if (y < 0 || y >= treeHeight) return;

	// Which node was clicked on?
	int node_id = -1;
	for (unsigned int i = 0; i < node_ypos.size(); ++i) {
		if (y >= node_ypos[i] && y < (node_ypos[i] + lineheight)) {
			node_id = i;
			break;
		}
	}
	if (node_id == -1) return; // clicked outside nodes


	VersionTreeEvent vt_event(wxEVT_VERSIONTREE_CONTEXTMENU, GetId());
	vt_event.SetEventObject(this);
	vt_event.SetItem(node_id);
	vt_event.SetInt(0); // single-click
	GetEventHandler()->ProcessEvent(vt_event);
}

void VersionTree::OnMouseDClick(wxMouseEvent& event) {
	//int x = event.GetX();
	const int y = event.GetY() + verticalScrollPos;
	if (y < 0 || y >= treeHeight) return;

	// Which node was clicked on?
	int node_id = -1;
	for (unsigned int i = 0; i < node_ypos.size(); ++i) {
		if (y >= node_ypos[i] && y < (node_ypos[i] + lineheight)) {
			node_id = i;
			break;
		}
	}
	if (node_id == -1) return; // clicked outside nodes

	VersionTreeEvent vt_event(wxEVT_VERSIONTREE_SEL_CHANGED, GetId());
	vt_event.SetEventObject(this);
	vt_event.SetItem(node_id);
	vt_event.SetInt(1); // double-click
	GetEventHandler()->ProcessEvent(vt_event);
}

void VersionTree::OnMouseMotion(wxMouseEvent& event) {
	const int y = event.GetY() + verticalScrollPos;

	if (y >= 0 && y < treeHeight) {
		// Which node is pointer over?
		size_t node_id = size_t(-1);
		for (unsigned int i = 0; i < node_ypos.size(); ++i) {
			if (y >= node_ypos[i] && y < (node_ypos[i] + lineheight)) {
				node_id = i;
				break;
			}
		}
		if (node_id == size_t(-1)) return; // outside nodes

		// Only set tooltip if pointer is inside mirrored node
		if (y >= node_ypos[node_id] && y <= (node_ypos[node_id] + nodeheight)) {
			const int x = event.GetX();
			if (x >= node_xpos[node_id] && x <= (node_xpos[node_id] + nodewidth)) {
				if (node_id == tooltippedNode) return; // tooltip already set

				// Give parent ctrl a chance to set tooltip
				VersionTreeEvent vt_event(wxEVT_VERSIONTREE_TOOLTIP, GetId());
				vt_event.SetEventObject(this);
				vt_event.SetItem(node_id);
				vt_event.Veto(); // default is no tooltip
				GetEventHandler()->ProcessEvent(vt_event);

				if (vt_event.IsAllowed()) {
					SetToolTip(vt_event.GetToolTip());
					tooltippedNode = node_id;
					return;
				}
			}
		}
	}

	// If we get to here we can delete previous tooltip
	if (GetToolTip()) {
		//tooltip->SetWindow(NULL); // wxWidgets bug workaround: SetToolTip dont delete the tooltip
		SetToolTip(NULL);
		tooltippedNode = size_t(-1);
	}
}
