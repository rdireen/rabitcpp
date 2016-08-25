/**
 * Both RabitMessages and PublishSubscribeMessages are tested here because
 * of the circular referencing.
 */

#include <iostream>
#include "testrabitmessage.h"
#include <memory>
#include "GlobalPublishSubscribeException.h"
#include "PublishSubscribeMessage.h"
#include "ManagerStatusMessage.h"
#include <string>
#include <typeindex>


using namespace std;
using namespace Rabit;

/*****************************************************************************
 *                       RabitMessage Tests
 * **************************************************************************/

TEST_F(RabitMessageTest, CompareTimeTrue)
{

    auto msg = MessageA();
    auto msg2 = msg.Clone();

    auto isSame = msg.CompareTime(msg2.get());

    EXPECT_EQ(isSame, true);
}

TEST_F(RabitMessageTest, CompareTimeFalse)
{

    auto msg = MessageA();
    msg.SetTimeNow();
    auto msg2 = MessageA();
    msg2.SetTimeNow();

    auto isSame = msg.CompareTime(&msg2);

    EXPECT_EQ(isSame, false);
}

TEST_F(RabitMessageTest, CompareTypeIndex)
{

    auto msg = MessageA();
    auto msg2 = MessageA();
    auto msg3 = MessageB();

    EXPECT_EQ(msg.GetTypeIndex(), type_index(typeid(MessageA)));
    EXPECT_EQ(msg3.GetTypeIndex(), type_index(typeid(MessageB)));
    EXPECT_EQ(msg2.GetTypeIndex(), msg.GetTypeIndex());

    EXPECT_NE(msg.GetTypeIndex(), msg3.GetTypeIndex());
}

TEST_F(RabitMessageTest, CopyMessage)
{

    auto msg = unique_ptr<RabitMessage>(new MessageA);
    static_cast<MessageA *>(msg.get())->a = 5;
    static_cast<MessageA *>(msg.get())->b = 7;

    auto msg2 = MessageA();

    EXPECT_EQ(msg2.CopyMessage(msg.get()), true);

    EXPECT_EQ(msg2.a, static_cast<MessageA *>(msg.get())->a);
    EXPECT_EQ(msg2.b, static_cast<MessageA *>(msg.get())->b);
}

TEST_F(RabitMessageTest, CopyMessage2)
{

    auto msg = unique_ptr<RabitMessage>(new MessageA);
    static_cast<MessageA *>(msg.get())->a = 5;
    static_cast<MessageA *>(msg.get())->b = 7;

    auto msg3 = MessageB();
    // These arn't the same message so it wont get copied
    bool test = msg3.CopyMessage(msg.get());
    EXPECT_EQ(test, false);
}

TEST_F(RabitMessageTest, GlobalPSHasNoReferenceException)
{

    auto msg = unique_ptr<RabitMessage>(new MessageA);

    bool caught = false;
    try
    {
        msg->PostMessage();
    } catch (GlobalPublishSubscribeException &e)
    {
        caught = true;
    }

    EXPECT_EQ(caught, true);

    caught = false;
    try
    {
        msg->FetchMessage();
    } catch (GlobalPublishSubscribeException &e)
    {
        caught = true;
    }

    EXPECT_EQ(caught, true);

    auto msg2 = unique_ptr<RabitMessage>(new MessageA);
    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    msg->SetGlobalPublishSubscribeMessageRef(psm);

    caught = false;
    try
    {
        msg->PostMessage();
    } catch (GlobalPublishSubscribeException &e)
    {
        caught = true;
    }

    EXPECT_EQ(caught, false);

    caught = false;
    try
    {
        msg->FetchMessage();
    } catch (GlobalPublishSubscribeException &e)
    {
        caught = true;
    }

    EXPECT_EQ(caught, false);

}

/*****************************************************************************
 *                       Publish Subscribe tests
 * **************************************************************************/

TEST(PublishSubscribeMessageTest, MsgTypeIndex)
{

    auto msg2 = unique_ptr<RabitMessage>(new MessageA);
    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    EXPECT_EQ(psm->MsgTypeIndex(), type_index(typeid(MessageA)));
    EXPECT_NE(psm->MsgTypeIndex(), type_index(typeid(MessageB)));
}

