#include "define.h"
#include "eagle_soc.h"

struct servercfg g_servercfg[] = {
    { "hileia2.4", "hiscene2016", "192.168.18.233", 1883 },
    { "Xiaomi_zyc", "1qazxsw2", "192.168.2.100", 1883 },
    { "MERCURY_zt", "1qazxsw2", "192.168.2.100", 1883 },
};
uint8_t g_servercfg_cnt = sizeof(g_servercfg) / sizeof(struct servercfg);


struct portcfg g_portcfg[] = {
    //{ PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0, GPIO_ID_PIN(0), "0", true },
    { PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2, GPIO_ID_PIN(2), "lamp", false },
};
uint8_t g_portcfg_cnt = sizeof(g_portcfg) / sizeof(struct portcfg);