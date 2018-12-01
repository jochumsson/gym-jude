#include "GuiStateServer.h"

GuiStateServer::GuiStateServer()
{
}

void GuiStateServer::change_state(const GuiState new_state)
{
    if (new_state != m_current_state)
    {
        m_current_state = new_state;
        for(auto it: m_state_observers)
        {
            if (auto observer = it.lock())
            {
                observer->on_gui_state_changed(new_state);
            }
        }
    }
}

GuiState GuiStateServer::get_state() const
{
    return m_current_state;
}

void GuiStateServer::register_observer(std::weak_ptr<IGuiStateObserver> observer)
{
    m_state_observers.push_back(observer);
}

