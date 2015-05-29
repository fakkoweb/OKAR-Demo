/************************************************************************************

Filename    :   Vision_SensorStateReader.h
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

#ifndef OVR_Vision_SensorStateReader_EXTENT_h
#define OVR_Vision_SensorStateReader_EXTENT_h

//This header file extends the TrackingStateReader class by reimplementing some tracking functions
#include "Vision\SensorFusion\Vision_SensorStateReader.h"


namespace OVR { namespace Vision {


//-----------------------------------------------------------------------------
// TrackingStateReaderExtended

// User interface to retrieve pose from the sensor fusion subsystem
class TrackingStateReaderExtended : public TrackingStateReader
{


public:

    // Get the full dynamical system state of the CPF, which includes velocities and accelerations,
    // predicted at a specified absolute point in time.
	// "Extended" to support absoluteTime in the past
    bool         GetTrackingStateAtTimeExtended(double absoluteTime, TrackingState& state) const;

    // Get the predicted pose (orientation, position) of the center pupil frame (CPF) at a specific point in time.
	// "Extended" to support absoluteTime in the past
    bool         GetPoseAtTimeExtended(double absoluteTime, Posef& transform) const;

};


}} // namespace OVR::Vision

#endif // Vision_SensorStateReader_h
