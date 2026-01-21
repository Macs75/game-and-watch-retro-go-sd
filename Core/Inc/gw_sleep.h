#ifndef _GW_SLEEP_H_
#define _GW_SLEEP_H_

#include <stdint.h>
#include <stdbool.h>

typedef void (*sleep_wake_hook_t)();
void GW_EnterDeepSleep(bool standby, sleep_wake_hook_t wakeup_callback);

#endif