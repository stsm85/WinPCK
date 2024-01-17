// Copyright(c) 2015-present, Gabi Melman, mguludag and spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

//
// Custom sink for QPlainTextEdit or QTextEdit and its childs(QTextBrowser...
// etc) Building and using requires Qt library.
//
// Warning: the qt_sink won't be notified if the target widget is destroyed.
// If the widget's lifetime can be shorter than the logger's one, you should provide some permanent QObject,
// and then use a standard signal/slot.
//

#include "spdlog/common.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/details/synchronous_factory.h"
#include "spdlog/sinks/base_sink.h"
#include <array>

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include <Richedit.h>

//
// winedit_sink class
//
namespace spdlog {
namespace sinks {
#if 1
template<typename Mutex>
class winedit_sink : public base_sink<Mutex>
{
public:
    winedit_sink(HWND _edit, int max_lines)
        : m_edit(_edit)
        , max_lines_(max_lines)
    {
        if (!this->m_edit)
        {
            throw_spdlog_ex("winedit_sink: hwnd is null");
        }

        this->lines_per_delete = this->max_lines_ / 10;
        if (0 == this->lines_per_delete)
            this->lines_per_delete = 1;

    }

    ~winedit_sink()
    {
        this->flush_();
    }

protected:

    inline LRESULT SendMsgA(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        return ::SendMessageA(this->m_edit, Msg, wParam, lParam);
    }

    inline LRESULT SendMsgW(UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        return ::SendMessageW(this->m_edit, Msg, wParam, lParam);
    }

    inline uint32_t GetLineCount()
    {
        return this->SendMsgA(EM_GETLINECOUNT, 0, 0);
    }

    inline void SetSelLine(int start, int end)
    {
        int lineStart = this->SendMsgA(EM_LINEINDEX, start, 0); //this->LineIndex(start);//取第一行的第一个字符的索引
        int lineEnd = this->SendMsgA(EM_LINEINDEX, end + 1, 0); //this->LineIndex(end + 1) /*- 1*/;//取第一行的最后一个字符的索引——用第二行的第一个索引减1来实现
        this->SendMsgA(EM_SETSEL, lineStart, lineEnd);//选取第一行字符
    }

    inline void DeleteLine(int start, int end)
    {
        this->SetSelLine(start, end);
        this->SendMsgA(EM_REPLACESEL, 0, (LPARAM)"");
    }

#if defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
    inline void AppendText(const char* text)
    {
        auto scroll = this->IsVScrollOnBottom();
        this->SendMsgA(EM_SETSEL, -2, -1);
        SETTEXTEX SetTxtEx{ .flags = ST_SELECTION | ST_UNICODE,.codepage = CP_UTF8 };
        this->SendMsgW(EM_SETTEXTEX, (WPARAM)&SetTxtEx, (LPARAM)text);
        if (scroll)
            this->SendMsgA(WM_VSCROLL, SB_BOTTOM, 0);
    }
#else
    inline void AppendText(const char* text)
    {
        auto scroll = this->IsVScrollOnBottom();
        this->SendMsgA(EM_SETSEL, -2, -1);
        this->SendMsgA(EM_REPLACESEL, 0, (LPARAM)text);
        if (scroll)
            this->SendMsgA(WM_VSCROLL, SB_BOTTOM, 0);
    }
#endif
    inline BOOL IsVScrollOnBottom()
    {
        SCROLLINFO si{
        .cbSize = sizeof(SCROLLINFO),
        .fMask = SIF_ALL
        };

        if (!::GetScrollInfo(this->m_edit, SB_VERT, &si))
            return TRUE;

		//加2是为了滚动条正好差1-2行时不滚动的问题
        if ((si.nPos + si.nPage / 8) >= (si.nMax - si.nPage))
            return TRUE;

        return FALSE;
    }

    virtual void sink_it_(const details::log_msg &msg) override
    {

        while (this->GetLineCount() > this->max_lines_)
        {
            this->DeleteLine(0, this->lines_per_delete);//删除10行
        }

        this->set_line_color(msg.level);

        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);

