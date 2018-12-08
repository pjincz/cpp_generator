#include <vector>
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

Generator<int &> evenOf(vector<int> & vec) {
    return [&](Yield<int&>& yield) -> int & {
        for (int i = 0; i < vec.size(); i += 2) {
            yield(vec[i]);
        }
    };
}

int main() {
    for (int x : Generator<int>(func)) {
        cout << x << ", ";
    }
    cout << endl;

    for (int x : primesOf(334455)) {
        cout << x << ", ";
    }
    cout << endl;

    vector<int> A = {0,1,2,3,4,5,6,7,8,9};
    for (int & x : evenOf(A)) {
        cout << x << ", ";
        x = 999;
    }
    cout << endl;

    for (int x : A) {
        cout << x << ", ";
    }
    cout << endl;

    return 0;
}
