rock_library(modemdriver
    SOURCES Driver.cpp ModemParser.cpp
    HEADERS Driver.hpp ModemParser.hpp ModemDriverTypes.hpp
    DEPS_PKGCONFIG base-types iodrivers_base)


rock_executable(modemdriver_bin Main.cpp
    DEPS modemdriver)

