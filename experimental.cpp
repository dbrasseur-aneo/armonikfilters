#include "armonik/api/experimental/experimental.h"
#include "armonik/client/tasks_common.pb.h"
#include "armonik/client/tasks_service.grpc.pb.h"
#include "armonik/common/options/ControlPlane.h"
#include <grpc++/grpc++.h>
#include <memory>

bool ShutdownOnFailure(std::shared_ptr<grpc::Channel> channel) {
  switch ((*channel).GetState(true)) {
  case GRPC_CHANNEL_TRANSIENT_FAILURE:
    channel.reset();
  case GRPC_CHANNEL_SHUTDOWN:
    return true;

  case GRPC_CHANNEL_CONNECTING:
  case GRPC_CHANNEL_IDLE:
  case GRPC_CHANNEL_READY:
  default:
    return false;
  }
}

armonik::api::experimental::RawTask
task_detailed_to_rawtask(const armonik::api::grpc::v1::tasks::TaskDetailed &detailed) {
  return {detailed.id(),
          detailed.session_id(),
          detailed.owner_pod_id(),
          detailed.initial_task_id(),
          std::vector<std::string>(detailed.parent_task_ids().begin(), detailed.parent_task_ids().end()),
          std::vector<std::string>(detailed.data_dependencies().begin(), detailed.data_dependencies().end()),
          std::vector<std::string>(detailed.expected_output_ids().begin(), detailed.expected_output_ids().end()),
          std::vector<std::string>(detailed.retry_of_ids().begin(), detailed.retry_of_ids().end()),
          static_cast<armonik::api::experimental::TaskStatus>(detailed.status()),
          detailed.status_message(),
          {std::vector<std::pair<std::string, std::string>>(detailed.options().options().begin(),
                                                            detailed.options().options().end()),
           {detailed.options().max_duration().seconds(), detailed.options().max_duration().nanos()},
           detailed.options().max_retries(),
           detailed.options().priority(),
           detailed.options().partition_id(),
           detailed.options().application_name(),
           detailed.options().application_version(),
           detailed.options().application_namespace(),
           detailed.options().application_service(),
           detailed.options().engine_type()},
          {detailed.created_at().seconds(), detailed.created_at().nanos()},
          {detailed.submitted_at().seconds(), detailed.submitted_at().nanos()},
          {detailed.started_at().seconds(), detailed.started_at().nanos()},
          {detailed.ended_at().seconds(), detailed.ended_at().nanos()},
          {detailed.creation_to_end_duration().seconds(), detailed.creation_to_end_duration().nanos()},
          {detailed.processing_to_end_duration().seconds(), detailed.processing_to_end_duration().nanos()},
          {detailed.pod_ttl().seconds(), detailed.pod_ttl().nanos()},
          {detailed.output().success(), detailed.output().error()},
          detailed.pod_hostname(),
          {detailed.received_at().seconds(), detailed.received_at().nanos()},
          {detailed.acquired_at().seconds(), detailed.acquired_at().nanos()}};
}

std::vector<armonik::api::experimental::RawTask>
armonik::api::experimental::list_tasks_in_session(const std::string &endpoint, const std::string &session_id) {
  const int page_size = 500;
  int retries = 5;
  std::vector<RawTask> tasks;

  std::shared_ptr<::grpc::Channel> channel = ::grpc::CreateChannel(endpoint, ::grpc::InsecureChannelCredentials());
  auto stub = armonik::api::grpc::v1::tasks::Tasks::NewStub(channel);

  armonik::api::grpc::v1::tasks::FilterField filterField;
  armonik::api::grpc::v1::tasks::ListTasksRequest request;
  armonik::api::grpc::v1::tasks::ListTasksDetailedResponse response;

  request.set_page_size(page_size);
  request.set_with_errors(false);

  request.mutable_sort()->mutable_field()->mutable_task_summary_field()->set_field(
      grpc::v1::tasks::TASK_SUMMARY_ENUM_FIELD_CREATED_AT);
  request.mutable_sort()->set_direction(grpc::v1::sort_direction::SORT_DIRECTION_ASC);

  filterField.mutable_field()->mutable_task_summary_field()->set_field(
      grpc::v1::tasks::TASK_SUMMARY_ENUM_FIELD_SESSION_ID);
  filterField.mutable_filter_string()->set_value(session_id);
  filterField.mutable_filter_string()->set_operator_(grpc::v1::FILTER_STRING_OPERATOR_EQUAL);

  *request.mutable_filters()->mutable_or_()->Add()->mutable_and_()->Add() = filterField;

  int page = 0;

  do {
    request.set_page(page);
    ::grpc::ClientContext context;
    try {
      auto status = stub->ListTasksDetailed(&context, request, &response);
      if (!status.ok()) {
        throw std::runtime_error(status.error_message());
      }
      ++page;
      tasks.reserve(response.total());
      for (auto &&t : response.tasks()) {
        tasks.push_back(task_detailed_to_rawtask(t));
      }

      response.clear_page();
      response.clear_page_size();
      response.clear_tasks();

    } catch (const std::exception &e) {
      retries--;
      std::cerr << "Error during call : " << e.what() << std::endl;
      if (ShutdownOnFailure(channel) && retries > 0) {
        channel = ::grpc::CreateChannel(endpoint, ::grpc::InsecureChannelCredentials());
        stub = armonik::api::grpc::v1::tasks::Tasks::NewStub(channel);
      }
      if (retries <= 0) {
        throw std::runtime_error("Max retries exceeded");
      }
    }

  } while (response.total() > tasks.size());

  return tasks;
}
std::string armonik::api::experimental::status_name_from_value(const armonik::api::experimental::TaskStatus &status) {
  static const std::map<TaskStatus, std::string> values = {
      {TASK_STATUS_UNSPECIFIED, "TASK_STATUS_UNSPECIFIED"}, /** Task is in an unknown state. */
      {TASK_STATUS_CREATING, "TASK_STATUS_CREATING"},       /** Task is being created in database. */
      {TASK_STATUS_SUBMITTED, "TASK_STATUS_SUBMITTED"},     /** Task is submitted to the queue. */
      {TASK_STATUS_DISPATCHED, "TASK_STATUS_DISPATCHED"},   /** Task is dispatched to a worker. */
      {TASK_STATUS_COMPLETED, "TASK_STATUS_COMPLETED"},     /** Task is completed. */
      {TASK_STATUS_ERROR, "TASK_STATUS_ERROR"},             /** Task is in error state. */
      {TASK_STATUS_TIMEOUT, "TASK_STATUS_TIMEOUT"},         /** Task is in timeout state. */
      {TASK_STATUS_CANCELLING, "TASK_STATUS_CANCELLING"},   /** Task is being cancelled. */
      {TASK_STATUS_CANCELLED, "TASK_STATUS_CANCELLED"},     /** Task is cancelled. */
      {TASK_STATUS_PROCESSING, "TASK_STATUS_PROCESSING"},   /** Task is being processed. */
      {TASK_STATUS_PROCESSED, "TASK_STATUS_PROCESSED"},     /** Task is processed. */
      {TASK_STATUS_RETRIED, "TASK_STATUS_RETRIED"}};
  return values.at(status);
}
