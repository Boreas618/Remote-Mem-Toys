#include <memkind.h>
#include <stdio.h>
#include <numa.h>
#include <numaif.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE (1024 * 1024 * 16) // Array size of 256 M

void workload_pointer_chasing(size_t *array, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        size_t target_idx = rand() % size;
        array[i] = target_idx;
    }

    size_t index = 0;
    for (size_t i = 0; i < size; i++)
    {
        if (rand() % 6 == 1) {
            array[index] = (array[index] == 0) ? array[index] : array[index] - 1;
        }
        index = array[index];
    }
}

void *malloc_on_emulated_cxl(size_t size)
{
    void *ptr = memkind_malloc(MEMKIND_DAX_KMEM, size);
    return ptr;
}

void free_on_emulated_cxl(void *ptr)
{
    memkind_free(MEMKIND_DAX_KMEM, ptr);
}

int main()
{

    if (numa_available() == -1)
    {
        fprintf(stderr, "NUMA is not available\n");
        return 1;
    }

    int num_nodes = numa_max_node() + 1;
    if (num_nodes < 2)
    {
        fprintf(stderr, "A minimum of two NUMA nodes is required\n");
        return 1;
    }

    // Allocate memory on node 0
    size_t *local_memory = (size_t *)numa_alloc_onnode(SIZE * sizeof(size_t), 0);
    if (!local_memory)
    {
        fprintf(stderr, "Failed to allocate local memory\n");
        return 1;
    }

    // Allocate memory on node 1
    size_t *remote_memory = (size_t *)numa_alloc_onnode(SIZE * sizeof(size_t), 1);
    if (!remote_memory)
    {
        fprintf(stderr, "Failed to allocate remote memory\n");
        return 1;
    }

    size_t *cxl_memory = (size_t *)malloc_on_emulated_cxl(SIZE * sizeof(size_t));
    if (!cxl_memory)
    {
        fprintf(stderr, "Failed to allocate cxl memory\n");
        return 1;
    }

    struct timespec start, end;
    double local_duration, remote_duration, cxl_duration;

    // Test local memory access
    clock_gettime(CLOCK_MONOTONIC, &start);
    workload_pointer_chasing(local_memory, SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    local_duration = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Local memory access time: %f seconds\n", local_duration);

    // Test remote memory access
    clock_gettime(CLOCK_MONOTONIC, &start);
    workload_pointer_chasing(remote_memory, SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    remote_duration = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Remote memory access time: %f seconds\n", remote_duration);

    // Test cxl memory access
    clock_gettime(CLOCK_MONOTONIC, &start);
    workload_pointer_chasing(cxl_memory, SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    cxl_duration = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("CXL memory access time: %f seconds\n", cxl_duration);

    // Cleanup
    numa_free(local_memory, SIZE * sizeof(size_t));
    numa_free(remote_memory, SIZE * sizeof(size_t));
    free_on_emulated_cxl(cxl_memory);

    return 0;
}