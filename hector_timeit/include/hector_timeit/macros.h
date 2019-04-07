//
// Created by Stefan Fabian on 17.01.19.
//

#ifndef HECTOR_TIMEIT_MACROS_H
#define HECTOR_TIMEIT_MACROS_H

#include "hector_timeit/timer.h"

/* ******************************************************************** */
/* ********************* Default name definitions ********************* */
/* ******************************************************************** */
#define _HECTOR_TIMEIT_ANONYMOUS_NAME_STR(name, line) \
std::string( "anonymous timer at " ) + ([] () {\
  std::string file_name = name;\
  std::size_t pos = file_name.find_last_of("/\\");\
  if (pos == std::string::npos) return file_name;\
  return file_name.substr(pos+1);\
})() + std::string( ":"#line )
#define _HECTOR_TIMEIT_ANONYMOUS_NAME_STR_EXPAND(name, line) _HECTOR_TIMEIT_ANONYMOUS_NAME_STR(name, line)
#define HECTOR_TIMEIT_ANONYMOUS_NAME _HECTOR_TIMEIT_ANONYMOUS_NAME_STR_EXPAND(__FILE__, __LINE__)


/* ******************************************************************** */
/* *************** Time for console and ros definitions *************** */
/* ******************************************************************** */
#define _HECTOR_TIME(code, name, stream) \
stream << ::hector_timeit::Timer::time(std::function<void(void)>([&] () { code; }))->toString( name ) << std::endl

#define _HECTOR_TIME_CONSOLE_ANONYMOUS(code) _HECTOR_TIME(code, HECTOR_TIMEIT_ANONYMOUS_NAME, std::cout)
#define _HECTOR_TIME_CONSOLE(code, name) _HECTOR_TIME(code, name, std::cout)
#define _HECTOR_TIME_GET_MACRO(_1, _2, _3, name, ...) name
/*!
 * @define HECTOR_TIME
 * @brief Times the execution of the given code and outputs the result to the given stream.
 *
 * @b Usage: HECTOR_TIME(Code[, Name[, Stream]])
 *
 * @b Example @b 1: HECTOR_TIME(someFunction());
 *
 * @b Output: [Timer: anonymous timer at main.cpp:23] 1 run(s) took: 31.772us (Thread: 30.035us).
 *
 * @b Example @b 2: HECTOR_TIME(someFunction(), "SampleTimer", std::cerr);
 *
 * @b Output (on standard error stream): [Timer: SampleTimer] 1 run(s) took: 31.772us (Thread: 30.035us).
 *
 * @param Code The code that is timed, e.g., a function call. Can be multiple commands separated by semicolons.
 * @param Name (Optional) The name of the timer for the output string. @b Default: Generated using filename and line number
 * @param Stream (Optional) The stream to which the output is streamed. @b Default: std::cout
 */
#define HECTOR_TIME(...) \
_HECTOR_TIME_GET_MACRO(__VA_ARGS__, _HECTOR_TIME, _HECTOR_TIME_CONSOLE, _HECTOR_TIME_CONSOLE_ANONYMOUS)(__VA_ARGS__)

#define _HECTOR_TIME_ROS(code, name, level) \
ROS_##level("%s", ::hector_timeit::Timer::time(std::function<void(void)>([&] () { code; }))->toString( name ).c_str())

#define _HECTOR_TIME_ROS_INFO(code, name) _HECTOR_TIME_ROS(code, name, INFO)
#define _HECTOR_TIME_ROS_INFO_LINE(code) _HECTOR_TIME_ROS(code, HECTOR_TIMEIT_ANONYMOUS_NAME, INFO)
#define _HECTOR_TIME_ROS_GET_MACRO(_1, _2, _3, name, ...) name
/*!
 * @define HECTOR_TIME_ROS
 * @brief Times the execution of the given code and outputs the result to ROS..
 *
 * @b Usage: HECTOR_TIME_ROS(Code[, Name[, Level]])
 *
 * @b Example @b 1: HECTOR_TIME_ROS(someFunction());
 *
 * @b Output: [ INFO] [1554646062.174895641]: [Timer: anonymous timer at main.cpp:23] 1 run(s) took: 31.772us (Thread: 30.035us).
 *
 * @b Example @b 2: HECTOR_TIME_ROS(someFunction(), "SampleTimer", WARN);
 *
 * @b Output: [ WARN] [1554646062.174895641]: [Timer: SampleTimer] 1 run(s) took: 31.772us (Thread: 30.035us).
 *
 * @param Code The code that is timed, e.g., a function call. Can be multiple commands separated by semicolons.
 * @param Name (Optional) The name of the timer for the output string. @b Default: Generated using filename and line number
 * @param Level (Optional) The level of the output which can be one of the following: DEBUG, INFO, WARN, ERROR. @b Default: INFO
 */
#define HECTOR_TIME_ROS(...) \
_HECTOR_TIME_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_ROS, _HECTOR_TIME_ROS_INFO, _HECTOR_TIME_ROS_INFO_LINE)(__VA_ARGS__)


