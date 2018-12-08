#include <iostream>
#include "cpp_generator.h"

using namespace cpp_generator;
using namespace std;

void func(Yield<int>& yield) {
    yield(111);
    yield(222);
    yield(333);
}

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


int main() {
    for (int x : Generator<int>(func)) {
        cout << x << endl;
    }
    cout << endl;

    for (int x : primesOf(334455)) {
        cout << x << endl;
    }
    cout << endl;

    return 0;
}
