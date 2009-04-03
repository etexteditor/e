#include <wx/wx.h>
#include "key_hook.h"

#ifdef __WXGTK__
//#define XK_MISCELLANY
//#include <X11/Xlib.h>
//#include <X11/keysymdef.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkmain.h>

#include "tm_syntaxhandler.h"
#endif

#ifdef __WXGTK__
// translate X keychar into WX 
static unsigned int TranslateKeySymToWXK(guint keysym)
{
	unsigned int key_code = 0;
	switch ( keysym )
	{
	case GDK_Shift_L:
	case GDK_Shift_R:
	    key_code = WXK_SHIFT;
	    break;
	case GDK_Control_L:
	case GDK_Control_R:
	    key_code = WXK_CONTROL;
	    break;
	case GDK_Meta_L:
	case GDK_Meta_R:
	case GDK_Alt_L:
	case GDK_Alt_R:
	case GDK_Super_L:
	case GDK_Super_R:
	    key_code = WXK_ALT;
	    break;

	case GDK_Menu:
	    key_code = WXK_MENU;
	    break;

	case GDK_Help:
	    key_code = WXK_HELP;
	    break;

	case GDK_BackSpace:
	    key_code = WXK_BACK;
	    break;

	case GDK_ISO_Left_Tab:
	case GDK_Tab:
	    key_code = WXK_TAB;
	    break;

	case GDK_Linefeed:
	case GDK_Return:
	    key_code = WXK_RETURN;
	    break;

	case GDK_Print:
	    key_code = WXK_PRINT;
	    break;

	case GDK_Escape:
	    key_code = WXK_ESCAPE;
	    break;

	case GDK_Delete:
	    key_code = WXK_DELETE;
	    break;

	case GDK_Home:
	    key_code = WXK_HOME;
	    break;

	case GDK_Left:
	    key_code = WXK_LEFT;
	    break;

	case GDK_Up:
	    key_code = WXK_UP;
	    break;

	case GDK_Right:
	    key_code = WXK_RIGHT;
	    break;

	case GDK_Down:
	    key_code = WXK_DOWN;
	    break;

	case GDK_Prior:
	    key_code = WXK_PAGEUP;
	    break;

	case GDK_Next:
	    key_code = WXK_PAGEDOWN;
	    break;

	case GDK_End:
	    key_code = WXK_END;
	    break;

	case GDK_Begin:
	    key_code = WXK_HOME;
	    break;

	case GDK_Insert:
	    key_code = WXK_INSERT;
	    break;


	case GDK_KP_0:
	case GDK_KP_1:
	case GDK_KP_2:
	case GDK_KP_3:
	case GDK_KP_4:
	case GDK_KP_5:
	case GDK_KP_6:
	case GDK_KP_7:
	case GDK_KP_8:
	case GDK_KP_9:
	    key_code = WXK_NUMPAD0 + keysym - GDK_KP_0;
	    break;

	case GDK_KP_Space:
	    key_code = WXK_NUMPAD_SPACE;
	    break;

	case GDK_KP_Tab:
	    key_code = WXK_NUMPAD_TAB;
	    break;

	case GDK_KP_Enter:
	    key_code = WXK_NUMPAD_ENTER;
	    break;

	case GDK_KP_Home:
	    key_code = WXK_NUMPAD_HOME;
	    break;

	case GDK_KP_Left:
	    key_code = WXK_NUMPAD_LEFT;
	    break;

	case GDK_KP_Up:
	    key_code = WXK_NUMPAD_UP;
	    break;

	case GDK_KP_Right:
	    key_code = WXK_NUMPAD_RIGHT;
	    break;

	case GDK_KP_Down:
	    key_code = WXK_NUMPAD_DOWN;
	    break;

	case GDK_KP_Prior: // == GDK_KP_Page_Up
	    key_code = WXK_NUMPAD_PAGEUP;
	    break;

	case GDK_KP_Next: // == GDK_KP_Page_Down
	    key_code = WXK_NUMPAD_PAGEDOWN;
	    break;

	case GDK_KP_End:
	    key_code = WXK_NUMPAD_END;
	    break;

	case GDK_KP_Begin:
	    key_code = WXK_NUMPAD_BEGIN;
	    break;

	case GDK_KP_Insert:
	    key_code = WXK_NUMPAD_INSERT;
	    break;

	case GDK_KP_Delete:
	    key_code = WXK_NUMPAD_DELETE;
	    break;

	case GDK_KP_Equal:
	    key_code = WXK_NUMPAD_EQUAL;
	    break;

	case GDK_KP_Multiply:
	    key_code = WXK_NUMPAD_MULTIPLY;
	    break;

	case GDK_KP_Add:
	    key_code = WXK_NUMPAD_ADD;
	    break;

	case GDK_KP_Separator:
	    key_code = WXK_NUMPAD_SEPARATOR;
	    break;

	case GDK_KP_Subtract:
	    key_code = WXK_NUMPAD_SUBTRACT;
	    break;

	case GDK_KP_Decimal:
	    key_code = WXK_NUMPAD_DECIMAL;
	    break;

	case GDK_KP_Divide:
	    key_code = WXK_NUMPAD_DIVIDE;
	    break;


	case GDK_F1:
	case GDK_F2:
	case GDK_F3:
	case GDK_F4:
	case GDK_F5:
	case GDK_F6:
	case GDK_F7:
	case GDK_F8:
	case GDK_F9:
	case GDK_F10:
	case GDK_F11:
	case GDK_F12:
	    key_code = WXK_F1 + keysym - GDK_F1;
	    break;

	default: { // probably a char...
            wxChar uni = keysym;
            if (wxIsprint(uni) && tmKey::wxkToUni(uni, false, uni)) // remove register shift
            {
                key_code = uni;
            }
		}
	}

	wxLogDebug(wxT("keycode is 0x%0x\n"), key_code);
	return key_code; 
}

