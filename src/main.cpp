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

    MyStd::Vector<bool> bv;
    bv.pushBack(true);
    bv.pushBack(false);
    bv.pushBack(false);
    bv.pushBack(true);
    bv.pushBack(false);

    for (size_t i = 0; i < bv.size(); ++i)
    {
        std::cout << bv[i] << std::endl;
    }

    bv.popBack();
    bv.popBack();

    bv[2] = true;

    std::cout << "RESTART\n";


    for (size_t i = 0; i < bv.size(); ++i)
    {
        std::cout << bv[i] << std::endl;
    }
}