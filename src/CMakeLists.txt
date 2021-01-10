#Activer le support c++11 
if(CMAKE_VERSION VERSION_LESS "3.1")
  set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
else ()
  set(CMAKE_CXX_STANDARD 11)
endif ()

#Librairie PTHREAD 
if(CMAKE_VERSION VERSION_GREATER "3.0")
  set(THREADS_PREFER_PTHREAD_FLAG ON)
endif ()
find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated") # various warning flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s -O2") # no debug, active optimisation

include_directories(../dependencies/FragFramework/src)
include_directories(../dependencies/SimpleFolder/src)
include_directories(../dependencies/SimpleIni/include)
include_directories(../dependencies/SimpleLog/src)
include_directories(../dependencies/ServiceSkeleton/src)
include_directories(../dependencies/MqttDaemon/src)
 
#Génération de la liste des fichiers sources
file(
  GLOB_RECURSE
  MqttDaemon_files
  ../dependencies/MqttDaemon/src/*
)

file(
  GLOB_RECURSE
  ServiceSkeleton_files
  ../dependencies/ServiceSkeleton/src/*
)

file(
  GLOB_RECURSE
  SimpleFolders_files
  ../dependencies/SimpleFolder/src/*
)

file(
  GLOB_RECURSE
  SimpleIni_files
  ../dependencies/SimpleIni/src/*
)

file(
  GLOB_RECURSE
  SimpleLog_files
  ../dependencies/SimpleLog/src/*
)

file(
  GLOB_RECURSE
  FragFramework_files
  ../dependencies/FragFramework/src/*
)

#Déclaration de l'exécutable
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
add_executable(
  MqttTeleInfo
  ${MqttDaemon_files}
  ${ServiceSkeleton_files}
  ${SimpleFolders_files}
  ${SimpleIni_files}
  ${SimpleLog_files}
  ${SimpleSock_files}
  ${FragFramework_files}
  SerialTeleInfo.cpp
  MqttTeleInfo.cpp
  main.cpp
)

target_link_libraries(MqttTeleInfo paho-mqtt3a)
target_link_libraries(MqttTeleInfo paho-mqttpp3)
target_link_libraries(MqttTeleInfo ${CMAKE_THREAD_LIBS_INIT})

#Spécification WIN32
if(WIN32)
  target_link_libraries(MqttTeleInfo ws2_32)
endif()
