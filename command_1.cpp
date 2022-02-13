#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>
namespace h {
    //int maxDistanceLine(std::string str,std::string find="\n") {//splitしてからalgorithmでやるのがいいかもー(遅そうだから
    //    int max;
    //    while (true) {
    //        auto found = str.find(find);
    //        if (found == std::string::npos) {
    //            return str.size();
    //        }
    //    }
    //}
    template <class T>
    inline auto find(std::string str, const std::string cut, T func) {
        for (auto pos = str.find(cut); pos != std::string::npos; pos = str.find(cut)) {
            func(str.substr(0, pos));
            str = str.substr(pos + cut.size());
        }
        return str;
    }
    inline auto split(std::string str, const std::string cut) noexcept(false) {
        std::vector<std::string> data;
        str = find(str, cut, [&](std::string str) {
            data.push_back(str);
            });
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
    template <class T, class CT>
    inline auto beBigger(T& value, CT second) {
        if (value < second) {
            value = second;
            return true;
        }
        return false;
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
        inline auto wWrite(const std::wstring str, const bool reset = false) const noexcept(false) {
            std::wofstream file(name, reset ? std::ios_base::trunc : std::ios_base::app);
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
    class Point {
    protected:
        int x = 0, y = 0;
    public:
        virtual ~Point() {}//純仮想関数じゃなくてもいいかも
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
    class InputManagerIntarface :public Point {
    public:
        virtual bool enter() = 0;
        virtual bool backspace() = 0;
        virtual bool insert(char) = 0;
        virtual bool esc() = 0;
        virtual void absolute() {};
    };
    class InputManager {
    private:
        InputManagerIntarface* p;
        void inputBase(std::function<void()> absolute) {
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
    public:
        InputManager(InputManagerIntarface* p) :p(p) {

        }
        void input(std::function<void()> absolute) {
            inputBase(absolute);
        }
        void input() {
            inputBase(std::bind(&InputManagerIntarface::absolute, p));
        }
    };
    class Console{
    public:
        auto& setScrollSize(short width, short height) {
            if (beBigger(this->width, width) | beBigger(this->height, def + height)) {
                SetConsoleScreenBufferSize(console, { this->width,this->height });
            }
            return *this;
        }

    private:
        HANDLE console;
        WORD defColor;
        short width, height, def;
        Console() {
            CONSOLE_SCREEN_BUFFER_INFO info;
            console = GetStdHandle(STD_OUTPUT_HANDLE);
            GetConsoleScreenBufferInfo(console, &info);
            defColor = info.wAttributes;
            def = info.dwCursorPosition.Y;
            height = info.dwMaximumWindowSize.Y;
        }
        virtual ~Console() {

        }
    public:
        auto& addDefLine(decltype(def) def) {
            this->def += def;
            return *this;
        }
        Console(const Console&) = delete;
        Console& operator=(const Console&) = delete;
        Console& operator=(Console&&) = delete;
        Console(Console&&) = delete;
        static auto& getInstance() {
            static Console console;
            return console;
        }
        auto& move(short x, short y) {
            SetConsoleCursorPosition(console, { x,short(def + y) });
            return *this;
        }

        auto& scroll(short line, bool up = false) {
            line += def;
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
        auto& around(short x, short y, bool left = true, short width = 1) {
            y += def;
            CHAR_INFO info;
            SMALL_RECT range;
            range.Left = x;
            range.Right = this->width;
            range.Top = y;
            range.Bottom = y;
            info.Attributes = 0;
            info.Char.AsciiChar = ' ';
            x += width - (width * 2) * (left);
            ScrollConsoleScreenBuffer(console, &range, nullptr, { x,y }, &info);
            return *this;
        }
        auto& appendAboveCopyLine(short x, short y) {
            y += def;
            CHAR_INFO info;
            SMALL_RECT range, clip;
            range.Left = 0;
            range.Right = this->width;
            range.Top = y;
            range.Bottom = y+1;
            info.Attributes = 0;
            info.Char.AsciiChar = ' ';
            clip.Left = x;
            clip.Right = this->width;
            clip.Bottom = y;
            clip.Top = y;
            ScrollConsoleScreenBuffer(console, &range, &clip, { x,--y }, &info);
            return *this;
        }
        auto& appendUnderCopyLine(short x, short y) {
            y += def;
            CHAR_INFO info;
            SMALL_RECT range;
            range.Left = x;
            range.Right = this->width;
            range.Top = y;
            range.Bottom = y;
            info.Attributes = 0;
            info.Char.AsciiChar = ' ';
            ScrollConsoleScreenBuffer(console, &range, nullptr, { 0,++y }, &info);
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
        inline auto& color(DWORD length, short x, short y, bool def = true) {
            FillConsoleOutputAttribute(console, def ? defColor : ~defColor, length, { x,y += this->def }, &length);
            return *this;
        }
        inline auto& setColor(WORD color, DWORD length, short x, short y) {
            FillConsoleOutputAttribute(console, color, length, { x,y += def }, &length);
            return *this;
        }
        //inline std::wstring getLine(short y) {
        //    DWORD p;
        //    std::unique_ptr<wchar_t> str(new wchar_t[width]);
        //    if(!ReadConsoleOutputCharacter(console, str.get(), width, { 0,y+=def }, &p))return L"";
        //    return std::wstring(str.get(),str.get()+p);
        //}
        inline std::wstring getLine(short width,short y) {
            DWORD p;
            std::unique_ptr<wchar_t> str(new wchar_t[width]);
            if (!ReadConsoleOutputCharacter(console, str.get(), width, { 0,y += def }, &p))return L"";
            return std::wstring(str.get(), str.get() + p);
        }
    };
    class CUI_TextEditor:public InputManagerIntarface{
    private:
        std::vector<int> sizes;
        int max;
    public:
        ~CUI_TextEditor() {
            h::Console::getInstance().move(0,sizes.size());
        }
        CUI_TextEditor(std::string text) {
            h::Console::getInstance();
            std::cout << text;
            //x,y static dummy=
            auto lines = h::split(text, "\n");
            if (!lines.size()) {
                sizes.emplace_back(0);
                return;
            }
            max=std::max_element(lines.begin(), lines.end(), [](auto first, auto second) {
                return first.size() < second.size();
                })->size();
            h::Console::getInstance().move(0, 0).setScrollSize(max,lines.size());
            for (auto& line : lines) {
                sizes.emplace_back(line.size());
            }
            //std::copy(sizes.begin(), sizes.end(), sizes.begin(), [](std::string str) {return str.size(); });ドラフトに入ってくれー
        }
        auto& updataSize() {
            if (!beBigger(max, sizes[y]))return *this;
            h::Console::getInstance().setScrollSize(max, sizes.size());
            return *this;
        }
        int left() override{
            return x-=x>0;
        }
        int right() override{
            return x+= sizes[y] > x;
        }
        int fitY() {
            if (sizes[y] < x) {
                x = sizes[y];
            }
            return y;
        }
        int up() override{
            y-=y>0;
            return fitY();
        }
        int down()override {
            y += sizes.size() - 1 > y;
            return fitY();
        }
        bool esc()override {
            return false;
        }
        bool enter()override{
            h::Console::getInstance().scroll(y + 1).appendUnderCopyLine(x, y);
            sizes.insert(std::next(sizes.begin(), y), sizes[y] - x);
            sizes[y] = x;
            x = 0;
            ++y;
            return true;
        }
        bool backspace() override{
            if (!x) {
                if (!y)return true;
                x = sizes[--y];
                sizes[y] += sizes[y+1];
                updataSize();
                h::Console::getInstance().appendAboveCopyLine(x, y).scroll(y+1, true);
                sizes.erase(std::next(sizes.begin(),y+1));//最後に空白あるところだけ管理
                return true;
            }
            h::Console::getInstance().around(x, y, true);
            --sizes[y];
            left();
            return true;
        }
        bool insert(char c)override{
            h::Console::getInstance().around(x, y, false);
            std::cout << c;
            ++x;
            ++sizes[y];
            updataSize();
            return true;
        }
        void absolute()override {
            h::Console::getInstance().move(x, y);
        }
        auto toWstring() {
            std::wstring str;
            for (auto line=0; auto & size : sizes) {
                str += h::Console::getInstance().getLine(size, line)+L"\n";
                //str+="\n"のほうが早い
                ++line;
            }
            return str;
        }
    };
    
}

int main(int argc,char* argv[]) {
    enum OPTION{
        FILE_NAME=1,
        CODE_PAGE
    };
    h::File file("");
    if (argc >OPTION::FILE_NAME) {
        file.setName(argv[OPTION::FILE_NAME]);
    }
    if (argc > OPTION::CODE_PAGE) {
        h::Console::getInstance().setCodePage(std::stoi(argv[OPTION::CODE_PAGE]));
    }
    h::CUI_TextEditor editor(file.read().getContent());
    h::InputManager(&editor).input();
    file.wWrite(editor.toWstring(),true);
    return 0;
}
