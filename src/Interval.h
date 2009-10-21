#ifndef __INTERVAL_H__
#define __INTERVAL_H__

class interval {
public:
	interval() : start((unsigned int)-1), end((unsigned int)-1) {};
	interval(unsigned int s, unsigned int e) : start(s), end(e) {};
	bool operator==(const interval& iv) const {return (start == iv.start && end == iv.end);};
	bool operator!=(const interval& iv) const {return (start != iv.start || end != iv.end);};
	bool empty() const { return start >= end; };
	void Set(unsigned int s, unsigned int e) {start = s; end = e;};
	inline void Get(unsigned int& s, unsigned int& e) const {s = start; e = end;};
	unsigned int start;
	unsigned int end;
};

#endif
