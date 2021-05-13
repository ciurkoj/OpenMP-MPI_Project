#include "mpi.h"
#include "omp.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

void masterNodeInfo()
{
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Current node's CPU core count and clock speed.
    int node_core_count = omp_get_num_procs();
    char node_name[MPI_MAX_PROCESSOR_NAME];
    // Reducer for determining total cluster core count.
    int total_core_count = 0;
    MPI_Reduce(&node_core_count, &total_core_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (world_rank == 0)
    {
        std::cout << "Total cluster core count: " << total_core_count << '\n';
        std::cout << "Total node count: " << world_size << '\n';
        std::cout << "Master node: " << processor_name << '\n';
    }
}