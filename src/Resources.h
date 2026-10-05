#ifndef RESOURCES_H
#define RESOURCES_H

enum ResourceType { RESOURCE_TEXTURE, RESOURCE_BITMAP }
struct ResourceEntry {
  enum ResourceType type;
  const char *name;
  void *res;

  struct ResourceEntry *_prev, *_next;
};

void Resources_Register(ResourceEntry *entry);
void Resources_Unregister(ResourceEntry *entry);

void Resources_Init(void);
void Resources_Free(void);

#endif
