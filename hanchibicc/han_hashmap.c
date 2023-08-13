//han_hashmap.c

#include "han_chibicc.h"

// 这是一个开放寻址哈希表的实现。
// 初始哈希桶大小
#define INIT_SIZE 16

// 如果使用率超过70%，则重新哈希。
#define HIGH_WATERMARK 70

// 重新哈希后，我们将保持使用率低于50%。
#define LOW_WATERMARK 50

// 表示已删除的哈希条目
#define TOMBSTONE ((void *)-1)

// 使用FNV算法计算哈希值
static uint64_t fnv_hash(char *s, int len) {
  uint64_t hash = 0xcbf29ce484222325;  // 初始化哈希值为FNV偏移基准值
  for (int i = 0; i < len; i++) {
    hash *= 0x100000001b3;  // 使用FNV乘法器乘以标准FNV素数
    hash ^= (unsigned char)s[i];  // 将当前字符异或到哈希值中
  }
  return hash;  // 返回计算得到的哈希值
}

// 通过移除墓碑（已删除的条目）和可能扩展桶大小，为给定哈希映射中的新条目腾出空间。
static void rehash(HashMap *map) {
  // 计算新哈希映射的大小。
  int nkeys = 0;  // 用于记录有效键值对的数量
  for (int i = 0; i < map->capacity; i++)
    if (map->buckets[i].key && map->buckets[i].key != TOMBSTONE)
      nkeys++;  // 统计有效键值对的数量

  int cap = map->capacity;  // 当前桶大小
  while ((nkeys * 100) / cap >= LOW_WATERMARK)  // 如果使用率超过最低水位线
    cap = cap * 2;  // 扩大桶大小为原来的两倍
  assert(cap > 0);  // 断言桶大小大于0


	// 创建一个新的哈希映射并复制所有键值对。
  HashMap map2 = {};  // 创建新的哈希映射对象
  map2.buckets = calloc(cap, sizeof(HashEntry));  // 分配新的桶数组内存空间
  map2.capacity = cap;  // 设置新的哈希桶大小

  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[i];  // 获取当前位置的哈希条目指针
    if (ent->key && ent->key != TOMBSTONE)
      hashmap_put2(&map2, ent->key, ent->keylen, ent->val);  // 将有效的键值对插入新的哈希映射
  }

  assert(map2.used == nkeys);  // 断言新的哈希映射中有效键值对的数量与预期相等
  *map = map2;  // 将原哈希映射替换为新的哈希映射
}

// 判断哈希条目与给定键是否匹配
static bool match(HashEntry *ent, char *key, int keylen) {
  return ent->key && ent->key != TOMBSTONE &&  // 判断哈希条目是否有效且不是墓碑
         ent->keylen == keylen && memcmp(ent->key, key, keylen) == 0;  // 比较键的长度和内容是否相等
}

// 根据键获取哈希条目指针
static HashEntry *get_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets)
    return NULL;

  uint64_t hash = fnv_hash(key, keylen);  // 计算键的哈希值

  for (int i = 0; i < map->capacity; i++) {
    HashEntry *ent = &map->buckets[(hash + i) % map->capacity];  // 计算哈希条目的位置
    if (match(ent, key, keylen))  // 判断哈希条目是否与给定键匹配
      return ent;  // 返回匹配的哈希条目
    if (ent->key == NULL)
      return NULL;  // 如果哈希条目的键为空，表示未找到匹配的键，返回NULL
  }
  unreachable();  // 不应该执行到这里，表示逻辑错误
}

