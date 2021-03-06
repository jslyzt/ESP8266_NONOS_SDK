#include "ets_sys.h"
#include "driver/uart.h"
#include "osapi.h"
#include "mqtt.h"
#include "wifi.h"
#include "debug.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"
#include "define.h"

typedef unsigned long u32_t;

MQTT_Client mqttClient;
int8_t choose_config = -1;
char subtopic[128] = {0};
char pubtopic[128] = {0};
char smac[64] = {0};
char sip[64] = {0};
bool syncState = false;
static ETSTimer pub_timer;

////////////////////////////////////////////////////////////////////////////////////
void publish_state() {
    char oinfo[512] = {0};
    char pinfo[512] = {0};
    uint8_t i = 0;
    for (; i < g_portcfg_cnt; i++) {
        struct portcfg* cfg = &g_portcfg[i];
        if(cfg != NULL) {
            os_sprintf(pinfo + os_strlen(pinfo), "%s:%d,", cfg->name, (cfg->onoff ? 1 : 0));
        }
    }
    os_sprintf(oinfo, "%s|%s|%d|[%s]", smac, sip, (syncState ? 1 : 0), pinfo);
    MQTT_Publish(&mqttClient, pubtopic, oinfo, os_strlen(oinfo), 2, 0);
}

void wifiConnectCb(uint8_t status) {
    if (status != STATION_GOT_IP) {
        MQTT_Disconnect(&mqttClient);
        os_timer_disarm(&pub_timer);
    } else {
        struct ip_info ipconfig;
        if(wifi_get_ip_info(STATION_IF, &ipconfig) == true) {
            os_memset(sip, 0, sizeof(sip));
            os_sprintf(sip, IPSTR, IP2STR(&ipconfig.ip));
            os_printf("wifi connect ip: %s\n", sip);
        }
        MQTT_Connect(&mqttClient);
    }
}

////////////////////////////////////////////////////////////////////////////////////

void mqttConnectedCb(uint32_t* args) {
    os_printf("MQTT: Connected\r\n");
    MQTT_Client* client = (MQTT_Client*)args;
    if(client != NULL) {
        MQTT_Subscribe(client, subtopic, 2);    // 设置订阅
        os_timer_disarm(&pub_timer);
        os_timer_setfn(&pub_timer, (os_timer_func_t*)publish_state, NULL);
        os_timer_arm(&pub_timer, 5000, 1); //5s一次
        publish_state();
    }
}

void mqttDisconnectedCb(uint32_t* args) {
    os_printf("MQTT: Disconnected\r\n");
    os_timer_disarm(&pub_timer);
}

void mqttPublishedCb(uint32_t* args) {
    os_printf("MQTT: Published\r\n");
}

void pinOnOff(const char* data, uint32_t len, bool onoff) {
    uint8_t i = 0;
    for (; i < g_portcfg_cnt; i++) {
        struct portcfg* cfg = &g_portcfg[i];
        if(cfg != NULL && os_strncmp(cfg->name, data, len) == 0) {
            if(cfg->onoff != onoff) {
                cfg->onoff = onoff;
                GPIO_OUTPUT_SET(cfg->port, (onoff ? 1 : 0));
                os_printf("pin: %d, key: %s, %s\n", cfg->port, cfg->name, (onoff ? "on" : "off"));
            }
        }
    }
}

void pinOn(const char* data, uint32_t len) {
    pinOnOff(data, len, true);
}

void pinOff(const char* data, uint32_t len) {
    pinOnOff(data, len, false);
}

void pinState(const char* data, uint32_t len) {
    os_printf("pin state sync, data: %s, length: %d", data, len);

    syncState = false;
    unsigned int pos = 0, posk = 0, posv = 0;
    char ch = 0, val = 0;
    char key[11] = { 0 };

    while (pos < len) {
        ch = *(data + pos++);
        if (ch == ':') {
            posk = pos;
            if (posk < posv || posk - pos > 10) {
                os_printf("pin state sync, key out of range 10, %d", data + posv);
                break;
            }
            os_strncpy(key, data + posv, posk - posv - 1);
            key[posk - posv] = 0;
        }
        if (ch == ',') {
            val = *(data + posk);
            if(val == '0') {
                pinOnOff(key, posk - posv - 1, false);
            } else {
                pinOnOff(key, posk - posv - 1, true);
            }
            posv = pos;
        }
    }
}

