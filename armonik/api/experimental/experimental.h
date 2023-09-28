#pragma once

#include <string>
#include <vector>

namespace armonik {
namespace api {
namespace experimental {

extern "C" {

enum TaskStatus {
  TASK_STATUS_UNSPECIFIED = 0, /** Task is in an unknown state. */
  TASK_STATUS_CREATING = 1,    /** Task is being created in database. */
  TASK_STATUS_SUBMITTED = 2,   /** Task is submitted to the queue. */
  TASK_STATUS_DISPATCHED = 3,  /** Task is dispatched to a worker. */
  TASK_STATUS_COMPLETED = 4,   /** Task is completed. */
  TASK_STATUS_ERROR = 5,       /** Task is in error state. */
  TASK_STATUS_TIMEOUT = 6,     /** Task is in timeout state. */
  TASK_STATUS_CANCELLING = 7,  /** Task is being cancelled. */
  TASK_STATUS_CANCELLED = 8,   /** Task is cancelled. */
  TASK_STATUS_PROCESSING = 9,  /** Task is being processed. */
  TASK_STATUS_PROCESSED = 10,  /** Task is processed. */
  TASK_STATUS_RETRIED = 11     /** Task is retried. */
};

struct Duration {
  int64_t seconds;
  int32_t nanos;
};

struct Timestamp {
  int64_t seconds;
  int32_t nanos;
};

struct Output {
  int success;
  std::string error;
};

struct TaskOptions {
  std::vector<std::pair<std::string, std::string>> options;
  Duration max_duration;
  int max_retries;
  int priority;
  std::string partition_id;
  std::string application_name;
  std::string application_version;
  std::string application_namespace;
  std::string application_service;
  std::string engine_type;
};

struct RawTask {
  std::string id;
  std::string session_id;
  std::string owner_pod_id;

  std::string initial_task_id;
  std::vector<std::string> parent_task_ids;
  std::vector<std::string> data_dependencies;
  std::vector<std::string> expected_output_ids;
  std::vector<std::string> retry_of_ids;

  TaskStatus status;
  std::string status_message;

  TaskOptions options;

  Timestamp created_at;
  Timestamp submitted_at;
  Timestamp started_at;
  Timestamp ended_at;
  Duration creation_to_end_duration;
  Duration processing_to_end_duration;
  Timestamp pod_ttl;

  Output output;

  std::string pod_hostname;
  Timestamp received_at;
  Timestamp acquired_at;
};
}

std::vector<RawTask> list_tasks_in_session(const std::string &endpoint, const std::string &session_id);
std::string status_name_from_value(const TaskStatus &status);

} // namespace experimental
} // namespace api
} // namespace armonik
