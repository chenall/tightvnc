#ifndef _UPDATE_REQUEST_SENDER_
#define _UPDATE_REQUEST_SENDER_

#include <thread/Thread.h>
#include <rfb/FrameBuffer.h>
#include <log-writer/LogWriter.h>
#include <network/RfbOutputGate.h>

class UpdateRequestSender : public Thread
{
public:
	UpdateRequestSender(Lockable* m_fb_lock, FrameBuffer* m_frame_buffer, LogWriter* m_log_writer);

	~UpdateRequestSender();

	void setWasUpdated();
	void setTimeout(int miliseconds);
	void setIsIncremental(bool isIncremental);
	void setOutput(RfbOutputGate* output);

	int getTimeout();

protected:
	virtual void execute() override;


private:
	void sendFbUpdateRequest();

	bool isUpdated();
	bool isIncremental();
	RfbOutputGate* getOutput();

	bool m_wasUpdateRecieved;
	LocalMutex m_wasUpdatedLock;

	int m_timeOut;
	LocalMutex m_timeOutLock;

	bool m_isIncrimental;
	LocalMutex m_isIncrimentalLock;

	Lockable *m_fbLock;
	FrameBuffer *m_frameBuffer;

	LogWriter *m_logWriter;
	
	RfbOutputGate *m_output;
	LocalMutex m_outputLock;
};

#endif //_UPDATE_REQUEST_SENDER_

