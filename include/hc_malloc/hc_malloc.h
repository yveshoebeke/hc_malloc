/*

  Copyright (c) 2026 Yves Hoebeke and hc_malloc contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

*/

#ifndef HC_MALLOC_H
#define HC_MALLOC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Project Version
#define HC_MALLOC_VERSION_MAJOR 1
#define HC_MALLOC_VERSION_MINOR 0
#define HC_MALLOC_VERSION_PATCH 0

// Array size and expansion factor.
#define HEAP_ADDR_ARRAY_SIZE 32

// Default pagesize if system pagesize is not set. (unlimitted?)
#define DEFAULT_MEMORY_PAGESIZE 4096

/* 	

  The HC_MALLOC structure (Heap_Arena named): 

  next_idx: next available index to store an address pointer.
  capacity: current capacity of the array.
  pagesize: current system's mempry page size.
  alignemnt: page boundry size for arena address array allocation.
  addr: arena array containing all the pointers or NULL if unused.

*/
typedef struct {
	int next_idx;
	int capacity;
	size_t pagesize;
	size_t alignment;
	void **addr;
} Heap_Arena;

/* Public Functions: */
int hc_init();
void hc_cleanup();
int hc_reset();
void hc_free(void*);
void hc_display();
void *hc_malloc(size_t);
void *hc_calloc(size_t, size_t);
int hc_posix_memalign(void**, size_t, size_t);
void *hc_realloc(void*, size_t);
void *hc_reallocf(void*, size_t);
char *hc_strdup(char*);

#ifdef __cplusplus
}
#endif

#endif	// HC_MALLOC_H
