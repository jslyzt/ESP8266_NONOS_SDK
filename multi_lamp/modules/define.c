#include "define.h"
#include "eagle_soc.h"

struct servercfg g_servercfg[] = {
    { "h_7C1E", "hiscene2016", "192.168.18.233", 1883 },
    { "Xiaomi_zyc", "1qazxsw2", "192.168.31.100", 1883 },
    { "MERCURY_zt", "1qazxsw2", "192.168.2.100", 1883 },
};
uint8_t g_servercfg_cnt = sizeof(g_servercfg) / sizeof(struct servercfg);
