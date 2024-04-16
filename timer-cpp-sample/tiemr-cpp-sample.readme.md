
[toc]

## 不同version之间区别
1、version1.0 简单的输入输出  
2、version2.0 带有运行时，函数名解析的版本  
3、version3.0 除去冗余代码，运行地址解析的版本，可以用作测量的demo  

## 需要思考的问题：
### 1、使用STL的数据结构，会在一下函数调用之前，就会析构，导致数据结果为零（有可能是这个原因，需要进一步探索）
```c++
__attribute__((destructor)) traceEnd(void){
    printf("%s\n", "----------traceEnd----------");
}
```
```c++
exit func: std::deque<int, std::allocator<int> >::deque() father: std::stack<int, std::deque<int, std::allocator<int> > >::stack<std::deque<int, std::allocator<int> >, void>()
##exit 0x40250c 0x402367
exit func: std::stack<int, std::deque<int, std::allocator<int> > >::stack<std::deque<int, std::allocator<int> >, void>() father: main
##entr 0x4022ed 0x40236c
enter func: AA() father: main
##entr 0x4022b6 0x402310
enter func: BB() father: AA()
##exit 0x4022b6 0x402310
exit func: BB() father: AA()
##exit 0x4022ed 0x40236c
exit func: AA() father: main
##entr 0x4022b6 0x402371
enter func: BB() father: main
##exit 0x4022b6 0x402371
exit func: BB() father: main
##entr 0x402446 0x402382
enter func: std::stack<int, std::deque<int, std::allocator<int> > >::~stack() father: main
##entr 0x4025d8 0x402478
enter func: std::deque<int, std::allocator<int> >::~deque() father: std::stack<int, std::deque<int, std::allocator<int> > >::~stack()
##entr 0x4028ce 0x40261a
enter func: std::_Deque_base<int, std::allocator<int> >::_M_get_Tp_allocator() father: std::deque<int, std::allocator<int> >::~deque()
##exit 0x4028ce 0x40261a
exit func: std::_Deque_base<int, std::allocator<int> >::_M_get_Tp_allocator() father: std::deque<int, std::allocator<int> >::~deque()
........
........
exit func: std::_Deque_base<int, std::allocator<int> >::_Deque_impl::~_Deque_impl() father: std::_Deque_base<int, std::allocator<int> >::~_Deque_base()
##exit 0x40277a 0x402666
exit func: std::_Deque_base<int, std::allocator<int> >::~_Deque_base() father: std::deque<int, std::allocator<int> >::~deque()
##exit 0x4025d8 0x402478
exit func: std::deque<int, std::allocator<int> >::~deque() father: std::stack<int, std::deque<int, std::allocator<int> > >::~stack()
##exit 0x402446 0x402382
exit func: std::stack<int, std::deque<int, std::allocator<int> > >::~stack() father: main
##exit 0x402329 0x7f7b5e229d90
exit func: main father: (null)
----------traceEnd----------
```

### 2、调用关系记录方法：是否有必要使用栈，存函数函数调用关系？
因为这个回调函数，已经包含了函数调用关系，可能没有必要使用函数地址栈，存函数父函数。
```c++
__cyg_profile_func_enter(void *func, void *caller)
```
答案：调用关系没办法有效聚合。
因为这里的调用关系caller地址，是函数中call指令的调用地址，并不是父函数的调用地址。
