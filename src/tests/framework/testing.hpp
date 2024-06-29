#pragma once
#include <string>
#include <vector>
#include <iostream>

#define FUNC_NAME TestCase
#define GEN_CLASS_NAME3(name, line) name ## line
#define GEN_CLASS_NAME2(name, line) GEN_CLASS_NAME3(name, line)
#define GEN_CLASS_NAME(name) GEN_CLASS_NAME2(name, __LINE__)

#define GEN_INSTANCE(name) name ## Instance

struct TestCase;

struct TestCase {
    TestCase(std::string name) : name_(name) {
    };
    virtual auto run() -> void = 0;
    auto report() {
        std::cout << "Running... " << name_ << "...\n";
        run();
        std::cout << "Finished running " << name_ << "\n";
    };
    const std::string name_;
};

static std::vector<TestCase*> cases;


#define GEN_TEST(FUNCNAME, strdescriptor,...)\
struct FUNCNAME : public TestCase {\
    FUNCNAME() : TestCase(strdescriptor) {\
        cases.push_back(this);\
    };\
    auto run() -> void override;\
};\
static FUNCNAME GEN_INSTANCE(FUNCNAME){};\
void FUNCNAME::run()\

#define TEST_CASE(name) GEN_TEST(GEN_CLASS_NAME(FUNC_NAME),name)

int main() {
    for(const auto& tCase : cases) {
        tCase->report();
    }
}
