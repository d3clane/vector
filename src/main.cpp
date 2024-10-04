#include "Vector.hpp"

#include <iostream>

int main()
{
    MyStd::Vector<int> v;
    v.pushBack(1);
    v.pushBack(2);
    v.pushBack(3);
    v.pushBack(4);
    v.pushBack(5);
    v.pushBack(6);
    v.pushBack(7);
    v.pushBack(8);
    v.pushBack(9);
    v.pushBack(10);

    for (auto it = v.begin(); it != v.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
}