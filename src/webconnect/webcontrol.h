///////////////////////////////////////////////////////////////////////////////
// Name:        webcontrol.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-09-22
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_WEBCONTROL_H
#define __WXWEBCONNECT_WEBCONTROL_H


#include "dom.h"



///////////////////////////////////////////////////////////////////////////////
//  web states, used by the EVT_WEB_STATECHANGE event
///////////////////////////////////////////////////////////////////////////////


enum wxWebState
{
    wxWEB_STATE_NONE =         0,
    wxWEB_STATE_START =        1 << 0,
    wxWEB_STATE_REDIRECTING =  1 << 1,
    wxWEB_STATE_TRANSFERRING = 1 << 2,
    wxWEB_STATE_NEGOTIATING =  1 << 3,
    wxWEB_STATE_STOP =         1 << 4,
    
    wxWEB_STATE_IS_REQUEST =   1 << 5,
    wxWEB_STATE_IS_DOCUMENT =  1 << 6,
    wxWEB_STATE_IS_NETWORK =   1 << 7,
    wxWEB_STATE_IS_WINDOW =    1 << 8,
};


enum wxWebResult
{
    wxWEB_RESULT_NONE =                     0,
    wxWEB_RESULT_SUCCESS =                  1 << 0,
    wxWEB_RESULT_UNKNOWN_PROTOCOL =         1 << 1,
    wxWEB_RESULT_FILE_NOT_FOUND =           1 << 2,
    wxWEB_RESULT_UNKNOWN_HOST =             1 << 3,
    wxWEB_RESULT_CONNECTION_REFUSED =       1 << 4,
    wxWEB_RESULT_NET_INTERRUPT =            1 << 5,
    wxWEB_RESULT_NET_TIMEOUT =              1 << 6,
    wxWEB_RESULT_MALFORMED_URI =            1 << 7,
    wxWEB_RESULT_REDIRECT_LOOP =            1 << 8,
    wxWEB_RESULT_UNKNOWN_SOCKET_TYPE =      1 << 9,
    wxWEB_RESULT_NET_RESET =                1 << 10,
    wxWEB_RESULT_DOCUMENT_NOT_CACHED =      1 << 11,
    wxWEB_RESULT_DOCUMENT_IS_PRINTMODE =    1 << 12,
    wxWEB_RESULT_PORT_ACCESS_NOT_ALLOWED =  1 << 13,
    wxWEB_RESULT_UNKNOWN_PROXY_HOST =       1 << 14,
    wxWEB_RESULT_PROXY_CONNECTION_REFUSED = 1 << 15
};


enum wxWebLoadFlag
{
    wxWEB_LOAD_NORMAL =        0,
    wxWEB_LOAD_LINKCLICK =     1 << 1
};


enum wxWebFindFlag
{
    wxWEB_FIND_BACKWARDS =     1 << 1,
    wxWEB_FIND_WRAP =          1 << 2,
    wxWEB_FIND_ENTIRE_WORD =   1 << 3,
    wxWEB_FIND_MATCH_CASE =    1 << 4,
    wxWEB_FIND_SEARCH_FRAMES = 1 << 5
};


enum wxWebDownloadAction
{
    wxWEB_DOWNLOAD_OPEN = 1,
    wxWEB_DOWNLOAD_SAVE = 2,
    wxWEB_DOWNLOAD_SAVEAS = 3,
    wxWEB_DOWNLOAD_CANCEL = 4
};


enum wxWebChromeFlags
{
    wxWEB_CHROME_MODAL =     1 << 1,
    wxWEB_CHROME_RESIZABLE = 1 << 2,
    wxWEB_CHROME_CENTER =    1 << 3,
    wxWEB_CHROME_CENTRE =    1 << 3,
};


