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

#ifndef _RESOURCE_LOADER_H_
#define _RESOURCE_LOADER_H_

#include "winhdr.h"

#include "Singleton.h"

/**
 * Loader of resources from resource files linked with application.
 *
 * Singleton without support of "lazy" initialization.
 */
class ResourceLoader : public Singleton<ResourceLoader>
{
public:
  /**
   * Initializes resource loader singleton.
   * @param appInst windwos application instance.
   */
  ResourceLoader(HINSTANCE appInst);
  /**
   * Frees resource loader singleton.
   */
  virtual ~ResourceLoader();

  /**
   * Loads standart icon from resources.
   * @param iconName icon name.
   * @return icon handle of 0 if cannot load icon.
   */
  HICON loadStandartIcon(const TCHAR *iconName);

  /**
   * Loads icon from resources.
   * @param iconName icon name.
   * @return icon handle of 0 if cannot load icon.
   */
  HICON loadIcon(const TCHAR *iconName);

  /**
   * Loads string from resources.
   * @param [in] id identifier of string in resource file.
   * @param [out] string storage for string from resources.
   * @return true if string is loaded, false otherwise.
   */
  bool loadString(UINT id, StringStorage *string);

  /**
   * Loads accelerator from resources.
   * @param [in] id identifier of accelerator in resource file.
   * @return handle of accelerator if it's loaded.
   */
  HACCEL loadAccelerator(UINT id);

  /**
   * Loads standart cursor.
   * @param [in] id identifier of accelerator in resource file.
   * @return handle of cursor if it's loaded.
   */
  HCURSOR loadStandardCursor(const TCHAR *id);
  /**
   * Loads cursor from resources.
   * @param [in] id identifier of accelerator in resource file.
   * @return handle of cursor if it's loaded.
   */
  HCURSOR loadCursor(UINT id);

protected:
  /**
   * Application instance.
   */
  HINSTANCE m_appInstance;
};

#endif
