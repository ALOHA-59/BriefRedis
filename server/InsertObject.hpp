#ifndef INSERTOBJECT_H
#define INSERTOBJECT_H

#include "dict.hpp"
#include "sds.hpp"
#include "adlist.hpp"

bool dictInsertList(dict *d, char* key, char* val);
bool dictInsertSDS(dict *d, char* key, char* val);
bool dictInsertDict(dict *d, char* key, char* K, char * V);
bool dictInsertDict(dict *d, char* key, char* K, char * V);

// bool dictInsertSkipList(dict *d, char* key, char* val){
// 	unsigned int index;
// 	dictEntry* node;
// 	//不存在——进行插入操作
// 	if(!(node=lookup(d,key))){
// 		index=d->type->hashFunction(key,d->ht->size);
// 		//printf("key : %s\n",key);
// 		//printf("index :%d\n",index);
// 		node=(dictEntry*)malloc(sizeof(dictEntry));
// 		if(node==NULL)return false;
// 		//node->key=d->type->keyDup(key);
// 		//strcpy(node->key,key);
// 		//TODO
// 		node->key = new sdshdr(key);
// 		node->value = new SkipList<int, string>(6);
// 		//node->value="";
// 		node->next=d->ht->table[index];
// 		d->ht->table[index]=node;
// 	}
// 	//若存在——进行更新操作,直接修改其对应的value值
// 	//TODO
// 	// node->value=(char*)malloc(sizeof(char)*(strlen(val)+1));
//     // node->value = (list*)(node->value);
//     // node->value = listCreate();
// 	// strcpy(node->value,val); 
// 	else {
// 		node->value = sdscpy((sds)(node->value), val);
// 	}
	
// 	return true;
// }
# endif