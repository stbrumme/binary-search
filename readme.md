This is a mirror of my library hosted at https://create.stephan-brumme.com/binary-search/

# Binary Search Revisited

Some time ago I had to write code for a binary search of sorted data. Obviously there are libraries which can do the heavy lifting for me, such as C++'s STL.

When I looked up the source code of STL's binary_search I was surprised to find that it slightly differs from what I expected:
it always performs log2(N) less-than comparisons and only after that checks for equality exactly once: 
That means if it finds a match (something that equals what we are searching for) it still keeps going until subdivision finishes - and forfeits the chance for an early exit.

Since the STL guys are brilliant minds I really had to write benchmarks to compare `std::binary_search` against the binary search I was taught in school.
In addition, I tried various data types and container sizes.

## More ...
See my website https://create.stephan-brumme.com/binary-search/ for deeper explanations, code examples and benchmarks.
