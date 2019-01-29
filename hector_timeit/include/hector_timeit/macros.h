//
// Created by Stefan Fabian on 17.01.19.
//

#ifndef HECTOR_TIMEIT_MACROS_H
#define HECTOR_TIMEIT_MACROS_H

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
stream << hector_timeit::Timer::time(std::function<void(void)>([&] () { code; }))->toString( name ) << std::endl

#define _HECTOR_TIME_CONSOLE_ANONYMOUS(code) _HECTOR_TIME(code, HECTOR_TIMEIT_ANONYMOUS_NAME, std::cout)
#define _HECTOR_TIME_CONSOLE(code, name) _HECTOR_TIME(code, name, std::cout)
#define _HECTOR_TIME_GET_MACRO(_1, _2, _3, name, ...) name
#define HECTOR_TIME(...) \
_HECTOR_TIME_GET_MACRO(__VA_ARGS__, _HECTOR_TIME, _HECTOR_TIME_CONSOLE, _HECTOR_TIME_CONSOLE_ANONYMOUS)(__VA_ARGS__)

#define _HECTOR_TIME_ROS(code, name, level) \
ROS_##level("%s", hector_timeit::Timer::time(std::function<void(void)>([&] () { code; }))->toString(name).c_str())

#define _HECTOR_TIME_ROS_INFO(code, name) _HECTOR_TIME_ROS(code, name, INFO)
#define _HECTOR_TIME_ROS_INFO_LINE(code) _HECTOR_TIME_ROS(code, HECTOR_TIMEIT_ANONYMOUS_NAME, INFO)
#define _HECTOR_TIME_ROS_GET_MACRO(_1, _2, _3, name, ...) name
#define HECTOR_TIME_ROS(...) \
_HECTOR_TIME_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_ROS, _HECTOR_TIME_ROS_INFO, _HECTOR_TIME_ROS_INFO_LINE)(__VA_ARGS__)


/* ******************************************************************** */
/* *********** Time N times for console and ros definitions *********** */
/* ******************************************************************** */
#define _HECTOR_TIMEN(code, count, name, stream) \
do {\
hector_timeit::Timer hector_timeit_timer( name, hector_timeit::Timer::Default, false );\
bool used_break = false;\
for ( long i = 0; i < count; ++i ) \
{\
used_break = true;\
hector_timeit_timer.start();\
code;\
hector_timeit_timer.stop();\
used_break = false;\
hector_timeit_timer.reset( true );\
}\
if (used_break) hector_timeit_timer.stop();\
stream << hector_timeit_timer.toString() << std::endl;\
} while (false)

#define _HECTOR_TIMEN_CONSOLE_ANONYMOUS(code, count) _HECTOR_TIMEN(code, count, HECTOR_TIMEIT_ANONYMOUS_NAME, std::cout)
#define _HECTOR_TIMEN_CONSOLE(code, count, name) _HECTOR_TIMEN(code, count, name, std::cout)
#define _HECTOR_TIMEN_GET_MACRO(_1, _2, _3, _4, name, ...) name
#define HECTOR_TIMEN(...) \
_HECTOR_TIMEN_GET_MACRO(__VA_ARGS__, _HECTOR_TIMEN, _HECTOR_TIMEN_CONSOLE, _HECTOR_TIMEN_CONSOLE_ANONYMOUS)(__VA_ARGS__)

#define _HECTOR_TIMEN_ROS(code, count, name, level) \
do {\
hector_timeit::Timer hector_timeit_timer( name, hector_timeit::Timer::Default, false );\
bool used_break = false;\
for ( long i = 0; i < count; ++i ) \
{\
used_break = true;\
hector_timeit_timer.start();\
code;\
hector_timeit_timer.stop();\
used_break = false;\
hector_timeit_timer.reset( true );\
}\
if (used_break) hector_timeit_timer.stop();\
ROS_##level("%s", hector_timeit_timer.toString().c_str());\
} while (false)

