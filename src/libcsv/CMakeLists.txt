include_directories(${CMAKE_SOURCE_DIR}/src/libcoopy_core/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(coopy_csv csv.h libcsv.c CsvRead.cpp)
target_link_libraries(coopy_csv coopy_core)
set_target_properties(coopy_csv PROPERTIES LINKER_LANGUAGE CXX)
export(TARGETS coopy_csv APPEND FILE ${COOPY_DEPENDENCIES})
install(TARGETS coopy_csv COMPONENT ${BASELIB} ${DESTINATION_LIB})
