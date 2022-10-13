cmake_minimum_required(VERSION 3.16)
project(DAG_analysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(DAG_analysis main.cpp include/DAGVertex.h include/DAGTask.h src/DAGTask.cpp src/DAGVertex.cpp include/utils.h src/algorithm/Han2019.cpp src/DAGTaskIO.cpp src/utils.cpp include/DAGAlgorithm.h)
