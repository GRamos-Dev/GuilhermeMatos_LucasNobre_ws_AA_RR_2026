/*
 * Comparador entre Insertion Sort e Quick Sort.
 * Le os mesmos arquivos de entrada, mede o tempo de cada algoritmo
 * e executa um cache thrashing entre as duas medicoes para reduzir
 * interferencias de cache na comparacao.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LAST_RESULTS_FILE "ultimo_compare_sorts.csv"

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

static void copy_dataset(int *dest, const int *src, size_t n) {
	memcpy(dest, src, n * sizeof(int));
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

static int write_sorted_output(const char *input_filename, const char *algorithm_suffix, const int *arr, size_t n) {
	char output_filename[512];
	FILE *file;
	size_t i;
	const char *name = base_name(input_filename);
	const char *dot = strrchr(name, '.');
	size_t base_length = dot != NULL ? (size_t)(dot - name) : strlen(name);

	if (snprintf(output_filename, sizeof(output_filename), "%.*s_ordenado_%s.txt", (int)base_length, name, algorithm_suffix) >= (int)sizeof(output_filename)) {
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
	return 1;
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

static void swap_int(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

static int partition(int *arr, int low, int high) {
	int pivot = arr[low + (high - low) / 2];
	int i = low - 1;
	int j = high + 1;

	while (1) {
		do {
			i++;
		} while (arr[i] < pivot);

		do {
			j--;
		} while (arr[j] > pivot);

		if (i >= j) {
			return j;
		}

		swap_int(&arr[i], &arr[j]);
	}
}

static void quick_sort_recursive(int *arr, int low, int high) {
	if (low < high) {
		int p = partition(arr, low, high);
		quick_sort_recursive(arr, low, p);
		quick_sort_recursive(arr, p + 1, high);
	}
}

/*
 * Tenta reduzir o efeito de cache entre as duas execucoes.
 * Em C portavel nao existe uma limpeza real de cache de CPU, entao
 * usamos um buffer grande e o percorremos para desalojar dados antigos.
 */
static void flush_cache(void) {
	const size_t flush_size = 64ULL * 1024ULL * 1024ULL;
	static unsigned char *buffer = NULL;
	volatile unsigned char sink = 0;
	size_t i;

	if (buffer == NULL) {
		buffer = (unsigned char *)malloc(flush_size);
		if (buffer == NULL) {
			return;
		}
		memset(buffer, 1, flush_size);
	}

	for (i = 0; i < flush_size; i += 64) {
		sink ^= buffer[i];
	}

	if (sink == 255) {
		printf(" ");
	}
}

static double elapsed_ms(clock_t startTime, clock_t endTime) {
	return ((double)(endTime - startTime) * 1000.0) / CLOCKS_PER_SEC;
}

static int write_results_header(FILE *results) {
	return fprintf(results, "arquivo,n,insertion_ms,quick_ms,speedup\n") > 0;
}

static int write_result_row(FILE *results, const char *arquivo, size_t n, double insertionMs, double quickMs) {
	double speedup = (quickMs > 0.0) ? (insertionMs / quickMs) : 0.0;
	return fprintf(results, "%s,%zu,%.6f,%.6f,%.6f\n", arquivo, n, insertionMs, quickMs, speedup) > 0;
}

int main(int argc, char *argv[]) {
	const char *default_files[] = {
		"entrada_10000.txt",
		"entrada_100000.txt",
		"entrada_1000000.txt"
	};
	const char **files;
	int file_count;
	int argIndex;
	FILE *results;

	if (argc > 1) {
		files = (const char **)&argv[1];
		file_count = argc - 1;
	} else {
		files = default_files;
		file_count = (int)(sizeof(default_files) / sizeof(default_files[0]));
	}

	printf("Comparacao entre Insertion Sort e Quick Sort\n");
	printf("%-24s %-12s %-15s %-15s %-12s\n", "arquivo", "n", "insertion(ms)", "quick(ms)", "speedup");

	results = fopen(LAST_RESULTS_FILE, "w");
	if (results == NULL) {
		printf("Aviso: nao foi possivel criar %s\n", LAST_RESULTS_FILE);
	} else if (!write_results_header(results)) {
		printf("Aviso: nao foi possivel escrever cabecalho em %s\n", LAST_RESULTS_FILE);
		fclose(results);
		results = NULL;
	}

	for (argIndex = 0; argIndex < file_count; argIndex++) {
		int *base;
		int *work;
		size_t n;
		clock_t startTime;
		clock_t endTime;
		double insertionMs;
		double quickMs;

		base = load_dataset(files[argIndex], &n);
		if (base == NULL) {
			printf("Erro ao ler o arquivo: %s\n", files[argIndex]);
			continue;
		}

		work = (int *)malloc(n * sizeof(int));
		if (work == NULL) {
			printf("Erro de memoria para %s\n", files[argIndex]);
			free(base);
			continue;
		}

		copy_dataset(work, base, n);
		startTime = clock();
		insertion_sort(work, n);
		endTime = clock();
		insertionMs = elapsed_ms(startTime, endTime);
		printf("%-24s %-12zu %-15.3f ", files[argIndex], n, insertionMs);
		if (!write_sorted_output(files[argIndex], "insertion", work, n)) {
			printf("Aviso: nao foi possivel salvar a saida do insertion sort.\n");
		}

		flush_cache();

		copy_dataset(work, base, n);
		startTime = clock();
		if (n > 0) {
			quick_sort_recursive(work, 0, (int)n - 1);
		}
		endTime = clock();
		quickMs = elapsed_ms(startTime, endTime);
		if (!write_sorted_output(files[argIndex], "quick", work, n)) {
			printf("Aviso: nao foi possivel salvar a saida do quick sort.\n");
		}

		printf("%-15.3f %-12.2f\n", quickMs, (quickMs > 0.0) ? (insertionMs / quickMs) : 0.0);
		if (results != NULL && !write_result_row(results, files[argIndex], n, insertionMs, quickMs)) {
			printf("Aviso: falha ao salvar resultado em %s\n", LAST_RESULTS_FILE);
			fclose(results);
			results = NULL;
		}

		if (!is_sorted(work, n)) {
			printf("Aviso: quick sort nao ordenou corretamente em %s\n", files[argIndex]);
		}

		free(work);
		free(base);
		flush_cache();
	}

	if (results != NULL) {
		fclose(results);
		printf("Resultados salvos em: %s\n", LAST_RESULTS_FILE);
	}

	return 0;
}