enum wxWebDOMNodeType
{
    // see DOM Level 2 spec for more info
    wxWEB_NODETYPE_ELEMENT_NODE = 1,
    wxWEB_NODETYPE_ATTRIBUTE_NODE = 2,
    wxWEB_NODETYPE_TEXT_NODE = 3,
    wxWEB_NODETYPE_CDATA_SECTION_NODE = 4,
    wxWEB_NODETYPE_ENTITY_REFERENCE_NODE = 5,
    wxWEB_NODETYPE_ENTITY_NODE = 6,
    wxWEB_NODETYPE_PROCESSING_INSTRUCTION_NODE = 7,
    wxWEB_NODETYPE_COMMENT_NODE = 8,
    wxWEB_NODETYPE_DOCUMENT_NODE = 9,
    wxWEB_NODETYPE_DOCUMENT_TYPE_NODE = 10,
    wxWEB_NODETYPE_DOCUMENT_FRAGMENT_NODE = 11,
    wxWEB_NODETYPE_NOTATION_NODE = 12,
};


class BrowserChrome;
struct EmbeddingPtrs;




///////////////////////////////////////////////////////////////////////////////
//  wxWebContentHandler class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebContentHandler
// Category: Content
// Description: Used for intercepting and handling specific types of content
//     in order to override the default manner in which the web control 
//     handles certain types of content.
// Remarks: The wxWebContentHandler class is used for intercepting and handling 
//     specific types of content in order to override the default manner in 
//     which the web control handles certain types of content.

// (METHOD) wxWebContentHandler::CanHandleContent
// Syntax: bool wxWebContentHandler::CanHandleContent(const wxString& url,
//                                                    const wxString& mime_type)

// (METHOD) wxWebContentHandler::OnStartRequest
// Syntax: void wxWebContentHandler::OnStartRequest(const wxString& url)

// (METHOD) wxWebContentHandler::OnStopRequest
// Syntax: void wxWebContentHandler::OnStartRequest(const wxString& url)

// (METHOD) wxWebContentHandler::OnData
// Syntax: void wxWebContentHandler::OnData(const unsigned char* buf, 
//                                          size_t size)

class wxWebContentHandler
{
public:

    virtual ~wxWebContentHandler() { }

    virtual bool CanHandleContent(const wxString& url,
                                  const wxString& mime_type) = 0;

    virtual void OnStartRequest(const wxString& url) { }
    virtual void OnStopRequest() { }
    virtual void OnData(const unsigned char* buf, size_t size) { }
};

WX_DEFINE_ARRAY_PTR(wxWebContentHandler*, wxWebContentHandlerPtrArray);




///////////////////////////////////////////////////////////////////////////////
//  wxWebProgressBase class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebProgressBase
// Category: Content
// Description: Used to get information about the progress of a download.
//     Progress information receivers should derive from this class and 
//     override the desired methods.
// Remarks: The wxWebProgressBase class is used to get information about
//     the progress of a download.  Progress information receivers should
//     derive from this class and override the desired methods.

// (METHOD) wxWebProgressBase::OnStartRequest
// Syntax: void wxWebProgressBase::Init(const wxString& url,
//                                      const wxString& suggested_filename)

// (METHOD) wxWebProgressBase::Cancel
// Syntax: void wxWebProgressBase::Cancel()

// (METHOD) wxWebProgressBase::IsCancelled
// Syntax: bool wxWebProgressBase::IsCancelled()

// (METHOD) wxWebProgressBase::OnStart
// Syntax: void wxWebProgressBase::OnStart()

// (METHOD) wxWebProgressBase::OnFinish
// Syntax: void wxWebProgressBase::OnFinish()

// (METHOD) wxWebProgressBase::OnError
// Syntax: void wxWebProgressBase::OnError()

// (METHOD) wxWebProgressBase::OnProgressChange
// Syntax: void wxWebProgressBase::OnProgressChange(wxLongLong cur_progress,
//                                                  wxLongLong max_progress)

class wxWebProgressBase
{
public:

    wxWebProgressBase()
    {
        m_cancelled = false;
    }
    
    virtual ~wxWebProgressBase() { }
    
    virtual void Init(const wxString& url,
                      const wxString& suggested_filename) { }
    
    virtual void Cancel() { m_cancelled = true; }
    virtual bool IsCancelled() const { return m_cancelled; }
    virtual void OnStart() { }
    virtual void OnFinish() { }
    virtual void OnError(const wxString& message) { }
    
