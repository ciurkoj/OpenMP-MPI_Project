#include "./clusterInfo.h"
#include "./Node.h"
#include "globalVariables.h"
#include "mpi.h"
#include "omp.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

std::vector<std::string> getClockSpeed()
{
    std::ifstream cpuInfo("/proc/cpuinfo");
    std::string clockSpeed;
    std::vector<std::string> speeds;
    // Find the line where the clock speed is listed.
    while (getline(cpuInfo, clockSpeed))
    {
        if (clockSpeed.find("cpu MHz", 0) != std::string::npos)
        {
            speeds.push_back(clockSpeed);
        }
    }
    cpuInfo.close();
    // Return the first line containing CPU clock speeds.
    return speeds;
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

// void stats()
// {
//     const unsigned int numberOfCores = std::thread::hardware_concurrency();
//     std::string clockSpeed;
//     std::string systemRAM;
//     std::string homeFolderMBs;

// #ifdef __linux__
//     clockSpeed = run("cat /proc/cpuinfo | grep \"cpu MHz\"");
//     systemRAM = run("free -t");
//     homeFolderMBs = run(" df -h --total");
// #else

// #endif

//     std::cout << "Total core count: " << numberOfCores << std::endl;
//     std::cout << "Core clock speed: " << clockSpeed << std::endl;
//     std::cout << "Total system RAM: " << systemRAM << "GB" << std::endl;
//     std::cout << "Home folder size: " << homeFolderMBs << std::endl;
// }

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
    MPI_Reduce(&node_core_count, &total_core_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    memset(processor_name + name_len, 0, MPI_MAX_PROCESSOR_NAME - name_len);

    if (PROCESS_RANK == 0)
    {

        masterNode = new MasterNode(processor_name, CLUSTER_SIZE, "  ", 1, total_core_count);
        std::cout << "Total cluster core count: " << masterNode->nodeCount << "\n";
        std::cout << "Total node count: " << masterNode->coreNumber << " nodes\n";
        std::cout << "Master node: " << masterNode->processor_name << '\n';

        MPI_Barrier(MPI_COMM_WORLD);
    }

    else
    {
        clockSpeed = run("cat /proc/cpuinfo | grep \"cpu MHz\"");
        int numberOfCoresPerNode = std::thread::hardware_concurrency();
        std::vector<std::string> core_clock_speed = getClockSpeed();
        long RAM = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);

        std::cout << "*********" << std::endl
                  << "*********" << std::endl;
        std::cout << "Node name: " << processor_name << std::endl;
        std::cout << "Core number: " << numberOfCoresPerNode << std::endl;
        std::cout << "Core clock speed:";
        for (std::vector<std::string>::iterator it = core_clock_speed.begin(); it != core_clock_speed.end(); ++it)
        {
            std::cout << "\n " << *it;
        }
        std::cout << "\nRam memory: " << RAM / 1000 / 1000 / 1000 << "GB" << std::endl;

        MPI_Barrier(MPI_COMM_WORLD);
    }
    
}
