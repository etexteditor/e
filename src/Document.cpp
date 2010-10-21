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

#include "Document.h"
#include "DataList.h"
#include <wx/fontmap.h>
#include <wx/wfstream.h>
#include "doc_byte_iter.h"
#include "cx_pcre.h"
#include "Utf.h"
#include "eSettings.h"
#include "Strings.h"
#include "eDocumentPath.h"


Document::Document(const doc_id& di, CatalystWrapper cw):
	m_catalyst(cw.m_catalyst),
	dispatcher(cw.GetDispatcher()),
	m_textData(cw.m_catalyst),
	in_change(false),
	change_level(0),
	do_notify(true),
	do_notify_top(0),
	m_re(NULL),
	m_trackChanges(NULL)
{
	SetDocument(di);

	// Make sure we get notified if the document gets deleted
	dispatcher.SubscribeC(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);
}

Document::Document(CatalystWrapper cw):
	m_catalyst(cw.m_catalyst),
	dispatcher(cw.GetDispatcher()),
	m_docId(DRAFT,-1,-1),
	m_textData(cw.m_catalyst),
	in_change(false),
	change_level(0),
	do_notify(true),
	do_notify_top(0),
	m_re(NULL),
	m_trackChanges(NULL)
{
	// Make sure we get notified if the document gets deleted
	dispatcher.SubscribeC(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);
}

Document::~Document() {
	dispatcher.UnSubscribe(wxT("DOC_DELETED"), (CALL_BACK)OnDocDeleted, this);

	// Do standard clean-up
	if (IsOk()) Close();

	// Clean up stuff not handled by Close()
	if (m_re) free(m_re); // check if we have a cached regex
}


// - Public API ---

void Document::CreateNew(const ISettings& settings) {
	// Create the new document
	SetDocument(m_catalyst.NewDocument());

	// The user may have defined a default encoding
	SetDefaultsFromSettings(settings);
}

void Document::SetDefaultsFromSettings(const ISettings& settings) {
	// Check if we need to set eol property
	wxString eolStr;
	if (settings.GetSettingString(wxT("formatEol"), eolStr)) {
		wxTextFileType eol = wxTextBuffer::typeDefault;
		if (eolStr == wxT("crlf")) eol = wxTextFileType_Dos;
		else if (eolStr == wxT("lf")) eol = wxTextFileType_Unix;
		else if (eolStr == wxT("cr")) eol = wxTextFileType_Mac;
		SetPropertyEOL(eol);
	}

	// Check if we need to set encoding property
	wxString encStr;
	if (settings.GetSettingString(wxT("formatEncoding"), encStr)) {
		const wxFontEncoding enc = wxFontMapper::GetEncodingFromName(encStr);
		SetPropertyEncoding(enc);
	}

	// Check if we need to set bom property
	bool bom;
	if (settings.GetSettingBool(wxT("formatBom"), bom))
		SetPropertyBOM(bom);
}

bool Document::operator==(const doc_id& di) const {
	wxASSERT(IsOk());
	return m_docId == di;
}

bool Document::operator!=(const doc_id& di) const {
	wxASSERT(IsOk());
	return m_docId != di;
}

void Document::Close() {
	wxASSERT(m_docId.document_id != -1 && m_docId.version_id != -1); // Cannot close invalid document

	m_textData.Close();

	// Invalidate document
	m_docId.document_id = m_docId.version_id = -1;
}

bool Document::IsOk() const {
	bool bad_document = m_docId.document_id == -1 || m_docId.version_id == -1;
	return !bad_document;
}

bool Document::IsEmpty() const {
	wxASSERT(IsOk());

	// Document can have a single empty revision and still
	// be considered empty (ignore property changes)
	return (m_docId.type == DRAFT && vHistory.GetSize() <= 1 && GetLength() == 0); // && GetPropnode().node_id == -1);
}

void Document::Commit(const wxString& label, const wxString& desc) {
	wxASSERT(IsOk());
	wxASSERT(label.Find('\n') == -1); // label should not contain newlines

	// If the document version is already committed we have to
	// create a new draft to start out with.
	if (m_docId.IsDocument()) NewRevision();

	// Check if the Draft has a parent
	c4_View vDocuments = m_catalyst.GetDocView();
	const doc_id parent = GetParent();

	// If it does not have a parent, we have to create a new document
	const int new_doc_id = parent.IsOk() ? parent.document_id : m_catalyst.CreateDocument();
	wxASSERT(new_doc_id != -1);
	c4_RowRef rDoc = vDocuments[new_doc_id];
	wxASSERT(pBuffPath(rDoc) != 0);

	c4_View vDocHistory = pHistory(rDoc);

	// Create new version
	const int new_version_id = vDocHistory.Add(c4_Row());
	c4_RowRef rVersion = vDocHistory[new_version_id];
	const doc_id new_doc(DOCUMENT, new_doc_id, new_version_id);

	// Add it as a child of the parent document
	// (root points to itself (rev_id=0) as parent)
	if (parent.IsOk()) {
		pParent(rVersion) = parent.version_id;

		// Add it to parents list of children
		c4_View vChildren = pChildren(vDocHistory[parent.version_id]);
		vChildren.Add(pType[DOCUMENT] + pChildref[new_version_id]);
	}

	// Get the Draft
	const c4_RowRef rDraft = vRevisions[m_docId.document_id];

	// Set Date (to same date as latest change to draft)
	pDate(rVersion) = pDate(rDraft);
	pTimezone(rVersion) = wxDateTime::TimeZone(wxDateTime::Local).GetOffset();

	// Set Label
	if (label.IsEmpty()) pLabelNode(rVersion) = -1;
	else {
		DataText labelText(m_catalyst, m_docId);
		labelText.Insert(0, label);
		const node_ref new_labelnode = labelText.Consolidate(new_doc);
		wxASSERT(new_labelnode.version_id == new_doc.version_id);

		pLabelNode(rVersion) = new_labelnode.node_id;
	}

	// Set Description
	if (desc.IsEmpty()) pDescNode(rVersion) = -1;
	else {
		DataText descText(m_catalyst, m_docId);
		descText.Insert(0, desc);
		const node_ref new_descnode = descText.Consolidate(new_doc);
		wxASSERT(new_descnode.version_id == new_doc.version_id);

		pDescNode(rVersion) = new_descnode.node_id;
	}

	// Consolidate properties
	const node_ref propnode = GetPropnode();
	if(propnode.IsOk()) {
		DataList properties(m_catalyst, m_docId, propnode);
		const node_ref new_propnode = properties.Consolidate(new_doc);

		pPropVer(rVersion) = new_propnode.version_id;
		pPropnode(rVersion) = new_propnode.node_id;
	}
	else {
		pPropVer(rVersion) = -1;
		pPropnode(rVersion) = -1;
	}

	// Consolidate data
	const node_ref headnode = m_textData.Consolidate(new_doc);
	pHeadVer(rVersion) = headnode.version_id;
	pHeadnode(rVersion) = headnode.node_id;

	// Set State and Length
	pState(rVersion) = cxREVSTATE_NORMAL;
	pLength(rVersion) = m_textData.GetLength();

	// Create the Document Signature
	m_catalyst.SignRevision(new_doc);

	// Move any file mirrors to point to the new document version
	wxArrayString mirrors;
	wxDateTime modifiedDate;
	doc_id mirrorId;
	if (m_catalyst.GetMirrorPathsForDraft(m_docId, mirrors)) {
		for (unsigned int i = 0; i < mirrors.GetCount(); ++i) {
			if (m_catalyst.GetFileMirror(mirrors[0], mirrorId, modifiedDate)) {
				if (mirrorId == m_docId) {
					m_catalyst.SetFileMirror(mirrors[i], new_doc, modifiedDate);
				}
				else {
					// If there are mirrors from intermediate revisions in the draft
					// we have to invalidate them.
					m_catalyst.SetFileMirrorToModified(mirrors[i], new_doc);
				}
			}
			else wxASSERT(false);
		}
	}

	// Delete the Draft
	m_catalyst.DeleteDraft(m_docId);

	const doc_id old_doc = m_docId;
	SetDocument(new_doc);
	MakeHead();

	// Notify that we have commited the document
	// so that other who might have a version of the draft open
	// get a chance to upgrade to the new doc.
	const docid_pair docPair(old_doc, new_doc);
	if (do_notify) {
		m_catalyst.UnLock();
			dispatcher.Notify(wxT("DOC_COMMITED"), &docPair, 0);
		m_catalyst.ReLock();
	}
}

wxString Document::GetText() const {
	wxASSERT(IsOk());
	m_catalyst.ResetIdle();

	if (GetLength() == 0) return wxString();
	return m_textData.GetText();
}

void Document::WriteText(wxOutputStream& stream) const {
	m_textData.WriteText(stream);
}

void Document::StartChange(bool doNotify) {
	wxASSERT(IsOk());

	// Always freeze before starting grouped changes
	if (change_level == 0) {
		Freeze();
		in_change = true;
	}

	// If notification is stopped at any nesting level
	// no lower level can enable it
	if (do_notify && !doNotify) {
		do_notify = false;
		do_notify_top = change_level;
	}

	++change_level;
}

void Document::EndChange(int forceTo) {
	wxASSERT(IsOk());
	wxASSERT(change_level > 0);
	wxASSERT(forceTo < change_level);

	// Change nesting level
	if (forceTo == -1) --change_level;
	else change_level = forceTo;

	// Restore notifications
	if (change_level <= do_notify_top) {
		do_notify = true;
		do_notify_top = 0;
	}

	// Freeze when the entire change is complete
	if (change_level == 0) {
		in_change = false;
		Freeze();

		// Notify subscribers that the revision has changed
		m_catalyst.UnLock();
			dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}
}

wxString Document::GetTextPart(int start_pos, int end_pos) const {
	wxASSERT(IsOk());
	m_catalyst.ResetIdle();
	wxString text;
	m_textData.GetTextPart(start_pos, end_pos, text);
	return text;
}

