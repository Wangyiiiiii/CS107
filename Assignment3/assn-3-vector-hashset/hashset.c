#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn != NULL);
	assert(comparefn !=NULL);
	h->elems = malloc(numBuckets * sizeof(vector));
	h->numBuckets = numBuckets;
	h->elemSize = elemSize;
	h->hashfn = hashfn;
	h->comparefn = comparefn;
	h->freefn = freefn;
	h->manage = malloc(numBuckets * sizeof(bool));
	for (int i = 0;i < numBuckets;i++){
		h->manage[i] = 0;
	}
}

void HashSetDispose(hashset *h)
{
	if (h->freefn != NULL){
		for (int i = 0;i < h->numBuckets;i++){
			vector* addr = (char*)h->elems + i * sizeof(vector);
			if (h->manage[i] == 1)
				VectorDispose(addr);
		}
	}
	free(h->elems);
	free(h->manage);
}

int HashSetCount(const hashset *h)
{
	 int count = 0; 
	 for (int i = 0;i < h->numBuckets;i++){
	 	vector* addr = (char*)h->elems + i * sizeof(vector);
	 	if (h->manage[i] == 1)
	 		count += VectorLength(addr);
	 }
	 return count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
	for (int i = 0;i < h->numBuckets;i++){
		vector* addr = (char*)h->elems + i * sizeof(vector);
		if (h->manage[i] == 1)
			VectorMap (addr,mapfn,auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	int hashcode;
	vector* addr; 
	assert(elemAddr != NULL);
	hashcode = h->hashfn(elemAddr,h->numBuckets);
	assert(hashcode >= 0);
	assert(hashcode < h->numBuckets);
	addr = (char*)h->elems + hashcode * sizeof(vector);
	if (h->manage[hashcode] == 0){
		vector temp;
		VectorNew(addr,h->elemSize,h->freefn,4);
		VectorAppend(addr,elemAddr);
		// memcpy(addr,&temp,sizeof(vector));
		h->manage[hashcode] = 1;
	}else if(h->manage[hashcode] == 1){
		int position = -1;
		position = VectorSearch(addr,elemAddr,h->comparefn,0,false);
		if (position == -1)
			VectorAppend(addr,elemAddr);
		else 
			VectorReplace(addr,elemAddr,position);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	int hashcode;
	vector* addr;
	int position;

	assert(elemAddr != NULL);
	hashcode = h->hashfn(elemAddr,h->numBuckets);
	assert(hashcode >= 0);
	assert(hashcode < h->numBuckets);
	addr = (char*)h->elems + hashcode * sizeof(vector);
	if (h->manage[hashcode] == 0)
		return NULL;
	position = VectorSearch(addr,elemAddr,h->comparefn,0,false);
	if (position != -1)
		return (VectorNth(addr,position));
	return NULL;
}
