#ifndef __FINDFLAGS_H__
#define __FINDFLAGS_H__

// Define option bits for Find
static const unsigned int FIND_MATCHCASE = 1;
static const unsigned int FIND_USE_REGEX = 2;
static const unsigned int FIND_RESTART   = 4;
static const unsigned int FIND_HIGHLIGHT = 8;
static const unsigned int FIND_REVERSE   = 16;

enum cxFindResult {
	cxFOUND,
	cxFOUND_AFTER_RESTART,
	cxNOT_FOUND
};

enum cxCase {
	cxUPPERCASE,
	cxLOWERCASE,
	cxTITLECASE,
	cxREVERSECASE
};

#endif // __FINDFLAGS_H__
