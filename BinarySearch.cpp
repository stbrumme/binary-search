// //////////////////////////////////////////////////////////
// BinarySearch.cpp
// Copyright (c) 2011 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//

#ifdef _MSC_VER
// unchecked iterators
#define _SECURE_SCL 0
#else
// need size_t
#include <cstddef>
// g++ -o BinarySearch.exe BinarySearch.cpp -std=c++0x -O3 -march=native -mtune=native
#endif

// //////////////////////////////////////////////////////////
// benchmark configuration:
/// data type
typedef double Number;
/// number of elements
const size_t numElements = 1000*1000;
/// hybrid version's threshold
const size_t threshold = 8;

// supported containers
#include <vector>
#include <list>
#include <deque>

/// current container to be tested
std::vector<Number> sorted;
// //////////////////////////////////////////////////////////

// create a gap between elements to allow searching for non-existing elements at arbitrary positions
/// gap's size
const Number step        = 10;
/// somewhere in the middle of the gap ...
const Number notFound    = step/2;
const Number maxElement  = step * numElements;

// STL's binary_search
#include <algorithm>

#include <iostream>
#include <cassert>


namespace my
{
  /// same algorithm as STL
  template <typename Iterator, typename Value>
  bool binary_search(Iterator begin, Iterator end, Value value)
  {
    // narrow down range of candidates
    auto numElements = std::distance(begin, end);

    // repeat until a single element is left
    while (numElements > 0)
    {
      // pivot element
      auto middlePos = numElements >> 1;
      auto middle    = begin;
      std::advance(middle, middlePos);

      if (*middle < value)
      {
        // value must be on the right side of pivot element
        begin = middle;
        ++begin;
        numElements = numElements - middlePos - 1;
      }
      else
      {
        // value must be on the left  side of pivot element (or it is the pivot element)
        numElements = middlePos;
      }
    }

    // out of bounds ?
    if (begin == end)
      return false;

    // hit ?
    return value == *begin;
  }


  /// compare for equality each time, not just at the end
  template <typename Iterator, typename Value>
  bool binary_search_with_equality_test(Iterator begin, Iterator end, Value value)
  {
    // narrow down range of candidates
    auto numElements = std::distance(begin, end);

    // repeat until a single element is left
    while (numElements > 0)
    {
      // pivot element
      auto middlePos = numElements >> 1;
      auto middle    = begin;
      std::advance(middle, middlePos);

      // early exit ?
      if (value == *middle)
        return true;

      if (*middle < value)
      {
        // value must be on the right side of pivot element
        begin = middle;
        ++begin;
        numElements = numElements - middlePos - 1;
      }
      else
      {
        // value must be on the left  side of pivot element (or it is the pivot element)
        numElements = middlePos;
      }
    }

    // not found
    return false;
  }


  /// recursive implementation
  template <typename Iterator, typename Value>
  bool binary_search_recursive(Iterator begin, Iterator end, Value value)
  {
    // empty ?
    if (begin == end)
      return false;

    // stop recursion ?
    auto numElements = std::distance(begin, end);
    if (numElements == 1)
      return value == *begin;

    // pivot element
    auto middlePos = numElements >> 1;
    auto middle    = begin;
    std::advance(middle, middlePos);

    // early exit ?
    //if (value == *middle)
    //  return true;

    // keep looking on the right or left side of pivot element ?
    if (value < *middle)
      return binary_search_recursive(begin,  middle, value);
    else
      return binary_search_recursive(middle, end,    value);
  }


  /// same algorithm as STL's find
  template <typename Iterator, typename Value>
  Iterator find(Iterator begin, Iterator end, Value value)
  {
    // compare all values until found
    while (begin != end)
    {
      if (*begin == value)
        return begin; // found !
      ++begin;
    }

    // not found
    return end; // == current
  }


  /// find, optimized for sorted data
  // requires: iff !(x<y) && !(x>y) then x == y
  template <typename Iterator, typename Value>
  Iterator find_sorted(Iterator begin, Iterator end, Value value)
  {
    // skip all smaller values
    while (begin != end && *begin < value)
      ++begin;

    // hit ?
    if (begin != end && *begin == value)
      return begin;
    else
      return end;
  }


  /// linear search in inner loop
  template <size_t thresholdLinear, typename Iterator, typename Value>
  bool binary_search_hybrid(Iterator begin, Iterator end, Value value)
  {
    // narrow down range of candidates
    auto numElements = std::distance(begin, end);

    // repeat until a single element is left
    while (numElements > int(thresholdLinear))
    {
      // pivot element
      auto middlePos = numElements >> 1;
      auto middle    = begin;
      std::advance(middle, middlePos);

      if (*middle < value)
      {
        // value must be on the right side of pivot element
        begin = middle;
        ++begin;
        numElements = numElements - middlePos - 1;
      }
      else
      {
        // value must be on the left  side of pivot element (or it is the pivot element)
        numElements = middlePos;
      }
    }

    // skip all smaller values
    while (begin != end && *begin < value)
      ++begin;

    // hit ?
    return begin != end && *begin == value;
  }
}



