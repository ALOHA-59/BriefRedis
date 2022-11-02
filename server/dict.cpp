#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "dict.hpp"
#include "sds.hpp"
#include "adlist.hpp"


using namespace std;
//哈希表的大小
#define HASHSIZE 16

//创建一个新的字典
dict *dictCreate(dictType *type,int hashSize){
	dict* d=(dict*)malloc(sizeof(dict));
	if(d==NULL){
		printf("PIG Redis ERROR : Dict malloc fail.\n");
		return NULL;
	}
	//对hashTable进行相关操作的特定函数集
	if(type==NULL){
		printf("PIG Redis WARNING : Type is NULL.\n");
	}
	d->type=type;
	//哈希表初始化
	d->ht=(dictht*)malloc(sizeof(dictht));
	d->ht->size=hashSize;
	d->ht->used=0;
	d->ht->table=(dictEntry**)malloc(sizeof(dictEntry*)*hashSize);
	//全部结点都设为NULL
	for(int i=0;i<hashSize;i++){
		d->ht->table[i]=NULL;
	}
	return d;
}

//根据key寻找其在hashTable中对应的结点
dictEntry* lookup(dict *d,char* key){
	dictEntry* node;
	//该key在hashTable中对应的下标
	unsigned int index;
	index=d->type->hashFunction(key,d->ht->size);
	//printf("%s------%d\n",key,index);
	if(d->ht->table[index]==NULL){
		//printf("NULL\n");
		return NULL;
	}
	for(node=d->ht->table[index];node;node=node->next){
		if(!(d->type->keyCompare(key,((sds)node->key)->buf))){
			return node;
		}
	}
	return NULL;
}

//返回给定的键的值
void* dictFetchValue(dict *d, char* key){
	dictEntry* node;
	//找不到这个结点
	if(!(node=lookup(d,key))){
		return nullptr;
	}
	//cout<<node->value<<"coutcoutcout"<<endl;
	//printf("printf : %s\n",node->value);
	return node->value;
}

//从字典中删除给定键所对应的键值对
void dictDelete(dict *d, char* key){
	dictEntry* node;
	dictEntry* temp;
	//该key在hashTable中对应的下标
	unsigned int index;
	index=d->type->hashFunction(key,d->ht->size);
	node=d->ht->table[index];
	//key相同
	if(!(d->type->keyCompare(key,((sds)node->key)->buf))){
		d->ht->table[index]=node->next;
		d->type->keyDestructor(node->key);
		d->type->valDestructor(node->value);
		free(node);
		return;
	}
	temp=node;
	node=node->next;
	while(node){
		if(!(d->type->keyCompare(key,node->key))){
			temp->next=node->next;
			d->type->keyDestructor(node->key);
			d->type->valDestructor(node->value);			
			free(node);
			return;
		}
		temp=node;
		node=node->next;
	}
	return;
}
//释放给定字典，以及字典中包含的所有键值对
void dictRelease(dict *d){
	if(d==NULL)return;
	dictEntry* node;
	dictEntry* temp;
	for(int i=0;i<d->ht->size;i++){
		node=d->ht->table[i];
		while(node!=NULL){
			//printf("hh\n");
			//char* t=(char*)node->value;
			temp=node;
			node=node->next;
			d->type->keyDestructor(temp->key);
			d->type->valDestructor(temp->value);
			free(temp);
		}
	}
	free(d->ht);
/*	free(d->type);*/
	free(d);
}

//计算哈希值的函数
unsigned int myHashFunction(void *key,int size){
	char* charkey=(char*)key;
	unsigned int hash=0;
	for(; *charkey; ++charkey){
		hash = hash * 33 + *charkey;
	}
	return hash%size;
}

//复制键的函数
void *myKeyDup(void *key){
	return key;
}
//复制值的函数
void *myValDup(void *obj){
	return obj;
}
//对比键的函数
int myKeyCompare(void *key1, void *key2){
	char*charkey1=(char*)key1;
	char*charkey2=(char*)key2;
	return strcmp(charkey1,charkey2);
}
//销毁键的函数
void myKeyDestructor(void *key){
	//free(key);
}
//销毁值的函数
void myValDestructor(void *obj){
	//free(obj);
}


/*
int main(){
	dictType*type=(dictType*)malloc(sizeof(dictType));
	type->hashFunction=myHashFunction;
	type->keyDup=myKeyDup;
	type->valDup=myValDup;
	type->keyCompare=myKeyCompare;
	type->keyDestructor=myKeyDestructor;
	type->valDestructor=myValDestructor;
	dict* d=dictCreate(type,HASHSIZE);
	
	char*key1="sss";
	char*value1="111";
	bool result=dictInsertSDS(d,key1,value1);
	if(result){
		printf("insert1 success\n");
	}else{
		printf("insert1 fail\n");
	}

	char*key2="sss";
	char*value2="ddd";
	result=dictInsertSDS(d,key2,value2);
	if(result){
		printf("insert2 success\n");
	}else{
		printf("insert2 fail\n");
	}

	char*key3="dss";
	char* K = "key";
	char*value3="1ss";
	result=dictInsertDict(d,key3, K, value3);
	if(result){
		printf("insert3 success\n");
	}else{
		printf("insert3 fail\n");
	}
	
	dict* value4_dict = (dict*)dictFetchValue(d,key3);
	sds value4 = (sds)dictFetchValue(value4_dict, K);
	if(value4 == nullptr) {
		printf("nullptr");
	}
	printf("%s\n", value4->buf);

	sds value5 = (sds)dictFetchValue(d, key2);
	
	printf("%s\n", value5->buf);
	// printf("%d\n", value4->len);
	// char *value4=(char*)dictFetchValue(d,key3);
	// printf("---%s\n",value4);

	// dictDelete(d,key3);
	// value4=(char*)dictFetchValue(d,key3);
	// printf("---%s\n",value4);

	// dictRelease(d);
	// system("pause");
	return 0;
}


*/