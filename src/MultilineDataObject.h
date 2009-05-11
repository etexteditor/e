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

#ifndef __MULTILINEDATAOBJECT_H__
#define __MULTILINEDATAOBJECT_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#ifdef __WXGTK__
   #include <wx/wx.h>
#endif

class MultilineDataObject : public wxDataObjectSimple {
public:
	MultilineDataObject();

	void AddText(const wxString& text);
	void GetText(wxArrayString& text) const;

	// implement base class pure virtuals
	virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void *buf) const;
    virtual bool SetData(size_t len, const void *buf);

	static const wxChar* FormatId;

private:
	wxString m_nullSeparatedText;
};

#endif //__MULTILINEDATAOBJECT_H__

