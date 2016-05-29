#include "CatchMemoryLeak.h"


// CatchMemoryLeak.cpp
#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;
struct HeapInfo_s {
	string fileName;
	unsigned long lineNo;
	adrInHeap;
	unsigned long nBytes;
};
typedef map<unsigned long, HeapInfo_s* > heapStorage_t;
heapStorage_t* heapStorage; // global variable is initialized to 0
void saveInStorage(unsigned long addr,  unsigned long nBytes, 
	const char* fileName, unsigned long lineNo)
{
	HeapInfo_s* hInfo;
	if(!heapStorage) {
		heapStorage = new(heapStorage_t);
	}
	hInfo = new (HeapInfo_s);
	hInfo->adrInHeap = addr;
	hInfo->fileName = fileName;
	hInfo->lineNo = lineNo;
	hInfo->nBytes = nBytes;
	(*heapStorage)[addr] = hInfo;
}

void removeFromStorage(unsigned long addr) {
	if( heapStorage) {
		heapStorage_t::iterator itor;
		itor = heapStorage->find(addr);
		if (itor != heapStorage->end() ) {
			heapStorage->erase((itor));
		}
	}
}
void reportUnreleasedHeap() {
	ofstream ofs("Leaks.txt");
	if( heapStorage) {
		heapStorage_t::iterator itor;
		for(itor = heapStorage->begin(); 
			itor != heapStorage->end(); 
			++itor) {
				ofs << "File Name : " << (*itor).second->fileName << endl;
				ofs << "Line No   : " << (*itor).second->lineNo << endl;
				ofs << "Number of unreleased bytes : " 
					<<  (*itor).second->nBytes << endl;
				ofs << endl;
		}
	}
}