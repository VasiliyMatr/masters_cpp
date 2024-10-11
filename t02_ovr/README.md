# T02 OVR

The second task pack is about overload resolution process.

### 1. Example 1

The task is to explain following example in accordance with C++23 standard

```C++

template <class T1, class T2> struct Pair {
  template<class U1 = T1, class U2 = T2> Pair(U1&&, U2&&) {}
};

struct S { S() = default; };
struct E { explicit E() = default; };

int f(Pair<E, E>) { return 1; }
int f(Pair<S, S>) { return 2; }

static_assert(f({{}, {}}) == 2, ""); // Error or Assert or OK?

```

Found info:

1) Both `f` functions are found during unqualified name resolution

2) Then, those functions participate in overload resolution [over.match]

3) [over.match.geneal] (12.2.1)

Overload resolution will happend in the context of:

(2.1) invocation of a function named in the function call syntax (12.2.2.2.2)

4) [over.call.func] (12.2.2.2.2)

Both functions fall into unqualified function call category. And both functions pass all checks for this category.

Thus, both functions are candidates.

5) [over.match.viable] (12.2.3)

Then, a set of viable functions is choosen with following checks:

(12.2.3.2) Parameters number is OK for both functions

(12.2.3.3) There are no associated constaints for both functions -> OK

(12.2.3.4) There should exist for each argument an `implicit conversion sequence` (12.2.4.2)

There is such a conversion sequence for both functions (sequence with template ctor of `Pair` will be matched).

6) Thus

Both functions are viable candidates for call. And implicit conversion sequences for both functions are the same (regardless of different types).

__Call to overloaded `f` is ill formed__

### 2. Example 2

The task is to explain following example in accordance with C++23 standard

```C++
struct Foo {};
struct Bar : Foo {};
struct Baz : Foo {};

struct X {
  operator Bar() { std::cout << "Bar\n"; return Bar{}; }
  operator Baz() const { std::cout << "Baz\n"; return Baz{}; }
};

void foo(const Foo &f) {}

int main() { foo(X{}); } // Bar or Baz?
```

Found info:

1) [over.best.ics.general] (12.2.4.2.1)

A well-formed implicit conversion sequence is one of the following:
- a standard conv sequence (12.2.4.2.2)
- a user-defined conversion sequence (12.2.4.2.3), or
- an elipsis conversion sequence (12.2.4.2.4)

2) [over.ics.user] (12.2.4.2.3)

A user-defined conversion sequence consists of:
- an initial standard conversion sequence
- followed by a user-defined conversion
- followed by a second standard conversion sequence

If the user-defined conversion is specified by a conversion function, the initial standard conversion sequence converts the source type to the type of the object parameter of that conversion function.

3) Thus, call to `foo` will trigger search for the best function to convert prvalue of type X to entity of type T. Where T can be converted to `cosnt Foo &` via standard conversions sequence.

Viable candidates: `X::operator Bar` (\*) and `X::operator Baz` (\*\*).

5) [over.match.best] (12.2.4)

(2.1) (\*) will be better candidate than (\*\*) (and thus, (\*) will be choosen) if the implicit conversion sequence of (\*) for implicit argument of type X will be better than that of (\*\*).

6) [over.ics.rank] (12.2.4.3)

(3.2.3) Implicit conversion of (\*) is better that that of (\*\*)

__"Bar\n" will be printed__
