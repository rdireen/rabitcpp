#include "testsafequeue.h"
#include <memory>
#include <string>

using namespace std;
using namespace Rabit;

typedef std::unique_ptr<SafeQueue<int>> SafeIntQueuePtr;
typedef std::unique_ptr<SafeQueue<string>> SafeStringQueuePtr;

TEST_F(SafeQueueTest, SafeQueueIntEnqueueDequeueTest) {
  auto q = SafeIntQueuePtr(new SafeQueue<int>(10));

  q->enqueue(1);
  q->enqueue(2);
  q->enqueue(3);

  int val;
  bool tst = q->dequeue(val);
  EXPECT_EQ(tst, true);
  EXPECT_EQ(val, 1);

  tst = q->dequeue(val);
  EXPECT_EQ(tst, true);
  EXPECT_EQ(val, 2);

  tst = q->dequeue(val);
  EXPECT_EQ(tst, true);
  EXPECT_EQ(val, 3);

  tst = q->dequeue(val);
  EXPECT_EQ(tst, false);
}

TEST_F(SafeQueueTest, SafeQueueIntSizeTest) {
  auto q = SafeIntQueuePtr(new SafeQueue<int>(10));

  q->enqueue(1);
  q->enqueue(2);
  q->enqueue(3);

  EXPECT_EQ(q->size(), 3);
}

TEST_F(SafeQueueTest, SafeQueueIntEmptyTest) {
  auto q = SafeIntQueuePtr(new SafeQueue<int>(10));

  EXPECT_EQ(q->empty(), true);
}

TEST_F(SafeQueueTest, SafeQueueStringEnqueueDequeueTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>(10));

  q->enqueue("1");
  q->enqueue("2");
  q->enqueue("3");

  string val;
  bool tst;

  tst = q->dequeue(val);
  EXPECT_EQ(tst, true);
  EXPECT_EQ(val, "1");

  tst = q->dequeue(val);
  EXPECT_EQ(tst, true);
  EXPECT_EQ(val, "2");

  tst = q->dequeue(val);
  EXPECT_EQ(tst, true);
  EXPECT_EQ(val, "3");

  tst = q->dequeue(val);
  EXPECT_EQ(tst, false);
}

TEST_F(SafeQueueTest, SafeQueueStringSizeTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>(10));

  q->enqueue("1");
  q->enqueue("2");
  q->enqueue("3");

  EXPECT_EQ(q->size(), 3);
}

TEST_F(SafeQueueTest, SafeQueueStringEmptyTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>(10));

  EXPECT_EQ(q->empty(), true);
}

TEST_F(SafeQueueTest, SafeQueueStringClearTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>(10));

  q->enqueue("1");
  q->enqueue("2");
  q->enqueue("3");
  q->clear();

  EXPECT_EQ(q->empty(), true);
}
