AUX_SOURCE_DIRECTORY(./src SRC_LIST)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -g")
SET(EXECUTER_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
ADD_LIBRARY(executer SHARED ${SRC_LIST})

