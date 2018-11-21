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

struct portcfg {
    int32_t periphsid;
    int8_t funcid;
    uint8_t port;
    const char* name;
    bool onoff;
};

extern struct portcfg g_portcfg[];
extern uint8_t g_portcfg_cnt;
