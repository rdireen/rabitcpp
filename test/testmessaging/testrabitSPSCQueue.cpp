#include "testrabitSPSCQueue.h"
#include <memory>
#include <string>

using namespace std;
using namespace Rabit;

typedef unique_ptr <RabitNonBlockingSPSCQueue<int>> RabitIntQueuePtr;
typedef unique_ptr <RabitNonBlockingSPSCQueue<string>> RabitStringQueuePtr;
typedef unique_ptr <RabitNonBlockingSPSCQueue<std::shared_ptr<ManagerStatusMessage>>> RabitMgrStatsQueuePtr;

TEST_F(TestRabitNonBlockingSPSCQueue, GetNameTest)
{

    auto rq = RabitIntQueuePtr(new RabitNonBlockingSPSCQueue<int>(3, "rname"));

    EXPECT_EQ(rq->GetMessageQueueName(), "rname");
}

TEST_F(TestRabitNonBlockingSPSCQueue, GetMaxNumTest)
{

    auto rq = RabitIntQueuePtr(new RabitNonBlockingSPSCQueue<int>(3, "rname"));

    EXPECT_EQ(rq->GetMaxNoMessagesAllowedInQueue(), 3);
}



TEST_F(TestRabitNonBlockingSPSCQueue, AddMessageOverFlowTest)
{
    int val;
    bool tst;
    auto rq = RabitIntQueuePtr(new RabitNonBlockingSPSCQueue<int>(3, "rname"));

    EXPECT_EQ(rq->AddMessage(1), true);
    EXPECT_EQ(rq->AddMessage(2), true);
    EXPECT_EQ(rq->AddMessage(7), true);
    EXPECT_EQ(rq->AddMessage(9), false);

    tst = rq->GetMessage(val);
    EXPECT_EQ(tst, true);
    EXPECT_EQ(val, 1);
    EXPECT_EQ(rq->GetMessage(val), true);
    EXPECT_EQ(val, 2);
    EXPECT_EQ(rq->GetMessage(val), true);
    EXPECT_EQ(val, 7);

    tst = rq->GetMessage(val);
    EXPECT_EQ(tst, false);

}

TEST_F(TestRabitNonBlockingSPSCQueue, AddMessageNoEventTest)
{

    auto rq = RabitIntQueuePtr(new RabitNonBlockingSPSCQueue<int>(3, "rname"));

    EXPECT_EQ(rq->AddMessageNoEventTrigger(1), true);
    EXPECT_EQ(rq->AddMessageNoEventTrigger(2), true);
}

TEST_F(TestRabitNonBlockingSPSCQueue, ClearMessagesTest)
{
    int val;
    auto rq = RabitIntQueuePtr(new RabitNonBlockingSPSCQueue<int>(3, "rname"));

    rq->AddMessage(1);
    rq->AddMessage(2);
    rq->ClearMessageQueue();

    bool tst = rq->GetMessage(val);
    EXPECT_EQ(tst, false);
}

TEST_F(TestRabitNonBlockingSPSCQueue, GetMessageTest)
{
    int msg;
    auto rq = RabitIntQueuePtr(new RabitNonBlockingSPSCQueue<int>(3, "rname"));

    rq->AddMessage(1);
    rq->AddMessage(2);

    rq->GetMessage(msg);
    EXPECT_EQ(msg, 1);

    rq->GetMessage(msg);
    EXPECT_EQ(msg, 2);
}

TEST_F(TestRabitNonBlockingSPSCQueue, GetMessageStringTest)
{
    std::string msg;
    auto rq = RabitStringQueuePtr(new RabitNonBlockingSPSCQueue<string>(3, "rname"));

    rq->AddMessage("1");
    rq->AddMessage("2");

    rq->GetMessage(msg);
    EXPECT_EQ(msg, "1");

    rq->GetMessage(msg);
    EXPECT_EQ(msg, "2");
}

TEST_F(TestRabitNonBlockingSPSCQueue, SharedPointerMessageTest)
{
    std::shared_ptr<ManagerStatusMessage> mgrStatsMsg1_sptr = make_shared<ManagerStatusMessage>("MessageA");
    std::shared_ptr<ManagerStatusMessage> mgrStatsMsg2_sptr = make_shared<ManagerStatusMessage>("MessageB");
    std::shared_ptr<ManagerStatusMessage> mgrStatsMsg3_sptr;

    auto rq = RabitMgrStatsQueuePtr(new RabitNonBlockingSPSCQueue<shared_ptr<ManagerStatusMessage>>(10, "rname"));

    mgrStatsMsg1_sptr->ManagerName = "MgrA";
    mgrStatsMsg1_sptr->ErrorCode = 13;
    mgrStatsMsg1_sptr->ManagerStats.NumberOfExecuteCycles = 27;

    mgrStatsMsg2_sptr->ManagerName = "MgrB";
    mgrStatsMsg2_sptr->ErrorCode = 25;
    mgrStatsMsg2_sptr->ManagerStats.NumberOfExecuteCycles = 79;

    rq->AddMessage(mgrStatsMsg1_sptr);
    rq->AddMessage(mgrStatsMsg2_sptr);

    rq->GetMessage(mgrStatsMsg3_sptr);
    EXPECT_EQ(mgrStatsMsg1_sptr->ManagerName, mgrStatsMsg3_sptr->ManagerName);
    EXPECT_EQ(mgrStatsMsg1_sptr->ErrorCode, mgrStatsMsg3_sptr->ErrorCode);
    EXPECT_EQ(mgrStatsMsg1_sptr->ManagerStats.NumberOfExecuteCycles, mgrStatsMsg3_sptr->ManagerStats.NumberOfExecuteCycles);

    rq->GetMessage(mgrStatsMsg3_sptr);
    EXPECT_EQ(mgrStatsMsg2_sptr->ManagerName, mgrStatsMsg3_sptr->ManagerName);
    EXPECT_EQ(mgrStatsMsg2_sptr->ErrorCode, mgrStatsMsg3_sptr->ErrorCode);
    EXPECT_EQ(mgrStatsMsg2_sptr->ManagerStats.NumberOfExecuteCycles, mgrStatsMsg3_sptr->ManagerStats.NumberOfExecuteCycles);

}



