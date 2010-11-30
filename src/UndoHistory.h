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

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include "Catalyst.h"
#include <vector>
#include "Cell.h"

// Pre-declarations
class EditorCtrl;
class IFrameUndoPane;
class VersionTree;
class VersionTreeEvent;

class UndoHistory : public wxControl {
public:
	UndoHistory(CatalystWrapper& cw, IFrameUndoPane* parentFrame, int win_id, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	~UndoHistory();

	void SetEditorCtrl(EditorCtrl* ctrl);
	void Clear();
	void SetDocument(const doc_id& di, bool doCenter=false);

	bool IsSelectionMode() const {return !m_rangeHistory.empty();};
	
	bool Show(bool show);

private:
	void UpdateTree(bool doCenter=false);
	void UpdateCaption(const wxChar* caption);
	void DrawLayout(wxDC& dc);

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnIdle(wxIdleEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnVersionTreeSel(VersionTreeEvent& event);
	void OnVersionTreeContextMenu(VersionTreeEvent& event);
	void OnVersionTreeTooltip(VersionTreeEvent& event);
	void OnVersionTreeDrawitem(VersionTreeEvent& event);
	void OnMenuDiffToCurrent(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Notifier handlers
	static void OnClosePage(UndoHistory* self, void* data, int filter);
	static void OnChangeDoc(UndoHistory* self, void* data, int filter);
	static void OnDocDeleted(UndoHistory* self, void* data, int filter);
	static void OnNewRevision(UndoHistory* self, void* data, int filter);
	static void OnUpdateRevision(UndoHistory* self, void* data, int filter);
	static void OnDocUpdated(UndoHistory* self, void* data, int filter);
	static void OnDocCommited(UndoHistory* self, void* data, int filter);


	// Member variables
	CatalystWrapper m_catalyst;
	DocumentWrapper m_doc; // Initialized with rev0
	Dispatcher& m_dispatcher;

	wxMemoryDC m_mdc;
	wxBitmap m_bitmap;
	DiffLineCell m_cell;

	bool m_ignoreUpdates;

	EditorCtrl* m_editorCtrl;
	IFrameUndoPane* m_parentFrame;

	VersionTree* m_pTree;

	wxBrush bgBrush;
	wxPen linePen;

	doc_id m_sourceDoc;
	const int m_source_win_id;
	interval m_range;
	std::vector<cxDiffEntry> m_rangeHistory;

	bool m_needRedrawing;
	int m_selectedNode;
	int m_lineHeight;
	int m_treeHeight;
	int m_treeWidth;
	int m_verticalScrollPos, m_horizontalScrollPos;
	int m_oldVerticalScrollPos, m_oldHorizontalScrollPos;
	bool m_isScrolling;

	int m_popupItem;
};
