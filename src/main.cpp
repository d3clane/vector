#include "Vector.hpp"
#include "Allocators/StaticAllocator.hpp"

#include <iostream>
#include <vector>

#include "Log.h"

struct TestStruct
{
    int val = 78;

    TestStruct() { std::cout << "constructor\n"; }
    TestStruct(const TestStruct& other) { val = other.val; std::cout << "copy constructor\n"; }

    TestStruct(TestStruct&&) = delete;

    TestStruct& operator=(TestStruct&&) = delete;
    TestStruct& operator=(const TestStruct& other) { val = other.val; std::cout << "copy assignment\n"; return *this; }

    ~TestStruct() { std::cout << "destructor\n";  }

    operator int() const { return val; }
};

void test()
{
    TestStruct myTmp;
    myTmp.val = 23;
    std::cout << "BEGIN\n";

    MyStd::Vector<TestStruct, MyStd::StaticAllocator<TestStruct, 15> > v{5, myTmp};

    for (size_t i = 0; i < v.size(); ++i) std::cout << v[i] << " ";
    std::cout << "\n";
    std::cout << "OPERATOR=\n";
    v[1] = myTmp;

    std::cout << "RESERVE:\n";
    v.reserve(4);

    std::cout << "PUSHBACK:\n";
    v.pushBack(myTmp);

    std::cout << "POPBACK:\n";
    v.popBack();
    
    v.popBack();
    v.popBack();

    //std::cout << "CLEAR\n";
    //v.clear();

    std::cout << "PRINT\n";
    
    const auto cv{v};
    for (auto it = cv.begin(); it != cv.end(); ++it)
    {
        std::cout << it->val << std::endl;
    }
    
    std::cout << "NEXT\n";
    for (auto it = v.begin(); it != v.end(); ++it)
    {
        std::cout << it->val << std::endl;
    }

    std::cout << "OR\n";

    for (size_t i = 0; i < v.size(); ++i)
    {
        //printf("%d", static_cast<int>(v[i]));
        std::cout << static_cast<int>(v[i]) << std::endl;
    }
    //exit(0);

    std::cout << "END\n";
}

int main()
{
    test();
#if 0
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
#endif

#if 0
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

    bv.front() = 0;

    std::cout << bv.front() << " " << bv.back() << std::endl;
#endif

#if 0
    std::vector<tmp> v;
    v.reserve(10);
    v.push_back(tmp());

    std::cout << "HE\n";
    v.assign(4, tmp());
    //v.clear();

    std::cout << "ME\n";
    
    tmp t;

    std::cout << "ZE\n";

    new(&t) tmp();
    std::cout << "ENDED\n";
#endif

}