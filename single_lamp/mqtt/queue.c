#include "queue.h"

#include "user_interface.h"
#include "osapi.h"
#include "os_type.h"
#include "mem.h"
#include "proto.h"

void ICACHE_FLASH_ATTR QUEUE_Init(QUEUE* queue, int bufferSize) {
    queue->buf = (uint8_t*)os_zalloc(bufferSize);
    RINGBUF_Init(&queue->rb, queue->buf, bufferSize);
}

int32_t ICACHE_FLASH_ATTR QUEUE_Puts(QUEUE* queue, uint8_t* buffer, uint16_t len) {
    return PROTO_AddRb(&queue->rb, buffer, len);
}

int32_t ICACHE_FLASH_ATTR QUEUE_Gets(QUEUE* queue, uint8_t* buffer, uint16_t* len, uint16_t maxLen) {

    return PROTO_ParseRb(&queue->rb, buffer, len, maxLen);
}

BOOL ICACHE_FLASH_ATTR QUEUE_IsEmpty(QUEUE* queue) {
    if (queue->rb.fill_cnt <= 0)
        return TRUE;
    return FALSE;
}
