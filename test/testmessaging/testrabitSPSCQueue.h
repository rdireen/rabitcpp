#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "RabitNonBlockingSPSCQueue.h"
#include "ManagerStatusMessage.h"

// The fixture for testing class Rabit.
class TestRabitNonBlockingSPSCQueue : public ::testing::Test
{

protected:

    // You can do set-up work for each test here.
    TestRabitNonBlockingSPSCQueue(){}

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~TestRabitNonBlockingSPSCQueue(){}

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp(){}

    // Code here will be called immediately after each test (right
    // before the destructor).
    //virtual void TearDown();

};


