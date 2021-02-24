cmake_minimum_required(VERSION 3.7...3.19)

project(DccExCommandline) 


# Added for Windows
if(WIN32)
  if(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
  endif()
  set(BOOSTFLAG "-D_WIN32_WINNT=0x0601")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BOOSTFLAG}")
endif()


find_package(Boost)
find_package(fmt)
find_package(nlohmann_json_schema_validator)

# The source code is here
add_subdirectory(src)

