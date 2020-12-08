#include "MemUsage.h"
#include "win-system/Environment.h"

size_t MemUsage::getCurrentMemUsage()
{
   if (!Environment::isVistaOrLater()) {
     return 0;
   }
  PROCESS_MEMORY_COUNTERS pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
  return (size_t)pmc.WorkingSetSize;
}
