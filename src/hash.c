/*****
 *
 * Description: Optimized Hash Table Implementation
 *
 * Copyright (c) 2008-2023, Ron Dilley
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****/

/****
 *
 * includes
 *
 ****/

#include "hash.h"
#include "mem.h"
#include <stdint.h>

/****
 *
 * local variables
 *
 ****/

/* Prime numbers for hash table sizing */
static size_t hashPrimes[] = {
    53, 97, 193, 389, 769, 1543, 3079,
    6151, 12289, 24593, 49157, 98317, 196613, 393241,
    786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653,
    100663319, 201326611, 402653189, 805306457, 1610612741, 0
};

/* FNV-1a hash constants */
#define FNV_OFFSET_BASIS 2166136261U
#define FNV_PRIME 16777619U

/* Memory pool constants */
#define POOL_SIZE 1024

/****
 *
 * external global variables
 *
 ****/

extern Config_t *config;

/****
 *
 * Memory pool management
 *
 ****/

static struct hashRec_s *allocHashRecord(struct hash_s *hash)
{
  struct hashRecPool_s *pool = hash->pools;
  struct hashRec_s *record;
  
  /* Find pool with available space */
  while (pool && pool->used >= pool->capacity) {
    pool = pool->next;
  }
  
  /* Create new pool if needed */
  if (!pool) {
    if ((pool = (struct hashRecPool_s *)XMALLOC(sizeof(struct hashRecPool_s))) == NULL)
      return NULL;
      
    if ((pool->records = (struct hashRec_s *)XMALLOC(sizeof(struct hashRec_s) * POOL_SIZE)) == NULL) {
      XFREE(pool);
      return NULL;
    }
    
    pool->capacity = POOL_SIZE;
    pool->used = 0;
    pool->next = hash->pools;
    hash->pools = pool;
  }
  
  /* Return next available record */
  record = &pool->records[pool->used++];
  XMEMSET(record, 0, sizeof(struct hashRec_s));
  return record;
}

static void freePools(struct hash_s *hash)
{
  struct hashRecPool_s *pool = hash->pools;
  struct hashRecPool_s *next;
  
  while (pool) {
    next = pool->next;
    if (pool->records)
      XFREE(pool->records);
    XFREE(pool);
    pool = next;
  }
  hash->pools = NULL;
}

/****
 *
 * FNV-1a hash function
 *
 ****/

uint32_t fnv1aHash(const char *keyString, int keyLen)
{
  uint32_t hash = FNV_OFFSET_BASIS;
  int i;
  
  for (i = 0; i < keyLen; i++) {
    hash ^= (uint8_t)keyString[i];
    hash *= FNV_PRIME;
  }
  
  return hash;
}

/****
 *
 * Initialize hash table
 *
 ****/

struct hash_s *initHash(uint32_t hashSize)
{
  struct hash_s *tmpHash;
  int i;
  
  if ((tmpHash = (struct hash_s *)XMALLOC(sizeof(struct hash_s))) == NULL) {
    fprintf(stderr, "ERR - Unable to allocate hash\n");
    return NULL;
  }
  XMEMSET(tmpHash, 0, sizeof(struct hash_s));
  
  /* Pick a good prime hash size */
  for (i = 0; ((hashSize > hashPrimes[i]) && (hashPrimes[i] > 0)); i++)
    ;
  
  if (hashPrimes[i] == 0) {
    fprintf(stderr, "ERR - Hash size too large\n");
    XFREE(tmpHash);
    return NULL;
  }
  
  tmpHash->primeOff = i;
  tmpHash->size = hashPrimes[i];
  
  /* Allocate bucket array */
  if ((tmpHash->buckets = (struct hashRec_s **)XMALLOC(
           sizeof(struct hashRec_s *) * tmpHash->size)) == NULL) {
    fprintf(stderr, "ERR - Unable to allocate hash buckets\n");
    XFREE(tmpHash);
    return NULL;
  }
  XMEMSET(tmpHash->buckets, 0, sizeof(struct hashRec_s *) * tmpHash->size);
  
  tmpHash->pools = NULL;
  tmpHash->totalRecords = 0;
  tmpHash->maxDepth = 0;
  
#ifdef DEBUG
  if (config->debug >= 4)
    printf("DEBUG - Hash initialized [%u]\n", tmpHash->size);
#endif
  
