#ifndef __DETECTTRIPLECLICKS_H__
#define __DETECTTRIPLECLICKS_H__

class wxStopWatch;

class DetectTripleClicks {
public:
	DetectTripleClicks();
	~DetectTripleClicks();

	void Reset();
	void Start(int doubleClickedLine);
	bool TripleClickedLine(int line_id);

private:
	int m_doubleClickedLine;
	wxStopWatch* m_timer;
};

#endif
