#include "testsafequeue.h"
#include <memory>
#include <string>

using namespace std;
using namespace Rabit;

typedef std::unique_ptr<SafeQueue<int>> SafeIntQueuePtr;
typedef std::unique_ptr<SafeQueue<string>> SafeStringQueuePtr;

TEST_F(SafeQueueTest, SafeQueueIntEnqueueDequeueTest) {
  auto q = SafeIntQueuePtr(new SafeQueue<int>());

  q->enqueue(1);
  q->enqueue(2);
  q->enqueue(3);

  EXPECT_EQ(q->dequeue(), 1);
  EXPECT_EQ(q->dequeue(), 2);
  EXPECT_EQ(q->dequeue(), 3);
}

TEST_F(SafeQueueTest, SafeQueueIntSizeTest) {
  auto q = SafeIntQueuePtr(new SafeQueue<int>());

  q->enqueue(1);
  q->enqueue(2);
  q->enqueue(3);

  EXPECT_EQ(q->size(), 3);
}

TEST_F(SafeQueueTest, SafeQueueIntEmptyTest) {
  auto q = SafeIntQueuePtr(new SafeQueue<int>());

  EXPECT_EQ(q->empty(), true);
}

TEST_F(SafeQueueTest, SafeQueueStringEnqueueDequeueTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>());

  q->enqueue("1");
  q->enqueue("2");
  q->enqueue("3");

  EXPECT_EQ(q->dequeue(), "1");
  EXPECT_EQ(q->dequeue(), "2");
  EXPECT_EQ(q->dequeue(), "3");
}

TEST_F(SafeQueueTest, SafeQueueStringSizeTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>());

  q->enqueue("1");
  q->enqueue("2");
  q->enqueue("3");

  EXPECT_EQ(q->size(), 3);
}

TEST_F(SafeQueueTest, SafeQueueStringEmptyTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>());

  EXPECT_EQ(q->empty(), true);
}

TEST_F(SafeQueueTest, SafeQueueStringClearTest) {
  auto q = SafeStringQueuePtr(new SafeQueue<string>());

  q->enqueue("1");
  q->enqueue("2");
  q->enqueue("3");
  q->clear();

  EXPECT_EQ(q->empty(), true);
}
