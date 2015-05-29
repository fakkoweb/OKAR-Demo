/************************************************************************************

Filename    :   [EXTENDED FROM] CAPI_HMDState.cpp
Content     :   State associated with a single HMD
Created     :   January 24, 2014
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

#include "oculussdkextent\HMDStateExtended.h"
#include "oculussdkextent\TrackingStateReaderExtended.h"

/*
#include "OVR_Profile.h"
#include "Service/Service_NetClient.h"

#ifdef OVR_OS_WIN32
    #include "Displays/OVR_Win32_ShimFunctions.h"

    // For auto-detection of window handle for direct mode:
    #include <OVR_CAPI_D3D.h>
    #include <GL/CAPI_GLE.h>
    #include <OVR_CAPI_GL.h>

#elif defined(OVR_OS_LINUX)

    #include "Displays/OVR_Linux_SDKWindow.h" // For screen rotation

#endif

#include "Tracing/Tracing.h"
	*/

using namespace OVR::Vision;

namespace OVR { namespace CAPI {

// Returns prediction for time (also for past times!).
ovrTrackingState HMDStateExtended::PredictedTrackingStateExtended(double absTime, void*)
{
    ovrTrackingState ss;
	// CAST current TrackingStateReader object so that we can use the new function included
	OVR::Vision::TrackingStateReaderExtended* tsr_cast = reinterpret_cast<OVR::Vision::TrackingStateReaderExtended*>(&TheTrackingStateReader);
    
	// Use the new TrackingStateReaderExtended function over the already existing TrackingStateReader instances
	tsr_cast->GetTrackingStateAtTimeExtended(absTime, ss);

    // Zero out the status flags
    if (!pClient)// || !pClient->IsConnected(false, false))
    {
        ss.StatusFlags = 0;
    }

#ifdef OVR_OS_WIN32
    // Set up display code for Windows
    //OVR::Win32::DisplayShim::GetInstance().Active = (ss.StatusFlags & ovrStatus_HmdConnected) != 0;
#endif

    return ss;
}

}} // namespace OVR::CAPI
