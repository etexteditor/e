/*! \file iehtmlwin.h
    \brief implements wxIEHtmlWin window class
*/
#ifndef _IEHTMLWIN_H_
#define _IEHTMLWIN_H_

#if defined(_MSC_VER)
#pragma warning( disable : 4101 4786)
#pragma warning( disable : 4786)
#endif


#include <wx/setup.h>
#include <wx/wx.h>

// Memory leak checking is not compatible with STL.
// Should configure wxWindows without leak checking
// but this makes it compile at least.
#ifdef new
#undef new
#endif

#include <exdisp.h>

// STL can't compile with Level 4
#pragma warning(push, 1)
#include <iostream>
#pragma warning(pop)
using namespace std;

#include "wxactivex.h"
#include "IHtmlWnd.h"

class IStreamAdaptorBase;

class wxIEHtmlWin : public wxActiveX, public IHtmlWnd
{
public:
    wxIEHtmlWin(wxWindow * parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxNO_BORDER | wxWANTS_CHARS,
        const wxString& name = wxPanelNameStr);
	virtual ~wxIEHtmlWin();

	wxWindow* GetWindow();

	void LoadUrl(const wxString &_url, const wxString &_frame = wxEmptyString, bool keepHistory=false);
    bool LoadString(const wxString& html, bool prependHtml=true);
    bool LoadStream(istream *strm, bool prependHtml=true);
    bool LoadStream(wxInputStream *is, bool prependHtml=true);

	bool AppendString(const wxString& html);

	void SetCharset(wxString charset);
    void SetEditMode(bool seton);
    bool GetEditMode();
    wxString GetStringSelection(bool asHTML = false);
	wxString GetText(bool asHTML = false);
	wxString GetRealLocation();
	wxString GetTitle();

	bool GoBack();
	bool GoForward();
	bool GoHome();
	bool GoSearch();
	bool Refresh(wxHtmlRefreshLevel level);
	bool Stop();

	void RegisterAsDropTarget(bool _register);

	void FreezeRedraw(bool on);
protected:
    void SetupBrowser();
    bool LoadStream(IStreamAdaptorBase *pstrm, bool prependHtml=true);

	wxAutoOleInterface<IWebBrowser2>		m_webBrowser;
private:
	void OnMSHTMLBeforeNavigate2X(wxActiveXEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif /* _IEHTMLWIN_H_ */
