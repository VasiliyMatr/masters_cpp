# T03 Inference

The third task pack is about types inference.

### 1. void inference proposal

The task is to consider void inference proposal. Example:

```C++
template<typename T> int foo(T x) {
    return 42;
}

....

int x = foo(); // -> int foo<void>()
```

And investigate on problems/advantages of such mechanism.

#### Investigation results:

Following examples shows that in order to preserve backward compatibility template functions with void inference should have the lowest priority in OVR.

Example 1:
```C++
template<typename T1, typename T2> int foo(T1 x, T2 y) {
    return 42;
}

struct Foo {
    Foo(int) {}
};

int foo(Foo, int = 0) {
    return 43;
}

....

int x = foo(1.0); // == 43 -> void inference should lose
```

Example 2:
```C++
template<typename T> int foo(T x) {
    return 42;
}

int foo(...) {
    return 43;
}

....

int x = foo(); // == 43 -> void inference should lose
```

And this priority feature will complicate OVR. As usually template functions have high OVR priority (higher than functions that need user conversions or have variadic arguments).

__And this is a solid reason to reject such a proposal__

### 2. std forward example

The task is to explain following example (a few typos were fixed):

```C++
int g(int &&t) { return 1; }
int g(int &t) { return 2; }

template <typename T>
int h(T &&t) { return g(std::forward<T> (t)); }

const int x = 1; int z = h(x); // ???
```

#### Explanation:

1) There is template argument deduction context
2) Argument is annotated with `&&`
3) Lvalue is passed to `h`

-> `T` will be deduced to `const int &`. And `t` will have type `const int &` (after reference collapsing).

Thus, `std::forward` will return value of type `const int &`.

And there are no viable candidates for calling `g` with `const int &` argument.

__The program is ill formed__
