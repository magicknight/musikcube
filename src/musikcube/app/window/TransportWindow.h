//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2016 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cursespp/Window.h>
#include <cursespp/IKeyHandler.h>
#include <core/library/track/Track.h>
#include <core/runtime/IMessage.h>
#include <core/audio/PlaybackService.h>
#include <sigslot/sigslot.h>

namespace musik {
    namespace cube {
        struct TransportDisplayCache;

        class TransportWindow :
            public cursespp::Window,
            public cursespp::IKeyHandler,
#if (__clang_major__ == 7 && __clang_minor__ == 3)
            public std::enable_shared_from_this<TransportWindow>,
#endif
            public sigslot::has_slots<>
        {
            public:
                enum FocusTarget {
                    FocusNone = 0,
                    FocusVolume = 1,
                    FocusTime = 2
                };

                TransportWindow(musik::core::audio::PlaybackService& playback);
                virtual ~TransportWindow();

                virtual void ProcessMessage(musik::core::runtime::IMessage &message);
                virtual void OnFocusChanged(bool focused);
                virtual void OnRedraw();
                virtual bool KeyPress(const std::string& key);

                void SetFocus(FocusTarget target);
                FocusTarget GetFocus() const;
                bool FocusNext();
                bool FocusPrev();
                void FocusFirst();
                void FocusLast();
                void RestoreFocus();

            private:
                enum TimeMode {
                    TimeLast = 0,
                    TimeSmooth = 1,
                    TimeSync = 2
                };

                void Update(TimeMode mode = TimeSmooth);

                void OnPlaybackServiceTrackChanged(size_t index, musik::core::TrackPtr track);
                void OnPlaybackModeChanged();
                void OnTransportVolumeChanged();
                void OnTransportTimeChanged(double time);
                void OnPlaybackShuffled(bool shuffled);

                bool paused;
                musik::core::audio::ITransport& transport;
                musik::core::audio::PlaybackService& playback;
                musik::core::TrackPtr currentTrack;
                FocusTarget focus, lastFocus;
                std::unique_ptr<TransportDisplayCache> displayCache;
                double lastTime;
        };
    }
}
