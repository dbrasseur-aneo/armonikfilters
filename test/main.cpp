#include "armonik/api/experimental/experimental.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

std::string to_time(armonik::api::experimental::Timestamp ts){
  char buffer[128];
  std::strftime(buffer, 128, "%Y-%m-%d-%H-%M-%S", std::gmtime(&ts.seconds));
  std::stringstream out;
  out << buffer << '.' << std::setfill('0') << std::setw(9) << ts.nanos;
  return out.str();
}

std::string to_duration(armonik::api::experimental::Duration d){
  std::stringstream out;
  out << d.seconds << '.' <<std::setfill('0') << std::setw(9) << d.nanos;
  return out.str();
}

int main(int argc, char* argv[]){
  if(argc < 3){
    std::cout << "Usage : " << argv[0] << " endpoint session_id" << std::endl;
    return 1;
  }
  auto tasks = armonik::api::experimental::list_tasks_in_session(argv[1], argv[2]);
  std::cout << "Found " << tasks.size() << " tasks in session " << argv[2] << '\n';
  for(auto && t: tasks) {
    std::cout << t.id << " : Status " << armonik::api::experimental::status_name_from_value(t.status) << " , Created at " << to_time(t.created_at) << " , Ended at " << to_time(t.ended_at) << " , Processing duration " << to_duration(t.processing_to_end_duration) << '\n';
  }
  std::cout << std::endl;
  return 0;
}