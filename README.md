# cpp_generator

**Write generator in C++!**

Build on the darkest magic.

## usage

```c++
// include necessary files for demos
#include <vector>
#include <iostream>
// and include me
#include "cpp_generator.h"

// just import namespace, that's not necessary, if you don't like it
using namespace cpp_generator;
using namespace std;

// OK, our first generator, let's generate some fibonacci numbers
Generator<int> fibonacci() {
    return [](Yield<int>& yield) {
        int a = 1, b = 1;
        while (true) {
            yield(a);
            b = a + b;
            a = b - a;
        }
    };
}

// Ya, the second generator, I want to know all primes of a number
Generator<int> primesOf(int x) {
    return [=](Yield<int>& yield) mutable {
        for (int i = 2; i * i <= x; ++i) {
            if (x % i == 0) {
                yield(i);
                x /= i;
                i -= 1;
            }
        }
        if (x != 1)
            yield(x);
    };
}

// The third generator, this generator returns a reference, so we can change values of vector
Generator<int &> evenOf(vector<int> & vec) {
    return [&](Yield<int&>& yield) -> int & {
        for (int i = 0; i < vec.size(); i += 2) {
            yield(vec[i]);
        }
    };
}

int main() {
    // OK, let's generate some fibonacci number
    for (int x : fibonacci()) {
        cout << x << ", ";
        if (x > 100)
            break; // it's safe to break in middle way
    }
    cout << endl;
    // we got: 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144,

    // now, let's get all primes of 334455
    for (int x : primesOf(334455)) {
        cout << x << ", ";
    }
    cout << endl;
    // we got: 3, 5, 11, 2027,

    // it's time to try something more complex
    vector<int> A = {0,1,2,3,4,5,6,7,8,9};
    for (int & x : evenOf(A)) {
        cout << x << ", ";
        x = 999; // yes, change it
    }
    cout << endl;
    // we got: 0, 2, 4, 6, 8,

    // let's verify whether original vector is changed
    for (int x : A) {
        cout << x << ", ";
    }
    cout << endl;
    // ya, we got: 999, 1, 999, 3, 999, 5, 999, 7, 999, 9,

    return 0;
}

```

Would you like it? If so, give me a star please~ :))))

## Thanks to

https://github.com/ramonza/libcoro
