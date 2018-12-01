#pragma once

#include "IGuiStateObserver.h"
#include <vector>
#include <memory>

class GuiStateServer
{
public:
    GuiStateServer();

    void change_state(const GuiState new_state);

    GuiState get_state() const;

    void register_observer(std::weak_ptr<IGuiStateObserver> observer);

private:
    GuiState m_current_state = GuiState::Uninitialized;
    std::vector<std::weak_ptr<IGuiStateObserver>> m_state_observers;

};
