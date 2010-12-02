#include "IEHtmlWin.h"
#include <wx/strconv.h>
#include <wx/string.h>
#include <wx/event.h>
#include <wx/listctrl.h>
#include <wx/mstream.h>
#include <oleidl.h>
#include <winerror.h>
#include <exdispid.h>
#include <exdisp.h>
#include <olectl.h>
#include <Mshtml.h>
#include <sstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
DEFINE_EVENT_TYPE(wxEVT_HTMLWND_BEFORE_LOAD)

BEGIN_EVENT_TABLE(wxIEHtmlWin, wxActiveX)
	EVT_ACTIVEX(wxID_ANY, "BeforeNavigate2", wxIEHtmlWin::OnMSHTMLBeforeNavigate2X)
END_EVENT_TABLE()


static const CLSID CLSID_MozillaBrowser =
{ 0x1339B54C, 0x3453, 0x11D2,
  { 0x93, 0xB9, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00 } };


//#define PROGID "Shell.Explorer"
#define PROGID CLSID_WebBrowser
//#define PROGID CLSID_MozillaBrowser
//#define PROGID CLSID_HTMLDocument
//#define PROGID "MSCAL.Calendar"
//#define PROGID "WordPad.Document.1"
//#define PROGID "SoftwareFX.ChartFX.20"

wxIEHtmlWin::wxIEHtmlWin(wxWindow * parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name) :
    wxActiveX(parent, PROGID, id, pos, size, style, name)
{
    SetupBrowser();
}

wxIEHtmlWin::~wxIEHtmlWin()
{
}

void wxIEHtmlWin::SetupBrowser()
{
	HRESULT hret;

	// Get IWebBrowser2 Interface
	hret = m_webBrowser.QueryInterface(IID_IWebBrowser2, m_ActiveX);
	assert(SUCCEEDED(hret));

	// web browser setup
	m_webBrowser->put_MenuBar(VARIANT_FALSE);
	m_webBrowser->put_AddressBar(VARIANT_FALSE);
	m_webBrowser->put_StatusBar(VARIANT_FALSE);
	m_webBrowser->put_ToolBar(VARIANT_FALSE);

	m_webBrowser->put_RegisterAsBrowser(VARIANT_TRUE);
	m_webBrowser->put_RegisterAsDropTarget(VARIANT_TRUE);

    m_webBrowser->Navigate( L"about:blank", NULL, NULL, NULL, NULL );
}


void wxIEHtmlWin::SetEditMode(bool seton)
{
    m_bAmbientUserMode = ! seton;
    AmbientPropertyChanged(DISPID_AMBIENT_USERMODE);
};

bool wxIEHtmlWin::GetEditMode()
{
    return ! m_bAmbientUserMode;
};


void wxIEHtmlWin::SetCharset(wxString charset)
{
	// HTML Document ?
	IDispatch *pDisp = NULL;
	m_webBrowser->get_Document(&pDisp);
	wxAutoOleInterface<IDispatch> disp(pDisp);

	if (disp.Ok())
	{
		wxAutoOleInterface<IHTMLDocument2> doc(IID_IHTMLDocument2, disp);
		if (doc.Ok())
            doc->put_charset((BSTR) (const wchar_t *) charset.wc_str(wxConvUTF8));
			//doc->put_charset((BSTR) wxConvUTF8.cMB2WC(charset).data());
	};
};


class IStreamAdaptorBase : public IStream
{
private:
    DECLARE_OLE_UNKNOWN(IStreamAdaptorBase);

public:
	string prepend;

    IStreamAdaptorBase() {}
    virtual ~IStreamAdaptorBase() {}

	virtual int Read(char *buf, int cb) = 0;


    // ISequentialStream
    HRESULT STDMETHODCALLTYPE Read(void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbRead)
	{
		if (prepend.size() > 0)
		{
			int n = wxMin(prepend.size(), cb);
			prepend.copy((char *) pv, n);
			prepend = prepend.substr(n);
			if (pcbRead)
				*pcbRead = n;

			return S_OK;
		};

		int rc = Read((char *) pv, cb);
		if (pcbRead)
			*pcbRead = rc;

		return S_OK;
	};

    HRESULT STDMETHODCALLTYPE Write(const void __RPC_FAR *WXUNUSED(pv), ULONG WXUNUSED(cb), ULONG __RPC_FAR *WXUNUSED(pcbWritten)) {return E_NOTIMPL;}