wxString Document::GetTextPart(const doc_id& version, int start_pos, int end_pos) const {
	wxASSERT(IsOk());
	wxASSERT(version.IsOk());

	const DataText textData(m_catalyst, version, GetHeadnode(version));
	m_catalyst.ResetIdle();

	wxString text;
	textData.GetTextPart(start_pos, end_pos, text);
	return text;
}

void Document::GetTextPart(int start_pos, int end_pos, vector<char>& buffer) const {
	wxASSERT(IsOk());
	m_catalyst.ResetIdle();

	const unsigned int len = end_pos - start_pos;
	if (len == 0) return;

	// Make sure that there is room for text in the buffer
	buffer.resize(len);

	// Get the text
	unsigned char* buffptr = (unsigned char*)&*buffer.begin();
	m_textData.GetTextPart(start_pos, end_pos, buffptr);
}

wxChar Document::GetChar(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetChar(pos);
}

unsigned int Document::GetLength() const {
	wxASSERT(IsOk());
	if (m_textData.IsOk()) return m_textData.GetLength();
	return 0;
}

unsigned int Document::GetLengthInChars(unsigned int start_pos, unsigned int end_pos) const {
	wxASSERT(IsOk());
	return m_textData.GetLengthInChars(start_pos, end_pos);
}

// This function can count both forward and backward, but if the count
// goes beyond the contents it returns the last valid pos.
unsigned int Document::GetCharPos(unsigned int offset, int char_count) const {
	wxASSERT(IsOk());
	return m_textData.GetCharPos(offset, char_count);
}

unsigned int Document::GetCharPos(const doc_id& version, unsigned int offset, int char_count) const {
	wxASSERT(IsOk());
	wxASSERT(m_catalyst.IsOk(version));

	const DataText textData(m_catalyst, m_docId, GetHeadnode(version));
	return textData.GetCharPos(offset, char_count);
}

unsigned int Document::GetCharLength(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetCharLength(pos);
}

unsigned int Document::GetNextCharPos(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetNextCharPos(pos);
}

unsigned int Document::GetPrevCharPos(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetPrevCharPos(pos);
}

unsigned int Document::GetValidCharPos(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetValidCharPos(pos);
}

unsigned int Document::GetLineStart(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetLineStart(pos);
}

unsigned int Document::GetLineEnd(unsigned int pos) const {
	wxASSERT(IsOk());
	return m_textData.GetLineEnd(pos);
}

void Document::Clear(bool initialRevision) {
	wxASSERT(IsOk());
	wxASSERT(m_docId.type == DRAFT); // Only Drafts can be cleared

	const c4_RowRef rDoc = vRevisions[m_docId.document_id];

	// Clear the entire document (removes all revisions)
	// (We cannot just clear the entire row as there might
	// be other revisions that have cached views)
	vNodes.RemoveAll();
	vHistory.RemoveAll();
	((c4_View)pFreenodes(rDoc)).RemoveAll();
	pBuffPath(rDoc) = 0;

	// Refresh references
	m_textData.Invalidate();

	if (initialRevision) NewRevision();
}

cxFileResult Document::LoadText(const wxFileName& path, vector<unsigned int>& offsets, wxFontEncoding enc, const wxString& mirror) {
	wxASSERT(IsOk());
	wxASSERT(path.IsOk());
	wxASSERT(path.IsAbsolute());
	wxASSERT(offsets.empty());

	// Clean up the cache first, so that we do not later risk
	// referencing an out-of-sync memorymap
	m_textData.ClearBuffCache();

	// Setup variables
	wxFileOffset len;
	MMapBuffer buffer;

	// Get the filename
	const wxString filename = mirror.empty() ? path.GetFullName() : mirror.AfterLast(wxT('/'));

	// Open the file & load the text
	wxDateTime fileDate;
	if (!path.FileExists()) len = 0; // fileDate will be invalid
	else {
		fileDate = path.GetModificationTime();

		// Use memory mapped files
		buffer.Open(path);
		if (!buffer.IsOpened()) return cxFILE_OPEN_ERROR; // Could not open file

		len = buffer.Length();
	}

	// Load the text
	if (len > 0)
    {
		if (!buffer.IsMapped()) return cxFILE_OPEN_ERROR; // Could not memory map file
    }
    else {
        // Empty file is ok
		// We want the new revision to either be the first (base) revision
		// or if there is a parent, directly following it.
		if (IsEmpty()) {
			Clear();
			NewRevision();
		}
		else DeleteAll();

		// Set the name
		SetPropertyName(filename);

		// Set the date
		if (fileDate.IsValid()) {
			pDate(vRevisions[m_docId.document_id]) = fileDate.GetValue().GetValue();
		}
		else {
			pDate(vRevisions[m_docId.document_id]) = wxDateTime::Now().GetValue().GetValue();
		}

		const ISettings& settings = eGetSettings();
		SetDefaultsFromSettings(settings);

		Freeze();

		// An invalid date indicates that the document is modified
		// or in this case does not yet exist.
		const wxString pathStr = mirror.empty() ? path.GetFullPath() : mirror;
		if (fileDate.IsValid()) m_catalyst.SetFileMirror(pathStr, m_docId, fileDate);
		else m_catalyst.SetFileMirrorToModified(pathStr, m_docId);

		// Notify that document mirror info has changed
		const doc_id di = m_docId;
		m_catalyst.UnLock();
			dispatcher.Notify(wxT("DOC_UPDATED"), (void*)&di, 0);
		m_catalyst.ReLock();

		return cxFILE_OK;
	}
	const char* bufptr = buffer.data();
	wxASSERT(bufptr);

	// See if we can detect the encoding
	unsigned int bom_len = 0;
	wxFontEncoding det_enc;
	const bool result = DetectTextEncoding(bufptr, len, det_enc, bom_len);
	if (result && enc == wxFONTENCODING_SYSTEM) {
		enc = det_enc; // Initial system means we should follow detection
	}
	if (enc == wxFONTENCODING_DEFAULT) enc = wxFONTENCODING_SYSTEM; // Set default to the system default

	// If there is a Byte Order Marker (BOM), we have to adjust offset & length
	if (bom_len) {
		bufptr += bom_len;
		len -= bom_len;
	}

	int buff_file_num = m_docId.IsDocument() ? 0 : pBuffPath(vRevisions[m_docId.document_id]);
	wxFileName buff_path = m_catalyst.GetDraftPath();
	if (buff_file_num == 0) { // Get a unique filename for buffer file
		do {
			buff_file_num = m_catalyst.GetRand();
			const wxString random_filename = wxString::Format(wxT("%x"), buff_file_num);
			buff_path.SetFullName(random_filename);
		} while (buff_file_num == 0 || buff_path.FileExists());
	}
	else buff_path.SetFullName(wxString::Format(wxT("%x"), buff_file_num));


	// Create the buffer file
	wxFFile bufferfile(buff_path.GetFullPath(), _("ab"));
	if (!bufferfile.IsOpened()) {
		wxASSERT_MSG(false, wxT("Could not open buffer file"));
		return cxFILE_OPEN_ERROR;
	}
	bufferfile.SeekEnd();
	wxFileOffset buff_offset = bufferfile.Tell();

	// Pre-reserve entries to avoid unneeded allocs in vector
	offsets.reserve(len/35);

	unsigned int pos = 0;
	unsigned int nl_count_dos = 0;
	unsigned int nl_count_mac = 0;
	unsigned int nl_count_unix = 0;

	if (enc == wxFONTENCODING_UTF8) {
		// Convert and save one line at a time while counting newlines
		char nl = '\n';
		bool prev_r = false;
		int linestart = 0;
		int utf_bytes = 0;

		for (off_t i = 0; i < len; ++i) {
			const char c = bufptr[i];

			// Detect invalid UTF-8 sequences
			if (utf_bytes == 0) {
				if ((c & 0xC0) == 0x80 || c == 0) goto decode_error;
				else {
					utf_bytes = utf8_len(c) - 1;
					if (utf_bytes > 3) goto decode_error;
				}
			}
			else if ((c & 0xC0) == 0x80) --utf_bytes;
			else goto decode_error;

			switch (c) {
			case '\r':
				if (prev_r) ++nl_count_mac;
				bufferfile.Write(&bufptr[linestart], i - linestart);
				pos += i - linestart;
				bufferfile.Write(&nl, 1); ++pos;
				offsets.push_back(pos);
				linestart = i+1;
				prev_r = true;
				break;

			case '\n':
				if (!prev_r) {
					++nl_count_unix;
					bufferfile.Write(&bufptr[linestart], (i - linestart)+1);
					pos += (i - linestart)+1;
					offsets.push_back(pos);
				} else ++nl_count_dos;
				linestart = i+1;
				prev_r = false;
				break;

			default:
				if (prev_r) ++nl_count_mac;
				prev_r = false;
			}
		}

		// Add text after last newline
		if (linestart < len) {
			bufferfile.Write(&bufptr[linestart], len - linestart);
			pos += len - linestart;
			offsets.push_back(pos);
		}
	}
	else {
		// Allocate buffers
		size_t temp_buff_len = 128;
		wxCharBuffer temp_buff(temp_buff_len);
		size_t wchar_buff_len = 128;
		wxWCharBuffer wchar_buff(wchar_buff_len);
		size_t utf8_buff_len = 128;
		wxCharBuffer utf8_buff(utf8_buff_len);

		// Initialize variables
		wxCSConv conv(enc);
		unsigned int char_len;
		const char* nulls = "\0\0\0\0";
		const char* nl;
		const char* cr;
		switch (enc) {
		case wxFONTENCODING_UTF32BE:
			char_len = 4;
			nl = "\x00\x00\x00\x0A";
			cr = "\x00\x00\x00\x0D";
			break;
		case wxFONTENCODING_UTF32LE:
			char_len = 4;
			nl = "\x0A\x00\x00\x00";
			cr = "\x0D\x00\x00\x00";
			break;
		case wxFONTENCODING_UTF16BE:
			char_len = 2;
			nl = "\x00\x0A";
			cr = "\x00\x0D";
			break;
		case wxFONTENCODING_UTF16LE:
			char_len = 2;
			nl = "\x0A\x00";
			cr = "\x0D\x00";
			break;
		default:
			char_len = 1;
			nl = "\n";
			cr = "\r";
		}

		// Convert and save one line at a time while counting newlines
		bool prev_r = false;
		int linestart = 0;
		for (off_t i = 0; i < len; i += char_len) {
			if (memcmp(cr, &bufptr[i], char_len) == 0)  { // '\r'
				if (prev_r) ++nl_count_mac;
				if (i - linestart > 0) {
					size_t convlen = ConvertToUTF8(&bufptr[linestart], i - linestart, conv, temp_buff, temp_buff_len, wchar_buff, wchar_buff_len, utf8_buff, utf8_buff_len, char_len);
					if (convlen == (size_t)-1) goto decode_error; // conversion failed
					bufferfile.Write(utf8_buff, convlen);
					pos += convlen;
				}
				bufferfile.Write("\n", 1); pos += 1;
				offsets.push_back(pos);
				linestart = i+char_len;
				prev_r = true;
			}
			else if (memcmp(nl, &bufptr[i], char_len) == 0)  { // '\n'
				if (!prev_r) {
					++nl_count_unix;
					size_t convlen = ConvertToUTF8(&bufptr[linestart], (i - linestart)+char_len, conv, temp_buff, temp_buff_len, wchar_buff, wchar_buff_len, utf8_buff, utf8_buff_len, char_len);
					if (convlen == (size_t)-1) goto decode_error; // conversion failed
					bufferfile.Write(utf8_buff, convlen);
					pos += convlen;
					offsets.push_back(pos);
				} else ++nl_count_dos;
				linestart = i+char_len;
				prev_r = false;
			}
			else if (memcmp(nulls, &bufptr[i], char_len) == 0) { // conversion fails on null bytes
				if (i - linestart > 0) {
					size_t convlen = ConvertToUTF8(&bufptr[linestart], i - linestart, conv, temp_buff, temp_buff_len, wchar_buff, wchar_buff_len, utf8_buff, utf8_buff_len, char_len);
					if (convlen == (size_t)-1) goto decode_error; // conversion failed
					bufferfile.Write(utf8_buff, convlen);
					pos += convlen;
				}
				bufferfile.Write("\xEF\xA3\xBF", 3); pos += 3; // Private use character as stand-in for null
				linestart = i+char_len;
				prev_r = false;
			}
			else {
				if (prev_r) ++nl_count_mac;
				prev_r = false;
			}
		}

		// Add text after last newline
		if (linestart < len) {
			size_t convlen = ConvertToUTF8(&bufptr[linestart], len - linestart, conv, temp_buff, temp_buff_len, wchar_buff, wchar_buff_len, utf8_buff, utf8_buff_len, char_len);
			if (convlen == (size_t)-1) goto decode_error; // conversion failed
			bufferfile.Write(utf8_buff, convlen);
			pos += convlen;
			offsets.push_back(pos);
		}

		// With null bytes in the text the end offset might not have been set yet.
		if (pos && (offsets.empty() || offsets.back() != pos)) offsets.push_back(pos);
	}
	if (buff_offset + pos != bufferfile.Length()) goto decode_error;

	// Close buffer file to ensure changes af flushed to disk
	bufferfile.Close();

	// Create the new revision
	StartChange();
	{
		// We want the new revision to either be the first (base) revision
		// or if there is a parent, directly following it.
		if (IsEmpty()) {
			Clear();
			NewRevision();
		}
		else PrepareForChange();

		m_textData.SetToFile(buff_offset, pos);
		UpdateHeadnode();

		// Update version info
		pLength(vHistory[m_docId.version_id]) = pos;
		pBuffPath(vRevisions[m_docId.document_id]) = buff_file_num;

		// Set the date
		pDate(vRevisions[m_docId.document_id]) = fileDate.GetValue().GetValue();

		// Set the document name
		SetPropertyName(filename);

		// Determine newline type
		wxTextFileType nl_type = wxTextFileType_None;
		if (nl_count_dos >= nl_count_unix && nl_count_dos >= nl_count_mac) nl_type = wxTextFileType_Dos;
		else if (nl_count_unix >= nl_count_dos && nl_count_unix >= nl_count_mac) nl_type = wxTextFileType_Unix;
		else if (nl_count_mac >= nl_count_dos && nl_count_mac >= nl_count_unix) nl_type = wxTextFileType_Mac;
		if (nl_type != wxTextFileType_None && nl_type != wxTextBuffer::typeDefault) SetPropertyEOL(nl_type);

		// Set Encoding & BOM
		if (enc != wxFONTENCODING_SYSTEM && enc != wxFONTENCODING_DEFAULT) SetPropertyEncoding(enc);
		if (bom_len > 0) SetPropertyBOM(true);

		Freeze();

		// Set file mirror
		if (mirror.empty()) m_catalyst.SetFileMirror(path.GetFullPath(), m_docId, fileDate);
		else m_catalyst.SetFileMirror(mirror, m_docId, fileDate);
	}
	EndChange();

	// Notify that document mirror info has changed
	// (needs own scope to avoid interferring with goto)
	{
		const doc_id di = m_docId;
		m_catalyst.UnLock();
			dispatcher.Notify(wxT("DOC_UPDATED"), (void*)&di, 0);
		m_catalyst.ReLock();
	}

	//wxLogDebug(wxT("reserved %d needed %u"), (int)(len/35), offsets.size());
	return cxFILE_OK;

decode_error:
	// This is where we end up if the decoding fails

	// Clean up the buffer file
	if (IsEmpty() && buff_offset == 0) {
		bufferfile.Close();
		wxRemoveFile(buff_path.GetFullPath());
	}
	else {
		// Remove any data added during this session
#ifdef __WXMSW__
		const int fd = _fileno(bufferfile.fp());
		bufferfile.Seek(buff_offset);
		HANDLE hFile = (HANDLE)_get_osfhandle(fd);
		SetEndOfFile(hFile);
#else
		const int fd = fileno(bufferfile.fp());
		ftruncate(fd, buff_offset);
#endif
	}

	// Notify the user that decoding failed
	return cxFILE_CONV_ERROR;
}

