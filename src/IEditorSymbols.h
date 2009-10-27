#ifndef __IEDITORSYMBOLS_H__
#define __IEDITORSYMBOLS_H__

#include "SymbolRef.h"
#include "EditorChangeState.h"
#include <vector>

class IEditorSymbols : public IGetChangeState {
public:
	virtual int GetSymbols(std::vector<SymbolRef>& symbols) const = 0;
	virtual wxString GetSymbolString(const SymbolRef& sr) const = 0;
	virtual void GotoSymbolPos(unsigned int pos) = 0;
};

#endif // __IEDITORSYMBOLS_H__
