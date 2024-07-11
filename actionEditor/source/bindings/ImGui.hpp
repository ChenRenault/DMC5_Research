#pragma once
namespace sol {
    class state;
}

namespace bindings {
void open_imgui(sol::state& s);
}

namespace api::imgui {
void cleanup();
}