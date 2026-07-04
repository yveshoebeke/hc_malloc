/*
	HEAP MANAGEMENT

	These routines will be synonimous with std library functions, but
	will keep track of all address pointers pointing to heap memory
	locations.

	A cleanup routine will know which addresses need to to be de-allocated
	prior to program termination.

	hc_init					- initialize array, capacity and next_idx in struct.
	hc_cleanup				- free all addresses in the array.
	hc_free					- free and remove a pointer from array.
	hc_display				- debug/visibility routine.
	hc_malloc				- generate heap memory area, store and return pointer.
	hc_realloc				- realloc with different size and retain orig. value.
	hc_strdup				- same as above, but moves content to heap pointed area.

	Internal:
	_hc_address_array_capacity_increase 	- increase array capacity (in heap block chunks).
	_hc_address_add				- add address to array & update next_idx.
	_hc_address_remove			- remove address from array.
	_hc_address_replace			- replace address in array to a new value.
	_hc_address_locate			- locates address in array and returns the index.
*/

#include <hc_malloc/hc_malloc.h>

Heap_Arena *hc = NULL;

/* (Private) Reallocate address space w/ higher capacity.
   (+ HEAP_ADDR_ARRAY_SIZE value chunk at a time) */
static void _hc_address_array_capacity_increase() {
	int new_capacity = hc->capacity + HEAP_ADDR_ARRAY_SIZE;
	void *temp = realloc(hc->addr, new_capacity * sizeof(void*));
	if (temp == NULL) {
		fprintf(stderr, "fatal error: addr array capacity increase failed");
		free(hc->addr);
		free(hc);
		exit(EXIT_FAILURE);
	}
	
	hc->addr = temp;
	hc->capacity = new_capacity;
}

/* (Private) Add address that was allocated on the heap array. */
static void _hc_address_add(void *addr) {
	hc->addr[hc->next_idx++] = addr;
	if (hc->next_idx == hc->capacity - 1) {
		_hc_address_array_capacity_increase();
	}
}

/* (Private) Find address in the array and return its index */
static int _hc_address_locate( void *addr_ptr ) {
	int i = 0;
	int idx = -1;
	while( i < hc->next_idx ) {
		if ( hc->addr[i] == addr_ptr ) {
			idx = i;
			break;
		}

		i++;
	}

	return idx;
}
	
/* (Private) Replace address in Heap Arena array with a new address */
static void _hc_address_replace(void *old_ptr, void *new_ptr) {
	int idx;
	if ( (idx = _hc_address_locate( old_ptr )) != -1 ) {
		hc->addr[idx] = new_ptr;
	}
}

/* (Private) Remove address from Heap Arena array 
   by setting it to NULL and moving the stack down by one. */
static void _hc_address_remove(void *addr_ptr) {
	int i = _hc_address_locate( addr_ptr );
	if ( i == -1 ) return; 
	for ( ; i < hc->next_idx; i++ ) {
		hc->addr[i] = hc->addr[i + 1];
	}

	hc->next_idx -= 1;
}

/* (Public) (Utility) Display content of the Heap array. */
void hc_display() {
	printf("Heap Arena address -> [%p]\n", (void *)hc);
	printf("capacity: %d - next_idx: %d - addresses:\n", hc->capacity, hc->next_idx);
	for (int i = 0; i < hc->capacity; i++) {
		printf("%02d -> [%p]\n", i, hc->addr[i]);
	}
}

/* (Public) Initializes the Heap structure and sets array capacity
   to the HEAP_ADDR_ARRAY_SIZE constant. */
int hc_init() {
	errno = 0;

	hc = malloc(sizeof(Heap_Arena));
	if (hc == NULL) {
		fprintf(stderr, "[%s] - fatal error: heap allocation failed", __func__);
		return 1;
	}

	hc->addr = malloc(HEAP_ADDR_ARRAY_SIZE * sizeof(void*));
	if (hc->addr == NULL) {
		fprintf(stderr, "[%s] - fatal error: heap address array allocation failed", __func__);
		free(hc);
		return 1;
	}

	hc->next_idx = 0;
	hc->capacity = HEAP_ADDR_ARRAY_SIZE;
	hc->pagesize = sysconf(_SC_PAGESIZE);
	if ( hc->pagesize == -1 ) {
		if ( errno == 0 ) {
			hc->pagesize = DEFAULT_MEMORY_PAGESIZE;
		} else {
			perror("sysconf error when requesting pagesize");
			fprintf(stderr, "[%s] - pagesize request error: [%d]: %s", __func__, errno, strerror(errno));
			free(hc);
			return 1;
		}
	}

	return 0;
}

