## !! This repo is in beta. Bugs may happen !!
# Hector TimeIt
This library provides a simple interface to time the execution of code lines or sections of code.   
Code can be timed using the Timer class or a collection of convenience Macros.  

#### Dependencies
C++11  
(ROS) If you are using the ROS macros otherwise it shouldn't be required.

## Examples
### Using the class
Simply timing some code using the default settings: 
  
```cpp
hector_timeit::Timer timer("MyTimer");
/* code */
timer.stop();
std::cout << timer;
```
**Output:**
> ```[Timer: MyTimer] 1 run(s) took: 215.621ms (Thread: 200.301ms).```
 
Timing multiple runs of the same code, e.g., the average run time of the iterations of for loop.

```cpp
// Pass false to constructor for autostart parameter
hector_timeit::Timer timer("IterationTimer", hector_timeit::Timer::Default, false );
for (size_t i = 0; i < some_vector.size(); ++i)
{
  timer.start();
  /* code */
  timer.stop();
  // Optional parameter 'new_run' controls whether to record a new run (true) or reset the
  // timer (omit or pass false)
  timer.reset( true );
}
std::cout << timer;
```
**Output:**
>```
>  [Timer: IterationTimer] 7104 run(s) took:  
>     Type                  Average                     Longest         Shortest          Sum        
>     Real           1206.089us +- 60.192us            1639.461us      1085.497us        8.568s       
>    Thread           116.570us +- 57.334us             586.997us        28.864us       828.112ms  
>```

### Using the macros
####Timing the execution of code
```cpp
HECTOR_TIME(executeCode(), "ExecuteCodeTimer");
// Or using an anonymous timer
HECTOR_TIME(executeCode());
```
**Output:**
>```
>[Timer: ExecuteCodeTimer] 1 run(s) took: 213.905ms (Thread: 213.906ms).
>[Timer: anonymous timer at main.cpp:35] 1 run(s) took: 213.832ms (Thread: 213.829ms).
>```

####Profile code by timing it n times
```cpp
HECTOR_TIMEN(executeCode(), 50, "RepeatedExecutionTimer");
```
**Output:**
>```
>  [Timer: RepeatedExecutionTimer] 50 run(s) took:  
>     Type                  Average                     Longest         Shortest          Sum        
>     Real           204.189us +- 41.265us             300.134us      125.497us        11.568ms       
>    Thread          182.074us +- 48.142us             281.972us       78.681us        10.112ms  
>```

####Time and return
In some cases you may want to time the execution of a return statement:
```cpp
ReturnType sampleFunction()
{
  return someOtherFunctionCall();
}
```
Instead of having to introduce a temp variable, you can just do:
```cpp
return HECTOR_TIME_AND_RETURN(ReturnType, someOtherFunctionCall());
```
**Output:**
>```
>[Timer: anonymous timer at sample.cpp:23] 1 run(s) took: 48.739ms (Thread: 48.731ms).
>```

