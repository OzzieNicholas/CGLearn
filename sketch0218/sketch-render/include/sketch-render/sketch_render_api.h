#pragma once

#ifdef _WIN32

#ifndef SKETCH_RENDER_EXPORT
#define SKETCH_RENDER_API __declspec(dllimport)
#else
#define SKETCH_RENDER_API __declspec(dllexport)
#endif

#elif __APPLE__

#define SKETCH_RENDER_API __attribute__((visibility("default")))

#endif
