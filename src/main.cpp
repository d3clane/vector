#include "Vector.hpp"
#include "Allocators/StaticAllocator.hpp"

#include <iostream>
#include <vector>

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

    MyStd::Vector<bool> v{10};

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

    for (int i = 0; i < v.size(); ++i)
    {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";

    v.pushBack(true);
    v.pushBack(false);
    v.pushBack(false);
    v.pushBack(true);
    v.pushBack(false);

    for (size_t i = 0; i < v.size(); ++i)
    {
        std::cout << v[i] << " ";
    }

    std::cout << "\n";

    v.popBack();
    v.popBack();

    v[2] = true;

    std::cout << "RESTART\n";


    for (size_t i = 0; i < v.size(); ++i)
    {
        std::cout << v[i] << " ";
    }
    
    std::cout << "\n";
    v.front() = 0;

    std::cout << v.front() << " " << v.back() << std::endl;

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