TEST(PublishSubscribeMessageTest, GetCopyOfMessage)
{

    auto msg2 = unique_ptr<RabitMessage>(new MessageA);
    auto msgc = msg2->Clone();
    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    auto up = psm->GetCopyOfMessage();

    EXPECT_EQ(static_cast<MessageA *>(msgc.get())->a, static_cast<MessageA *>(up.get())->a);
    EXPECT_EQ(static_cast<MessageA *>(msgc.get())->b, static_cast<MessageA *>(up.get())->b);
}

TEST(PublishSubscribeMessageTest, PostMessage)
{

    auto msg2 = unique_ptr<RabitMessage>(new Rabit::ManagerStatusMessage);
    msg2->SetTimeNow();
    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    Rabit::ManagerStatusMessage msgc;
    msgc.ManagerName = "TestMgr";
    msgc.ErrorCode = 7;
    msgc.ManagerStats.AveSleepTime_Sec = 13.75;
    msgc.ManagerStats.NumberOfExecuteCycles = 58;
    msgc.ManagerStats.NumberOfWakeUpCallsWhileAsleep = 9;
    msgc.ManagerStats.NumberOfWakeUpCallsWhileAwake = 27;

    psm->PostMessage(&msgc);

    Rabit::ManagerStatusMessage * psmMsgAptr = static_cast<Rabit::ManagerStatusMessage *>(psm->GetTestMessage().get());
    EXPECT_EQ(msgc.ErrorCode, psmMsgAptr->ErrorCode);
    EXPECT_EQ(msgc.ManagerStats.NumberOfExecuteCycles, psmMsgAptr->ManagerStats.NumberOfExecuteCycles);

    auto isSame = msgc.CompareTime(psmMsgAptr);
    EXPECT_EQ(isSame, true);
}

TEST(PublishSubscribeMessageTest, FetchMessage)
{

    auto msg2 = unique_ptr<RabitMessage>(new MessageA);
    msg2->SetTimeNow();
    auto msgc = msg2->Clone();
    msgc->SetTimeNow();
    static_cast<MessageA *>(msg2.get())->a = 7;
    static_cast<MessageA *>(msg2.get())->b = 9;

    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    auto fetchedNew = psm->FetchMessage(msgc.get());

    EXPECT_EQ(fetchedNew, true);

    EXPECT_EQ(static_cast<MessageA *>(msgc.get())->a, static_cast<MessageA *>(psm->GetTestMessage().get())->a);
    EXPECT_EQ(static_cast<MessageA *>(msgc.get())->b, static_cast<MessageA *>(psm->GetTestMessage().get())->b);

    auto isSame = static_cast<MessageA *>(msgc.get())->CompareTime(psm->GetTestMessage().get());
    EXPECT_EQ(isSame, true);
}

TEST(PublishSubscribeMessageTest, DoesntFetchMessage)
{

    auto msg2 = unique_ptr<RabitMessage>(new MessageA);
    msg2->SetTimeNow();
    auto msgc = msg2->Clone();

    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    auto fetchedNew = psm->FetchMessage(msgc.get());

    EXPECT_EQ(fetchedNew, false);
}

TEST(PublishSubscribeMessageTest, ForceFetchMessage)
{

    auto msg2 = unique_ptr<RabitMessage>(new MessageA);
    msg2->SetTimeNow();
    auto msgc = msg2->Clone();
    msgc->SetTimeNow();
    static_cast<MessageA *>(msg2.get())->a = 7;
    static_cast<MessageA *>(msg2.get())->b = 9;

    auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

    psm->ForceFetchMessage(msgc.get());

    EXPECT_EQ(static_cast<MessageA *>(msgc.get())->a, static_cast<MessageA *>(psm->GetTestMessage().get())->a);
    EXPECT_EQ(static_cast<MessageA *>(msgc.get())->b, static_cast<MessageA *>(psm->GetTestMessage().get())->b);

    auto isSame = static_cast<MessageA *>(msgc.get())->CompareTime(psm->GetTestMessage().get());
    EXPECT_EQ(isSame, true);
}


