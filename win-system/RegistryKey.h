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

#ifndef _REGISTRY_KEY_H_
#define _REGISTRY_KEY_H_

#include "util/CommonHeader.h"

class RegistryKey
{
public:
  RegistryKey(HKEY rootKey, const TCHAR *entry, bool createIfNotExists = true, SECURITY_ATTRIBUTES *sa = 0);
  RegistryKey(RegistryKey *rootKey, const TCHAR *entry, bool createIfNotExists = true, SECURITY_ATTRIBUTES *sa = 0);
  RegistryKey(HKEY rootKey);
  // Default contructor for a defer initialization.
  RegistryKey();

public:

  virtual ~RegistryKey();


  // Defer initialization. Can be used only when it has been
  // created by the default constructor.
  void open(HKEY rootKey, const TCHAR *entry,
            bool createIfNotExists = true,
            SECURITY_ATTRIBUTES *sa = 0);

  // Defer initialization. Can be used only when it has been
  // created by the default constructor.
  void open(RegistryKey *rootKey,
            const TCHAR *entry,
            bool createIfNotExists = true,
            SECURITY_ATTRIBUTES *sa = 0);

  //
  // Returns WinAPI HKEY handle.
  //
  HKEY getHKEY() const;

  // Creates subkey.
  //
  // Returns true in two cases:
  // 1) subkey exists;
  // 2) subkey didn't exists, and subkey was succerfully created
  //    by method call.
  //
  bool createSubKey(const TCHAR *subkey);
  // Deletes subkey from this registry entry.
  bool deleteSubKey(const TCHAR *subkey);
  // Deletes subkey tree from this registry entry
  bool deleteSubKeyTree(const TCHAR *subkey);
  // Deletes value from this registry entry.
  bool deleteValue(const TCHAR *name);

  //
  // Set value methods
  //

  bool setValueAsInt32(const TCHAR *name, int value);
  bool setValueAsInt64(const TCHAR *name, long value);
  bool setValueAsString(const TCHAR *name, const TCHAR *value);
  bool setValueAsBinary(const TCHAR *name, const void *value,
                        size_t sizeInBytes);

  //
  // Get value methods
  //

  bool getValueAsInt32(const TCHAR *name, int *out);
  bool getValueAsInt64(const TCHAR *name, long *out);
  bool getValueAsString(const TCHAR *name, StringStorage *out);
  bool getValueAsBinary(const TCHAR *name, void *value, size_t *sizeInBytes);

  //
  // Sets subkey names list to subKeyNames array.
  // Remark: if subKeyNames is NULL then count of subkeys
  // is sets to count output var.
  //
  // Parameters:
  // [out, optional] subKeyNames - output array to place subkey names,
  // can be NULL.
  // [out, optional] count - output value to place subkey count,
  // can be NULL.
  //

  bool getSubKeyNames(StringStorage *subKeyNames, size_t *count);

  // Returns true if key is valid and opened.
  bool isOpened();
  // Closes registry entry if it's opened.
  void close();

private:

  // Helper method to avoid code duplicate in class constructor.
  void initialize(HKEY rootKey, const TCHAR *entry, bool createIfNotExists, SECURITY_ATTRIBUTES *sa);

  // Sets subkey name to name output variable not depending
  // on output buffer size, cause buffer allocates inside method.
  //
  // Returns return value of RegEnumKey if it's
  // not equal to ERROR_MORE_DATA.
  DWORD enumKey(DWORD i, StringStorage *name);

  /**
   * Opens subkey or creates it if it's does not exists.
   * @param [in] key root key.
   * @param [in] subkey subkey name (can include more than one subkey).
   * @param [out] openedKey created or opened key.
   * @param [in] createIfNotExists flag determinates to create key if it does not exists.
   * @param [in, opt] sa security attributes.
   * @return true if operation successfull executed, false otherwise.
   */
  static bool tryOpenSubKey(HKEY key, const TCHAR *subkey,
                            HKEY *openedKey, bool createIfNotExists,
                            SECURITY_ATTRIBUTES *sa);

protected:
  // Registry entry associated with this class instance.
  HKEY m_key;
  // Root registry key (local machine, current user, etc).
  HKEY m_rootKey;
  // Registry entry name.
  StringStorage m_entry;

  friend class Registry;
};

#endif
