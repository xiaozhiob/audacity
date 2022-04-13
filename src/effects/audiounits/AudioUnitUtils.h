/*!********************************************************************

  Audacity: A Digital Audio Editor

  @file AudioUnitUtils.h

  Paul Licameli

***********************************************************************/

#ifndef __AUDACITY_AUDIO_UNIT_UTILS__
#define __AUDACITY_AUDIO_UNIT_UTILS__

#include <algorithm>
#include <AudioUnit/AudioUnit.h>

namespace AudioUnitUtils {
   //! Type-erased function to set an AudioUnit property
   OSStatus SetPropertyPtr(AudioUnit unit, AudioUnitPropertyID inID,
      const void *pProperty, UInt32 size, AudioUnitScope inScope,
      AudioUnitElement inElement);

   //! Set an AudioUnit property of deduced type,
   //! supplying most often used values as defaults for scope and element
   template<typename T>
   OSStatus SetProperty(AudioUnit unit, AudioUnitPropertyID inID,
      const T &property,
      AudioUnitScope inScope = kAudioUnitScope_Global,
      AudioUnitElement inElement = 0)
   {
      return SetPropertyPtr(unit, inID,
         &property, sizeof(property), inScope, inElement);
   }
}

#endif
