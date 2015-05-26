#include "Globals.h"

bool ROTATE_VIEW = false;
//bool NO_RIFT = false;
bool DEBUG_WINDOW = true;

//Globals used from Camera.cpp and Scene.cpp
std::chrono::steady_clock::time_point camera_last_frame_request_time = std::chrono::system_clock::now();
std::chrono::duration< int, std::milli > camera_last_frame_display_delay;