    virtual void OnProgressChange(wxLongLong cur_progress,
                                  wxLongLong max_progress) { }
    
protected:

    bool m_cancelled;
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebPostData class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebPostData
// Category: Content
// Description: Used for building up a POST request, which can be issued 
//     through wxWebControl::openURI().
// Remarks: The wxWebPostData class is used for building up a POST request,
//     which can be issued through wxWebControl::openURI().

// (METHOD) wxWebPostData::Add
// Syntax: void wxWebPostData::Add(const wxString& variable, 
//                                 const wxString& value)

// (METHOD) wxWebPostData::GetPostString
// Syntax: wxString wxWebPostData::GetPostString()

class wxWebPostData
{
public:

    void Add(const wxString& variable, const wxString& value);
    wxString GetPostString();
    
private:

    wxArrayString m_vars;
    wxArrayString m_values;
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebPreferences class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebPreferences
// Category: Content
// Description: Used for configuring a the preferences of a web control.
// Remarks: The wxWebPreferences class is used for configuring the
//     preferences of a web control.

// (METHOD) wxWebPreferences::GetBoolPref
// Syntax: bool wxWebPreferences::GetBoolPref(const wxString& name)

// (METHOD) wxWebPreferences::GetStringPref
// Syntax: wxString wxWebPreferences::GetStringPref(const wxString& name)

// (METHOD) wxWebPreferences::GetIntPref
// Syntax: int wxWebPreferences::GetIntPref(const wxString& name)

// (METHOD) wxWebPreferences::SetIntPref
// Syntax: void wxWebPreferences::SetIntPref(const wxString& name, 
//                                           int value)

// (METHOD) wxWebPreferences::SetStringPref
// Syntax: void wxWebPreferences::SetStringPref(const wxString& name, 
//                                              const wxString& value)

// (METHOD) wxWebPreferences::SetBoolPref
// Syntax: void wxWebPreferences::SetBoolPref(const wxString& name, 
//                                            bool value)

class wxWebPreferences
{
friend class wxWebControl;

private:

    // private constructor - please use wxWebControl::GetWebPreferences()
    wxWebPreferences();
    
public:

    bool GetBoolPref(const wxString& name);
    wxString GetStringPref(const wxString& name);
    int GetIntPref(const wxString& name);

    void SetIntPref(const wxString& name, int value);
    void SetStringPref(const wxString& name, const wxString& value);
    void SetBoolPref(const wxString& name, bool value);
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebControl class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebControl
// Category: Control
// Derives: wxControl
// Description: Primary web browser control.  Used for displaying and
//     interacting with web content.
// Remarks:  The wxWebControl class is the primary web browser control
//     class; it is used for displaying and interacting with web content.

class wxWebFavIconProgress;
class wxWebControl : public wxControl
{
friend class BrowserChrome;
friend class WindowCreator;
friend class wxWebFavIconProgress;

public:

    static bool InitEngine(const wxString& path);
    static bool IsInitialized();
    static bool AddContentHandler(wxWebContentHandler* handler, bool take_ownership = false);
    static void AddPluginPath(const wxString& path);
    static wxWebPreferences GetPreferences();
    
    static bool SaveRequest(
                 const wxString& uri,
                 const wxString& destination_path,
                 wxWebPostData* post_data = NULL,
                 wxWebProgressBase* listener = NULL);
                 
    static bool SaveRequestToString(
                 const wxString& uri,
                 wxString* result = NULL,
                 wxWebPostData* post_data = NULL,
                 wxWebProgressBase* listener = NULL);
                 
    static bool ClearCache();
                 
public:

    wxWebControl(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize);
    ~wxWebControl();
    
    bool IsOk() const;
    
    // navigation
    void OpenURI(const wxString& uri,
                 unsigned int flags = wxWEB_LOAD_NORMAL,
                 wxWebPostData* post_data = NULL,
				 bool bGrabFocus = true);
                     
    wxString GetCurrentURI() const;
    void GoForward();
    void GoBack();
    void Reload();
    void Stop();
    bool IsContentLoaded() const;
    
