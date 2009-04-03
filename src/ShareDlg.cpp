#ifdef FEAT_COLLABORATION

#include "ShareDlg.h"
#include <wx/mstream.h>
#include "SyncThread.h"
#include "images/userpic.h"
#include "zeroconf.h"

// Ctrl id's
enum {
	CTRL_USERADDRESS = 100,
	CTRL_NOTEBOOK,
	CTRL_LOCAL_USERLIST,
	CTRL_INET_USERLIST,
	CTRL_SHAREELIST,
	CTRL_BUTTON_APPROVE
};

BEGIN_EVENT_TABLE(ShareDlg, wxDialog)
	EVT_TEXT_ENTER(CTRL_USERADDRESS, ShareDlg::OnEnterUserAddress)
	EVT_SYNC_USERLIST_RECEIVED(ShareDlg::OnReceivingUserList)
	EVT_NOTEBOOK_PAGE_CHANGED(CTRL_NOTEBOOK, ShareDlg::OnNotebookSelChanged)
	EVT_LIST_ITEM_SELECTED(CTRL_INET_USERLIST, ShareDlg::OnUserListSelection)
	EVT_LIST_ITEM_DESELECTED(CTRL_INET_USERLIST, ShareDlg::OnUserListSelection)
	EVT_LIST_ITEM_SELECTED(CTRL_LOCAL_USERLIST, ShareDlg::OnLocalListSelection)
	EVT_LIST_ITEM_DESELECTED(CTRL_LOCAL_USERLIST, ShareDlg::OnLocalListSelection)
	EVT_LIST_ITEM_SELECTED(CTRL_SHAREELIST, ShareDlg::OnShareeListSelection)
	EVT_LIST_ITEM_DESELECTED(CTRL_SHAREELIST, ShareDlg::OnShareeListSelection)
	EVT_BUTTON(wxID_ADD, ShareDlg::OnButtonAdd)
	EVT_BUTTON(CTRL_BUTTON_APPROVE, ShareDlg::OnButtonApprove)
	EVT_BUTTON(wxID_REMOVE, ShareDlg::OnButtonRemove)
	EVT_SIZE(ShareDlg::OnSize)
END_EVENT_TABLE()


ShareDlg::ShareDlg(wxWindow *parent, CatalystWrapper cat, const doc_id& di)
: wxDialog (parent, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ),
  m_docId(di), m_catalyst(cat), m_dispatcher(cat.GetDispatcher()), m_localImageList(48, 48)
{
	SetTitle (_("Share Document"));
	
	// Buttons
	m_addButton = new wxButton(this, wxID_ADD, _("Add"));
	m_approveButton = new wxButton(this, CTRL_BUTTON_APPROVE, _("Approve"));
	m_removeButton = new wxButton(this, wxID_REMOVE, _("Remove"));
	wxButton* doneButton = new wxButton(this, wxID_OK, _("Done"));
	
	// Create the notebook with two pages
	m_notebook = new wxNotebook(this, CTRL_NOTEBOOK);
	{
		// List of local users (from zeroconf)
		m_localUserList = new wxListCtrl(m_notebook, CTRL_LOCAL_USERLIST);
		m_localUserList->SetImageList(&m_localImageList, wxIMAGE_LIST_NORMAL);
		m_notebook->AddPage(m_localUserList, _("Local"), true);

		// List of users from internet address (currently disabled)
		/*
		wxPanel* internetPage = new wxPanel(m_notebook, wxID_ANY);
		{
			wxBoxSizer* inetSizer = new wxBoxSizer(wxVERTICAL);
			{
				wxTextCtrl* addressCtrl = new wxTextCtrl(internetPage, CTRL_USERADDRESS);
				addressCtrl->SetWindowStyle(wxTE_PROCESS_ENTER);
				inetSizer->Add(addressCtrl, 0, wxEXPAND);

				m_inetUserList = new wxListCtrl(internetPage, CTRL_INET_USERLIST);
				inetSizer->Add(m_inetUserList, 1, wxEXPAND);
			}

			internetPage->SetSizerAndFit(inetSizer);
			m_notebook->AddPage(internetPage, _("Internet"), true);
		}
		*/
	}
	
	// List of current shares
	m_shareeList = new SizingListCtrl(this, CTRL_SHAREELIST);

	// Add columns to shareeList
	wxListItem itemCol;
	itemCol.SetText(_("User"));
	m_shareeList->InsertColumn(0, itemCol);
	itemCol.SetText(_("Share Status"));
	m_shareeList->InsertColumn(1, itemCol);

	// Create the layout
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	{
		wxBoxSizer* listSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			listSizer->Add(m_notebook, 2, wxEXPAND|wxALL, 5);

			wxStaticBoxSizer* sharesSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Document is currently shared with:"));
			sharesSizer->Add(m_shareeList, 1, wxEXPAND);
			listSizer->Add(sharesSizer, 3, wxEXPAND|wxALL, 5);
		}
		sizer->Add(listSizer, 1, wxEXPAND);

		wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
		buttonSizer->Add(m_addButton, 0, wxALL|wxALIGN_RIGHT, 5);
		buttonSizer->Add(m_approveButton, 0, wxALL|wxALIGN_RIGHT, 5);
		buttonSizer->Add(m_removeButton, 0, wxALL|wxALIGN_RIGHT, 5);
		buttonSizer->Add(doneButton, 0, wxALL|wxALIGN_RIGHT, 5);
		sizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
	}

	// Set the starting state
	m_notebook->SetSelection(0);
	UpdateShareList();
	m_addButton->Disable();
	m_approveButton->Disable();
	m_removeButton->Disable();

	//m_docListCtrl->SetFocus();
	SetSizerAndFit(sizer);
	SetSize(400, 450);
	Centre();

	// Get notified on changed to share list
