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

/* project version */
#define HC_MALLOC_VERSION_MAJOR 1
#define HC_MALLOC_VERSION_MINOR 0
#define HC_MALLOC_VERSION_PATCH 0

/* Arrray size and growth */
#define HEAP_ADDR_ARRAY_SIZE 32

/* 	
 The HC_MALLOC structure (Heap_Arena named): 
	next_idx: next available index to store an address pointer.
	capacity: current capacity of the array
	addr: array containing all the pointers or NULL if unused.
*/
typedef struct {
	int next_idx;
	int capacity;
	void **addr;
} Heap_Arena;

/* Public Functions: */

/* Initializes the Heap structure and sets array capacity
   to the HEAP_ADDR_ARRAY_SIZE constant. */
int hc_init();
/* Frees all pointers stored in the Heap array and
   the Heap structure itself. */
void hc_cleanup();
/* (Public) Frees address and removes it (NULLified) from the array */
void hc_free(void*);
/* Utility to display content of the Heap array */
void hc_display();
/* Allocates requested memory block, saves
  the address pointer into the Heap array and
  updates the next_idx value. */
void *hc_malloc(size_t);
/* Allocates requested memory block, initialized to NULL
   and saves the address pointer into the Heap array and
  updates the next_idx value. */
void *hc_calloc(size_t, size_t);
/* Reallocates previously defined memory to a 
   a newly defined sized memory area and updates
   the previously assigned pointer with
   the new address pointer. */
void *hc_realloc(void*, size_t);
/* Copies the content of given content to 
   a Heap location, adds the address to the array
   and updates the next_idx value. */
char *hc_strdup(char*);

#ifdef __cplusplus
}
#endif

#endif	// HC_MALLOC_H
