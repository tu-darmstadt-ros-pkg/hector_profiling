//
// Created by Stefan Fabian on 14.06.18.
//

#include "hector_timeit/timer.h"

#include <sstream>
#include <iostream>
#include <cmath>


namespace hector_timeit
{

Timer::Timer( std::string name, TimeUnit print_time_unit, bool autostart, bool print_on_destruct )
  : name_( std::move( name )), print_time_unit_( print_time_unit ), print_on_destruct_( print_on_destruct )
{
  if ( autostart ) start();
}

Timer::~Timer()
{
  if ( print_on_destruct_ ) std::cout << *this << std::endl;
}

void Timer::reset( bool new_run )
{
  stop();
  if ( new_run )
  {
    if ( elapsed_time_ > 0 )
    {
      run_times_.push_back( elapsed_time_ );
      cpu_run_times_.push_back( cpu_time_valid_a_ ? elapsed_cpu_time_ : -1 );
    }
  }
  else
  {
    run_times_.clear();
    cpu_run_times_.clear();
  }
  elapsed_time_ = 0;
  elapsed_cpu_time_ = 0;
  cpu_time_valid_a_ = true;
  cpu_time_valid_b_ = true;
}

std::vector<long> Timer::getRunTimes() const
{
  std::vector<long> result = run_times_;
  long elapsed_time = getElapsedTime();
  if ( elapsed_time != 0 )
  {
    result.push_back( elapsed_time );
  }
  return result;
}


std::vector<long> Timer::getCpuRunTimes() const
{
  std::vector<long> result = cpu_run_times_;
  long elapsed_cpu_time = getElapsedCpuTime();
  if ( elapsed_cpu_time > 0 )
  {
    result.push_back( elapsed_cpu_time );
  }
  return result;
}

template<>
std::unique_ptr<Timer::TimerResult<void>> Timer::time<void>( const std::function<void( void )> &function )
{
  Timer timer( "anonymous" );
  function();
  timer.stop();
  std::unique_ptr<Timer::TimerResult<void> > result( new Timer::TimerResult<void>());
  result->time = timer.getElapsedTime();
  result->cpu_time = timer.getElapsedCpuTime();
  return result;
}

std::string Timer::toString() const
{
  return internalPrint( name_, getRunTimes(), getCpuRunTimes(), print_time_unit_ );
}

namespace
{
void printPaddedString( std::ostringstream &stream, const std::string &text, size_t pad = 0 )
{
  size_t i = 0;
  for ( ; pad != 0 && i < (pad - text.length()) / 2; ++i ) stream << " ";
  stream << text;
  for ( i += text.length(); i < pad; ++i ) stream << " ";
}

template<typename T>
void printTimeString( std::ostringstream &outstream, T time, Timer::TimeUnit print_time_unit, int pad = 0 )
{
  std::ostringstream stream;
  stream.precision( 3 );
  stream.setf( std::ios::fixed, std::ios::floatfield );
  switch ( print_time_unit )
  {
    case Timer::Seconds:
      stream << time / 1E9 << "s";
      break;
    case Timer::Milliseconds:
      stream << time / 1E6 << "ms";
      break;
    case Timer::Microseconds:
      stream << time / 1000.0 << "us";
      break;
    case Timer::Nanoseconds:
      stream << time << "ns";
      break;
    case Timer::Default:
    default:
      if ( time < 5000 )
      {
        stream << time << "ns";
      }
      else if ( time < 5E6 )
      {
        stream << time / 1E3 << "us";
      }
      else if ( time < 5E9 )
      {
        stream << time / 1E6 << "ms";
      }
      else
      {
        stream << time / 1E9 << "s";
      }
      break;
  }
  printPaddedString( outstream, stream.str(), pad );
}

double square( double x ) { return x * x; }

void printStats( std::ostringstream &stream, const std::vector<long> &run_times, Timer::TimeUnit print_time_unit )
{
  long max = 0;
  long min = INT64_MAX;
  long long sum = 0;
  size_t count = 0;
  for ( long time : run_times )
  {
    if ( time == -1 ) continue;
    sum += time;
    ++count;
    if ( time > max ) max = time;
    if ( time < min ) min = time;
  }
  double mean = (double) sum / count;
  double var = 0;
  for ( long time : run_times )
  {
    if ( time == -1 ) continue;
    var += square( time - mean );
  }
  var /= (count - 1);
  if ( count == 0 )
  {
    stream << "None of the runs had valid times!";
    return;
  }
  // Average
  std::ostringstream avg_stream;
  printTimeString( avg_stream, mean, print_time_unit, 0 );
  avg_stream << " +- ";
  printTimeString( avg_stream, sqrt( var ), print_time_unit, 0 );
  printPaddedString( stream, avg_stream.str(), 40 );
  // Longest
  printTimeString( stream, max, print_time_unit, 16 );
  // Shortest
  printTimeString( stream, min, print_time_unit, 16 );
  // Sum
  printTimeString( stream, sum, print_time_unit, 16 );
  if ( count != run_times.size())
  {
    stream << std::endl << "Warning: Only " << count << " of " << run_times.size() << " had valid times!";
  }
}
}

std::string Timer::internalPrint( const std::string &name, const std::vector<long> &run_times,
                                  const std::vector<long> &cpu_run_times, TimeUnit print_time_unit )
{
  std::ostringstream stringstream;
  stringstream << "[Timer: " << name << "] " << run_times.size() << " run(s) took: ";
  if ( run_times.empty())
  {
    stringstream << "no time at all.";
  }
  else if ( run_times.size() == 1 )
  {
    printTimeString( stringstream, run_times[0], print_time_unit, 0 );
    if ( cpu_run_times[0] != -1 )
    {
#ifdef _POSIX_THREAD_CPUTIME
      printPaddedString( stringstream, " (Thread: ", 0 );
#else
      printPaddedString( stringstream, " (CPU: ", 0 );
#endif
      printTimeString( stringstream, cpu_run_times[0], print_time_unit, 0 );
      stringstream << ")";
    }
    stringstream << ".";
  }
  else
  {
    stringstream << std::endl;
    printPaddedString( stringstream, "Type", 8 );
    printPaddedString( stringstream, "Mean (+/- stddev)", 40 );
    printPaddedString( stringstream, "Longest", 16 );
    printPaddedString( stringstream, "Shortest", 16 );
    printPaddedString( stringstream, "Sum", 16 );
    stringstream << std::endl;
    printPaddedString( stringstream, "Real", 8 );
    printStats( stringstream, run_times, print_time_unit );
    stringstream << std::endl;
#ifdef _POSIX_THREAD_CPUTIME
    printPaddedString( stringstream, "Thread", 8 );
#else
    printPaddedString(stringstream, "CPU", 8);
#endif
    printStats( stringstream, cpu_run_times, print_time_unit );
  }
  return stringstream.str();
}
}

std::ostream &operator<<( std::ostream &stream, const hector_timeit::Timer &timer )
{
  return stream << timer.toString();
}
