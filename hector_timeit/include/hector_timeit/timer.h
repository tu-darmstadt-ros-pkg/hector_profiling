//
// Created by Stefan Fabian on 14.06.18.
//

#ifndef HECTOR_TIMEIT_TIMER_H
#define HECTOR_TIMEIT_TIMER_H

#include "hector_timeit/macros.h"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#ifdef __unix__
#include <unistd.h>
#include <time.h>
#endif

namespace hector_timeit
{

/*!
 * Timer class that can be used for simple profiling.
 * The runtime of a single method can be measured using the static time method.
 * To measure multiple runs use a Timer instance and pass true to the reset method between runs.
 */
class Timer
{
public:
  enum TimeUnit
  {
    Default = 0,
    Seconds = 1,
    Milliseconds = 2,
    Microseconds = 3,
    Nanoseconds = 4
  };

  template<typename T>
  struct TimerResult
  {
    long time;
    long cpu_time;
    T result;

    std::string toString( const std::string &name )
    {
      return Timer::internalPrint( name, { time }, { cpu_time }, Timer::TimeUnit::Default );
    }
  };

  /*!
   * Constructs a new Timer instance.
   * @param name: The name of the timer. Used for printing in the toString method and stream operator.
   * @param print_time_unit The time unit used for printing. If Default the time unit is automatically chosen.
   */
  explicit Timer( std::string name, TimeUnit print_time_unit = Default );

  /*!
   * Starts the timer if it isn't already running.
   */
  inline void start()
  {
    if ( running_ ) return;
    running_ = true;
    start_ = std::chrono::high_resolution_clock::now();
    if ( cpu_time_valid_ )
    {
      cpu_time_valid_ = getCpuTime( cpu_start_ );
    }
  }

  /*!
   * Stops the timer if it isn't already stopped. Timing can be resumed using the start method.
   */
  inline void stop()
  {
    auto time_point = std::chrono::high_resolution_clock::now();
    long time;
    if ( cpu_time_valid_ )
    {
      cpu_time_valid_ = getCpuTime( time );
      if ( !running_ ) return;
      if ( cpu_time_valid_ )
      {
        elapsed_cpu_time_ += time - cpu_start_;
      }
    }
    if ( !running_ ) return;
    elapsed_time_ += internalGetDuration( start_, time_point );
    running_ = false;
  }

  /*!
   * Resets the timer. If new_run is false all runs are cleared as well.
   * If you want to time multiple runs pass true.
   * @param new_run Whether or not you want to time a new run. If false, everything is reset including the runs.
   */
  void reset( bool new_run = false );

  /*!
   * Returns the elapsed time since the timer or run was started excluding the time where it was paused using the stop
   *  method.
   * @return The elapsed time in nanoseconds.
   */
  inline long getElapsedTime() const
  {
    long result = elapsed_time_;
    if ( running_ )
      result += internalGetDuration( start_, std::chrono::high_resolution_clock::now());
    return result;
  }

  /*!
   * Returns the elapsed cpu or thread time (depending on what is available) since the timer or run was started
   *  excluding the time where it was paused using the stop method.
   * @return The elapsed cpu or thread time in nanoseconds.
   */
  inline long getElapsedCpuTime() const
  {
    if ( !cpu_time_valid_ ) return -1;
    long result = elapsed_cpu_time_;
    if ( running_ )
    {
      long time;
      if ( !getCpuTime( time )) return -1;
      result += time - cpu_start_;
    }
    return result;
  }

  std::vector<long> getRunTimes() const;

  std::vector<long> getCpuRunTimes() const;

  std::string toString() const;

  template<typename T>
  static std::unique_ptr<TimerResult<T>> time( const std::function<T( void )> &function );

protected:
  static inline bool getCpuTime( long &val )
  {
    // This could also maybe made a parameter
#ifdef _POSIX_THREAD_CPUTIME
    struct timespec spec;
    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &spec) == -1)
    {
      return false;
    }
    val = spec.tv_sec * 1000L * 1000L * 1000L + spec.tv_nsec;
#elif defined(_POSIX_CPUTIME)
    struct timespec spec;
  if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &spec) == -1)
  {
    return false;
  }
  val = spec.tv_sec * 1000L * 1000L * 1000L + spec.tv_nsec;
#else
  val = std::clock() * 1000L * 1000L * 1000L / CLOCKS_PER_SEC;
#endif
    return true;
  }

  static std::string internalPrint( const std::string &name, const std::vector<long> &run_times,
                                    const std::vector<long> &cpu_run_times, TimeUnit print_time_unit );

  static long internalGetDuration( const std::chrono::high_resolution_clock::time_point &start,
                                   const std::chrono::high_resolution_clock::time_point &end );

  std::string name_;
  bool running_;
  std::vector<long> run_times_;
  std::vector<long> cpu_run_times_;
  long elapsed_time_;
  long elapsed_cpu_time_;
  std::chrono::high_resolution_clock::time_point start_;
  long cpu_start_;
  TimeUnit print_time_unit_;
  bool cpu_time_valid_;
};

template<typename T>
std::unique_ptr<Timer::TimerResult<T>> Timer::time( const std::function<T( void )> &function )
{
  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
  long cpu_start;
  bool cpu_time_valid = Timer::getCpuTime( cpu_start );
  T function_result = function();
  long cpu_end;
  cpu_time_valid &= getCpuTime( cpu_end );
  long time = internalGetDuration( start, std::chrono::high_resolution_clock::now());
  std::unique_ptr<Timer::TimerResult<T> > result( new Timer::TimerResult<T>());
  result->time = time;
  result->cpu_time = cpu_time_valid ? cpu_end - cpu_start : -1;
  result->result = std::move( function_result );
  return result;
}

template <>
struct Timer::TimerResult<void>
{
  long time;
  long cpu_time;

  std::string toString( const std::string &name )
  {
    return Timer::internalPrint( name, { time }, { cpu_time }, Timer::TimeUnit::Default );
  }
};

template<>
std::unique_ptr<Timer::TimerResult<void>> Timer::time<void>( const std::function<void( void )> &function );
}

std::ostream &operator<<(std::ostream &stream, const hector_timeit::Timer &timer);

#endif //HECTOR_TIMEIT_TIMER_H
