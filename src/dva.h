#ifndef __DVA_H
#define __DVA_H

#include <stdint.h>

#define DVA_MAGIC 0x544847494C415644

struct DVARequest {
  uint64_t magic;
  uint32_t deviceId;
} __attribute__((packed));

void DVA_MakeRequest(void);

#endif
