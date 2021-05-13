#include "Node.h"
#include <iostream>
#include <string>

Node::Node(std::string processor_name,
           int coreNumber,
           std::string core_clock_speed,
           long memory) : processor_name(processor_name),
                          coreNumber(coreNumber),
                          core_clock_speed(core_clock_speed),
                          memory(memory) {}

std::string Node::nodeInfo()
{

    std::string nodeInfo = "Node name: " + this->processor_name + "\n" +
                           "Core number: " + std::to_string(coreNumber) + "\n" +
                           "Core clock speed:\n" + this->core_clock_speed + "MHz\n\n" +
                           "Ram memory: " + std::to_string(this->memory / 1000 / 1000 / 1000) + "GB\n";
    return nodeInfo;
}

MasterNode::MasterNode(std::string processor_name,
                       int coreNumber,
                       std::string core_clock_speed,
                       long memory, int nodeCount) : Node{processor_name, coreNumber,
                                                          core_clock_speed, memory},
                                                     nodeCount(nodeCount) {}