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

#ifndef __LINELISTNOWRAP_H__
#define __LINELISTNOWRAP_H__

#include "LineList.h"

class FixedLine;
class DocumentWrapper;

class LineListNoWrap : public LineList {
public:
	LineListNoWrap(FixedLine& l, const DocumentWrapper& dw);
	virtual ~LineListNoWrap() {};

	unsigned int offset(unsigned int index);
	unsigned int end(unsigned int index);
	unsigned int top(unsigned int index);
	unsigned int bottom(unsigned int index);

	unsigned int size() const;
	unsigned int last() const;
	unsigned int height() const;
	unsigned int length() const;
	unsigned int width() const {return m_maxWidth;};

	bool IsLineEnd(unsigned int pos);
	unsigned int EndFromPos(unsigned int pos);
	unsigned int StartFromPos(unsigned int pos);

	vector<unsigned int>& GetOffsets();
	void SetOffsets(const vector<unsigned int>& offsets);
	void NewOffsets();

	void insert(unsigned int index, int newend);
	void insertlines(unsigned int index, vector<unsigned int>& newlines);
	void update(unsigned int index, unsigned int newend);
	void update_parsed_line(unsigned int index);
	void update_line_extent(unsigned int index, unsigned int extent);
	void remove(unsigned int start, unsigned int end);
	void clear();

	int find_offset(int offset);
	int find_ypos(unsigned int ypos);

	void invalidate(int index=0);
	void widthchanged(int) {};
	int prepare(int) {return 0;};

	int OnIdle() {return 0;};
	bool NeedIdle() const {return false;};

	void Print();
	void verify(bool deep=false) const;

private:
	bool IsValidIndex(unsigned int index) const;

	// Member variables
	FixedLine& m_line;
	const DocumentWrapper& m_doc;
	unsigned int m_maxWidth;
	vector<unsigned int> m_textOffsets;
	vector<unsigned int> m_lineWidths;
};

#endif //__LINELISTNOWRAP_H__
