rock_library(modemdriver
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(modemdriver_bin Main.cpp
    DEPS modemdriver)