    // IStream
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER WXUNUSED(dlibMove), DWORD WXUNUSED(dwOrigin), ULARGE_INTEGER __RPC_FAR *WXUNUSED(plibNewPosition)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER WXUNUSED(libNewSize)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE CopyTo(IStream __RPC_FAR *WXUNUSED(pstm), ULARGE_INTEGER WXUNUSED(cb), ULARGE_INTEGER __RPC_FAR *WXUNUSED(pcbRead), ULARGE_INTEGER __RPC_FAR *WXUNUSED(pcbWritten)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Commit(DWORD WXUNUSED(grfCommitFlags)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Revert(void) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER WXUNUSED(libOffset), ULARGE_INTEGER WXUNUSED(cb), DWORD WXUNUSED(dwLockType)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER WXUNUSED(libOffset), ULARGE_INTEGER WXUNUSED(cb), DWORD WXUNUSED(dwLockType)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Stat(STATSTG __RPC_FAR *WXUNUSED(pstatstg), DWORD WXUNUSED(grfStatFlag)) {return E_NOTIMPL;}
    HRESULT STDMETHODCALLTYPE Clone(IStream __RPC_FAR *__RPC_FAR *WXUNUSED(ppstm)) {return E_NOTIMPL;}
};

DEFINE_OLE_TABLE(IStreamAdaptorBase)
	OLE_IINTERFACE(IUnknown)
	OLE_IINTERFACE(ISequentialStream)
	OLE_IINTERFACE(IStream)
END_OLE_TABLE;

class IStreamAdaptor : public IStreamAdaptorBase
{
private:
    istream *m_is;

public:

    IStreamAdaptor(istream *is)	: IStreamAdaptorBase(), m_is(is)
    {
        wxASSERT(m_is != NULL);
    }
    ~IStreamAdaptor()
    {
        delete m_is;
    }

    int Read(char *buf, int cb)
	{
		m_is->read(buf, cb);
		return m_is->gcount();
	};
};

class IwxStreamAdaptor : public IStreamAdaptorBase
{
private:
    wxInputStream *m_is;

public:

    IwxStreamAdaptor(wxInputStream *is)	: IStreamAdaptorBase(), m_is(is)
    {
        wxASSERT(m_is != NULL);
    }
    ~IwxStreamAdaptor()
    {
        delete m_is;
    }

    // ISequentialStream
    int Read(char *buf, int cb)
	{
		m_is->Read(buf, cb);
		return m_is->LastRead();
	};
};

wxWindow* wxIEHtmlWin::GetWindow()
{
	return static_cast<wxWindow*>(this);
}

void wxIEHtmlWin::LoadUrl(const wxString &_url, const wxString &_frame, bool keepHistory)
{
	VARIANTARG navFlag, targetFrame, postData, headers;
	navFlag.vt = VT_EMPTY;
	navFlag.vt = VT_I2;
	navFlag.iVal = navNoReadFromCache | navNoWriteToCache;
	if (!keepHistory) navFlag.iVal |= navNoHistory;
	targetFrame.vt = VT_EMPTY;
	postData.vt = VT_EMPTY;
	headers.vt = VT_EMPTY;

	HRESULT hret = 0;

	m_webBrowser->put_Offline(VARIANT_FALSE);

	if(_frame.Length())
	{
		V_VT(&targetFrame) = VT_BSTR;
		V_BSTR(&targetFrame) = SysAllocString((const wchar_t *) _frame.wc_str(wxConvUTF8));
	}

	hret = m_webBrowser->Navigate((BSTR) (const wchar_t *) _url.wc_str(wxConvUTF8),
		&navFlag, &targetFrame, &postData, &headers);

	if(_frame.Length())
	{
		SysFreeString(V_BSTR(&targetFrame));
	}
};

class wxOwnedMemInputStream : public wxMemoryInputStream
{
public:
    char *m_data;

    wxOwnedMemInputStream(char *data, size_t len) :
        wxMemoryInputStream(data, len), m_data(data)
    {}
    ~wxOwnedMemInputStream()
    {
        free(m_data);
    }
};

bool  wxIEHtmlWin::LoadString(const wxString& html, bool prependHtml)
{
    const wxCharBuffer buf = html.utf8_str();
    char *data = NULL;
    size_t len = strlen(buf.data());
    data = (char *) malloc(len);
    memcpy(data, buf, len);
    return LoadStream(new wxOwnedMemInputStream(data, len), prependHtml);
};

bool wxIEHtmlWin::LoadStream(IStreamAdaptorBase *pstrm, bool prependHtml)
{
	// need to prepend this as poxy MSHTML will not recognise a HTML comment
	// as starting a html document and treats it as plain text
	// Does nayone know how to force it to html mode ?
	if(prependHtml) pstrm->prepend = "<html>";

	// strip leading whitespace as it can confuse MSHTML
	wxAutoOleInterface<IStream>	strm(pstrm);

    // Document Interface
    IDispatch *pDisp = NULL;
    m_webBrowser->get_Document(&pDisp);
	if (! pDisp)
		return false;
	wxAutoOleInterface<IDispatch> disp(pDisp);


	// get IPersistStreamInit
    wxAutoOleInterface<IPersistStreamInit>
		pPersistStreamInit(IID_IPersistStreamInit, disp);

    if (pPersistStreamInit.Ok())
    {
        HRESULT hr = pPersistStreamInit->InitNew();
        if (SUCCEEDED(hr))
            hr = pPersistStreamInit->Load(strm);

		return SUCCEEDED(hr);
    }
	else
	    return false;
};

bool  wxIEHtmlWin::LoadStream(istream *is, bool prependHtml)
{
	// wrap reference around stream
    IStreamAdaptor *pstrm = new IStreamAdaptor(is);
	pstrm->AddRef();

    return LoadStream(pstrm, prependHtml);
};

bool wxIEHtmlWin::LoadStream(wxInputStream *is, bool prependHtml)
{
	// wrap reference around stream
    IwxStreamAdaptor *pstrm = new IwxStreamAdaptor(is);
	pstrm->AddRef();

    return LoadStream(pstrm, prependHtml);
};


bool wxIEHtmlWin::GoBack()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoBack();
    return hret == S_OK;
}

bool wxIEHtmlWin::GoForward()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoForward();
    return hret == S_OK;
}

