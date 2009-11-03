#ifndef __RANKEDITEM_H__
#define __RANKEDITEM_H__

#include <vector>

// T must define "operator <"
template <class T> class RankedItem {
public:
	RankedItem() : data(NULL), rank(0) {};
	RankedItem(const T* a, const std::vector<unsigned int>& hl):
		data(a), hlChars(hl)
	{
		// Calculate rank (total distance between chars)
		this->rank = 0;
		if (hlChars.size() <= 1) return;

		unsigned int prev = hlChars[0]+1;
		for (std::vector<unsigned int>::const_iterator p = hlChars.begin()+1; p != hlChars.end(); ++p) {
			this->rank += *p - prev;
			prev = *p+1;
		}
	};

	bool operator<(const RankedItem& other) const {
		if (rank != other.rank) return rank < other.rank;
		if (data && other.data) return data < other.data;
		return false;
	};

	void swap(RankedItem& other){
		const T* const temp_data = data;
		data = other.data;
		other.data = temp_data;

		const unsigned int temp_rank = rank;
		rank = other.rank;
		other.rank = temp_rank;

		hlChars.swap(other.hlChars);
	};

	const T* data;
	std::vector<unsigned int> hlChars;
	unsigned int rank;
};

#endif
