// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#include "FTMessage.h"

const char FTMessage::COMPRESSION_SUPPORT_REQUEST_SIG[] = "FTCCSRST";
const char FTMessage::COMPRESSION_SUPPORT_REPLY_SIG[]   = "FTSCSRLY";
const char FTMessage::FILE_LIST_REQUEST_SIG[]           = "FTCFLRST";
const char FTMessage::FILE_LIST_REPLY_SIG[]             = "FTSFLRLY";
const char FTMessage::MD5_REQUEST_SIG[]                 = "FTCM5RST";
const char FTMessage::MD5_REPLY_SIG[]                   = "FTSM5RLY";
const char FTMessage::UPLOAD_START_REQUEST_SIG[]        = "FTCFURST";
const char FTMessage::UPLOAD_START_REPLY_SIG[]          = "FTSFURLY";
const char FTMessage::UPLOAD_DATA_REQUEST_SIG[]         = "FTCUDRST";
const char FTMessage::UPLOAD_DATA_REPLY_SIG[]           = "FTSUDRLY";
const char FTMessage::UPLOAD_END_REQUEST_SIG[]          = "FTCUERST";
const char FTMessage::UPLOAD_END_REPLY_SIG[]            = "FTSUERLY";
const char FTMessage::DOWNLOAD_START_REQUEST_SIG[]      = "FTCFDRST";
const char FTMessage::DOWNLOAD_START_REPLY_SIG[]        = "FTSFDRLY";
const char FTMessage::DOWNLOAD_DATA_REQUEST_SIG[]       = "FTCDDRST";
const char FTMessage::DOWNLOAD_DATA_REPLY_SIG[]         = "FTSDDRLY";
const char FTMessage::DOWNLOAD_END_REPLY_SIG[]          = "FTSDERLY";
const char FTMessage::MKDIR_REQUEST_SIG[]               = "FTCMDRST";
const char FTMessage::MKDIR_REPLY_SIG[]                 = "FTSMDRLY";
const char FTMessage::REMOVE_REQUEST_SIG[]              = "FTCFRRST";
const char FTMessage::REMOVE_REPLY_SIG[]                = "FTSFTRLY";
const char FTMessage::RENAME_REQUEST_SIG[]              = "FTCFMRST";
const char FTMessage::RENAME_REPLY_SIG[]                = "FTSFMRLY";
const char FTMessage::DIRSIZE_REQUEST_SIG[]             = "FTCDSRST";
const char FTMessage::DIRSIZE_REPLY_SIG[]               = "FTSDSRLY";
const char FTMessage::LAST_REQUEST_FAILED_REPLY_SIG[]   = "FTLRFRLY";
