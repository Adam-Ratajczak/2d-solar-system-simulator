#pragma once
#include "Container.hpp"
#include "ListView.hpp"
#include "Widget.hpp"
#include <filesystem>
#include <memory>

namespace GUI {

class FileExplorer : public Container{
public:
    explicit FileExplorer(Container& c);

private:
    std::filesystem::path m_current_path;
    ListView* m_list;
};

}
