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

#ifndef __VERSIONTREE_H__
#define __VERSIONTREE_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>

class VersionTree : public wxControl {
public:
	VersionTree(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void Clear();
	size_t AddRoot();
	size_t AddNode(int parent, int ypos=-1);
	//int InsertNode(int pos, int parent);
	void CalculateLayout();
	size_t GetNodeCount() const {return parents.size();};

	void Select(size_t node_id);
	void MakeNodeVisible(size_t node_id, bool doCenter=false);

	void ScrollVertical(int pos);
	void Scroll(int xpos, int ypos);
	int GetVerticalScrollPos() const {return verticalScrollPos;};
	int GetHorizontalScrollPos() const {return horizontalScrollPos;};
	void UpdateTree();
	int GetWidth() { return treeWidth; }

protected:
	wxSize DoGetBestSize() const {return wxSize(treeWidth,-1);};

private:
	void DrawTree(wxRect& rect);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseRightDown(wxMouseEvent& event);
	void OnMouseDClick(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	DECLARE_EVENT_TABLE();

	wxMemoryDC mdc;
	wxBitmap bitmap;

	int verticalScrollPos, horizontalScrollPos;
	bool needRecalc;
	bool needRedrawing;
	wxBrush bgBrush;
	wxBrush nodeBrush;
	wxBrush selectBrush;
	wxBrush savedBrush;
	wxPen linePen;
	wxPen nodePen;
	wxPen draftPen;

	// Tree layout
	int xoffset;
	int yoffset;
	int nodewidth;
	int nodeheight;
	int nodespacing;
	int lineheight;

	// Tree structure
	size_t selectedNode;
	size_t tooltippedNode;
	int treeWidth;
	int treeHeight;
	std::vector<size_t> parents;
	std::vector<int> node_xpos;
	std::vector<int> node_ypos;

	// Bitmaps
	const wxBitmap m_bmDoc;
	const wxBitmap m_bmDocPending;
	const wxBitmap m_bmOverlaySelected;
	const wxBitmap m_bmOverlayUnread;
	const wxBitmap m_bmOverlayMirrored;
	const wxBitmap m_bmDraft;
	const wxBitmap m_bmDraftSelected;
};

static const int VTREESTYLE_NONE    = 0x0000;
static const int VTREESTYLE_SAVED   = 0x0001;
static const int VTREESTYLE_DRAFT   = 0x0002;
static const int VTREESTYLE_PENDING = 0x0004;
static const int VTREESTYLE_UNREAD  = 0x0008;

class VersionTreeEvent : public wxNotifyEvent
{
public:
    VersionTreeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
		: wxNotifyEvent(commandType, id) {};
    VersionTreeEvent(const VersionTreeEvent & event)
		: wxNotifyEvent(event) {m_label = event.m_label; m_item = event.m_item; m_style = event.m_style;};

    virtual wxEvent *Clone() const { return new VersionTreeEvent(*this); }

	size_t GetItem() const {return m_item;};
	void SetItem(size_t item_id) {m_item = item_id;};

	void SetItemStyle(int style) {m_style = style;};
	int GetItemStyle() const {return m_style;};

	void SetToolTip(const wxString& toolTip) { m_label = toolTip; }
    wxString GetToolTip() const { return m_label; }

private:
	wxString m_label;
	size_t m_item;
	int m_style;

	DECLARE_DYNAMIC_CLASS(VersionTreeEvent)
};

typedef void (wxEvtHandler::*VersionTreeEventFunction)(VersionTreeEvent&);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_VERSIONTREE_SEL_CHANGED, 800)
	DECLARE_EVENT_TYPE(wxEVT_VERSIONTREE_CONTEXTMENU, 801)
	DECLARE_EVENT_TYPE(wxEVT_VERSIONTREE_TOOLTIP, 802)
	DECLARE_EVENT_TYPE(wxEVT_VERSIONTREE_DRAWITEM, 803)
END_DECLARE_EVENT_TYPES()

#define VersionTreeEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(VersionTreeEventFunction, &func)

#define DECLARE_VERSIONTREEEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_VERSIONTREE_ ## evt, id, VersionTreeEventHandler(fn))


#define EVT_VERSIONTREE_SEL_CHANGED(id, fn) DECLARE_VERSIONTREEEVT(SEL_CHANGED, id, fn)
#define EVT_VERSIONTREE_CONTEXTMENU(id, fn) DECLARE_VERSIONTREEEVT(CONTEXTMENU, id, fn)
#define EVT_VERSIONTREE_TOOLTIP(id, fn) DECLARE_VERSIONTREEEVT(TOOLTIP, id, fn)
#define EVT_VERSIONTREE_DRAWITEM(id, fn) DECLARE_VERSIONTREEEVT(DRAWITEM, id, fn)

#endif // __VERSIONTREE_H__
