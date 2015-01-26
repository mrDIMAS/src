#ifndef _MEMORY_MANAGER_
#define _MEMORY_MANAGER_

#include <iostream>
#include <stdlib.h>

using namespace std;

class MemoryManager {
private:
	struct MemoryNode {	
		void * data;
		int size;
		MemoryNode * next;
		MemoryNode * prev;
		explicit MemoryNode();
	};
	MemoryNode * msRoot;
	MemoryNode * msLast;	
	int mTotalBytesAllocated;	
	explicit MemoryManager();
	~MemoryManager();	
public:
	static MemoryManager & Instance();
	void * Allocate( int count );
	void Free( void * data );
};




#endif