#ifdef CATALYST_OLD_CODE
	cxLOCK_WRITE(m_catalyst)
		catalyst.GetOnlineUsers((CALL_BACK)OnNewUser, this);
	cxENDLOCK
#endif
	m_dispatcher.SubscribeC(wxT("NEW_ONLINE_USER"), (CALL_BACK)OnNewUser, this);
	m_dispatcher.SubscribeC(wxT("DOC_CHANGED_SHARE"), (CALL_BACK)OnChangeShare, this);
}

ShareDlg::~ShareDlg() {
	m_dispatcher.UnSubscribe(wxT("NEW_ONLINE_USER"), (CALL_BACK)OnNewUser, this);
	m_dispatcher.UnSubscribe(wxT("DOC_CHANGED_SHARE"), (CALL_BACK)OnChangeShare, this);
}

void ShareDlg::UpdateShareList() {
	m_shareeList->DeleteAllItems();

	vector<cxShare> shareList;
	cxLOCK_READ(m_catalyst)
#ifdef CATALYST_OLD_CODE
		catalyst.GetShares(m_docId, shareList);
#endif
	
		wxImageList* imageList = new wxImageList(48, 48);
		m_shareeList->AssignImageList(imageList, wxIMAGE_LIST_SMALL);

		for (unsigned int i = 0; i < shareList.size(); ++i) {
			const unsigned int userId = shareList[i].userId;
			const wxString userName = catalyst.GetUserName(userId);
			imageList->Add(catalyst.GetUserPic(userId));

			m_shareeList->InsertItem(i, userName, i);
			m_shareeList->SetItemData(i, userId);
			switch (shareList[i].status) {
			case cxSHARE_FULL:
				m_shareeList->SetItem(i, 1, _("shared"));
				break;
			case cxSHARE_PENDING:
				m_shareeList->SetItem(i, 1, _("pending"));
				break;
			case cxSHARE_READ:
				m_shareeList->SetItem(i, 1, _("read-only"));
				break;
			case cxSHARE_WRITE:
				m_shareeList->SetItem(i, 1, _("write-only"));
				break;
			default:
				wxASSERT(false);
			}
		}
	cxENDLOCK

	// The items are unselected
	m_approveButton->Disable();
	m_removeButton->Disable();
}

void ShareDlg::OnEnterUserAddress(wxCommandEvent& event) {
	const wxString address = event.GetString();
	if (address.empty()) return;

	// Try to connect to the remote address
	// (Get a list of users on the remote machine)
	wxIPV4address addr;
	addr.Hostname(address);
	addr.Service(cxCLIENT_PORT);
	new SyncThread(cxSYNC_GET_USERS, m_catalyst, addr, *this);
}

