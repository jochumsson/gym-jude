#pragma once

#include "GuiState.h"

class IGuiStateObserver
{
public:
    virtual ~IGuiStateObserver() = default;

    virtual void on_gui_state_changed(const GuiState new_state) = 0;

};
