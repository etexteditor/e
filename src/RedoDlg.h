/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __REDODLG_H__
#define __REDODLG_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

// Pre-definitions
class CatalystWrapper;
class doc_id;

class RedoDlg : public wxDialog {
public:
	RedoDlg(wxWindow *parent, CatalystWrapper& cw, int editorId, const doc_id& di);

private:
	// Event handlers
	void OnHistoryChar(wxKeyEvent& event);
	void OnHistoryDClick(wxCommandEvent& event);
};

#endif // __REDODLG_H__
