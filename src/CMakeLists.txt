rock_library(modemdriver_tritech
    SOURCES AckDriver.cpp Driver.cpp ModemParser.cpp DriverInterface.cpp
    HEADERS AckDriver.hpp Driver.hpp ModemParser.hpp ModemDriverTypes.hpp DriverInterface.hpp
    DEPS_PKGCONFIG base-types iodrivers_base)


rock_executable(modemdriver_bin Main.cpp
    DEPS modemdriver_tritech)

rock_executable(receiver_bin Receiver.cpp
    DEPS modemdriver_tritech)

rock_executable(sender_bin Sender.cpp
    DEPS modemdriver_tritech)

