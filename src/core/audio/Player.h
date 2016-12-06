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

#include <core/config.h>
#include <core/audio/IStream.h>
#include <core/sdk/IOutput.h>
#include <core/sdk/IBufferProvider.h>

#include <sigslot/sigslot.h>

#include <thread>
#include <mutex>
#include <condition_variable>

namespace musik { namespace core { namespace audio {

    struct FftContext;

    class Player : public musik::core::sdk::IBufferProvider {
        public:
            class PlayerEventListener {
                public:
                    virtual void OnPlaybackStarted(Player *player) = 0;
                    virtual void OnPlaybackAlmostEnded(Player *player) = 0;
                    virtual void OnPlaybackFinished(Player *player) = 0;
                    virtual void OnPlaybackError(Player *player) = 0;
            };

            static Player* Create(
                const std::string &url,
                std::shared_ptr<musik::core::sdk::IOutput> output,
                PlayerEventListener *listener);

            virtual void OnBufferProcessed(musik::core::sdk::IBuffer *buffer);

            void Play();
            void Destroy();

            double Position();
            void SetPosition(double seconds);

            std::string GetUrl() const { return this->url; }

            bool Exited();

        private:
            bool PreBuffer();
            int State();
            void ReleaseAllBuffers();

        private:
            friend void playerThreadLoop(Player* player);

            Player(
                const std::string &url,
                std::shared_ptr<musik::core::sdk::IOutput> output,
                PlayerEventListener *listener);

            virtual ~Player();

            typedef std::shared_ptr<std::thread> ThreadPtr;
            typedef std::list<BufferPtr> BufferList;
            typedef std::set<BufferPtr> BufferSet;

            typedef enum {
                Precache = 0,
                Playing = 1,
                Quit = 2
            } States;

            std::shared_ptr<musik::core::sdk::IOutput> output;
            StreamPtr stream;
            ThreadPtr thread;
            BufferList lockedBuffers;
            PlayerEventListener* listener;

            std::string url;

            BufferList prebufferQueue;

            std::mutex queueMutex, positionMutex;
            std::condition_variable writeToOutputCondition;

            double volume;
            double currentPosition;
            double setPosition;
            int state;
            bool notifiedStarted;
            float* spectrum;
            uint64 samplesWritten;

            FftContext* fftContext;
    };

} } }
