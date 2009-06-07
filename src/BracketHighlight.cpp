#include "BracketHighlight.h"

BracketHighlight::BracketHighlight(){}

void BracketHighlight::Set(unsigned int start, unsigned int end){ m_interval.Set(start, end);}
void BracketHighlight::Clear() { m_interval.start = m_interval.end = 0; }

bool BracketHighlight::IsEndPoint(const unsigned int pos) const { return (pos == m_interval.start) || (pos == m_interval.end); }

unsigned int BracketHighlight::OtherEndPoint(const unsigned int pos) const {
	if (pos == m_interval.start) return m_interval.end;
	if (pos == m_interval.end) return m_interval.start;

	// Not on an endpoint, but have to return something.
	return (unsigned int)-1;
}

const interval& BracketHighlight::GetInterval() const {return m_interval;}
bool BracketHighlight::HasInterval() const {return m_interval.start != m_interval.end; }

bool BracketHighlight::HasOrderedInterval() const {
	return m_interval.start < m_interval.end;
}

bool BracketHighlight::UpdateIfChanged(unsigned int changeToken, unsigned int pos) {
	// If no change to document or position, then stop.
	if (m_lastChangeToken == changeToken && m_lastPos == pos) return false;

	// Set new change token and position.
	m_lastChangeToken = changeToken;
	m_lastPos = pos;
	return true;
}
