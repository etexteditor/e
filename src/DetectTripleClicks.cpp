#include "DetectTripleClicks.h"
#include <wx/stopwatch.h>

DetectTripleClicks::DetectTripleClicks(): 
	m_doubleClickedLine(-1),
	m_timer(new wxStopWatch())
{}

DetectTripleClicks::~DetectTripleClicks() {
	delete m_timer;
}

void DetectTripleClicks::Reset() { 
	m_doubleClickedLine = -1;
	m_timer->Pause();
}

void DetectTripleClicks::Start(int doubleClickedLine){
	m_doubleClickedLine = doubleClickedLine;
	m_timer->Start();
}

bool DetectTripleClicks::TripleClickedLine(int line_id){
	return m_doubleClickedLine == line_id && m_timer->Time() < 250;
}
