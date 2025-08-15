#include "requestadapter.h"

#include "sni_user.h"
#include "sni_thread.h"

#include <string_view>
#include <vector>
#include <cstdlib> // strtol

#include "sn_pch.h"


namespace SNI
{
    RequestAdapter::RequestAdapter(const char* qs)
    { 
        build_params(qs ? qs : "");
    }

    long RequestAdapter::threadnum(long def) const
    { 
        return qint("threadnum", def);
    }

    bool RequestAdapter::dispatch(std::string_view path, SNI_User& u, SNI_Thread& t, RequestAdapter::Reply& out) {
        m_user = &u;
        m_thr = &t;
        const auto& R = routes();
        auto it = R.find(path);
        if (it == R.end()) return false;
        out = (this->*(it->second))();    // zero-arg member call
        return true;
    }

    // -------- HTML routes --------
    RequestAdapter::Reply RequestAdapter::SkynetHtml()
    {
        return { m_thr->Skynet(doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::RunHtml()
    { 
        return { m_thr->RunWeb(doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::RunToEndHtml()
    { 
        return { m_thr->RunToEndWeb(doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::DebugHtml()
    {
        return { m_thr->DebugWeb(doDebugPointsHTML), "html" }; 
    }
    RequestAdapter::Reply RequestAdapter::CodeBreakHtml() 
    {
        return { m_thr->CodeBreakWeb(doDebugPointsHTML), "html" }; 
    }
    RequestAdapter::Reply RequestAdapter::StepOverHtml() 
    {
        return { m_thr->StepOverWeb(doDebugPointsHTML), "html" }; 
    }
    RequestAdapter::Reply RequestAdapter::StepIntoHtml() 
    { 
        return { m_thr->StepIntoWeb(doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::StepOutHtml() 
    {
        return { m_thr->StepOutWeb(doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::StepParamHtml()
    {
        return { m_thr->StepParamWeb(doDebugPointsHTML), "html" }; 
    }
    RequestAdapter::Reply RequestAdapter::GotoStepCountHtml()
    {
        long n = qint("stepcount", 0);
        return (n > 0) ? RequestAdapter::Reply{ m_thr->GotoStepCountWeb(n, doDebugPointsHTML), "html" }
        : RequestAdapter::Reply{ std::string{}, "html" };
    }
    RequestAdapter::Reply RequestAdapter::MaxStackFramesHtml()
    {
        long n = qint("maxstackframes", 0);
        return { m_thr->SetMaxStackFramesWeb(n, doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::ThreadHtml()
    {
        return { m_thr->Skynet(doDebugPointsHTML), "html" };
    }
    RequestAdapter::Reply RequestAdapter::QuitHtml()
    {
        return { m_thr->QuitWeb(doDebugPointsHTML), "html" };
    }

    // -------- JSON action routes (return tiny ack JSON) --------
    RequestAdapter::Reply RequestAdapter::RunJS()
    { 
        m_thr->Run(); 
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::RunToEndJS()
    { 
        m_thr->RunToEnd(); 
        return { R"({"ok":true})", "json" }; 
    }
    RequestAdapter::Reply RequestAdapter::DebugJS() 
    {
        auto it = m_q.find("breakpoints");
        if (it != m_q.end()) m_thr->LoadBreakPoints(it->second);
        m_thr->Debug();
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::CodeBreakJS()
    { 
        m_thr->CodeBreak();
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::RerunJS() 
    { 
        m_thr->Rerun();
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::StepOverJS()
    { 
        m_thr->StepOver();
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::StepIntoJS()
    {
        m_thr->StepInto(static_cast<SN::DebuggingStop>(qint("debugstop", 0)));
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::StepOutJS()
    { 
        m_thr->StepOut();
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::StepOutCallJS()
    { 
        m_thr->StepOutCall();
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::StepParamJS()
    { 
        m_thr->StepParam();
        return { R"({"ok":true})", "json" }; 
    }
    RequestAdapter::Reply RequestAdapter::GotoStepCountJS()
    {
        long n = qint("stepcount", 0);
        if (n > 0) m_thr->GotoStepCount(n);
        return { R"({"ok":true})", "json" };
    }
    RequestAdapter::Reply RequestAdapter::QuitJS()
    { 
        m_thr->Quit(); 
        return { R"({"ok":true})", "json" };
    }

    // -------- JSON data routes --------
    RequestAdapter::Reply RequestAdapter::DashboardJson() 
    { 
        return { m_thr->DashboardJS(doDebugPointsJS), "json" };
    }
    RequestAdapter::Reply RequestAdapter::StackJson() 
    {
        return { m_thr->StackJS(qint("maxstackframes"),
                                qint("startstackframes"),
                                qint("stepcount"),
                                doDebugPointsJS), "json" };
    }
    RequestAdapter::Reply RequestAdapter::CallStackJson()
    {
        return { m_thr->CallStackJS(qint("maxcallstackframes"),
                                    qint("startcallstackframes"),
                                    qint("stepcount"),
                                    doDebugPointsJS), "json" };
    }
    RequestAdapter::Reply RequestAdapter::WatchListJson()
    { 
        return { m_thr->WatchListJS(qint("stepcount"), doDebugPointsJS), "json" };
    }
    RequestAdapter::Reply RequestAdapter::ChangeHistoryJson() {
        return { m_thr->ChangeHistoryJS(doDebugPointsJS,
                                        (size_t)qint("columnwidth"),
                                        (size_t)qint("fromstep"),
                                        (size_t)qint("tostep")), "json" };
    }
    RequestAdapter::Reply RequestAdapter::LogJson() {
        return { m_thr->LogJS(qint("maxlogentries"),
                                qint("startlog"),
                                qint("stepcount")), "json" };
    }
    RequestAdapter::Reply RequestAdapter::DerivationJson()
    { 
        return { m_thr->DerivationJS(qint("maxlogentries")), "json" };
    }
    RequestAdapter::Reply RequestAdapter::CodeJson() {
        return { m_thr->CodeJS(qint("maxcode"),
                                qint("startcode"),
                                qint("stepcount"),
                                doDebugPointsJS), "json" };
    }
    RequestAdapter::Reply RequestAdapter::StepCountJson()
    { 
        return { m_user->StepCountJS(), "json" }; 
    }
    RequestAdapter::Reply RequestAdapter::ErrorJson()
    { 
        return { m_thr->ErrorJS(doDebugPointsJS), "json" }; 
    }
    RequestAdapter::Reply RequestAdapter::WorldSetsJson()
    {
        return { m_thr->WorldSetsJS(doTextOnly), "json" };
    }
    RequestAdapter::Reply RequestAdapter::DelayedJson() 
    { 
        return { m_thr->DelayedJS(doDebugPointsJS), "json" };
    }

    /*static*/ const std::unordered_map<std::string_view, RequestAdapter::Handler>& RequestAdapter::routes()
    {
        using SV = std::string_view;
        static const std::unordered_map<SV, Handler, std::hash<SV>, std::equal_to<SV>> R
        {
            // HTML
            {SV{"/skynet"},          &RequestAdapter::SkynetHtml},
            {SV{"/run"},             &RequestAdapter::RunHtml},
            {SV{"/runtoend"},        &RequestAdapter::RunToEndHtml},
            {SV{"/debug"},           &RequestAdapter::DebugHtml},
            {SV{"/codebreak"},       &RequestAdapter::CodeBreakHtml},
            {SV{"/stepover"},        &RequestAdapter::StepOverHtml},
            {SV{"/stepinto"},        &RequestAdapter::StepIntoHtml},
            {SV{"/stepout"},         &RequestAdapter::StepOutHtml},
            {SV{"/stepparam"},       &RequestAdapter::StepParamHtml},
            {SV{"/gotostepcount"},   &RequestAdapter::GotoStepCountHtml},
            {SV{"/maxstackframes"},  &RequestAdapter::MaxStackFramesHtml},
            {SV{"/thread"},          &RequestAdapter::ThreadHtml},
            {SV{"/quit"},            &RequestAdapter::QuitHtml},

            // JSON actions
            {SV{"/runjs"},           &RequestAdapter::RunJS},
            {SV{"/runtoendjs"},      &RequestAdapter::RunToEndJS},
            {SV{"/debugjs"},         &RequestAdapter::DebugJS},
            {SV{"/codebreakjs"},     &RequestAdapter::CodeBreakJS},
            {SV{"/rerunjs"},         &RequestAdapter::RerunJS},
            {SV{"/stepoverjs"},      &RequestAdapter::StepOverJS},
            {SV{"/stepintojs"},      &RequestAdapter::StepIntoJS},
            {SV{"/stepoutjs"},       &RequestAdapter::StepOutJS},
            {SV{"/stepoutcalljs"},   &RequestAdapter::StepOutCallJS},
            {SV{"/stepparamjs"},     &RequestAdapter::StepParamJS},
            {SV{"/gotostepcountjs"}, &RequestAdapter::GotoStepCountJS},
            {SV{"/quitjs"},          &RequestAdapter::QuitJS},

            // JSON data
            {SV{"/dashboard.json"},  &RequestAdapter::DashboardJson},
            {SV{"/stack.json"},      &RequestAdapter::StackJson},
            {SV{"/callstack.json"},  &RequestAdapter::CallStackJson},
            {SV{"/watchlist.json"},  &RequestAdapter::WatchListJson},
            {SV{"/changehistory.json"}, &RequestAdapter::ChangeHistoryJson},
            {SV{"/log.json"},        &RequestAdapter::LogJson},
            {SV{"/derivation.json"}, &RequestAdapter::DerivationJson},
            {SV{"/code.json"},       &RequestAdapter::CodeJson},
            {SV{"/stepcount.json"},  &RequestAdapter::StepCountJson},
            {SV{"/error.json"},      &RequestAdapter::ErrorJson},
            {SV{"/worldsets.json"},  &RequestAdapter::WorldSetsJson},
            {SV{"/delayed.json"},    &RequestAdapter::DelayedJson},
        };
        return R;
    }

    // --- params + helpers ---
    void RequestAdapter::build_params(const std::string& qs) {
        m_q.clear();
        std::size_t pos = 0;
        while (pos < qs.size()) {
            std::size_t amp = qs.find('&', pos);
            std::size_t eq = qs.find('=', pos);
            if (eq != std::string::npos && (amp == std::string::npos || eq < amp)) {
                m_q.emplace(qs.substr(pos, eq - pos),
                    qs.substr(eq + 1, (amp == std::string::npos ? qs.size() : amp) - (eq + 1)));
            }
            if (amp == std::string::npos) break;
            pos = amp + 1;
        }
    }

    long RequestAdapter::qint(const char* k, long def) const
    {
        auto it = m_q.find(k); if (it == m_q.end()) return def;
        char* e = nullptr; long v = std::strtol(it->second.c_str(), &e, 10);
        return (e && *e == '\0') ? v : def;
    }

} // SNI

