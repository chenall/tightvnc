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

#ifndef _FT_MESSAGE_TYPE_H_
#define _FT_MESSAGE_TYPE_H_

#include "util/inttypes.h"

/**
 * File transfer rfb protocol extension, that used to transfer files
 * and browse file system on remote computer.
 *
 * -----------------------------------------------------------------------------
 *
 * All of file tranfser proto messages are separated to:
 *  1) Client to server messages.
 *  2) Server to client messages.
 *
 * Client to server messages have following format:
 *  UINT32 messageId 32 bit message id.
 *  UINT8[] messageBody message body depends on messageId.
 * @remark rfb dispatcher in common case read first byte (UINT8) as message id,
 * but if first byte is equal to 0xFC then it's TightVNC extension message and
 * must read next 3 bytes and create UINT32 message id for processing.
 *
 * Server to client message have following format:
 *  UINT32 message id meaning like in client->server messages.
 *  UINT8[] messageBody meaning like in client->server messages.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * Structures used in file transfer proto:
 *
 *  struct StringUTF8 { string encoded in UTF8 encoding.
 *    UINT32 size;        // size of encoded string buffer in bytes;
 *    UINT8 buffer[size]; // buffer with encoded string data.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * File names in file transfer proto:
 *
 * File transfer sends and recives file names in unix-like format (sample: "/C:/folder1/folder2",
 * "/etc/f") and uses "/" path separator beggining with "/" root folder.
 *
 * Folders are stored without last slash (sample: path to disk C on win is "/C:",
 * to /etc/ on unix is "/etc").
 *
 * @remark on windows OS "/" path contains disk list, on unix "/" means real unix root folder.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * Files modification time is stored as count of milliseconds single unix epoch.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * File flags is UINT8 that can be conbination of following values: 0x1 - file is
 * direction, 0x2 - executable file.
 *
 * Flags 0x1 and 0x2 cannot be set together.
 *
 * -----------------------------------------------------------------------------
 *
 * @readme
 * Compression support.
 *
 * Server can support zlib data compressing to minimaze traffic, information about it
 * can be send to client though COMPRESSION_SUPPORT_REPLY message.
 *
 * Some of client to server message has compressionLevel field, meaning of this field
 * is just prefence of client to server to compress some of reply data for request, server
 * can but mustn't compress data (server decide to compress data or not if compressLevel is not zero,
 * but if requested compression level is zero, than server will not use compression cause it
 * thinks that client does not support compression).
 *
 * Data that can be compressed marked as "CompressedBlock:" in messages description.
 * "CompressedBlock:" has several mandatory fields:
 *   UINT8 compressionLevel compression level (0 - not compressed, [1..9] compressed).
 *   UINT32 compressedSize size of compressed data.
 *   UINT32 uncompressedSize size of uncompressed data.
 *   UINT8 compressedData[compressedSize] compressed usefull data.
 * @note that if compressionLevel is zero than compressedSize is equals to uncompressedSize and
 * compressedData is not compressed.
 *
 * -----------------------------------------------------------------------------
 *
 * @note all constants in this class which has _REQUEST prefix is client to server messages,
 * _REPLY prefix - server to client messages.
 */
class FTMessage {
public:
  const static char COMPRESSION_SUPPORT_REQUEST_SIG[];
  const static char COMPRESSION_SUPPORT_REPLY_SIG[];
  /**
   * Checks if data compression (zlib) is supported on server side.
   *
   * @body has no body.
   *
   * @reply COMPRESSION_SUPPORT_REQUEST message.
   */
  const static UINT32 COMPRESSION_SUPPORT_REQUEST = 0xFC000100;
  /**
   * Reply to COMPRESSION_SUPPORT_REQUEST message.
   *
   * @body:
   *   UINT8 isCompressionSupported - 1 if compression supported, 0 otherwise.
   */
  const static UINT32 COMPRESSION_SUPPORT_REPLY = 0xFC000101;

