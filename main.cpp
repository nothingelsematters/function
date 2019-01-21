#include "function.h"

#include <iostream>
#include <functional>
#include <vector>

void rvalue(int&& a) {
    std::cout << a << '\n';
}

void func(int a) {
    std::cout << "test function " << a << "\n";
}

void func_empty() {
    std::cout << "empty func_test\n";
}

class FuncStruct {
public:
    void operator()() const {
        std::cout << "test class member\n";
    }
};

class FuncStructAnother {
public:
    void operator()() const {
        std::cout << "test class operator()\n";
    }
};

class Enormous {
public:
    int a = 10;
    int b = 10;
    int c = 10;
    int d = 10;
    int e = 10;
    int f = 10;
    int g = 10;
    int h = 10;
    int y = 10;
    int a1 = 10;
    int b1 = 10;
    int c1 = 10;
    int d1 = 10;
    int e1 = 10;
    int f1 = 10;
    int g1 = 10;
    int h1 = 10;
    int y1 = 10;
    void operator()() {
        std::cout << "test big(" << sizeof(*this) << ")\n";
    }
};

int summarizing(int a, int b) {
    return a + b;
}

template<typename T>
void with_templates(const T& arg) {
    std::cout << "HEY\n";
    std::cout << arg;
}




void test_empty() {
    function<void ()> f;
    std::cout << "empty test: " << static_cast<bool>(f) << '\n';
}

void test_null() {
    function<void ()> f(nullptr);
    std::cout << "nullptr test: " << static_cast<bool>(f) << '\n';
}

void test_function() {
    function<void (int)> f1(func);
    f1(1);
}

void test_structure() {
    FuncStructAnother fuun;
    function<void ()> f2(fuun);
    f2();
}

void test_lambda() {
    function<void ()> f3([]() {
        std::cout << "test lambda function\n";
    });
    f3();
}

void test_member() {
    function<void (FuncStruct)> f4(&FuncStruct::operator());
    FuncStruct memb;
    f4(memb);
}

void test_sum() {
    function<int (int, int)> f5(summarizing);
    int a = 5;
    int b = 10;
    std::cout << "test sum: " << a << " + " << b << ' ' << f5(a, b) << '\n';
}

void test_bool() {
    function<void ()> f;
    function<void (FuncStruct)> f4(&FuncStruct::operator());
    FuncStruct funct;
    f4(funct);

    std::cout << "test bool: empty " << static_cast<bool>(f) << '\n'
           << "test bool: function " << static_cast<bool>(f4) << '\n';
}

void test_templates() {
    function<void (std::string)> f(with_templates<std::string>);
    f(std::string("test with templates\n"));
}

template<typename R, typename... Args>
void move_test_handler(function<R (Args...)>&& func, Args... args) {
    std::cout << "test move: " << func(args...) << '\n';
}

void test_move() {
    function<int (int, int)> f1(summarizing);
    move_test_handler(std::move(f1), 5, 8);
}

template<typename R, typename... Args>
void copy_test_handler(function<R (Args...)> func, Args... args) {
    std::cout << "test copy: " << func(args...) << '\n';
}

void test_copy() {
    function<int (int, int)> f1(summarizing);
    copy_test_handler(f1, 5, 8);
}

void test_swap() {
    FuncStructAnother fuun;
    function<void ()> f2(fuun);

    function<void ()> f3(func_empty);
    std::cout << "swap test: \n was: ";
    f3();
    f2();
    f3.swap(f2);
    std::cout << "now: ";
    f3();
    f2();
}

void test_bind() {
    function<void ()> binder = std::bind(func, 5);
    std::cout << "bind test: ";
    binder();
}

void test_bind_with_params() {
    using namespace std::placeholders;
    function<void (int)> binder = std::bind(func, _1);
    std::cout << "bind test with parametres: ";
    binder(5);
}

void test_rvalue() {
    int a = 5;
    function<void (int)> rvl(rvalue);
    rvl(5);
    rvl(std::move(a));
}

void test_enormous() {
    Enormous big;
    function<void ()> f(big);
    f();
    FuncStructAnother small;
    function<void ()> f2(small);
    f2();
    f.swap(f2);
    f();
    f2();
}

void test_exception() {
    function<void (int)> f;
    try {
        f(5);
    } catch (bad_function_call& e) {
        std::cout << "exception test: " << e.what() << '\n';
    }
}

int main() {
    test_empty();
    test_null();
    test_function();
    test_structure();
    test_member();
    test_lambda();
    test_sum();
    test_bool();
    test_move();
    test_copy();
    test_templates();
    test_swap();
    test_enormous();
    test_bind();
    test_bind_with_params();
    test_rvalue();
    test_exception();
}
