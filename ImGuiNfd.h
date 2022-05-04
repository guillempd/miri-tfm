#pragma once

#include <imgui.h>
#include <nfd.hpp>

namespace ImGuiNfd
{
    std::string Load(const nfdu8filteritem_t* filterList = nullptr, nfdfiltersize_t filterCount = 0)
    {
        NFD::UniquePath path;
        NFD::OpenDialog(path, filterList, filterCount);
        if (path)
        {
            return path.get();
        }
        else return "";
    }
}