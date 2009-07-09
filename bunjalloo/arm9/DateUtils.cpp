#include <cstdio>
#include "DateUtils.h"
#include "config_defs.h"
#ifdef ARM9
// time.h doesn't have timezone on the DS.
static int timezone = 0;
#endif

static const char FORMAT[] = "%a, %d %b %Y %H:%M:%S GMT";

time_t DateUtils::parseDate(const char *date)
{
  struct tm val;
  if (strptime(date, FORMAT, &val) == 0) {
    return 0;
  }
  return mktime(&val) - timezone;
}