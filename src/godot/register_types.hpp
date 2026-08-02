#pragma once

#include <godot_cpp/core/class_db.hpp>

namespace gd = godot;

void initialize_pixeldither_module(gd::ModuleInitializationLevel p_level);
void uninitialize_pixeldither_module(gd::ModuleInitializationLevel p_level);
