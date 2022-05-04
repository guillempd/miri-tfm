#include "ImGuiNfd.h"

std::string ImGuiNfd::Load(const nfdu8filteritem_t* filterList, nfdfiltersize_t filterCount)
{
    NFD::UniquePath path;
    NFD::OpenDialog(path, filterList, filterCount);
    if (path)
    {
        return path.get();
    }
    else return "";
}
