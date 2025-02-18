#include "sketch-rs/core.h"
#include "sketch/Interface/IF_Controller.h"
#include "sketch/Interface/IF_Document.h"
#include "sketch/STCore.h"

void sketch_core_init() { sketch::STCore::inst->init_standalone(); }

void sketch_core_create_project(const wchar_t* path, const wchar_t* env) { newDocument(path, env); }

void sketch_core_active_controller(const wchar_t* name) { activateController(name); }

void sketch_core_tick() { sketch::STCore::inst->tick(); }
