#include "poem.h"
#include <vector>
#include <string>
#include <fstream>
#include "globalVariables.h"
#include <mpi.h>
#include <iostream>
#include <sstream>
// read the poem and cast it into a vector
std::vector<std::string> readPoemLines(){
    std::vector<std::string> mPoemLines;
    std::ifstream file ("A_Woman's_Heart.txt");
    if(file.is_open()){
        std::string single_line;
        while(std::getline(file, single_line))
        {
            mPoemLines.push_back(single_line);
        }
    }
    file.close();
    return mPoemLines;

};
void linesToBeExpected( int totalNumberOfLines)
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

void sendLineToSlave(const int slave, const std::string line, int lineIndex)
{
    std::stringstream ssLine;
    ssLine<<lineIndex<<". "<< line;
    std::string mline = ssLine.str();
    const char *message = mline.c_str();
    int messageLength = (int)strlen(message);
    MPI_Send(message, messageLength, MPI_CHAR, slave, TAG_LINE, MPI_COMM_WORLD);

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
      sendLineToSlave(slave, lines.at(lineIndex), lineIndex+1);
      lineIndex++;
    }
  }

}

void headNode()
{
  std::vector<std::string> lines = readPoemLines();
  linesToBeExpected(lines.size());
  distributeLines(lines);
  // MPI_Barrier(MPI_COMM_WORLD);
}

int getNumberOfLines()
{
    int numberOfLines = 0;
    MPI_Recv(&numberOfLines, 1, MPI_INT, MASTER_RANK, TAG_NUMBER_OF_LINES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return numberOfLines;
}

std::string receivePoemLine()
{
  MPI_Status mpi_status;
  MPI_Probe(MASTER_RANK, TAG_LINE, MPI_COMM_WORLD, &mpi_status);
  int count;
  MPI_Get_count(&mpi_status, MPI_CHAR, &count);
  char *buffer = (char *)malloc(sizeof(char) * count + 1);
  MPI_Recv(buffer, count, MPI_CHAR, MASTER_RANK, TAG_LINE, MPI_COMM_WORLD, &mpi_status);
  std::string receivedLine(buffer);
  free(buffer);
  return receivedLine;
}

void clusterNode(int clusterRank)
{
    int numberOfLines = getNumberOfLines();
    for (int lineNumber = 0; lineNumber < numberOfLines; lineNumber++)
    {
        std::string line = receivePoemLine();
        std::cout <<" Node: "<< PROCESS_RANK << " number of lines: " << numberOfLines <<". "<< line << std::endl;
    }
    std::cout<<std::endl;
    // MPI_Barrier(MPI_COMM_WORLD);

}