cxFileResult Document::SaveText(const wxFileName& path, bool forceNativeEOL, const wxString& realpath, bool keepMirrorDate, bool noAtomic) {
	wxASSERT(IsOk());
	wxASSERT(path.IsOk());
	wxASSERT(path.IsAbsolute());

	const wxString fullPath = path.GetFullPath();
	const wxString tmpPath = noAtomic ? fullPath : fullPath + wxT(".etmp");

	if (path.FileExists() && !path.IsFileWritable()) {
		//wxMessageBox(fullPath + _T(" is write protected."), _T("e Error"), wxICON_ERROR);
		return cxFILE_WRITABLE_ERROR;
	}

	// Do the actual saving
	{
		// Open the file
		// (it will close itself when we exit scope)
		wxFFile file(tmpPath, _("wb"));
		if (!file.IsOpened()) {
			wxLogDebug(wxT("Could not open file"));
			return cxFILE_OPEN_ERROR;
		}

		// Set end-of-line type
		wxTextFileType eol = GetPropertyEOL();
		if (eol == wxTextFileType_None || forceNativeEOL) eol = wxTextBuffer::typeDefault;
		wxFontEncoding propEncoding = GetPropertyEncoding();
		const char* nl = "";
		const char* nulls = "\0\0\0\0";
		unsigned int nl_len = 0;
		unsigned int char_len = 0;
		switch (propEncoding) {
		case wxFONTENCODING_UTF32BE:
			if (eol == wxTextFileType_Mac) {
				nl = "\x00\x00\x00\x0D";
				nl_len = 4;
			}
			else if (eol == wxTextFileType_Dos) {
				nl = "\x00\x00\x00\x0D\x00\x00\x00\x0A";
				nl_len = 8;
			}
			char_len = 4;
			break;
		case wxFONTENCODING_UTF32LE:
			if (eol == wxTextFileType_Mac) {
				nl = "\x0D\x00\x00\x00";
				nl_len = 4;
			}
			else if (eol == wxTextFileType_Dos) {
				nl = "\x0D\x00\x00\x00\x0A\x00\x00\x00";
				nl_len = 8;
			}
			char_len = 4;
			break;
		case wxFONTENCODING_UTF16BE:
			if (eol == wxTextFileType_Mac) {
				nl = "\x00\x0D";
				nl_len = 2;
			}
			else if (eol == wxTextFileType_Dos) {
				nl = "\x00\x0D\x00\x0A";
				nl_len = 4;
			}
			char_len = 2;
			break;
		case wxFONTENCODING_UTF16LE:
			if (eol == wxTextFileType_Mac) {
				nl = "\x0D\x00";
				nl_len = 2;
			}
			else if (eol == wxTextFileType_Dos) {
				nl = "\x0D\x00\x0A\x00";
				nl_len = 4;
			}
			char_len = 2;
			break;
		default:
			if (eol == wxTextFileType_Mac) {
				nl = "\x0D";
				nl_len = 1;
			}
			else if (eol == wxTextFileType_Dos) {
				nl = "\x0D\x0A";
				nl_len = 2;
			}
			char_len = 1;
		}

		// Write Byte-Order-Marker
		if (GetPropertyBOM()) {
			switch (propEncoding) {
			case wxFONTENCODING_UTF32BE:
				file.Write("\x00\x00\xFE\xFF", 4);
				break;
			case wxFONTENCODING_UTF32LE:
				file.Write("\xFF\xFE\x00\x00", 4);
				break;
			case wxFONTENCODING_UTF16BE:
				file.Write("\xFE\xFF", 2);
				break;
			case wxFONTENCODING_UTF16LE:
				file.Write("\xFF\xFE", 2);
				break;
			case wxFONTENCODING_UTF8:
				file.Write("\xEF\xBB\xBF", 3);
				break;
			case wxFONTENCODING_UTF7:
				file.Write("\x2B\x2F\x76\x38\x2D", 5);
				break;
			default:
				wxASSERT(false);
			}
		}

		// Prepare converter & buffers
		wxFontEncoding enc = propEncoding == wxFONTENCODING_DEFAULT ? wxFONTENCODING_SYSTEM : propEncoding;
		wxCSConv conv(enc);
		size_t utf8_buff_len = 128;
		wxCharBuffer utf8_buff(utf8_buff_len);
		size_t wchar_buff_len = 128;
		wxWCharBuffer wchar_buff(wchar_buff_len);
		size_t out_buff_len = 128;
		wxCharBuffer out_buff(out_buff_len);

		// Convert and save one line at a time
		int len = GetLength();
		unsigned int line_start = 0;
		for(doc_byte_iter dbi(*this); dbi.GetIndex() < len; ++dbi) {
			if (*dbi == '\n') {
				// Define the line
				unsigned int line_end = dbi.GetIndex();
				if (eol == wxTextFileType_Unix) ++line_end; // Only include newline in conversion when same as internal
				unsigned int line_len = line_end - line_start;

				if (line_len) {
					// Copy line into temp buffer
					if (utf8_buff_len < line_len+1) {
						utf8_buff_len = line_len+1;
						utf8_buff = wxCharBuffer(utf8_buff_len);
					}
					m_textData.GetNodeTextPart((unsigned char*)utf8_buff.data(), GetHeadnode(), line_start, line_end);
					utf8_buff.data()[line_len] = '\0';

					// Write line to file
					if (enc == wxFONTENCODING_UTF8) file.Write(utf8_buff.data(), line_len);
					else {
						size_t out_len = ConvertFromUTF8(utf8_buff, conv, wchar_buff, wchar_buff_len, out_buff, out_buff_len, char_len);
						if (out_len == (size_t)-1) {
							goto conv_failed; // Conversion failed
						}
						if (out_len) file.Write(out_buff.data(), out_len);
					}
				}
				if (eol != wxTextFileType_Unix) file.Write(nl, nl_len);

				line_start = dbi.GetIndex() + 1;
			}
			else if ((char)*dbi == '\xEF' && dbi.GetIndex()+2 < len) { // Check for null stand-in & conv to real nulls if needed
				if ((char)dbi[1] == '\xA3' && (char)dbi[2] == '\xBF') {
					// Define the line
					unsigned int line_end = dbi.GetIndex();
					unsigned int line_len = line_end - line_start;

					if (line_len) {
						// Copy line into temp buffer
						if (utf8_buff_len < line_len+1) {
							utf8_buff_len = line_len+1;
							utf8_buff = wxCharBuffer(utf8_buff_len);
						}
						m_textData.GetNodeTextPart((unsigned char*)utf8_buff.data(), GetHeadnode(), line_start, line_end);
						utf8_buff.data()[line_len] = '\0';

						// Write line to file
						if (enc == wxFONTENCODING_UTF8) file.Write(utf8_buff.data(), line_len);
						else {
							size_t out_len = ConvertFromUTF8(utf8_buff, conv, wchar_buff, wchar_buff_len, out_buff, out_buff_len, char_len);
							if (out_len == (size_t)-1) {
								goto conv_failed; // Conversion failed
							}
							if (out_len) file.Write(out_buff.data(), out_len);
						}
					}
					file.Write(nulls, char_len);

					line_start = dbi.GetIndex() + 3;
					dbi += 2;
				}
			}
		}
		if ((int)line_start < len) { // Save rest after last newline
			unsigned int line_len = len - line_start;

			// Copy line into temp buffer
			if (utf8_buff_len < line_len+1) {
				utf8_buff_len = line_len+1;
				utf8_buff = wxCharBuffer(utf8_buff_len);
			}
			m_textData.GetNodeTextPart((unsigned char*)utf8_buff.data(), GetHeadnode(), line_start, len);
			utf8_buff.data()[line_len] = '\0';

			// Write line to file
			if (enc == wxFONTENCODING_UTF8) file.Write(utf8_buff.data(), line_len);
			else {
				size_t out_len = ConvertFromUTF8(utf8_buff, conv, wchar_buff, wchar_buff_len, out_buff, out_buff_len, char_len);
				if (out_len == (size_t)-1) goto conv_failed; // Conversion failed
				if (out_len) file.Write(out_buff.data(), out_len);
			}
		}
	}

	// Atomic save.
	if (!noAtomic) {
		FILE_PERMISSIONS permissions = 0;
		bool previous_file_exists = path.FileExists();

		if (previous_file_exists) {
			permissions = eDocumentPath::GetPermissions(fullPath);
			wxRemoveFile(fullPath);
		}
		
		wxRenameFile(tmpPath, fullPath);

		if (previous_file_exists)
			eDocumentPath::SetPermissions(fullPath, permissions);
	}

	{
		// If filename has changed we have to update it
		const wxString oldName = GetPropertyName();
		const wxString realname = realpath.AfterLast(wxT('/'));
		const wxString newName = realname.empty() ? path.GetFullName() : realname;
		if (oldName != newName) {
			const bool notifyCache = do_notify;
			do_notify = false; // we don't want any notification from name change
				SetPropertyName(newName);
			do_notify = notifyCache;
		}

		// We need to freeze current version so that we can mark it as saved.
		Freeze();

		if (keepMirrorDate) {
			wxASSERT(!realpath.empty());

			doc_id di;
			wxDateTime modDate;
			if (m_catalyst.GetFileMirror(realpath, di, modDate))
				wxFileName(path).SetTimes(NULL, &modDate, NULL);
			else wxASSERT(false);
		}
		else {
			// Set the dates
			const wxDateTime modDate = IsDocument() ? GetDate() : path.GetModificationTime();
			if (IsDocument()) {
				// Documents get modification date set to the commit date
				wxFileName(path).SetTimes(NULL, &modDate, NULL);
			}

			// Set file mirror
			m_catalyst.SetFileMirror(realpath.empty() ? path.GetFullPath() : realpath, m_docId, modDate);

			// Notify that document mirror info has changed
			const doc_id di = m_docId;
			m_catalyst.UnLock();
				dispatcher.Notify(wxT("DOC_UPDATED"), (void*)&di, 0);
			m_catalyst.ReLock();
		}
	}

	return cxFILE_OK;

conv_failed:
	// clean up atomic save attempt
	if (wxFileExists(tmpPath)) wxRemoveFile(tmpPath);
	return cxFILE_CONV_ERROR;
}