/* ******************************************************************** */
/* *********** Time N times for console and ros definitions *********** */
/* ******************************************************************** */
#define _HECTOR_TIMEN(code, count, timer_name, stream) \
do {\
::hector_timeit::Timer hector_timeit_timer_4SFD78SFA8( timer_name, ::hector_timeit::Timer::Default, false );\
bool used_break_4SFD78SFA8 = false;\
for ( long i = 0; i < count; ++i ) \
{\
used_break_4SFD78SFA8 = true;\
hector_timeit_timer_4SFD78SFA8.start();\
code;\
hector_timeit_timer_4SFD78SFA8.stop();\
used_break_4SFD78SFA8 = false;\
hector_timeit_timer_4SFD78SFA8.reset( true );\
}\
if (used_break_4SFD78SFA8)\
{\
  hector_timeit_timer_4SFD78SFA8.stop();\
  stream << "Timer '" << hector_timeit_timer_4SFD78SFA8.name() << "' was interrupted! Timings might be wrong!" << std::endl;\
}\
stream << hector_timeit_timer_4SFD78SFA8.toString() << std::endl;\
} while (false)

#define _HECTOR_TIMEN_CONSOLE_ANONYMOUS(code, count) _HECTOR_TIMEN(code, count, HECTOR_TIMEIT_ANONYMOUS_NAME, std::cout)
#define _HECTOR_TIMEN_CONSOLE(code, count, name) _HECTOR_TIMEN(code, count, name, std::cout)
#define _HECTOR_TIMEN_GET_MACRO(_1, _2, _3, _4, name, ...) name
/*!
 * @define HECTOR_TIMEN
 * @brief Times the execution of the given code N times and outputs the results to the given stream.
 *
 * @b Usage: HECTOR_TIMEN(Code, Count[, Name[, Stream]])
 *
 * @b Example @b 1: HECTOR_TIMEN(someFunction(), 1000);
 *
 * @b Output:
 * @code
 * [Timer: anonymous timer at heightmap_node.cpp:102] 1000 run(s) took:
 * Type                  Average                     Longest         Shortest          Sum
 * Real            20.799us +- 76.991us             1795.266us       14.418us        20.799ms
 * Thread          17.396us +- 3913.282ns             41.471us        14.412us        17.396ms
 * @endcode
 *
 * @param Code The code that is timed, e.g., a function call. Can be multiple commands separated by semicolons. Can't have side effects since it is executed multiple times.
 * @param Count How many times the code should be executed.
 * @param Name (Optional) The name of the timer for the output string. @b Default: Generated using filename and line number
 * @param Stream (Optional) The stream to which the output is streamed. @b Default: std::cout
 */
#define HECTOR_TIMEN(...) \
_HECTOR_TIMEN_GET_MACRO(__VA_ARGS__, _HECTOR_TIMEN, _HECTOR_TIMEN_CONSOLE, _HECTOR_TIMEN_CONSOLE_ANONYMOUS)(__VA_ARGS__)

#define _HECTOR_TIMEN_ROS(code, count, timer_name, level) \
do {\
::hector_timeit::Timer hector_timeit_timer_4SFD78SFA8( timer_name, ::hector_timeit::Timer::Default, false );\
bool used_break_4SFD78SFA8;\
for ( long i = 0; i < count; ++i ) \
{\
used_break_4SFD78SFA8 = true;\
hector_timeit_timer_4SFD78SFA8.start();\
code;\
hector_timeit_timer_4SFD78SFA8.stop();\
used_break_4SFD78SFA8 = false;\
hector_timeit_timer_4SFD78SFA8.reset( true );\
}\
if (used_break_4SFD78SFA8)\
{\
  hector_timeit_timer_4SFD78SFA8.stop();\
  ROS_##level("Timer '%s' was interrupted! Timings might be wrong!", hector_timeit_timer_4SFD78SFA8.name().c_str());\
}\
ROS_##level("%s", hector_timeit_timer_4SFD78SFA8.toString().c_str());\
} while (false)

