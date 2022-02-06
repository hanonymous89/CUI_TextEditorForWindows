#include <windows.h>
#include <iostream>
#include <iterator>
#include <string>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
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
    template <class T,class CT>
    inline auto beBigger(T& value, CT second) {
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

    //class TextEditor {
    //protected:
    //    std::vector<std::string> data;
    //public:
    //    TextEditor(std::string data) {
    //        this->data = split(data, "\n");
    //    }
    //    auto& insert(int line, int column, char replace) {
    //        if (checkOutOfRange(data, line) || checkOutOfRange(data[line], column)) {
    //            return *this;
    //        }
    //        data[line].insert(column, 1, replace);
    //        return *this;
    //    }
    //    auto& erase(int line, int column) {
    //        if (checkOutOfRange(data, line) || checkOutOfRange(data[line], column)) {
    //            return *this;
    //        }
    //        data[line].erase(column, 1);
    //        return *this;
    //    }
    //    auto toString() {
    //        std::stringstream ss;
    //        std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(ss, "\n"));
    //        return ss.str();
    //    }
    //};

    class Point {
    protected:
        int x=0, y=0;
    public:
        virtual ~Point(){}
        virtual int up() = 0;
        virtual int down() = 0;
        virtual int left() = 0;
        virtual int right() = 0;
        virtual int getX() {
            return x;
        }
        virtual int getY() {
            return y;
        }
    };
    class InputManagerIntarface :public Point{
    public:
        virtual bool enter() = 0;
        virtual bool backspace() = 0;
        virtual bool insert(char) = 0;
        virtual bool esc() = 0;
        virtual void absolute() = 0;
    };
    class TextEditorPos:public Point{
    private:
        std::vector<std::string> data;
        int  maxSize;
    public:
        TextEditorPos(std::string data) {
            this->data = split(data, "\n");
            if (data.empty()) {
                this->data.push_back("");
            }
            maxSize = std::max_element(this->data.begin(), this->data.end(), [](std::string first, std::string second) {return first.size() < second.size(); })->size();
        }
        auto toString() {
            std::stringstream ss;
            std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(ss,"\n"));
            return ss.str();
        }
        int up() override{
            if (y <= 0)return y;
            --y;
            if (x && data[y].size() <= x) {
                x = data[y].size() - 1;
            }
            if (x > 0 && IsDBCSLeadByte(data[y][x - 1])) {
                --x;
            }
            return y;
        }

        int down()override {
            if (data.size() <= y + 1)return y;
            ++y;
            if (x && data[y].size() <= x) {
                x = data[y].size() - 1;
            }
            if (x > 0 && IsDBCSLeadByte(data[y][x - 1])) {
                --x;
            }
            return y;
        }

        int left() override{
            if (x > 0) {
                --x;
                return x-= (x > 0 && IsDBCSLeadByte(data[y][x-1]));
            }
            up();
            x = data[y].size();
            return x;
        }

        int right() override{
            if (x < data[y].size()) {
                x += IsDBCSLeadByte(data[y][x]);
                return ++x;
            }
            down();
            x = 0;
            return x;
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
            else if (not checkOutOfRange(data, y) && not checkOutOfRange(data[y], x)) {
                    data[y].insert(x, 1, replace);
            }
            right();
            beBigger(maxSize, data[y].size());
            return data[y].substr(x - 1);
        }
        std::string backspace() {
            if (x <= 0) {
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
            else if (not checkOutOfRange(data, y) && not checkOutOfRange(data[y], x)) {
                data[y].erase(x, 1);
            }
            return data[y].substr(x - (x > 0));
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
    public:
        auto& setScrollSize(short width, short height) {
            if (beBigger(this->width, width) | beBigger(this->height, height)) {
                SetConsoleScreenBufferSize(console, { this->width,this->height });
            }
            return *this;
        }

    private:
        HANDLE console;
        short width, height;
        Console() {
            CONSOLE_SCREEN_BUFFER_INFO info;
            console = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfo(console, &info);
            height = info.dwMaximumWindowSize.Y;
        }
        virtual ~Console() {

        }
    public:
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;
        Console& operator=(Console&&) = delete;
        Console(Console&&) = delete;
        static auto& getInstance() {
            static Console console;
            return console;
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
        inline auto getCodePage() {
            return GetConsoleOutputCP();
        }
        inline auto& setCodePage(UINT codePage) {
            SetConsoleOutputCP(codePage);
            SetConsoleCP(codePage);
            return *this;
        }
        inline auto getTitle() {
            WCHAR title[MAX_PATH];
            auto bufSize = GetConsoleTitle(title, MAX_PATH);
            return std::wstring(title, title + bufSize);
        }
        inline auto& setTitle(std::wstring str) {
            SetConsoleTitle(str.c_str());
            return *this;
        }
    };
    class InputManager {
    private:
        InputManagerIntarface* p;
        //std::unordered_map<int, void(*)()> func;
    public:
        InputManager(InputManagerIntarface* p) {
            this->p = p;
        }
        void input(std::function<void()> absolute/*=[] {}*/) {
            if (p == nullptr)return;
            bool loop = true;
            while (loop) {
                bool hit = true;
                auto c = _getch();
                switch (c) {
                case 0x1b:
                    loop = p->esc();
                    break;
                case 0x0d:
                    loop = p->enter();
                    break;
                case 8:
                    loop = p->backspace();
                    break;
                default:
                    hit = false;
                }
                if (hit) {
                    absolute();
                    continue;
                }
                if (!hit && c != 224) {
                    loop = p->insert(c);
                    absolute();
                    continue;
                }
                switch (_getch()) {
                case 0x48:
                    p->up();
                    break;
                case 0x50:
                    p->down();
                    break;
                case 0x4b:
                    p->left();
                    break;
                case 0x4d:
                    p->right();
                    break;
                }
                absolute();
            }
        }
        void input() {
            if (p == nullptr)return;
            bool loop = true;
            while (loop) {
                bool hit = true;
                auto c = _getch();
                switch (c) {
                case 0x1b:
                    loop = p->esc();
                    break;
                case 0x0d:
                    loop = p->enter();
                    break;
                case 8:
                    loop = p->backspace();
                    break;
                default:
                    hit = false;
                }
                if (hit) {
                    p->absolute();
                    continue;
                }
                if (!hit && c != 224) {
                    loop = p->insert(c);
                    p->absolute();
                    continue;
                }
                switch (_getch()) {
                case 0x48:
                    p->up();
                    break;
                case 0x50:
                    p->down();
                    break;
                case 0x4b:
                    p->left();
                    break;
                case 0x4d:
                    p->right();
                    break;
                }
                p->absolute();
            }
        }
    };
    class InputCmd :public InputManagerIntarface{//インターフェイスにしてもいいかも setExe(key,funcValue())
    private:
        std::string cmd,def;
    public:
        InputCmd(std::string def):def(def) {

        }
        auto getCmd() {
            return cmd;
        }
        auto toString() {
            auto show = def + ">" + cmd;
            show.insert(def.size() + x+1, "|");
            return show;
        }
        int left(){
            if (x >0) {
                --x;
                
                //x -= (x > 0 && IsDBCSLeadByte(cmd[x - 1]));
            }
            return x;
        }
        int right()  {
            if (x <cmd.size()) {
                x += IsDBCSLeadByte(cmd[x]);
                ++x;
            }
            return x;
        }
        auto &setDefault(std::string def){
            this->def = def;//考える必要あり
            return *this;
        }
        
        bool insert(char c) override{
            cmd.insert(x, 1, c);
            right();
            return true;
        }
        bool backspace() override{
            left();
            if (IsDBCSLeadByte(cmd[x])) {
                cmd.erase(x, 2);
            }
            else{
                cmd.erase(x, 1);
            }
            return true;
         }
         bool enter() override{
             return cmd.empty();
         }
         void absolute()override {}
         int up() { return 0; }
         int down() { return 0; }
         bool esc()override{
             return true;
         }

    };
    class Vim:public InputManagerIntarface {
    private:
        File file;
        TextEditorPos editor;
    public:
        Vim(int argc,char *argv[]):file(argc>1?argv[1]:""), editor(file.read().getContent()) {
            system("cls");
            h::Console::getInstance().setScrollSize(editor.getMax(), editor.getHeight() + 1);
            if (argc > 2) {
                h::Console::getInstance().setCodePage(std::stoi(argv[2]));
            }
            std::cout << editor.toString();
            h::Console::getInstance().move(0, 0);
        }
        bool esc()override {
            switch (_getch()) {
            case 's':
                if (file.getName().empty()) {
                    h::InputCmd cmd("filename");
                    h::Console::getInstance().setTitle(h::stringToWstring(cmd.toString()));
                    h::InputManager(&cmd).input([&] {
                        h::Console::getInstance().setTitle(h::stringToWstring(cmd.toString()));
                        });
                    file.setName(cmd.getCmd());
                }
                file.write(editor.toString(), true);
                break;
            case 'c'://cmd
                h::Console::getInstance().move(0, editor.getHeight());
                *std::istream_iterator<std::string>(std::cin);
                break;
            case 'e':
                return false;
                break;
            }
            return true;
        }
        bool enter() override{
            h::Console::getInstance().setScrollSize(editor.getMax(), editor.getHeight());
            h::Console::getInstance().scroll(editor.getY() + 1);
            std::cout << editor.enter();
            return true;
        }
        bool backspace() override {
            auto length = editor.getY();
            h::Console::getInstance().move(editor.getX() - 2, editor.getY());
            auto show = editor.backspace();
            if (!editor.getX())h::Console::getInstance().move(editor.getX(), editor.getY());
            if (editor.getY() == length)
                std::cout << show << "  ";
            else {
                h::Console::getInstance().move(editor.getX(), editor.getY());
                std::cout << show;
                h::Console::getInstance().scroll(length, true);
            }
            return true;
        }
        bool insert(char c) override{
            std::cout << editor.insert(c);
            h::Console::getInstance().setScrollSize(editor.getMax() + 1, editor.getHeight());
            return true;
        }
        void absolute()override {
            h::Console::getInstance().move(editor.getX(), editor.getY());
        }
        int up()override {
            return editor.up();//同じ処理が多い
        }

        int down()override {
            return editor.down();
        }

        int left()override {
            return editor.left();
        }

        int right()override {
            return editor.right();
        }
    };

}
int main(int argc, char* argv[]) {
    h::Vim vim(argc, argv);
    h::InputManager(&vim).input();//manager(h::Vim(...)) error
    return 0;
}