bool wxIEHtmlWin::GoHome()
{
    try
    {
        CallMethod(wxT("GoHome"));
        return true;
    }
    catch(exception&)
    {
        return false;
    };
}

bool wxIEHtmlWin::GoSearch()
{
    HRESULT hret = 0;
    hret = m_webBrowser->GoSearch();
    return hret == S_OK;
}

bool wxIEHtmlWin::Refresh(wxHtmlRefreshLevel level)
{
    VARIANTARG levelArg;
    HRESULT hret = 0;

    levelArg.vt = VT_I2;
    levelArg.iVal = level;
    hret = m_webBrowser->Refresh2(&levelArg);
    return hret == S_OK;
}

bool wxIEHtmlWin::Stop()
{
    HRESULT hret = 0;
    hret = m_webBrowser->Stop();
    return hret == S_OK;
}

void wxIEHtmlWin::RegisterAsDropTarget(bool _register)
{
	m_webBrowser->put_RegisterAsDropTarget(_register);
}

///////////////////////////////////////////////////////////////////////////////

static wxAutoOleInterface<IHTMLSelectionObject> GetSelObject(IOleObject *oleObject)
{
	// Query for IWebBrowser interface
    wxAutoOleInterface<IWebBrowser2> wb(IID_IWebBrowser2, oleObject);
    if (! wb.Ok())
    	return wxAutoOleInterface<IHTMLSelectionObject>();

	IDispatch *iDisp = NULL;
    HRESULT hr = wb->get_Document(&iDisp);
    if (hr != S_OK)
    	return wxAutoOleInterface<IHTMLSelectionObject>();

	// Query for Document Interface
    wxAutoOleInterface<IHTMLDocument2> hd(IID_IHTMLDocument2, iDisp);
    iDisp->Release();

    if (! hd.Ok())
    	return wxAutoOleInterface<IHTMLSelectionObject>();

    IHTMLSelectionObject *_so = NULL;
    hr = hd->get_selection(&_so);

    // take ownership of selection object
	wxAutoOleInterface<IHTMLSelectionObject> so(_so);

    return so;
};

static wxAutoOleInterface<IHTMLTxtRange> GetSelRange(IOleObject *oleObject)
{
	wxAutoOleInterface<IHTMLTxtRange> tr;

    wxAutoOleInterface<IHTMLSelectionObject> so(GetSelObject(oleObject));
    if (! so)
    	return tr;

	IDispatch *iDisp = NULL;
    HRESULT hr = so->createRange(&iDisp);
    if (hr != S_OK)
    	return tr;

	// Query for IHTMLTxtRange interface
	tr.QueryInterface(IID_IHTMLTxtRange, iDisp);
    iDisp->Release();
    return tr;
};


wxString wxIEHtmlWin::GetStringSelection(bool asHTML)
{
	wxAutoOleInterface<IHTMLTxtRange> tr(GetSelRange(m_oleObject));
    if (! tr)
    	return wxEmptyString;

    BSTR text = NULL;
    HRESULT hr = E_FAIL;

	if (asHTML)
		hr = tr->get_htmlText(&text);
	else
		hr = tr->get_text(&text);
    if (hr != S_OK)
    	return wxEmptyString;

    wxString s = text;
    SysFreeString(text);

    return s;
};

