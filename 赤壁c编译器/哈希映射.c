// This is an implementation of the open-addressing hash table.

#include "chibicc.h"

// Initial hash bucket 大小
#define 初始的哈希桶大小 16

// Rehash if the 用法_小 exceeds 70%.
#define 高度_水印 70

// We'll keep the 用法_小 below 50% after rehashing.
#define 低_水印 50

// Represents a deleted hash entry
#define 墓碑 ((void *)-1)

static uint64_t fnv_哈希(char *s, int 长度_短) {
  uint64_t hash = 0xcbf29ce484222325;
  for (int i = 0; i < 长度_短; i++) {
    hash *= 0x100000001b3;
    hash ^= (unsigned char)s[i];
  }
  return hash;
}

// Make room for 新的 entires in a given hashmap by removing
// tombstones and possibly extending the bucket 大小.
static void 重新哈希(哈希映射 *映射) {
  // Compute the 大小 of the 新的 hashmap.
  int nkeys = 0;
  for (int i = 0; i < 映射->容量; i++)
    if (映射->桶[i].键 && 映射->桶[i].键 != 墓碑)
      nkeys++;

  int cap = 映射->容量;
  while ((nkeys * 100) / cap >= 低_水印)
    cap = cap * 2;
  assert(cap > 0);

  // Create a 新的 hashmap and copy all 键-values.
  哈希映射 map2 = {};
  map2.桶 = calloc(cap, sizeof(哈希入口));
  map2.容量 = cap;

  for (int i = 0; i < 映射->容量; i++) {
    哈希入口 *ent = &映射->桶[i];
    if (ent->键 && ent->键 != 墓碑)
      哈希映射_放置2(&map2, ent->键, ent->键长度, ent->值_短);
  }

  assert(map2.已用 == nkeys);
  *映射 = map2;
}

static bool 匹配(哈希入口 *ent, char *键, int 键长度) {
  return ent->键 && ent->键 != 墓碑 &&
         ent->键长度 == 键长度 && memcmp(ent->键, 键, 键长度) == 0;
}

static 哈希入口 *取_入口(哈希映射 *映射, char *键, int 键长度) {
  if (!映射->桶)
    return NULL;

  uint64_t hash = fnv_哈希(键, 键长度);

  for (int i = 0; i < 映射->容量; i++) {
    哈希入口 *ent = &映射->桶[(hash + i) % 映射->容量];
    if (匹配(ent, 键, 键长度))
      return ent;
    if (ent->键 == NULL)
      return NULL;
  }
  不可达();
}

static 哈希入口 *取_或_插入_入口(哈希映射 *映射, char *键, int 键长度) {
  if (!映射->桶) {
    映射->桶 = calloc(初始的哈希桶大小, sizeof(哈希入口));
    映射->容量 = 初始的哈希桶大小;
  } else if ((映射->已用 * 100) / 映射->容量 >= 高度_水印) {
    重新哈希(映射);
  }

  uint64_t hash = fnv_哈希(键, 键长度);

  for (int i = 0; i < 映射->容量; i++) {
    哈希入口 *ent = &映射->桶[(hash + i) % 映射->容量];

    if (匹配(ent, 键, 键长度))
      return ent;

    if (ent->键 == 墓碑) {
      ent->键 = 键;
      ent->键长度 = 键长度;
      return ent;
    }

    if (ent->键 == NULL) {
      ent->键 = 键;
      ent->键长度 = 键长度;
      映射->已用++;
      return ent;
    }
  }
  不可达();
}

void *哈希映射_取(哈希映射 *映射, char *键) {
  return 哈希映射_取2(映射, 键, strlen(键));
}

void *哈希映射_取2(哈希映射 *映射, char *键, int 键长度) {
  哈希入口 *ent = 取_入口(映射, 键, 键长度);
  return ent ? ent->值_短 : NULL;
}

void 哈希映射_放置(哈希映射 *映射, char *键, void *值_短) {
   哈希映射_放置2(映射, 键, strlen(键), 值_短);
}

void 哈希映射_放置2(哈希映射 *映射, char *键, int 键长度, void *值_短) {
  哈希入口 *ent = 取_或_插入_入口(映射, 键, 键长度);
  ent->值_短 = 值_短;
}

void 哈希映射_删除(哈希映射 *映射, char *键) {
  哈希映射_删除2(映射, 键, strlen(键));
}

void 哈希映射_删除2(哈希映射 *映射, char *键, int 键长度) {
  哈希入口 *ent = 取_入口(映射, 键, 键长度);
  if (ent)
    ent->键 = 墓碑;
}

void 哈希映射_测试(void) {
  哈希映射 *映射 = calloc(1, sizeof(哈希映射));

  for (int i = 0; i < 5000; i++)
    哈希映射_放置(映射, 格式化("键 %d", i), (void *)(size_t)i);
  for (int i = 1000; i < 2000; i++)
    哈希映射_删除(映射, 格式化("键 %d", i));
  for (int i = 1500; i < 1600; i++)
    哈希映射_放置(映射, 格式化("键 %d", i), (void *)(size_t)i);
  for (int i = 6000; i < 7000; i++)
    哈希映射_放置(映射, 格式化("键 %d", i), (void *)(size_t)i);

  for (int i = 0; i < 1000; i++)
    assert((size_t)哈希映射_取(映射, 格式化("键 %d", i)) == i);
  for (int i = 1000; i < 1500; i++)
    assert(哈希映射_取(映射, "no such 键") == NULL);
  for (int i = 1500; i < 1600; i++)
    assert((size_t)哈希映射_取(映射, 格式化("键 %d", i)) == i);
  for (int i = 1600; i < 2000; i++)
    assert(哈希映射_取(映射, "no such 键") == NULL);
  for (int i = 2000; i < 5000; i++)
    assert((size_t)哈希映射_取(映射, 格式化("键 %d", i)) == i);
  for (int i = 5000; i < 6000; i++)
    assert(哈希映射_取(映射, "no such 键") == NULL);
  for (int i = 6000; i < 7000; i++)
    哈希映射_放置(映射, 格式化("键 %d", i), (void *)(size_t)i);

  assert(哈希映射_取(映射, "no such 键") == NULL);
  printf("OK\n");
}
