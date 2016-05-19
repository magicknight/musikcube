#pragma once

#include "stdafx.h"

#include <core/playback/Transport.h>

using musik::core::audio::Transport;

namespace musik {
    namespace box {
        namespace text {
            void Truncate(std::string& str, size_t len);
            void Ellipsize(std::string& str, size_t len);
            void Duration(std::string& str);
        }
    }
}
