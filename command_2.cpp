#include <windows.h>
#include <iostream>
#include <iterator>
#include <string>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
namespace h {
    inline auto split(std::wstring str, const std::wstring cut) noexcept(false) {
        std::vector<std::wstring> data;
        for (auto pos = str.find(cut); pos != std::string::npos; pos = str.find(cut)) {
            data.push_back(str.substr(0, pos));
            str = str.substr(pos + cut.size());
        }
        if (!str.empty())data.push_back(str);
        return data;
    }
    inline auto split(std::string str, const std::string cut) noexcept(false) {
        std::vector<std::string> data;
        for (auto pos = str.find(cut); pos != std::string::npos; pos = str.find(cut)) {
            data.push_back(str.substr(0, pos));
            str = str.substr(pos + cut.size());
        }
        if (!str.empty())data.push_back(str);
        return data;
    }
    template <class T>
    concept haveSizeFunc = requires(T & obj) {
        obj.size();
    };
    template <haveSizeFunc T>
    inline auto checkOutOfRange(const T arr, const size_t index) {
        return 0 > index or arr.size() <= index;
    }
    template <class T>
    inline auto beBigger(T& value, T second) {
        if (value < second) {
            value = second;
            return true;
        }
        return false;
    }
    inline std::wstring stringToWstring(const std::string str)noexcept(true) {
        const int BUFSIZE = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, (wchar_t*)NULL, 0);
        std::unique_ptr<wchar_t> wtext(new wchar_t[BUFSIZE]);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wtext.get(), BUFSIZE);
        return std::wstring(wtext.get(), wtext.get() + BUFSIZE - 1);
    }
    class File {
    private:
        std::string name,
            content;
    public:
        inline File(const std::string name)noexcept(true) :name(name) {
            read();
        }
        inline auto& setName(const std::string name)noexcept(true) {
            this->name = name;
            return *this;
        }
        inline auto& getName() {
            return name;
        }
        inline auto& getContent()const noexcept(true) {
            return content;
        }
        inline File& read() noexcept(false) {
            std::fstream file(name);
            if (!file.is_open())return *this;
            content = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
            return *this;
        }
        inline auto write(const std::string str, const bool reset = false) const noexcept(false) {
            std::ofstream file(name, reset ? std::ios_base::trunc : std::ios_base::app);
            file << str;
            file.close();
            return *this;
        }
        inline  auto& replace(const std::string data, const bool reset = false)const noexcept(false) {
            std::ofstream file(name, reset ? std::ios_base::out : std::ios_base::app);
            if (file.fail())return *this;
            file << data;
            file.close();
            return *this;
        }
    };

    class TextEditor {
    protected:
        std::vector<std::string> data;
    public:
        TextEditor(std::string data) {
            this->data = split(data, "\n");
        }
        auto& insert(int line, int column, char replace) {
            if (checkOutOfRange(data, line) || checkOutOfRange(data[line], column)) {
                return *this;
            }
            data[line].insert(column, 1, replace);
            return *this;
        }
        auto& erase(int line, int column) {
            if (checkOutOfRange(data, line) || checkOutOfRange(data[line], column)) {
                return *this;
            }
            data[line].erase(column, 1);
            return *this;
        }
        auto toString() {
            std::stringstream ss;
            std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(ss, "\n"));
            return ss.str();
        }
    };
    class TextEditorPos :private TextEditor {
    private:
        int x, y, maxSize;
    public:
        TextEditorPos(std::string data) :TextEditor(data) {
            if (data.empty()) {
                this->data.push_back("");
            }//縦の長さが足りないとバグ
            maxSize = std::max_element(this->data.begin(), this->data.end(), [](std::string first, std::string second) {return first.size() < second.size(); })->size();
        }

        auto toString() {
            return TextEditor::toString();
        }
        auto& up() {
            if (y <= 0)return *this;
            --y;
            if (x&&data[y].size() <= x) {
                x = data[y].size() - 1;
            }
            if (x>0&&IsDBCSLeadByte(data[y][x-1])) {
                --x;
            }
            return *this;
        }

        auto& down() {
            if (data.size() <= y + 1)return *this;
            ++y;
            if (x&&data[y].size() <= x) {
                x = data[y].size() - 1;
            }
            if (x > 0 && IsDBCSLeadByte(data[y][x - 1])) {
                --x;
            }
            return *this;
        }

        auto& left() {
            if (x > 0) {
                --x;
                x -= (x > 0 && IsDBCSLeadByte(data[y][x - 1]));
                return *this;
            }
            up();
            x = data[y].size();
            return *this;
        }

        auto& right() {
            //x += (x < data[y].size()) + IsDBCSLeadByte(data[y][x]);
            if (x < data[y].size()) {
                x += IsDBCSLeadByte(data[y][x]);
                ++x;
                return *this;
            }
            down();
            x = 0;
            return *this;
        }
        auto getX() {
            return x;
        }
        auto getY() {
            return y;
        }
        auto getMax() {
            return maxSize;
        }
        auto getHeight() {
            return data.size();
        }
        std::string insert(char replace) {
            if (x >= data[y].size()) {
                data[y] += replace;
            }
            else
                TextEditor::insert(y, x, replace);
            right();
            if (maxSize < data[y].size())maxSize = data[y].size();
            return data[y].substr(x - 1);
        }
        std::string backspace() {
            if (x<=0) {
                if (!y)return data[y];
                auto show = data[y];
                left();
                data[y] += show;
                data.erase(std::next(data.begin(), y + 1));
                return show;
            }
            left();
            if (IsDBCSLeadByte(data[y][x])) {
                data[y].erase(x, 2);
                return data[y].substr(x);
            }
            TextEditor::erase(y, x);
            return data[y].substr(x-(x>0));
        }
        std::string enter() {
            auto length = data[y].size() - x;
            data.insert(std::next(data.begin(), y + 1), data[y].substr(x));
            data[y] = data[y].substr(0, x);
            ++y;
            x = 0;
            return std::string(length, ' ') + "\n" + data[y];
        }
        auto& get() {
            return data;
        }
    };
    class Console {
    private:
        HANDLE console;
        short width, height;
    public:
        auto& setScrollSize(short width, short height) {
            if (beBigger(this->width, width) | beBigger(this->height, height)) {
                SetConsoleScreenBufferSize(console, { this->width,this->height });
            }
            return *this;
        }
        Console(short width,short height) {
            CONSOLE_SCREEN_BUFFER_INFO info;
            console = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfo(console, &info);
            beBigger(height, info.dwMaximumWindowSize.Y);
            setScrollSize(width,height);
        }
        auto& move(short x, short y) {
            SetConsoleCursorPosition(console, { x,y });
            return *this;
        }

        auto& scroll(short line, bool up = false) {
            CHAR_INFO info;
            SMALL_RECT range;
            range.Left = 0;
            range.Right = width;
            range.Top = line + up;
            range.Bottom = height + up;
            info.Attributes = 0;
            info.Char.AsciiChar = ' ';
            line += !up;
            ScrollConsoleScreenBuffer(console, &range, nullptr, { 0,line }, &info);
            return *this;
        }
    };
}
int main(int argc, char* argv[]) {
    system("cls");
    h::File file("");
    if (argc > 1) {
        file.setName(argv[1]);
    }
    if (argc > 2) {
        auto codePage = std::stoi(argv[2]);
        SetConsoleOutputCP(codePage);
        SetConsoleCP(codePage);
    }
    h::TextEditorPos editor(file.read().getContent());
    h::Console console(editor.getMax(), editor.getHeight()+1);
    std::cout <<editor.toString();
    console.move(0, 0);
    while (true) {
        auto c = _getch();
        if (c == 0x1b) {
            switch (_getch()) {
            case 's':
                if (file.getName().empty()) {
                    console.move(0, editor.getHeight());
                    std::cout << "filename:";
                    file.setName(*std::istream_iterator<std::string>(std::cin));
                }
                file.write(editor.toString(), true);
                break;
            case 'c'://cmd
                console.move(0, editor.getHeight());
                *std::istream_iterator<std::string>(std::cin);
                break;
            case 'e':
                return 0;
                break;
            }

            continue;
        }
        else if (c == 0x0d) {
            console.setScrollSize(editor.getMax(), editor.getHeight());
            console.scroll(editor.getY() + 1);
            std::cout << editor.enter();

        }
        else if (c == 8) {
            auto length = editor.getY();
            console.move(editor.getX() - 2, editor.getY());
            auto show = editor.backspace();
            if (editor.getY() == length)
                std::cout << show << "  ";
            else {
                console.move(editor.getX(), editor.getY());
                std::cout << show;
                console.scroll(length, true);
            }
        }
        else if (c != 224) {
            std::cout << editor.insert(c);
            console.setScrollSize(editor.getMax() + 1, editor.getHeight());
        }
        else {
            switch (_getch()) {
            case 0x48:
                editor.up();
                break;
            case 0x50:
                editor.down();
                break;
            case 0x4b:
                editor.left();
                break;
            case 0x4d:
                editor.right();
                break;
            }
        }
        console.move(editor.getX(), editor.getY());
    }

    return 0;
}