void Document::GetLines(vector<unsigned int>& list) const {
	wxASSERT(IsOk());
	wxASSERT(list.empty());

	const int len = GetLength();
	if (len <= 0) return;

	list.reserve(len/35); // Aprox characters per line.

	for (doc_byte_iter subject(*this); subject.GetIndex() < len; ++subject) {
		if (*subject == '\n') // Lines end right after newlines
			list.push_back(subject.GetIndex()+1);
	}

	// If the text does not end with a newline, put the rest of the text in as the last
	// (non-terminated) line.
	if (list.empty() || list.back() != GetLength())
		list.push_back(GetLength());
}

bool Document::HasProperty(const wxString& name) const {
	wxASSERT(IsOk());
	if (vHistory.GetSize() == 0) return false; // empty doc
	
	// Nothing to do if there are no properties
	const node_ref propnode = GetPropnode();
	if (!propnode.IsOk()) return false;

	const DataList propList(m_catalyst, m_docId, propnode);
	return propList.HasProperty(name);
}

bool Document::GetProperty(const wxString& name, wxString& value) const {
	wxASSERT(IsOk());
	if (vHistory.GetSize() == 0) return false; // empty doc

	const node_ref propnode = GetPropnode();
	if (!propnode.IsOk()) return false; // no properties

	DataList propList(m_catalyst, m_docId, propnode);
	return propList.GetProperty(name, value);
}

void Document::SetProperty(const wxString& name, const wxString& value) {
	wxASSERT(IsOk());

	// No value is indicated by lack of property
	if (name.IsEmpty()) {
		DeleteProperty(name);
		return;
	}

	// No need to make a change if the property is already set
	wxString oldValue;
	bool got_property = GetProperty(name, oldValue);
	if (got_property && value == oldValue) return;

	const node_ref propNode = GetPropnode();

	PrepareForChange();
	const node_ref invalidNode;
	DataList propList(m_catalyst, m_docId, invalidNode);

	if (!propNode.IsOk()) propList.Create();
	else propList.SetHeadnode(propNode);

	// Set the property
	propList.SetProperty(name, value);
	const node_ref newPropNode = propList.GetHeadnode();

	// Update the document
	if (propNode != newPropNode) {
		UpdatePropnode(newPropNode);

		// Notify subscribers that the revision has changed
		if (do_notify) {
			m_catalyst.UnLock();
				dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
			m_catalyst.ReLock();
		}
	}
}

void Document::DeleteProperty(const wxString& name) {
	wxASSERT(IsOk());

	const node_ref propNode = GetPropnode();

	// Nothing to do if there are no properties
	if (vHistory.GetSize() == 0 || !propNode.IsOk()) return;

	PrepareForChange();
	DataList propList(m_catalyst, m_docId, propNode);

	// Delete the property (nothing will happen if it does not exists)
	propList.DeleteProperty(name);
	const node_ref newPropNode = propList.GetHeadnode();

	// Update the document
	if (propNode != newPropNode) {
		UpdatePropnode(newPropNode);

		// Notify subscribers that the revision has changed
		if (do_notify) {
			m_catalyst.UnLock();
				dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
			m_catalyst.ReLock();
		}
	}
}

wxString Document::GetPropertyName() const {
	wxString value;
	if (GetProperty(wxT("name"), value)) return value;
	else return wxString(); // If there is no name property, return default
}

void Document::SetPropertyName(const wxString& name) {
	SetProperty(wxT("name"), name);
}

wxFontEncoding Document::GetPropertyEncoding() const {
	wxString value;
	if (GetProperty(wxT("enc"), value)) return wxFontMapper::GetEncodingFromName(value);
	else return wxFONTENCODING_DEFAULT; // If there is no encoding property, return default
}

void Document::SetPropertyEncoding(wxFontEncoding encoding) {
	if (encoding == wxFONTENCODING_SYSTEM || encoding == wxFONTENCODING_DEFAULT) {
		DeleteProperty(wxT("enc")); // Default encoding is indicated by lack of property
	}
	else SetProperty(wxT("enc"), wxFontMapper::GetEncodingName(encoding));
}

