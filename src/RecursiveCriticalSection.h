#ifndef __RECURSIVECRITICALSECTION_H__
#define __RECURSIVECRITICALSECTION_H__

#include "wx/wxprec.h" // For compilers that support precompilation, includes "wx/wx.h".

class RecursiveCriticalSection : public wxCriticalSection {
#ifndef __WXMSW__ // Windows version is already recursive-aware
public:
	inline RecursiveCriticalSection() : m_recursive_mutex(wxMUTEX_RECURSIVE) {}
	inline ~RecursiveCriticalSection() {}

	inline void Enter()
	{
		m_recursive_mutex.Lock();
	}
	
	inline void Leave()
	{
		m_recursive_mutex.Unlock();
	}

private:
    wxMutex m_recursive_mutex;

    DECLARE_NO_COPY_CLASS(RecursiveCriticalSection)
#endif
};

class RecursiveCriticalSectionLocker {
public:
	RecursiveCriticalSectionLocker(RecursiveCriticalSection &cs) : m_recursive_critsect(cs)
	{
		m_recursive_critsect.Enter();
	}
	~RecursiveCriticalSectionLocker()
	{
		m_recursive_critsect.Leave();
	}
private:
    RecursiveCriticalSection& m_recursive_critsect;

    DECLARE_NO_COPY_CLASS(RecursiveCriticalSectionLocker)
};

#endif