  return tmpHash;
}

/****
 *
 * Free hash table
 *
 ****/

void freeHash(struct hash_s *hash)
{
  uint32_t key;
  struct hashRec_s *record, *next;
  
  if (hash == NULL)
    return;
    
  /* Free bucket chains and key strings */
  if (hash->buckets != NULL) {
    for (key = 0; key < hash->size; key++) {
      record = hash->buckets[key];
      while (record) {
        next = record->next;
        if (record->keyString)
          XFREE(record->keyString);
        record = next;
      }
    }
    XFREE(hash->buckets);
  }
  
  /* Free memory pools */
  freePools(hash);
  
  XFREE(hash);
}

/****
 *
 * Add unique record to hash
 *
 ****/

struct hashRec_s *addUniqueHashRec(struct hash_s *hash, const char *keyString, int keyLen, void *data)
{
  uint32_t hashValue;
  uint32_t bucket;
  struct hashRec_s *record, *newRecord;
  uint16_t depth = 0;
  
  if (!hash || !keyString)
    return NULL;
    
  if (keyLen == 0)
    keyLen = strlen(keyString) + 1;
    
  /* Calculate hash and bucket */
  hashValue = fnv1aHash(keyString, keyLen);
  bucket = hashValue % hash->size;
  
  /* Check for existing record */
  record = hash->buckets[bucket];
  while (record) {
    if (record->hashValue == hashValue &&
        record->keyLen == keyLen &&
        XMEMCMP(record->keyString, keyString, keyLen) == 0) {
      /* Found existing record - update access time */
      record->lastSeen = config->current_time;
      record->accessCount++;
      return NULL; /* Duplicate */
    }
    record = record->next;
    depth++;
  }
  
  /* Allocate new record from pool */
  if ((newRecord = allocHashRecord(hash)) == NULL) {
    fprintf(stderr, "ERR - Unable to allocate hash record\n");
    return NULL;
  }
  
  /* Allocate and copy key string */
  if ((newRecord->keyString = (char *)XMALLOC(keyLen)) == NULL) {
    fprintf(stderr, "ERR - Unable to allocate key string\n");
    return NULL;
  }
  XMEMCPY(newRecord->keyString, keyString, keyLen);
  
  /* Initialize record */
  newRecord->keyLen = keyLen;
  newRecord->hashValue = hashValue;
  newRecord->data = data;
  newRecord->lastSeen = newRecord->createTime = config->current_time;
  newRecord->accessCount = 1;
  newRecord->modifyCount = 0;
  
  /* Add to front of bucket chain */
  newRecord->next = hash->buckets[bucket];
  hash->buckets[bucket] = newRecord;
  
  /* Update statistics */
  hash->totalRecords++;
  if (depth > hash->maxDepth)
    hash->maxDepth = depth;
    
#ifdef DEBUG
  if (config->debug >= 4)
    printf("DEBUG - Added hash record [bucket:%u, depth:%u, total:%u]\n", 
           bucket, depth, hash->totalRecords);
#endif
  
  return newRecord;
}

/****
 *
 * Get hash record
 *
 ****/

struct hashRec_s *getHashRecord(struct hash_s *hash, const char *keyString, int keyLen)
{
  uint32_t hashValue;
  uint32_t bucket;
  struct hashRec_s *record;
  
  if (!hash || !keyString)
    return NULL;
    
  if (keyLen == 0)
    keyLen = strlen(keyString) + 1;
    
  /* Calculate hash and bucket */
  hashValue = fnv1aHash(keyString, keyLen);
  bucket = hashValue % hash->size;
  
  /* Search bucket chain */
  record = hash->buckets[bucket];
  while (record) {
    if (record->hashValue == hashValue &&
        record->keyLen == keyLen &&
        XMEMCMP(record->keyString, keyString, keyLen) == 0) {
      /* Found record - update access info */
      record->lastSeen = config->current_time;
      record->accessCount++;
      return record;
    }
    record = record->next;
  }
  
  return NULL; /* Not found */
}

/****
 *
 * Get hash data
 *
 ****/

