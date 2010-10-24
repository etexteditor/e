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

#ifndef CELL_H
#define CELL_H

#include <wx/dc.h>
#include <wx/gdicmn.h>
#include "Document.h"

#include <vector>

// pre-declare
class CellRegion;

class Cell {
public:
	explicit Cell(const wxDC& dc);
	virtual ~Cell() {};
	virtual void Destroy();
	int GetWidth();
	void SetStyle(const wxColour& fc, const wxColour& bc);
	virtual void DrawCell(int xoffset, int yoffset, const wxRect& rect)=0;

protected:
	// Member variables
	const wxDC& dc;
	int width;
	int height;

	wxColour fcolor;
	wxColour bgcolor;
	int bgmode;
};

class WordCell : public Cell {
public:
	WordCell(const wxDC& dc, const wxString& wrd);
	virtual ~WordCell() {};
	void Destroy();
	void DrawCell(int xoffset, int yoffset, const wxRect& rect);

private:
	// Member variables
	wxString word;
};

class DiffLineCell : public Cell {
public:
	DiffLineCell(const wxDC& dc, const DocumentWrapper& dw);
	virtual ~DiffLineCell();
	void AddSubCell(Cell* cell);
	void DrawCell(int xoffset, int yoffset, const wxRect& rect);
	void CalcLayout(const doc_id& rev1, const doc_id& rev2);
	void CalcLayoutRange(const doc_id& rev1, const doc_id& rev2, const interval& range);
	int SetWidth(int w);

private:
	void CalcSubCells(const wxString& text, const wxColour& fc, const wxColour& bc);
	void Clear();

	// Member variables
	const DocumentWrapper& m_doc;
	int line_width;
	int char_width;
	std::vector<Cell*> subCells;
	std::vector<int> top_xpos;
	wxColour m_insertColour;
	wxColour m_deleteColour;
	wxColour m_hiddenColour;
};

class NewlineImageCell : public Cell {
public:
	explicit NewlineImageCell(const wxDC& dc);
	virtual ~NewlineImageCell() {};
	void DrawCell(int xoffset, int yoffset, const wxRect& rect);
};

class TabImageCell : public Cell {
public:
	explicit TabImageCell(const wxDC& dc);
    virtual ~TabImageCell() {};
	void DrawCell(int xoffset, int yoffset, const wxRect& rect);
};


#endif // CELL_H