wxTextFileType Document::GetPropertyEOL() const{
	wxString value;
	if (GetProperty(wxT("eol"), value)) {
		if (value == wxT("crlf")) return wxTextFileType_Dos;
		else if (value == wxT("lf")) return wxTextFileType_Unix;
		else if (value == wxT("cr")) return wxTextFileType_Mac;
		else wxASSERT(false);
	}

	// If there is no property, return default value
	return wxTextFileType_None; // Native
}

void Document::SetPropertyEOL(wxTextFileType eol) {
	if (eol == wxTextFileType_None) {
		DeleteProperty(wxT("eol")); // Default encoding is indicated by lack of property
		return;
	}

	switch (eol) { // Set the property
	case wxTextFileType_Unix:
		SetProperty(wxT("eol"), wxString(wxT("lf")));
		break;
	case wxTextFileType_Dos:
		SetProperty(wxT("eol"), wxString(wxT("crlf")));
		break;
	case wxTextFileType_Mac:
		SetProperty(wxT("eol"), wxString(wxT("cr")));
		break;
	default:
		wxASSERT(false);
	}
}

bool Document::GetPropertyBOM() const {
	wxASSERT(IsOk());

	node_ref propnode = GetPropnode();

	if (vHistory.GetSize() && propnode.node_id != -1) {
		DataList propList(m_catalyst, m_docId, propnode);
		bool value;
		bool result = propList.GetProperty(wxT("bom"), value);
		if (result) return value;
	}

	// If there is no property, return default value
	return false;
}

void Document::SetPropertyBOM(bool hasBOM) {
	wxASSERT(IsOk());

	// No need to make a change if the property is already set
	if (hasBOM == GetPropertyBOM()) return;

	const node_ref propNode = GetPropnode();
	node_ref newPropNode;

	// Default encoding is indicated by lack of property
	if (hasBOM == false) {
		if (vHistory.GetSize() == 0 || !propNode.IsOk()) return;

		PrepareForChange();
		DataList propList(m_catalyst, m_docId, propNode);

		// Delete the property (nothing will happen if it does not exists)
		propList.DeleteProperty(wxT("bom"));
		newPropNode = propList.GetHeadnode();
	}
	else {
		PrepareForChange();
		const node_ref invalidNode;
		DataList propList(m_catalyst, m_docId, invalidNode);

		if (GetPropnode().node_id == -1) propList.Create();
		else propList.SetHeadnode(propNode);

		// Set the property
		propList.SetProperty(wxT("bom"), true);
		newPropNode = propList.GetHeadnode();
	}

	// Update the document
	if (propNode != newPropNode) {
		UpdatePropnode(newPropNode);

		// Notify subscribers that the revision has changed
		if (do_notify) {
			m_catalyst.UnLock();
				dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
			m_catalyst.ReLock();
		}
	}
}

search_result Document::RegExFind(const pcre* re, const pcre_extra* study, int start_pos, map<unsigned int,interval> *captures, int end_pos, int search_options) const {
	wxASSERT(end_pos == 0 || (end_pos > 0 && end_pos <= (int)GetLength()));
	if (end_pos == 0) end_pos = GetLength();

#ifdef __WXDEBUG__
	// Check that we are not breaking into a UTF8 byte sequence
	doc_byte_iter dbi(*this, start_pos);
	wxASSERT((*dbi & 0xC0) != 0x80);
	if (end_pos < pLength(vHistory[m_docId.version_id])) {
		dbi.SetIndex(end_pos);
		wxASSERT((*dbi & 0xC0) != 0x80);
	}
#endif  //__WXDEBUG__

	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	search_result sr;

	// Do the search
	doc_byte_iter subject(*this);
	//int search_options = allowEmpty ? 0 : PCRE_NOTEMPTY;
	//search_options |= PCRE_NO_UTF8_CHECK;
	//if (end_pos ! (int)GetLength()) search_options |= PCRE_NOTEOL;

	//const char *pSubject = (const char *)subject.operator*();
	const int rc = cx_pcre_exec(
		re,                   // the compiled pattern
		study,                // extra data - if we study the pattern
		subject,              // the subject string
		end_pos,              // the length of the subject
		start_pos,            // start at offset in the subject
		search_options,       // options
		ovector,              // output vector for substring information
		OVECCOUNT);           // number of elements in the output vector
	// Copy match info from ovector to result struct
	sr.error_code = rc;
	if (rc >= 0) {
		wxASSERT(ovector[0] >= 0 && ovector[0] <= (int)GetLength());
		wxASSERT(ovector[1] >= ovector[0] && ovector[1] <= (int)GetLength());
		sr.start = ovector[0];
		sr.end = ovector[1];
	}

	if (captures != NULL && rc > 0) {
		// copy intervals to the supplied vector
		for (int i = 0; i < rc; ++i) {
			if (ovector[2*i] != -1)
				(*captures)[i] = interval(ovector[2*i], ovector[2*i+1]);
		}
	}

	return sr;
}


search_result Document::RegExFind(const wxString& searchtext, int start_pos, bool matchcase, map<unsigned int,interval> *captures, int end_pos) const {
	wxASSERT(end_pos == 0 || (0 < end_pos && end_pos <= (int)GetLength()));
	if (end_pos == 0) end_pos = GetLength();

#ifdef __WXDEBUG__
	// Check that we are not breaking into a UTF8 byte sequence
	doc_byte_iter dbi(*this, start_pos);
	wxASSERT((*dbi & 0xC0) != 0x80);
	if (end_pos < pLength(vHistory[m_docId.version_id])) {
		dbi.SetIndex(end_pos);
		wxASSERT((*dbi & 0xC0) != 0x80);
	}
#endif  //__WXDEBUG__

	int options = PCRE_UTF8|PCRE_MULTILINE;
	if (!matchcase) options |= PCRE_CASELESS;

	// Check if we have the regex cached
	if (m_re) {
		if (searchtext == m_regex_cache && options == m_options_cache) {
			// We might want the study the regex the first time we end here
		}
		else {
			free(m_re);
			m_re = NULL;
		}
	}

	// Compile the pattern
	if (!m_re) {
		const char *error;
		int erroffset;

		m_re = pcre_compile(
			searchtext.mb_str(wxConvUTF8),   // the pattern
			options,              // options
			&error,               // for error message
			&erroffset,           // for error offset
			NULL);                // use default character tables

		if (m_re == 0) {
			search_result sr;
			sr.error_code = -4; // invalid pattern
			return sr;
		}
		else {
			// The compiled regex (m_re) is cached for possible reuse in next search
			// it is finally free'd in destructor
			m_options_cache = options;
			m_regex_cache = searchtext;
		}
	}

	// Do the search
	return RegExFind(m_re, NULL, start_pos, captures, end_pos);
}

search_result Document::RegExFindBackwards(const wxString& searchtext, int start_pos, bool matchcase) const {
	pcre *re;
	const char *error;
	int erroffset;
	const int OVECCOUNT = 30;
	int ovector[OVECCOUNT];
	search_result sr;

	int options = PCRE_UTF8|PCRE_MULTILINE;
	if (!matchcase) options = options | PCRE_CASELESS;

	re = pcre_compile(
		searchtext.mb_str(wxConvUTF8),   // the pattern
		options,              // options
		&error,               // for error message
		&erroffset,           // for error offset
		NULL);                // use default character tables

	if (re == 0) {
		sr.error_code = -4; // invalid pattern
		return sr;
	}

	int rc = -1;
	doc_byte_iter subject(*this);
	for (int offset = start_pos-1; offset >= 0; --offset) {
	//const char *pSubject = (const char *)subject.operator*();
		rc = cx_pcre_exec(
			re,                   // the compiled pattern
			NULL,                 // no extra data - we didn't study the pattern
			subject,              // the subject string
			start_pos - subject.GetIndex(),  // the length of the subject
			offset,                    // start at offset in the subject
			PCRE_ANCHORED,        // options
			ovector,              // output vector for substring information
			OVECCOUNT);           // number of elements in the output vector
		if (rc >= 0) break; // Match found!
	}

	// Clean up
	free(re);

	// Copy match info from ovector to result struct
	sr.error_code = rc;
	if (rc >= 0) {
		wxASSERT(ovector[0] >= 0 && ovector[1] <= (int)GetLength());
		wxASSERT(ovector[1] >= ovector[0] && ovector[1] <= (int)GetLength());
		sr.start = ovector[0];
		sr.end = ovector[1];
	}

	return sr;
}

