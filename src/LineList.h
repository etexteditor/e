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

#ifndef __LINELIST_H__
#define __LINELIST_H__

// STL can't compile with Level 4
#ifdef __WXMSW__
    #pragma warning(push, 1)
#endif
#include <vector>
#ifdef __WXMSW__
    #pragma warning(pop)
#endif
using namespace std;

// abstract base class
class LineList {
public:
    virtual ~LineList() {};

	virtual unsigned int offset(unsigned int index) = 0;
	virtual unsigned int end(unsigned int index) = 0;
	virtual unsigned int top(unsigned int index) = 0;
	virtual unsigned int bottom(unsigned int index) = 0;

	virtual unsigned int size() const = 0;
	virtual unsigned int last() const = 0;
	virtual unsigned int height() const = 0;
	virtual unsigned int length() const = 0;
	virtual unsigned int width() const = 0;

	virtual bool IsLineEnd(unsigned int pos) = 0;
	virtual unsigned int EndFromPos(unsigned int pos) = 0;
	virtual unsigned int StartFromPos(unsigned int pos) = 0;

	virtual vector<unsigned int>& GetOffsets() = 0;
	virtual void SetOffsets(const vector<unsigned int>& offsets) = 0;
	virtual void NewOffsets() = 0;

	virtual void insert(unsigned int index, int newend) = 0;
	virtual void insertlines(unsigned int index, vector<unsigned int>& newlines) = 0;
	virtual void update(unsigned int index, unsigned int newend) = 0;
	virtual void update_parsed_line(unsigned int index) = 0;
	virtual void update_line_extent(unsigned int index, unsigned int extent) = 0;
	virtual void remove(unsigned int start, unsigned int end) = 0;
	virtual void clear() = 0;

	virtual int find_offset(int offset) = 0;
	virtual int find_ypos(unsigned int ypos) = 0;

	virtual void invalidate(int index=0) = 0;
	virtual void widthchanged(int index=0) = 0;
	virtual int prepare(int) = 0;
	int prepare_all() {return prepare(-1);};
	virtual bool in_window(unsigned int) {return true;};

	virtual int OnIdle() = 0;
	virtual bool NeedIdle() const = 0;

	virtual void Print() = 0;
	virtual void verify(bool deep=false) const = 0;
};

#endif //__LINELIST_H__
