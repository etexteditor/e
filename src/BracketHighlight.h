#ifndef __BRACKETHIGHLIGHT_H__
#define __BRACKETHIGHLIGHT_H__

#include "Interval.h"

class BracketHighlight {
public:
	BracketHighlight();

	void Set(unsigned int start, unsigned int end);
	void Clear();

	bool IsEndPoint(const unsigned int pos) const;

	unsigned int OtherEndPoint(const unsigned int pos) const;

	const interval& GetInterval() const;
	bool HasInterval() const;

	bool HasOrderedInterval() const;

	bool UpdateIfChanged(unsigned int changeToken, unsigned int pos);

	unsigned int Start() const {return m_interval.start;};
	unsigned int End() const {return m_interval.end;};

private:
	interval m_interval; // Interval for the current bracket pair.
	unsigned int m_lastChangeToken;
	unsigned int m_lastPos;
};

#endif
