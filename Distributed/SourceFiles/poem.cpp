#include "poem.h"
#include "globalVariables.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <string>
#include <vector>

/** readPoemLines reads poem into the program memory
 * @return Poem split into a vector of strings.
 */
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

/** linesToBeExpected calculate how many lines a node should expect
 * @param totalNumberOfLines an integer of total number of lines in the original text input
 * @return MPI_Send sends a number of lines to expect of a type MPI_INT 
 */
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

/** createMessageDataType creates a custom MPI data type that is sent to other node.
 * @return custom MPI_Datatype message type
 */
MPI_Datatype createMessageDataType()
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

/** Sends line between nodes.
 * @param node is a recipient of the message,
 * @param line is a string type of the message,
 * @param lineIndex is a line index in the original poem.
 * @param tag is a message identifier.
 * @return Sends message of a message_t datatype with MPI_Send
 */
void sendLine(const int node, const std::string line, int lineIndex, int tag)
{
    MPI_Datatype message_type = createMessageDataType();
    struct message_t buffer;
    buffer.node = node;
    buffer.index = lineIndex;
    strncpy(buffer.line, line.c_str(), 50);
    buffer.line[50] = '\0';
    MPI_Send(&buffer, 1, message_type, node, tag, MPI_COMM_WORLD);
}

/** distributeLines sends each line in the lines vector to the cluster nodes.
 * @param lines is a vector of the original poem lines
 * @return distributes lines between nodes, except for head node.
 */
void distributeLines(std::vector<std::string> lines)
{
    size_t lineIndex = 0;
    for (std::string line : lines)
    {
        for (int cluster_node = 0; cluster_node < CLUSTER_SIZE; cluster_node++)
        {
            if (cluster_node == MASTER_RANK)
            {
                continue;
            }
            if (lineIndex >= lines.size())
            {
                break;
            }
            sendLine(cluster_node, lines.at(lineIndex), lineIndex + 1, TAG_LINE);
            lineIndex++;
        }
    }
}

/** Gets messages from specified node
 * @param tag is a message tag identifier.
 * @param node is a sender from whom to expect the message
 * @return received_message of a struct message_t, is a custom data type message.
 */
message_t getMessage(int tag, int node)
{
    MPI_Datatype message_type = createMessageDataType();
    struct message_t received_message;
    MPI_Recv(&received_message, 1, message_type, node, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return received_message;
}

/** headNode() manages poem distribution and collection, then prints out the poem from received messages.
 */
void headNode()
{
    std::vector<std::string> lines = readPoemLines();
    linesToBeExpected(lines.size());
    distributeLines(lines);
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << std::endl;

    /** Collect all messages sent by cluster nodes 
     * @param gatheredMessages A vector container of all messages received from cluster nodes.
     */
    std::vector<message_t> gatheredMessages;
    for (int i = 0; i < lines.size(); i++)
    {
        message_t message;
        message = getMessage(TAG_LINE_TO_HEAD_NODE, MPI_ANY_SOURCE);
        gatheredMessages.push_back(message);
    }

    /* Print out all messages */
    for (message_t message : gatheredMessages)
    {
        std::cout << message.index << ". " << message.line << std::endl;
    }
}

/** getNumberOfLines() get a message with information about how many lines of poem to expect
 * Uses MPI_Recv with a tag "TAG_NUMBER_OF_LINES" to identify the correct message
 * Sender is set to MASTER_RANK, as the information comes from the head node.
 * @return An integer how many lines to expect 
 */
int getNumberOfLines()
{
    int numberOfLines = 0;
    MPI_Recv(&numberOfLines, 1, MPI_INT, MASTER_RANK, TAG_NUMBER_OF_LINES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return numberOfLines;
}

/** clusterNode() receives messages from the head node ands sends back received lines.
 */
void clusterNode()
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
        sendLine(MASTER_RANK, message.line, message.index, TAG_LINE_TO_HEAD_NODE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}