search_result Document::Find(const wxString& searchtext, int start_pos, bool matchcase, int end_pos) const {
	wxASSERT(IsOk());
	wxASSERT(start_pos >= 0);

	wxASSERT(end_pos == 0 || (end_pos > 0 && end_pos <= (int)GetLength()));
	if (end_pos == 0) end_pos = GetLength();

	search_result sr = {-1, 0, 0};
	if (searchtext.empty()) return sr;
	if (start_pos == (int)GetLength() || GetLength() == 0) return sr;
	wxASSERT(start_pos < (int)GetLength());

	// We need both upper- & lowercase versions for caseless search
	wxString text = searchtext;
	wxString textUpper;
	if (!matchcase) {
		text.MakeLower();
		textUpper = searchtext;
		textUpper.MakeUpper();
	}

	// Convert the searchtext to UTF8
	wxCharBuffer UTF8buffer = wxConvUTF8.cWC2MB(text);
	unsigned int byte_len = strlen(UTF8buffer);
	wxCharBuffer UTF8bufferUpper;
	if (!matchcase) {
		UTF8bufferUpper = wxConvUTF8.cWC2MB(textUpper);
		wxASSERT(byte_len == strlen(UTF8bufferUpper));
	}

	int maxsearch = GetLength() - byte_len;
	if (start_pos > maxsearch) return sr;

	unsigned int last_char_pos = byte_len-1;

	// Build a dictionary of char-to-last distances in the search string
	map<char, int> charmap;
	map<char, int> charmap_upper;
	for (unsigned int i = 0; i < byte_len-1; ++i) {
		charmap[UTF8buffer[i]] = last_char_pos-i;
		if (!matchcase) charmap_upper[UTF8bufferUpper[i]] = last_char_pos-i;
	}

	// Prepare vars to avoid lookups in loop
	char lastChar = UTF8buffer[last_char_pos];
	char lastCharUpper = '\0';
	if (!matchcase) lastCharUpper = UTF8bufferUpper[last_char_pos];

	// Search
	doc_byte_iter subject(*this, start_pos + last_char_pos); // start from last char in search string

	// WARNING: This algorithm assumes that UTF8 upper- & lowercase chars have same byte width
	while (subject < end_pos) {
		const char c = *subject; // Get candidate for last char

		if (c == lastChar || (!matchcase && c == lastCharUpper)) {
			// Match indiviual chars
			doc_byte_iter byte_ptr = subject-1;
			int first_byte_pos = subject.GetIndex() - last_char_pos;
			unsigned int char_pos = last_char_pos-1;
			while (byte_ptr.GetIndex() >= first_byte_pos) {
				const char c2 = *byte_ptr;
				if (c2 != UTF8buffer[char_pos]) {
					if (matchcase || c2 != UTF8bufferUpper[char_pos]) break;
				}
				--byte_ptr; --char_pos;
			}

			if (byte_ptr.GetIndex() < first_byte_pos) {
				sr.error_code = 0;
				sr.start = first_byte_pos;
				sr.end = sr.start + byte_len;
				return sr; // text found!
			}
		}

		// If we don't have a match, see how far we can move char_pos
		map<char, int>::iterator result = charmap.find(c);
		map<char, int>::iterator result_upper = charmap_upper.find(c);
		if (result != charmap.end()) subject += result->second;
		else if (!matchcase && result_upper != charmap_upper.end()) subject += result_upper->second;
		else subject += byte_len;

		// Make sure that we end on a valid UTF8 char
		//while ((*subject & 0xC0) == 0x80) ++subject;
	}

	return sr; // reached end without finding text
}


search_result Document::FindBackwards(const wxString& searchtext, int start_pos, bool matchcase) const {
	wxASSERT(IsOk());
	wxASSERT(start_pos >= 0);

	search_result sr = {-1, 0, 0};
	if (start_pos == 0 || GetLength() == 0) return sr;
	wxASSERT(start_pos <= (int)GetLength());
	if (searchtext.empty()) return sr;

	// We need both upper- & lowercase versions for caseless search
	wxString text = searchtext;
	wxString textUpper;
	if (!matchcase) {
		text.MakeLower();
		textUpper = searchtext;
		textUpper.MakeUpper();
	}

	// Convert the searchtext to UTF8
	wxCharBuffer UTF8buffer = wxConvUTF8.cWC2MB(text);
	unsigned int byte_len = strlen(UTF8buffer);
	wxCharBuffer UTF8bufferUpper;
	if (!matchcase) {
		UTF8bufferUpper = wxConvUTF8.cWC2MB(textUpper);
		wxASSERT(byte_len == strlen(UTF8bufferUpper));
	}

	// Build a dictionary of char-to-first distances in the search string
	map<char, int> charmap;
	for (unsigned int i = byte_len-1; i > 0; --i) {
		charmap[UTF8buffer[i]] = i;
	}

	// Prepare vars to avoid lookups in loop
	char firstChar = UTF8buffer[(unsigned int)0];
	char firstCharUpper = '\0';
	if (!matchcase) firstCharUpper = UTF8bufferUpper[(unsigned int)0];

	// Search
	const int lastPossible = GetLength()-byte_len;
	doc_byte_iter subject(*this, wxMin(start_pos-1, lastPossible)); // start from char before start_pos

	// WARNING: This algorithm assumes that UTF8 upper- & lowercase chars have same byte width
	while (subject.GetIndex() >= 0) {
		char c = *subject; // Get candidate for last char

		if (c == firstChar || (!matchcase && c == firstCharUpper)) {
			// TODO: optimize by matching individual chars
			wxString subtext = GetTextPart(subject.GetIndex(), subject.GetIndex()+byte_len);

			// Check if the found text matches
			if (!matchcase) subtext.MakeLower();
			if (text == subtext) {
				sr.error_code = 0;
				sr.start = subject.GetIndex();
				sr.end = sr.start + byte_len;
				return sr; // text found!
			}
		}

		// If we don't have a match, see how far we can move char_pos
		map<char, int>::iterator result = charmap.find(c);
		if (result != charmap.end()) subject -= result->second;
		else subject -= byte_len;

		// Make sure that we end on a valid UTF8 char
		while ((*subject & 0xC0) == 0x80) --subject;
	}

	return sr; // reached end without finding text
}

unsigned int Document::Insert(int pos, const wxString& text) {
	wxASSERT(IsOk());
	wxASSERT(pos >= 0); // asserts (pos <= pLength) later

	// Don't do anything on an empty insertion
	if (text.empty()) return 0;

	if (vNodes.GetSize() == 0) {
		// First insertion
		NewRevision();
	}
	else PrepareForChange();

	// Insert the text
	const unsigned int bytes_len = m_textData.Insert(pos, text);
	UpdateHeadnode(); // Check if the headnode has been updated

	// Adjust the length
	pLength(vHistory[m_docId.version_id]) = m_textData.GetLength();

	// DEBUG
#ifdef __WXDEBUG__
	if (pLength(vHistory[m_docId.version_id]) != pLength(vNodes[GetHeadnode().node_id])) {
		wxLogDebug(wxT("WARNING: Insert - length mismatch in history"));
	}
#endif  //__WXDEBUG__

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) m_trackChanges(cxINSERTION, pos, bytes_len, m_trackChangesData);
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}

	return bytes_len; // Return number of actual bytes inserted
}

unsigned int Document::Insert(int pos, const char* text) {
	wxASSERT(IsOk());
	wxASSERT(pos >= 0); // asserts (pos <= pLength) later

	// Don't do anything on an empty insertion
	if (text[0] == '\0') return 0;

	if (vNodes.GetSize() == 0) {
		// First insertion
		NewRevision();
	}
	else PrepareForChange();

	// Insert the text
	const unsigned int bytes_len = m_textData.Insert(pos, text);
	UpdateHeadnode(); // Check if the headnode has been updated

	// Adjust the length
	pLength(vHistory[m_docId.version_id]) = m_textData.GetLength();

	// DEBUG
#ifdef __WXDEBUG__
	if (pLength(vHistory[m_docId.version_id]) != pLength(vNodes[GetHeadnode().node_id])) {
		wxLogDebug(wxT("WARNING: Insert - length mismatch in history"));
	}
#endif  //__WXDEBUG__

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) m_trackChanges(cxINSERTION, pos, bytes_len, m_trackChangesData);
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}

	return bytes_len; // Return number of actual bytes inserted
}

int Document::DeleteChar(unsigned int pos, bool nextchar) {
	wxASSERT(IsOk());

	PrepareForChange();

	// Delete the char
	const unsigned int bytes_len = m_textData.DeleteChar(pos, nextchar);
	UpdateHeadnode(); // Check if the headnode has been updated

	// Adjust the length
	pLength(vHistory[m_docId.version_id]) = m_textData.GetLength();

	// DEBUG
#ifdef __WXDEBUG
	if (pLength(vHistory[m_docId.version_id]) != pLength(vNodes[GetHeadnode().node_id])) {
		cout << "WARNING: Delete - length mismatch in history" << endl;
		cout << pLength(vHistory[version_id]) << " " << pLength(vNodes[GetHeadnode()]) << " " << nc.sizediff << endl;
	}
#endif  //__WXDEBUG__

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) m_trackChanges(cxDELETION, (nextchar ? pos : pos-bytes_len), bytes_len, m_trackChangesData);
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}

	return bytes_len;
}

void Document::Delete(int start_pos, int end_pos) {
	wxASSERT(IsOk());
	if (start_pos == end_pos) return; // nothing to delete

	PrepareForChange();

	// Delete the text
	m_textData.Delete(start_pos, end_pos);
	UpdateHeadnode(); // Check if the headnode has been updated

	// Adjust the length
	pLength(vHistory[m_docId.version_id]) = m_textData.GetLength();

	// DEBUG
#ifdef __WXDEBUG
	if (pLength(vHistory[m_docId.version_id]) != pLength(vNodes[GetHeadnode().node_id])) {
		cout << "WARNING: Delete - length mismatch in history" << endl;
		cout << pLength(vHistory[version_id]) << " " << pLength(vNodes[GetHeadnode()]) << " " << nc.sizediff << endl;
	}
#endif  //__WXDEBUG__

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) m_trackChanges(cxDELETION, start_pos, end_pos - start_pos, m_trackChangesData);
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}
}

void Document::DeleteAll() {
	wxASSERT(IsOk());
	if (IsEmpty()) return; // nothing to delete
	const unsigned int bytes_len = GetLength();

	PrepareForChange();

	// Delete the text
	m_textData.Clear();
	UpdateHeadnode(); // Check if the headnode has been updated

	// Adjust the length
	pLength(vHistory[m_docId.version_id]) = 0;

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) m_trackChanges(cxDELETION, 0, bytes_len, m_trackChangesData);
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}
}

void Document::Move(int source_startpos, int source_endpos, int dest_pos) {
	wxASSERT(IsOk());
	wxASSERT(source_startpos >= 0 && source_startpos <= pLength(vHistory[m_docId.version_id]));
	wxASSERT(source_endpos >= source_startpos && source_endpos <= pLength(vHistory[m_docId.version_id]));
	wxASSERT(dest_pos >= 0 && dest_pos <= pLength(vHistory[m_docId.version_id]));
	wxASSERT(dest_pos <= source_startpos || dest_pos >= source_endpos);
	if (source_startpos == source_endpos) return;
	if (source_startpos <= dest_pos && dest_pos <= source_endpos) return;

	PrepareForChange();

	// Move the text
	m_textData.Move(source_startpos, source_endpos, dest_pos);
	UpdateHeadnode(); // Check if the headnode has been updated

	wxASSERT(pLength(vHistory[m_docId.version_id]) == (int)m_textData.GetLength());

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) {
				const unsigned int bytes_len = source_endpos - source_startpos;
				m_trackChanges(cxDELETION, source_startpos, bytes_len, m_trackChangesData);
				m_trackChanges(cxINSERTION, ((dest_pos < source_startpos) ? dest_pos : dest_pos - bytes_len), bytes_len, m_trackChangesData);
			}
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}
}

