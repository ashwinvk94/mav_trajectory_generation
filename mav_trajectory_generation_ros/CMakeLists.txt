cmake_minimum_required(VERSION 2.8.3)
project(mav_trajectory_generation_ros)

find_package(catkin_simple REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

set(CMAKE_MACOSX_RPATH 0)
add_definitions(-std=c++11)

#############
# LIBRARIES #
#############
cs_add_library(${PROJECT_NAME}
  src/feasibility_analytic.cpp
  src/feasibility_base.cpp
  src/feasibility_recursive.cpp
  src/feasibility_sampling.cpp
  src/input_constraints.cpp
  src/ros_conversions.cpp
  src/ros_visualization.cpp
)

############
# BINARIES #
############
cs_add_executable(trajectory_sampler_node
  src/trajectory_sampler_node.cpp
)
target_link_libraries(trajectory_sampler_node ${PROJECT_NAME})

cs_add_executable(time_evaluation_node
  src/time_evaluation_node.cpp
)
target_link_libraries(time_evaluation_node ${PROJECT_NAME})

cs_add_executable(static_traj_pub
  src/static_traj_pub.cpp
)
target_link_libraries(static_traj_pub ${PROJECT_NAME})

cs_add_executable(dynamic_traj_pub
  src/dynamic_traj_pub.cpp
)
target_link_libraries(dynamic_traj_pub ${PROJECT_NAME})

#########
# TESTS #
#########
catkin_add_gtest(test_feasibility
  test/test_feasibility.cpp
)
target_link_libraries(test_feasibility ${PROJECT_NAME} ${catkin_LIBRARIES})

##########
# EXPORT #
##########
cs_install()
cs_export()
