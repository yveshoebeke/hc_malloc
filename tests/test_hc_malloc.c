#include <stdio.h>
#include <string.h>
#include <hc_malloc/hc_malloc.h>

#define LOG_ERROR(msg) fprintf(stderr, "[%s] - ERROR: %s\n", __func__, msg)

void continue_after_enter() {
	printf("\nPress [Enter] to continue... ");
	getchar();
	printf("\n");
}

int main() {
	if ( hc_init() ) { return 1; }
	printf("hc dump after init (0):\n");
	hc_display();

	continue_after_enter();

	char *a_string_1 = "Now the time hase come.";
	char *a_string_3 = hc_malloc(80);
	if ( a_string_3 == NULL ) { LOG_ERROR("a_string_3 hc_malloc failed"); }

	strcpy(a_string_3, "Yves Hoebeke");

	char *a_string_2 = hc_strdup(a_string_1);
	if ( a_string_2 == NULL ) { LOG_ERROR("a_string_2 hc_strdup failed"); }

	printf("a_string_1: %s\t[%p](stack)\na_string_2: %s\t[%p](strdup of a_string_1)\na_string_3: %s\t[%p](regular malloc)\n", a_string_1, a_string_1, a_string_2, a_string_2, a_string_3, a_string_3);

	printf("hc dump after allocations (1):\n");
	hc_display();

	continue_after_enter();

	char *a_string_4 = hc_realloc(a_string_3, 128);
	if ( a_string_4 == NULL ) { LOG_ERROR("a_string_4 hc_realloc failed"); }

	strcpy(a_string_4, "Yves Hoebeke - Vijverslei 24B - 2950 Kapellen - Belgium");
	printf("a_string_4: %s\t[%p] (realloc of a_string_3)\n", a_string_4, a_string_4);
	
	printf("hc dump after allocations (2):\n");
	hc_display();

	continue_after_enter();

	char *a_string_5 = hc_calloc(80, sizeof(char));
	printf("calloc of empty string)\n");
	printf("hc dump after allocations (3):\n");
	hc_display();

	continue_after_enter();

	printf("Adding 40 integers (will cause array expansion):\n");
	int *numbers[40];
	for(int i = 0; i < 40; i++) {
		numbers[i] = hc_malloc(sizeof(int));
		if ( numbers[i] == NULL ) { LOG_ERROR("array number allocation failed"); }

		*numbers[i] = i * 3;
	}

	const int j = 5;
	printf("[%p] -> numbers[%d] = %d\n", numbers[j], j, *numbers[j]);
	printf("hc dump after allocations (4):\n");
	hc_display();

	continue_after_enter();

	printf("Removing a_string_2: %s\t[%p](free)\n", a_string_2, a_string_2);
	hc_free(a_string_2);
	printf("Removing numbers[%d]: %d\t[(%p)](free)\n", j, *numbers[j], numbers[j]);
	hc_free(numbers[j]);
	printf("hc dump after allocations (5):\n");
	hc_display();

	hc_cleanup();

	return 0;
}

