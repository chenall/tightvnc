#include "UpdateRequestSender.h"
#include <thread/AutoLock.h>
#include "RfbFramebufferUpdateRequestClientMessage.h"

UpdateRequestSender::UpdateRequestSender(Lockable* m_fb_lock, FrameBuffer* m_frame_buffer, LogWriter* m_log_writer):
	m_wasUpdateRecieved(false),
	m_timeOut(0),
	m_isIncrimental(true),
    m_fbLock(m_fb_lock),
    m_frameBuffer(m_frame_buffer),
    m_logWriter(m_log_writer),
    m_output(0)
{
	
}

UpdateRequestSender::~UpdateRequestSender()
{
	try
	{
		terminate();
		wait();
	}
	catch (...)
	{
	}
}

void UpdateRequestSender::setWasUpdated()
{
	AutoLock al(&m_wasUpdatedLock);
	m_wasUpdateRecieved = true;
}

void UpdateRequestSender::setTimeout(int miliseconds)
{
	AutoLock al(&m_timeOutLock);
	m_timeOut = miliseconds;
}

void UpdateRequestSender::setIsIncremental(bool isIncremental)
{
	AutoLock al(&m_isIncrimentalLock);
	m_isIncrimental = isIncremental;
}

void UpdateRequestSender::setOutput(RfbOutputGate* output)
{
	{
		AutoLock al(&m_outputLock);
		m_output = output;
	}

	// Start thread.
	resume();
	m_logWriter->debug(_T("UpdateRequestServer is started"));
}

void UpdateRequestSender::execute()
{
	try
	{
		while(!isTerminating())
		{
			if (isUpdated())
			{
				sendFbUpdateRequest();
			}

			int timeout = getTimeout();
			timeout = timeout > 0 ? timeout : 500;
			//todo: check if it is best approoach?
			Thread::sleep(timeout);
		}
	}
	catch(const Exception &ex)
	{
		m_logWriter->message(_T("UpdateRequestSender. Exception: %s"), ex.getMessage());
	}
	catch(...)
	{
		m_logWriter->error(_T("UpdateRequestSender. Unknow error has occured."));
	}
}

void UpdateRequestSender::sendFbUpdateRequest()
{
	RfbOutputGate* output = getOutput();

	if(output == 0)
		return;

	Rect updateRect;
	{
		AutoLock al(m_fbLock);
		updateRect = m_frameBuffer->getDimension().getRect();
	}

	bool isIncremental = this->isIncremental();

	if (isIncremental)
	{
		m_logWriter->debug(_T("Sending frame buffer incremental update request [%dx%d]..."),
                      updateRect.getWidth(), updateRect.getHeight());
	}
	else
	{
		m_logWriter->debug(_T("Sending frame buffer full update request [%dx%d]..."),
                      updateRect.getWidth(), updateRect.getHeight());
	}

	RfbFramebufferUpdateRequestClientMessage fbUpdReq(isIncremental, updateRect);
	fbUpdReq.send(m_output);
	m_logWriter->debug(_T("Frame buffer update request is sent"));
}

bool UpdateRequestSender::isUpdated()
{
	AutoLock al(&m_wasUpdatedLock);
	bool result = m_wasUpdateRecieved;
	m_wasUpdateRecieved = false;
	return result;
}

int UpdateRequestSender::getTimeout()
{
	AutoLock al(&m_timeOutLock);
	return m_timeOut;
}

bool UpdateRequestSender::isIncremental()
{
	AutoLock al(&m_isIncrimentalLock);
	return m_isIncrimental;
}

RfbOutputGate* UpdateRequestSender::getOutput()
{
	AutoLock al(&m_outputLock);
	return m_output;
}