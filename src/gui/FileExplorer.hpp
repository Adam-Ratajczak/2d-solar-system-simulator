#pragma once
#include "Container.hpp"
#include "ListView.hpp"
#include "Textbox.hpp"
#include "Widget.hpp"
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

namespace GUI {

class FileView : public Model {
public:
    virtual size_t row_count() const override {
        return m_content.size();
    }
    virtual size_t column_count() const override {
        return 4;
    }
    virtual std::string data(size_t row, size_t column) const override {
        return m_content[row][column];
    }
    virtual Column column(size_t column) const override {
        switch (column) {
        case 0:
            return { .width = 200, .name = "Name" };
        case 1:
            return { .width = 100, .name = "Size" };
        case 2:
            return { .width = 350, .name = "Modified" };
        case 3:
            return { .width = 100, .name = "File type" };
        }
        return {};
    }

    std::filesystem::path get_path(size_t row) const {
        return m_paths[row];
    }

    void update_content(
        std::filesystem::path path, std::function<bool(std::filesystem::path)> condition = [](std::filesystem::path path) { return true; }) {
        m_content.clear();
        m_paths.clear();

        for (const auto& o : std::filesystem::directory_iterator(path)) {
            if (!std::filesystem::exists(o) || !condition(o.path().filename()))
                continue;

            m_paths.push_back(o.path());

            std::time_t cftime = std::chrono::system_clock::to_time_t(
                std::chrono::file_clock::to_sys(o.last_write_time()));

            m_content.push_back(std::vector<std::string>(4));
            m_content.back()[0] = o.path().filename();
            m_content.back()[1] = (!std::filesystem::is_directory(o)) ? std::to_string(o.file_size()) : "";
            m_content.back()[2] = std::asctime(std::localtime(&cftime));
            m_content.back()[3] = o.path().extension();

            // for(const auto& e : m_content.back())
            //     std::cout << e << "\t";
            // std::cout << "\n";
        }

        std::sort(m_content.begin(), m_content.end(), [](const std::vector<std::string>& a, const std::vector<std::string>& b){
            if(a[1] == b[1])
                return a[0] < b[0];
            else{
                if(a[1].size() == 0)
                    return true;
                else if(b[1].size() == 0)
                    return false;
                return a[0] < b[0];
            }
        });

        std::sort(m_paths.begin(), m_paths.end(), [](const std::filesystem::path& a, const std::filesystem::path& b){
            if(std::filesystem::is_directory(a) == std::filesystem::is_directory(b))
                return a < b;
            else{
                if(std::filesystem::is_directory(a))
                    return true;
                else if(std::filesystem::is_directory(b))
                    return false;
            }

            return a < b;
        });
    }

private:
    std::vector<std::vector<std::string>> m_content;
    std::vector<std::filesystem::path> m_paths;
};

class FileExplorer : public Container {
public:
    explicit FileExplorer(Container& c);

private:
    void open_path(std::filesystem::path path);

    std::filesystem::path m_current_path;
    Textbox* m_path_textbox {};
    FileView* m_model {};
};

}
