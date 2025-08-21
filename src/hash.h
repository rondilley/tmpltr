/*****
 *
 * Description: Hash Function Headers
 *
 * Copyright (c) 2009-2025, Ron Dilley
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

#ifndef HASH_DOT_H
#define HASH_DOT_H

/****
 *
 * includes
 *
 ****/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
#error something is messed up
#endif

#include "mem.h"
#include "util.h"
#include "../include/common.h"
#include <stdint.h>

/****
 *
 * typedefs and enums
 *
 ****/

struct hashRec_s
{
  char *keyString;
  int keyLen;
  uint32_t hashValue;    /* Cached hash value for faster lookups */
  void *data;
  time_t lastSeen;
  time_t createTime;
  uint32_t accessCount;
  uint16_t modifyCount;
  struct hashRec_s *next;  /* For linked list in buckets */
};


/* Memory pool for hash records */
struct hashRecPool_s
{
  struct hashRec_s *records;
  size_t capacity;
  size_t used;
  struct hashRecPool_s *next;
};

struct hash_s
{
  uint32_t size;
  uint32_t totalRecords;
  uint16_t maxDepth;
  uint8_t primeOff;
  struct hashRec_s **buckets;      /* Optimized bucket array using FNV-1a */
  struct hashRecPool_s *pools;     /* Memory pools for records */
};

/****
 *
 * function prototypes
 *
 ****/

uint32_t calcHash(uint32_t hashSize, const char *keyString);
uint32_t fnv1aHash(const char *keyString, int keyLen);
uint32_t djb2Hash(const char *keyString, int keyLen);
uint32_t calcHashWithLen(const char *keyString, int keyLen);
struct hash_s *initHash(uint32_t hashSize);
void freeHash(struct hash_s *hash);

struct hashRec_s *addUniqueHashRec(struct hash_s *hash, const char *keyString, int keyLen, void *data);
struct hashRec_s *addUniqueHashRecWithHash(struct hash_s *hash, const char *keyString, int keyLen, uint32_t hashValue, void *data);
int insertUniqueHashRec(struct hash_s *hash, struct hashRec_s *hashRec);

struct hashRec_s *getHashRecord(struct hash_s *hash, const char *keyString, int keyLen);
struct hashRec_s *snoopHashRecord(struct hash_s *hash, const char *keyString, int keyLen);
void *getHashData(struct hash_s *hash, const char *keyString, int keyLen);
void *snoopHashData(struct hash_s *hash, const char *keyString, int keyLen);

void *deleteHashRecord(struct hash_s *hash, const char *keyString, int keyLen);

struct hash_s *dyGrowHash(struct hash_s *oldHash);
struct hash_s *dyShrinkHash(struct hash_s *oldHash);

int traverseHash(const struct hash_s *hash, int (*fn)(const struct hashRec_s *hashRec));

void **purgeOldHashRecords(struct hash_s *hash, time_t age, void **dataList);

char *hexConvert(const char *keyString, int keyLen, char *buf, const int bufLen);
char *utfConvert(const char *keyString, int keyLen, char *buf, const int bufLen);
uint32_t getHashSize(struct hash_s *hash);

#endif /* end of HASH_DOT_H */
