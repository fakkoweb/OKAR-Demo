#pragma once

#include <opencv2\opencv.hpp>
#include <thread>
#include <mutex>
#include "Rift.h"
#include "OVR.h"
#include "OGRE/Ogre.h"
#include "Globals.h"

struct FrameCaptureData {
	Ogre::Quaternion pose;
	cv::Mat image;
};

class FrameCaptureHandler
{
	private:

		bool hasFrame{ false };
		bool stopped{ false };
		cv::VideoCapture videoCapture;
		std::thread captureThread;
		std::mutex mutex;
		FrameCaptureData frame;
		Rift* headset;
		ovrHmd hmd;
		const unsigned int deviceId;

	public:

		FrameCaptureHandler(const unsigned int input_device, Rift* input_headset) : headset(input_headset), deviceId(input_device)
		{
			hmd=headset->getHandle();
		}

		// Spawn capture thread and return webcam aspect ratio (width over height)
		float startCapture();
		void stopCapture();
		void set(const FrameCaptureData & newFrame);
		bool get(FrameCaptureData & out);
		void captureLoop();

};

