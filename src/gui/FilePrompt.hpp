#pragma once

#include "FileExplorer.hpp"
#include "ToolWindow.hpp"

#include <optional>

namespace GUI {

class FilePrompt : public ToolWindow {
public:
    explicit FilePrompt(sf::RenderWindow& wnd, sf::String help_text, sf::String window_title, sf::String placeholder);

    std::optional<sf::String> result() const { return m_result; }

    FileExplorer* file_explorer(){return m_file_explorer;}

private:
    std::optional<sf::String> m_result;
    FileExplorer* m_file_explorer;
};

FilePrompt* file_prompt(sf::String help_text, sf::String window_title = "Prompt", sf::String placeholder = "");

}