    // javascript
    bool Execute(const wxString& js_code);
    
    // printing
    void Print(bool silent = false);
    void SetPageSettings(double page_width, double page_height,
                         double left_margin, double right_margin, double top_margin, double bottom_margin);
    void GetPageSettings(double* page_width, double* page_height,
                         double* left_margin, double* right_margin, double* top_margin, double* bottom_margin);

    // view source
    void ViewSource();
    void ViewSource(wxWebControl* source_web_browser);
    void ViewSource(const wxString& uri);
    
    // save
    bool SaveCurrent(const wxString& destination_path);
    
    // zoom
    void GetTextZoom(float* zoom);
    void SetTextZoom(float zoom);

    // find
    bool Find(const wxString& text, unsigned int flags = 0);
    
    // clipboard
    bool CanCutSelection();
    bool CanCopySelection();
    bool CanCopyLinkLocation();
    bool CanCopyImageLocation();
    bool CanCopyImageContents();
    bool CanPaste();
    void CutSelection();
    void CopySelection();
    void CopyLinkLocation();
    void CopyImageLocation();
    void CopyImageContents();
    void Paste();
    void SelectAll();
    void SelectNone();
    
    // other
    wxImage GetFavIcon() const;
    wxDOMDocument GetDOMDocument();
    
private:

    void OnSize(wxSizeEvent& evt);
    void OnSetFocus(wxFocusEvent& evt);
    void OnKillFocus(wxFocusEvent& evt);

    void ResetFavicon();
    void FetchFavIcon(void* uri);
    void OnFavIconFetched(const wxString& filename);
    void OnDOMContentLoaded();
    
private:

    EmbeddingPtrs* m_ptrs;
    BrowserChrome* m_chrome;
    bool m_ok;
    wxWebContentHandlerPtrArray m_content_handlers;
    wxWebContentHandlerPtrArray m_to_delete;
    wxWebFavIconProgress* m_favicon_progress;
    
    wxImage m_favicon;
    bool m_favicon_fetched;
    bool m_content_loaded;
    
