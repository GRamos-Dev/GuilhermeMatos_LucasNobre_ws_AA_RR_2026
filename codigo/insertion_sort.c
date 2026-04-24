/*
 * Insertion Sort para leitura de arquivo.
 * O arquivo de entrada deve conter a quantidade n na primeira linha
 * e depois n numeros inteiros separados por espacos ou quebras de linha.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static int *load_dataset(const char *filename, size_t *n) {
	FILE *file = fopen(filename, "r");
	int *values = NULL;
	size_t i;

	if (file == NULL) {
		return NULL;
	}

	if (fscanf(file, "%zu", n) != 1 || *n == 0) {
		fclose(file);
		return NULL;
	}

	values = (int *)malloc(*n * sizeof(int));
	if (values == NULL) {
		fclose(file);
		return NULL;
	}

	for (i = 0; i < *n; i++) {
		if (fscanf(file, "%d", &values[i]) != 1) {
			free(values);
			fclose(file);
			return NULL;
		}
	}

	fclose(file);
	return values;
}

static void insertion_sort(int *arr, size_t n) {
	size_t i;

	for (i = 1; i < n; i++) {
		int key = arr[i];
		size_t j = i;

		while (j > 0 && arr[j - 1] > key) {
			arr[j] = arr[j - 1];
			j--;
		}
		arr[j] = key;
	}
}

static int is_sorted(const int *arr, size_t n) {
	size_t i;

	for (i = 1; i < n; i++) {
		if (arr[i - 1] > arr[i]) {
			return 0;
		}
	}
	return 1;
}

static void print_sample(const int *arr, size_t n) {
	size_t i;
	size_t limit = n < 10 ? n : 10;

	printf("[");
	for (i = 0; i < limit; i++) {
		printf("%d", arr[i]);
		if (i + 1 < limit) {
			printf(", ");
		}
	}
	if (n > limit) {
		printf(", ...");
	}
	printf("]");
}

static const char *base_name(const char *path) {
	const char *slash = strrchr(path, '/');
	const char *backslash = strrchr(path, '\\');

	if (slash == NULL && backslash == NULL) {
		return path;
	}
	if (slash == NULL) {
		return backslash + 1;
	}
	if (backslash == NULL) {
		return slash + 1;
	}
	return (slash > backslash) ? slash + 1 : backslash + 1;
}

static int write_sorted_output(const char *input_filename, const int *arr, size_t n) {
	char output_filename[512];
	FILE *file;
	size_t i;
	const char *name = base_name(input_filename);
	const char *dot = strrchr(name, '.');
	size_t base_length = dot != NULL ? (size_t)(dot - name) : strlen(name);

	if (snprintf(output_filename, sizeof(output_filename), "%.*s_ordenado_insertion.txt", (int)base_length, name) >= (int)sizeof(output_filename)) {
		return 0;
	}

	file = fopen(output_filename, "w");
	if (file == NULL) {
		return 0;
	}

	fprintf(file, "%zu\n", n);
	for (i = 0; i < n; i++) {
		fprintf(file, "%d\n", arr[i]);
	}

	fclose(file);
	printf("Saida ordenada salva em: %s\n", output_filename);
	return 1;
}

int main(int argc, char *argv[]) {
	const char *filename;
	int *values;
	size_t n;
	clock_t startTime;
	clock_t endTime;
	double elapsedMs;

	if (argc != 2) {
		printf("Uso: %s <arquivo_de_entrada.txt>\n", argv[0]);
		return 1;
	}

	filename = argv[1];
	values = load_dataset(filename, &n);
	if (values == NULL) {
		printf("Erro ao ler o arquivo: %s\n", filename);
		return 1;
	}

	printf("Arquivo: %s\n", filename);
	printf("Quantidade de elementos: %zu\n", n);

	startTime = clock();
	insertion_sort(values, n);
	endTime = clock();

	elapsedMs = ((double)(endTime - startTime) * 1000.0) / CLOCKS_PER_SEC;

	printf("Tempo de execucao: %.3f ms\n", elapsedMs);
	printf("Ordenado corretamente: %s\n", is_sorted(values, n) ? "sim" : "nao");
	if (!write_sorted_output(filename, values, n)) {
		printf("Aviso: nao foi possivel salvar o arquivo ordenado.\n");
	}
	printf("Amostra do resultado: ");
	print_sample(values, n);
	printf("\n");

	free(values);
	return 0;
}