unsigned int Document::Replace(unsigned int start_pos, unsigned int end_pos, const wxString& text) {
	wxASSERT(IsOk());
	wxASSERT(start_pos >= 0 && (int)start_pos <= pLength(vHistory[m_docId.version_id]));
	wxASSERT(end_pos >= start_pos && (int)end_pos <= pLength(vHistory[m_docId.version_id]));

	PrepareForChange();

	// Delete text
	if (start_pos < end_pos)
		m_textData.Delete(start_pos, end_pos);

	// Insert new text
	unsigned int byte_len = 0;
	if (!text.empty())
		byte_len = m_textData.Insert(start_pos, text);

	// Check if the headnode has been updated
	UpdateHeadnode();

	// Adjust the length
	pLength(vHistory[m_docId.version_id]) = m_textData.GetLength();

	// Notify subscribers that the revision has changed
	if (do_notify || m_trackChanges) {
		m_catalyst.UnLock();
			if (m_trackChanges) {
				m_trackChanges(cxDELETION, start_pos, end_pos - start_pos, m_trackChangesData);
				m_trackChanges(cxINSERTION, start_pos, byte_len, m_trackChangesData);
			}
			if (do_notify) dispatcher.Notify(wxT("DOC_UPDATEREVISION"), &m_docId, 0);
		m_catalyst.ReLock();
	}

	return byte_len; // return number of bytes inserted
}

void Document::Freeze() {
	wxASSERT(IsOk());
	if (in_change) return; // Don't freeze during grouped changes
	if (m_docId.IsDocument()) return; // Only a Draft can be frozen

	if (vHistory.GetSize() == 0) NewRevision();
	c4_RowRef rRevision = vHistory[m_docId.version_id];

	const node_ref propnode = GetPropnode();
	if (pState(rRevision) == STATE_FROZEN) {
		const node_ref headnode = GetHeadnode();
		wxASSERT(headnode.IsDocument() || pState(vNodes[headnode.node_id]) == STATE_FROZEN);
		wxASSERT(!propnode.IsOk() || propnode.IsDocument() || pState(vNodes[propnode.node_id]) == STATE_FROZEN);
		return; // You can't freeze a version that is already frozen
	}

	// Freeze all nodes in version
	m_textData.Freeze();
	if (propnode.IsOk() && propnode.IsDraft()) {
		DataList(m_catalyst, m_docId, propnode).Freeze();
	}

	pState(rRevision) = STATE_FROZEN;
}

bool Document::IsFrozen() const {
	if (m_docId.IsDocument()) return true; // Only Drafts can be editable
	
	c4_RowRef rRevision = vHistory[m_docId.version_id];
	return pState(rRevision) == STATE_FROZEN;
}

wxDateTime Document::GetDate() const {
	wxASSERT(IsOk());

	// In a draft there is a single date for last changed
	// while in the document each version has a commit date
	const wxLongLong date = m_docId.IsDraft() ? pDate.Get(vRevisions[m_docId.document_id]) : pDate.Get(vHistory[m_docId.version_id]);
	return wxDateTime(date);
}

void Document::SetDocument(const doc_id& di) {
	//wxASSERT(m_catalyst.IsOk(di));
	if (m_docId == di) return;

	// During grouped changes moving away from current
	// change cleans it up
	if (in_change && !IsFrozen()) {
		m_catalyst.DeleteDraft(m_docId);
	}

	m_docId = di;

	vRevisions = (m_docId.IsDraft()) ? m_catalyst.GetRevView() : m_catalyst.GetDocView();

	const c4_RowRef rDocument = vRevisions[m_docId.document_id];
	vNodes = pNodes(rDocument);
	vHistory = pHistory(rDocument);

	if (m_docId.IsDraft() && vHistory.GetSize() == 0) {
		// We have to create the first revision in this draft doc
		wxASSERT(di.version_id == 0);
		NewRevision();
	}
	else {
		wxASSERT(di.version_id >= 0 && di.version_id < vHistory.GetSize());

		const c4_RowRef rVersion = vHistory[m_docId.version_id];
		const node_ref headnode(pHeadVer(rVersion), pHeadnode(rVersion));
		m_textData.SetDocument(m_docId, headnode);
	}

	if (m_trackChanges) {
		m_catalyst.UnLock();
			m_trackChanges(cxVERSIONCHANGE, 0, 0, m_trackChangesData);
		m_catalyst.ReLock();
	}
}

void Document::SetDocRead(bool isRead) {
	if (m_docId.IsDocument()) {
		c4_RowRef rRev = vHistory[m_docId.version_id];

		const bool isUnread = pUnread(rRev) > 0;
		if (isUnread == isRead) {
			pUnread(rRev) = !isRead;

			// Notify subscribers that the document history has changed
			const doc_id di = m_docId;
			m_catalyst.UnLock();
				dispatcher.Notify(wxT("DOC_UPDATED"), (void*)&di, 0);
			m_catalyst.ReLock();
		}
	}
}

/*
void Document::SetVersion(VERSION_ID vid) {
	wxASSERT(document_id != -1); // Only document have to be valid (opposed to IsOk() also needing version)
	wxASSERT(vid >= 0);
	wxASSERT(vid < vHistory.GetSize() || (vid == 0 && vHistory.GetSize() == 0));

	if (vHistory.GetSize() == 0) m_textData.Invalidate();
	else {
		const node_ref headnode(-1, pHeadnode(vHistory[vid])); // DRAFT headnode
		m_textData.SetHeadnode(headnode);
	}

	version_id = vid;
}
*/


int Document::GetDocumentID() const {
	wxASSERT(IsOk());
	return m_docId.document_id;
}

int Document::GetVersionID() const {
	wxASSERT(IsOk());
	return m_docId.version_id;
}

int Document::GetVersionCount() const {
	if (IsOk())	return vHistory.GetSize();
	else return 0;
}

// Returns the document parent (not the draft (undo history) parent)
doc_id Document::GetParent() const {
	wxASSERT(IsOk());

	if (m_docId.IsDraft()) {
		const c4_RowRef rDocument = vRevisions[m_docId.document_id];
		return doc_id(DOCUMENT, pParentDoc(rDocument), pParentVer(rDocument));
	}
	else
		return doc_id(DOCUMENT, m_docId.document_id, pParent(vHistory[m_docId.version_id]));
}

doc_id Document::GetDraftParent() const {
	wxASSERT(IsOk());
	wxASSERT(m_docId.IsDraft());

	return doc_id(DRAFT, m_docId.document_id, pParent(vHistory[m_docId.version_id]));
}

doc_id Document::GetDraftParent(int version_id) const {
	wxASSERT(IsOk());
	wxASSERT(m_docId.IsDraft());
	wxASSERT(version_id >= 0 && version_id < vHistory.GetSize());

	return doc_id(DRAFT, m_docId.document_id, pParent(vHistory[version_id]));
}

bool Document::IsDraftAttached() const {
	wxASSERT(IsOk());
	wxASSERT(m_docId.IsDraft());

	const c4_RowRef rDocument = vRevisions[m_docId.document_id];

	if (pParentDoc(rDocument) == -1 || pParentVer(rDocument) == -1) return false;
	else return true;
}

int Document::GetVersionLength(const doc_id& di) const {
	wxASSERT(IsOk());
	wxASSERT(m_catalyst.IsOk(di));

	const c4_View vRevs = (di.IsDraft()) ? m_catalyst.GetRevView() : m_catalyst.GetDocView();
	const c4_RowRef rDoc = vRevs[di.document_id];
	const c4_View vHis = pHistory(rDoc);

	return pLength(vHis[di.version_id]);
}

void Document::GetVersionChildren(int version, vector<int>& childlist) const {
	wxASSERT(IsOk());
	wxASSERT(version >= 0 && version < vHistory.GetSize());

	int result = 0;
	while((result = vHistory.Find(pParent[version], result)) != -1) {
		if (result) childlist.push_back(result); // head can't be child
		++result;
	}
}

void Document::MakeHead() {
	wxASSERT(IsOk());

	int docId = m_docId.document_id;

	if (m_docId.IsDraft()) {
		pHead(vRevisions[m_docId.document_id]) = m_docId.version_id;

		// Get the parent (if there is any)
		const c4_RowRef rDraft = vRevisions[m_docId.document_id];
		docId = pParentDoc(rDraft);
	}

	// If this is a Document or a Draft with a Document parent we have
	// set the Document Head.
	if (docId != -1) {
		c4_View vDocuments = m_catalyst.GetDocView();
		wxASSERT(docId >= 0 && docId < vDocuments.GetSize());

		c4_RowRef rDoc = vDocuments[docId];
		pHeadType(rDoc) = m_docId.Type();
		pHead(vRevisions[m_docId.document_id]) = m_docId.version_id;
	}
}

vector<match> Document::Diff(const doc_id& version1, const doc_id& version2) const {
	wxASSERT(IsOk());
	wxASSERT(m_catalyst.InSameHistory(version1, version2));
	//wxASSERT(version1 < version2);

	const node_ref headnode1 = GetHeadnode(version1);
	const node_ref headnode2 = GetHeadnode(version2);
	const DataText text1(m_catalyst, version1, headnode1);

	return text1.Diff(version2, headnode2);
}

void Document::PartialDiff(const interval& range, vector<cxDiffEntry>& rangeHistory) const {
	const c4_RowRef rVersion = vHistory[m_docId.version_id];
	const node_ref headnode(pHeadVer(rVersion), pHeadnode(rVersion));
	DataText text1(m_catalyst, m_docId, headnode);

	text1.PartialDiff(range, rangeHistory);
	if (rangeHistory.empty()) return;

	reverse(rangeHistory.begin(), rangeHistory.end());
	for (size_t i = 1; i < rangeHistory.size(); ++i)
		rangeHistory[i].parent = i-1;
}

