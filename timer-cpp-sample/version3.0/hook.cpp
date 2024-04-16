
#include <cstdio>

#include "perf_counter.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <tuple>
#include <stack>
#include <iostream>

using namespace std;

struct pairt_hash   // Hash函数
{
    template<typename T1, typename T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        return std::hash<T1>{}(p.first) ^ std::hash<T2>{}(p.second);
    }
};

// 类型定义
using ULL = unsigned long long;
using KEY   = std::pair<ULL, ULL>;
using VALUE = std::tuple<int, ULL, ULL>;
using MAP = std::unordered_map<KEY, VALUE, pairt_hash>;

// 变量定义
static std::stack<ULL> funcTimeStack;   // 时间栈
static std::stack<ULL> funcAddrStack;   // 函数地址栈

static ULL curAddr, fatherAddr;
static MAP funcSampleInfo;  // 函数调用关系的数据结构


// 全局变量
static ULL shell_start_time;
static ULL shell_end_time;
static ULL self_start_time;
static ULL self_end_time;


void printFuncInfo(const MAP& map); // 输出函数调用关系数据


extern "C" void __attribute__((constructor)) traceBegin(void) { // 被测程序运行前，会执行这个函数
    fprintf(stdout, "----------traceBegin----------\n");
}

extern "C" void __attribute__((destructor)) traceEnd(void) {    // 被测程序运行结束，会执行这个函数
    fprintf(stdout, "----------traceEnd----------\n");
}

extern "C" 
void __attribute__((no_instrument_function)) __cyg_profile_func_enter(void *func, void *caller) {

    shell_start_time = perf_counter();  // 时钟打点
    funcTimeStack.push(shell_start_time);   // 压栈

    funcAddrStack.push((ULL)func);  // 函数地址压栈

    self_start_time = perf_counter();   // 时钟打点
    funcTimeStack.push(self_start_time);    // 压栈

}

extern "C" void __attribute__((no_instrument_function)) __cyg_profile_func_exit(void *func, void *caller) {

    self_end_time = perf_counter();     // 时钟打点


    self_start_time = funcTimeStack.top();   funcTimeStack.pop();   // 从时间栈中，获取压入栈中的时间
    shell_start_time = funcTimeStack.top();  funcTimeStack.pop();
    
    funcAddrStack.pop();
    fatherAddr = funcAddrStack.size() == 0 ? (ULL)caller : funcAddrStack.top(); // 获取父函数地址

    KEY key{(ULL)func, fatherAddr}; // 构建函数调用关系的KEY

    if(funcSampleInfo.find(key) == funcSampleInfo.end()) {  // 如果函数调用关系KEY，不在MAP中，插入
        funcSampleInfo[key] = VALUE{1, self_end_time - self_start_time, perf_counter() - shell_start_time};
    } else {    // 如果函数调用关系KEY，在MAP中，更新调用次数，自身运行时间，和带探针开销的时间
        std::get<0>(funcSampleInfo[key]) ++;
        std::get<1>(funcSampleInfo[key]) += self_end_time - self_start_time;
        std::get<2>(funcSampleInfo[key]) += perf_counter() - shell_start_time;
    }
    
    if(funcTimeStack.size() == 0) { // 当时间栈为空，说明插桩结束
        printFuncInfo(funcSampleInfo);  // 将数据输出
    }
}


void printFuncInfo(const MAP& map)
{
    for(const auto& item : map) {
        cout << (void*)item.first.first << " " << (void*)item.first.second << " ";
        cout << std::get<0>(item.second) << " " << std::get<1>(item.second) << " " << std::get<2>(item.second) << endl;
    }
}



