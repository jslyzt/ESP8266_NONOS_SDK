#pragma once

#include "c_types.h"

struct servercfg {
    const char* ssid;
    const char* passwd;
    const char* server;
    uint16_t port;
};
extern struct servercfg g_servercfg[];
extern uint8_t g_servercfg_cnt;
