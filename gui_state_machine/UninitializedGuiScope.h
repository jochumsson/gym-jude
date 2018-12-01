#pragma once
#include "GuiStateServer.h"

/**
 * @brief The UninitializedGuiScope class provides uninitialized guis scope
 *
 */
class UninitializedGuiScope
{
public:
    UninitializedGuiScope(GuiStateServer & state_observer, bool force_initialization = false):
        m_state_observer(state_observer),
        m_force_initialized(force_initialization)
    {
        if (m_state_observer.get_state() != GuiState::Uninitialized)
        {
            m_state_observer.change_state(GuiState::Uninitialized);
            m_is_recursive = false;
        }
    }

    ~UninitializedGuiScope()
    {
        if (m_force_initialized || not m_is_recursive)
        {
            m_state_observer.change_state(GuiState::Initialized);
        }
    }

private:
    GuiStateServer & m_state_observer;
    bool m_force_initialized;
    bool m_is_recursive = true;
};
