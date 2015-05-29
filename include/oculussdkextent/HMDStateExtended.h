/************************************************************************************

Filename    :   [EXTENDED FROM] CAPI_HMDState.h
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

#ifndef OVR_HMDStateExtended_h
#define OVR_HMDStateExtended_h

//This header file creates a new HMDStateExtended class by reimplementing HMDState and casting TrackingStateReaderExtended
#include "CAPI\CAPI_HMDState.h"


namespace OVR { namespace CAPI {

//-------------------------------------------------------------------------------------
// ***** HMDStateExtended

// Describes a single HMD.
class HMDStateExtended : public HMDState
{
public:
	ovrTrackingState PredictedTrackingStateExtended(double absTime, void* unused = nullptr);
};

}} // namespace OVR::CAPI

#endif // OVR_CAPI_HMDState_h
