
#include <cstdio>
#include <dlfcn.h>
#include <cxxabi.h>

#include "perf_counter.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <tuple>
#include <stack>
#include <iostream>

using namespace std;


struct pairt_hash
{
    template<typename T1, typename T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
    }
};

using ULL = unsigned long long;
using KEY   = std::pair<ULL, ULL>;
using VALUE = std::tuple<int, ULL, ULL>;
using MAP = std::unordered_map<KEY, VALUE, pairt_hash>;

static std::stack<ULL> funcTimeStack;
static std::stack<ULL> funcAddrStack;

static ULL curAddr, fatherAddr;

static MAP funcSampleInfo;




// 探针功能开关
#define __DEBUG_PRT__
#define __DEBGU_DLADDR__


// 全局变量
static ULL shell_start_time;
static ULL shell_end_time;
static ULL self_start_time;
static ULL self_end_time;


void printFuncInfo(const MAP& map);

inline const char *get_funcname(const char *src) {
        int status = 99;
        const char *f = abi::__cxa_demangle(src, nullptr, nullptr, &status);
        return f == nullptr ? src : f;
}

extern "C" void __attribute__((constructor)) traceBegin(void) {
    #ifdef __DEBUG_PRT__
    fprintf(stdout, "----------traceBegin----------\n");
    #endif
}

extern "C" void __attribute__((destructor)) traceEnd(void) {
    #ifdef __DEBUG_PRT__
    fprintf(stdout, "----------traceEnd----------\n");
    #endif
}

extern "C" 
void __attribute__((no_instrument_function)) __cyg_profile_func_enter(void *func, void *caller) {

    shell_start_time = perf_counter();

    funcTimeStack.push(shell_start_time);

    funcAddrStack.push((ULL)func);

    #ifdef __DEBUG_PRT__
    fprintf(stdout, "##entr %p %p\n", func, caller);
    #endif

    #ifdef __DEBGU_DLADDR__
    Dl_info info1, info2;
    if (dladdr(func, &info1) & dladdr(caller, &info2)) {
        fprintf(stdout, "enter func: %s father: %s\n", get_funcname(info1.dli_sname), get_funcname(info2.dli_sname));
    }
    #endif


    self_start_time = perf_counter();
    funcTimeStack.push(self_start_time);

}

extern "C" void __attribute__((no_instrument_function)) __cyg_profile_func_exit(void *func, void *caller) {

    self_end_time = perf_counter();


    #ifdef __DEBUG_PRT__
    fprintf(stdout, "##exit %p %p\n", func, caller);
    #endif

    #ifdef __DEBGU_DLADDR__
    Dl_info info1, info2;
    if (dladdr(func, &info1) & dladdr(caller, &info2)) {
        fprintf(stdout, "exit func: %s father: %s\n", get_funcname(info1.dli_sname), get_funcname(info2.dli_sname));
    }
    #endif


    self_start_time = funcTimeStack.top();   funcTimeStack.pop();
    shell_start_time = funcTimeStack.top();  funcTimeStack.pop();

    
    funcAddrStack.pop();
    fatherAddr = funcAddrStack.size() == 0 ? (ULL)caller : funcAddrStack.top();
    KEY key{(ULL)func, fatherAddr};

    if(funcSampleInfo.find(key) == funcSampleInfo.end()) {
        funcSampleInfo[key] = VALUE{1, self_end_time - self_start_time, perf_counter() - shell_start_time};
    } else {
        std::get<0>(funcSampleInfo[key]) ++;
        std::get<1>(funcSampleInfo[key]) += self_end_time - self_start_time;
        std::get<2>(funcSampleInfo[key]) += perf_counter() - shell_start_time;
    }
    
    if(funcTimeStack.size() == 0) {
        printFuncInfo(funcSampleInfo);
    }
}


void printFuncInfo(const MAP& map)
{
    for(const auto& item : map) {
        cout << (void*)item.first.first << " " << (void*)item.first.second << " ";
        // fprintf(stdout, "%p %p", (void*)item.first.first, (void*)item.first.second);
        cout << std::get<0>(item.second) << " " << std::get<1>(item.second) << " " << std::get<2>(item.second) << endl;
    }
}