        formatted.push_back('\0'); // add a null terminator for OutputDebugString
//#    if defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
//        wmemory_buf_t wformatted;
//        details::os::utf8_to_wstrbuf(string_view_t(formatted.data(), formatted.size()), wformatted);
//        this->AppendText(wformatted.data());
//#    else
//        this->AppendText(formatted.data());
//#    endif
        this->AppendText(formatted.data());
    }

    virtual void flush_() override {}

    virtual void set_line_color(level::level_enum level) {};

private:

    HWND m_edit = nullptr;
    int max_lines_;
    int lines_per_delete;
};
#endif

template<typename Mutex>
class winedit_color_sink : public winedit_sink<Mutex>
{
public:
    winedit_color_sink(HWND _text_edit, int max_lines)
        : winedit_sink<Mutex>(_text_edit, max_lines)
    {
        this->init_richedit_color();
    }

    ~winedit_color_sink()
    {
        //flush_();
    }

    inline void init_richedit_color()
    {
        memset(&default_color_, 0, sizeof(CHARFORMAT2W));
        default_color_.cbSize = sizeof(CHARFORMAT2W);
        default_color_.dwMask = CFM_ALL;
        this->SendMsgW(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&default_color_);

        // set colors
        CHARFORMAT2W format{ 0 };
        format.cbSize = sizeof(CHARFORMAT2W);
        format.dwMask = CFM_COLOR;

        constexpr COLORREF Gray = RGB(128, 128, 128);
        constexpr COLORREF RoyalBlue = RGB(65, 105, 225);
        constexpr COLORREF Green = RGB(0, 128, 0);
        constexpr COLORREF DarkOrange = RGB(255, 140, 0);
        constexpr COLORREF Red = RGB(255, 0, 0);
        constexpr COLORREF White = RGB(255, 255, 255);

        // trace
        format.crTextColor = Gray;
        colors_.at(level::trace) = format;
        // debug
        format.crTextColor = RoyalBlue;
        colors_.at(level::debug) = format;
        // info
        format.crTextColor = Green;
        colors_.at(level::info) = format;
        // warn
        format.crTextColor = DarkOrange;
        colors_.at(level::warn) = format;
        // err
        format.crTextColor = Red;
        format.dwMask = CFM_BOLD | CFM_COLOR;
        format.dwEffects = CFE_BOLD;
        colors_.at(level::err) = format;
        // critical
        format.dwMask = CFM_BACKCOLOR | CFM_COLOR | CFM_BOLD;
        format.crTextColor = White;
        format.crBackColor = Red;
        colors_.at(level::critical) = format;
    }

    void set_default_color(const CHARFORMAT2W* format)
    {
        // std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        if(nullptr != format)
            memcpy(&default_color_, format, format->cbSize);
    }

    void set_level_color(level::level_enum color_level, const CHARFORMAT2W& format)
    {
        // std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        colors_.at(static_cast<size_t>(color_level)) = format;
    }

    const CHARFORMAT2W& get_level_color(level::level_enum color_level)
    {
        std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        return colors_.at(static_cast<size_t>(color_level));
    }

    const CHARFORMAT2W& get_default_color()
    {
        std::lock_guard<Mutex> lock(base_sink<Mutex>::mutex_);
        return default_color_;
    }

protected:

    virtual void set_line_color(level::level_enum level) override {
    
        this->SetLineFormat(&colors_.at(level));
    }

 private:
    inline void SetLineFormat(const CHARFORMAT2W* cf)
    {
        this->SendMsgA(EM_SETSEL, -2, -1);
        this->SendMsgW(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)cf);
    }

 private:
    CHARFORMAT2W default_color_;
    std::array<CHARFORMAT2W, level::n_levels> colors_;
};

#include "spdlog/details/null_mutex.h"
#include <mutex>

using winedit_sink_mt = winedit_sink<std::mutex>;
using winedit_sink_st = winedit_sink<details::null_mutex>;

using winedit_color_sink_mt = winedit_color_sink<std::mutex>;
using winedit_color_sink_st = winedit_color_sink<details::null_mutex>;

} // namespace sinks
} // namespace spdlog
