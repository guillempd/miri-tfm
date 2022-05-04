#pragma once

#include <imgui.h>
#include <nfd.hpp>

#include <string>

namespace ImGuiNfd
{
    std::string Load(const nfdu8filteritem_t* filterList = nullptr, nfdfiltersize_t filterCount = 0);
}