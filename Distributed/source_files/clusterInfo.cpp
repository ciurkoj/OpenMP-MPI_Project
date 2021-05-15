#include "./clusterInfo.h"
#include "./Node.cpp"
#include "globalVariables.h"
#include "mpi.h"
#include "omp.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

std::vector<Node *> nodes;
std::string getClockSpeed()
{
    std::ifstream cpuInfo("/proc/cpuinfo");
    std::string clockSpeed;
    // Find the line where the clock speed is listed.
    while (getline(cpuInfo, clockSpeed))
    {
        if (clockSpeed.find("cpu MHz", 0) != std::string::npos)
            break;
    }
    cpuInfo.close();
    // Return the first line containing CPU clock speeds.
    return clockSpeed.substr(clockSpeed.find(':') + 1);
}

const char *fileName = "output.txt";

std::string run(std::string command)
{
    std::system((command + ">>" + fileName).c_str());
    std::ifstream file(fileName);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::remove(fileName);
    return buffer.str();
}

void stats()
{
    const unsigned int numberOfCores = std::thread::hardware_concurrency();
    std::string clockSpeed;
    std::string systemRAM;
    std::string homeFolderMBs;

#ifdef __linux__
    clockSpeed = run("cat /proc/cpuinfo | grep \"cpu MHz\"");
    systemRAM = run("free -t");
    homeFolderMBs = run(" df -h --total");
#else

#endif

    std::cout << "Total core count: " << numberOfCores << std::endl;
    std::cout << "Core clock speed: " << clockSpeed << std::endl;
    std::cout << "Total system RAM: " << systemRAM << "GB" << std::endl;
    std::cout << "Home folder size: " << homeFolderMBs << std::endl;
}

void clusterInfo()
{

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Current node's CPU core count and clock speed.
    int node_core_count = omp_get_num_procs();
    // Reducer for determining total cluster core count.
    int total_core_count = 0;
    std::string clockSpeed;
    const MasterNode *masterNode;
    // masterNodeInfo();
    MPI_Reduce(&node_core_count, &total_core_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    memset(processor_name + name_len, 0, MPI_MAX_PROCESSOR_NAME - name_len);
    char letter[30];
    char mesg[] = "heeenlo :P";
    int source = 0;
    int destination = 2;
    if (PROCESS_RANK == 0)
    {

        masterNode = new MasterNode(processor_name, CLUSTER_SIZE, "  ", 1, total_core_count);
        // std::cout<<MPI_COMM_WORLD<<std::endl;
        std::cout << "Total cluster core count: " << masterNode->nodeCount << "\n";
        std::cout << "Total node count: " << masterNode->coreNumber << " nodes\n";
        std::cout << "Master node: " << masterNode->processor_name << '\n';

        MPI_Barrier(MPI_COMM_WORLD);

        //  std::cout << "> hello from "<< processor_name<< std::endl; //standard debugging code, remove later
        //   MPI_Send(&mesg,strlen(mesg)+1, MPI_CHAR, destination, 10, MPI_COMM_WORLD);
    }
    // else if (PROCESS_RANK == destination){
    // // char local_letter = 'z'; // just so we can be certain it's local to this node, and it's initialised with z, so we'll know if nothing's happening.
    //  // this of course means you can't use z.
    //  std::cout << "> hello from "<< processor_name<< std::endl; //standard debugging code, remove later

    //  MPI_Recv(&letter, 30, MPI_CHAR, source, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //  std::cout << "> Character: " << letter << " Received by "<< processor_name<< std::endl;
    // }
    else
    {
        clockSpeed = run("cat /proc/cpuinfo | grep \"cpu MHz\"");
        int numberOfCoresPerNode = std::thread::hardware_concurrency();
        std::string core_clock_speed = getClockSpeed();
        long RAM = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
        nodes.push_back(new Node(processor_name, numberOfCoresPerNode, clockSpeed, RAM));
        // std::cout << processor_name << " - RAM: " << RAM << '\n';
    }

    for (Node *node : nodes)
    {
        std::cout << "*********" << std::endl
                  << "*********" << std::endl;
        std::cout << "Node name: " << node->processor_name << std::endl;
        std::cout << "Core number: " << node->coreNumber << std::endl;
        std::cout << "Core clock speed:" << std::endl
                  << node->core_clock_speed;
        std::cout << "Ram memory: " << node->memory / 1000 / 1000 / 1000 << "GB" << std::endl;

        MPI_Barrier(MPI_COMM_WORLD);
    }

    // stats();
    // Determine physical memory size
}
