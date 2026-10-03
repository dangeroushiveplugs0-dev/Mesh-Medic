#pragma once

#include "EglContext.h"

namespace meshmedic::rendering {

class GlesRenderer {
public:
    bool render(EglContext& context, int width, int height);
};

} // namespace meshmedic::rendering
