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
		
		//Save time point for request time of the frame (for camera frame rate calculation)
		camera_last_frame_request_time = std::chrono::system_clock::now();	//GLOBAL VARIABLE
		
		std::cout << "Retrieving frame from " << deviceId << " ..." << std::endl;
		
		// save tracking state before grabbing a new frame
		// grab() will ALWAYS return a frame OLDER than time of its call..
		// a CAMERA_BUFFERING_DELAY is used to predict a PAST pose relative to this moment
		// LOCAL OCULUSSDK HAS BEEN TWEAKED TO SUPPORT THIS (AND EXTEND THE BEHAVIOUR OF ovrHmd_GetTrackingState)
		ovrTrackingState tracking = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds() - CAMERA_BUFFERING_DELAY);	// [TODO]
		
		// grab a new frame
		if (videoCapture.grab())	// grabs a frame without decoding it
		{
			// if frame is valid, decode and save it
			videoCapture.retrieve(captured.image);

			// and save pose as well
			if (tracking.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
				Posef pose = tracking.HeadPose.ThePose;		// The cpp compatibility layer is used to convert ovrPosef to Posef (see OVR_Math.h)
				captured.pose = Ogre::Quaternion(pose.Rotation.w, pose.Rotation.x, pose.Rotation.y, pose.Rotation.z);
			}
			else
			{
				// try to predict prosition since last pose predicted
			}

			// set new capture as available
			set(captured);
			std::cout << "Frame retrieved from " << deviceId << "." << std::endl;
		}
		else
		{
			std::cout << "FAILED to retrieve frame from " << deviceId << "." << std::endl;
		}

		//cv::flip(captured.image.clone(), captured.image, 0);
		/*
		auto camera_last_frame_request_time_since_epoch = camera_last_frame_request_time.time_since_epoch();
		std::cout << "Time differences in implementations"
		<< "\n1. OpenCV Timestamp ms: " << (long)videoCapture.get(CV_CAP_PROP_POS_MSEC)
		<< "\n2. Chrono TimeSinceEpoch ms " << std::chrono::duration_cast<std::chrono::milliseconds>(camera_last_frame_request_time_since_epoch).count()
		<< "\n3. OculusSDK Timestamp ms " << captureTime << std::endl;
		*/
	}
}
