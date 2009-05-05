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

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"
#include <vector>
#include "Cell.h"
#include "Timeline.h"
#include "RevTooltip.h"

class VersionTree;
class VersionTreeEvent;

class DocHistory : public wxControl {
public:
	DocHistory(CatalystWrapper& cw, int win_id, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	~DocHistory();

private:
	void Clear();
	void SetDocument(const doc_id& di);
	void ReBuildTree();
	void Select(unsigned int node_id);
	void MakeItemVisible(unsigned int item_id);
	void DrawLayout(wxDC& dc);
	void AddChildren(int parent_pos, const doc_id& doc, const doc_id& sel_doc, const Catalyst& catalyst);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMotion(wxMouseEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnLeaveWindow(wxMouseEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnVersionTreeSel(VersionTreeEvent& event);
	void OnVersionTreeTooltip(VersionTreeEvent& event);
	void OnVersionTreeDrawitem(VersionTreeEvent& event);
	void OnTooltipTimer(wxTimerEvent& event);
	DECLARE_EVENT_TABLE();

	// Notifier handlers
	static void OnChangeDoc(DocHistory* self, void* data, int filter);
	static void OnDocUpdated(DocHistory* self, void* data, int filter);
	static void OnDocDeleted(DocHistory* self, void* data, int filter);
	static void OnDocCommited(DocHistory* self, void* data, int filter);
	static void OnNewRevision(DocHistory* self, void* data, int filter);
	static void OnUpdateRevision(DocHistory* self, void* data, int filter);

	// Member variables
	CatalystWrapper& m_catalyst;
	Dispatcher& m_dispatcher;
	DocumentWrapper m_doc;
	wxMemoryDC m_mdc;
	wxBitmap m_bitmap;
	DiffLineCell m_cell;
	VersionTree* m_pTree;
	Timeline* m_pTimeline;
	wxTimer m_tooltipTimer;
	int m_hotNode;
	RevTooltip m_revTooltip;

	wxBrush bgBrush;
	wxPen linePen;

	struct item {
		wxDateTime date;
		int parent;
		doc_id doc;
		int ypos;
	};
	vector<item> m_items;
	vector<int> m_positions;

	bool m_needRedrawing;
	int m_document_id;
	doc_id m_sourceDoc;
	int m_selectedNode;
	int m_source_win_id;
	int m_lineHeight;
	int m_treeHeight;
	int m_scrollPos;
	int m_oldScrollPos;
	bool m_isScrolling;
	int m_xposDesc;
};
