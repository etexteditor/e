#include "Macro.h"
#include "plistHandler.h"

void eMacroCmd::SaveTo(PListArray& arr) const {
	PListDict dict = arr.InsertNewDict(arr.GetSize());

	dict.wxSetString("command", m_cmd);

	if (m_argNames.empty()) return;
	PListArray args = dict.NewArray("arguments");
	for (size_t i = 0; i < m_argNames.size(); ++i) {
		const wxString& name = m_argNames[i];
		const wxVariant& arg = m_args[i];
		PListArray a = args.InsertNewArray(args.GetSize());

		a.AddString(name);
		if (arg.GetType() == wxT("bool")) a.AddBool(arg.GetBool());
		else if (arg.GetType() == wxT("long")) a.AddInt(arg.GetLong());
		else if (arg.GetType() == wxT("string")) a.AddString(arg.GetString());
		else wxASSERT(false);
	}
}

void eMacro::SaveTo(PListDict& dict) const {
	PListArray cmdArray = dict.NewArray("e_commands");

	for (boost::ptr_vector<eMacroCmd>::const_iterator p = m_cmds.begin(); p != m_cmds.end(); ++p) {
		p->SaveTo(cmdArray);
	}
}