void ShareDlg::OnReceivingUserList(SyncEvent& event) {
	m_inetUserList->ClearAll();

	wxImageList* imageList = new wxImageList(48, 48);
	m_inetUserList->AssignImageList(imageList, wxIMAGE_LIST_NORMAL);

	m_userCache = event.GetUsers();
	for (int i = 0; i < m_userCache.GetSize(); ++i) {
		const c4_RowRef rUser = m_userCache[i];
		const wxString userName(pUserName(rUser), wxConvUTF8);

		// Get the user picture
		const c4_Bytes picBytes = pUserPic(rUser);
		if (picBytes.Size() == 0) {
			// If the users profile does not contain an image
			// we just return the generic userpid
			wxMemoryInputStream in(generic_userpic_png, GENERIC_USERPIC_PNG_LEN);
			const wxImage genericImage(in, wxBITMAP_TYPE_PNG);
		
			imageList->Add(wxBitmap(genericImage));
		}
		else {
			// We need to load the png image from profile and
			// convert it to a wxBitmap
			wxMemoryInputStream in(picBytes.Contents(), picBytes.Size());
			const wxImage userImage(in, wxBITMAP_TYPE_PNG);
			
			imageList->Add(wxBitmap(userImage));
		}
		
		m_inetUserList->InsertItem(i, userName, i);
	}
}

void ShareDlg::OnNotebookSelChanged(wxNotebookEvent& WXUNUSED(event)) {
	if (m_notebook->GetSelection() == 0) { // Local
		if (m_localUserList->GetSelectedItemCount() > 0) m_addButton->Enable();
		else m_addButton->Disable();
	}
	else if (m_notebook->GetSelection() == 1) { // Internet
		if (m_inetUserList->GetSelectedItemCount() > 0) m_addButton->Enable();
		else m_addButton->Disable();
	}
	else wxASSERT(false); // invalid page
}


void ShareDlg::OnUserListSelection(wxListEvent& WXUNUSED(event)) {
	if (m_inetUserList->GetSelectedItemCount() > 0) m_addButton->Enable();
	else m_addButton->Disable();
}

void ShareDlg::OnLocalListSelection(wxListEvent& WXUNUSED(event)) {
	if (m_localUserList->GetSelectedItemCount() > 0) m_addButton->Enable();
	else m_addButton->Disable();
}

void ShareDlg::OnShareeListSelection(wxListEvent& WXUNUSED(event)) {
	if (m_shareeList->GetSelectedItemCount() > 0) {
		m_removeButton->Enable();

		// Check if any pending users are selected
		cxLOCK_READ(m_catalyst)
			long item = -1;
			while (1) {
				item = m_shareeList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
				if (item == -1) break;

#ifdef CATALYST_OLD_CODE
				// Check if the user is in pending state
				cxShareStatus shareStatus;
				if (catalyst.GetShareStatus(m_docId, m_shareeList->GetItemData(item), shareStatus)) {
					if (shareStatus == cxSHARE_PENDING) {
						m_approveButton->Enable();
						break; // no need to check for more
					}
				}
#endif
			}
		cxENDLOCK
	}
	else {
		m_approveButton->Disable();
		m_removeButton->Disable();
	}
}

void ShareDlg::OnButtonAdd(wxCommandEvent& WXUNUSED(event)) {
	long item = -1;

	if (m_notebook->GetSelection() == 0) { // Local
		while (1) {
			item = m_localUserList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if (item == -1) break;

			const c4_RowRef rUser = m_localUserCache[item];

			cxLOCK_WRITE(m_catalyst)
				// Check if we know this user (adds if we don't)
				const int userId = catalyst.AddUser(rUser);
				wxASSERT(userId != -1);

#ifdef CATALYST_OLD_CODE
				// Share document with user
				catalyst.Share(m_docId, userId);
#endif
			cxENDLOCK
		}
	}
	else if (m_notebook->GetSelection() == 1) { // Internet
		while (1) {
			item = m_inetUserList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if (item == -1) break;

			const c4_RowRef rUser = m_userCache[item];
			
			cxLOCK_WRITE(m_catalyst)
				// Check if we know this user (adds if we don't)
				const int userId = catalyst.AddUser(rUser);
				wxASSERT(userId != -1);

#ifdef CATALYST_OLD_CODE
				// Share document with user
				catalyst.Share(m_docId, userId);
#endif
			cxENDLOCK
		}
	}
	else wxASSERT(false); // invalid page
}

