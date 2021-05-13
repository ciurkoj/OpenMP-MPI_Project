#pragma once
#include <string>
#include <vector>

class Node
{
public:
    Node(std::string processor_name,
         int coreNumber,
         std::string core_clock_speed,
         long memory);

    std::string processor_name;
    int coreNumber;
    std::string core_clock_speed;
    long memory;

    std::string nodeInfo();
};

class MasterNode : public Node
{

public:
    int nodeCount;
    MasterNode(std::string processor_name,
               int coreNumber,
               std::string core_clock_speed,
               long memory, int nodeCount);
};
