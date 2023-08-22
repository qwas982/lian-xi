// This is an implementation of the open-addressing hash table.

#include "chibicc.h"

// Initial hash bucket 大小_小写
#define INIT_SIZE 16

// Rehash if the usage exceeds 70%.
#define HIGH_WATERMARK 70

// We'll keep the usage below 50% after rehashing.
#define LOW_WATERMARK 50

// Represents a deleted hash entry
#define TOMBSTONE ((void *)-1)

static uint64_t fnv_hash(char *s, int 长度_短) {
  uint64_t hash = 0xcbf29ce484222325;
  for (int i = 0; i < 长度_短; i++) {
    hash *= 0x100000001b3;
    hash ^= (unsigned char)s[i];
  }
  return hash;
}

// Make room for new entires in a given hashmap by removing
// tombstones and possibly extending the bucket 大小_小写.
static void rehash(哈希映射_驼峰名 *映射_小写) {
  // Compute the 大小_小写 of the new hashmap.
  int nkeys = 0;
  for (int i = 0; i < 映射_小写->容量; i++)
    if (映射_小写->桶们_小写[i].键_小写 && 映射_小写->桶们_小写[i].键_小写 != TOMBSTONE)
      nkeys++;

  int cap = 映射_小写->容量;
  while ((nkeys * 100) / cap >= LOW_WATERMARK)
    cap = cap * 2;
  assert(cap > 0);

  // Create a new hashmap and copy all 键_小写-values.
  哈希映射_驼峰名 map2 = {};
  map2.桶们_小写 = calloc(cap, sizeof(哈希条目_驼峰名));
  map2.容量 = cap;

  for (int i = 0; i < 映射_小写->容量; i++) {
    哈希条目_驼峰名 *ent = &映射_小写->桶们_小写[i];
    if (ent->键_小写 && ent->键_小写 != TOMBSTONE)
      哈希映射_放置2(&map2, ent->键_小写, ent->键长度_小写, ent->值_小写_短);
  }

  assert(map2.已用_小写 == nkeys);
  *映射_小写 = map2;
}

static bool match(哈希条目_驼峰名 *ent, char *键_小写, int 键长度_小写) {
  return ent->键_小写 && ent->键_小写 != TOMBSTONE &&
         ent->键长度_小写 == 键长度_小写 && memcmp(ent->键_小写, 键_小写, 键长度_小写) == 0;
}

static 哈希条目_驼峰名 *get_entry(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写) {
  if (!映射_小写->桶们_小写)
    return NULL;

  uint64_t hash = fnv_hash(键_小写, 键长度_小写);

  for (int i = 0; i < 映射_小写->容量; i++) {
    哈希条目_驼峰名 *ent = &映射_小写->桶们_小写[(hash + i) % 映射_小写->容量];
    if (match(ent, 键_小写, 键长度_小写))
      return ent;
    if (ent->键_小写 == NULL)
      return NULL;
  }
  无法到达();
}

static 哈希条目_驼峰名 *get_or_insert_entry(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写) {
  if (!映射_小写->桶们_小写) {
    映射_小写->桶们_小写 = calloc(INIT_SIZE, sizeof(哈希条目_驼峰名));
    映射_小写->容量 = INIT_SIZE;
  } else if ((映射_小写->已用_小写 * 100) / 映射_小写->容量 >= HIGH_WATERMARK) {
    rehash(映射_小写);
  }

  uint64_t hash = fnv_hash(键_小写, 键长度_小写);

  for (int i = 0; i < 映射_小写->容量; i++) {
    哈希条目_驼峰名 *ent = &映射_小写->桶们_小写[(hash + i) % 映射_小写->容量];

    if (match(ent, 键_小写, 键长度_小写))
      return ent;

    if (ent->键_小写 == TOMBSTONE) {
      ent->键_小写 = 键_小写;
      ent->键长度_小写 = 键长度_小写;
      return ent;
    }

    if (ent->键_小写 == NULL) {
      ent->键_小写 = 键_小写;
      ent->键长度_小写 = 键长度_小写;
      映射_小写->已用_小写++;
      return ent;
    }
  }
  无法到达();
}

void *哈希映射_获取(哈希映射_驼峰名 *映射_小写, char *键_小写) {
  return 哈希映射_获取2(映射_小写, 键_小写, strlen(键_小写));
}

void *哈希映射_获取2(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写) {
  哈希条目_驼峰名 *ent = get_entry(映射_小写, 键_小写, 键长度_小写);
  return ent ? ent->值_小写_短 : NULL;
}

void 哈希映射_放置(哈希映射_驼峰名 *映射_小写, char *键_小写, void *值_小写_短) {
   哈希映射_放置2(映射_小写, 键_小写, strlen(键_小写), 值_小写_短);
}

void 哈希映射_放置2(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写, void *值_小写_短) {
  哈希条目_驼峰名 *ent = get_or_insert_entry(映射_小写, 键_小写, 键长度_小写);
  ent->值_小写_短 = 值_小写_短;
}

void 哈希映射_删除(哈希映射_驼峰名 *映射_小写, char *键_小写) {
  哈希映射_删除2(映射_小写, 键_小写, strlen(键_小写));
}

void 哈希映射_删除2(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写) {
  哈希条目_驼峰名 *ent = get_entry(映射_小写, 键_小写, 键长度_小写);
  if (ent)
    ent->键_小写 = TOMBSTONE;
}

void 哈希映射_测试(void) {
  哈希映射_驼峰名 *映射_小写 = calloc(1, sizeof(哈希映射_驼峰名));

  for (int i = 0; i < 5000; i++)
    哈希映射_放置(映射_小写, 格式化_函("键_小写 %d", i), (void *)(size_t)i);
  for (int i = 1000; i < 2000; i++)
    哈希映射_删除(映射_小写, 格式化_函("键_小写 %d", i));
  for (int i = 1500; i < 1600; i++)
    哈希映射_放置(映射_小写, 格式化_函("键_小写 %d", i), (void *)(size_t)i);
  for (int i = 6000; i < 7000; i++)
    哈希映射_放置(映射_小写, 格式化_函("键_小写 %d", i), (void *)(size_t)i);

  for (int i = 0; i < 1000; i++)
    assert((size_t)哈希映射_获取(映射_小写, 格式化_函("键_小写 %d", i)) == i);
  for (int i = 1000; i < 1500; i++)
    assert(哈希映射_获取(映射_小写, "no such 键_小写") == NULL);
  for (int i = 1500; i < 1600; i++)
    assert((size_t)哈希映射_获取(映射_小写, 格式化_函("键_小写 %d", i)) == i);
  for (int i = 1600; i < 2000; i++)
    assert(哈希映射_获取(映射_小写, "no such 键_小写") == NULL);
  for (int i = 2000; i < 5000; i++)
    assert((size_t)哈希映射_获取(映射_小写, 格式化_函("键_小写 %d", i)) == i);
  for (int i = 5000; i < 6000; i++)
    assert(哈希映射_获取(映射_小写, "no such 键_小写") == NULL);
  for (int i = 6000; i < 7000; i++)
    哈希映射_放置(映射_小写, 格式化_函("键_小写 %d", i), (void *)(size_t)i);

  assert(哈希映射_获取(映射_小写, "no such 键_小写") == NULL);
  printf("OK\n");
}
