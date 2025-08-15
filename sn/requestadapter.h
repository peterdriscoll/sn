#pragma once

#include <vector>
#include <cstdlib> // strtol

#include "sn_pch.h"
#include <string_view>

namespace SNI
{
    // Minimal per-request adapter that holds params/user/thread and exposes zero-arg routes.
    class RequestAdapter {
    public:
        struct Reply 
        { 
            std::string body;
            const char* ext;  // "json" | "html"
        };

        typedef Reply(RequestAdapter::* Handler)();           // uniform function type

        explicit RequestAdapter(const char* qs);

        long threadnum(long def = -1) const;

        bool dispatch(std::string_view path, SNI_User& u, SNI_Thread& t, Reply& out);
        
        // -------- HTML routes --------
        Reply SkynetHtml();
        Reply RunHtml();
        Reply RunToEndHtml();
        Reply DebugHtml();
        Reply CodeBreakHtml();
        Reply StepOverHtml();
        Reply StepIntoHtml();
        Reply StepOutHtml();
        Reply StepParamHtml();
        Reply GotoStepCountHtml();
        Reply MaxStackFramesHtml();
        Reply ThreadHtml();
        Reply QuitHtml();

        // -------- JSON action routes (return tiny ack JSON) --------
        Reply RunJS();
        Reply RunToEndJS();
        Reply DebugJS();
        Reply CodeBreakJS();
        Reply RerunJS();
        Reply StepOverJS();
        Reply StepIntoJS();
        Reply StepOutJS();
        Reply StepOutCallJS();
        Reply StepParamJS();
        Reply GotoStepCountJS();;
        Reply QuitJS();

        // -------- JSON data routes --------
        Reply DashboardJson();
        Reply StackJson();
        Reply CallStackJson();
        Reply WatchListJson();
        Reply ChangeHistoryJson();
        Reply LogJson();
        Reply DerivationJson();
        Reply CodeJson();
        Reply StepCountJson();
        Reply ErrorJson();
        Reply WorldSetsJson();
        Reply DelayedJson();


    private:
        static const std::unordered_map<std::string_view, Handler>& routes();

        // --- params + helpers ---
        void build_params(const std::string& qs);
        long qint(const char* k, long def = 0) const;

        string_umap  m_q;
        SNI_User* m_user = nullptr;
        SNI_Thread* m_thr = nullptr;
    };
} // SNI