#define _HECTOR_TIMEN_ROS_INFO(code, count, name) _HECTOR_TIMEN_ROS(code, count, name, INFO)
#define _HECTOR_TIMEN_ROS_INFO_LINE(code, count) _HECTOR_TIMEN_ROS(code, count, HECTOR_TIMEIT_ANONYMOUS_NAME, INFO)
#define _HECTOR_TIMEN_ROS_GET_MACRO(_1, _2, _3, _4, name, ...) name
/*!
 * @define HECTOR_TIMEN_ROS
 * @brief Times the execution of the given code N times and outputs the results to ROS.
 *
 * @b Usage: HECTOR_TIMEN_ROS(Code, Count[, Name[, Level]])
 *
 * @b Example @b 1: HECTOR_TIMEN_ROS(someFunction(), 1000, "SampleTimer", WARN);
 *
 * @b Output:
 * @code
 * [ WARN] [1554646062.197184621]: [Timer: SampleTimer] 1000 run(s) took:
 * Type                  Average                     Longest         Shortest          Sum
 * Real            20.799us +- 76.991us             1795.266us       14.418us        20.799ms
 * Thread          17.396us +- 3913.282ns             41.471us        14.412us        17.396ms
 * @endcode
 *
 * @param Code The code that is timed, e.g., a function call. Can be multiple commands separated by semicolons. Can't have side effects since it is executed multiple times.
 * @param Count How many times the code should be executed.
 * @param Name (Optional) The name of the timer for the output string. @b Default: Generated using filename and line number
 * @param Level (Optional) The level of the output which can be one of the following: DEBUG, INFO, WARN, ERROR. @b Default: INFO
 */
#define HECTOR_TIMEN_ROS(...) \
_HECTOR_TIMEN_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIMEN_ROS, _HECTOR_TIMEN_ROS_INFO, _HECTOR_TIMEN_ROS_INFO_LINE)(__VA_ARGS__)

/* ******************************************************************** */
/* ********* Time and return for console and ros definitions ********** */
/* ******************************************************************** */
// We need to define a helper struct to allow types that have to be wrapped by parenthesis because they contain commas,
// e.g., std::map<int, int>. See https://stackoverflow.com/questions/13842468/comma-in-c-c-macro
namespace hector_timeit
{
namespace macros
{
template<typename T>
struct argument_type;

template<typename T, typename U>
struct argument_type<T( U )>
{
  typedef U type;
};
}
}

#define _HECTOR_TIME_AND_RETURN(return_type, code, name, stream) ([&] () {\
auto timer_result = ::hector_timeit::Timer::time(std::function<::hector_timeit::macros::argument_type<void(return_type)>::type (void)>([&] () -> ::hector_timeit::macros::argument_type<void(return_type)>::type { return code; }));\
stream << timer_result->toString( name ) << std::endl;\
return timer_result->result;\
})()

#define _HECTOR_TIME_AND_RETURN_CONSOLE_ANONYMOUS(return_type, code) _HECTOR_TIME_AND_RETURN(return_type, code, HECTOR_TIMEIT_ANONYMOUS_NAME, std::cout)
#define _HECTOR_TIME_AND_RETURN_CONSOLE(return_type, code, name) _HECTOR_TIME_AND_RETURN(return_type, code, name, std::cout)
#define _HECTOR_TIME_AND_RETURN_GET_MACRO(_1, _2, _3, _4, name, ...) name
/*!
 * @define HECTOR_TIME_AND_RETURN
 * @brief Times the execution of the given code, outputs it to the given stream and returns the result.
 *
 * @b Usage: HECTOR_TIME_AND_RETURN((ReturnType), Code[, Name[, Stream]])
 *
 * @b Example @b 1: int result = HECTOR_TIME_AND_RETURN(int, 1337 + 42);
 *
 * @b Example @b 2: std::pair<int, int> result = HECTOR_TIME_AND_RETURN((std::pair&lt;int, int&gt), {1337, 42}, "SampleTimer", WARN);
 *
 * @param ReturnType The type of the value returned by the code, has to be encapsulated in parenthesis in certain cases,
 *      e.g., if it contains commas as in std::pair&lt;int, int&gt;
 * @param Code The code that is timed, e.g., a function call that returns something. Has to be a single statement.
 * @param Name (Optional) The name of the timer for the output string. @b Default: Generated using filename and line number
 * @param Stream (Optional) The stream to which the output is streamed. @b Default: std::cout
 */
