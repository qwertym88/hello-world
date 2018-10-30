#include "bind.h"

#include <iostream>
#include <string>
#include <functional>
#include <utility>

//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#   define TEST_FUNC_NAME __FUNCSIG__
#elif defined(__GNUC__)
#   define TEST_FUNC_NAME __PRETTY_FUNCTION__
#else
#   define TEST_FUNC_NAME __func__
#endif

#define TEST_CASE(...) \
    std::cout << std::endl \
              << "----------------------------------" << std::endl \
              << TEST_FUNC_NAME << " ->: " __VA_ARGS__ << std::endl \
              << "----------------------------------" << std::endl

//////////////////////////////////////////////////////////////////////////检查返回值判断是否正确 
//大神级测试代码看不懂很正常... 

namespace test_result_traits_np
{
    using namespace simple;

    class Foo {};

    class Func
    {
    public:
        int operator()(void) { return 0; }
    };

    template <typename>
    struct check;

    template <>
    struct check<int>
    {
        check(void)
        {
            std::cout << "int!" << std::endl;
        }
    };
}

void test_result_traits(void) //应全部输出int（实际也是） 
{
    TEST_CASE();
    using namespace test_result_traits_np;

    check<result_traits<int(*)(void)>::type>();
    check<result_traits<int(Foo::*)(void)>::type>();
    check<result_traits<Func>::type>();
}

//////////////////////////////////////////////////////////////////////////检查绑定器（如果可以这么说） 

namespace test_fr_np
{
    using namespace simple;

    void func(void)
    {
        std::cout << TEST_FUNC_NAME << std::endl;
    }

    class Func
    {
    public:
        int operator()(int a, double b)
        {
            std::cout << TEST_FUNC_NAME << "(" << a << ", " << b << ")" << std::endl;
            return 0;
        }
    };
}

void test_fr(void)//事实证明达到了我们需要的效果 
{
    TEST_CASE();
    using namespace test_fr_np;

    fr<void(void)> f1(func);
    f1();

    Func func;
    fr<Func, placeholder<2>, placeholder<1>> f2(func, _2, _1);
    f2(123, 321);
}

//////////////////////////////////////////////////////////////////////////综合测试 

namespace test_bind_np
{
    using namespace simple;

    void func0_0(void)
    {
        std::cout << TEST_FUNC_NAME << std::endl;
    }

    int func0_1(void)
    {
        std::cout << TEST_FUNC_NAME << " ->: ";
        return 0;
    }

    int func1(long a)
    {
        std::cout << TEST_FUNC_NAME << "(" << a << ")" << " ->: ";
        return 1;
    }

    static int func2(long a, int b)
    {
        std::cout << TEST_FUNC_NAME << "(" << a << ", " << b << ")" << " ->: ";
        return 2;
    }

    class A
    {
    public:
        virtual int func(long a, int b, long c, int d)
        {
            std::cout << TEST_FUNC_NAME << "(" << a << ", "
                << b << ", "
                << c << ", "
                << d << ")" << " ->: ";
            return 4;
        }
    };

    class B : public A
    {
    public:
        int func(long a, int b, long c, int d)
        {
            std::cout << TEST_FUNC_NAME << "(" << a << ", "
                << b << ", "
                << c << ", "
                << d << ")" << " ->: ";
            return 4;
        }

        int func_const(long a) const
        {
            std::cout << TEST_FUNC_NAME << "(" << a << ")" << " ->: ";
            return 1;
        }

        static int funcS(long a)
        {
            std::cout << TEST_FUNC_NAME << "(" << a << ")" << " ->: ";
            return 1;
        }
    };

    class Func0
    {
    public:
        int operator()(void)
        {
            std::cout << TEST_FUNC_NAME << " ->: ";
            return 0;
        }
    };

    class Func1
    {
    public:
        std::string& operator()(std::string& s)
        {
            std::cout << TEST_FUNC_NAME << "(" << s << ")" << " ->: ";
            return s;
        }
    };

    class Func2
    {
    public:
        int operator()(void) const volatile
        {
            std::cout << TEST_FUNC_NAME << " ->: ";
            return 0;
        }
    };

    class UncopyableFunc
    {
    public:
        UncopyableFunc(void) {}
        UncopyableFunc(UncopyableFunc&&) {}
        UncopyableFunc(const UncopyableFunc&) = delete;

        int operator()(void)
        {
            std::cout << TEST_FUNC_NAME << " ->: ";
            return 0;
        }
    };
}

void test_bind(void)
{
    TEST_CASE();
    using namespace test_bind_np;//普通函数 
    {
        bind(func0_0)();
        std::cout << bind(func0_1)() << std::endl;
        std::cout << bind(func1, _1)(123) << std::endl;
        std::cout << bind(func2, _1, _1)(321) << std::endl;
    }
    std::cout << std::endl;//成员函数 
    {
        B a;
        std::cout << bind(&B::func, a, 1, _1, 3, 4)(2) << std::endl;//普通 
        std::cout << bind(&B::func_const, &a, 123)() << std::endl;//const 
        A* pa = &a;
        std::cout << bind(&A::func, pa, _4, _3, _2, _1)(1, 2, 3, 4) << std::endl;//继承（多态） 
        std::cout << bind(B::funcS, 123)() << std::endl;//static函数 
    }
    std::cout << std::endl;//仿函数 
    {
        Func0 f0;
        std::cout << bind(&f0)() << std::endl;
        Func1 f1;
        std::string s("Hello Bind!");
        std::cout << simple::bind(f1, std::ref(s))() << std::endl; // avoids ADL// ? 
        Func2 f2;
        std::cout << bind(f2)() << std::endl;
    }
    std::cout << std::endl;//特殊定义了的函数 
    {
        UncopyableFunc ff;
        std::cout << bind(std::move(ff))() << std::endl;
    }
}

//////////////////////////////////////////////////////////////////////////

int main(void)
{
    test_result_traits();
    test_fr();
    test_bind();
    TEST_CASE(<< "Finished!");
    return 0;
}
