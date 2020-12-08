#include "ProfileLogger.h"

#include "util/GetCPUTime.h"
#include <algorithm>
#include <numeric> 
#include <utility> 
#include "thread/AutoLock.h"

// does not works on XP
// #include "Realtimeapiset.h"

ProfileLogger::~ProfileLogger()
{
}

ProcessorTimes ProfileLogger::checkPoint(const TCHAR *description)
{
  AutoLock al(&m_mapMut);

  /*
  try {
    QueryProcessCycleTime(GetCurrentProcess(), &m_last.cycle);
  }
  catch (...) {
    m_lastCycle = 0;
  }*/
  ProcessorTimes t;
  t.cycle = m_last.cycle;
  m_last.cycle = rdtsc();
  t.cycle = m_last.cycle - t.cycle;

  t.process = m_last.process;
  m_last.process = getCPUTime();
  t.process = m_last.process - t.process;
  
  t.kernel = m_last.kernel;
  m_last.kernel = getKernelTime();
  t.kernel = m_last.kernel - t.kernel;

  t.wall = m_last.wall;
  m_last.wall = DateTime::now();
  t.wall = m_last.wall - t.wall;

// #if DROP_TIME_STAT  // test code
  m_checkPoints[description].push_back(m_last);
// #endif
  return t;
}

typedef std::pair<ProcessorTimes, const TCHAR *> CHECKPPOINTPAIR;
typedef std::pair<const TCHAR *, const TCHAR *> STRINGPAIR;
// helper function for std::sort
bool pairCompare(const CHECKPPOINTPAIR& firstElem, const CHECKPPOINTPAIR& secondElem) {
  return firstElem.first.cycle < secondElem.first.cycle;
}

std::vector<TCHAR> printVectorStats(const TCHAR *header, const TCHAR *tag1, const TCHAR *tag2, std::vector<double> v) {
  double min = *std::min_element(v.begin(), v.end());
  double max = *std::max_element(v.begin(), v.end());
  int num = v.size();
  double avg = std::accumulate(v.begin(), v.end(), 0.) / num;
  const TCHAR *fmt = L"%s for %s - %s distance: avg: %f, min: %f, max: %f, executed %d times\n";
  int count = _sctprintf(fmt, header, tag1, tag2, avg, min, max, num);
  count++;
  std::vector<TCHAR> formattedString(count);
  _stprintf_s(&formattedString.front(), count, fmt, header, tag1, tag2, avg, min, max, num);
  return formattedString;
}

std::vector<std::vector<TCHAR>> ProfileLogger::dropStat()
{
  AutoLock al(&m_mapMut);
  std::vector<std::vector<TCHAR>> resultStrings;
  if (m_checkPoints.size() == 0)
    return resultStrings;
  DateTime dt = DateTime::now() - m_lastDrop;
  if (dt.getTime() / 1000. < m_dropRate) {
    return resultStrings;
  }
  m_lastDrop = DateTime::now();

  std::vector<CHECKPPOINTPAIR> checkPointPairs;
  std::map<const TCHAR *, std::vector<ProcessorTimes>>::iterator i;
  // Build vector with times-description pairs and sort it by times order
  for (i = m_checkPoints.begin(); i != m_checkPoints.end(); i++) {
    std::vector<ProcessorTimes> times = (*i).second;
    std::vector<ProcessorTimes>::iterator t;
    for (t = times.begin(); t != times.end(); t++) {
      checkPointPairs.push_back(CHECKPPOINTPAIR((*t), (*i).first));
    }
  }
  std::sort(checkPointPairs.begin(), checkPointPairs.end(), pairCompare);

  if (checkPointPairs.size() == 0)
    return resultStrings;

  // calc deltas for consequent points
  struct ProcessorTimesDeltas {
    std::vector<double> deltac; // CPU cycles deltas
    std::vector<double> deltap; // Process time deltas
    std::vector<double> deltak; // Kernel time deltas
  };
  std::map<STRINGPAIR, ProcessorTimesDeltas> deltas;
  std::vector<CHECKPPOINTPAIR>::iterator p = checkPointPairs.begin();
  for (;;) {
    const TCHAR *tag1 = (*p).second;
    ProcessorTimes pt1 = (*p).first;
    ++p;
    if (p == checkPointPairs.end())
      break;
    ProcessorTimes pt2 = (*p).first;
    const TCHAR *tag2 = (*p).second;
    STRINGPAIR sp = STRINGPAIR(tag1, tag2);
    deltas[sp].deltac.push_back(double(pt2.cycle - pt1.cycle) / 1000000.);
    deltas[sp].deltap.push_back(pt2.process - pt1.process);
    deltas[sp].deltak.push_back(pt2.kernel - pt1.kernel);
  }

  std::map<STRINGPAIR, ProcessorTimesDeltas>::iterator d;
  // print results
  for (d = deltas.begin(); d != deltas.end(); ++d) {
    const TCHAR *tag1 = (*d).first.first;
    const TCHAR *tag2 = (*d).first.second;
    std::vector<TCHAR> str = printVectorStats(L"CPU cycles", tag1, tag2, (*d).second.deltac);
    resultStrings.push_back(str);
    str = printVectorStats(L"Process times", tag1, tag2, (*d).second.deltap);
    resultStrings.push_back(str);
    str = printVectorStats(L"Kernel times", tag1, tag2, (*d).second.deltak);
    resultStrings.push_back(str);
  }
  m_checkPoints.clear();
  return resultStrings;
}


