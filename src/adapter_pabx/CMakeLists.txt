cmake_minimum_required(VERSION 3.8)
project(adapter_pabx)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(kabam_msgs REQUIRED)
find_package(nlohmann_json REQUIRED)


include_directories(
  include
)
add_executable(announcement_server
  src/announcement_server.cpp
  src/sip_endpoint.cpp
  src/sip_account.cpp
  src/sip_call.cpp
)
target_link_libraries(announcement_server
  nlohmann_json::nlohmann_json
  pjsua2
)
ament_target_dependencies(announcement_server
  rclcpp
  rclcpp_action
  kabam_msgs
)

install(TARGETS announcement_server
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
