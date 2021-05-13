#include "clusterManagement.h"
#include "globalVariables.h"

#include <mpi.h>


void finalize(){
  MPI_Comm_rank(MPI_COMM_WORLD, &PROCESS_RANK);
  //MPI_Barrier(MPI_COMM_WORLD);
  int result = MPI_Finalize();
  if (result != 0)
  {
    MPI_Abort(MPI_COMM_WORLD, result);
  }
}

void initialise(int argc, char **argv){
  int result = MPI_Init(&argc, &argv);
  if (result != 0)
  {
    MPI_Abort(MPI_COMM_WORLD, result);
  }
  MPI_Comm_size(MPI_COMM_WORLD, &CLUSTER_SIZE);
  SLAVE_COUNT = CLUSTER_SIZE - 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &PROCESS_RANK);
}
