#ifndef __SHAREDLG_H__
#define __SHAREDLG_H__

#ifdef FEAT_COLLABORATION

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include <wx/imaglist.h>
#include "Catalyst.h"
#include <wx/listctrl.h>
#include <wx/notebook.h>

// pre-definitions
class SyncEvent;

class ShareDlg : public wxDialog {
public:
	ShareDlg(wxWindow *parent, CatalystWrapper cat, const doc_id& di);
	~ShareDlg();

private:
	class SizingListCtrl : public wxListCtrl {
	public:
		SizingListCtrl(wxWindow* parent, wxWindowID id);
	private:
		void OnSize(wxSizeEvent& event);
		DECLARE_EVENT_TABLE();
		unsigned int m_colSize1;
	};

	void UpdateShareList();

	// Event handlers
	void OnEnterUserAddress(wxCommandEvent& event);
	void OnReceivingUserList(SyncEvent& event);
	void OnNotebookSelChanged(wxNotebookEvent& event);
	void OnUserListSelection(wxListEvent& event);
	void OnLocalListSelection(wxListEvent& event);
	void OnShareeListSelection(wxListEvent& event);
	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonApprove(wxCommandEvent& event);
	void OnButtonRemove(wxCommandEvent& event);
	DECLARE_EVENT_TABLE();

	// Static notifier handlers
	static void OnChangeShare(ShareDlg* self, void* data, int filter);
	static void OnNewUser(ShareDlg* self, void* data, int filter);

	// Member Ctrl's
	wxNotebook* m_notebook;
	wxListCtrl* m_localUserList;
	wxImageList m_localImageList;
	wxListCtrl* m_inetUserList;
	wxListCtrl* m_shareeList;
	wxButton* m_addButton;
	wxButton* m_approveButton;
	wxButton* m_removeButton;

	// Member variables
	const doc_id m_docId;
	CatalystWrapper m_catalyst;
	Dispatcher& m_dispatcher;
	c4_View m_userCache;
	c4_View m_localUserCache;
};

#endif // FEAT_COLLABORATION

#endif // __SHAREDLG_H_