#define HECTOR_TIME_AND_RETURN(...) \
_HECTOR_TIME_AND_RETURN_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_AND_RETURN, _HECTOR_TIME_AND_RETURN_CONSOLE, _HECTOR_TIME_AND_RETURN_CONSOLE_ANONYMOUS)(__VA_ARGS__)

#define _HECTOR_TIME_AND_RETURN_ROS(return_type, code, name, level) ([&] () {\
auto timer_result = ::hector_timeit::Timer::time(std::function<::hector_timeit::macros::argument_type<void(return_type)>::type (void)>([&] () -> ::hector_timeit::macros::argument_type<void(return_type)>::type  { return code; }));\
ROS_##level( "%s", timer_result->toString( name ).c_str());\
return timer_result->result;\
})()
#define _HECTOR_TIME_AND_RETURN_ROS_INFO(return_type, code, name) _HECTOR_TIME_AND_RETURN_ROS(return_type, code, name, INFO)
#define _HECTOR_TIME_AND_RETURN_ROS_INFO_LINE(return_type, code) _HECTOR_TIME_AND_RETURN_ROS(return_type, code, HECTOR_TIMEIT_ANONYMOUS_NAME, INFO)
#define _HECTOR_TIME_AND_RETURN_ROS_GET_MACRO(_1, _2, _3, _4, name, ...) name
/*!
 * @define HECTOR_TIME_AND_RETURN_ROS
 * @brief Times the execution of the given code, outputs it using ROS and returns the result.
 *
 * @b Usage: HECTOR_TIME_AND_RETURN_ROS((ReturnType), Code[, Name[, Level]])
 *
 * @b Example @b 1: int result = HECTOR_TIME_AND_RETURN_ROS(int, 1337 + 42);
 *
 * @b Example @b 2: std::pair<int, int> result = HECTOR_TIME_AND_RETURN_ROS((std::pair&lt;int, int&gt), {1337, 42}, "SampleTimer", WARN);
 *
 * @param ReturnType The type of the value returned by the code, has to be encapsulated in parenthesis in certain cases,
 *      e.g., if it contains commas as in std::pair&lt;int, int&gt;
 * @param Code The code that is timed, e.g., a function call that returns something Has to be a single statement
 * @param Name (Optional) The name of the timer for the output string. @b Default: Generated using filename and line number
 * @param Level (Optional) The level of the output which can be one of the following: DEBUG, INFO, WARN, ERROR. @b Default: INFO
 */
#define HECTOR_TIME_AND_RETURN_ROS(...) \
_HECTOR_TIME_AND_RETURN_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_AND_RETURN_ROS, _HECTOR_TIME_AND_RETURN_ROS_INFO, _HECTOR_TIME_AND_RETURN_ROS_INFO_LINE)(__VA_ARGS__)

/* ******************************************************************** */
/* ************************ Hector time section *********************** */
/* ******************************************************************** */
#define _HECTOR_TIME_SECTION(sectionname, autostart)\
::hector_timeit::Timer __hector_timeit_timer_##sectionname(#sectionname, ::hector_timeit::Timer::Default, autostart)
#define _HECTOR_TIME_SECTION_AUTOSTART(sectionname) _HECTOR_TIME_SECTION(sectionname, true)
#define _HECTOR_TIME_SECTION_GET_MACRO(_1, _2, name, ...) name
/*!
 * @define HECTOR_TIME_SECTION
 * @brief Creates a timer section with the given name.
 *
 * @b Usage: HECTOR_TIME_SECTION(Name, Autostart)
 *
 * @param Name The name of the timer. Used for printing the result.
 * @param Autostart (Optional) Pass true to immediately start the timer, pass false to start off as paused. @b Default: true.
 */
