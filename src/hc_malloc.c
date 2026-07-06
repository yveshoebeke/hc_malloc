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

  ----

  HEAP MANAGEMENT

  This is a utility that will keep track of all pointers to allocated memory,
  created during your process.

  The functions wrap around the standard memory allocation functions and in 
  some cases improve upn them. Their names are a direct reference to these 
  standard utilities. i.e. hc_malloc wraps malloc.

  A cleanup routine will know which addresses need to to be de-allocated
  prior to program termination, assisting you to free up all pointers
  allocated during your process.

  See hc_malloc.h for the heap arena structure.

  hc_init			- initialize array, capacity and next_idx in struct.
  hc_cleanup			- free all addresses in the array and remove all..
  hc_reset			- free all addresses in the array, and re-initializes a new arena address array.
  hc_clear			- free all addresses in the array, but leaves struct intact.
  hc_free			- free and remove a pointer from array.
  hc_display			- debug/visibility routine.
  hc_malloc			- generate heap memory area, store and return pointer.
  hc_posix_memalign		- generate page boundry aligned heap memory area, store and return pointer.
  hc_realloc			- realloc with different size and retain orig. value.
  hc_reallocf			- reallocf, frees original pointer, just like hc_realloc. Drop-in replacement.
  hc_strdup			- same as above, but moves content to heap pointed area.

  Internal:
  _hc_address_array_expand	- increase array capacity (in heap block chunks).
  _hc_address_add		- add address to array & update next_idx.
  _hc_address_remove		- remove address from array.
  _hc_address_replace		- replace address in array to a new value.
  _hc_address_locate		- locates address in array and returns the index.

*/

#include <hc_malloc/hc_malloc.h>

Heap_Arena *arena = NULL;

/* (Private) Reallocate arena address array space with higher 
   capacity. (+ HEAP_ADDR_ARRAY_SIZE value chunk at a time) */
int _hc_address_array_expand() {
	// 1. calculate new size.
	int new_capacity = arena->capacity + HEAP_ADDR_ARRAY_SIZE;
	size_t new_size_bytes = new_capacity * sizeof(void *);

	// 2. create new addr array with new capacity.
	void **new_addr = NULL;
	int result = posix_memalign((void **)&new_addr, arena->alignment, new_size_bytes);
	if ( result != 0 ) {
		fprintf(stderr, "fatal error: addr array capacity increase failed");
		return result;	
	}

	// 3. copy tracked addr to new addr location.
	size_t old_size_bytes = arena->capacity * sizeof(void *);
	memcpy(new_addr, arena->addr, old_size_bytes);

	// 4. initialize newly added addr elements.
	for(int i = arena->capacity; i < new_capacity; i++) {
		new_addr[i] = NULL;
	}

	// 5. free old array allocation.
	free(arena->addr);

	// 6. assign new addr to struct addr, update capacity value.
	arena->addr = new_addr;
	arena->capacity = new_capacity;

	return 0;
}

/* (Private) Add address that was allocated on the arena array. */
int _hc_address_add(void *addr) {
	arena->addr[arena->next_idx++] = addr;
	if (arena->next_idx == arena->capacity - 1) {
		int rc = _hc_address_array_expand();
		if( rc != 0 ) {
			return 1;
		}
	}

	return 0;
}

/* (Private) Find address in the arena array and return its index */
static int _hc_address_locate( void *addr_ptr ) {
	int i = 0;
	int idx = -1;
	while( i < arena->next_idx ) {
		if ( arena->addr[i] == addr_ptr ) {
			idx = i;
			break;
		}

		i++;
	}

	return idx;
}
	
/* (Private) Replace address in arena array with a new address */
int _hc_address_replace(void *old_ptr, void *new_ptr) {
	int idx;
	if ( (idx = _hc_address_locate( old_ptr )) != -1 ) {
		arena->addr[idx] = new_ptr;
		return 0;
	}

	return 1;
}

/* (Private) Remove address from Heap Arena array 
   by setting it to NULL and moving the stack down by one. */
static void _hc_address_remove(void *addr_ptr) {
	int i = _hc_address_locate( addr_ptr );
	if ( i == -1 ) return; 
	for ( ; i < arena->next_idx; i++ ) {
		arena->addr[i] = arena->addr[i + 1];
	}

	arena->next_idx -= 1;
}

/* (Public) (Utility) Display content of the Heap array. 
   (intended for some soft debugging, it needed) */
void hc_display() {
	printf("Heap Arena address -> [%p]\n", (void *)arena);
	printf("capacity: %d\n", arena->capacity);
	printf("next_idx: %d\n", arena->next_idx);
	printf("pagesize: %zu bytes\n", arena->pagesize);
	printf("alignment: %zu bytes\n", arena->alignment);
	printf("addresses:\n");
	for (int i = 0; i < arena->capacity; i++) {
		printf("[%02d] -> (%p)  ", i, arena->addr[i]);
		if ( (i + 1) % 4  == 0) { printf("\n"); }
	}
}

/* (Public) Initializes the Heap structure, sets the arena array capacity
   to the HEAP_ADDR_ARRAY_SIZE constant, initializes the control values and
   allocates the arena address array.*/