#define _HECTOR_TIMEN_ROS_INFO(code, count, name) _HECTOR_TIMEN_ROS(code, count,name, INFO)
#define _HECTOR_TIMEN_ROS_INFO_LINE(code, count) _HECTOR_TIMEN_ROS(code, count, HECTOR_TIMEIT_ANONYMOUS_NAME, INFO)
#define _HECTOR_TIMEN_ROS_GET_MACRO(_1, _2, _3, _4, name, ...) name
#define HECTOR_TIMEN_ROS(...) \
_HECTOR_TIMEN_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIMEN_ROS, _HECTOR_TIMEN_ROS_INFO, _HECTOR_TIMEN_ROS_INFO_LINE)(__VA_ARGS__)

/* ******************************************************************** */
/* ********* Time and return for console and ros definitions ********** */
/* ******************************************************************** */
#define _HECTOR_TIME_AND_RETURN(type, code, name, stream) ([&] () {\
auto timer_result = hector_timeit::Timer::time(std::function<type(void)>([&] () { return code; }));\
stream << timer_result->toString( name ) << std::endl;\
return std::move( timer_result->result );\
})()

#define _HECTOR_TIME_AND_RETURN_CONSOLE_ANONYMOUS(type, code) _HECTOR_TIME_AND_RETURN(type, code, HECTOR_TIMEIT_ANONYMOUS_NAME, std::cout)
#define _HECTOR_TIME_AND_RETURN_CONSOLE(type, code, name) _HECTOR_TIME_AND_RETURN(type, code, name, std::cout)
#define _HECTOR_TIME_AND_RETURN_GET_MACRO(_1, _2, _3, _4, name, ...) name
#define HECTOR_TIME_AND_RETURN(...) \
_HECTOR_TIME_AND_RETURN_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_AND_RETURN, _HECTOR_TIME_AND_RETURN_CONSOLE, _HECTOR_TIME_AND_RETURN_CONSOLE_ANONYMOUS)(__VA_ARGS__)

#define _HECTOR_TIME_AND_RETURN_ROS(type, code, name, level) ([&] () {\
auto timer_result = hector_timeit::Timer::time(std::function<type(void)>([&] () { return code; }));\
ROS_##level( "%s", timer_result->toString( name ).c_str());\
return std::move( timer_result->result );\
})()
#define _HECTOR_TIME_AND_RETURN_ROS_INFO(type, code, name) _HECTOR_TIME_AND_RETURN_ROS(type, code, name, INFO)
#define _HECTOR_TIME_AND_RETURN_ROS_INFO_LINE(type, code) _HECTOR_TIME_AND_RETURN_ROS(type, code, HECTOR_TIMEIT_ANONYMOUS_NAME, INFO)
#define _HECTOR_TIME_AND_RETURN_ROS_GET_MACRO(_1, _2, _3, _4, name, ...) name
#define HECTOR_TIME_AND_RETURN_ROS(...) \
_HECTOR_TIME_AND_RETURN_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_AND_RETURN_ROS, _HECTOR_TIME_AND_RETURN_ROS_INFO, _HECTOR_TIME_AND_RETURN_ROS_INFO_LINE)(__VA_ARGS__)

/* ******************************************************************** */
/* ************************ Hector time section *********************** */
/* ******************************************************************** */ // TODO
#define HECTOR_TIME_SECTION(sectionname) \
  hector_timeit::Timer __hector_timeit_timer_##sectionname(#sectionname);

#define HECTOR_TIME_SECTION_PAUSE(sectionname) \
  __hector_timeit_timer_##sectionname.stop()

#define HECTOR_TIME_SECTION_RESUME(sectionname) \
  __hector_timeit_timer_##sectionname.start()

#define HECTOR_TIME_SECTION_END(sectionname) HECTOR_TIME_SECTION_PAUSE(sectionname)

#define HECTOR_TIME_SECTION_END_RUN(sectionname) \
  HECTOR_TIME_SECTION_PAUSE(sectionname);\
  __hector_timeit_timer_##sectionname.reset( true )\

