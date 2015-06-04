#include "Globals.h"

bool ROTATE_VIEW = false;
//bool NO_RIFT = false;
bool DEBUG_WINDOW = true;
unsigned short int CAMERA_BUFFERING_DELAY = 0;		// delay between a grab() call and when the frame was actually captured by the camera
													// This value will be made adjustable at runtime: you will know that the value is
													// right when you will see that the plane where the camera image is projected is
													// moving accordingly to the environment that the camera is actually capturing
													// ex.	plane moves then the image moves -> reduce delay value
													//		image moves then the plane moves -> increase delay value
													// WARNING: this value depends from your OS/Camera setup!

//Globals used from Camera.cpp and Scene.cpp
std::chrono::steady_clock::time_point camera_last_frame_request_time = std::chrono::system_clock::now();
std::chrono::duration< int, std::milli > camera_last_frame_display_delay;