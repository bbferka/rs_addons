cmake_minimum_required(VERSION 2.8.3)
project(rs_addons)


find_package(simtrack_nodes QUIET)
if(simtrack_nodes_FOUND)
    message(STATUS "OPTIONAL MODULE simtrack FOUND")
    set(OPTIONAL_simtrack simtrack_nodes)
endif(simtrack_nodes_FOUND)

find_package(Caffe QUIET)
find_package(aruco QUIET)

if(Caffe_FOUND)
    message(STATUS "OPTIONAL MODULE Caffe FOUND")
    set(OPTIONAL_Caffe Caffe)
    add_definitions( -DCAFFE_FOUND ${Caffe_DEFINITIONS})
    include_directories(${Caffe_INCLUDE_DIRS})
endif(Caffe_FOUND)

find_package(catkin REQUIRED robosherlock
    message_generation
    message_runtime
    ${OPTIONAL_simtrack}
    tf_conversions
    )

################################################################################
## Constants for project                                                      ##
################################################################################
set(NAMESPACE rs_addons)
set(TYPESYSTEM_CPP_PATH ${PROJECT_SOURCE_DIR}/include/rs_addons/types)
set(TYPESYSTEM_XML_PATH ${PROJECT_SOURCE_DIR}/descriptors/typesystem)
set(ANNOTATOR_PATH      ${PROJECT_SOURCE_DIR}/descriptors/annotators)
set(ENGINE_PATH         ${PROJECT_SOURCE_DIR}/descriptors/analysis_engines)
set(CMAKE_MODULE_PATH   ${PROJECT_SOURCE_DIR}/cmake)
################################################################################
## Configure project configuration files                                      ##
################################################################################
configure_file(${RS_PROJECT_CONFIG} ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}_config.cmake @ONLY)
################################################################################
## Find all include directories                                               ##
################################################################################
find_include_dirs(RS_INCLUDE_DIRS_LIST)

catkin_package(
    INCLUDE_DIRS ${RS_INCLUDE_DIRS_LIST}
    LIBRARIES ${RS_LIBS}
    CATKIN_DEPENDS robosherlock
    CFG_EXTRAS ${PROJECT_NAME}_config.cmake
    )

include_directories(
    ${RS_INCLUDE_DIRS_LIST}
    ${catkin_INCLUDE_DIRS}
    )

################################################################################
## Update analysis engines, typesystem and include all relevant files         ##
################################################################################
##Update xml list of annotators inside analysis engines
update_analysis_engines(robosherlock)
## generate classes from the typesystem xml files
generate_type_system(robosherlock)
#find all relevant files
find_additional_files()
################################################################################
## Sub Projects                                                               ##
################################################################################
#If you want to divide your projects into subprojects include the subdirectories
#each containing a CMakeLists.txt here
#add_subdirectory(src/xxx)

if(simtrack_nodes_FOUND)
    rs_add_library(rs_simtrack src/SimtrackDetection.cpp)
    target_link_libraries(rs_simtrack ${catkin_LIBRARIES})
endif(simtrack_nodes_FOUND)

if(aruco_FOUND)
    message(STATUS "OPTIONAL MODULE aruco FOUND")
    add_definitions( -DARUCO_FOUND)
    message(STATUS "Building AR Markers Detector")
    include_directories(${aruco_INCLUDE_DIRS})
    rs_add_library(rs_arMarkersDetector src/ARMarkersDetector.cpp)
    target_link_libraries(rs_arMarkersDetector rs_core ${aruco_LIBS})
else(aruco_FOUND)
    message(WARNING "Aruco not found. AR Markers Detector will NOT be built")
endif(aruco_FOUND)

if(Caffe_FOUND)
   ## add_executable(featureExtractor src/ext/FeatureExtractor.cpp)
   ## target_link_libraries(featureExtractor
   ##     ${PCL_LIBRARIES}
   ##     ${catkin_LIBRARIES}
   ##     ${OpenCV_LIBRARIES}
   ##     ${Boost_LIBRARIES}
   ##     ${Caffe_LIBRARIES}
   ##     )
    rs_add_library(rs_caffeClassification src/CaffeClassificationAnnotator.cpp)
    target_link_libraries(rs_caffeClassification
        ${catkin_LIBRARIES}
        ${Caffe_LIBRARIES}
        )
    rs_add_library(rs_deCafClassifier src/DeCafClassifier.cpp)
    target_link_libraries(rs_deCafClassifier
        ${catkin_LIBRARIES}
        ${PCL_LIBRARIES}
        ${Caffe_LIBRARIES}
        )
endif(Caffe_FOUND)

rs_add_library(rs_svmAnnotator 
    src/SvmAnnotator.cpp
    src/RSClassifier.cpp
    src/RSSVM.cpp
    )
target_link_libraries(rs_svmAnnotator 
    ${CATKIN_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    )
rs_add_library(rs_rfAnnotator 
    src/RfAnnotator.cpp
    src/RSClassifier.cpp
    src/RSRF.cpp
    )
target_link_libraries(rs_rfAnnotator 
    ${CATKIN_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    )
rs_add_library(rs_knnAnnotator 
    src/KnnAnnotator.cpp
    src/RSClassifier.cpp
    src/RSKNN.cpp
    )
target_link_libraries(rs_knnAnnotator 
    ${CATKIN_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    )
rs_add_library(rs_gbAnnotator 
    src/GbAnnotator.cpp
    src/RSClassifier.cpp
    src/RSGBT.cpp
    )
target_link_libraries(rs_gbAnnotator 
    ${CATKIN_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    )
rs_add_library(rs_trainerAnnotator 
    src/TrainerAnnotator.cpp
    src/RSClassifier.cpp
    src/RSRF.cpp
    src/RSSVM.cpp
    src/RSGBT.cpp
    src/RSKNN.cpp
    )
target_link_libraries(rs_trainerAnnotator 
    ${CATKIN_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    )
rs_add_library(rs_classifyOfflineAnnotator 
    src/ClassifyOfflineAnnotator.cpp
    src/RSClassifier.cpp
    src/RSRF.cpp
    src/RSSVM.cpp
    src/RSGBT.cpp
    src/RSKNN.cpp
    )
target_link_libraries(rs_classifyOfflineAnnotator 
    ${CATKIN_LIBRARIES}
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PCL_LIBRARIES}
    )