wxString wxIEHtmlWin::GetText(bool asHTML)
{
	if (!m_webBrowser || !m_webBrowser.Ok())
		return wxEmptyString;

	// get document dispatch interface
	IDispatch *iDisp = NULL;
    HRESULT hr = m_webBrowser->get_Document(&iDisp);
    if (hr != S_OK)
    	return wxEmptyString;

	// Query for Document Interface
    wxAutoOleInterface<IHTMLDocument2> hd(IID_IHTMLDocument2, iDisp);
    iDisp->Release();

    if (! hd.Ok())
		return wxEmptyString;

	// get body element
	IHTMLElement *_body = NULL;
	hd->get_body(&_body);
	if (! _body)
		return wxEmptyString;
	wxAutoOleInterface<IHTMLElement> body(_body);

	// get inner text
    BSTR text = NULL;
    hr = E_FAIL;

	if (asHTML)
		hr = body->get_innerHTML(&text);
	else
		hr = body->get_innerText(&text);
    if (hr != S_OK)
    	return wxEmptyString;

    wxString s = text;
    SysFreeString(text);

    return s;
};

bool wxIEHtmlWin::AppendString(const wxString& html) {
	if (!m_webBrowser || !m_webBrowser.Ok())
		return false;

	// get document dispatch interface
	IDispatch *iDisp = NULL;
    HRESULT hr = m_webBrowser->get_Document(&iDisp);
    if (hr != S_OK) return false;

	// Query for Document Interface
    wxAutoOleInterface<IHTMLDocument2> hd(IID_IHTMLDocument2, iDisp);
    iDisp->Release();

    if (! hd.Ok())
		return false;

	// get body element
	IHTMLElement *_body = NULL;
	hd->get_body(&_body);
	if (! _body)
		return false;
	wxAutoOleInterface<IHTMLElement> body(_body);

	// get inner text
    BSTR text = NULL;
    hr = E_FAIL;

	hr = body->get_innerHTML(&text);
    if (hr != S_OK) return false;

    wxString s = text;
    SysFreeString(text);

	// append the new text
	s += html;

	// update inner text
	hr = body->put_innerHTML((BSTR) (const wchar_t *) s.wc_str(wxConvUTF8));
    if (hr != S_OK) return false;

    return true;
}

wxString wxIEHtmlWin::GetRealLocation()
{
HRESULT res;
IDispatch *documentD = 0;
IHTMLDocument2 *document = 0;
IHTMLLocation *location = 0;
BSTR href;
wxString realLocation;

	if (!m_webBrowser || !m_webBrowser.Ok())
		return wxEmptyString;

	res = m_webBrowser->get_Document(&documentD);
	if(res != S_OK) return wxEmptyString;

	res = documentD->QueryInterface(IID_IHTMLDocument2,(void **)&document);
	if(res != S_OK) return wxEmptyString;

	res = document->get_location(&location);
	if(res != S_OK) return wxEmptyString;

	res = location->toString(&href);
	if(res != S_OK) return wxEmptyString;
	realLocation = href;

	res = location->get_href(&href);
	if(res != S_OK) return wxEmptyString;

	realLocation = href;

	return realLocation;
}

wxString wxIEHtmlWin::GetTitle()
{
HRESULT res;
IDispatch *documentD = 0;
IHTMLDocument2 *document = 0;
BSTR href = NULL;
wxString title;

	if (!m_webBrowser || !m_webBrowser.Ok())
		return wxEmptyString;

	res = m_webBrowser->get_Document(&documentD);
	if(res != S_OK) return wxEmptyString;

	res = documentD->QueryInterface(IID_IHTMLDocument2,(void **)&document);
	if(res != S_OK) return wxEmptyString;

	res = document->get_title(&href);
	if(res != S_OK) return wxEmptyString;

	title = href;
	SysFreeString(href);

	return title;
}

void wxIEHtmlWin::FreezeRedraw(bool on) {
HRESULT res;
long hIE;

	if (!m_webBrowser || !m_webBrowser.Ok())
		return;

	res = m_webBrowser->get_HWND(&hIE);
	if(res != S_OK) return;

	
	//if (on) ::LockWindowUpdate(NULL);
	//else ::LockWindowUpdate(GetHwnd());
	::SendMessage((HWND)hIE, WM_SETREDRAW, (WPARAM)on, 0);
}

void wxIEHtmlWin::OnMSHTMLBeforeNavigate2X(wxActiveXEvent& event) {
	IHtmlWndBeforeLoadEvent anEvent(GetWindow());
	anEvent.SetId(event.GetId());
	anEvent.SetURL(event[wxT("Url")]);

	GetWindow()->GetEventHandler()->ProcessEvent(anEvent);
	if (anEvent.IsCancelled())
		event[wxT("Cancel")] = true;
}

IMPLEMENT_DYNAMIC_CLASS(IHtmlWndBeforeLoadEvent, wxCommandEvent)
IHtmlWndBeforeLoadEvent::IHtmlWndBeforeLoadEvent(wxWindow* win)
{
	m_cancelled = false;
	SetEventType(wxEVT_HTMLWND_BEFORE_LOAD);
	SetEventObject(win);
}
