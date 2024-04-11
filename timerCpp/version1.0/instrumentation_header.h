// instrumentation_header.h

#ifndef __cplusplus
// 不是C++代码时进行插装
#pragma GCC optimize ("-finstrument-functions -g ")
#endif
