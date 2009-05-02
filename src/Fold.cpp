#include "Fold.h"

cxFold::cxFold(unsigned int line, cxFoldType type, unsigned int indent)
	: line_id(line), type(type), count(0), indent(indent) {}