static void TranslateEventToWxKeyEvent(GdkEventKey* xevent, wxKeyEvent& wx_event)
{
	wx_event.m_shiftDown = (xevent->state & GDK_SHIFT_MASK) != 0;
	wx_event.m_controlDown = (xevent->state & GDK_CONTROL_MASK) != 0;
	wx_event.m_altDown = (xevent->state & GDK_MOD1_MASK) != 0;
	wx_event.m_metaDown = (xevent->state & GDK_MOD2_MASK) != 0; 
	wx_event.m_keyCode = TranslateKeySymToWXK(xevent->keyval);
}

KeyHooks::binds_map KeyHooks::binds;
RecursiveCriticalSection KeyHooks::critsect;
bool KeyHooks::hooked = false;

gboolean KeyHooks::key_snooper (GtkWidget *widget, GdkEventKey *gdk_kevent, gpointer WXUNUSED(data))
{
	if (!GTK_IS_WIDGET(widget)) return FALSE;

	GtkWidget* window = gtk_widget_get_toplevel(widget);
	if (!window || !GTK_IS_WINDOW(window)) return FALSE;
	widget = gtk_window_get_focus((GtkWindow*)window);
	if (!widget || !GTK_IS_WIDGET(widget)) return FALSE;

	wxKeyEvent event (gdk_kevent->type == GDK_KEY_PRESS ? wxEVT_KEY_DOWN : wxEVT_KEY_UP);
	TranslateEventToWxKeyEvent(gdk_kevent, event);
	if (event.m_keyCode) {
		RecursiveCriticalSectionLocker lock(critsect);
		GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
		do {
			if (GTK_WIDGET_IS_SENSITIVE(widget)) {
				WXWidget handle = widget;
				binds_map::const_iterator i = binds.find(handle);
				if (i != binds.end()) {
					return i->second->OnEvent(event) ? TRUE : FALSE;
				}
			}
			widget = widget->parent;
		} while(widget && gtk_widget_get_toplevel(widget) == toplevel);
	}
	return FALSE;
}
#endif
