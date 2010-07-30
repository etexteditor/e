/*******************************************************************************
 *
 * Copyright (C) 2010, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __EMACRO_H__
#define __EMACRO_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <boost/ptr_container/ptr_vector.hpp>
#include <vector>

// Pre-declarations
class PListDict;
class PListArray;

class eMacroArg {
public:
	eMacroArg(const wxString& name, bool value) : m_name(name), m_value(value) {};
	eMacroArg(const wxString& name, int value) : m_name(name), m_value(value) {};
	eMacroArg(const wxString& name, const wxString& value) : m_name(name), m_value(value) {};

	const wxString& GetName() const {return m_name;};
	const wxVariant& GetValue() const {return m_value;};

private:
	wxString m_name;
	wxVariant m_value;
};

class eMacroCmd {
public:
	eMacroCmd(const wxString& cmd) : m_cmd(cmd) {};

	const wxString& GetName() const {return m_cmd;};
	size_t GetArgCount() const {return m_args.size();};
	const wxString& GetArgName(size_t ndx) const {return (ndx < m_argNames.size()) ? m_argNames[ndx] : wxEmptyString;};
	const wxVariant& GetArg(size_t ndx) const {return m_args[ndx];};
	wxVariant& GetArg(size_t ndx) {return m_args[ndx];};
	const std::vector<wxVariant>& GetArgs() const {return m_args;};

	bool GetArgBool(size_t ndx, bool default=false) const {
		if (ndx >= m_args.size()) return default;
		const wxVariant& value = m_args[ndx];
		if (!value.IsType(_("bool"))) return default;
		return value.GetBool();
	}

	int GetArgInt(size_t ndx, int default=0) const {
		if (ndx >= m_args.size()) return default;
		const wxVariant& value = m_args[ndx];
		if (!value.IsType(_("long"))) return default;
		return value.GetLong();
	}

	wxString GetArgString(size_t ndx, const wxString& default=wxEmptyString) const {
		if (ndx >= m_args.size()) return default;
		const wxVariant& value = m_args[ndx];
		if (!value.IsType(_("string"))) return default;
		return value.GetString();
	}

	void ExtendString(size_t ndx, const wxChar& c) {
		if (ndx >= m_args.size()) {wxASSERT(false); return;}
		wxVariant& value = m_args[ndx];
		if (!value.IsType(_("string"))) {wxASSERT(false); return;}
		wxString str = value.GetString();
		str += c;
		value = str;
	}

	template<class T> void AddArg(T value) {m_args.push_back(wxVariant(value));};
	template<class T> void AddArg(const wxString& name, T value) {m_argNames.push_back(name); m_args.push_back(wxVariant(value));};

	template<class T> void SetArg(size_t ndx, const wxString& name, T value) {
		m_argNames.resize(ndx+1);
		m_args.resize(ndx+1);
		m_argNames[ndx] = name;
		m_args[ndx] = value;
	};

	void SaveTo(PListArray& arr) const;

private:
	wxString m_cmd;
	std::vector<wxString> m_argNames;
	std::vector<wxVariant> m_args;
};

class eMacro {
public:
	eMacro() : m_record(false), m_isModified(false) {};

	bool IsRecording() const {return m_record;};
	void StartRecording() {m_record = true;};
	void EndRecording() {m_record = false;};
	void ToogleRecording() {m_record = !m_record;};
	void Clear() {m_isModified = true; m_cmds.clear();};

	bool IsModified() const {return m_isModified;};
	void ResetMod() {m_isModified = false;};

	bool IsEmpty() const {return m_cmds.empty();};
	size_t GetCount() const {return m_cmds.size();};

	void Delete(size_t ndx) {m_isModified = true; m_cmds.erase(m_cmds.begin()+ndx);};
	void MoveUp(size_t ndx) {m_isModified = true; std::swap(*(m_cmds.begin()+ndx-1).base(),*(m_cmds.begin()+ndx).base());};
	void MoveDown(size_t ndx) {m_isModified = true; std::swap(*(m_cmds.begin()+ndx).base(),*(m_cmds.begin()+ndx+1).base());};


	const eMacroCmd& GetCommand(size_t ndx) const {return m_cmds[ndx];};
	eMacroCmd& GetCommand(size_t ndx) {m_isModified = true; return m_cmds[ndx];};
	const eMacroCmd& Last() const {m_cmds.back();};
	eMacroCmd& Last() {m_isModified = true; return m_cmds.back();};

	eMacroCmd& Add(const wxString& cmd) {
		wxLogDebug(wxT("Adding macro: %s"), cmd);
		m_cmds.push_back(new eMacroCmd(cmd));
		m_isModified = true;
		return m_cmds.back();
	};

	template<class T> eMacroCmd& Add(const wxString& cmd, const wxString& arg, T value) {
		wxLogDebug(wxT("Adding macro: %s"), cmd);
		m_cmds.push_back(new eMacroCmd(cmd));
		m_cmds.back().AddArg(arg, value);
		m_isModified = true;
		return m_cmds.back();
	};

	void SaveTo(PListDict& dict) const;


private:
	bool m_record;
	bool m_isModified;
	boost::ptr_vector<eMacroCmd> m_cmds;
};

#endif //__EMACRO_H__