// Copyright (C) 2015 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#ifndef _WATERMARKS_CONTROLLER_H_
#define _WATERMARKS_CONTROLLER_H_

#include "region/Rect.h"
#include "region/Dimension.h"
#include "rfb/FrameBuffer.h"
#include "rfb/PixelFormat.h"
#include "rfb/StandardPixelFormatFactory.h"
#include "thread/LocalMutex.h"

class WatermarksController
{
public:
	WatermarksController(void);

	void setNewFbProperties(const Rect *rect, const PixelFormat *pf);
	

	void showWaterMarks(FrameBuffer *frameBuffer,
		LocalMutex *fbLock);

	void hideWatermarks(FrameBuffer *frameBuffer,
		LocalMutex *fbLock);

	const Rect CurrentRect();

private:
	Rect m_currentRect;
	Rect m_currentFrameBufferRect;

	void setNewPixelFormat(const PixelFormat *pf);

	void setNewFbSize(const Rect *rect);
	
	FrameBuffer m_frameBuffer;
	FrameBuffer& frameBuffer(bool fromFile = false);
	void loadFromfile();

	bool isEmpty();

	FrameBuffer m_overlay;

	int m_height;
	int m_width;
};

#endif _WATERMARKS_CONTROLLER_H_


