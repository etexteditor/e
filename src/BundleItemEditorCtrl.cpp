#include "BundleItemEditorCtrl.h"
#include "IUpdatePanel.h"
#include "jsonreader.h"
#include "tm_syntaxhandler.h"
#include <wx/tokenzr.h>
#include <wx/regex.h>

#include "EditorFrame.h" // For ShowOutput() method; refator after other bundle-specific editing is pulled up to this class.

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

bool BundleItemEditorCtrl::SaveText(bool WXUNUSED(askforpath)) {
	return SaveBundleItem();
}

void BundleItemEditorCtrl::SetPath(const wxString& WXUNUSED(newpath)) {
	// Bundles items can't have their path set.
}

bool BundleItemEditorCtrl::SaveBundleItem() {
	wxASSERT(IsBundleItem());

	unsigned int bundleId;
	unsigned int itemId;

	// Check if bundle item still exists (may have been deleted by a bundle update)
	PListHandler& plistHandler = m_syntaxHandler.GetPListHandler();
	if (!plistHandler.GetBundleItemFromUri(m_remotePath, m_bundleType, bundleId, itemId)) {
		wxMessageBox(_("Bundle item has been deleted"), _("Could not save bundle item!"), wxICON_ERROR|wxOK, this);
		return false;
	}

	// Only save if there are changes
	if (!IsModified()) return true;

	PListDict itemDict = plistHandler.GetEditable(m_bundleType, bundleId, itemId);
	if (!itemDict.IsOk()) return false;

	cxLOCKDOC_READ(m_doc)
		// Set new bundle contents
		switch (m_bundleType) {
			case BUNDLE_COMMAND:
			case BUNDLE_DRAGCMD:
				{
					// runEnvironment and contents
					wxString runEnv;
					doc.GetProperty(wxT("bundle:runEnvironment"), runEnv);
					
					if (runEnv == wxT("windows")) itemDict.SetString("runEnvironment", "windows");
					else itemDict.DeleteItem("runEnvironment");

					if (runEnv.empty()) itemDict.DeleteItem("winCommand");
					const char* co = runEnv.empty() ? "command" : "winCommand";
					if (doc.GetLength()) {
						vector<char> buffer;
						doc.GetTextPart(0, doc.GetLength(), buffer);
						buffer.push_back('\0');
						itemDict.SetString(co, &*buffer.begin());
					}
					else itemDict.SetString(co, "");

					// Get Properties
					if (m_bundleType == BUNDLE_COMMAND) {
						wxString beforeRunningCommand;
						wxString input;
						wxString fallbackInput;
						wxString output;
						wxString keyEquivalent;
						wxString tabTrigger;
						doc.GetProperty(wxT("bundle:beforeRunningCommand"), beforeRunningCommand);
						doc.GetProperty(wxT("bundle:input"), input);
						doc.GetProperty(wxT("bundle:fallbackInput"), fallbackInput);
						doc.GetProperty(wxT("bundle:output"), output);
						doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
						doc.GetProperty(wxT("bundle:tabTrigger"), tabTrigger);

						if (!beforeRunningCommand.empty()) itemDict.wxSetString("beforeRunningCommand", beforeRunningCommand);
						if (!output.empty()) itemDict.wxSetString("input", input);
						if (!output.empty()) itemDict.wxSetString("output", output);
						if (!keyEquivalent.empty()) itemDict.wxSetString("keyEquivalent", keyEquivalent); else itemDict.DeleteItem("keyEquivalent");
						if (!tabTrigger.empty()) itemDict.wxSetString("tabTrigger", tabTrigger); else itemDict.DeleteItem("tabTrigger");
						if (input == wxT("selection") && !fallbackInput.empty()) itemDict.wxSetString("fallbackInput", fallbackInput); else itemDict.DeleteItem("fallbackInput");
					}
					else { // BUNDLE_DRAGCMD
						PListArray extArray = itemDict.NewArray("draggedFileExtensions");
						extArray.Clear();

						wxString exts;
						doc.GetProperty(wxT("bundle:draggedFileExtensions"), exts);
						wxStringTokenizer tokens(exts, wxT(" ,"), wxTOKEN_STRTOK);

						for (wxString tok = tokens.GetNextToken(); !tok.empty(); tok = tokens.GetNextToken()) {
							extArray.AddString(tok.mb_str(wxConvUTF8));
						}
					}
					
					wxString scope;
					doc.GetProperty(wxT("bundle:scope"), scope);
					itemDict.wxSetString("scope", scope);
				}
				break;

			case BUNDLE_SNIPPET:
				{
					// contents
					if (doc.GetLength()) {
						vector<char> buffer;
						doc.GetTextPart(0, doc.GetLength(), buffer);
						buffer.push_back('\0');
						itemDict.SetString("content", &*buffer.begin());
					}
					else itemDict.SetString("content", "");

					wxString keyEquivalent;
					wxString tabTrigger;
					wxString scope;
					doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
					doc.GetProperty(wxT("bundle:tabTrigger"), tabTrigger);
					doc.GetProperty(wxT("bundle:scope"), scope);

					if (!keyEquivalent.empty()) itemDict.wxSetString("keyEquivalent", keyEquivalent); else itemDict.DeleteItem("keyEquivalent");
					if (!tabTrigger.empty()) itemDict.wxSetString("tabTrigger", tabTrigger); else itemDict.DeleteItem("tabTrigger");
					itemDict.wxSetString("scope", scope);
				}
				break;

			case BUNDLE_PREF:
			case BUNDLE_LANGUAGE:
				{
					const wxString jsonStr = doc.GetText();

					// Parse the JSON string
					wxJSONReader reader;
					wxJSONValue root;
					const int numErrors = reader.Parse(jsonStr, &root);
					if ( numErrors > 0 )  {
						// if there are errors in the JSON document, print the errors 
						wxString msg = _("<h2>Invalid JSON syntax:</h2>\n");
						const wxArrayString& errors = reader.GetErrors();
						wxRegEx reLineCol(wxT("line ([[:digit:]]+), col ([[:digit:]]+)"));
						for ( int i = 0; i < numErrors; i++ )  {
							if (i) msg += wxT("<br>\n");

							const wxString& error = errors[i];
							if (reLineCol.Matches(error)) {
								size_t matchStart;
								size_t matchLen;
								reLineCol.GetMatch(&matchStart, &matchLen);
								const wxString line = reLineCol.GetMatch(error, 1);
								const wxString col = reLineCol.GetMatch(error, 2);
								
								msg += error.substr(0, matchStart);
								msg += wxT("<a href=\"txmt://open?") + m_remotePath + wxT("&line=") + line + wxT("&column=") + col;
								msg += wxT("\">") + error.substr(matchStart, matchLen) + wxT("</a>") + error.substr(matchStart+matchLen);
							}
							else msg += error;
						}
						m_parentFrame.ShowOutput(_("Syntax error"), msg);
						return false;
					}

					if (m_bundleType == BUNDLE_PREF) {
						PListDict settingsDict;
						if (!itemDict.GetDict("settings", settingsDict)) settingsDict = itemDict.NewDict("settings");

						settingsDict.SetJSON(root);

						wxString scope;
						doc.GetProperty(wxT("bundle:scope"), scope);
						itemDict.wxSetString("scope", scope);
					}
					else { // BUNDLE_LANGUAGE
						itemDict.SetJSON(root, true /*strip*/);

						wxString keyEquivalent;
						doc.GetProperty(wxT("bundle:keyEquivalent"), keyEquivalent);
						if (!keyEquivalent.empty()) itemDict.wxSetString("keyEquivalent", keyEquivalent); else itemDict.DeleteItem("keyEquivalent");
					}
				}
				break;

			default: wxASSERT(false);
		}
	cxENDLOCK

	// Save to plist
	if (!plistHandler.Save(m_bundleType, bundleId, itemId)) return false;

	// Update mirror
	const wxDateTime modDate = itemDict.GetModDate();
	const doc_id di = GetDocID();
	cxLOCK_WRITE(m_catalyst)
		catalyst.SetFileMirror(m_remotePath, di, modDate);
	cxENDLOCK

	// Reload bundles
	wxBusyCursor wait; // Show user that we are reloading
	if (m_bundleType == BUNDLE_PREF || m_bundleType == BUNDLE_LANGUAGE) {
		// we have to call LoadBundles since all syntaxes will have to be reloaded
		m_syntaxHandler.LoadBundles(TmSyntaxHandler::cxRELOAD);
	}
	else m_syntaxHandler.ReParseBundles();

	return true;
}

