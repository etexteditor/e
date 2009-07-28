///////////////////////////////////////////////////////////////////////////////
// Name:        promptservice.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-07
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#include <string>
#include <wx/wx.h>
#include <wx/artprov.h>
#include "webcontrol.h"
#include "nsinclude.h"
#include "promptservice.h"



///////////////////////////////////////////////////////////////////////////////
//  utilities
///////////////////////////////////////////////////////////////////////////////


wxWebControl* GetWebControlFromBrowserChrome(nsIWebBrowserChrome* chrome);

wxWebControl* GetWebControlFromDOMWindow(nsIDOMWindow* window)
{
    ns_smartptr<nsIWindowWatcher> window_watcher = nsGetWindowWatcherService();
    ns_smartptr<nsIWebBrowserChrome> chrome;
    
    if (window == NULL || window_watcher.empty())
    {
        // we don't have either a dom window pointer or
        // access to the window watcher service.  return error
        return NULL;
    }
    
    window_watcher->GetChromeForWindow(window, &chrome.p);
    
    return GetWebControlFromBrowserChrome(chrome);
}

wxWindow* GetTopFrameFromDOMWindow(nsIDOMWindow* window)
{
    wxWindow* win = GetWebControlFromDOMWindow(window);
    if (!win)
        return NULL;
        
    // now that we have a window, go up the window
    // hierarchy to find a frame
    
    wxWindow* w = win;
    while (1)
    {
        if (w->IsKindOf(CLASSINFO(wxFrame)))
            return w;
        
        wxWindow* old_win = w;
        w = w->GetParent();
        if (!w)
            return old_win;
    }
    
    return win;
}




///////////////////////////////////////////////////////////////////////////////
//  various dialogs
///////////////////////////////////////////////////////////////////////////////


class PromptDlgPassword : public wxDialog
{

    enum
    {
        ID_UsernameTextCtrl = wxID_HIGHEST+1,
        ID_PasswordTextCtrl
    };
    
public:

    PromptDlgPassword(wxWindow* parent)
                         : wxDialog(parent,
                                    -1,
                                    _("Authentication Required"),
                                    wxDefaultPosition,
                                    wxSize(400, 200),
                                    wxDEFAULT_DIALOG_STYLE |
                                    wxCENTER)
    {
        // create the username sizer
        
        wxStaticText* label_username = new wxStaticText(this,
                                                        -1,
                                                        _("User Name:"),
                                                        wxDefaultPosition,
                                                        wxDefaultSize);
        m_username_ctrl = new wxTextCtrl(this, ID_UsernameTextCtrl, m_username);
        
        wxBoxSizer* username_sizer = new wxBoxSizer(wxHORIZONTAL);
        username_sizer->Add(label_username, 0, wxALIGN_CENTER);
        username_sizer->Add(m_username_ctrl, 1, wxEXPAND);
        
        
        // create the password sizer
        
        wxStaticText* label_password = new wxStaticText(this,
                                                        -1,
                                                        _("Password:"),
                                                        wxDefaultPosition,
                                                        wxDefaultSize);
        m_password_ctrl = new wxTextCtrl(this,
                                         ID_PasswordTextCtrl,
                                         wxEmptyString,
                                         wxDefaultPosition,
                                         wxDefaultSize,
                                         wxTE_PASSWORD);
        
        wxBoxSizer* password_sizer = new wxBoxSizer(wxHORIZONTAL);
        password_sizer->Add(label_password, 0, wxALIGN_CENTER);
        password_sizer->Add(m_password_ctrl, 1, wxEXPAND);


        // create a platform standards-compliant OK/Cancel sizer
        
        wxButton* ok_button = new wxButton(this, wxID_OK);
        wxButton* cancel_button = new wxButton(this, wxID_CANCEL);
        
        wxStdDialogButtonSizer* ok_cancel_sizer = new wxStdDialogButtonSizer;
        ok_cancel_sizer->AddButton(ok_button);
        ok_cancel_sizer->AddButton(cancel_button);
        ok_cancel_sizer->Realize();
        ok_cancel_sizer->AddSpacer(5);
        
        ok_button->SetDefault();
        
        // this code is necessary to get the sizer's bottom margin to 8
        wxSize min_size = ok_cancel_sizer->GetMinSize();
        min_size.SetHeight(min_size.GetHeight()+16);
        ok_cancel_sizer->SetMinSize(min_size);
        
        
        // code to allow us to line up the static text elements
        wxSize s1 = label_username->GetSize();
        wxSize s2 = label_password->GetSize();
        wxSize max_size = wxSize(wxMax(s1.x, s2.x), wxMax(s1.y, s2.y));
        max_size.x += 10;
        username_sizer->SetItemMinSize(label_username, max_size);
        password_sizer->SetItemMinSize(label_password, max_size);


        // create username/password sizer
        
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_MESSAGE_BOX);
        wxStaticBitmap* bitmap_question = new wxStaticBitmap(this, -1, bmp);
        m_message_ctrl = new wxStaticText(this, -1, m_message);
        
