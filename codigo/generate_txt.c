/*
 * Gerador de arquivos .txt para os experimentos do seminario.
 * Produz datasets com numeros bem espalhados, embaralhados por Fisher-Yates,
 * para os tamanhos 10000, 100000 e 1000000 por padrao.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned long long rng_state = 88172645463393265ULL;

static unsigned int next_u32(void) {
	rng_state ^= rng_state << 7;
	rng_state ^= rng_state >> 9;
	return (unsigned int)(rng_state & 0xFFFFFFFFu);
}

static void shuffle_ints(int *arr, size_t n) {
	size_t i;

	for (i = n; i > 1; i--) {
		size_t j = (size_t)(next_u32() % i);
		int temp = arr[i - 1];
		arr[i - 1] = arr[j];
		arr[j] = temp;
	}
}

static int generate_dataset(const char *filename, size_t n) {
	FILE *file;
	int *values;
	size_t i;

	values = (int *)malloc(n * sizeof(int));
	if (values == NULL) {
		return 0;
	}

	/*
	 * Valores pseudoaleatorios no intervalo [0, 1000000].
	 */
	rng_state ^= (unsigned long long)n * 0x9E3779B97F4A7C15ULL;
	for (i = 0; i < n; i++) {
		values[i] = (int)(next_u32() % 1000001U);
	}

	shuffle_ints(values, n);

	file = fopen(filename, "w");
	if (file == NULL) {
		free(values);
		return 0;
	}

	fprintf(file, "%zu\n", n);
	for (i = 0; i < n; i++) {
		fprintf(file, "%d\n", values[i]);
	}

	fclose(file);
	free(values);
	return 1;
}

int main(void) {
	const size_t sizes[] = {10000, 100000, 1000000};
	const size_t count = sizeof(sizes) / sizeof(sizes[0]);
	size_t i;

	printf("Gerando arquivos com numeros bem espalhados...\n");
	for (i = 0; i < count; i++) {
		char filename[128];
		snprintf(filename, sizeof(filename), "entrada_%zu.txt", sizes[i]);
		if (!generate_dataset(filename, sizes[i])) {
			printf("Falha ao gerar %s\n", filename);
			return 1;
		}
		printf("Gerado: %s\n", filename);
	}

	printf("Concluido.\n");
	return 0;
}