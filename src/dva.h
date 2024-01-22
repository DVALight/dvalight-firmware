#ifndef __DVA_H
#define __DVA_H

#include <stdint.h>

#define DVA_MAGIC 0x544847494C415644

struct DVARequest {
  uint64_t magic;
  uint32_t deviceId;
} __attribute__((packed));

struct DVAResponse {
  uint64_t magic;
  uint32_t deviceId;
  uint32_t color;
  uint8_t state;
} __attribute__((packed));


void DVA_MakeRequest(void);
struct DVAResponse* DVA_ReadResponse(void); // returns NULL if not valid response

#endif
