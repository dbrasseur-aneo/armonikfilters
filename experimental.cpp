#include "experimental.h"
#include <memory>
#include "armonik/client/tasks_common.pb.h"
#include "armonik/client/tasks_service.grpc.pb.h"
#include "armonik/common/options/ControlPlane.h"
#include <grpc++/grpc++.h>
#include <iostream>

void hello() {
    std::cout << "Hello, World!" << std::endl;
}
std::vector<armonik::api::experimental::RawTask> armonik::api::experimental::list_tasks_in_session(const armonik::api::common::utils::Configuration& config, const std::string& session_id) {
    const int page_size = 500;
    std::vector<RawTask> tasks;

    armonik::api::common::options::ControlPlane control_plane = config.get_control_plane();

    std::shared_ptr<::grpc::Channel> channel = ::grpc::CreateChannel(std::string(control_plane.getEndpoint()) , ::grpc::InsecureChannelCredentials());
    auto stub = armonik::api::grpc::v1::tasks::Tasks::NewStub(channel);


    armonik::api::grpc::v1::tasks::FilterField filterField;
    armonik::api::grpc::v1::tasks::ListTasksRequest request;
    armonik::api::grpc::v1::tasks::ListTasksDetailedResponse response;

    request.set_page_size(page_size);
    request.set_with_errors(false);

    request.mutable_sort()->mutable_field()->mutable_task_summary_field()->set_field(grpc::v1::tasks::TASK_SUMMARY_ENUM_FIELD_ENDED_AT);

    filterField.mutable_field()->mutable_task_summary_field()->set_field(grpc::v1::tasks::TASK_SUMMARY_ENUM_FIELD_SESSION_ID);
    filterField.mutable_filter_string()->set_value(session_id);
    filterField.mutable_filter_string()->set_operator_(grpc::v1::FILTER_STRING_OPERATOR_EQUAL);

    *request.mutable_filters()->mutable_or_()->Add()->mutable_and_()->Add() = filterField;



    int page=0;


    do{
      request.set_page(page++);
      ::grpc::ClientContext context;
      auto status = stub->ListTasksDetailed(&context,request, &response);

    }while(response.total() > page*page_size);


    return tasks;

}
