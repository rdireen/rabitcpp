/**
 * Both RabitMessages and PublishSubscribeMessages are tested here because
 * of the circular referencing.
 */

#include "testrabitmessage.h"
#include <memory>
#include "GlobalPublishSubscribeException.h"
#include "PublishSubscribeMessage.h"
#include <string>
#include <typeindex>



using namespace std;
using namespace Rabit;

/*****************************************************************************
 *                       RabitMessage Tests
 * **************************************************************************/

TEST_F(RabitMessageTest, CompareTimeTrue) {

  auto msg = MessageA("MessageA");
  auto msg2 = msg.Clone();

  auto isSame = msg.CompareTime(msg2.get());

  EXPECT_EQ(isSame, true);
}

TEST_F(RabitMessageTest, CompareTimeFalse) {

  auto msg = MessageA("MessageA");
  msg.SetTimeNow();
  auto msg2 = MessageA("MessageA");
  msg2.SetTimeNow();

  auto isSame = msg.CompareTime(&msg2);

  EXPECT_EQ(isSame, false);
}

TEST_F(RabitMessageTest, CompareTypeIndex) {

  auto msg = MessageA("MessageA");
  auto msg2 = MessageA("MessageA");
  auto msg3 = MessageB("MessageB");

  EXPECT_EQ(msg.GetTypeIndex(), type_index(typeid(MessageA)));
  EXPECT_EQ(msg3.GetTypeIndex(), type_index(typeid(MessageB)));
  EXPECT_EQ(msg2.GetTypeIndex(), msg.GetTypeIndex());

  EXPECT_NE(msg.GetTypeIndex(),msg3.GetTypeIndex());
}

TEST_F(RabitMessageTest, CopyMessage) {

  auto msg = unique_ptr<RabitMessage>(new MessageA("MessageA"));
  static_cast<MessageA*>(msg.get())->a = 5;
  static_cast<MessageA*>(msg.get())->b = 7;

  auto msg2 = MessageA("MessageA");

  EXPECT_EQ(msg2.CopyMessage(msg.get()), true);

  EXPECT_EQ(msg2.a, static_cast<MessageA*>(msg.get())->a);
  EXPECT_EQ(msg2.b, static_cast<MessageA*>(msg.get())->b);

  auto msg3 = MessageB("MessageB");
  // These arn't the same message so it wont get copied
  EXPECT_EQ(msg3.CopyMessage(msg.get()), false);
}

TEST_F(RabitMessageTest, GlobalPSHasNoReferenceException) {

  auto msg = unique_ptr<RabitMessage>(new MessageA("MessageA"));

  bool caught = false;
  try{
    msg->PostMessage();
  }catch(GlobalPublishSubscribeException& e){
    caught = true;
  }

  EXPECT_EQ(caught, true);

  caught = false;
  try{
    msg->FetchMessage();
  }catch(GlobalPublishSubscribeException& e){
    caught = true;
  }

  EXPECT_EQ(caught, true);

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  msg->GlobalPublishSubscribeMessageRef(psm);

  caught = false;
  try{
    msg->PostMessage();
  }catch(GlobalPublishSubscribeException& e){
    caught = true;
  }

  EXPECT_EQ(caught, false);

  caught = false;
  try{
    msg->FetchMessage();
  }catch(GlobalPublishSubscribeException& e){
    caught = true;
  }

  EXPECT_EQ(caught, false);

}

/*****************************************************************************
 *                       Publish Subscribe tests
 * **************************************************************************/

TEST(PublishSubscribeMessageTest,MsgTypeIndex){

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  EXPECT_EQ(psm->MsgTypeIndex(),type_index(typeid(MessageA)));
  EXPECT_NE(psm->MsgTypeIndex(),type_index(typeid(MessageB)));
}

TEST(PublishSubscribeMessageTest,GetCopyOfMessage){

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  auto msgc = msg2->Clone();
  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  auto up = psm->GetCopyOfMessage();

  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->a, static_cast<MessageA*>(up.get())->a);
  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->b, static_cast<MessageA*>(up.get())->b);
}

TEST(PublishSubscribeMessageTest,PostMessage){

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  msg2->SetTimeNow();
  auto msgc = msg2->Clone();
  static_cast<MessageA*>(msgc.get())->a = 7;
  static_cast<MessageA*>(msgc.get())->b = 9;
  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  psm->PostMessage(msgc.get());

  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->a, static_cast<MessageA*>(psm->GetTestMessage().get())->a);
  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->b, static_cast<MessageA*>(psm->GetTestMessage().get())->b);

  auto isSame = static_cast<MessageA*>(msgc.get())->CompareTime(psm->GetTestMessage().get());
  EXPECT_EQ(isSame, true);
}

TEST(PublishSubscribeMessageTest,FetchMessage){

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  msg2->SetTimeNow();
  auto msgc = msg2->Clone();
  msgc->SetTimeNow();
  static_cast<MessageA*>(msg2.get())->a = 7;
  static_cast<MessageA*>(msg2.get())->b = 9;

  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  auto fetchedNew = psm->FetchMessage(msgc.get());

  EXPECT_EQ(fetchedNew, true);

  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->a, static_cast<MessageA*>(psm->GetTestMessage().get())->a);
  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->b, static_cast<MessageA*>(psm->GetTestMessage().get())->b);

  auto isSame = static_cast<MessageA*>(msgc.get())->CompareTime(psm->GetTestMessage().get());
  EXPECT_EQ(isSame, true);
}

TEST(PublishSubscribeMessageTest,DoesntFetchMessage){

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  msg2->SetTimeNow();
  auto msgc = msg2->Clone();

  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  auto fetchedNew = psm->FetchMessage(msgc.get());

  EXPECT_EQ(fetchedNew, false);
}

TEST(PublishSubscribeMessageTest,ForceFetchMessage){

  auto msg2 = unique_ptr<RabitMessage>(new MessageA("MessageAStatus"));
  msg2->SetTimeNow();
  auto msgc = msg2->Clone();
  msgc->SetTimeNow();
  static_cast<MessageA*>(msg2.get())->a = 7;
  static_cast<MessageA*>(msg2.get())->b = 9;

  auto psm = make_shared<PublishSubscribeMessage>(std::move(msg2));

  psm->ForceFetchMessage(msgc.get());

  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->a, static_cast<MessageA*>(psm->GetTestMessage().get())->a);
  EXPECT_EQ(static_cast<MessageA*>(msgc.get())->b, static_cast<MessageA*>(psm->GetTestMessage().get())->b);

  auto isSame = static_cast<MessageA*>(msgc.get())->CompareTime(psm->GetTestMessage().get());
  EXPECT_EQ(isSame, true);
}