    DECLARE_EVENT_TABLE()
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebEvent class
///////////////////////////////////////////////////////////////////////////////


// (CLASS) wxWebEvent
// Category: Control
// Derives: wxNotifyEvent
// Description: Primary web browser event.  Encapsulates information about
//     web control state changes, status changes, and other web-related events.
// Remarks: The wxWebEvent class represents a web browser event.  It 
//     encapsulates information about web control state changes, status changes, 
//     and other web-related events.

// (CONSTRUCTOR) wxWebEvent::wxWebEvent
// Description: Creates a new wxWebEvent object.
//
// Syntax: wxWebEvent::wxWebEvent(wxEventType command_type = wxEVT_NULL,
//                                int win_id = 0)
// Remarks: Creates a new wxWebEvent object.

// (METHOD) wxWebEvent::Clone
// Syntax: wxEvent *wxWebEvent::Clone() const

// (METHOD) wxWebEvent::GetState
// Syntax: int wxWebEvent::GetState()    
    
// (METHOD) wxWebEvent::SetState
// Syntax: void wxWebEvent::SetState(int value)    
    
// (METHOD) wxWebEvent::GetResult
// Syntax: int wxWebEvent::GetResult() const
        
// (METHOD) wxWebEvent::SetResult
// Syntax: void wxWebEvent::SetResult(int value)    
    
// (METHOD) wxWebEvent::SetHref
// Syntax: void wxWebEvent::SetHref(const wxString& value)
        
// (METHOD) wxWebEvent::GetHref
// Syntax: wxString wxWebEvent::GetHref() const    
    
// (METHOD) wxWebEvent::SetFilename
// Syntax: void wxWebEvent::SetFilename(const wxString& value)
        
// (METHOD) wxWebEvent::GetFilename
// Syntax: wxString wxWebEvent::GetFilename() const    
    
// (METHOD) wxWebEvent::SetContentType
// Syntax: void wxWebEvent::SetContentType(const wxString& value)
        
// (METHOD) wxWebEvent::GetContentType
// Syntax: wxString wxWebEvent::GetContentType() const    
    
// (METHOD) wxWebEvent::GetTargetNode
// Syntax: wxDOMNode wxWebEvent::GetTargetNode()
        
// (METHOD) wxWebEvent::GetDOMEvent
// Syntax: wxDOMEvent wxWebEvent::GetDOMEvent()    
    
// (METHOD) wxWebEvent::GetCreateChromeFlags
// Syntax: int wxWebEvent::GetCreateChromeFlags() const
        
// (METHOD) wxWebEvent::SetCreateChromeFlags
// Syntax: void wxWebEvent::SetCreateChromeFlags(int value)
        
// (METHOD) wxWebEvent::SetCreateBrowser
// Syntax: void wxWebEvent::SetCreateBrowser(wxWebControl* ctrl)    
    
// (METHOD) wxWebEvent::SetDownloadAction
// Syntax: void wxWebEvent::SetDownloadAction(int action)
        
// (METHOD) wxWebEvent::SetDownloadTarget
// Syntax: void wxWebEvent::SetDownloadTarget(const wxString& path)
        
// (METHOD) wxWebEvent::SetDownloadListener
// Syntax: void wxWebEvent::SetDownloadListener(wxWebProgressBase* listener)    
    
// (METHOD) wxWebEvent::SetShouldHandle
// Syntax: void wxWebEvent::SetShouldHandle(bool value)
        
// (METHOD) wxWebEvent::SetOutputContentType
// Syntax: void wxWebEvent::SetOutputContentType(const wxString& value)    

class WXDLLIMPEXP_AUI wxWebEvent : public wxNotifyEvent
{
public:
    wxWebEvent(wxEventType command_type = wxEVT_NULL,
                       int win_id = 0)
          : wxNotifyEvent(command_type, win_id)
    {
        m_state = wxWEB_STATE_NONE;
        m_res = wxWEB_RESULT_NONE;
        m_create_browser = NULL;
        m_create_chrome_flags = 0;
        m_download_action = 0;
        m_download_listener = NULL;
        m_should_handle = true;
    }
#ifndef SWIG
    wxWebEvent(const wxWebEvent& c) : wxNotifyEvent(c)
    {
        m_state = c.m_state;
        m_res = c.m_res;
        m_target_node = c.m_target_node;
        m_dom_event = c.m_dom_event;
        m_href = c.m_href;
        m_create_browser = c.m_create_browser;
        m_create_chrome_flags = c.m_create_chrome_flags;
        m_filename = c.m_filename;
        m_content_type = c.m_content_type;
        m_output_content_type = c.m_output_content_type;
        m_should_handle = c.m_should_handle;
        m_download_action = c.m_download_action;
        m_download_action_path = c.m_download_action_path;
        m_download_listener = c.m_download_listener;
    }
#endif

    wxEvent *Clone() const { return new wxWebEvent(*this); }

    int GetState() const { return m_state; }
    void SetState(int value) { m_state = value; }
    
    int GetResult() const { return m_res; }
    void SetResult(int value) { m_res = value; }
    
    void SetHref(const wxString& value) { m_href = value; }
    wxString GetHref() const { return m_href; }
    
    void SetFilename(const wxString& value) { m_filename = value; }
    wxString GetFilename() const { return m_filename; }
    
    void SetContentType(const wxString& value) { m_content_type = value; }
    wxString GetContentType() const { return m_content_type; }
    
    wxDOMNode GetTargetNode() { return m_target_node; }
    wxDOMEvent GetDOMEvent() { return m_dom_event; }
    
    int GetCreateChromeFlags() const { return m_create_chrome_flags; }
    void SetCreateChromeFlags(int value) { m_create_chrome_flags = value; }
    void SetCreateBrowser(wxWebControl* ctrl) { m_create_browser = ctrl; }
    
    void SetDownloadAction(int action) { m_download_action = action; }
    void SetDownloadTarget(const wxString& path) { m_download_action_path = path; }
    void SetDownloadListener(wxWebProgressBase* listener) { m_download_listener = listener; }
    
    void SetShouldHandle(bool value) { m_should_handle = value; }
    void SetOutputContentType(const wxString& value) { m_output_content_type = value; }
    
