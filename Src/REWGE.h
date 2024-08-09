#pragma once

#define WIN32

#define DEBUG
#define LOG_PROFILER
#define EDITOR


//有些编译器默认将 char 作为有符号类型，范围是 -128 到 127，而有些编译器默认将 char 作为无符号类型。因此，使用 char 可能导致跨平台的不一致行为。

typedef unsigned char			uint8;
typedef signed char				int8;
typedef unsigned short			uint16;
typedef signed short			int16;
typedef unsigned int			uint32;
typedef signed int				int32;
typedef unsigned long long		uint64;
typedef signed long long		int64;

