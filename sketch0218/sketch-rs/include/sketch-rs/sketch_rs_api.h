#pragma once

#ifdef _WIN32

#ifdef SKETCH_RS_EXPORT
#define SKETCH_RS_API __declspec(dllexport)
#else
#define SKETCH_RS_API __declspec(dllimport)
#endif

#else

// TODO: mac and linux
#define SKETCH_RS_API
#endif
