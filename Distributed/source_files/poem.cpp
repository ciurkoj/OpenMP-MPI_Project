#include "poem.h"
#include "globalVariables.h"
// #include "message.h"
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <string>
#include <vector>

// read the poem and cast it into a vector
std::vector<std::string> readPoemLines()
{
    std::vector<std::string> mPoemLines;
    std::ifstream file("A_Woman's_Heart.txt");
    if (file.is_open())
    {
        std::string single_line;
        while (std::getline(file, single_line))
        {
            mPoemLines.push_back(single_line);
        }
    }
    file.close();
    return mPoemLines;
};
void linesToBeExpected(int totalNumberOfLines)
{
    for (int slave = 1; slave <= SLAVE_COUNT; slave++)
    {
        // calculate how many lines need to be send
        int numberOfLines = totalNumberOfLines / SLAVE_COUNT;
        if (totalNumberOfLines % SLAVE_COUNT >= slave)
        {
            numberOfLines++;
        }
        // sending line
        MPI_Send(&numberOfLines, 1, MPI_INT, slave, TAG_NUMBER_OF_LINES, MPI_COMM_WORLD);
    }
}

MPI_Datatype create_message_data_type()
{
    MPI_Datatype message_type;
    int lengths[3] = {1, 1, 50};

    MPI_Aint displacements[3];
    struct message_t message;
    MPI_Aint base_address;
    MPI_Get_address(&message, &base_address);
    MPI_Get_address(&message.node, &displacements[0]);
    MPI_Get_address(&message.index, &displacements[1]);
    MPI_Get_address(&message.line[0], &displacements[2]);
    displacements[0] = MPI_Aint_diff(displacements[0], base_address);
    displacements[1] = MPI_Aint_diff(displacements[1], base_address);
    displacements[2] = MPI_Aint_diff(displacements[2], base_address);

    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_CHAR};
    MPI_Type_create_struct(3, lengths, displacements, types, &message_type);
    MPI_Type_commit(&message_type);
    return message_type;
};

void sendLine(const int node, const std::string line, int lineIndex ,int tag)
{
    MPI_Datatype message_type = create_message_data_type();
    struct message_t buffer;
    buffer.node = node;
    buffer.index = lineIndex;
    strncpy(buffer.line, line.c_str(), 50);
    buffer.line[50] = '\0';
    MPI_Send(&buffer, 1, message_type, node, tag, MPI_COMM_WORLD);
}

void distributeLines(std::vector<std::string> lines)
{
    size_t lineIndex = 0;
    for (std::string line : lines)
    {
        for (int slave = 0; slave < CLUSTER_SIZE; slave++)
        {
            if (slave == MASTER_RANK)
            {
                continue;
            }
            if (lineIndex >= lines.size())
            {
                break;
            }
            sendLine(slave, lines.at(lineIndex), lineIndex + 1, TAG_LINE);
            lineIndex++;
        }
    }
}
message_t getMessage(int tag, int node)
{
    MPI_Datatype message_type = create_message_data_type();
    struct message_t received_message;
    MPI_Recv(&received_message, 1, message_type, node, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return received_message;
}

void headNode()
{
    std::vector<std::string> lines = readPoemLines();
    linesToBeExpected(lines.size());
    distributeLines(lines);
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout<<std::endl;
    std::vector<message_t> gatheredMessages;
    for (int i=0;i<lines.size();i++){
        
    message_t message;
    message = getMessage(TAG_LINE_TO_HEAD_NODE, MPI_ANY_SOURCE);
    gatheredMessages.push_back(message);
    }

    for (message_t message : gatheredMessages){
        std::cout<<message.index<<". "<<message.line<<std::endl;
    }
}

int getNumberOfLines()
{
    int numberOfLines = 0;
    MPI_Recv(&numberOfLines, 1, MPI_INT, MASTER_RANK, TAG_NUMBER_OF_LINES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return numberOfLines;
}

void clusterNode(int clusterRank)
{
    std::vector<message_t> messages;
    int numberOfLines = getNumberOfLines();
    for (int lineNumber = 0; lineNumber < numberOfLines; lineNumber++)
    {
        message_t received_message = getMessage(TAG_LINE, MASTER_RANK);
        messages.push_back(received_message);
    }

    for (message_t message : messages)
    {
        sendLine(MASTER_RANK,message.line,message.index, TAG_LINE_TO_HEAD_NODE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}
