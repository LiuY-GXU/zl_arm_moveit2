cmake_minimum_required(VERSION 3.8)
project(zl_arm_hardware)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(fmt REQUIRED)          # fmt::fmt

# 使用 pkg-config 查找 libserial
find_package(PkgConfig REQUIRED)
pkg_check_modules(libserial REQUIRED libserial)

# 包含头文件路径（包括 libserial 的）
include_directories(include ${libserial_INCLUDE_DIRS})

# 构建库
add_library(${PROJECT_NAME} SHARED
  src/zl_arm_hardware_interface.cpp
)

# add_executable(serial_test src/my_serial_test.cpp)
# ament_target_dependencies(serial_test rclcpp)

# ROS 2 ament 依赖
ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  rclcpp_lifecycle
  hardware_interface
  pluginlib
)

# 链接 libserial 和 fmt
target_link_libraries(${PROJECT_NAME}
  ${libserial_LIBRARIES}
  fmt::fmt
   
)



# 导出插件描述文件（确保文件存在）
pluginlib_export_plugin_description_file(hardware_interface
  my_robot_hardware_interface.xml
)

# 安装
install(
  TARGETS ${PROJECT_NAME} 
  DESTINATION lib
)

# install(TARGETS serial_test
# DESTINATION lib/${PROJECT_NAME})


install(
  DIRECTORY include
  DESTINATION include
)

ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(
  hardware_interface
  rclcpp
  rclcpp_lifecycle
  pluginlib
)

ament_package()