void mqttDataCb(uint32_t* args, const char* topic, uint32_t topic_len, const char* data, uint32_t data_len) {
    MQTT_Client* client = (MQTT_Client*)args;
    if(client == NULL || data_len <= 0 || data == NULL) {
        return;
    }
    char key[64] = {0};
    uint32_t klen = 0, pos = 0;
    if (*(data + (pos++)) == '[') {
        char ch = 0;
        while (pos < data_len) {
            ch = *(data + (pos++));
            if (ch != ']') {
                key[klen++] = ch;
            } else {
                break;
            }
        }
    }

    if (pos >= data_len) {
        os_printf("receive data length error, topic: %s, data: %s", topic, data);
        return;
    }

    if (klen <= 0) {
        os_printf("receive data key error, topic: %s, data: %s", topic, data);
        return;
    }

    if(klen == 2 && ((key[0] == 'o' && key[1] == 'n') || (key[0] == 'O' && key[1] == 'N'))) {
        pinOn(data + pos, data_len - pos);
    } else if(klen == 3 && ((key[0] == 'o' && key[1] == 'f' && key[2] == 'f') || (key[0] == 'O' && key[1] == 'F' && key[2] == 'F'))) {
        pinOff(data + pos, data_len - pos);
    } else if(klen == 5 && key[0] == 's' && key[1] == 't' && key[2] == 'a' && key[3] == 't' && key[4] == 'e') {
        pinState(data + pos, data_len - pos);
    }
}

////////////////////////////////////////////////////////////////////////////////////
void get_config_done();
// 扫描wifi
void ICACHE_FLASH_ATTR
scan_done(void* arg, STATUS status) {
    uint8 ssid[33];
    struct station_config stationConf;
    if (status == OK) {
        struct bss_info* bss_link = (struct bss_info*)arg;
        while (bss_link != NULL) {
            os_memset(ssid, 0, 33);
            if (os_strlen(bss_link->ssid) <= 32) {
                os_memcpy(ssid, bss_link->ssid, os_strlen(bss_link->ssid));
            } else {
                os_memcpy(ssid, bss_link->ssid, 32);
            }
            if (choose_config < 0) {
                uint8_t i = 0;
                for (; i < g_servercfg_cnt; i++) {
                    struct servercfg* cfg = &g_servercfg[i];
                    if(cfg != NULL && os_strcmp(ssid, cfg->ssid) == 0) {
                        choose_config = i;
                        break;
                    }
                }
            }
            os_printf("+CWLAP:(%d,\"%s\",%d,\"" MACSTR "\",%d)\r\n",
                       bss_link->authmode, ssid, bss_link->rssi,
                       MAC2STR(bss_link->bssid), bss_link->channel);
            bss_link = bss_link->next.stqe_next;
        }
    } else {
        os_printf("err, scan status %d\r\n", status);
    }
    get_config_done();
}

void to_scan_wifi(void) {
    wifi_station_scan(NULL, scan_done);
}

////////////////////////////////////////////////////////////////////////////////////
void get_config_done() {
    if(choose_config < 0 || choose_config >= g_servercfg_cnt) {
         os_printf("scan wifi error, can not find choose wifi, restart\n");
         system_restart();
         return;
    }
    
    struct servercfg* cfg = &g_servercfg[choose_config];
    if(cfg == NULL) {
        os_printf("get wifi config error, restart\n");
        system_restart();
        return;
    }

    os_printf("scan wifi done, choose wifi is %s\n", cfg->ssid);

    uint8 bssid[6];
    os_memset(bssid, 0, 6);
    if(wifi_get_macaddr(STATION_IF, bssid) == false) {
        os_printf("get mac address error, restart\n");
        system_restart();
        return;
    }

    os_memset(smac, 0, sizeof(smac));
    os_sprintf(smac, "%02x-%02x-%02x-%02x-%02x-%02x", MAC2STR(bssid));
    os_printf("wifi connect mac: %s\n", smac);

    os_memset(subtopic, 0, sizeof(subtopic));
    os_sprintf(subtopic, "/arduino/stoc/%s", smac);
    os_memset(pubtopic, 0, sizeof(pubtopic));
    os_sprintf(pubtopic, "/arduino/ctos/%s", smac);

    MQTT_InitConnection(&mqttClient, (uint8_t*)cfg->server, cfg->port, DEFAULT_SECURITY);
    MQTT_InitClient(&mqttClient, smac, "", "", 3, 1);

    MQTT_OnConnected(&mqttClient, mqttConnectedCb);         // mqtt 连接回调
    MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);   // mqtt 断开连接回调
    //MQTT_OnPublished(&mqttClient, mqttPublishedCb);         // mqtt 发布数据回调
    MQTT_OnData(&mqttClient, mqttDataCb);                   // mqtt 收到数据回调

    WIFI_Connect((uint8_t*)cfg->ssid, (uint8_t*)cfg->passwd, wifiConnectCb);      // wifi 连接状态回调

    os_printf("\r\nSystem started ...\r\n");
}

// 入口
void user_init(void) {
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    //os_delay_us(60000);

    gpio_init();
    uint8_t i = 0;
    for (; i < g_portcfg_cnt; i++) {
        struct portcfg* cfg = &g_portcfg[i];
        if(cfg != NULL) {
            PIN_FUNC_SELECT(cfg->periphsid,cfg->funcid);
            GPIO_OUTPUT_SET(cfg->port, (cfg->onoff ? 1 : 0));
        }
    }

    wifi_set_opmode(STATION_MODE);
    wifi_station_set_auto_connect(false);
    system_init_done_cb(to_scan_wifi);
}