int hc_init() {
	errno = 0;
	// allocate the structure.
	arena = malloc(sizeof(Heap_Arena));
	if (arena == NULL) {
		fprintf(stderr, "[%s] - fatal error: heap allocation failed", __func__);
		return 1;
	}

	// set control values
	arena->next_idx = 0;
	arena->capacity = HEAP_ADDR_ARRAY_SIZE;
	arena->alignment = HEAP_ADDR_ARRAY_SIZE * 2;
	arena->pagesize = sysconf(_SC_PAGESIZE);
	if ( arena->pagesize == -1 ) {
		if ( errno == 0 ) {
			arena->pagesize = DEFAULT_MEMORY_PAGESIZE;
		} else {
			perror("sysconf error when requesting pagesize");
			fprintf(stderr, "[%s] - pagesize request error: [%d]: %s", __func__, errno, strerror(errno));
			free(arena);
			return 1;
		}
	}

	/* The arena address area is allocated using the posix_memalign functionality to align 
	   to the boundary of a page, to Cache Lines (64 bytes). By using an alignment value of 
	   (HEAP_ADDR_ARRAY_SIZE (32) * 2)  64, the math aligns  with the 32-element chunks, 
	   asking for 256 bytes of data. Because 256 is an exact multiple of 64, the system 
	   allocator lays the arena array perfectly across exactly 4 CPU cache lines. There
	   is 0% memory waste and optimizes access speed. */
	arena->addr = NULL;
	int result = posix_memalign((void **)&arena->addr, arena->alignment, arena->capacity * sizeof(void*));
	if (result != 0) {
		fprintf(stderr, "[%s] - fatal error: heap address array allocation failed", __func__);
		free(arena->addr);
		return 1;
	}
	
	return 0;
}

/* (Public) Frees all pointers stored in the arena array and
   frees the Heap arena structure itself. */
void hc_cleanup() {
	for (int i = 0; i < arena->next_idx; i++) {
		free(arena->addr[i]);
	}
	free(arena->addr);
	free(arena);
}

/* (Public) Frees all pointers stored in the Heap array, 
   reinitializes the address array to initial capacity
   and resets index/capacity values.
   See hc_init for associated commentary. */
int hc_reset() {
	/* 1. free addresses present in the arena address array.
	      Warning! If the pointers are still activily used
              in your process they will become zombies. */
	for (int i = 0; i < arena->next_idx; i++) {
		free(arena->addr[i]);
	}

	// 2. release arena address array allocation.
	free(arena->addr);
	// 3. re-allocate the arena address array with initial capacity.
	arena->addr = NULL;
	int result = posix_memalign((void **)&arena->addr, arena->alignment, arena->capacity * sizeof(void*));
	if (result != 0) {
		fprintf(stderr, "[%s] - fatal error: heap address array allocation failed", __func__);
		free(arena->addr);
		return 1;
	}
	// 4. reset control values.
	arena->next_idx = 0;
	arena->capacity = HEAP_ADDR_ARRAY_SIZE;
	
	// 5. assure the arena address array is clean (all: NULL values).
	for ( int i = 0; i < arena->capacity; i++ ) {
		arena->addr[i] = NULL;
	}

	return 0;
}

/* (Public) Frees address and removes it from the arena 
   address array. */
void hc_free(void *ptr) {
	_hc_address_remove(ptr);
	free(ptr);
}

/* (Public) Allocates requested memory block, saves
   the address pointer into the arena address array
    and updates the next_idx value. */
void *hc_malloc(size_t size) {
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL) return NULL;

	if ( _hc_address_add(ptr) != 0 ) { return NULL; }
	return ptr;
} 

/* (Public) Allocates requested memory block, 
   initializes its values, saves the address pointer into 
   the arena address array and updates the next_idx value. */
void *hc_calloc(size_t number_of_elements, size_t size_of_element) {
	void *ptr;
	ptr = calloc(number_of_elements, size_of_element);
	if (ptr == NULL) return NULL;

	if ( _hc_address_add(ptr) != 0 ) { return NULL; }
	return ptr;
} 

/* (Public) Will allocated the requesed size memory area on
   the page boundry. This will optimize memory address assignment
   and access speed.
   Every modern Linux, macOS, and BSD system fully supports posix_memalign(). 
   Use hc_posix_memalign() if you are writing standard software for Linux, macOS, or UNIX.*/
int hc_posix_memalign(void **ptr, size_t alignment,  size_t size) {
	int rc = posix_memalign(ptr, arena->alignment, size);
	if ( rc != 0 ) {
		fprintf(stderr, "[%s] - Memory allocation on aligned boundry failed", __func__);
		*ptr = NULL;
		return 1;
	}

	if ( _hc_address_add(*ptr) != 0 ) { return 1; }
	return 0;
}

/* (Public) Reallocates previously defined memory to a newly defined
   sized memory area previously assigned pointer with the new address pointer. 
   Updates the arena address array and frees the old pointer (see hc_reallocf). */
void *hc_realloc(void *orig_ptr, size_t new_size) {
	void *temp = realloc(orig_ptr, new_size * sizeof(void*));
	if (temp == NULL) {
		_hc_address_remove(orig_ptr);
		free(orig_ptr);
		return NULL;
	}
	
	if ( _hc_address_replace(orig_ptr, temp) ) { return NULL; }
	orig_ptr = temp;
	
	return orig_ptr;
}

/* (Public) Because reallocf() automatically frees the old memory on failure
   it eliminates the memory leak vulnerability of standard realloc(). 
   For drop-in compatibility, sin hc_realloc also frees this pointer. */
void *hc_reallocf(void *orig_ptr, size_t new_size) {
	void *temp = reallocf(orig_ptr, new_size);
	if (temp == NULL) {
		_hc_address_remove(orig_ptr);
		return NULL;
	}

	_hc_address_replace(orig_ptr, temp);
	orig_ptr = temp;

	return orig_ptr;
}

/* (Public) Copies the content of given string content to 
   a Heap location, adds the pointer to the arena address
   array and updates the next_idx value. */
char *hc_strdup(char *str) {
	size_t length = strlen(str) + 1;
	char *ptr = hc_malloc( length * sizeof(char) );
	if ( ptr == NULL ) {
		return NULL;
	}

	strncpy(ptr, str, length);

	return ptr;
}