    int m_state;
    int m_res;
    wxDOMNode m_target_node;
    wxDOMEvent m_dom_event;
    wxString m_href;
    wxString m_filename;
    bool m_should_handle;
    wxString m_content_type;
    wxString m_output_content_type;
    
    int m_create_chrome_flags;
    wxWebControl* m_create_browser;
    
    int m_download_action;
    wxString m_download_action_path;
    wxWebProgressBase* m_download_listener;
    
#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxWebEvent)
#endif
};


BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_TITLECHANGE, 0)
    //DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_CONTENTTYPE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_OPENURI, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_LOCATIONCHANGE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_DOMCONTENTLOADED, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_STATECHANGE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_STATUSCHANGE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_STATUSTEXT, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_SHOWCONTEXTMENU, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_LEFTDOWN, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_MIDDLEDOWN, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_RIGHTDOWN, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_LEFTUP, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_MIDDLEUP, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_RIGHTUP, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_LEFTDCLICK, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_DRAGDROP, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_CREATEBROWSER, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_INITDOWNLOAD, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_SHOULDHANDLECONTENT, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_FAVICONAVAILABLE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_WEB_DOMEVENT, 0)
END_DECLARE_EVENT_TYPES()


typedef void (wxEvtHandler::*wxWebEventFunction)(wxWebEvent&);

#define wxWebEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxWebEventFunction, &func)

#define EVT_WEB_OPENURI(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_OPENURI, winid, wxWebEventHandler(func))
    
#define EVT_WEB_TITLECHANGE(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_TITLECHANGE, winid, wxWebEventHandler(func))

#define EVT_WEB_LOCATIONCHANGE(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_LOCATIONCHANGE, winid, wxWebEventHandler(func))

#define EVT_WEB_DOMCONTENTLOADED(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_DOMCONTENTLOADED, winid, wxWebEventHandler(func))

#define EVT_WEB_STATECHANGE(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_STATECHANGE, winid, wxWebEventHandler(func))

#define EVT_WEB_STATUSCHANGE(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_STATUSCHANGE, winid, wxWebEventHandler(func))
   
#define EVT_WEB_STATUSTEXT(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_STATUSTEXT, winid, wxWebEventHandler(func))

#define EVT_WEB_SHOWCONTEXTMENU(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_SHOWCONTEXTMENU, winid, wxWebEventHandler(func))
   
#define EVT_WEB_LEFTDOWN(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_LEFTDOWN, winid, wxWebEventHandler(func))
   
#define EVT_WEB_MIDDLEDOWN(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_MIDDLEDOWN, winid, wxWebEventHandler(func))
   
#define EVT_WEB_RIGHTDOWN(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_RIGHTDOWN, winid, wxWebEventHandler(func))

#define EVT_WEB_LEFTUP(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_LEFTUP, winid, wxWebEventHandler(func))
   
#define EVT_WEB_MIDDLEUP(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_MIDDLEUP, winid, wxWebEventHandler(func))
   
#define EVT_WEB_RIGHTUP(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_RIGHTUP, winid, wxWebEventHandler(func))

#define EVT_WEB_LEFTDCLICK(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_LEFTDCLICK, winid, wxWebEventHandler(func))
   
#define EVT_WEB_DRAGDROP(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_DRAGDROP, winid, wxWebEventHandler(func))
   
#define EVT_WEB_CREATEBROWSER(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_CREATEBROWSER, winid, wxWebEventHandler(func))

#define EVT_WEB_INITDOWNLOAD(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_INITDOWNLOAD, winid, wxWebEventHandler(func))

#define EVT_WEB_SHOULDHANDLECONTENT(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_SHOULDHANDLECONTENT, winid, wxWebEventHandler(func))

#define EVT_WEB_FAVICONAVAILABLE(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_FAVICONAVAILABLE, winid, wxWebEventHandler(func))

#define EVT_WEB_DOMEVENT(winid, func) \
   wx__DECLARE_EVT1(wxEVT_WEB_DOMEVENT, winid, wxWebEventHandler(func))


#endif