#define USE_WINDOWS_TIMER
#ifdef USE_WINDOWS_TIMER
#include <windows.h>
#else
#include <sys/time.h>
#endif

// OS-specific timing
static double seconds()
{
#ifdef USE_WINDOWS_TIMER
  LARGE_INTEGER frequency, now;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter  (&now);
  return now.QuadPart / double(frequency.QuadPart);
#else
  timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec + now.tv_usec/1000000.0;
#endif
}


int main()
{
  // create sorted data
  for (size_t i = 0; i < numElements; i++)
    sorted.push_back(Number(i*step));


  // STL
  auto startTime = seconds();
  for (size_t i = 0; i < numElements; i++)
  {
    // hit
    Number succeed = Number(i*step);
    if (!std::binary_search(sorted.begin(), sorted.end(), succeed))
      std::cout << "error @" << succeed << std::endl;
    // miss
    Number fail    = Number(i*step+notFound);
    if ( std::binary_search(sorted.begin(), sorted.end(), fail))
      std::cout << "error @" << fail << std::endl;
  }
  auto duration  = seconds() - startTime;
  std::cout << "STL: " << (duration*1000) << "ms" << std::endl;


  // my own binary search, same algorithm as STL
  startTime = seconds();
  for (size_t i = 0; i < numElements; i++)
  {
    // hit
    Number succeed = Number(i*step);
    if (!my::binary_search(sorted.begin(), sorted.end(), succeed))
      std::cout << "error @" << succeed << std::endl;
    // miss
    Number fail    = Number(i*step+notFound);
    if ( my::binary_search(sorted.begin(), sorted.end(), fail))
      std::cout << "error @" << fail << std::endl;
  }
  duration  = seconds() - startTime;
  std::cout << "Binary Search: " << (duration*1000) << "ms" << std::endl;


  // my own binary search, early comparison for equality
  startTime = seconds();
  for (size_t i = 0; i < numElements; i++)
  {
    // hit
    Number succeed = Number(i*step);
    if (!my::binary_search_with_equality_test(sorted.begin(), sorted.end(), succeed))
      std::cout << "error @" << succeed << std::endl;
    // miss
    Number fail    = Number(i*step+notFound);
    if ( my::binary_search_with_equality_test(sorted.begin(), sorted.end(), fail))
      std::cout << "error @" << fail << std::endl;
  }
  duration  = seconds() - startTime;
  std::cout << "Early Equality Test: " << (duration*1000) << "ms" << std::endl;


  // my own binary search, recursive implementation
  startTime = seconds();
  for (size_t i = 0; i < numElements; i++)
  {
    // hit
    Number succeed = Number(i*step);
    if (!my::binary_search_recursive(sorted.begin(), sorted.end(), succeed))
      std::cout << "error @" << succeed << std::endl;
    // miss
    Number fail    = Number(i*step+notFound);
    if ( my::binary_search_recursive(sorted.begin(), sorted.end(), fail))
      std::cout << "error @" << fail << std::endl;
  }
  duration  = seconds() - startTime;
  std::cout << "Recursive: " << (duration*1000) << "ms" << std::endl;


  if (numElements <= 10000)
  {
    // compare all elements until match
    startTime = seconds();
    for (size_t i = 0; i < numElements; i++)
    {
      // hit
      Number succeed = Number(i*step);
      if (succeed != *my::find(sorted.begin(), sorted.end(), succeed))
        std::cout << "error @" << succeed << std::endl;
      // miss
      Number fail    = Number(i*step+notFound);
      if (sorted.end() != my::find(sorted.begin(), sorted.end(), fail))
        std::cout << "error @" << fail << std::endl;
    }
    duration  = seconds() - startTime;
    std::cout << "Simple Scan (unsorted): " << (duration*1000) << "ms" << std::endl;

    // compare sorted elements until match
    startTime = seconds();
    for (size_t i = 0; i < numElements; i++)
    {
      // hit
      Number succeed = Number(i*step);
      if (succeed != *my::find_sorted(sorted.begin(), sorted.end(), succeed))
        std::cout << "error @" << succeed << std::endl;
      // miss
      Number fail    = Number(i*step+notFound);
      if (sorted.end() != my::find_sorted(sorted.begin(), sorted.end(), fail))
        std::cout << "error @" << fail << std::endl;
    }
    duration  = seconds() - startTime;
    std::cout << "Simple Scan (sorted): " << (duration*1000) << "ms" << std::endl;
  }


  // my own binary search, inner loop is linear search
  startTime = seconds();
  for (size_t i = 0; i < numElements; i++)
  {
    // hit
    Number succeed = Number(i*step);
    if (!my::binary_search_hybrid<threshold>(sorted.begin(), sorted.end(), succeed))
      std::cout << "error @" << succeed << std::endl;
    // miss
    Number fail    = Number(i*step+notFound);
    if ( my::binary_search_hybrid<threshold>(sorted.begin(), sorted.end(), fail))
      std::cout << "error @" << fail << std::endl;
  }
  duration  = seconds() - startTime;
  std::cout << "Hybrid: " << (duration*1000) << "ms" << std::endl;

  return 0;
}
