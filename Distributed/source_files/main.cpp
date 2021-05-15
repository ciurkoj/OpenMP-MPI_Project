#include "./clusterInfo.h"
#include "clusterManagement.h"
#include "globalVariables.h"
#include "poem.h"
#include <iostream>
#include <mpi.h>

int main(int argc, char **argv)
{
    initialise(argc, argv);
    clusterInfo();
    MPI_Barrier(MPI_COMM_WORLD);
    if (PROCESS_RANK == MASTER_RANK)
    {
        headNode();
        // MPI_Barrier(MPI_COMM_WORLD);
    }
    else
    {
        clusterNode(PROCESS_RANK);
        // MPI_Barrier(MPI_COMM_WORLD);
    }
    // switch(PROCESS_RANK){
    //     case MASTER_RANK:
    //     {
    //         headNode();
    //         MPI_Barrier(MPI_COMM_WORLD);
    //     }
    //     default:
    //     {
    //     clusterNode(PROCESS_RANK);
    // MPI_Barrier(MPI_COMM_WORLD);
    //     }
    // }
    finalize();
}