// 获取或插入哈希条目
static HashEntry *get_or_insert_entry(HashMap *map, char *key, int keylen) {
  if (!map->buckets) {
    // 如果哈希映射的桶数组为空，则进行初始化
    map->buckets = calloc(INIT_SIZE, sizeof(HashEntry));  // 分配初始大小的桶数组内存空间
    map->capacity = INIT_SIZE;  // 设置初始桶大小
  } else if ((map->used * 100) / map->capacity >= HIGH_WATERMARK) {
    // 如果哈希映射的使用率超过高水位线，则进行重新哈希
    rehash(map);
  }

  uint64_t hash = fnv_hash(key, keylen);  // 计算键的哈希值

  for (int i = 0; i < map->capacity; i++) {  // 遍历哈希表中的每个元素
    HashEntry *ent = &map->buckets[(hash + i) % map->capacity];  // 获取当前位置的哈希桶

    if (match(ent, key, keylen))  // 如果当前位置的键与给定的键匹配，则返回当前位置的条目
      return ent;

    if (ent->key == TOMBSTONE) {  // 如果当前位置的键是墓碑值（已删除的键），则将其替换为新的键并返回当前位置的条目
      ent->key = key;
      ent->keylen = keylen;
      return ent;
    }

    if (ent->key == NULL) {  // 如果当前位置的键为空，则将其替换为新的键，并更新哈希表的使用数，并返回当前位置的条目
      ent->key = key;
      ent->keylen = keylen;
      map->used++;
      return ent;
    }
  }
  unreachable();  // 如果执行到这里，表示哈希表已满且无法找到合适的位置来插入键值对，此时应该是不可达的代码，即不应该执行到这里
}

// 使用键获取哈希表中的值
void *hashmap_get(HashMap *map, char *key) {
  return hashmap_get2(map, key, strlen(key));  // 调用具有键长度的哈希表获取函数
}

// 使用键和键长度获取哈希表中的值
void *hashmap_get2(HashMap *map, char *key, int keylen) {
  HashEntry *ent = get_entry(map, key, keylen);  // 获取键对应的哈希条目
  return ent ? ent->val : NULL;  // 如果哈希条目存在，则返回对应的值；否则返回空指针
}

// 将键值对存入哈希表
void hashmap_put(HashMap *map, char *key, void *val) {
   hashmap_put2(map, key, strlen(key), val);  // 调用具有键长度的哈希表存入函数
}

// 使用键、键长度和值将键值对存入哈希表
void hashmap_put2(HashMap *map, char *key, int keylen, void *val) {
  HashEntry *ent = get_or_insert_entry(map, key, keylen);  // 获取或插入键对应的哈希条目
  ent->val = val;  // 将值存入哈希条目
}

// 使用键删除哈希表中的键值对
void hashmap_delete(HashMap *map, char *key) {
  hashmap_delete2(map, key, strlen(key));  // 调用具有键长度的哈希表删除函数
}

// 使用键和键长度删除哈希表中的键值对
void hashmap_delete2(HashMap *map, char *key, int keylen) {
  HashEntry *ent = get_entry(map, key, keylen);  // 获取键对应的哈希条目
  if (ent)
    ent->key = TOMBSTONE;  // 如果哈希条目存在，则将其标记为墓碑值（已删除的键）
}

// 哈希表测试函数
void hashmap_test(void) {
  HashMap *map = calloc(1, sizeof(HashMap));  // 创建哈希表

  for (int i = 0; i < 5000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);  // 向哈希表中插入键值对

  for (int i = 1000; i < 2000; i++)
    hashmap_delete(map, format("key %d", i));  // 从哈希表中删除键值对

  for (int i = 1500; i < 1600; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);  // 向哈希表中再次插入键值对

  for (int i = 6000; i < 7000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);  // 继续向哈希表中插入键值对


  for (int i = 0; i < 1000; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);  // 验证哈希表中的值

  for (int i = 1000; i < 1500; i++)
    assert(hashmap_get(map, "no such key") == NULL);  // 验证哈希表中不存在的键的返回值

  for (int i = 1500; i < 1600; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);  // 验证哈希表中更新后的值

  for (int i = 1600; i < 2000; i++)
    assert(hashmap_get(map, "no such key") == NULL);  // 验证哈希表中不存在的键的返回值

  for (int i = 2000; i < 5000; i++)
    assert((size_t)hashmap_get(map, format("key %d", i)) == i);  // 验证哈希表中的值

  for (int i = 5000; i < 6000; i++)
    assert(hashmap_get(map, "no such key") == NULL);  // 验证哈希表中不存在的键的返回值

  for (int i = 6000; i < 7000; i++)
    hashmap_put(map, format("key %d", i), (void *)(size_t)i);  // 继续向哈希表中插入键值对

  assert(hashmap_get(map, "no such key") == NULL);  // 验证哈希表中不存在的键的返回值

  printf("OK\n");  // 测试通过
}