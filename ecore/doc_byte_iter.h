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

#ifndef __DOCBYTEITER_H__
#define __DOCBYTEITER_H__

// Predefinitions
class Document;
class cData;
class c4_Row;

class doc_byte_iter {
public:
	doc_byte_iter() throw();
	doc_byte_iter(const Document& doc, const int ndx = 0) throw();
	doc_byte_iter(const cData& cdata, const int ndx = 0) throw();

	doc_byte_iter& operator=(const doc_byte_iter& dbi) throw();
	doc_byte_iter& operator=(void*) {m_seg_ptr = 0; return *this;}; // can only be used to set null pointer

	int GetIndex() const throw() {return m_index;};
	void SetIndex(int ndx) throw();
	void RefreshIndex(const int ndx) throw();
	int GetSegEnd() const throw() {return m_seg_end;};

	int compare(const unsigned char* string, int length) throw();

	bool operator==(void* aptr) const throw() {return m_seg_ptr == (const unsigned char*)aptr;};
	bool operator<(int ndx) const throw() {return m_index < ndx;};
	bool operator==(const doc_byte_iter& dbi) const throw() {return m_index == dbi.m_index;};
	bool operator!=(const doc_byte_iter& dbi) const throw() {return m_index != dbi.m_index;};
	bool operator>=(const doc_byte_iter& dbi) const throw() {return m_index >= dbi.m_index;};
	bool operator<=(const doc_byte_iter& dbi) const throw() {return m_index <= dbi.m_index;};
	bool operator>(const doc_byte_iter& dbi) const throw() {return m_index > dbi.m_index;};
	bool operator<(const doc_byte_iter& dbi) const throw() {return m_index < dbi.m_index;};

	const unsigned char& operator*() const throw() {return *m_seg_ptr;};
	const unsigned char& operator[](int offset) const throw();

	doc_byte_iter operator+(int offset) const throw() {doc_byte_iter dbi(*this);dbi.SetIndex(m_index+offset);return dbi;};
	int operator+(doc_byte_iter& dbi) const throw() {return m_index + dbi.m_index;};
	doc_byte_iter operator-(int offset) const throw() {doc_byte_iter dbi(*this);dbi.SetIndex(m_index-offset);return dbi;};
	int operator-(doc_byte_iter& dbi) const throw() {return m_index - dbi.m_index;};

	doc_byte_iter& operator++()   throw() {SetIndex(m_index+1);return *this;};   // prefix
	doc_byte_iter operator++(int) throw() {doc_byte_iter dbi(*this);SetIndex(m_index+1);return dbi;}; // postfix
	doc_byte_iter& operator--()	  throw() {SetIndex(m_index-1);return *this;};   // prefix
	doc_byte_iter operator--(int) throw() {doc_byte_iter dbi(*this);SetIndex(m_index-1);return dbi;}; // postfix
	doc_byte_iter& operator-=(int offset) throw() {SetIndex(m_index-offset); return *this;};
	doc_byte_iter& operator+=(int offset) throw() {SetIndex(m_index+offset); return *this;};

private:
	//friend class doc_byte_iter;

	// Private member variables
	const cData* m_document;
	int m_index;

	int m_seg_start;
	int m_seg_end;
	const unsigned char* m_seg_ptr;
	static c4_Row s_rTempRow;

	// Constant variables
	static const unsigned char m_endpoint; // this is where m_seg_ptr points when outside range
};

#endif // __DOCBYTEITER_H__
