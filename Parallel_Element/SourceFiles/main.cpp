#include "./ScheduleType.h"
#include "./particleManipulation.h"
#include <iostream>
#include <omp.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

int main() {
  std::string type = select_schedule_type();

  if (type == "auto") omp_set_schedule(omp_sched_auto, 0);
  if (type == "static") omp_set_schedule(omp_sched_static, 0);
  if (type == "dynamic") omp_set_schedule(omp_sched_dynamic, 0);
  if (type == "guided") omp_set_schedule(omp_sched_guided, 0);
  particleManipulation();
  return 0;
}