  const static char FILE_LIST_REQUEST_SIG[];
  const static char FILE_LIST_REPLY_SIG[];
  /**
   * Get file list of specified folder on remote computer.
   *
   * @body:
   *  UINT8 compressionLevel preffered compression level.
   *  StringUTF8 pathToFolder absolute path to folder, file list of that needs to get.
   *
   * @reply FILE_LIST_REPLY on success, LAST_REQUEST_FAILED_REPLY on fail.
   *
   * @see readme tags of FTMessage class.
   */
  const static UINT32 FILE_LIST_REQUEST = 0xFC000102;
  /*
   * Success reply to FILE_LIST_REQUEST message.
   *
   * @body:
   *  @compressedBlock:
   *    UINT32 filesCount count of files.
   *    struct {
   *      UINT64 fileSize file size in bytes (0 if directory).
   *      UINT64 modTime file last modification time.
   *      UINT8 fileFlags flags of file.
   *      StringUTF8 relFilename filename relative to parent folder without first "/" (sample "test.txt",
   *                             but not "/test.txt" and not "/a/test.txt")
   *    } fileInfo[filesCount] array of structures containing files info.
   * @see readme tags of FTMessage class.
   */
  const static UINT32 FILE_LIST_REPLY = 0xFC000103;

  const static char MD5_REQUEST_SIG[];
  const static char MD5_REPLY_SIG[];
  /**
   * Request for MD5 sum of file chunk.
   *
   * @body
   *   StringUTF pathToFile absolute path to file.
   *   UINT64 offset begin offset of file chunk in bytes.
   *   UINT64 dataSize size of data in bytes.
   *
   * @reply MD5_REPLY on success, LAST_REQUEST_FAILED_REPLY on fail.
   */
  const static UINT32 MD5_REQUEST = 0xFC000104;
  /**
   * Reply for MD5_REQUEST message.
   *
   * @body
   *   UINT8 md5[16] md5 hash of requested file chunk.
   */
  const static UINT32 MD5_REPLY = 0xFC000105;

  const static char UPLOAD_START_REQUEST_SIG[];
  const static char UPLOAD_START_REPLY_SIG[];
  const static UINT32 UPLOAD_START_REQUEST = 0xFC000106;
  const static UINT32 UPLOAD_START_REPLY = 0xFC000107;

  const static char UPLOAD_DATA_REQUEST_SIG[];
  const static char UPLOAD_DATA_REPLY_SIG[];
  const static UINT32 UPLOAD_DATA_REQUEST = 0xFC000108;
  const static UINT32 UPLOAD_DATA_REPLY = 0xFC000109;

  const static char UPLOAD_END_REQUEST_SIG[];
  const static char UPLOAD_END_REPLY_SIG[];
  const static UINT32 UPLOAD_END_REQUEST = 0xFC00010A;
  const static UINT32 UPLOAD_END_REPLY = 0xFC00010B;

  const static char DOWNLOAD_START_REQUEST_SIG[];
  const static char DOWNLOAD_START_REPLY_SIG[];
  const static UINT32 DOWNLOAD_START_REQUEST = 0xFC00010C;
  const static UINT32 DOWNLOAD_START_REPLY = 0xFC00010D;

  const static char DOWNLOAD_DATA_REQUEST_SIG[];
  const static char DOWNLOAD_DATA_REPLY_SIG[];
  const static UINT32 DOWNLOAD_DATA_REQUEST = 0xFC00010E;
  const static UINT32 DOWNLOAD_DATA_REPLY = 0xFC00010F;

  const static char DOWNLOAD_END_REPLY_SIG[];
  const static UINT32 DOWNLOAD_END_REPLY = 0xFC000110;

  const static char MKDIR_REQUEST_SIG[];
  const static char MKDIR_REPLY_SIG[];
  const static UINT32 MKDIR_REQUEST = 0xFC000111;
  const static UINT32 MKDIR_REPLY = 0xFC000112;

  const static char REMOVE_REQUEST_SIG[];
  const static char REMOVE_REPLY_SIG[];
  const static UINT32 REMOVE_REQUEST = 0xFC000113;
  const static UINT32 REMOVE_REPLY = 0xFC000114;

  const static char RENAME_REQUEST_SIG[];
  const static char RENAME_REPLY_SIG[];
  const static UINT32 RENAME_REQUEST = 0xFC000115;
  const static UINT32 RENAME_REPLY = 0xFC000116;

  const static char DIRSIZE_REQUEST_SIG[];
  const static char DIRSIZE_REPLY_SIG[];
  const static UINT32 DIRSIZE_REQUEST = 0xFC000117;
  const static UINT32 DIRSIZE_REPLY = 0xFC000118;

  const static UINT32 LAST_REQUEST_FAILED_REPLY = 0xFC000119;
  const static char LAST_REQUEST_FAILED_REPLY_SIG[];
};

#endif
