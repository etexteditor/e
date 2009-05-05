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

#ifndef __LINELISTWRAP_H__
#define __LINELISTWRAP_H__

#include "LineList.h"

class FixedLine;
class DocumentWrapper;

class LineListWrap : public LineList {
public:
	LineListWrap(FixedLine& l, const DocumentWrapper& dw);
	virtual ~LineListWrap() {};

	unsigned int offset(unsigned int index);
	unsigned int end(unsigned int index);
	unsigned int top(unsigned int index);
	unsigned int bottom(unsigned int index);

	unsigned int size() const;
	unsigned int last() const;
	unsigned int height() const;
	unsigned int length() const;
	unsigned int width() const;

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
	void widthchanged(int index=0) {invalidate(index);};
	int prepare(int ypos);
	bool in_window(unsigned int pos);

	int OnIdle();
	bool NeedIdle() const {return (firstLoadedPos > 0 || lastLoadedPos != size());};

	void Print();
	void verify(bool deep=false) const;

private:
	void add_line(int end);

	void validate_offsets(unsigned int index);
	void validate_positions(unsigned int index);

	void update_offsetDiff(unsigned int index, int diff);
	void update_posDiff(unsigned int index, int diff);

	void initwindow();
	int extendwindow(unsigned int index);
	int recalc_approx(bool movetop = false);

	// Safe unsigned minus (won't go below 0)
	unsigned int uMinus(unsigned int a, unsigned int b) {
		if (b > a) return 0;
		else return a - b;
	}

	// Safe unsigned plus signed (won't go below 0)
	unsigned int uPluss(unsigned int a, int b) {
		if (b < 0 && (unsigned int)-b > a) return 0;
		else return a + b;
	}

	// Constants
	static const unsigned int WINSIZE;

	// Private member variables
	FixedLine& line;
	const DocumentWrapper& m_doc;
	vector<unsigned int> yPositions;
	vector<unsigned int> textOffsets;

	unsigned int lastValidOffset;
	unsigned int lastValidPos;
	unsigned int firstLoadedPos;
	unsigned int lastLoadedPos;
	int offsetDiff;
	int posDiff;

	unsigned int approxTop;
	unsigned int approxBottom;
	unsigned int avr_lineheight;
	int approxDiff;
};

#endif // __LINELISTWRAP_H__
