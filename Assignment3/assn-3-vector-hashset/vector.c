#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "search.h"

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	assert(elemSize > 0);
	assert(initialAllocation >= 0);
	if (initialAllocation <= 0)
		initialAllocation = 4;
	v->elems = malloc(initialAllocation * elemSize);
	v->elemSize = elemSize;
	v->logLength = 0;
	v->allocLength = initialAllocation;
	v->VectorFreeFunction = freeFn;
	assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
	if (v->VectorFreeFunction != NULL){
		for (int i = 0;i < (v->logLength);i++){
			void * addr = (char*)v->elems + i * v->elemSize;
			v->VectorFreeFunction(addr);
		}
	}
	free (v->elems);
}

int VectorLength(const vector *v)
{
	return v->logLength; 
}

void *VectorNth(const vector *v, int position)
{
	void * temp;
	assert(position >= 0);
	assert(position < (v->logLength));
	temp = (char*) v->elems + position * v->elemSize;
	return temp;
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	void* temp;
	assert(position >= 0);
	assert(position < (v->logLength));
	temp = (char*)v->elems + position * v->elemSize;	
	if(v->VectorFreeFunction != NULL){
		v->VectorFreeFunction(temp);
	}
	memcpy(temp,elemAddr,v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	void* temp;
	assert(position >= 0);
	assert(position <= v->logLength);
	if (v->logLength == v->allocLength){	
		v->elems = realloc(v->elems,(v->allocLength + 4) * v->elemSize);
		v->allocLength += 4;
	}	
	if (position == v->logLength)
		VectorAppend(v,elemAddr);
	else{
	temp = (char*)v->elems + position * v->elemSize;
	memmove((char*)temp + v->elemSize,temp,(v->logLength - position) * v->elemSize);
	memcpy(temp,elemAddr,v->elemSize);
	v->logLength++;}
}

void VectorAppend(vector *v, const void *elemAddr)
{	
	if (v->logLength == v->allocLength){	
		v->elems = realloc(v->elems,(v->allocLength + 4) * v->elemSize);
		v->allocLength += 4;
	}
	memcpy((char*)v->elems + v->logLength * v->elemSize,elemAddr,v->elemSize);
	v->logLength++;
}

void VectorDelete(vector *v, int position)
{
	void* temp;
	assert(position >= 0);
	assert(position < (v->logLength));
	temp = (char*)v->elems + position * v->elemSize;
	if(v->VectorFreeFunction != NULL){
		v->VectorFreeFunction(temp);
	}
	memmove(temp,(char*)temp + v->elemSize,(v->logLength - position - 1) * v->elemSize);	
	v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	assert(compare != NULL);
	qsort(v->elems,v->logLength,v->elemSize,compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	assert(mapFn != NULL);
	for (int i = 0;i < v->logLength;i++){
		mapFn(((char*)v->elems + i * v->elemSize),auxData);
	}
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
	void* temp;
	long unsigned int nmemb = v->logLength - startIndex;
	assert(searchFn != NULL);
	assert(startIndex >= 0);
	assert(startIndex < (v->logLength));
	if (isSorted == true){
		temp = bsearch(key,(char*)v->elems + startIndex * v->elemSize,v->logLength - startIndex,v->elemSize,searchFn);
		if (temp == NULL)
			return -1;
		return ((char*)temp - (char*)v->elems)/(v->elemSize);
	}
	else {
		temp = lfind(key,(char*)v->elems + startIndex * v->elemSize,&nmemb,v->elemSize,searchFn);
		if (temp == NULL)
			return -1;
		return ((char*)temp - (char*)v->elems)/(v->elemSize);
	}
} 