//
// Created by Stefan Fabian on 14.06.18.
//

#ifndef HECTOR_TIMEIT_TIMER_H
#define HECTOR_TIMEIT_TIMER_H

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

  template<typename T>
  static std::unique_ptr<TimerResult<T>> time( const std::function<T( void )> &function );

  static inline bool getCpuTime( long &val )
  {
    // This could also maybe made a parameter
#ifdef _POSIX_THREAD_CPUTIME
    struct timespec spec;
    if ( clock_gettime( CLOCK_THREAD_CPUTIME_ID, &spec ) == -1 )
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

  /*!
   * Constructs a new Timer instance.
   * @param name: The name of the timer. Used for printing in the toString method and stream operator.
   * @param print_time_unit The time unit used for printing. If Default the time unit is automatically chosen.
   * @param autostart If true, the timer starts immediately after construction. If false, it has to be manually started
   *  using the start() method.
   */
  explicit Timer( std::string name, TimeUnit print_time_unit = Default, bool autostart = true );

  const std::string &name() const { return name_; }

  /*!
   * Starts the timer if it isn't already running.
   */
  inline void start()
  {
    if ( running_ ) return;
    running_ = true;
    /*
     * To get a more accurate measurement, the time it takes to measure the time is subtracted by using the following method:
     * We assume that each measurement takes roughly the same time
     * Time measurement is split into _I which is time call initialization (time until the time is measured) and _R
     *  which is time call return time (time until the measured time is returned)
     * WALL_START_A    XI XR
     * WALL_START_B    XI XR
     * CPU_START_A     YI YR
     * CPU_START_B     YI YR
     * RUN_TIME        R
     * CPU_END_B       YI YR
     * CPU_END_A       YI YR
     * WALL_END_B      XI XR
     * WALL_END_A      XI XR
     *
     * CPU A duration: YR + YI + YR + R + YI + YR + YI
     * CPU B duration:           YR + R + YI
     * Diff CPU = CPU A - CPU B = YR + YI + YR + YI = 2 * (YR + YI)
     * CPU Time = CPU B - 1/2 * Diff CPU = R
     *
     * Wall time A duration: XR + XI + XR + YI + YR + YI + YR + R + YI + YR + YI + YR + XI + XR + XI
     * Wall time B duration:           XR + YI + YR + YI + YR + R + YI + YR + YI + YR + XI
     * Diff Wall time = Wall time A - Wall time B = XR + XI + XR + XI = 2 * (XR + XI)
     * Wall time = Wall time B - 1/2 * Diff Wall time - 2 * Diff CPU = 1.5 * Wall time B - 0.5 * Wall time A - 2 * Diff CPU = R
     */
    start_a_ = std::chrono::high_resolution_clock::now();
    start_b_ = std::chrono::high_resolution_clock::now();
    if ( cpu_time_valid_a_ )
    {
      cpu_time_valid_a_ = getCpuTime( cpu_start_a_ );
      if ( cpu_time_valid_b_ )
      {
        cpu_time_valid_b_ = getCpuTime( cpu_start_b_ );
      }
    }
  }

  /*!
   * Stops the timer if it isn't already stopped. Timing can be resumed using the start method.
   */
  inline void stop()
  {
    if ( !running_ ) return;
    // See start method for a documentation of the algorithm used to get precise time measurements
    long time_a = 0;
    long time_b = 0;
    if ( cpu_time_valid_a_ )
    {
      if ( cpu_time_valid_b_ )
      {
        cpu_time_valid_b_ = getCpuTime( time_b );
      }
      cpu_time_valid_a_ = getCpuTime( time_a );
    }
    auto time_point_b = std::chrono::high_resolution_clock::now();
    auto time_point_a = std::chrono::high_resolution_clock::now();
    long cpu_diff = 0;
    if ( cpu_time_valid_a_ )
    {
      long elapsed;
      if ( cpu_time_valid_b_ )
      {
        cpu_diff = (time_a - cpu_start_a_) - (time_b - cpu_start_b_);
        elapsed = (time_b - cpu_start_b_) - cpu_diff / 2;
      }
      else
      {
        elapsed = time_a - cpu_start_a_;
      }
      if ( elapsed < 0 ) // TODO IF DEBUG
      {
        elapsed = 0;
      }
      elapsed_cpu_time_ += elapsed;
    }
    long wall_time_b = internalGetDuration( start_b_, time_point_b );
    long wall_time_a = internalGetDuration( start_a_, time_point_a );
    long elapsed = wall_time_b + wall_time_b / 2 - wall_time_a / 2 - 2 * cpu_diff;

    if ( elapsed < 0 ) // TODO IF DEBUG
    {
      elapsed = 0;
    }
    elapsed_time_ += elapsed;
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
   * Does not use any logic to account for the timing function call.
   * @return The elapsed time in nanoseconds.
   */
  inline long getElapsedTime() const
  {
    long result = elapsed_time_;
    if ( running_ )
      result += internalGetDuration( start_b_, std::chrono::high_resolution_clock::now());
    return result;
  }

  /*!
   * Returns the elapsed cpu or thread time (depending on what is available) since the timer or run was started
   *  excluding the time where it was paused using the stop method.
   * Does not use any logic to account for the timing function call.
   * @return The elapsed cpu or thread time in nanoseconds.
   */
  inline long getElapsedCpuTime() const
  {
    if ( !cpu_time_valid_a_ ) return -1;
    long result = elapsed_cpu_time_;
    if ( running_ )
    {
      long time;
      if ( !getCpuTime( time )) return -1;
      result += time - cpu_start_a_;
    }
    return result;
  }

  std::vector<long> getRunTimes() const;

  std::vector<long> getCpuRunTimes() const;

  std::string toString() const;

protected:

  static std::string internalPrint( const std::string &name, const std::vector<long> &run_times,
                                    const std::vector<long> &cpu_run_times, TimeUnit print_time_unit );

  static inline long internalGetDuration( const std::chrono::high_resolution_clock::time_point &start,
                                          const std::chrono::high_resolution_clock::time_point &end )
  {
    return std::chrono::duration_cast<std::chrono::nanoseconds>( end - start ).count();
  }

  std::vector<long> run_times_;
  std::vector<long> cpu_run_times_;
  std::string name_;
  TimeUnit print_time_unit_;
  std::chrono::high_resolution_clock::time_point start_a_;
  std::chrono::high_resolution_clock::time_point start_b_;
  long elapsed_time_;
  long elapsed_cpu_time_;
  long cpu_start_a_;
  long cpu_start_b_;
  bool running_;
  bool cpu_time_valid_a_;
  bool cpu_time_valid_b_;
};

template<typename T>
std::unique_ptr<Timer::TimerResult<T>> Timer::time( const std::function<T( void )> &function )
{
  Timer timer( "anonymous" );
  T function_result = function();
  timer.stop();
  std::unique_ptr<Timer::TimerResult<T> > result(
    new Timer::TimerResult<T>{ .time = timer.getElapsedTime(), .cpu_time = timer.getElapsedCpuTime(), .result = function_result } );
  return result;
}

template<>
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

#include "hector_timeit/macros.h"

#endif //HECTOR_TIMEIT_TIMER_H
