rock_library(modemdriver
    SOURCES AckDriver.cpp Driver.cpp ModemParser.cpp
    HEADERS AckDriver.hpp Driver.hpp ModemParser.hpp ModemDriverTypes.hpp DriverInterface.hpp
    DEPS_PKGCONFIG base-types iodrivers_base)


rock_executable(modemdriver_bin Main.cpp
    DEPS modemdriver)