/* (Public) Frees all pointers stored in the Heap array and
   the Heap structure itself. */
void hc_cleanup() {
	for (int i = 0; i < hc->next_idx; i++) {
		free(hc->addr[i]);
	}
	free(hc->addr);
	free(hc);
}

/* (Public) Frees address and removes it (NULLified) from the array */
void hc_free(void *ptr) {
	_hc_address_remove(ptr);
	free(ptr);
}

/* (Public) Allocates requested memory block, saves
   the address pointer into the Heap arrau and
   updates the next_idx value. */
void *hc_malloc(size_t size) {
	void *ptr;
	ptr = malloc(size);
	if (ptr == NULL) return NULL;

	_hc_address_add(ptr);
	return ptr;
} 

/* (Public) Allocates requested memory block, 
   initializes its values and saves the address pointer
   into the Heap array and updates the next_idx value. */
void *hc_calloc(size_t number_of_elements, size_t size_of_element) {
	void *ptr;
	ptr = calloc(number_of_elements, size_of_element);
	if (ptr == NULL) return NULL;

	_hc_address_add(ptr);
	return ptr;
} 

/* 
Recommendation (memory allignment functions):

Use hc_posix_memalign() if you are writing standard software for Linux, macOS, or UNIX.

Use hc_aligned_alloc() only if your primary requirement is cross-platform C11 compliance outside of UNIX environments, and you don't mind the forced size rounding. 

*/

/* (Public) Every modern Linux, macOS, and BSD system fully supports posix_memalign(). */
int hc_posix_memalign(void *ptr, size_t size) {
	ptr = NULL;
	int rc = posix_memalign(ptr, hc->pagesize, size);
	if ( rc != 0 ) {
		fprintf(stderr, "[%s] - Memory allocation on aligned boundry failed", __func__);
		return 1;
	}

	_hc_address_add(ptr);
	return 0;
}

/* (Public) Cross platform C11 compliant outside UNIX environments. */
void *hc_aligned_alloc(size_t size) {
	void *ptr = aligned_alloc(hc->pagesize * 2, size);
	if ( ptr == NULL ) {
		return NULL;
	}

	_hc_address_add(ptr);
	return ptr;
}

/* (Public)  Drop-in replacement for: void *ptr = valloc(size); */
void* hc_valloc(size_t size) {
	void *ptr = NULL;

	/* hc_posix_memalign returns 0 on success, or an error code
	   address saved by hc_posix_memalign function */
	if (hc_posix_memalign(&ptr, size) != 0) {
	    return NULL; // Mimic valloc return behavior on failure
	}
	
	return ptr;
}

/* (Public) Drop-in replacement for: void *ptr = valloc(size);
   C11 compliant, forces size allignment. Also consider hc_aligned_alloc. */
void* hc_C11_valloc(size_t size) {
	// Size MUST be a multiple of alignment for aligned_alloc
	size_t remainder = size % hc->pagesize;
	size_t aligned_size = (remainder == 0) ? size : (size + (hc->pagesize - remainder));
	// address saved by hc_aligned_alloc()
	return hc_aligned_alloc(aligned_size);
}

/* (Public) Reallocates previously defined memory to a 
   a newly defined sized memory area and updates
   the previously assigned pointer with
   the new address pointer. */
void *hc_realloc(void *orig_ptr, size_t new_size) {
	void *temp = realloc(orig_ptr, new_size * sizeof(void*));
	if (temp == NULL) {
		_hc_address_remove(orig_ptr);
		free(orig_ptr);
		return NULL;
	}
	
	_hc_address_replace(orig_ptr, temp);
	orig_ptr = temp;
	
	return orig_ptr;
}

/* (Public) Because reallocf() automatically frees the old memory on failure.
   It eliminates the memory leak vulnerability of standard realloc(). 
   This means you can technically use the concise syntax that is forbidden with regular realloc.
   Originally introduced by FreeBSD and widely used in macOS/Darwin systems. */
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

/* (Public) Copies the content of given content to 
   a Heap location, adds the address to the array
   and updates the next_idx value. */
char *hc_strdup(char *str) {
	size_t l = strlen(str) + 1;
	char *ptr = hc_malloc( l );
	if ( ptr == NULL ) {
		return NULL;
	}

	strncpy(ptr, str, l);

	return ptr;
}

