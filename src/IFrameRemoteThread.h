#ifndef __IFRAMEREMOTETHREAD_H__
#define __IFRAMEREMOTETHREAD_H__

class RemoteThread;

class IFrameRemoteThread {
public:
	// For remote file access.
	virtual RemoteThread& GetRemoteThread() = 0;
};

#endif // __IFRAMEREMOTETHREAD_H__
