#ifndef __INTERVAL_H__
#define __INTERVAL_H__

class interval {
public:
	interval() : start((unsigned int)-1), end((unsigned int)-1) {}
	interval(unsigned int s, unsigned int e) : start(s), end(e) {}

	inline bool operator==(const interval& iv) const {return (start == iv.start && end == iv.end);}
	inline bool operator!=(const interval& iv) const {return (start != iv.start || end != iv.end);}

	inline bool empty() const { return start >= end; }

	inline void Set(unsigned int s, unsigned int e) {start = s; end = e;}
	inline void Get(unsigned int& s, unsigned int& e) const {s = start; e = end;}

	inline bool IsPoint(unsigned int p) const { return p == start && p == end; }

	unsigned int start;
	unsigned int end;
};

#endif
