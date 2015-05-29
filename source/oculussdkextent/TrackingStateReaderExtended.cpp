/************************************************************************************

Filename    :   Vision_SensorStateReader.cpp
Content     :   Separate reader component that is able to recover sensor pose
Created     :   June 4, 2014
Authors     :   Chris Taylor

Copyright   :   Copyright 2014 Oculus VR, LLC All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.2 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.2 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*************************************************************************************/

#include "oculussdkextent\TrackingStateReaderExtended.h"
#include "util.h"				// for sgn() or other custom made util functions in this project



namespace OVR { namespace Vision {


//-------------------------------------------------------------------------------------

// This is a "perceptually tuned predictive filter", which means that it is optimized
// for improvements in the VR experience, rather than pure error.  In particular,
// jitter is more perceptible at lower speeds whereas latency is more perceptible
// after a high-speed motion.  Therefore, the prediction interval is dynamically
// adjusted based on speed.  Significant more research is needed to further improve
// this family of filters.
// EXTENDED TO CALCULATE "PREDICTION" IN THE PAST (negative predictionDt)
static Pose<double> calcPredictedPoseExtended(const PoseState<double>& poseState, double predictionDt)
{
    Pose<double> pose = poseState.ThePose;
    const double linearCoef = 1.0;
    Vector3d angularVelocity = poseState.AngularVelocity;
    double angularSpeed = angularVelocity.Length();

    // This could be tuned so that linear and angular are combined with different coefficients
    double speed = angularSpeed + linearCoef * poseState.LinearVelocity.Length();
	
    const double slope = 0.2; // The rate at which the dynamic prediction interval varies
    double candidateDt = slope * speed; // TODO: Replace with smoothstep function
										// Assumption made:	if predictionDt > 0 speed is constant until t=candidateDt
										//					if predictionDt < 0 speed has been constant from t=-candidateDt

    double dynamicDt = predictionDt;

    // Choose the candidate if candidate is shorter than prediction, to improve stability
    if (abs(predictionDt) > candidateDt)	// TWEAKED!
    {
        dynamicDt = candidateDt*sgn(predictionDt);	// must preserve the sign
    }

    if (angularSpeed > 0.001)
    {
        pose.Rotation = pose.Rotation * Quatd(angularVelocity, angularSpeed * dynamicDt);
    }

    pose.Translation += poseState.LinearVelocity * dynamicDt;

    return pose;
}

PoseState<float> calcPredictedPoseStateExtended(const LocklessSensorState& sensorState, double absoluteTime, const Posed& centeredFromWorld)
{
    // Delta time from the last available data
    double pdt = absoluteTime - sensorState.WorldFromImu.TimeInSeconds;
    static const double maxPdt = 0.1;

    // If delta went negative due to synchronization problems between processes or just a lag spike,
    /* if (pdt < 0)		// CONSIDER ALSO THIS CASE!
    {
        pdt = 0;
    } */
    if (abs(pdt) > maxPdt)	// if prediction is too in the future (or in the past), it is clamped to maxPdt
    {
        pdt = maxPdt*sgn(pdt);	// TWEAKED!
        static double lastLatWarnTime = 0;
        if (lastLatWarnTime != sensorState.WorldFromImu.TimeInSeconds)
        {
            lastLatWarnTime = sensorState.WorldFromImu.TimeInSeconds;
			// I AM FAILING TO LINK THE FOLLOWING FUNCTION...
            // LogText("[TrackingStateReader] Prediction interval too high: %f s, clamping at %f s\n", pdt, maxPdt);
        }
    }

    PoseStatef result;
    result = PoseStatef(sensorState.WorldFromImu);
    result.TimeInSeconds = absoluteTime;
    result.ThePose = Posef(centeredFromWorld * calcPredictedPoseExtended(sensorState.WorldFromImu, pdt) * sensorState.ImuFromCpf);
    return result;
}


bool TrackingStateReaderExtended::GetTrackingStateAtTimeExtended(double absoluteTime, TrackingState& ss) const
{
    LocklessCameraState cameraState;
    LocklessSensorState sensorState;

    if (CameraUpdater)
        cameraState = CameraUpdater->GetState();
    if (HmdUpdater)
        sensorState = HmdUpdater->SensorState.GetState();

    // Update the status flags
    ss.StatusFlags = cameraState.StatusFlags | sensorState.StatusFlags;

    // If no hardware is connected, override the tracking flags
    if (0 == (ss.StatusFlags & Status_HMDConnected))
    {
        ss.StatusFlags &= ~Status_TrackingMask;
    }
    if (0 == (ss.StatusFlags & Status_PositionConnected))
    {
        ss.StatusFlags &= ~(Status_PositionTracked | Status_CameraPoseTracked);
    }

    // If tracking info is invalid,
    if (0 == (ss.StatusFlags & Status_TrackingMask))
    {
        return false;
    }
    
    ss.HeadPose = calcPredictedPoseStateExtended(sensorState, absoluteTime, CenteredFromWorld);

    ss.CameraPose        = Posef(CenteredFromWorld * cameraState.WorldFromCamera);
    ss.LeveledCameraPose = Posef(CenteredFromWorld * Posed(Quatd(), cameraState.WorldFromCamera.Translation));

    ss.RawSensorData = sensorState.RawSensorData;
    return true;
}

bool TrackingStateReaderExtended::GetPoseAtTimeExtended(double absoluteTime, Posef& transform) const
{
    TrackingState ss;

    if (!GetTrackingStateAtTimeExtended(absoluteTime, ss))
    {
        return false;
    }

    transform = ss.HeadPose.ThePose;

    return true;
}



}} // namespace OVR::Vision
