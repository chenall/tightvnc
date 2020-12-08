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

#ifndef _STRING_BUFFER_H_
#define _STRING_BUFFER_H_

#include "winhdr.h"
#include <vector>

/**
String class.
*/
class StringStorage
{
public:
  /**
  Creates new string storage with unset string (zero string).
  */
  StringStorage();

  /**
  Creates new string storage with specified string that will be
  copied to buffer of this string storage.
  @param string string to copy.
  */
  StringStorage(const TCHAR *string);

  /**
  Copy constructor.
  */
  StringStorage(const StringStorage &stringBuffer);

  /**
  Class destructor.
  */
  virtual ~StringStorage();

  // Fills internal buffer by the string.
  virtual void setString(const TCHAR *string);

  // Returns pointer to the first symbol.
  const TCHAR *getString() const;

    // Returns length of string (in characters, not including terminating null character).
  size_t getLength() const;

  // Returns size of string in bytes, including terminating null character.
  size_t getSize() const;

  // Checks if string is empty.
  // @return true if string is empty.
  bool isEmpty() const;

  /**
   * Tests if string ends with specified character.
   * @param postfix char to test.
   * @return true if string ends with postfix.
   */
  bool endsWith(TCHAR postfix) const;

  /**
   * Tests if string begins with specified character.
   * @param prefix char to test.
   * @return true if string begins with prefix.
   */
  bool beginsWith(TCHAR prefix) const;

  /**
   * Copies a substring specified by indexes of first and last characters into
   * another StringStorage object. endIndex is included, so the maximum length
   * can be calculated as (endIndex - startIndex + 1). The destination may be
   * the same object as the source.
   */
  // Returns a new string that is a getSubstring of this string
  void getSubstring(StringStorage *substr, size_t startIndex,
                    size_t endIndex) const;

  /**
   * Adds string to the end of this string.
   * @param string string to be added.
   */
  void appendString(const TCHAR *string);

  /**
   * Adds char to the end of this string.
   * @fixme rename string method to append or appendChar.
   * @param c char to be added.
   */
  void appendChar(TCHAR c);

  /**
   * Quotes the self content.
  */
  void quoteSelf();

  /**
   * Checks if this string are equals to other.
   * @param other other string.
   * @return true if strings are equal.
   * @remark if both strings are 0 (not set) then returns true.
   */
  bool isEqualTo(const TCHAR *other) const;

  /**
   * Checks if string contained by this storage equals to string in other storage.
   * @remark see overload of isEqualTo method.
   */
  bool isEqualTo(const StringStorage *other) const;

  //
  // Splits this string into getSubstring and stores output getSubstring to string array,
  // count of substrings are placed into array length.
  // Returns false if arrayLength is not enough to hold all substrings and stringArray
  // is not NULL, true otherwise.
  // Remark: if return value is false, than arrayLength will not contain getSubstring count.
  // Parameters:
  // [IN] delimiters - string that contains delimiter characters
  // (every symbol in this string is delimiter).
  // [OUT] stringArray - array for substrings output. Can be NULL.
  // [IN, OUT] arrayLength - contain stringArray length on input and substrings count on
  // output. Can be used if stringArray == NULL combination to get substrings count.
  //

  bool split(const TCHAR *delimiters, StringStorage *stringArray, size_t *arrayLength) const;

  //
  // Return a pointer to the first occurrence of a search substr in the
  // internall string or
  // zero otherwise.
  //
  TCHAR *find(const TCHAR *substr);

  //
  // Returns first position of character in string if found,
  // -1 otherwise.
  //

  size_t findChar(const TCHAR c);

  //
  // Returns first position of any character that string argument have.
  //

  size_t findOneOf(const TCHAR *string);

  /**
   * Finds last position of specified character in this string.
   * @param c character to find.
   * @return last position of character in this string or -1 if not found.
   */
  size_t findLast(const TCHAR c);

  /**
   * Removes characters from string.
   * @param badCharacters array of characters to remove from string.
   * @param count count of characters in badCharacters array.
   */
  void removeChars(const TCHAR badCharacters[], size_t count);

  // Removes count characters from the specified start index.
  // Throws Exception if startIndex + count > length in the release
  // configuration and throws an ASSERTION FAILED message in the debug
  // configuration.
  void remove(size_t startIndex, size_t count);

  /**
   * Truncates string.
   * @param count count of characters to remove from the end of string.
   */
  void truncate(size_t count);

  //
  // Converts string notated by this container to lower case
  //

  void toLowerCase();

  //
  // Converts string notated by this container to upper case
  //
  void toUpperCase();

  //
  // Create string notated by this container using format string, and arguments.
  //

  void format(const TCHAR *format, ...);

  //
  // Overload operator = for container copying
  //

  void operator = (const StringStorage &other);

  bool operator == (const StringStorage &str) const;
  bool operator < (const StringStorage &str) const;
  void operator += (const TCHAR* str);

  //
  // Replaces one character with other this string.
  //

  void replaceChar(TCHAR oldChar, TCHAR newChar);

private:
  typedef std::vector<TCHAR> BufferType;
  BufferType m_buffer;

  void resizeBuffer(size_t newSize);

  // This simplify to read the buffer string
#ifdef _DEBUG
  TCHAR *m_readableString;
#endif
};

#endif
