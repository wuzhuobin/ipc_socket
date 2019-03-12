find_package(Boost REQUIRED COMPONENTS date_time system regex)
add_executable(
    ${PROJECT_NAME}
    main.cpp
    server_singleton.cpp
    wave_spo2_session.cpp
    server_singleton_initialization.cpp
)
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${Boost_INCLUDE_DIRS}
)
if(UNIX)
  set(OTHER_LIBRARY rt pthread)
endif()
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    ${Boost_LIBRARIES}
    ${OTHER_LIBRARY}
)