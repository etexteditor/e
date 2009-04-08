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

#ifndef __DATATEXT_H__
#define __DATATEXT_H__

#include "Catalyst.h"

// pre-declarations
class cDataText;

class DataText {
public:
	DataText(const Catalyst& cat, const doc_id& di, const node_ref& headnode);
	DataText(const Catalyst& cat, const doc_id& di);
	DataText(const Catalyst& cat);
	~DataText();

	// Document
	void Create();
	void SetDocument(const doc_id& di, const node_ref& headnode);
	bool IsOk() const;
	void Invalidate();
	void Close();
	const cDataText* GetData() const;

	// Length & Positions
	wxULongLong_t GetLength() const;

	// Text Retrieval
	wxString GetText() const;
	unsigned int GetLine(unsigned int start_pos, vector<char>& buffer) const;
	void GetTextPart(int start_pos, int end_pos, wxString& text) const;
	void GetTextPart(int start_pos, int end_pos, unsigned char* buffer) const;
	void GetNodeTextPart(unsigned char* text, const node_ref& nr, int start_pos, int end_pos) const;
	wxChar GetChar(unsigned int pos) const;
	void WriteText(wxOutputStream& stream) const;

	// Modification
	unsigned int Insert(int pos, const wxString& text);
	unsigned int Insert(int pos, const char* text);
	void Delete(int start_pos, int end_pos);
	int DeleteChar(unsigned int pos, bool nextchar=true);
	void Move(int source_startpos, int source_endpos, int dest_pos);
	void Clear();
	void SetToFile(int offset, int length);
	void Freeze();

	// Length & Positions;
	unsigned int GetLengthInChars(unsigned int start_pos, unsigned int end_pos) const;
	unsigned int GetCharPos(unsigned int offset, int char_count) const;
	unsigned int GetCharLength(unsigned int pos) const;
	unsigned int GetNextCharPos(unsigned int pos) const;
	unsigned int GetPrevCharPos(unsigned int pos) const;
	unsigned int GetValidCharPos(unsigned int pos) const;
	unsigned int GetLineStart(unsigned int pos) const;
	unsigned int GetLineEnd(unsigned int pos) const;

	// Consolidation
	node_ref Consolidate(const doc_id& di);

	// Diff
	cxNodeInfo GetNodeInfo(unsigned int pos) const;
	vector<match> Diff(const doc_id& version2, const node_ref& headnode2) const;
	void PartialDiff(interval range, vector<cxDiffEntry>& rangeHistory);

	// Nodes
	node_ref GetHeadnode() const;

	// File mapping
	void ClearBuffCache();

private:
	cDataText* m_dt;
};

#endif // __CDATATEXT_H__

