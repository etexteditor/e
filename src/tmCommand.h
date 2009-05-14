#ifndef __TMCOMMAND_H__
#define __TMCOMMAND_H__

#include "tmAction.h"

class tmCommand : public tmAction {
public:
	tmCommand()
	: save(csNONE), input(ciNONE), inputXml(false), fallbackInput(ciNONE), output(coNONE) {};
	virtual ~tmCommand() {};

	bool IsCommand() const {return true;};

	enum CmdSave {csNONE, csDOC, csALL};
	enum CmdInput {ciNONE, ciSEL, ciDOC, ciLINE, ciWORD, ciCHAR, ciSCOPE};
	enum CmdOutput {coNONE, coSEL, coDOC, coINSERT, coSNIPPET, coTOOLTIP, coHTML, coNEWDOC, coREPLACEDOC};

	CmdSave save;
	CmdInput input;
	bool inputXml;
	CmdInput fallbackInput;
	CmdOutput output;
};

#endif // __TMCOMMAND_H__