        wxBoxSizer* vert_sizer = new wxBoxSizer(wxVERTICAL);
        vert_sizer->Add(m_message_ctrl, 0, wxEXPAND);
        vert_sizer->AddSpacer(16);
        vert_sizer->Add(username_sizer, 0, wxEXPAND);
        vert_sizer->AddSpacer(8);
        vert_sizer->Add(password_sizer, 0, wxEXPAND);
        
        // create top sizer
        
        wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);
        top_sizer->AddSpacer(7);
        top_sizer->Add(bitmap_question, 0, wxTOP, 7);
        top_sizer->AddSpacer(15);
        top_sizer->Add(vert_sizer, 1, wxEXPAND | wxTOP, 7);

        // create main sizer
        
        wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
        main_sizer->AddSpacer(8);
        main_sizer->Add(top_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
        main_sizer->AddStretchSpacer();
        main_sizer->Add(ok_cancel_sizer, 0, wxEXPAND);

        SetSizer(main_sizer);
        Layout();
    }
    
    ~PromptDlgPassword()
    {
        // clear out password in memory
        m_password = wxT("            ");
    }

    wxString GetUserName() const
    {
        return m_username;
    }
    
    wxString GetPassword() const
    {
        return m_password;
    }
    
    void SetMessage(const wxString& message)
    {
        m_message = message;
        m_message_ctrl->SetLabel(m_message);
        wxSizer* sizer = m_message_ctrl->GetContainingSizer();
        m_message_ctrl->Wrap(sizer->GetSize().GetWidth());
        Layout();
    }
    
    void SetUserName(const wxString& username)
    {
        m_username = username;
    }
    
    
private:

    // event handlers
    
    void OnOK(wxCommandEvent& evt)
    {
        m_username = m_username_ctrl->GetValue();
        m_password = m_password_ctrl->GetValue();
        
        EndModal(wxID_OK);
    }
    
    void OnCancel(wxCommandEvent& evt)
    {
        m_username = wxT("");
        m_password = wxT("");
        
        EndModal(wxID_CANCEL);
    }

private:
    
    wxString m_message;
    wxString m_username;
    wxString m_password;
    
    wxStaticText* m_message_ctrl;
    wxTextCtrl* m_username_ctrl;
    wxTextCtrl* m_password_ctrl;
    
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(PromptDlgPassword, wxDialog)
    EVT_BUTTON(wxID_OK, PromptDlgPassword::OnOK)
    EVT_BUTTON(wxID_CANCEL, PromptDlgPassword::OnCancel)
END_EVENT_TABLE()




///////////////////////////////////////////////////////////////////////////////
//  PromptService class implementation
///////////////////////////////////////////////////////////////////////////////


class PromptService : public nsIPromptService,
                      public nsIBadCertListener
{
public:

    PromptService();
    virtual ~PromptService();

    NS_DECL_ISUPPORTS
    NS_DECL_NSIPROMPTSERVICE
    NS_DECL_NSIBADCERTLISTENER
};


NS_IMPL_ADDREF(PromptService)
NS_IMPL_RELEASE(PromptService)

NS_INTERFACE_MAP_BEGIN(PromptService)
    NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIPromptService)
    NS_INTERFACE_MAP_ENTRY(nsIPromptService)
    NS_INTERFACE_MAP_ENTRY(nsIBadCertListener)