#define HECTOR_TIME_SECTION(...)\
_HECTOR_TIME_SECTION_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION, _HECTOR_TIME_SECTION_AUTOSTART)(__VA_ARGS__)

/*!
 * @define HECTOR_TIME_SECTION_PAUSE
 * @brief Pauses the timer for the given section.
 *
 * @b Usage: HECTOR_TIME_SECTION_PAUSE(Name)
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 */
#define HECTOR_TIME_SECTION_PAUSE(sectionname) \
  __hector_timeit_timer_##sectionname.stop()

/*!
 * @define HECTOR_TIME_SECTION_RESUME
 * @brief Resumes the timer for the given section.
 *
 * @b Usage: HECTOR_TIME_SECTION_RESUME(Name)
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 */
#define HECTOR_TIME_SECTION_RESUME(sectionname) \
  __hector_timeit_timer_##sectionname.start()

/*!
 * @define HECTOR_TIME_SECTION_END
 * @brief Pauses the timer for the given section.
 *
 * @b Usage: HECTOR_TIME_SECTION_END(Name)
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 */
#define HECTOR_TIME_SECTION_END(sectionname) HECTOR_TIME_SECTION_PAUSE(sectionname)

/*!
 * @define HECTOR_TIME_SECTION_END_RUN
 * @brief Ends one run of the section.
 *
 * @b Usage: HECTOR_TIME_SECTION_END_RUN(Name)
 *
 * Possible usage scenarios include the timing of the individual iterations of a loop:
 * @code
 * HECTOR_TIME_SECTION(ExampleSection, false);
 * for (int i = 0; i &lt; 10000; ++i)
 * {
 *   HECTOR_TIME_SECTION_NEW_RUN(ExampleSection);
 *   // Some code
 *   HECTOR_TIME_SECTION_END_RUN(ExampleSection);
 * }
 * HECTOR_TIME_SECTION_PRINT(ExampleSection);
 * @endcode
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 */
#define HECTOR_TIME_SECTION_END_RUN(sectionname) \
  HECTOR_TIME_SECTION_PAUSE(sectionname);\
  __hector_timeit_timer_##sectionname.reset( true )\

/*!
 * @define HECTOR_TIME_SECTION_NEW_RUN
 * @brief Ends one run of the section.
 *
 * @b Usage: HECTOR_TIME_SECTION_NEW_RUN(Name)
 *
 * Possible usage scenarios include the timing of the individual iterations of a loop:
 * @code
 * // It's important to pass false here for autostart, otherwise we'd have one extra run
 * // with a possibly wildly different timing.
 * HECTOR_TIME_SECTION(ExampleSection, false);
 * for (int i = 0; i &lt; 10000; ++i)
 * {
 *   HECTOR_TIME_SECTION_NEW_RUN(ExampleSection);
 *   // Some code
 *   HECTOR_TIME_SECTION_END_RUN(ExampleSection);
 * }
 * HECTOR_TIME_SECTION_PRINT(ExampleSection);
 * @endcode
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 */
#define HECTOR_TIME_SECTION_NEW_RUN(sectionname) \
  __hector_timeit_timer_##sectionname.reset( true );\
  HECTOR_TIME_SECTION_RESUME(sectionname)

#define _HECTOR_TIME_SECTION_PRINT(sectionname, stream) \
  __hector_timeit_timer_##sectionname.stop();\
  stream << __hector_timeit_timer_##sectionname << std::endl;\
  __hector_timeit_timer_##sectionname.start()
#define _HECTOR_TIME_SECTION_PRINT_CONSOLE(sectionname) _HECTOR_TIME_SECTION_PRINT(sectionname, std::cout)
#define _HECTOR_TIME_SECTION_PRINT_GET_MACRO(_1, _2, name, ...) name
/*!
 * @define HECTOR_TIME_SECTION_PRINT
 * @brief Prints the time measured by the timer for the current section to the given output stream.
 *
 * This should have little impact to the timings since the timer is paused while writing to the stream.
 * @b Usage: HECTOR_TIME_SECTION_PRINT(Name[, Stream]])
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 * @param Stream (Optional) The stream to which the output is streamed. @b Default: std::cout
 */
