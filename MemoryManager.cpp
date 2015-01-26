#include "MemoryManager.hpp"
#include <malloc.h>
#include <windows.h>

void * operator new( std::size_t count ) throw( std::bad_alloc ) {
	return MemoryManager::Instance().Allocate( count );
}

void operator delete( void * data ) {
	MemoryManager::Instance().Free( data );
}

MemoryManager::MemoryNode::MemoryNode() {
	data = nullptr;
	size = 0;
	next = nullptr;
	prev = nullptr;
}
		
MemoryManager::MemoryManager() {
	msRoot = nullptr;
	msLast = nullptr;
	mTotalBytesAllocated = 0;
}

MemoryManager::~MemoryManager() {	
	cout << "Exiting application... Memory still allocated: " << mTotalBytesAllocated << " bytes! Cleanup..." << endl;
	while( mTotalBytesAllocated ) {
		Free( msRoot->data );
	}
	cout << "Done!" << endl;
}

MemoryManager & MemoryManager::Instance() {
	static MemoryManager instance;
	return instance;
}

void * MemoryManager::Allocate( int count ){
	void * data = malloc( count );
	MemoryNode * memNode = (MemoryNode*)malloc(sizeof(MemoryNode));	
	memNode->data = data;
	memNode->next = nullptr;	
	memNode->size = count;
	memNode->prev = msLast;
	mTotalBytesAllocated += count;
	if( msLast ) {
		msLast->next = memNode;	
	}
	if( !msRoot ) {
		msRoot = memNode;
	}
	msLast = memNode;	
	return data;
}

void MemoryManager::Free( void * data ) {
	MemoryNode * current = msRoot;
	while( current ) {
		if( current->data == data ) {
			if( current->next ) {
				current->next->prev = current->prev;					
			}
			if( current->prev ) {
				current->prev->next = current->next;
			}
			if( current == msRoot ) {
				if( msRoot->next ) {
					msRoot = msRoot->next;				
				} else {
					msRoot = nullptr;
				}
			}
			if( current == msLast ) {
				if( msLast->prev ) {
					msLast = msLast->prev;
				} else {
					msLast = nullptr;				
				}
			}			
			mTotalBytesAllocated -= current->size;
			free( data );
			free( current );			
			return;
		}
		current = current->next;
	}
}

