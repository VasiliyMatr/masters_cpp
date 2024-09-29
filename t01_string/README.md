# T01 String

The first task pack is about string routines.

### 1. [DONE] std::println for char arrays

The task is to find info about how std::println should process char arrays (according to C++23 standard).

Found info:

1) [ostream.formatted.print]:

std::print delegates formatting to std::vformat. std::make_format_args is used to pass format arguments.

std::println delegates formatting to std::format (args are passed as-is).

2) [format.functions]:

std::format delegates formatting to std::vformat. std::make_format_args is used to pass format arguments.

3) Thus,

std::print and std::println both delegate formating to std::vformat and std::make_format_args is used to pass format arguments.

4) [format.arg.store]:
 
std::make_format_args returns an object that stores an array of formatting arguments and can be implicitly converted to std::basic_format_args<Context>.

This array elements are created with basic_format_arg<Context> constructor

5) [format.arg]:

basic_format_arg constructor stores static_cast<const char_type*>(input) to internal value if decay_t<TD> is char_type* or const char_type* . (6.9)

__Conclusion: char arrays are treated as C-style strings by std::print and std::println.__

### 2. [DONE] testqual

The task is to read [conv.qual] section of C++23 standard and implement char-related types casts checker. Checked types will contain `const`, `char`, `[]` or `*` only.

E.g:

```C++
testqual("const char **", "char **"); // -> false
```

Quick and dirty solution `include/ms/testqual.hpp`

### 3. [DONE] operator== for std::basic_string

The task is to implement operator== for std::basic_string.

Implementation details:
- Operators should be moved out of class for `"abcdef" == str` to work.
- It only makes sense to compare strings with the same CharT and Traits
- Different allocators should not affect comparison. Thus Allocator_1 and Allocator_2 parameters are introduced in the operator template.

Solution `include/ms/string_eq.hpp`

### 4. [DONE] COW string

The task is to implement copy on write (COW) string.

Following functionality should be implemented:

1) Big 5
2) Tokenization
3) Substring search

Solution `include/ms/cow.hpp`

### 5. String twine

The task is to implement string_twie class for O(log(N)) std::string_view concatination.

Example:

```C++
auto sv1 = "Hello,"sv, sv2 = "World!"sv;
auto s = string_twine(sv1, " ", sv2).str(); // -> std::string
```