####Using time sections
```cpp
HECTOR_TIME_SECTION(IterationTimer);
for ( size_t i = 0; i < some_vector.size(); ++i )
{
  // Using runs is of course optional but this is a for loop, hence, it makes sense
  HECTOR_TIME_SECTION_NEW_RUN(IterationTimer);
  // Some code
  HECTOR_TIME_SECTION_PAUSE(IterationTimer);
  // Some unrelated code, we don't want to measure.
  HECTOR_TIME_SECTION_RESUME(IterationTimer);
  HECTOR_TIME_SECTION_END_RUN(IterationTimer);
}
HECTOR_TIME_SECTION_END_AND_PRINT(IterationTimer);
```
**Output:**
>```
>[Timer: IterationTimer] 7105 run(s) took: 
>   Type                  Average                     Longest         Shortest          Sum       
>   Real            30.730us +- 24.004us             244.821us        6.089us        218.338ms    
>  Thread           30.804us +- 24.064us             248.257us        6.110us        218.865ms
>```
## API
#### Timer
* `static std::unique_ptr<TimerResult<T>> time( const std::function<T( void )> &function )`
function: A function whose execution time is timed.  
returns A struct containing the result of the function (if it isn't void) and the elapsed real and cpu time.
* constructor `Timer(std::string name, Timer::TimeUnit print_time_unit = Timer::Default, bool autostart = true)`  
`name`: The name of the timer.  
`print_time_unit`: The time unit used for printing can be one of the following:
  * Nanoseconds
  * Microseconds
  * Milliseconds
  * Seconds
  * Default  
Default automatically determines the unit depending on the magnitude of the measured time value.  

  `autostart`: Whether or not to immediately start the timer.
* `void start()`  
Starts the timer if it isn't already running
* `void stop()`  
Stops the timer if it isn't already stopped. After it was stopped, timing can be resumed using `start()` again.
* `void reset( new_run )`  
Resets the timer. If new_run is false all runs are cleared as well.
If you want to time multiple runs pass true.
* `long getElapsedTime()`  
Returns the elapsed time in nanoseconds since the timer or run was started excluding the time where it was
 paused using the stop method.
* `long getElapsedCpuTime()`  
Returns the elapsed cpu or thread time (depending on what is available) in nanoseconds since the timer or run was
 started excluding the time where it was paused using the stop method.
 * `std::vector<long> getRunTimes()`  
Returns a vector containing the elapsed time for each run in nanoseconds.
* `std::vector<long> getCpuRunTimes()`  
Returns a vector containing the elapsed cpu or thread time (depending on what is available) for each run in nanoseconds.
* `std::string toString()`  
Prints the data contained in this Timer in a pleasantly readable format. Check the examples for examples.

#### Macros
* `HECTOR_TIME(code[, name[, stream]])`  
`code`: The code that is timed.  
`name`: The name of the timer.  
Default: Combination of filename and line number of macro call.   
`stream`: Output stream the resulting information string is streamed to.  
Default: std::cout  
Executes the given code and prints it to the given stream.

* `HECTOR_TIME_ROS(code[, name[, level]])`  
As above.  
`level`: The logging level which can be one of the following:
  * DEBUG
  * INFO (default)
  * WARN
  * ERROR

* `HECTOR_TIMEN(code, count[, name[, stream]])`  
Runs the given `code` `count` times. Rest as above.  

* `HECTOR_TIMEN_ROS(code, count[, name[, level]])` 

* `HECTOR_TIME_AND_RETURN(type, code[, name[, stream]])`  
`type`: The return type of the executed code.  
Times the execution of the given code and returns what the given code returned.

* `HECTOR_TIME_AND_RETURN_ROS(type, code[, name[, level]])`

##### Time section macros
* `HECTOR_TIME_SECTION(sectionname)`  
`sectionname`: Name of the section. Unlike the previous string attribute name this string property can not be quoted and
 can only contain characters that are acceptable in a variable name.  
Creates and starts a timer.

* `HECTOR_TIME_SECTION_PAUSE(sectionname)`  
Pauses the timer of the given section.

* `HECTOR_TIME_SECTION_RESUME(sectionname)`  
Resumes the timer of the given section.

* `HECTOR_TIME_SECTION_NEW_RUN(sectionname)`  
Starts a new timer run.

* `HECTOR_TIME_SECTION_END_RUN(sectionname)`  
Ends a timer run.

* `HECTOR_TIME_SECTION_PRINT(sectionname[, stream])`  
Prints the info contained in the section's timer.

* `HECTOR_TIME_SECTION_PRINT_ROS(sectionname[, level])`  
As above.

* `HECTOR_TIME_SECTION_END(sectionname)`  
Ends the time section. Not required. Timer doesn't require cleanup.

* `HECTOR_TIME_SECTION_END_AND_PRINT(sectionname[, stream])`  
Ends the time section and prints the info contained in the section's timer.

* `HECTOR_TIME_SECTION_END_AND_PRINT_ROS(sectionname[, level])`  
As above.

### TODOs
- [ ] Write tests
