#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "days.h"

static int64_t factorial(int64_t num) {
    int64_t total = 1;
    while(num > 1)
        total *= num--;
    return total;
}

static void swap(int64_t* a, int64_t* b) {
    int64_t temp = *a;
    *a = *b;
    *b = temp;
}

static void permute(int64_t* array, size_t n, size_t size, Queue* q) {
    if (n == 1) {
        int64_t* perm = calloc(size, sizeof(int64_t));
        memcpy(perm, array, size * sizeof(int64_t));
        queue_push(q, perm);
    }

    for (size_t i = 0; i < n; i++) {
        permute(array, n - 1, size, q);
        swap(&array[n % 2 == 1 ? 0 : i], &array[n - 1]);
    }
}

void day7(VMProgram prog) {
    int64_t vals[] = { 0, 1, 2, 3, 4 };
    Queue q = queue_create();
    permute(vals, 5, 5, &q);

    int64_t* perm;
    int64_t maxsig = 0;
    while (queue_try_remove(&q, (void**) &perm)) {
        int64_t input_val = 0;
        for (size_t i = 0; i < 5; i++) {
            VM vm = vm_create();
            vm_load(&vm, &prog);
            vm_append_input(&vm, perm[i]);
            vm_append_input(&vm, input_val);
            vm_run_til_halt(&vm);
            vm_try_get_output(&vm, &input_val);
            vm_destroy(&vm);
        }
        free(perm);
        maxsig = maxsig > input_val ? maxsig : input_val;
    }

    printf("%ld\n", maxsig);

    queue_destroy(&q, &free);
}

void day7b(VMProgram prog) {
}