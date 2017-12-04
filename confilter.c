#include <string.h>
#include <stdlib.h>
#include "redismodule.h"

int command_cf(RedisModuleCtx* ctx, RedisModuleString** argv, int argc) {
  if (argc != 3)
    return RedisModule_WrongArity(ctx);

  RedisModuleKey* key = RedisModule_OpenKey(ctx,argv[1], REDISMODULE_READ);
  size_t len;
  const char* content = RedisModule_StringPtrLen(argv[2], &len);
  
  double score;
  RedisModuleString* ele;
  RedisModule_ZsetFirstInScoreRange(key, REDISMODULE_NEGATIVE_INFINITE, REDISMODULE_POSITIVE_INFINITE, 0, 0);

  int found = 0;
  while (!RedisModule_ZsetRangeEndReached(key)) {
    ele = RedisModule_ZsetRangeCurrentElement(key, &score);

    size_t word_len;
    const char* word = RedisModule_StringPtrLen(ele, &word_len);
    if (strstr(content, word) != NULL) {
	found = 1;
	break;
    }
    RedisModule_ZsetRangeNext(key);
  }
  RedisModule_ZsetRangeStop(key);
  RedisModule_CloseKey(key);
  
  RedisModule_ReplyWithLongLong(ctx, found);
  return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx* ctx, RedisModuleString** argv, int argc) {
  REDISMODULE_NOT_USED(argv);
  REDISMODULE_NOT_USED(argc);
  
  if (RedisModule_Init(ctx, "confilter", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR)
    return REDISMODULE_ERR;

  if (RedisModule_CreateCommand(ctx, "cf", command_bl, "readonly", 1, 1, 1) == REDISMODULE_ERR)
    return REDISMODULE_ERR;
  
  return REDISMODULE_OK;
}
