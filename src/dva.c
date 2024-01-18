#include "dva.h"
#include "network.h"

void DVA_MakeRequest(void)
{
  struct DVARequest* req = (struct DVARequest*)NET_UDP;
  req->magic = DVA_MAGIC;
  req->deviceId = DVA_DEVICE_ID;

  NET_SendUDP(sizeof(struct DVARequest));
}

struct DVAResponse* DVA_ReadResponse(void)
{
  struct DVAResponse* res = (struct DVAResponse*)NET_UDP;
  if (res->magic != DVA_MAGIC || res->deviceId != DVA_DEVICE_ID)
    return NULL;
  
  return res;
}
