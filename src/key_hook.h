#ifndef __KEY_HOOK_H__
#define __KEY_HOOK_H__

#include "RecursiveCriticalSection.h"

#ifndef WX_PRECOMP
	#include <wx/event.h>
	#include <wx/log.h>
	#include <wx/textctrl.h>
#endif

#ifdef __WXGTK__
#define GSocket GlibGSocket
	#include <gtk/gtk.h>
//#include <gdk/gdkevents.h>
//#include <gdk/gdkwindow.h>
#undef GSocket
#endif

#include <map>

class _keyBindBase {
protected:
	_keyBindBase() {} // only available for subclasses
public:
	virtual ~_keyBindBase(){}
	virtual bool OnEvent(wxKeyEvent& WXUNUSED(event))=0;
};

#if defined(__WXGTK__)
class KeyHooks {
private:
	typedef std::map<WXWidget,_keyBindBase*> binds_map;
	static binds_map binds;
	static RecursiveCriticalSection critsect;
	static bool hooked;

	static gboolean key_snooper (GtkWidget *widget, GdkEventKey *event, gpointer data);


	static void Hook() {
		hooked = true;
		gtk_key_snooper_install (key_snooper, NULL );
	}
public:
	static void Register(_keyBindBase* bind, WXWidget handle) {
		RecursiveCriticalSectionLocker lock(critsect);
		if (!hooked) Hook();

wxLogDebug(wxT("registering 0x%x\n"), handle);
		binds.insert(binds_map::value_type(handle,bind));
	};
	static void Unregister(_keyBindBase* bind) {
		RecursiveCriticalSectionLocker lock(critsect);
		for(binds_map::iterator i = binds.begin(); i != binds.end(); i++)
			if (i->second == bind)
				binds.erase(i);
	};
};
#endif

template< class Base > class KeyHookable : public Base {
public:
	virtual bool ProcessEvent(wxEvent& event) // catch our widget creation and register binder
	{
		if (event.GetEventType() == wxEVT_CREATE)
		{
			wxWindow* wnd = ((wxWindowCreateEvent*)&event)->GetWindow();
			if (wnd == this)
				RegisterKeyHook();
		}
		return Base::ProcessEvent(event);
	}

protected:
	void RegisterKeyHook(void)
	{
		_key_binder.Register(this);
	}
	virtual bool OnPreKeyDown(wxKeyEvent& WXUNUSED(event)){return false;}
	virtual bool OnPreKeyUp(wxKeyEvent& WXUNUSED(event)){return false;}
#ifdef __WXMSW__
	virtual bool MSWTranslateMessage(WXMSG* pMsg) {
		if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN || 
			pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
		{
			// Don't allow input if we are in busy state, otherwise we might get recursive Yield in cxExecute
			if (wxIsBusy()) return true;

			if (_key_binder.OnMSWTranslateMessage(pMsg))
				return true;
		}
		return Base::MSWTranslateMessage(pMsg);
	}
#endif

private:
	class keyBind : public _keyBindBase {
	private:
		KeyHookable<Base> *delegate;
	public:
		keyBind() : delegate(NULL) {} // no default constructor
		void Register(KeyHookable<Base>* delegate) {
			if (!this->delegate) {
				this->delegate = delegate;
#ifdef __WXGTK__
				KeyHooks::Register(this, delegate->GetHandle());
#endif
			}
		}
		virtual ~keyBind() {
#ifdef __WXGTK__
			if (delegate != NULL)
				KeyHooks::Unregister(this);
#endif
		}
		virtual bool OnEvent(wxKeyEvent& event) {
			if (delegate)
			{
				if (event.GetEventType() == wxEVT_KEY_DOWN)    return delegate->OnPreKeyDown(event);
				else if (event.GetEventType() == wxEVT_KEY_UP) return delegate->OnPreKeyUp(event);
				wxFAIL;
			}
			return false;
		}
#ifdef __WXMSW__
		bool OnMSWTranslateMessage(WXMSG* pMsg) {
			if (!delegate) return false;

			int id = wxCharCodeMSWToWX(pMsg->wParam, pMsg->lParam);
			if ( !id ) id = pMsg->wParam; // normal ASCII char

			wxKeyEvent event = delegate->CreateKeyEvent(
				pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN ? wxEVT_KEY_DOWN : wxEVT_KEY_UP,
				id, pMsg->lParam, pMsg->wParam);

			return OnEvent(event);
		}
#endif
	};
friend class KeyHookable<Base>::keyBind;
	keyBind _key_binder;
};

#endif // __KEY_HOOK_H_

