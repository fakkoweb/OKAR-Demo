#include "Camera.h"


// Spawn capture thread and return webcam aspect ratio (width over height)
float FrameCaptureHandler::startCapture()
{
	float aspectRatio = 0;
	videoCapture.open(deviceId);
	if (!videoCapture.isOpened() || !videoCapture.read(frame.image))
	{
		std::cerr << "Could not open video source to capture first frame";
	}
	else
	{
		aspectRatio = (float)frame.image.cols / (float)frame.image.rows;
		captureThread = std::thread(&FrameCaptureHandler::captureLoop, this);
		std::cout << "Capture loop for camera " << deviceId << " started." << std::endl;
	}
	return aspectRatio;
}

void FrameCaptureHandler::stopCapture() {
	stopped = true;
	captureThread.join();
	videoCapture.release();
}

void FrameCaptureHandler::set(const FrameCaptureData & newFrame) {
	std::lock_guard<std::mutex> guard(mutex);
	frame = newFrame;
	hasFrame = true;
}

bool FrameCaptureHandler::get(FrameCaptureData & out) {
	if (!hasFrame) {
		return false;
	}
	std::lock_guard<std::mutex> guard(mutex);
	out = frame;
	hasFrame = false;
	return true;
}

void FrameCaptureHandler::captureLoop() {
	FrameCaptureData captured;
	while (!stopped) {
		float captureTime = ovr_GetTimeInSeconds();		//does not count the camera delay - measure it!
		ovrTrackingState tracking = ovrHmd_GetTrackingState(hmd, captureTime);

		if (tracking.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
			// The cpp compatibility layer is used to convert ovrPosef to Posef (see OVR_Math.h)
			Posef pose = tracking.HeadPose.ThePose;
			//headPose = pose;
			//float yaw, pitch, roll;
			//pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

			// Optional: move cursor back to starting position and print values
			//std::cout << "yaw: "   << RadToDegree(yaw)   << std::endl;
			//std::cout << "pitch: " << RadToDegree(pitch) << std::endl;
			//std::cout << "roll: "  << RadToDegree(roll)  << std::endl;
			captured.pose = Ogre::Quaternion(pose.Rotation.w,
				pose.Rotation.x, pose.Rotation.y, pose.Rotation.z);
		}


		//Save time point for request time of the frame (for camera frame rate calculation)
		camera_last_frame_request_time = std::chrono::system_clock::now();	//GLOBAL VARIABLE

		std::cout << "Retrieving frame from " << deviceId << " ..." << std::endl;
		if (videoCapture.read(captured.image))
		{
			set(captured);
			std::cout << "Frame retrieved from " << deviceId << "." << std::endl;
		}
		else
		{
			std::cout << "FAILED to retrieve frame from " << deviceId << "." << std::endl;
		}
		//cv::flip(captured.image.clone(), captured.image, 0);
		
	}
}
