//
// Created by Stefan Fabian on 29.01.19.
//
#include <iostream>
#include <math.h>

#include "hector_timeit/timer.h"


int main(int argc, char **argv)
{
  std::cout << "Hector TimeIt demo v1.0" << std::endl << std::endl;

  {
    std::cout << "i++ vs ++i Test (10^9 iterations)" << std::endl;
    constexpr long iterations = 1000 * 1000 * 1000;
    HECTOR_TIME_SECTION( I_PLUS_PLUS );
    for ( long i = 0; i < iterations; i++ )
    {
      asm(""); // Scaring the optimizer
    }
    HECTOR_TIME_SECTION_END_AND_PRINT( I_PLUS_PLUS );

    HECTOR_TIME_SECTION( PLUS_PLUS_I );
    for ( long i = 0; i < iterations; ++i )
    {
      asm("");
    }
    HECTOR_TIME_SECTION_END_AND_PRINT( PLUS_PLUS_I );
  }

  {
    std::cout << std::endl << "Measuring accuracy of timing adjustment (Removal of execution time for get time calls) with 10^6 iterations."
              << std::endl;
    constexpr long iterations = 1000 * 1000;
    long double fibonacci_a = 1;
    long double fibonacci_b = 2;

    // Timer
    hector_timeit::Timer timer( "AccuracyTimer" );
    for ( long i = 0; i < iterations; ++i )
    {
      fibonacci_a = fibonacci_a * fibonacci_a + fibonacci_b * fibonacci_b;
      fibonacci_b = sqrtl(fibonacci_a + fibonacci_b);
      asm("");
      timer.stop();
      timer.start();
    }
    timer.stop();

    // Naive approach
    long elapsed = 0;
    fibonacci_a = 1;
    fibonacci_b = 2;

    auto start = std::chrono::high_resolution_clock::now();
    for ( long i = 0; i < iterations; ++i )
    {
      fibonacci_a = fibonacci_a * fibonacci_a + fibonacci_b * fibonacci_b;
      fibonacci_b = sqrtl(fibonacci_a + fibonacci_b);
      asm("");
      auto end = std::chrono::high_resolution_clock::now();
      elapsed += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
      start = std::chrono::high_resolution_clock::now();
    }

    // Reference
    fibonacci_a = 1;
    fibonacci_b = 2;
    start = std::chrono::high_resolution_clock::now();
    for ( long i = 0; i < iterations; ++i )
    {
      fibonacci_a = fibonacci_a * fibonacci_a + fibonacci_b * fibonacci_b;
      fibonacci_b = sqrtl(fibonacci_a + fibonacci_b);
      asm("");
    }
    auto end = std::chrono::high_resolution_clock::now();
    long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    long diff_timer = labs(duration - timer.getElapsedTime());
    long diff_naive = labs(duration - elapsed);
    std::cout << "Timer:  " << timer.getElapsedTime() << "ns" << std::endl;
    std::cout << "Naive:  " << elapsed << "ns" << std::endl;
    std::cout << "Chrono: " << duration << "ns" << std::endl;
    std::cout << "Difference Timer: " << diff_timer << " (" << fabs((diff_timer * 1.0 / duration)) * 100 << "%)" << std::endl;
    std::cout << "Difference Naive: " << diff_naive << " (" << fabs((diff_naive * 1.0 / duration)) * 100 << "%)" << std::endl;
    // Could be improved by using averages and accounting for stddev
  }
  return 0;
}
