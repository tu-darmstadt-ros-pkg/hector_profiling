//
// Created by Stefan Fabian on 06.04.19.
//

#include <gtest/gtest.h>

#include "hector_timeit/timer.h"

using namespace hector_timeit;

template <class T>
class Counter
{
private:
  static int count;
public:
  Counter()
  {
    count++;
  }
  Counter(const Counter &c)
  {
    count++;
  }
  ~Counter()
  {
    count--;
  }
  static int GetCount() {

    return count;
  }
};

template<class T>
int Counter<T>::count = 0;



class CreationCounter : private Counter<CreationCounter>
{
public:
  using Counter<CreationCounter>::GetCount;
};

CreationCounter waitAndCreate(long duration)
{
  usleep(duration);
  return CreationCounter();
}

long extractTime(const std::string &string)
{
  // TODO Extract time from string output
}

TEST(TimerResult, NoCopies)
{
  EXPECT_EQ(1, Timer::time(std::function<CreationCounter(void)>([]() { return CreationCounter(); }))->result.GetCount());
}

TEST(Macros, TimeAndReturn)
{
  std::stringstream stream;
  EXPECT_EQ(1, HECTOR_TIME_AND_RETURN(CreationCounter, waitAndCreate(10000), "Name", stream).GetCount());
  EXPECT_EQ(1, HECTOR_TIME_AND_RETURN(const CreationCounter &, waitAndCreate(1000), "Name", stream).GetCount());
}

int main( int argc, char **argv )
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