void ShareDlg::OnButtonApprove(wxCommandEvent& WXUNUSED(event)) {
	cxLOCK_WRITE(m_catalyst)
		long item = -1;
		while (1) {
			item = m_shareeList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if (item == -1) break;

			// Share document with user
			const unsigned int userId = m_shareeList->GetItemData(item);
#ifdef CATALYST_OLD_CODE
			catalyst.Share(m_docId, userId);
#endif
		}
	cxENDLOCK
}

void ShareDlg::OnButtonRemove(wxCommandEvent& WXUNUSED(event)) {
	cxLOCK_WRITE(m_catalyst)
		long item = -1;
		while (1) {
			item = m_shareeList->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if (item == -1) break;

			// Remove Share from document
			const unsigned int userId = m_shareeList->GetItemData(item);
#ifdef CATALYST_OLD_CODE
			catalyst.RemoveShare(m_docId, userId);
#endif
		}
	cxENDLOCK
}

void ShareDlg::OnChangeShare(ShareDlg* self, void* data, int WXUNUSED(filter)) { // static
	if (*((doc_id*)data) == self->m_docId) {
		self->UpdateShareList();
	}
}

void ShareDlg::OnNewUser(ShareDlg* self, void* data, int WXUNUSED(filter)) { // static
#ifdef FEAT_COLLABORATION
	cxUserMsg* msg = (cxUserMsg*)data;
	const c4_RowRef rUser = msg->rUser;
	
	if (msg->flags & wxZEROCONF_ADD) {
		const wxString userName(pUserName(rUser), wxConvUTF8);

		// Get the user picture
		const c4_Bytes picBytes = pUserPic(rUser);
		if (picBytes.Size() == 0) {
			// If the users profile does not contain an image
			// we just return the generic userpid
			wxMemoryInputStream in(generic_userpic_png, GENERIC_USERPIC_PNG_LEN);
			const wxImage genericImage(in, wxBITMAP_TYPE_PNG);
		
			self->m_localImageList.Add(wxBitmap(genericImage));
		}
		else {
			// We need to load the png image from profile and
			// convert it to a wxBitmap
			wxMemoryInputStream in(picBytes.Contents(), picBytes.Size());
			const wxImage userImage(in, wxBITMAP_TYPE_PNG);
			
			self->m_localImageList.Add(wxBitmap(userImage));
		}
		
		const int ndx = self->m_localUserList->GetItemCount();
		self->m_localUserList->InsertItem(ndx, userName, ndx);
		self->m_localUserCache.Add(rUser);
	}
	else { // wxZEROCONF_DELETE
		int ndx = self->m_localUserCache.Find(pUserID[pUserID(rUser)]);
		if (ndx != -1) {
			self->m_localUserCache.RemoveAt(ndx);
			self->m_localUserList->DeleteItem(ndx);
			self->m_localImageList.Remove(ndx);

		}
		else wxASSERT(false); // unknown user
	}
#else
	wxUnusedVar(self);
	wxUnusedVar(data);
#endif
}


BEGIN_EVENT_TABLE(ShareDlg::SizingListCtrl, wxListCtrl)
	EVT_SIZE(ShareDlg::SizingListCtrl::OnSize)
END_EVENT_TABLE()

ShareDlg::SizingListCtrl::SizingListCtrl(wxWindow* parent, wxWindowID id)
: wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT), m_colSize1(100)
{}

void ShareDlg::SizingListCtrl::OnSize(wxSizeEvent& WXUNUSED(event)) {
	// Resize columns
	const wxSize listSize = GetClientSize();
	SetColumnWidth(0, listSize.x - m_colSize1);
	SetColumnWidth(1, m_colSize1);
}

#endif // FEAT_COLLABORATION