vector<cxChange> Document::GetChanges(const doc_id& version1, const doc_id& version2) const {
	// Calculate diff
	const vector<match> matchlist = Diff(version1, version2);
	
	// Build list of changes
	vector<cxChange> changeset;
	unsigned int pos1 = 0;
	unsigned int pos2 = 0;
	for (vector<match>::const_iterator m = matchlist.begin(); m != matchlist.end(); ++m) {
		if (pos1 < m->iv1_start_pos) { // Deleted
			const cxChange change = {cxDELETION, pos2, pos1, m->iv1_start_pos, 0};
			changeset.push_back(change);
		}
		if (pos2 < m->iv2_start_pos) { // Inserted
			const cxChange change = {cxINSERTION, pos1, pos2, m->iv2_start_pos, 0};
			changeset.push_back(change);
		}

		pos1 = m->iv1_end_pos;
		pos2 = m->iv2_end_pos;
	}
	
	return changeset;
}

bool Document::TextChanged(const doc_id& version1, const doc_id& version2) const {
	wxASSERT(IsOk());
	wxASSERT(m_catalyst.InSameHistory(version1, version2));

	return GetHeadnode(version1) != GetHeadnode(version2);
}

bool Document::PropertiesChanged(const doc_id& version1, const doc_id& version2) const {
	wxASSERT(IsOk());
	wxASSERT(m_catalyst.InSameHistory(version1, version2));

	return GetPropnode(version1) != GetPropnode(version2);
}


// - Private support functions --------------------------------------------------

void Document::PrepareForChange() {
	if (m_docId.type == DOCUMENT || pState(vHistory[m_docId.version_id]) == STATE_FROZEN) NewRevision();
}

node_ref Document::GetHeadnode() const {
	const node_ref headnode = m_textData.GetHeadnode();

	wxASSERT(pHeadVer(vHistory[m_docId.version_id]) == headnode.version_id);
	wxASSERT(pHeadnode(vHistory[m_docId.version_id]) == headnode.node_id);

	return headnode;
}

node_ref Document::GetHeadnode(const doc_id& di) const {
	wxASSERT(di.IsOk());

	const c4_View vRevs = (di.IsDraft()) ? m_catalyst.GetRevView() : m_catalyst.GetDocView();
	const c4_RowRef rDoc = vRevs[di.document_id];
	const c4_View vHis = pHistory(rDoc);
	const c4_RowRef rVer = vHis[di.version_id];

	return node_ref(pHeadVer(rVer), pHeadnode(rVer));
}

node_ref Document::GetPropnode() const {
	wxASSERT(m_docId.IsOk());
	wxASSERT(m_docId.version_id < vHistory.GetSize());
	const int id = m_docId.version_id;

	const c4_RowRef rVer = vHistory[id];
	return node_ref(pPropVer(rVer), pPropnode(rVer));
}

node_ref Document::GetPropnode(const doc_id& di) const {
	wxASSERT(di.IsOk());

	const c4_View vRevs = (di.IsDraft()) ? m_catalyst.GetRevView() : m_catalyst.GetDocView();
	const c4_RowRef rDoc = vRevs[di.document_id];
	const c4_View vHis = pHistory(rDoc);
	const c4_RowRef rVer = vHis[di.version_id];

	return node_ref(pPropVer(rVer), pPropnode(rVer));
}

void Document::UpdateHeadnode() {
	wxASSERT(m_docId.type == DRAFT);
	wxASSERT(pState(vHistory[m_docId.version_id]) == STATE_EDITABLE);

	// Check if the headnode has been updated
	const node_ref headnode = m_textData.GetHeadnode();
	const c4_RowRef rVersion = vHistory[m_docId.version_id];

	pHeadVer(rVersion) = headnode.version_id;
	pHeadnode(rVersion) = headnode.node_id;
}

void Document::UpdatePropnode(node_ref propnode) {
	wxASSERT(m_docId.IsDraft());
	wxASSERT(pState(vHistory[m_docId.version_id]) == STATE_EDITABLE);
	wxASSERT(m_catalyst.IsOk(m_docId, propnode));

	// Check if the headnode has been updated
	const c4_RowRef rRevision = vHistory[m_docId.version_id];
	if (pPropVer(rRevision) != propnode.version_id || pPropnode(rRevision) != propnode.node_id) {
		pPropVer(rRevision) = propnode.version_id;
		pPropnode(rRevision) = propnode.node_id;
	}
}

void Document::NewRevision() {
	if (m_docId.IsDocument()) {
		// We have to create a new draft to hold the changes
		const int new_draft_id = m_catalyst.NewDraft();
		c4_View vDrafts = m_catalyst.GetRevView();
		c4_RowRef rDraft = vDrafts[new_draft_id];

		// Set the parent
		pParentDoc(rDraft) = m_docId.document_id;
		pParentVer(rDraft) = m_docId.version_id;

		// Create the first revisions
		c4_View vDraftHistory = pHistory(rDraft);
		vDraftHistory.SetSize(2);
		c4_RowRef rNewRev = vDraftHistory[0];

		// Copy properties and headnode from parent document
		const c4_RowRef rParent = vHistory[m_docId.version_id];
		pLength(rNewRev)   = pLength(rParent);
		pPropVer(rNewRev)  = pPropVer(rParent);
		pPropnode(rNewRev) = pPropnode(rParent);
		pHeadVer(rNewRev)  = pHeadVer(rParent);
		pHeadnode(rNewRev) = pHeadnode(rParent);
		pState(rNewRev) = STATE_FROZEN;

		// Make an editable copy of the base revision
		// (parent is automatically initialized to 0(base))
		c4_RowRef rNewRev2 = vDraftHistory[1];
		rNewRev2 = rNewRev;
		pState(rNewRev2) = STATE_EDITABLE;

		// Add the new draft to parent documents list of children
		c4_View vChildren = pChildren(rParent);
		vChildren.Add(pType[DRAFT] + pChildref[new_draft_id]);

		SetDocument(doc_id(DRAFT, new_draft_id, 1));
	}
	else {
		if(vHistory.GetSize() == 0) {
			// First revision, we have to create the history entry
			vHistory.SetSize(1);
			m_docId.version_id = 0;
			const c4_RowRef rNewRev = vHistory[0];
			pPropVer(rNewRev)  = -1; // No properties
			pPropnode(rNewRev) = -1; // No properties

			// Create the headnode
			m_textData.SetDocument(m_docId, node_ref());
			m_textData.Create();
			const node_ref headnode = m_textData.GetHeadnode();
			pHeadVer(rNewRev)  = headnode.version_id;
			pHeadnode(rNewRev) = headnode.node_id;
		}
		else {
			// Set same headnode and length as current revision
			const int new_version_id = vHistory.Add(vHistory[m_docId.version_id]);
			const c4_RowRef newversion = vHistory[new_version_id];
			pParent(newversion) = m_docId.version_id;
			pState(newversion) = STATE_EDITABLE;
			m_docId.version_id = new_version_id;
			m_textData.SetDocument(m_docId, node_ref(pHeadVer(newversion), pHeadnode(newversion)));
		}
	}

	// Set Date
	wxDateTime now = wxDateTime::Now();
	pDate(vRevisions[m_docId.document_id]) = now.GetValue().GetValue();

	// The new revision becomes the head
	MakeHead();

	// Notify subscribers that we have created a new revision in this document
	const doc_id new_rev = GetDocument();
	if (do_notify) {
		m_catalyst.UnLock();
			dispatcher.Notify(wxT("DOC_NEWREVISION"), &new_rev, 0);
		m_catalyst.ReLock();
	}
}

// static notification handler
void Document::OnDocDeleted(Document* self, void* data, int WXUNUSED(filter)) {
	// NOTICE: This function is called from dispatcher
	//         so all invariants have to be locked before usage

	const doc_id* const di = (doc_id*)data;
	wxASSERT(di->IsDraft());

	if (self->m_docId.SameDoc(*di)) {
		self->m_docId.document_id = -1;
		self->m_docId.version_id = -1; // invalidate the document
	}
}

// - Debug functions ---

#ifdef __WXDEBUG__

int Document::NodeCount() const { return vNodes.GetSize(); }

void Document::Print() const {
	wxLogDebug(wxT("X Document: %d version: %d"), m_docId.document_id, m_docId.version_id);
	wxLogDebug(wxT("  versions: %d"), vHistory.GetSize());
	for(int i = 0; i < vHistory.GetSize(); ++i) wxLogDebug(wxT("    %d: len=%d"), i, (int)pLength(vHistory[i]));
	wxLogDebug(wxT("  nodes:    %d"), vNodes.GetSize());
	wxLogDebug(wxT("  length:   %d"), GetLength());
}

void Document::PrintAll() const {
	c4_ViewProp pNodes("nodes");
	c4_ViewProp pHistory("history");

	wxLogDebug(wxT("\nAll Documents"));
	for(int i = 0; i < vRevisions.GetSize(); ++i) {
		wxLogDebug(wxT("Document %d"), i);
		wxLogDebug(wxT("  versions: %d"), pHistory(vRevisions[i]).GetSize());
		c4_View vHis = pHistory(vRevisions[i]);
		for(int v = 0; v < pHistory(vRevisions[i]).GetSize(); ++v) {
			wxLogDebug(wxT("    %d: len=%d"), v, (int)pLength(vHis[v]));
		}
		wxLogDebug(wxT("  nodes:    %d"), pNodes(vRevisions[i]).GetSize());
	}
}

#endif  //__WXDEBUG__

DocumentWrapper::DocumentWrapper(CatalystWrapper& cw, bool createNew): m_doc(cw) {
	wxASSERT(createNew);
	if (createNew) {
		ISettings& settings = eGetSettings();
		RecursiveCriticalSectionLocker cx_lock(GetReadLock());
		m_doc.CreateNew(settings);
	}
}
