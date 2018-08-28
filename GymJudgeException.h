#pragma once

#include <stdexcept>

class IncompleteSelectionError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;

    IncompleteSelectionError(const char * what = "incomplete selection"):
        std::runtime_error(what)
    {
    }
};

class ModuleUpdateError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;

    ModuleUpdateError(const char * what = "module update failed"):
        std::runtime_error(what)
    {
    }
};