NS_INTERFACE_MAP_END


PromptService::PromptService()
{
}

PromptService::~PromptService()
{
}

NS_IMETHODIMP PromptService::Alert(nsIDOMWindow* parent,
                                   const PRUnichar* ns_dialog_title,
                                   const PRUnichar* ns_text)
{
    wxString title = ns2wx(ns_dialog_title);
    wxString text = ns2wx(ns_text);

    wxMessageBox(text,
                 title,
                 wxOK,
                 GetTopFrameFromDOMWindow(parent));
                 
    return NS_OK;
}

NS_IMETHODIMP PromptService::AlertCheck(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* ns_dialog_title,
                                    const PRUnichar* ns_text,
                                    const PRUnichar* ns_check_msg,
                                    PRBool* checkValue)
{
    wxString dialog_title = ns2wx(ns_dialog_title);
    wxString text = ns2wx(ns_text);
    wxString check_msg = ns2wx(ns_check_msg);
    
    return NS_OK;
}

NS_IMETHODIMP PromptService::Confirm(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* ns_dialog_title,
                                    const PRUnichar* ns_text,
                                    PRBool* retval)
{
    wxString dialog_title = ns2wx(ns_dialog_title);
    wxString text = ns2wx(ns_text);
    
    int res = wxMessageBox(text,
             dialog_title,
             wxYES_NO,
             GetTopFrameFromDOMWindow(parent));
    
    if (!retval)
        return NS_ERROR_NULL_POINTER;
    
    if (res == wxYES)
        *retval = PR_TRUE;
         else
        *retval = PR_FALSE;
           
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmCheck(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* dialog_title,
                                    const PRUnichar* text,
                                    const PRUnichar* check_msg,
                                    PRBool* check_value,
                                    PRBool* retval)
{
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmEx(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* dialog_title,
                                    const PRUnichar* text,
                                    PRUint32 buttonFlags,
                                    const PRUnichar* button0_title,
                                    const PRUnichar* button1_title,
                                    const PRUnichar* button2_title,
                                    const PRUnichar* check_msg,
                                    PRBool* check_value,
                                    PRInt32* button_pressed)
{
    return NS_OK;
}

NS_IMETHODIMP PromptService::Prompt(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* _dialog_title,
                                    const PRUnichar* _text,
                                    PRUnichar** _value,
                                    const PRUnichar* check_msg,
                                    PRBool* check_value,
                                    PRBool* retval)
{
    // check message and check value aren't implemented yet
    
    wxString dialog_title = ns2wx(_dialog_title);
    wxString text = ns2wx(_text);
    wxString value = ns2wx(*_value);
    
    wxTextEntryDialog dlg(GetTopFrameFromDOMWindow(parent),
                          text,
                          dialog_title,
                          value,
                          wxOK | wxCANCEL | wxCENTER);

    int res = dlg.ShowModal();
    if (res == wxID_OK)
    {
        *_value = wxToUnichar(dlg.GetValue());
        *retval = PR_TRUE;
    }
     else
    {
        *retval = PR_FALSE;
    }

    return NS_OK;
}

NS_IMETHODIMP PromptService::PromptUsernameAndPassword(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* dialog_title,
                                    const PRUnichar* text,
                                    PRUnichar** username,
                                    PRUnichar** password,
                                    const PRUnichar* check_msg,
                                    PRBool* check_value,
                                    PRBool* retval)
{
    wxWindow* wxparent = GetTopFrameFromDOMWindow(parent);
    
    PromptDlgPassword dlg(wxparent);
    dlg.SetMessage(ns2wx(text));
    
    int res = dlg.ShowModal();
    if (res == wxID_OK)
    {
        *username = wxToUnichar(dlg.GetUserName());
        *password = wxToUnichar(dlg.GetPassword());
        *retval = PR_TRUE;
    }
     else
    {
        *retval = PR_FALSE;
    }
    
    return NS_OK;
}

NS_IMETHODIMP PromptService::PromptPassword(
                                    nsIDOMWindow* parent,
                                    const PRUnichar* dialog_title,
                                    const PRUnichar* text,
                                    PRUnichar** password,
                                    const PRUnichar* check_msg,
                                    PRBool* check_value,
                                    PRBool* retval)
{
    return NS_OK;
}

NS_IMETHODIMP PromptService::Select(nsIDOMWindow* parent,
                                    const PRUnichar* dialog_title,
                                    const PRUnichar* text,
                                    PRUint32 count,
                                    const PRUnichar** select_list,
                                    PRInt32* out_selection,
                                    PRBool* retval)
{
    *retval = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmUnknownIssuer(
                                    nsIInterfaceRequestor* socketInfo,
                                    nsIX509Cert* cert,
                                    PRInt16* certAddType,
                                    PRBool* retval)
{
    int res = wxMessageBox(
        _("The requested web page is certified by an unknown authority.  Would you like to continue?"),
        _("Website Certified by an Unknown Authority"),
        wxICON_QUESTION | wxCENTER | wxYES_NO,
        NULL);
        
    if (res != wxYES)
    {
        *retval = PR_FALSE;
        return NS_OK;
    }
        
    *certAddType = nsIBadCertListener::ADD_TRUSTED_FOR_SESSION;
    *retval = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmMismatchDomain(
                                    nsIInterfaceRequestor* socketInfo,
                                    const nsACString& targetURL,
                                    nsIX509Cert *cert,
                                    PRBool* retval)
{
    int res = wxMessageBox(
        _("The requested web page uses a certificate which does not match the domain.  Would you like to continue?"),
        _("Domain Mismatch"),
        wxICON_QUESTION | wxCENTER | wxYES_NO,
        NULL);
        
    if (res != wxYES)
    {
        *retval = PR_FALSE;
        return NS_OK;
    }
    
    *retval = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmCertExpired(
                                    nsIInterfaceRequestor* socketInfo,
                                    nsIX509Cert* cert,
                                    PRBool* retval)
{
    int res = wxMessageBox(
        _("The requested web page uses a certificate which has expired.  Would you like to continue?"),
        _("Certificate Expired"),
        wxICON_QUESTION | wxCENTER | wxYES_NO,
        NULL);
    
    if (res != wxYES)
    {
        *retval = PR_FALSE;
        return NS_OK;
    }
    
    *retval = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP PromptService::NotifyCrlNextupdate(
                                    nsIInterfaceRequestor* socketInfo,
                                    const nsACString& targetURL,
                                    nsIX509Cert* cert)
{
    return NS_OK;
}




///////////////////////////////////////////////////////////////////////////////
//  PromptServiceFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class PromptServiceFactory : public nsIFactory
{
public:
    NS_DECL_ISUPPORTS
    
    PromptServiceFactory()
    {
        NS_INIT_ISUPPORTS();
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        
        PromptService* obj = new PromptService;
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        obj->Release();
        
        return res;
    }
    
    NS_IMETHOD LockFactory(PRBool lock)
    {
        return NS_OK;
    }
};

NS_IMPL_ISUPPORTS1(PromptServiceFactory, nsIFactory);


void CreatePromptServiceFactory(nsIFactory** result)
{
    PromptServiceFactory* obj = new PromptServiceFactory;
    obj->AddRef();
    *result = obj;
}




///////////////////////////////////////////////////////////////////////////////
//  TransferService class implementation
///////////////////////////////////////////////////////////////////////////////


class TransferService : public nsITransfer
{
public:

    NS_DECL_ISUPPORTS

    TransferService()
    {
    }

    NS_IMETHODIMP Init(nsIURI* source,
                       nsIURI* target,
                       const nsAString& display_name,
                       nsIMIMEInfo* mime_info,
                       PRTime start_time,
                       nsILocalFile* temp_file,
                       nsICancelable* cancelable)
    {
        return NS_OK;
    }

    NS_IMETHOD OnStateChange(nsIWebProgress* web_progress,
                             nsIRequest* request,
                             PRUint32 state_flags,
                             nsresult status)
    {
        return NS_OK;
    }

    NS_IMETHOD OnProgressChange(nsIWebProgress* web_progress,
                                nsIRequest* request,
                                PRInt32 cur_self_progress,
                                PRInt32 max_self_progress,
                                PRInt32 cur_total_progress,
                                PRInt32 max_total_progress)
    {
        return OnProgressChange64(web_progress,
                                  request,
                                  cur_self_progress,
                                  max_self_progress,
                                  cur_total_progress,
                                  max_total_progress);
    }
    
    NS_IMETHOD OnProgressChange64(
                                 nsIWebProgress* web_progress,
                                 nsIRequest* request,
                                 PRInt64 cur_self_progress,
                                 PRInt64 max_self_progress,
                                 PRInt64 cur_total_progress,
                                 PRInt64 max_total_progress)
    {
       return NS_OK;
    }
    
    NS_IMETHOD OnLocationChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             nsIURI* location)
    {
       return NS_OK;
    }

    NS_IMETHOD OnStatusChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             nsresult status,
                             const PRUnichar* message)
    {
        return NS_OK;
    }


    NS_IMETHOD OnSecurityChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             PRUint32 state)
    {
       return NS_OK;
    }
};


NS_IMPL_ADDREF(TransferService)
NS_IMPL_RELEASE(TransferService)

NS_INTERFACE_MAP_BEGIN(TransferService)
    NS_INTERFACE_MAP_ENTRY(nsISupports)
    NS_INTERFACE_MAP_ENTRY(nsITransfer)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener2)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
NS_INTERFACE_MAP_END




///////////////////////////////////////////////////////////////////////////////
//  TransferFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class TransferFactory : public nsIFactory
{
public:

    NS_DECL_ISUPPORTS
    
    TransferFactory()
    {
        NS_INIT_ISUPPORTS();
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        
        TransferService* obj = new TransferService();
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        obj->Release();
        
        return res;
    }
    
    NS_IMETHOD LockFactory(PRBool lock)
    {
        return NS_OK;
    }
};

NS_IMPL_ISUPPORTS1(TransferFactory, nsIFactory);


void CreateTransferFactory(nsIFactory** result)
{
    TransferFactory* obj = new TransferFactory;
    obj->AddRef();
    *result = obj;
}




///////////////////////////////////////////////////////////////////////////////
//  UnknownContentTypeHandler class implementation
///////////////////////////////////////////////////////////////////////////////


class UnknownContentTypeHandler : public nsIHelperAppLauncherDialog
{
public:

    NS_DECL_ISUPPORTS

    NS_IMETHOD Show(nsIHelperAppLauncher* launcher,
                    nsISupports* _context,
                    PRUint32 reason)
    {        
        ns_smartptr<nsISupports> context = _context;
        ns_smartptr<nsIDOMWindow> parent = nsRequestInterface(context);
        wxWebControl* ctrl = GetWebControlFromDOMWindow(parent);
        if (!ctrl)
        {
            // nobody to handle event, default action
            // is to save file to disk
            
            // BIW 7 May 2007 - this was causing save as dialogs to appear
            // during page loads.. we'll do nothing here instead
            return NS_OK;
        }
        
        
        wxString url;
        ns_smartptr<nsIURI> uri;
        launcher->GetSource(&uri.p);
        if (uri)
        {
            nsEmbedCString ns_spec;
            if (NS_SUCCEEDED(uri->GetSpec(ns_spec)))
                url = ns2wx(ns_spec);
        }
        
        
        
        nsEmbedString ns_filename;
        launcher->GetSuggestedFileName(ns_filename);
        wxString filename = ns2wx(ns_filename);
        
        
        nsEmbedCString ns_mimetype;
        ns_smartptr<nsIMIMEInfo> mime_info;
        launcher->GetMIMEInfo(&mime_info.p);
        wxString mime_type;
        if (mime_info)
        {
            mime_info->GetMIMEType(ns_mimetype);
            mime_type = ns2wx(ns_mimetype);
        }
        
        wxWebEvent evt(wxEVT_WEB_INITDOWNLOAD, ctrl->GetId());
        evt.SetEventObject(ctrl);
        evt.SetFilename(filename);
        evt.SetContentType(mime_type);
        evt.SetHref(url);
        bool handled = ctrl->GetEventHandler()->ProcessEvent(evt);

        if (handled)
        {
        
            switch (evt.m_download_action)
            {
                case wxWEB_DOWNLOAD_SAVE:
                    launcher->SaveToDisk(nsnull, PR_FALSE);
                    break;
                case wxWEB_DOWNLOAD_SAVEAS:
                    wxASSERT_MSG(evt.m_download_action_path.Length() > 0, wxT("You must specify a filename in the event object"));
                    if (evt.m_download_action_path.IsEmpty())
                    {
                        // no filename specified
                        launcher->Cancel(0x804b0002 /* = NS_BINDING_ABORTED */ );
                        return NS_OK;
                    }
                     else
                    {
                        std::string fname = (const char*)evt.m_download_action_path.mbc_str();
                        
                        nsILocalFile* filep = NULL;
                        NS_NewNativeLocalFile(nsDependentCString(fname.c_str()), PR_TRUE, &filep);

                        launcher->SaveToDisk(filep, PR_FALSE);
                        
                        if (filep)
                            filep->Release();
                    }
                    break;
                case wxWEB_DOWNLOAD_OPEN:
                    launcher->LaunchWithApplication(NULL, PR_FALSE);
                    break;
                case wxWEB_DOWNLOAD_CANCEL:
                    launcher->Cancel(0x804b0002 /* = NS_BINDING_ABORTED */ );
                    break;
            }
            
            
            if (evt.m_download_listener)
            {
                evt.m_download_listener->Init(url, evt.m_download_action_path);
                ProgressListenerAdaptor* progress = new ProgressListenerAdaptor(evt.m_download_listener);
                progress->AddRef();
                launcher->SetWebProgressListener(progress);
                progress->Release();
            }
        
        }
         else
        {
            launcher->SaveToDisk(nsnull, PR_FALSE);
            return NS_OK;
/*
            OpenOrSaveDlg dlg(GetTopFrameFromDOMWindow(parent), filename);
            int result = dlg.ShowModal();
            
            switch (result)
            {
                case wxID_OPEN:
                    break;
                case wxID_SAVE:
                    launcher->SaveToDisk(nsnull, PR_FALSE);
                    break;
                case wxID_CANCEL:
                    launcher->Cancel(0x804b0002 ); // = NS_BINDING_ABORTED
                    return NS_OK;
            }
            */
        }
            
        return NS_OK;
    }

    NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher* launcher,
                                   nsISupports* window_context,
                                   const PRUnichar* default_file,
                                   const PRUnichar* suggested_file_extension,
                                   nsILocalFile** new_file)
    {
        ns_smartptr<nsISupports> context = window_context;
        ns_smartptr<nsIDOMWindow> parent = nsRequestInterface(context);
        
        wxString default_filename = ns2wx(default_file);
        
        wxString filter;
        filter += _("All Files");
        filter += wxT(" (*.*)|*.*|");
        filter.RemoveLast(); // get rid of the last pipe sign

        wxFileDialog dlg(GetTopFrameFromDOMWindow(parent),
                         _("Save As"),
                         wxT(""),
                         default_filename,
                         filter,
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        
        if (dlg.ShowModal() != wxID_OK)
        {
            return NS_ERROR_FAILURE;
        }
        
        
        std::string fname = (const char*)dlg.GetPath().mbc_str();

        NS_NewNativeLocalFile(nsDependentCString(fname.c_str()), PR_TRUE, new_file);
        return NS_OK;
    }
};


NS_IMPL_ISUPPORTS1(UnknownContentTypeHandler, nsIHelperAppLauncherDialog);




///////////////////////////////////////////////////////////////////////////////
//  UnknownContentTypeHandlerFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class UnknownContentTypeHandlerFactory : public nsIFactory
{
public:
    NS_DECL_ISUPPORTS
    
    UnknownContentTypeHandlerFactory()
    {
        NS_INIT_ISUPPORTS();
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        
        UnknownContentTypeHandler* obj = new UnknownContentTypeHandler;
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        obj->Release();
        
        return res;
    }
    
    NS_IMETHOD LockFactory(PRBool lock)
    {
        return NS_OK;
    }
};

NS_IMPL_ISUPPORTS1(UnknownContentTypeHandlerFactory, nsIFactory);


void CreateUnknownContentTypeHandlerFactory(nsIFactory** result)
{
    UnknownContentTypeHandlerFactory* obj = new UnknownContentTypeHandlerFactory;
    obj->AddRef();
    *result = obj;
}

