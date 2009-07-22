#ifndef __AUTOPAIRS_H__
#define __AUTOPAIRS_H__

#include "Interval.h"
#include <vector>

class AutoPairs {
public:
	void AdjustIntervalsUp(unsigned int bytes_inserted);
	void AdjustIntervalsDown(unsigned int bytes_removed);
	void AdjustEndsUp(unsigned int bytes_inserted);
	void AdjustEndsDown(unsigned int bytes_removed);

	void Clear();
	void ClearIfInsertingOutsideInnerPair(unsigned int pos);

	bool HasPairs() const;
	bool AtEndOfPair(unsigned int pos) const;
	bool BeforeOuterPair(unsigned int pos) const;
	bool ContainedInInnerPair(unsigned int pos) const;

	const interval& InnerPair() const;
	const interval& OuterPair() const;

	void DropInnerPair();
	void AddInnerPair(unsigned int pos);

	bool Enabled() const;
	void Enable(bool enable);

private:
	std::vector<interval> m_pairStack;
	bool m_doAutoPair;
};

#endif
