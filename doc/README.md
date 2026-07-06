## HC_MALLOC


### General

hc_malloc is a replacement method to the classic C malloc, calloc and realloc functions.

The hc prefix alludes to the internally implemented Heap Control functionality.

---

### Install

* From the command line (terminal):
    * ``` curl -SL https://yveshoebeke.github.io/install/deploy_hc_malloc.sh | sh ```
* The installation process will start.
* Your system password will be required to install the required files.
* Pre-requisite to run this install is that your system needs to have ```make```, ```cmake``` and ```gcc```/```clang``` tools installed.

---

### File locations

* Documentation and License:

```
/usr/local/share/hc_malloc
                    └── doc
                        ├── README.md
                        ├── html
                        │   └── hc_malloc.html
                        ├── license
                        │   └── LICENSE
                        └── pdf
                            └── hc_malloc.pdf
```

* Library:
    * ``` /usr/local/share/lib/libhc_malloc.a ```

* Man page:
    * ``` /usr/local/man/man3/hc_malloc.3 ```
    * For more information regarding this library: ```man hc_malloc```

---

### Integration

* Add ```#include <hc_malloc/hc_malloc.h>``` to your source.
* At the beginning of your code add the line: ```hc_init();``` to set up the heap address arena.
* When compiling include the link to the library. Example: 
```gcc -o my_program -lhc_malloc my_program.c```.

---

### Functions

* ```hc_init()``` - initialize array, capacity and next_idx in struct.
* ```hc_cleanup()``` - free all addresses in the array.
* ```hc_free(addr_pntr)``` - free and remove a pointer from array.
* ```hc_display()``` - debug/visibility routine.
* ```hc_malloc(size)```	- generate heap memory area, store and return pointer.
* ```hc_calloc(num_elem, elem_size)``` - generate initialized heap memory area.
* ```hc_posix_memalign(addr_pntr, size)``` - allocates at a base address that is exactly a multiple of the system's page size alignment.
* ```hc_realloc(old_pntr, size)```- realloc with different size and retain orig. value.
* ```hc_reallocf(old_pntr, size)```- drop-in replacement for reallocf.
* ```hc_strdup(addr_pntr)``` - same as above, but moves content to heap pointed area.

---

### License

This code is released under a MIT License

---

