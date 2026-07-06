#include <stdio.h>
#include <string.h>
#include <time.h>

#include <hc_malloc/hc_malloc.h>

#define RED_TEXT    "\033[31m"
#define BOLD_RED    "\033[1;31m"
#define GREEN_TEXT  "\033[32m"
#define YELLOW_TEXT  "\033[33m"
#define BOLD_BLUE   "\033[1;34m"
#define RESET_COLOR "\033[0m"


#define LOG_ERROR(msg) fprintf(stderr, "[%s] - ERROR: %s\n", __func__, msg)

void continue_after_enter() {
	printf("\n" GREEN_TEXT "Press [Enter] to continue... " RESET_COLOR);
	getchar();
	printf("\n");
}

int main() {
	if ( hc_init() ) { return 1; }
	printf(BOLD_BLUE "hc dump after init (0):" RESET_COLOR "\n\n");

	hc_display();
	continue_after_enter();

	char *a_string_1 = "Now the time hase come for all good men to come to the aid of their country.";

	char *a_string_2 = hc_strdup(a_string_1);
	if ( a_string_2 == NULL ) { LOG_ERROR("a_string_2 hc_strdup failed"); }

	char *a_string_3 = hc_malloc(80);
	if ( a_string_3 == NULL ) { LOG_ERROR("a_string_3 hc_malloc failed"); }

	strcpy(a_string_3, "From high to low, watch out below!");


	printf(BOLD_BLUE "hc dump after allocations (1):\n" RED_TEXT 
		"\ta_string_1: " GREEN_TEXT "%s" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] (stack, not in address array)\n"
		"\ta_string_2: " GREEN_TEXT "%s" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] (heap, hc_strdup() content from a_string_1)\n"
		"\ta_string_3: " GREEN_TEXT "%s" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] (heap, hc_malloc() and content from a strcpy)\n\n" RESET_COLOR,
		a_string_1, a_string_1, a_string_2, a_string_2, a_string_3, a_string_3);

	hc_display();
	continue_after_enter();

	char *a_string_4 = hc_realloc(a_string_3, 128);
	if ( a_string_4 == NULL ) { LOG_ERROR("a_string_4 hc_realloc failed"); }

	strcpy(a_string_4, "Wind from to left: worstening weather");

	printf(BOLD_BLUE "hc dump after allocations (2):\n" RED_TEXT
		"\ta_string_4: " GREEN_TEXT "%s" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] (heap, from hc_realloc of q_string_3)\n\n" RESET_COLOR,
		a_string_4, a_string_4);

	hc_display();
	continue_after_enter();

	char *a_string_5 = hc_calloc(80, sizeof(char));

	printf(BOLD_BLUE "hc dump after allocations (3):\n" RED_TEXT
		"\ta_string_5: " GREEN_TEXT "%s" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] (heap, empty string from hc_calloc)\n\n" RESET_COLOR,
		a_string_5, a_string_5);

	hc_display();
	continue_after_enter();

	printf(BOLD_BLUE "hc dump after allocations (4):\n" GREEN_TEXT "\tAdding 40 random integer values" RESET_COLOR " (hc_malloc; " BOLD_RED "will cause array expansion" RESET_COLOR "):\n");

	srand(time(NULL));
	int *numbers[40];
	for(int i = 0; i < 40; i++) {
		numbers[i] = hc_malloc(sizeof(int));
		if ( numbers[i] == NULL ) { LOG_ERROR("array number allocation failed"); }
		*numbers[i] = (rand() % 900) + 100;

		printf("\t" RED_TEXT "%02d: " GREEN_TEXT "%d" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] " RESET_COLOR, i, *numbers[i], numbers[i]);
		if ( (i + 1) % 4 == 0 ) { printf("\n"); }
	}
	printf("\n");

	hc_display();
	continue_after_enter();

	int i = 12, j = 33;
	printf(BOLD_BLUE "hc dump after allocations (5):\n" RED_TEXT
		"\tRemoving a_string_2: " GREEN_TEXT "%s" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "] (heap, hc_strdup() content from a_string_1)\n"
		"\tRemoving numbers[" YELLOW_TEXT "%d" RED_TEXT "]: " GREEN_TEXT "%d" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "]\n"
		"\tRemoving numbers[" YELLOW_TEXT "%d" RED_TEXT "]: " GREEN_TEXT "%d" RED_TEXT " -> [" YELLOW_TEXT "%p" RED_TEXT "]\n\n" RESET_COLOR,
		a_string_2, a_string_2, i, *numbers[i], numbers[i], j, *numbers[j], numbers[j]);

	hc_free(a_string_2);
	hc_free(numbers[i]);
	hc_free(numbers[j]);

	hc_display();
	continue_after_enter();

	void *a_mem_area = NULL;
	if ( hc_posix_memalign((void **)&a_mem_area, 32,  (100 * sizeof(double))) ) { LOG_ERROR("hc_posic_memalign allocation failed"); }
	printf(BOLD_BLUE "hc dump after allocations (6):\n" RED_TEXT
		"\tAllocated boundry aligned memory for 100 double-s:  -> [" YELLOW_TEXT "%p" RED_TEXT "]\n" RESET_COLOR
		"\tCheck if aligned with pagesize boundry:\n"
		"\t",
		a_mem_area);

	if (((uintptr_t)a_mem_area % 64) == 0) {
		printf("< (uintptr)a_mem_area precent 64 is 0 > => Success! Address is perfectly aligned to %d bytes.\n", 64);
	} else {
		printf("< (uintptr)a_mem_area percent 64 is not 0 > => Alignment failure for page size %d bytes.\n", 64);
	}

	hc_display();
	continue_after_enter();

	printf(BOLD_BLUE "hc dump after allocations (7):\n" RED_TEXT
		"\tReset and freed everything.\n\n" RESET_COLOR);

	hc_reset();

	hc_display();
	continue_after_enter();

	printf(BOLD_BLUE "hc test exit with hc_cleanup().\n\n\n" RESET_COLOR);
	hc_cleanup();

	return 0;
}

