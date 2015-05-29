/************************************************************************************
Filename    :   [EXTENDED FROM] OVR_CAPI.cpp
Content     :   Experimental simple C interface to the HMD - version 1.
Created     :   November 30, 2013
Authors     :   Michael Antonov
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
************************************************************************************/

#include "OVR.h"
#include "OVR_CAPI.h"
#include "oculussdkextent/HMDStateExtended.h"

using namespace OVR::CAPI;

// Functions as GetNullTrackingState() and GetHMDStateFromOvrHmd() have been reimplemented here
ovrTrackingState ovrHmd_GetTrackingStateExtended(ovrHmd hmddesc, double absTime)
{
	ovrTrackingState nullState = ovrTrackingState();
	nullState.HeadPose.ThePose.Orientation.w = 1.f; // Provide valid quaternions for head pose.

	if (!hmddesc || !hmddesc->Handle)
		return nullState;

	HMDStateExtended* hmds = (HMDStateExtended*)hmddesc->Handle;
	return hmds->PredictedTrackingStateExtended(absTime);
}