#define HECTOR_TIME_SECTION_NEW_RUN(sectionname) \
  __hector_timeit_timer_##sectionname.reset( true );\
  HECTOR_TIME_SECTION_RESUME(sectionname)

#define _HECTOR_TIME_SECTION_PRINT(sectionname, stream) \
  __hector_timeit_timer_##sectionname.stop();\
  stream << __hector_timeit_timer_##sectionname << std::endl;\
  __hector_timeit_timer_##sectionname.start()
#define _HECTOR_TIME_SECTION_PRINT_CONSOLE(sectionname) _HECTOR_TIME_SECTION_PRINT(sectionname, std::cout)
#define _HECTOR_TIME_SECTION_PRINT_GET_MACRO(_1, _2, name, ...) name
#define HECTOR_TIME_SECTION_PRINT(...) \
_HECTOR_TIME_SECTION_PRINT_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_PRINT, _HECTOR_TIME_SECTION_PRINT_CONSOLE)(__VA_ARGS__)

#define _HECTOR_TIME_SECTION_END_AND_PRINT(sectionname, stream) \
  HECTOR_TIME_SECTION_END(sectionname);\
  _HECTOR_TIME_SECTION_PRINT(sectionname, stream)
#define _HECTOR_TIME_SECTION_END_AND_PRINT_CONSOLE(sectionname) _HECTOR_TIME_SECTION_PRINT(sectionname, std::cout)
#define _HECTOR_TIME_SECTION_END_AND_PRINT_GET_MACRO(_1, _2, name, ...) name
#define HECTOR_TIME_SECTION_END_AND_PRINT(...) \
_HECTOR_TIME_SECTION_END_AND_PRINT_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_END_AND_PRINT, _HECTOR_TIME_SECTION_END_AND_PRINT_CONSOLE)(__VA_ARGS__)


#define _HECTOR_TIME_SECTION_PRINT_FUNCTION(rosplaceholder, level, type, sectionname) \
  __hector_timeit_timer_##sectionname.stop();\
  rosplaceholder##level##type(__hector_timeit_timer_##sectionname);\
  __hector_timeit_timer_##sectionname.start()
#define _HECTOR_TIME_SECTION_PRINT_ROS(sectionname, level) _HECTOR_TIME_SECTION_PRINT_FUNCTION(ROS_, level, _STREAM, sectionname)
#define _HECTOR_TIME_SECTION_PRINT_ROS_INFO(sectionname) _HECTOR_TIME_SECTION_PRINT_ROS(sectionname, INFO)
#define _HECTOR_TIME_SECTION_PRINT_ROS_GET_MACRO(_1, _2, name, ...) name
#define HECTOR_TIME_SECTION_PRINT_ROS(...) \
_HECTOR_TIME_SECTION_PRINT_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_PRINT_ROS, _HECTOR_TIME_SECTION_PRINT_ROS_INFO)(__VA_ARGS__)

#define _HECTOR_TIME_SECTION_END_AND_PRINT_ROS(sectionname, level) \
  HECTOR_TIME_SECTION_END(sectionname);\
  _HECTOR_TIME_SECTION_PRINT_ROS(sectionname, level)
#define _HECTOR_TIME_SECTION_END_AND_PRINT_ROS_INFO(sectionname) _HECTOR_TIME_SECTION_END_AND_PRINT_ROS(sectionname, INFO)
#define _HECTOR_TIME_SECTION_END_AND_PRINT_ROS_GET_MACRO(_1, _2, name, ...) name
#define HECTOR_TIME_SECTION_END_AND_PRINT_ROS(...) \
_HECTOR_TIME_SECTION_END_AND_PRINT_ROS_GET_MACRO(__VA_ARGS__, _HECTOR_TIME_SECTION_END_AND_PRINT_ROS, _HECTOR_TIME_SECTION_END_AND_PRINT_ROS_INFO)(__VA_ARGS__)



#endif //HECTOR_TIMEIT_MACROS_H
