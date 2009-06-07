#include "AutoPairs.h"

void AutoPairs::AdjustIntervalsUp(unsigned int bytes_inserted) {
	for (std::vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
		t->start += bytes_inserted;
		t->end += bytes_inserted;
	}
}

void AutoPairs::AdjustIntervalsDown(unsigned int bytes_removed) {
	for (std::vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
		t->start -= bytes_removed;
		t->end -= bytes_removed;
	}
}

void AutoPairs::AdjustEndsUp(unsigned int bytes_inserted){
	for (std::vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
		t->end += bytes_inserted;
	}
}

void AutoPairs::AdjustEndsDown(unsigned int bytes_removed) {
	for (std::vector<interval>::iterator t = m_pairStack.begin(); t != m_pairStack.end(); ++t) {
		t->end -= bytes_removed;
	}
}

void AutoPairs::Clear() { m_pairStack.clear(); }

void AutoPairs::ClearIfInsertingOutsideInnerPair(unsigned int pos) {
	// Reset autoPair state if inserting outside inner pair
	if (HasPairs() && pos == InnerPair().end) Clear();
}

bool AutoPairs::HasPairs() const { return !m_pairStack.empty(); }

bool AutoPairs::AtEndOfPair(unsigned int pos) const {
	for (std::vector<interval>::const_iterator p = m_pairStack.begin(); p != m_pairStack.end(); ++p) {
		if (p->end == pos) return true;
	}
	return false;
}

bool AutoPairs::BeforeOuterPair(unsigned int pos) const {
	return HasPairs() && pos < OuterPair().start;
}

const interval& AutoPairs::InnerPair() const { return m_pairStack.back(); }
const interval& AutoPairs::OuterPair() const { return m_pairStack[0]; }

void AutoPairs::DropInnerPair() { m_pairStack.pop_back(); }
void AutoPairs:: AddInnerPair(unsigned int pos) { m_pairStack.push_back(interval(pos, pos)); }

bool AutoPairs::Enabled() const { return m_doAutoPair; }
void AutoPairs::Enable(bool enable) { m_doAutoPair = enable; }
