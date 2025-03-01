file(GLOB SOURCES *.cpp)
add_library(mmf_queue ${SOURCES})
add_library(mmf_queue::mmf_queue ALIAS mmf_queue)
target_include_directories(mmf_queue PUBLIC ${PROJECT_SOURCE_DIR}/include)
