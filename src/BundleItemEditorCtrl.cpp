#include "BundleItemEditorCtrl.h"

// Document Icons
#include "document.xpm"
#include "images/tmCommand.xpm"
#include "images/tmSnippet.xpm"
#include "images/tmDragCmd.xpm"
#include "images/tmPrefs.xpm"
#include "images/tmLanguage.xpm"

BundleItemEditorCtrl::BundleItemEditorCtrl(const int page_id, CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size):
	EditorCtrl(page_id, cw, bitmap,parent,parentFrame,pos,size){}

BundleItemEditorCtrl::BundleItemEditorCtrl(CatalystWrapper& cw, wxBitmap& bitmap, wxWindow* parent, EditorFrame& parentFrame, const wxPoint& pos, const wxSize& size):
	EditorCtrl(cw, bitmap, parent, parentFrame, pos, size){}

BundleItemEditorCtrl::~BundleItemEditorCtrl(){}

const char** BundleItemEditorCtrl::RecommendedIcon() {
	switch (m_bundleType) {
		case BUNDLE_COMMAND:
			return tmcommand_xpm;

		case BUNDLE_DRAGCMD:
			return tmdragcmd_xpm;

		case BUNDLE_SNIPPET:
			return tmsnippet_xpm;

		case BUNDLE_PREF:
			return tmprefs_xpm;

		case BUNDLE_LANGUAGE: 
			return tmlanguage_xpm;

		default: wxASSERT(false);
	}

	return document_xpm;
}

bool BundleItemEditorCtrl::SaveText(bool askforpath) {
	return SaveBundleItem(askforpath);
}