bool BundleItemEditorCtrl::LoadBundleItem(const wxString& bundleUri) {
	// Get information about bundle item
	unsigned int bundleId;
	unsigned int itemId;
	const PListHandler& plistHandler = m_syntaxHandler.GetPListHandler();
	if (!plistHandler.GetBundleItemFromUri(bundleUri, m_bundleType, bundleId, itemId)) return false;
	const PListDict itemDict = plistHandler.Get(m_bundleType, bundleId, itemId);

	// Invalidate all stylers
	StylersInvalidate();

	// Check if we already have a mirror
	bool doReload = true;
	doc_id di;
	wxDateTime modDate;
	bool hasMirror;
	cxLOCK_READ(m_catalyst)
		hasMirror = catalyst.GetFileMirror(bundleUri, di, modDate);
	cxENDLOCK
	if (hasMirror) {
		if (modDate == itemDict.GetModDate()) { // item unchanged since mirror?
			if (di == GetDocID()) return true; // already loaded
			doReload = false;
		}
		SetDocument(di, bundleUri);
	}

	if (doReload) {
		// Set item name and encoding
		cxLOCKDOC_WRITE(m_doc)
			if (doc.IsEmpty()) doc.Clear(true); // make this initial rev
			doc.DeleteAll();
			doc.SetPropertyName(itemDict.wxGetString("name"));
			doc.SetPropertyEncoding(wxFONTENCODING_UTF8); // bundle contents is always utf8
			doc.SetPropertyEOL(wxTextFileType_Unix);
			doc.SetProperty(wxT("bundle:uuid"), itemDict.wxGetString("uuid"));
		cxENDLOCK

		// Load the item contents
		switch (m_bundleType) {
			case BUNDLE_SNIPPET:
				{
					const char* co = itemDict.GetString("content");
					cxLOCKDOC_WRITE(m_doc)
						if (co) doc.Insert(0, co);

						// Set properties
						doc.SetProperty(wxT("bundle:keyEquivalent"), itemDict.wxGetString("keyEquivalent"));
						doc.SetProperty(wxT("bundle:tabTrigger"), itemDict.wxGetString("tabTrigger"));
						doc.SetProperty(wxT("bundle:scope"), itemDict.wxGetString("scope"));
					cxENDLOCK
					m_lines.ReLoadText();
				}
				break;

			case BUNDLE_COMMAND:
			case BUNDLE_DRAGCMD:
				{
					bool winCommand = true;
					const char* co = itemDict.GetString("winCommand");
					if (!co) {
						co = itemDict.GetString("command");
						winCommand = false;
					}
					if (co) {
						cxLOCKDOC_WRITE(m_doc)
							doc.Insert(0, co);

							// Set properties (for historical reasons runEnvironment is a bit cumbersome)
							const wxString runEnvironment = itemDict.wxGetString("runEnvironment");
							if (!runEnvironment.empty()) doc.SetProperty(wxT("bundle:runEnvironment"), runEnvironment);
							else if (winCommand) doc.SetProperty(wxT("bundle:runEnvironment"), wxT("cygwin"));

							if (m_bundleType == BUNDLE_COMMAND) {
								doc.SetProperty(wxT("bundle:beforeRunningCommand"), itemDict.wxGetString("beforeRunningCommand"));
								doc.SetProperty(wxT("bundle:input"), itemDict.wxGetString("input"));
								doc.SetProperty(wxT("bundle:fallbackInput"), itemDict.wxGetString("fallbackInput"));
								doc.SetProperty(wxT("bundle:output"), itemDict.wxGetString("output"));

								doc.SetProperty(wxT("bundle:keyEquivalent"), itemDict.wxGetString("keyEquivalent"));
								doc.SetProperty(wxT("bundle:tabTrigger"), itemDict.wxGetString("tabTrigger"));
							}
							else { // m_bundleType == BUNDLE_DRAGCMD
								// Get file extensions
								PListArray extArray;
								if (itemDict.GetArray("draggedFileExtensions", extArray)) {
									wxString fileTypes;
									for (unsigned int i = 0; i < extArray.GetSize(); ++i) {
										if (i) fileTypes += wxT(", ");
										fileTypes += extArray.wxGetString(i);
									}
									doc.SetProperty(wxT("bundle:draggedFileExtensions"), fileTypes);
								}
								else doc.DeleteProperty(wxT("bundle:draggedFileExtensions"));
							}

							doc.SetProperty(wxT("bundle:scope"), itemDict.wxGetString("scope"));
						cxENDLOCK
						m_lines.ReLoadText();
					}
				}
				break;

			case BUNDLE_LANGUAGE:
				{
					const wxString jsonSettings = itemDict.GetJSON(true /*strip*/);
					cxLOCKDOC_WRITE(m_doc)
						doc.Insert(0, jsonSettings);

						// Set properties
						doc.SetProperty(wxT("bundle:keyEquivalent"), itemDict.wxGetString("keyEquivalent"));
					cxENDLOCK
					m_lines.ReLoadText();

					m_syntaxstyler.SetSyntax(wxT("JSON"));
				}
				break;

			case BUNDLE_PREF:
				{
					wxString jsonSettings = wxT("{ }"); // default
					PListDict settingsDict;
					if (itemDict.GetDict("settings", settingsDict)) {
						jsonSettings = settingsDict.GetJSON();
					}
					
					cxLOCKDOC_WRITE(m_doc)
						doc.Insert(0, jsonSettings);

						// Set properties
						doc.SetProperty(wxT("bundle:scope"), itemDict.wxGetString("scope"));
					cxENDLOCK
					m_lines.ReLoadText();

					m_syntaxstyler.SetSyntax(wxT("JSON"));
				}
				break;

			default:
				wxASSERT(false);
		}
		Freeze();

		// Set mirror
		modDate = itemDict.GetModDate();
		di = GetDocID();
		cxLOCK_WRITE(m_catalyst)
			catalyst.SetFileMirror(bundleUri, di, modDate);
		cxENDLOCK
	}

	m_remotePath = bundleUri;

	// AdamV: Can we fold these items up to the above switch?
	// Set initial syntax
	switch (m_bundleType) {
		case BUNDLE_SNIPPET:
			m_syntaxstyler.SetSyntax(wxT("Snippet"));
			break;
		case BUNDLE_COMMAND:
		case BUNDLE_DRAGCMD:
			if (!m_syntaxstyler.UpdateSyntax()) {
				// Use MSDOS batch file for Windows commands, otherwise use Shell Script (Bash)
				m_syntaxstyler.SetSyntax(itemDict.HasKey("winCommand") 
					? wxT("MSDOS batch file") 
					:  wxT("Shell Script (Bash)"));
			}
			break;
		case BUNDLE_LANGUAGE:
		case BUNDLE_PREF:
			m_syntaxstyler.SetSyntax(wxT("JSON"));
			break;
		default: wxASSERT(false);
	}

	// Update the panel we're embedded in.
	m_bundlePanel->UpdatePanel();

	return true;
}
