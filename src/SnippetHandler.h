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

#ifndef __SNIPPETHANDLER_H__
#define __SNIPPETHANDLER_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".
#include "Catalyst.h"

// Pre-definitions
class EditorCtrl;
class cxEnv;
struct tmBundle;

class SnippetHandler {
public:
	SnippetHandler() : m_editor(NULL), m_snippet(NULL), m_env(NULL) {};

	bool IsActive() const {return m_snippet != NULL;};
	bool Validate();
	void Clear();
	void GotoEndAndClear();

	void StartSnippet(EditorCtrl* editor, const vector<char>& snippet, cxEnv& env, const tmBundle* bundle);

	void NextTab();
	void PrevTab();
	void Insert(const wxString& text);
	void Delete(unsigned int start, unsigned int end);

private:
	void ChangedTabstop(unsigned int tabstop, int diff, bool init=false);
	void UpdateVarTransforms();

	void AdjustIndentUnit();
	void AdjustIndent();
	void IndentString(wxString& text) const;

	struct Transformation {
		unsigned int iv;
		vector<char> regexp;
		wxString format;
		bool isGlobal;
	};

	struct TabInterval {
		int parent;
		unsigned int start;
		unsigned int end;
	};

	class TabStop {
	public:
		TabStop()
			: isValid(false) {}

		bool isValid;
		unsigned int iv;
		//unsigned int parent;
		vector<char> pipeCmd;
		vector<unsigned int> mirrors;
		vector<unsigned int> transforms;
		vector<unsigned int> children;   // contained tabstops
		vector<unsigned int> childrenTr; // contained transformations
		vector<unsigned int> childrenM;  // contained mirrors
	};

	wxString DoTransform(unsigned int start, unsigned int end, const Transformation& tr) const;
	void DoPipe(const TabStop& ts);

	bool Parse(bool isWrapped=false);
	bool ParseReplacement();
	bool ParseTabStop();
	bool ParseVariable();
	bool ParseTransform(Transformation& tr);

	void UpdateIntervals(unsigned int id, int diff);
	void UpdateIntervalsFromPos(unsigned int pos, int diff);
	void RemoveChildren(TabStop& ts);

	// Debug
	void Print();

	// Member variables
	EditorCtrl* m_editor;
	const vector<char>* m_snippet;
	vector<char> m_indentUnit;
	wxString m_indent;
	cxEnv* m_env;
	const tmBundle* m_bundle;

	map<unsigned int,TabStop> m_tabstops;
	vector<TabInterval> m_intervals;
	vector<Transformation> m_transforms;
	vector<Transformation> m_varTransforms;
	unsigned int m_endpos;
	unsigned int m_offset;
	unsigned int m_curTab;

	// Parser state
	unsigned int m_pos;
	//unsigned int m_len;
	unsigned int m_parentTabStop;
	int m_parentIv;
	bool m_endposSet;
	vector<char> m_snipText;
};

#endif // __TM_SYNTAXHANDLER_H__
