#pragma once

#include "file/ifile.hpp"

namespace file
{
    struct DocumentLocation
    {
        IFile* file;
        size_t index;
    };
}