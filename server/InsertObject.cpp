#include "dict.hpp"
#include "sds.hpp"
#include "adlist.hpp"

//将给定的键值对添加到字典里面
bool dictInsertList(dict *d, char *key, char *val) {
	unsigned int index;
	dictEntry *node;
	//不存在——进行插入操作
	if (!(node = lookup(d, key)))
	{
		index = d->type->hashFunction(key, d->ht->size);
		// printf("key : %s\n",key);
		// printf("index :%d\n",index);
		node = (dictEntry *)malloc(sizeof(dictEntry));
		if (node == NULL)
			return false;
		// node->key=d->type->keyDup(key);
		// strcpy(node->key,key);
		// TODO
		node->key = (char *)malloc(sizeof(char) * (strlen(key) + 1));
		node->key = new sdshdr(key);
		// strcpy(node->key,key);
		node->value = listCreate();
		// node->value="";
		node->next = d->ht->table[index];
		d->ht->table[index] = node;
	}
	//若存在——进行更新操作,直接修改其对应的value值
	// TODO
	// node->value=(char*)malloc(sizeof(char)*(strlen(val)+1));
	// node->value = (list*)(node->value);
	// node->value = listCreate();
	// strcpy(node->value,val);
	node->value = listAddNodeTail((list *)(node->value), val);
	return true;
}

bool dictInsertSDS(dict *d, char *key, char *val) {
	unsigned int index;
	dictEntry *node;
	//不存在——进行插入操作
	if (!(node = lookup(d, key)))
	{
		index = d->type->hashFunction(key, d->ht->size);
		// printf("key : %s\n",key);
		// printf("index :%d\n",index);
		node = (dictEntry *)malloc(sizeof(dictEntry));
		if (node == NULL)
			return false;
		// node->key=d->type->keyDup(key);
		// strcpy(node->key,key);
		// TODO
		node->key = new sdshdr(key);
		node->value = sdsnew(val);
		// node->value="";
		node->next = d->ht->table[index];
		d->ht->table[index] = node;
	}
	//若存在——进行更新操作,直接修改其对应的value值
	// TODO
	// node->value=(char*)malloc(sizeof(char)*(strlen(val)+1));
	// node->value = (list*)(node->value);
	// node->value = listCreate();
	// strcpy(node->value,val);
	else
	{
		node->value = sdscpy((sds)(node->value), val);
	}

	return true;
}

bool dictInsertDict(dict *d, char *key, char *K, char *V)
{
	unsigned int index;
	dictEntry *node;
	//不存在——进行插入操作
	if (!(node = lookup(d, key)))
	{
		index = d->type->hashFunction(key, d->ht->size);
		// printf("key : %s\n",key);
		// printf("index :%d\n",index);
		node = (dictEntry *)malloc(sizeof(dictEntry));
		if (node == NULL)
			return false;
		// node->key=d->type->keyDup(key);
		// strcpy(node->key,key);
		// TODO
		node->key = new sdshdr(key);

		dictType *type = (dictType *)malloc(sizeof(dictType));
		type->hashFunction = myHashFunction;
		type->keyDup = myKeyDup;
		type->valDup = myValDup;
		type->keyCompare = myKeyCompare;
		type->keyDestructor = myKeyDestructor;
		type->valDestructor = myValDestructor;

		node->value = dictCreate(type, 16);
		// node->value="";
		node->next = d->ht->table[index];
		d->ht->table[index] = node;
	}

	return dictInsertSDS((dict *)(node->value), K, V);
	;
}

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
