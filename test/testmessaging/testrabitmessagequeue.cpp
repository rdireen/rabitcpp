#include "testrabitmessagequeue.h"
#include <memory>
#include <string>

using namespace std;
using namespace Rabit;

typedef unique_ptr<RabitMessageQueue<int>> RabitIntQueuePtr;
typedef unique_ptr<RabitMessageQueue<string>> RabitStringQueuePtr;

TEST_F(RabitMessageQueueTest, GetNameTest)
{

    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    EXPECT_EQ(rq->GetMessageQueueName(), "rname");
}

TEST_F(RabitMessageQueueTest, GetMaxNumTest)
{

    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    EXPECT_EQ(rq->GetMaxNoMessagesAllowedInQueue(), 3);
}


TEST_F(RabitMessageQueueTest, AddMessageTest)
{

    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    EXPECT_EQ(rq->AddMessage(1), true);
    EXPECT_EQ(rq->AddMessage(2), true);

    EXPECT_EQ(rq->NoMessagesInQueue(), 2);
}

TEST_F(RabitMessageQueueTest, AddMessageOverFlowTest)
{

    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    EXPECT_EQ(rq->AddMessage(1), true);
    EXPECT_EQ(rq->AddMessage(2), true);
    EXPECT_EQ(rq->AddMessage(7), true);
    EXPECT_EQ(rq->AddMessage(9), false);

    EXPECT_EQ(rq->NoMessagesInQueue(), 3);
}

TEST_F(RabitMessageQueueTest, AddMessageNoEventTest)
{

    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    EXPECT_EQ(rq->AddMessageNoEventTrigger(1), true);
    EXPECT_EQ(rq->AddMessageNoEventTrigger(2), true);

    EXPECT_EQ(rq->NoMessagesInQueue(), 2);
}

TEST_F(RabitMessageQueueTest, ClearMessagesTest)
{

    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    rq->AddMessage(1);
    rq->AddMessage(2);
    rq->ClearMessageQueue();

    EXPECT_EQ(rq->NoMessagesInQueue(), 0);
}

TEST_F(RabitMessageQueueTest, GetMessageTest)
{

    int msg;
    auto rq = RabitIntQueuePtr(new RabitMessageQueue<int>(3, "rname"));

    rq->AddMessage(1);
    rq->AddMessage(2);

    rq->GetMessage(msg);
    EXPECT_EQ(msg, 1);

    rq->GetMessage(msg);
    EXPECT_EQ(msg, 2);
}

TEST_F(RabitMessageQueueTest, GetMessageStringTest)
{

    std::string msg;
    auto rq = RabitStringQueuePtr(new RabitMessageQueue<string>(3, "rname"));

    rq->AddMessage("1");
    rq->AddMessage("2");

    rq->GetMessage(msg);
    EXPECT_EQ(msg, "1");

    rq->GetMessage(msg);
    EXPECT_EQ(msg, "2");
}

