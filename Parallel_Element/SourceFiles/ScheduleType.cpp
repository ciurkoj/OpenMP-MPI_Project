#include <iostream>
#include <stdlib.h>
#include <string>

#include "ScheduleType.h"

std::string select_schedule_type() {
  std::string type;
  do {
    std::cout << "What type of scheduling would you like?(auto, static, dynamic, guided) ";
    std::getline(std::cin, type);
    if (type == "auto") return type;
    if (type == "static") return type;
    if (type == "dynamic") return type;
    if (type == "guided") return type;
  } while (true);
  return type;
}