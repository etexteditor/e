#ifndef __IEXECUTEAPPCOMMAND_H__
#define __IEXECUTEAPPCOMMAND_H__

class wxString;

class IExecuteAppCommand {
public:
	// Execute a command, ignoring any output.
	virtual bool ExecuteCmd(const wxString& cmd) = 0;
	// Execute a command, capturing the output as `result`.
	virtual bool ExecuteCmd(const wxString& cmd, wxString& result) = 0;
};

#endif // __IEXECUTEAPPCOMMAND_H__
