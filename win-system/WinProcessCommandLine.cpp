// Copyright (C) 2012 GlavSoft LLC.
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

#include "WinProcessCommandLine.h"

WinProcessCommandLine::WinProcessCommandLine() 
{
  std::vector<StringStorage> out;

  m_wcla = new WinCommandLineArgs(GetCommandLine());
  m_wcla->getArgVector(&out);
  for (size_t i = 0; i < out.size(); i++) {
    if (out[i].getString()[0] != _T('-')) {
      m_strParam.push_back(out[i]);
    } else {
      optionParser(&out[i]);
    }
  }
}

void WinProcessCommandLine::optionParser(StringStorage *out)
{
  pair<StringStorage, StringStorage> strPair;
  
  out->remove(0, 1); 
  size_t ipos = out->findChar(_T('='));
  if (ipos == -1) {
    strPair.first = *out;
    strPair.second = _T("");
  } else {
    StringStorage strTemp = *out;
    strTemp.remove(0, ipos+1);
    strPair.second = strTemp;
    strTemp = *out;
    strTemp.remove(ipos, strTemp.getLength()-ipos);
    strPair.first = strTemp;
  }
  strPair.first.toLowerCase();
  m_strParams.push_back(strPair);
}

size_t WinProcessCommandLine::getArgumentsCount()
{
  return m_strParam.size();
}

size_t WinProcessCommandLine::getOptionsCount()
{
  return m_strParams.size();
}

bool WinProcessCommandLine::findOptionValue(const StringStorage valName, StringStorage* strOut)
{
  for (size_t i = 0; i < getOptionsCount(); i++) {
    if (m_strParams[i].first == valName) {
      *strOut = m_strParams[i].second;

      return true;
    }
  }
  return false;
}

bool WinProcessCommandLine::getArgument(size_t index, StringStorage* strOut)
{
  if (index < getArgumentsCount()) {
    *strOut = m_strParam[index];

    return true;
  }
  return false;
}

bool WinProcessCommandLine::getOption(size_t index, StringStorage* strOut)
{
  if (index < getOptionsCount()) {
    *strOut = m_strParams[index].first;

    return true;
  }
  return false;
}

WinProcessCommandLine::~WinProcessCommandLine()
{
  if (m_wcla) {
    delete m_wcla;
  }
}
