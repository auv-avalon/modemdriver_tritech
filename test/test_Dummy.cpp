#include <boost/test/unit_test.hpp>
#include <modemdriver/Dummy.hpp>

using namespace modemdriver;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    modemdriver::DummyClass dummy;
    dummy.welcome();
}
