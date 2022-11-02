#ifndef __REDIS_H__
#define __REDIS_H__
#include <string>
#include "InsertObject.hpp"
using namespace std;
//数据库
typedef struct redisDb{
	//数据库在服务端对应的Id
	int id;
	//数据库中用来存放数据的字典类型
	//字典类型中的hashTable用来存放键值对
	dict* dictdb;
}redisDb;

typedef struct value {
	void* value_k;
	void* value_v;
	value(void* para_1, void* para_2) : value_k(para_1), value_v(para_2) { }
	value(void* para_2) : value_v(para_2) {}
} value;

//初始化一个数据库
redisDb* redisDbCreate(dictType*type,int hashSize,int id);

//初始化一个数据库
redisDb* redisDbCreate(dictType*type,int hashSize);

//向数据库中添加/更新新建
bool redisDbInsert(redisDb*db, char* key, value* val , int cap );

//从数据库中删除键
void redisDbDelete(redisDb*db,char* key);

//从数据库中获取给定key对应的value
char* redisDbFetchValue(redisDb*db,char* key);

//释放数据库
void redisDbRelease(redisDb*db);

#endif