#define HECTOR_TIME_SECTION_PRINT(...) \
_HECTOR_TIME_SECTION_PRINT_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_PRINT, _HECTOR_TIME_SECTION_PRINT_CONSOLE)(__VA_ARGS__)

#define _HECTOR_TIME_SECTION_END_AND_PRINT(sectionname, stream) \
  __hector_timeit_timer_##sectionname.stop();\
  stream << __hector_timeit_timer_##sectionname << std::endl;
#define _HECTOR_TIME_SECTION_END_AND_PRINT_CONSOLE(sectionname) _HECTOR_TIME_SECTION_PRINT(sectionname, std::cout)
#define _HECTOR_TIME_SECTION_END_AND_PRINT_GET_MACRO(_1, _2, name, ...) name
/*!
 * @define HECTOR_TIME_SECTION_END_AND_PRINT
 * @brief Stops the timer and prints the time measured for the current section to the given output stream.
 *
 * @b Usage: HECTOR_TIME_SECTION_END_AND_PRINT(Name[, Stream]])
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 * @param Stream (Optional) The stream to which the output is streamed. @b Default: std::cout
 */
#define HECTOR_TIME_SECTION_END_AND_PRINT(...) \
_HECTOR_TIME_SECTION_END_AND_PRINT_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_END_AND_PRINT, _HECTOR_TIME_SECTION_END_AND_PRINT_CONSOLE)(__VA_ARGS__)


#define _HECTOR_TIME_SECTION_PRINT_ROS(sectionname, level) \
  __hector_timeit_timer_##sectionname.stop();\
  ROS_##level##_STREAM(__hector_timeit_timer_##sectionname);\
  __hector_timeit_timer_##sectionname.start()
#define _HECTOR_TIME_SECTION_PRINT_ROS_INFO(sectionname) _HECTOR_TIME_SECTION_PRINT_ROS(sectionname, INFO)
#define _HECTOR_TIME_SECTION_PRINT_ROS_GET_MACRO(_1, _2, name, ...) name
/*!
 * @define HECTOR_TIME_SECTION_PRINT_ROS
 * @brief Prints the time measured by the timer for the current section to ROS with the given output level.
 *
 * This should have little impact to the timings since the timer is paused while writing to ROS output.
 * @b Usage: HECTOR_TIME_SECTION_PRINT_ROS(Name[, Level]])
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 * @param Level (Optional) The level of the output which can be one of the following: DEBUG, INFO, WARN, ERROR. Default: INFO
 */
#define HECTOR_TIME_SECTION_PRINT_ROS(...) \
_HECTOR_TIME_SECTION_PRINT_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_PRINT_ROS, _HECTOR_TIME_SECTION_PRINT_ROS_INFO)(__VA_ARGS__)

#define _HECTOR_TIME_SECTION_END_AND_PRINT_ROS(sectionname, level) \
  __hector_timeit_timer_##sectionname.stop();\
  ROS_##level##_STREAM(__hector_timeit_timer_##sectionname);
#define _HECTOR_TIME_SECTION_END_AND_PRINT_ROS_INFO(sectionname) _HECTOR_TIME_SECTION_END_AND_PRINT_ROS(sectionname, INFO)
#define _HECTOR_TIME_SECTION_END_AND_PRINT_ROS_GET_MACRO(_1, _2, name, ...) name
/*!
 * @define HECTOR_TIME_SECTION_END_AND_PRINT_ROS
 * @brief Stops the timer and prints the time measured for the current section to ROS with the given output level.
 *
 * @b Usage: HECTOR_TIME_SECTION_END_AND_PRINT_ROS(Name[, Level]])
 *
 * @param Name The name of the section. Has to be a valid section that has been started with HECTOR_TIME_SECTION(Name).
 * @param Level (Optional) The level of the output which can be one of the following: DEBUG, INFO, WARN, ERROR. Default: INFO
 */
#define HECTOR_TIME_SECTION_END_AND_PRINT_ROS(...) \
_HECTOR_TIME_SECTION_END_AND_PRINT_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_END_AND_PRINT_ROS, _HECTOR_TIME_SECTION_END_AND_PRINT_ROS_INFO)(__VA_ARGS__)



#endif //HECTOR_TIMEIT_MACROS_H
