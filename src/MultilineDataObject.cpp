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

#include "MultilineDataObject.h"

const wxChar* MultilineDataObject::s_formatId = wxT("eMultiLineText");

MultilineDataObject::MultilineDataObject() {
	SetFormat(s_formatId);
}

void MultilineDataObject::AddText(const wxString& text) {
	if (!m_nullSeparatedText.empty()) m_nullSeparatedText += wxT('\0');
	m_nullSeparatedText += text;
}

void MultilineDataObject::GetText(wxArrayString& text) const {
	size_t strStart = 0;
	const size_t len = m_nullSeparatedText.length();
	for (size_t i = 0; i < len; ++i) {
		if (m_nullSeparatedText[i] == wxT('\0')) {
			const wxString str = m_nullSeparatedText.substr(strStart, i - strStart);
			text.Add(str);
			strStart = i + 1;
		}
	}
	if (strStart < len) text.Add(m_nullSeparatedText.substr(strStart));
}

size_t MultilineDataObject::GetDataSize() const {
	return m_nullSeparatedText.Len() * sizeof(wxChar);
}

bool MultilineDataObject::GetDataHere(void *buf) const {
	const size_t len = GetDataSize();

	memcpy( (char*)buf, (const char*)m_nullSeparatedText.c_str(), len );
	return true;
}

bool MultilineDataObject::SetData(size_t len, const void *buf) {
	const size_t charlen = len / sizeof(wxChar);
	m_nullSeparatedText = wxString((const wxChar*)buf, charlen);
	return true;
}