void *getHashData(struct hash_s *hash, const char *keyString, int keyLen)
{
  struct hashRec_s *record = getHashRecord(hash, keyString, keyLen);
  return record ? record->data : NULL;
}

/****
 *
 * Traverse hash table
 *
 ****/

int traverseHash(const struct hash_s *hash, int (*fn)(const struct hashRec_s *hashRec))
{
  uint32_t bucket;
  struct hashRec_s *record;
  
  if (!hash || !fn)
    return FAILED;
    
#ifdef DEBUG
  if (config->debug >= 3)
    printf("DEBUG - Traversing hash table\n");
#endif
  
  /* Traverse all buckets and their chains */
  for (bucket = 0; bucket < hash->size; bucket++) {
    record = hash->buckets[bucket];
    while (record) {
      if (fn(record))
        return FAILED;
      record = record->next;
    }
  }
  
  return TRUE;
}

/****
 *
 * Grow hash table dynamically
 *
 ****/

struct hash_s *dyGrowHash(struct hash_s *oldHash)
{
  struct hash_s *newHash;
  uint32_t bucket;
  struct hashRec_s *record, *next, *newRecord;
  
  if (!oldHash || oldHash->primeOff >= (sizeof(hashPrimes)/sizeof(hashPrimes[0]) - 2))
    return oldHash;
    
  /* Create new larger hash */
  if ((newHash = initHash(hashPrimes[oldHash->primeOff + 1])) == NULL) {
    fprintf(stderr, "ERR - Unable to allocate new hash\n");
    return oldHash;
  }
  
  /* Rehash all records */
  for (bucket = 0; bucket < oldHash->size; bucket++) {
    record = oldHash->buckets[bucket];
    while (record) {
      next = record->next;
      
      /* Recalculate bucket in new table */
      uint32_t newBucket = record->hashValue % newHash->size;
      
      /* Allocate new record */
      if ((newRecord = allocHashRecord(newHash)) == NULL) {
        fprintf(stderr, "ERR - Failed to allocate during grow\n");
        freeHash(newHash);
        return oldHash;
      }
      
      /* Copy record (shallow copy of data pointer) */
      newRecord->keyLen = record->keyLen;
      newRecord->hashValue = record->hashValue;
      newRecord->data = record->data;
      newRecord->lastSeen = record->lastSeen;
      newRecord->createTime = record->createTime;
      newRecord->accessCount = record->accessCount;
      newRecord->modifyCount = record->modifyCount;
      
      /* Allocate and copy key */
      if ((newRecord->keyString = (char *)XMALLOC(record->keyLen)) == NULL) {
        fprintf(stderr, "ERR - Failed to allocate key during grow\n");
        freeHash(newHash);
        return oldHash;
      }
      XMEMCPY(newRecord->keyString, record->keyString, record->keyLen);
      
      /* Add to new hash bucket */
      newRecord->next = newHash->buckets[newBucket];
      newHash->buckets[newBucket] = newRecord;
      newHash->totalRecords++;
      
      record = next;
    }
  }
  
#ifdef DEBUG
  if (config->debug >= 2)
    printf("DEBUG - Grew hash from %u to %u buckets\n", oldHash->size, newHash->size);
#endif
  
  /* Free old hash */
  freeHash(oldHash);
  
  return newHash;
}

/****
 *
 * Calculate hash value with length (compatibility wrapper)
 *
 ****/

uint32_t calcHashWithLen(const char *keyString, int keyLen)
{
  return fnv1aHash(keyString, keyLen);
}

/****
 *
 * Calculate hash value (compatibility wrapper)
 *
 ****/

uint32_t calcHash(uint32_t hashSize, const char *keyString)
{
  return fnv1aHash(keyString, strlen(keyString) + 1) % hashSize;
}

/****
 *
 * Hex conversion utility
 *
 ****/

char *hexConvert(const char *keyString, int keyLen, char *buf, const int bufLen)
{
  int i, j = 0;
  
  for (i = 0; i < keyLen && j < bufLen - 3; i++) {
    sprintf(buf + j, "%02x", (unsigned char)keyString[i]);
    j += 2;
  }
  buf[j] = '\0';
  
  return buf;
}

/****
 *
 * Get hash size
 *
 ****/

uint32_t getHashSize(struct hash_s *hash)
{
  return hash ? hash->size : 0;
}