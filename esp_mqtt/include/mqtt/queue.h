#ifndef USER_QUEUE_H_
#define USER_QUEUE_H_

#include "os_type.h"
#include "ringbuf.h"

typedef struct {
    uint8_t* buf;
    RINGBUF rb;
} QUEUE;

void ICACHE_FLASH_ATTR QUEUE_Init(QUEUE* queue, int bufferSize);
int32_t ICACHE_FLASH_ATTR QUEUE_Puts(QUEUE* queue, uint8_t* buffer, uint16_t len);
int32_t ICACHE_FLASH_ATTR QUEUE_Gets(QUEUE* queue, uint8_t* buffer, uint16_t* len, uint16_t maxLen);
BOOL ICACHE_FLASH_ATTR QUEUE_IsEmpty(QUEUE* queue);
#endif
