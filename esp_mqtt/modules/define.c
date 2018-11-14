#include "define.h"
#include "eagle_soc.h"

struct servercfg g_servercfg[] = {
    { "h_7C1E", "hiscene2016", "192.168.18.233", 1883 },
    { "Xiaomi_zyc", "1qazxsw2", "192.168.31.100", 1883 },
};
uint8_t g_servercfg_cnt = sizeof(g_servercfg) / sizeof(struct servercfg);


struct portcfg g_portcfg[] = {
    { GPIO_ID_PIN0, "pin0", false },
    { GPIO_ID_PIN(2), "lamp", false },
};
uint8_t g_portcfg_cnt = sizeof(g_portcfg) / sizeof(struct portcfg);