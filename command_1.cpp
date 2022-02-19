#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>
#include <format>
#include <functional>
namespace h {
    template <class T>
    inline auto find(std::string str, const std::string cut, T func) {
        for (auto pos = str.find(cut); pos != std::string::npos; pos = str.find(cut)) {
            func(str.substr(0, pos));
            str = str.substr(pos + cut.size());
        }
        return str;
    }
    inline auto findAll(std::string str, const std::string cut) {
        std::vector<int> data;
        int pos = 0;
        find(str, cut, [&](std::string str) {
            pos += str.size() + cut.size();
            data.push_back(pos);
            });
        return data;
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
    inline std::string wstringToString(const std::wstring str)noexcept(true) {
        const int BUFSIZE = WideCharToMultiByte(CP_OEMCP, 0, str.c_str(), -1, (char*)nullptr, 0, nullptr, nullptr);
        std::unique_ptr<char> wtext(new char[BUFSIZE]);
        WideCharToMultiByte(CP_OEMCP, 0, str.c_str(), -1, wtext.get(), BUFSIZE, nullptr, nullptr);
        return std::string(wtext.get(), wtext.get() + BUFSIZE - 1);
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
        virtual int up() {
            return 0;
        }
        virtual int down() {
            return 0;
        }
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
    class Console {
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
            range.Bottom = y + 1;
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
        inline auto sGetTitle() {
            CHAR title[MAX_PATH];
            auto bufSize = GetConsoleTitleA(title, MAX_PATH);
            return std::string(title, title + bufSize);
        }
        inline auto& sSetTitle(std::string str) {
            SetConsoleTitleA(str.c_str());
            return *this;
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
        inline std::wstring getLine(short width, short y, short startPos = 0) {
            DWORD p;
            std::unique_ptr<wchar_t> str(new wchar_t[width]);
            if (!ReadConsoleOutputCharacter(console, str.get(), width, { startPos,y += def }, &p))return L"";
            return std::wstring(str.get(), str.get() + p);
        }
        inline std::string sGetLine(short width, short y, short startPos = 0) {
            DWORD p;
            std::unique_ptr<char> str(new char[width]);
            if (!ReadConsoleOutputCharacterA(console, str.get(), width, { startPos,y += def }, &p))return "";
            return std::string(str.get(), str.get() + p);
        }
    };
    class CUI_Cmd :public InputManagerIntarface {
    protected:
        //get+cだと|入れれない
        std::string def, cmd;
    public:
        CUI_Cmd(std::string def) :def(def) {
            absolute();
        }
        auto getCmd() {
            return cmd;
        }
        bool esc() override{
            return true;
        }
        bool enter() override{
            return true;
        }
        bool backspace() override{
            auto byteSize =1+(x > 0 && IsDBCSLeadByte(cmd[x - 2]));
            cmd.erase(x-=byteSize,byteSize);
            return true;
        }
        bool insert(char c) override{
            if (!c)return true;
            cmd.insert(x, 1, c);
            ++x;
            return true;
        }
        int left() override{
            if (x <= 0)return x;
            return x-=(--x>0&&IsDBCSLeadByte(cmd[x-1]));
        }
        int right() override{
            if (x >= cmd.size())return x;
            return x+=1+IsDBCSLeadByte(cmd[x]);
        }
        void absolute() {//find>
            auto str = def + ">" + cmd;
            str.insert(x + def.size() + 1, "|");
            h::Console::getInstance().sSetTitle(str);
        }
    };
    class CUI_CmdLine:public CUI_Cmd {
    public:
        CUI_CmdLine(std::string def) :CUI_Cmd(def) {

        }
        bool enter()override {
            return cmd.empty();
        }
    };
    class CUI_Find :public InputManagerIntarface {
    private:
        int size;
        std::vector<std::pair<int,std::vector<int> > > data;
    public:
        void paint(bool color = false) {
            for (auto& [y, xs] :data) {
                for (auto x : xs) {
                    h::Console::getInstance().color(size, x-size, y,color);
                }
            }
        }
        CUI_Find(std::vector<int> data,std::string find):size(find.size()) {
            for (auto y = 0; auto width : data) {
                auto found = findAll(h::Console::getInstance().sGetLine(width, y), find);
                if (found.empty()) {
                    ++y;
                    continue;
                }
                this->data.emplace_back(y,found);
                ++y;
            }
            paint();
        }
        ~CUI_Find() {
            paint(true);
        }
        bool enter()override {
            return false;
        }
        bool esc()override {
            return false;
        }
        bool backspace()override {
            return false;
        }
        int up() override{
            return y -= y > 0;
        }
        int down()override {
            return y += y < data.size()-1;
        }
        int left() override{
            if (--x >= 0)return x;
            up();
            return x=data[y].second.size()-1;
        }
        int right() override{
            if (++x < data[y].second.size())return x;
            down();
            return x=0;
        }
        bool insert(char c){
            return false;
        }
        void absolute()override{
            h::Console::getInstance().move(data[y].second[x], data[y].first);
        }
        int getX()override {
            return data[y].second[x];
        }
        int getY()override {
            return data[y].first;
        }
    };
    class CUI_TextEditor :public InputManagerIntarface {
    private:
        std::vector<int> sizes;
        int max;
        bool flag;
        File file;
        enum OPTION {
            //EXE PATH
            FILE_NAME = 1,
            CODE_PAGE
        };
    public:
        ~CUI_TextEditor() {
            h::Console::getInstance().move(0, sizes.size());
        }
        template <class T>
        auto& resetOption(int argc, T argv, int hash = 0) {
            if (argc > FILE_NAME - hash) {
                file.setName(argv[FILE_NAME - hash]);
            }
            if (argc > CODE_PAGE - hash) {
                h::Console::getInstance().setCodePage(std::stoi(argv[CODE_PAGE - hash]));
            }
            return *this;
        }
        template <class T>
        auto & reset(int argc, T argv, int hash=0) {
            h::Console::getInstance()
                .addDefLine(sizes.size())
                .move(0,0);
            sizes.clear();
            resetOption(argc, argv, hash);
            std::cout << file.read().getContent();
            auto lines = h::split(file.getContent(), "\n");
            if (!lines.size()) {
                sizes.emplace_back(0);
                return *this;
            }
            max = std::max_element(lines.begin(), lines.end(), [](auto first, auto second) {
                return first.size() < second.size();
                })->size();
                h::Console::getInstance().move(0, 0).setScrollSize(max, lines.size());
                for (auto& line : lines) {
                    sizes.emplace_back(line.size());
                }
            return *this;
        }
        CUI_TextEditor(int argc, char* argv[]) :file("") {
            reset(argc,argv);
        }
        auto& updataSize() {
            if (!beBigger(max, sizes[y]))return *this;
            h::Console::getInstance().setScrollSize(max, sizes.size());
            return *this;
        }
        int left() override{
            return x -= (x > 0)+ IsDBCSLeadByte(h::Console::getInstance().sGetLine(2, y, x-2)[0]);//(a+=b)+=func(2,y,x-1)
        }
        int right() override{
            return x+= (sizes[y] > x)+ IsDBCSLeadByte(h::Console::getInstance().sGetLine(2, y, x)[0]);
        }
        inline int fitXY() {
            if (sizes[y] < x) {
                x = sizes[y];
            }
            x -= IsDBCSLeadByte(h::Console::getInstance().sGetLine(2, y, x - 1)[0]);
            return y;
        }
        int up() override{
            y-=y>0;
            return fitXY();
        }
        int down()override {
            y += sizes.size() - 1 > y;
            return fitXY();
        }
        auto getCmdLine(std::string def) {
            CUI_CmdLine cmd(def);
            InputManager(&cmd).input();
            return cmd.getCmd();
        }
        auto toWstring() {
            std::wstring str;
            for (auto line = 0; auto & size : sizes) {
                str += h::Console::getInstance().getLine(size, line) + L"\n";
                ++line;
            }
            return str;
        }
        bool esc()override {
            switch (_getch()) {
                break;
            case 's':
            {

                h::Console::getInstance().sSetTitle("saving->" + file.getName());
                file.wWrite(toWstring(), true);
                h::Console::getInstance().sSetTitle("saved->"+file.getName());
                /*
                file.write("",true);
                for(auto y=0;auto width:sizes){
                    file.write(h::console::getline(...));
                    console->settitle(format(saveing...y...sizes.size())))
                    ++y;
                }
                */
            }
                break;
            case 'f':
            {
                CUI_Find find(sizes,getCmdLine("find"));
                InputManager(&find).input();
                x = find.getX();
                y = find.getY();
                h::Console::getInstance().sSetTitle(std::format("found->({},{})", x, y));
            }

                break;
            case 'o':
            {
                auto cmdLine = split(getCmdLine("option"), " ");
                resetOption(cmdLine.size(), cmdLine, 1);
                h::Console::getInstance().sSetTitle(std::format("fileName->{} codePage->{}", file.getName(), h::Console::getInstance().getCodePage()));
            }
                break;
            case 'r':
            {
                auto cmdLine = split(getCmdLine("read"), " ");
                reset(cmdLine.size(), cmdLine, 1);
                h::Console::getInstance().sSetTitle("read->"+file.getName());
            }
                break;
            case 'q':
                return false;
                break;
            }
            return true;
        }
        bool enter()override{
            h::Console::getInstance().scroll(y + 1).appendUnderCopyLine(x, y);
            sizes.insert(std::next(sizes.begin(), 1+y), sizes[y] - x);
            sizes[y] = x;
            x = 0;
            ++y;
            return true;
        }
        bool backspace() override{
            if (!x) {
                if (!y)return true;
                x = sizes[--y];
                sizes[y] += sizes[y + 1];
                updataSize();
                h::Console::getInstance().appendAboveCopyLine(x, y).scroll(y + 1, true);
                sizes.erase(std::next(sizes.begin(), y + 1));//最後に空白あるところだけ管理or最後に|とかでマークつける
                return true;
            }
            auto byteSize =1+IsDBCSLeadByte(h::Console::getInstance().sGetLine(2, y, x - 2)[0]);
            h::Console::getInstance().around(x, y,true, byteSize);
            sizes[y]-=byteSize;
            x -= byteSize;
            return true;
        }
        bool insert(char c)override{
            if (c == 0)return true;
            h::Console::getInstance().around(x, y,false);
            std::cout << c;
            ++x;
            if (flag) {
                flag = false;
                ++x;
            }
            else if (IsDBCSLeadByte(c)) {
                h::Console::getInstance()
                    .around(x, y,false)
                    .around(x, y);
                --x;
                flag = true;
            }
            ++sizes[y];
            updataSize();
            return true;
        }
        void absolute()override {
            h::Console::getInstance().move(x, y);
        }

    };
}

int main(int argc,char* argv[]) {
    h::CUI_TextEditor editor(argc,argv);
    h::InputManager(&editor).input();
    return 0;
}
