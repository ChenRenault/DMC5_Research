
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor_internal.h>
#include <imgui_node_editor.h>

#include "source/Math.hpp"

#include <filesystem>
#include <regex>
#include <fstream>
#include <filesystem>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include "ImGui.hpp"

namespace api::imgui {
int32_t g_disabled_counts{0};

void cleanup() {
    for (auto i = 0; i < g_disabled_counts; ++i) {
        ImGui::EndDisabled();
    }

    g_disabled_counts = 0;
}

ImVec2 create_imvec2(sol::object obj) {
    ImVec2 out{ 0.0f, 0.0f };

    if (obj.is<Vector2f>()) {
        auto vec = obj.as<Vector2f>();
        out.x = vec.x;
        out.y = vec.y;
    } else if (obj.is<sol::table>()) {
        auto table = obj.as<sol::table>();

        if (table.size() == 2) {
            out.x = table.get<float>(1);
            out.y = table.get<float>(2);
        } else {
            throw sol::error{ "Invalid table passed. Table size must be 2." };
        }
    } else if (obj.is<Vector3f>()) {
        auto vec = obj.as<Vector3f>();
        out.x = vec.x;
        out.y = vec.y;
    } else if (obj.is<Vector4f>()) {
        auto vec = obj.as<Vector4f>();
        out.x = vec.x;
        out.y = vec.y;
    }

    return out;
};

ImVec4 create_imvec4(sol::object obj) {
    ImVec4 out{ 0.0f, 0.0f, 0.0f, 0.0f };

    if (obj.is<Vector4f>()) {
        auto vec = obj.as<Vector4f>();
        out.x = vec.x;
        out.y = vec.y;
        out.z = vec.z;
        out.w = vec.w;
    } else if (obj.is<sol::table>()) {
        auto table = obj.as<sol::table>();

        if (table.size() == 4) {
            out.x = table.get<float>(1);
            out.y = table.get<float>(2);
            out.z = table.get<float>(3);
            out.w = table.get<float>(4);
        } else {
            throw sol::error{ "Invalid table passed. Table size must be 4." };
        }
    }

    return out;
};

bool button(const char* label, sol::object size_object) {
    if (label == nullptr) {
        label = "";
    }

    const auto size = create_imvec2(size_object);

    return ImGui::Button(label, size);
}

bool small_button(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::SmallButton(label);
}

bool invisible_button(const char* id, sol::object size_object, sol::object flags_object) {
    if (id == nullptr) {
        id = "";
    }

    const auto size = create_imvec2(size_object);

    ImGuiButtonFlags flags = 0;

    if (flags_object.is<int>()) {
        flags = (ImGuiButtonFlags)(flags_object.as<int>());
    }

    return ImGui::InvisibleButton(id, size, flags);
}

bool arrow_button(const char* str_id, int dir) {
    if (str_id == nullptr) {
        str_id = "";
    }

    return ImGui::ArrowButton(str_id, (ImGuiDir)dir);
}

void text(const char* text) {
    if (text == nullptr) {
        text = "";
    }

    ImGui::TextUnformatted(text);
}

void text_colored(const char* text, unsigned int color) {
    if (text == nullptr) {
        text = "";
    }

    auto r = color & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = (color >> 16) & 0xFF;
    auto a = (color >> 24) & 0xFF;

    ImGui::TextColored(ImVec4{ (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f }, text);
}

sol::variadic_results checkbox(sol::this_state s, const char* label, bool v) {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::Checkbox(label, &v);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float(sol::this_state s, const char* label, float v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat(label, &v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float2(sol::this_state s, const char* label, Vector2f v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat2(label, (float*)&v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_float3(sol::this_state s, const char* label, Vector3f v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat3(label, (float*)&v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}


sol::variadic_results drag_float4(sol::this_state s, const char* label, Vector4f v, float v_speed, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragFloat4(label, (float*)&v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results drag_int(sol::this_state s, const char* label, int v, float v_speed, int v_min, int v_max, const char* display_format = "%d") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::DragInt(label, &v, v_speed, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results slider_float(sol::this_state s, const char* label, float v, float v_min, float v_max, const char* display_format = "%.3f") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::SliderFloat(label, &v, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}


sol::variadic_results slider_int(sol::this_state s, const char* label, int v, int v_min, int v_max, const char* display_format = "%d") {
    if (label == nullptr) {
        label = "";
    }

    auto changed = ImGui::SliderInt(label, &v, v_min, v_max, display_format);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, v));

    return results;
}

sol::variadic_results input_text(sol::this_state s, const char* label, const std::string& v, ImGuiInputTextFlags flags) {
    flags |= ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackAlways;

    if (label == nullptr) {
        label = "";
    }

    static std::string buffer{""};
    buffer = v;

    static int selection_start, selection_end;

    static auto input_text_callback = [](ImGuiInputTextCallbackData* data) -> int {
        if ((data->EventFlag & ImGuiInputTextFlags_CallbackResize) != 0) {
            buffer.resize(data->BufTextLen);
            data->Buf = (char*)buffer.c_str();
        }

        selection_start = data->SelectionStart;
        selection_end = data->SelectionEnd;

        return 0;
    };

    auto changed = ImGui::InputText(label, buffer.data(), buffer.capacity() + 1, flags, input_text_callback);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, std::string{buffer.data()}));
    results.push_back(sol::make_object(s, selection_start));
    results.push_back(sol::make_object(s, selection_end));

    return results;
}

sol::variadic_results input_text_multiline(sol::this_state s, const char* label, const std::string& v, sol::object size_obj, ImGuiInputTextFlags flags) {
    flags |= ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_CallbackAlways;

    if (label == nullptr) {
        label = "";
    }

    static std::string buffer{""};
    buffer = v;

    static int selection_start, selection_end;

    static auto input_text_callback = [](ImGuiInputTextCallbackData* data) -> int {
        if ((data->EventFlag & ImGuiInputTextFlags_CallbackResize) != 0) {
            buffer.resize(data->BufTextLen);
            data->Buf = (char*)buffer.c_str();
        }

        selection_start = data->SelectionStart;
        selection_end = data->SelectionEnd;

        return 0;
    };

    const auto size = create_imvec2(size_obj);

    auto changed = ImGui::InputTextMultiline(label, buffer.data(), buffer.capacity() + 1, size, flags, input_text_callback);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, std::string{buffer.data()}));
    results.push_back(sol::make_object(s, selection_start));
    results.push_back(sol::make_object(s, selection_end));

    return results;
}


sol::variadic_results combo(sol::this_state s, const char* label, sol::object selection, sol::table values) {
    if (label == nullptr) {
        label = "";
    }

    const char* preview_value = "";

    if (!values.empty()) {
        if (selection.is<sol::nil_t>() || values.get_or(selection, sol::make_object(s, sol::nil)).is<sol::nil_t>()) {
            selection = (*values.begin()).first;
        }

        auto val_at_selection = values[selection].get<sol::object>();

        if (val_at_selection.is<const char*>()) {
            preview_value = val_at_selection.as<const char*>();
        }
    }

    auto selection_changed = false;

    if (ImGui::BeginCombo(label, preview_value)) {
        //for (auto i = 1u; i <= values.size(); ++i) {
        for (auto& [key, val] : values) {
            auto val_at_k = values[key].get<sol::object>();

            if (val_at_k.is<const char*>()) {
                auto entry = val_at_k.as<const char*>();

                if (ImGui::Selectable(entry, selection == key)) {
                    selection = key;
                    selection_changed = true;
                }
            }
        }

        ImGui::EndCombo();
    }

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, selection_changed));
    results.push_back(sol::make_object(s, selection));

    return results;
}

bool tree_node(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::TreeNode(label);
}

bool tree_node_ptr_id(const void* id, const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::TreeNode(id, label);
}

bool tree_node_str_id(const char* id, const char* label) {
    if (label == nullptr) {
        label = "";
    }

    return ImGui::TreeNode(id, label);
}

void tree_pop() {
    ImGui::TreePop();
}

void same_line(sol::this_state s, float offset_from_start_x=0.0f, float spacing=-1.0f) {
    ImGui::SameLine(offset_from_start_x, spacing);
}

bool is_item_hovered(sol::object flags_obj) {
    ImGuiHoveredFlags flags{0};

    if (flags_obj.is<int>()) {
        flags = (ImGuiHoveredFlags)flags_obj.as<int>();
    }

    return ImGui::IsItemHovered(flags);
}

bool is_item_active() {
    return ImGui::IsItemActive();
}

bool is_item_focused() {
    return ImGui::IsItemFocused();
}

bool begin_window(const char* name, sol::object open_obj, ImGuiWindowFlags flags = 0) {
    if (name == nullptr) {
        name = "";
    }

    bool open = true;
    bool* open_p = nullptr;

    if (!open_obj.is<sol::nil_t>() && open_obj.is<bool>()) {
        open = open_obj.as<bool>();
        open_p = &open;
    }

    if (!open) {
        return false;
    }

    ImGui::Begin(name, open_p, flags);

    return open;
}

void end_window() {
    ImGui::End();
}

bool begin_child_window(const char* name, sol::object size_obj, sol::object border_obj, ImGuiWindowFlags flags = 0) {
    if (name == nullptr) {
        name = "";
    }

    const auto size = create_imvec2(size_obj);
    bool border{false};

    if (border_obj.is<bool>()) {
        border = border_obj.as<bool>();
    }

    return ImGui::BeginChild(name, size, border, flags);
}

void end_child_window() {
    ImGui::EndChild();
}

void begin_group() {
    ImGui::BeginGroup();
}

void end_group() {
    ImGui::EndGroup();
}

void begin_rect() {
    ImGui::BeginGroup();
}

void end_rect(sol::object additional_size_obj, sol::object rounding_obj) {
    ImGui::EndGroup();

    float rounding = rounding_obj.is<float>() ? rounding_obj.as<float>() : ImGui::GetStyle().FrameRounding;
    float additional_size = additional_size_obj.is<float>() ? additional_size_obj.as<float>() : 0.0f;

    auto mins = ImGui::GetItemRectMin();
    mins.x -= additional_size;
    mins.y -= additional_size;

    auto maxs = ImGui::GetItemRectMax();
    maxs.x += additional_size;
    maxs.y += additional_size;

    ImGui::GetWindowDrawList()->AddRect(mins, maxs, ImGui::GetColorU32(ImGuiCol_Border), ImGui::GetStyle().FrameRounding, ImDrawFlags_RoundCornersAll, 1.0f);
}

void begin_disabled(sol::object disabled_obj) {
    bool disabled{true};

    if (disabled_obj.is<bool>()) {
        disabled = disabled_obj.as<bool>();
    }

    ++g_disabled_counts;
    ImGui::BeginDisabled(disabled);
}

void end_disabled() {
    if (g_disabled_counts > 0) {
        --g_disabled_counts;
        ImGui::EndDisabled();
    }
}

void separator() {
    ImGui::Separator();
}

void spacing() {
    ImGui::Spacing();
}

void new_line() {
    ImGui::NewLine();
}

sol::variadic_results Splitter(sol::this_state s, bool split_vertically, float thickness, float size1, float size2, float min_size1, float min_size2) {
    const float splitter_long_axis_size = -1.0f;
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(size1, 0.0f) : ImVec2(0.0f, size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    auto changed = SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, &size1, &size2, min_size1, min_size2, 0.0f);
    sol::variadic_results results{};
    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, size1));
    results.push_back(sol::make_object(s, size2));

    return results;
}

bool collapsing_header(const char* name) {
    return ImGui::CollapsingHeader(name);
}

int load_font(sol::object filepath_obj, int size, sol::object ranges) {
    // namespace fs = std::filesystem;
    // const char* filepath = "doesnt-exist.not-a-real-font";

    // if (filepath_obj.is<const char*>()) {
    //     filepath = filepath_obj.as<const char*>();
    // }

    // if (std::filesystem::path(filepath).is_absolute()) {
    //     throw std::runtime_error("Font filepath must not be absolute.");
    // }

    // if (std::string{filepath}.find("..") != std::string::npos) {
    //     throw std::runtime_error("Font filepath cannot access parent directories.");
    // }

    // const auto fonts_path = REFramework::get_persistent_dir() / "reframework" / "fonts";
    // const auto font_path = fonts_path / filepath;

    // fs::create_directories(fonts_path);
    // std::vector<ImWchar> ranges_vec{};

    // if (ranges.is<sol::table>()) {
    //     sol::table ranges_table = ranges;

    //     for (auto i = 1u; i <= ranges_table.size(); ++i) {
    //         ranges_vec.push_back(ranges_table[i].get<ImWchar>());
    //     }
    // }

    // return g_framework->add_font(font_path, size, ranges_vec);
    return  0;
}

void push_font(int font) {
    // ImGui::PushFont(g_framework->get_font(font));

}

void pop_font() {
    ImGui::PopFont();
}

int get_default_font_size() {
    // return g_framework->get_font_size();
    return 14;
}

sol::variadic_results color_picker(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto r = color & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = (color >> 16) & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorPicker4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r;
    new_color |= g << 8;
    new_color |= b << 16;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_picker_argb(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto r = (color >> 16) & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = color & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorPicker4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r << 16;
    new_color |= g << 8;
    new_color |= b;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_picker3(sol::this_state s, const char* label, Vector3f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorPicker3(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

sol::variadic_results color_picker4(sol::this_state s, const char* label, Vector4f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorPicker4(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

sol::variadic_results color_edit(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

   
    auto r = color & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = (color >> 16) & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorEdit4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r;
    new_color |= g << 8;
    new_color |= b << 16;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_edit_argb(sol::this_state s, const char* label, unsigned int color, sol::object flags_obj) {
    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto r = (color >> 16) & 0xFF;
    auto g = (color >> 8) & 0xFF;
    auto b = color & 0xFF;
    auto a = (color >> 24) & 0xFF;

    float col[4]{
        r / 255.0f,
        g / 255.0f,
        b / 255.0f,
        a / 255.0f,
    };

    auto changed = ImGui::ColorEdit4(label, col, flags);

    r = (unsigned int)(col[0] * 255.0f);
    g = (unsigned int)(col[1] * 255.0f);
    b = (unsigned int)(col[2] * 255.0f);
    a = (unsigned int)(col[3] * 255.0f);

    unsigned int new_color = 0;

    new_color |= r << 16;
    new_color |= g << 8;
    new_color |= b;
    new_color |= a << 24;

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, new_color));

    return results;
}

sol::variadic_results color_edit3(sol::this_state s, const char* label, Vector3f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorEdit3(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

sol::variadic_results color_edit4(sol::this_state s, const char* label, Vector4f color, sol::object flags_obj) {
    if (label == nullptr) {
        label = "";
    }

    ImGuiColorEditFlags flags{};

    if (flags_obj.is<int>()) {
        flags = (ImGuiColorEditFlags)flags_obj.as<int>();
    }

    auto changed = ImGui::ColorEdit4(label, &color.x, flags);

    sol::variadic_results results{};

    results.push_back(sol::make_object(s, changed));
    results.push_back(sol::make_object(s, color));

    return results;
}

void set_next_window_pos(sol::object pos_obj, sol::object condition_obj, sol::object pivot_obj) {
    ImGuiCond condition{};

    if (condition_obj.is<int>()) {
        condition = (ImGuiCond)condition_obj.as<int>();
    }

    auto pos = create_imvec2(pos_obj);
    auto pivot = create_imvec2(pivot_obj);

    ImGui::SetNextWindowPos(pos, condition, pivot);
}

void set_next_window_size(sol::object size_obj, sol::object condition_obj) {
    ImGuiCond condition{};

    if (condition_obj.is<int>()) {
        condition = (ImGuiCond)condition_obj.as<int>();
    }

    auto size = create_imvec2(size_obj);

    ImGui::SetNextWindowSize(size, condition);
}

void push_id(sol::object id) {
    if (id.is<int>()) {
        ImGui::PushID(id.as<int>());
    } else if (id.is<const char*>()) {
        ImGui::PushID(id.as<const char*>());
    } else if (id.is<void*>()) {
        ImGui::PushID(id.as<void*>());
    } else {
        throw sol::error("Type must be int, const char* or void*");
    }
}

void pop_id() {
    ImGui::PopID();
}

ImGuiID get_id(sol::object id) {
    if (id.is<int>()) {
        return id.as<ImGuiID>();
    } else if (id.is<const char*>()) {
        return ImGui::GetID(id.as<const char*>());
    } else if (id.is<void*>()) {
        return ImGui::GetID(id.as<void*>());
    } else {
        throw sol::error("Type must be int, const char* or void*");
    }

    return 0;
}

Vector2f get_mouse() {
    const auto mouse = ImGui::GetMousePos();

    return Vector2f{
        mouse.x,
        mouse.y,
    };
}

int get_key_index(int imgui_key) {
    return ImGui::GetKeyIndex((ImGuiKey)imgui_key);
}

bool is_key_down(int key) {
    return ImGui::IsKeyDown((ImGuiKey)key);
}

bool is_key_pressed(int key) {
    return ImGui::IsKeyPressed((ImGuiKey)key);
}

bool is_key_released(int key) {
    return ImGui::IsKeyReleased((ImGuiKey)key);
}

bool is_mouse_down(int button) {
    return ImGui::IsMouseDown(button);
}

bool is_mouse_clicked(int button) {
    return ImGui::IsMouseClicked(button);
}

bool is_mouse_released(int button) {
    return ImGui::IsMouseReleased(button);
}

bool is_mouse_double_clicked(int button) {
    return ImGui::IsMouseDoubleClicked(button);
}

void dummy(sol::object size_obj) {
    auto size = create_imvec2(size_obj);
    ImGui::Dummy(size);
}

void indent(int indent_width) {
    ImGui::Indent(indent_width);
}

void unindent(int indent_width) {
    ImGui::Unindent(indent_width);
}

void begin_tooltip() {
    ImGui::BeginTooltip();
}

void end_tooltip() {
    ImGui::EndTooltip();
}

void set_tooltip(const char* text) {
    if (text == nullptr) {
        text = "";
    }

    ImGui::SetTooltip(text);
}

void open_popup(const char* str_id, sol::object flags_obj) {
    if (str_id == nullptr) {
        str_id = "";
    }

    ImGuiWindowFlags flags{0};

    if (flags_obj.is<int>()) {
        flags = (ImGuiWindowFlags)flags_obj.as<int>();
    }

    ImGui::OpenPopup(str_id, flags);
}

bool begin_popup(const char* str_id, sol::object flags_obj) {
    int flags{0};

    if (flags_obj.is<int>()) {
        flags = flags_obj.as<int>();
    }

    return ImGui::BeginPopup(str_id, flags);
}

bool begin_popup_context_item(const char* str_id, sol::object flags_obj) {
    int flags{1};

    if (flags_obj.is<int>()) {
        flags = flags_obj.as<int>();
    }

    return ImGui::BeginPopupContextItem(str_id, flags);
}

void end_popup() {
    ImGui::EndPopup();
}

void close_current_popup() {
    ImGui::CloseCurrentPopup();
}

bool is_popup_open(const char* str_id) {
    return ImGui::IsPopupOpen(str_id);
}

Vector2f calc_text_size(const char* text) {
    const auto result = ImGui::CalcTextSize(text);

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_window_size() {
    const auto result = ImGui::GetWindowSize();

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_window_pos() {
    const auto result = ImGui::GetWindowPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

void set_next_item_open(bool is_open, sol::object condition_obj) {
    ImGuiCond condition{0};

    if (condition_obj.is<int>()) {
        condition = (ImGuiCond)condition_obj.as<int>();
    }

    ImGui::SetNextItemOpen(is_open, condition);
}

bool begin_list_box(const char* label, sol::object size_obj) {
    if (label == nullptr) {
        label = "";
    }

    auto size = create_imvec2(size_obj);

    return ImGui::BeginListBox(label, size);
}

void end_list_box() {
    ImGui::EndListBox();
}

bool begin_menu_bar() {
    return ImGui::BeginMenuBar();
}

void end_menu_bar() {
    ImGui::EndMenuBar();
}

bool begin_main_menu_bar() {
    return ImGui::BeginMainMenuBar();
}

void end_main_menu_bar() {
    ImGui::EndMainMenuBar();
}

bool begin_menu(const char* label, sol::object enabled_obj) {
    if (label == nullptr) {
        label = "";
    }

    bool enabled{true};

    if (enabled_obj.is<bool>()) {
        enabled = enabled_obj.as<bool>();
    }

    return ImGui::BeginMenu(label, enabled);
}

void end_menu() {
    ImGui::EndMenu();
}

bool menu_item(const char* label, sol::object shortcut_obj, sol::object selected_obj, sol::object enabled_obj) {
    if (label == nullptr) {
        label = "";
    }

    const char* shortcut{nullptr};
    bool selected{false};
    bool enabled{true};

    if (shortcut_obj.is<const char*>()) {
        shortcut = shortcut_obj.as<const char*>();
    } else {
        shortcut = "";
    }

    if (selected_obj.is<bool>()) {
        selected = selected_obj.as<bool>();
    }

    if (enabled_obj.is<bool>()) {
        enabled = enabled_obj.as<bool>();
    }

    return ImGui::MenuItem(label, shortcut, selected, enabled);
}

Vector2f get_display_size() {
    const auto& result = ImGui::GetIO().DisplaySize;

    return Vector2f{
        result.x,
        result.y,
    };
}

void push_item_width(float item_width) {
    ImGui::PushItemWidth(item_width);
}

void pop_item_width() {
    ImGui::PopItemWidth();
}

void set_next_item_width(float item_width) {
    ImGui::SetNextItemWidth(item_width);
}

float calc_item_width() {
    return ImGui::CalcItemWidth();
}

void item_size(sol::object pos, sol::object size, sol::object text_baseline_y) {
    if (text_baseline_y.is<float>()) {
        ImGui::ItemSize(ImRect{create_imvec2(pos), create_imvec2(size)}, text_baseline_y.as<float>());
    } else {
        ImGui::ItemSize(ImRect{create_imvec2(pos), create_imvec2(size)});
    }
}

bool item_add(const char* label, sol::object pos, sol::object size) {
    if (label == nullptr) {
        label = "";
    }

    const auto window = ImGui::GetCurrentWindow();

    if (window == nullptr) {
        return false;
    }

    return ImGui::ItemAdd(ImRect{create_imvec2(pos), create_imvec2(size)}, window->GetID(label));
}

void push_style_color(int style_color, sol::object color_obj) {
    if (color_obj.is<int>()) {
        ImGui::PushStyleColor((ImGuiCol)style_color, (ImU32)color_obj.as<int>());
    } else if (color_obj.is<Vector4f>()) {
        ImGui::PushStyleColor((ImGuiCol)style_color, create_imvec4(color_obj));
    }
}

void pop_style_color(sol::object count_obj) {
    int count{1};

    if (count_obj.is<int>()) {
        count = count_obj.as<int>();
    }

    ImGui::PopStyleColor(count);
}

void push_style_var(int idx, sol::object value_obj) {
    if (value_obj.is<float>()) {
        ImGui::PushStyleVar((ImGuiStyleVar)idx, value_obj.as<float>());
    } else if (value_obj.is<Vector2f>()) {
        ImGui::PushStyleVar((ImGuiStyleVar)idx, create_imvec2(value_obj));
    }
}

void pop_style_var(sol::object count_obj) {
    int count{1};

    if (count_obj.is<int>()) {
        count = count_obj.as<int>();
    }

    ImGui::PopStyleVar(count);
}

Vector2f get_cursor_pos() {
    const auto result = ImGui::GetCursorPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

void set_cursor_pos(sol::object pos) {
    ImGui::SetCursorPos(create_imvec2(pos));
}

Vector2f get_cursor_start_pos() {
    const auto result = ImGui::GetCursorStartPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f get_cursor_screen_pos() {
    const auto result = ImGui::GetCursorScreenPos();

    return Vector2f{
        result.x,
        result.y,
    };
}

void set_cursor_screen_pos(sol::object pos) {
    ImGui::SetCursorScreenPos(create_imvec2(pos));
}

void set_item_default_focus() {
    ImGui::SetItemDefaultFocus();
}

void set_clipboard(sol::object data) {
    ImGui::SetClipboardText(data.as<const char*>());
}

const char* get_clipboard() {
    return ImGui::GetClipboardText();
}

void progress_bar(float progress, sol::object size, const char* overlay ){
    if (overlay == nullptr) {
        overlay = "";
    }

    ImGui::ProgressBar(progress, create_imvec2(size), overlay);
}

bool begin_table(const char* str_id, int column, sol::object flags_obj, sol::object outer_size_obj, sol::object inner_width_obj) {
    if (str_id == nullptr) {
        str_id = "";
    }

    ImVec2 outer_size{};
    if (outer_size_obj.is<Vector2f>()) {
        const auto& vec = outer_size_obj.as<Vector2f>();
        outer_size = {vec.x, vec.y};
    }

    float inner_width = inner_width_obj.is<float>() ? inner_width_obj.as<float>() : 0.0f;
    int flags = flags_obj.is<int>() ? flags_obj.as<int>() : 0;

    return ImGui::BeginTable(str_id, column, flags, outer_size, inner_width);
}

void end_table() {
    ImGui::EndTable();
}

void table_next_row(sol::object row_flags, sol::object min_row_height) {
    int flags = row_flags.is<int>() ? row_flags.as<int>() : 0;
    float min_height = min_row_height.is<float>() ? min_row_height.as<float>() : 0.0f;
    ImGui::TableNextRow(flags, min_height);
}

bool table_next_column() {
    return ImGui::TableNextColumn();
}

bool table_set_column_index(int column_index) {
    return ImGui::TableSetColumnIndex(column_index);
}

void table_setup_column(const char* label, sol::object flags_obj, sol::object init_width_or_weight_obj, sol::object user_id_obj) {
    if (label == nullptr) {
        label = "";
    }

    auto flags = flags_obj.is<int>() ? flags_obj.as<int>() : 0;
    auto init_width = init_width_or_weight_obj.is<float>() ? init_width_or_weight_obj.as<float>() : 0.0f;
    auto user_id = user_id_obj.is<ImGuiID>() ? user_id_obj.as<ImGuiID>() : 0;

    ImGui::TableSetupColumn(label, flags, init_width, user_id);
}

void table_setup_scroll_freeze(int cols, int rows) {
    ImGui::TableSetupScrollFreeze(cols, rows);
}

void table_headers_row() {
    ImGui::TableHeadersRow();
}

void table_header(const char* label) {
    if (label == nullptr) {
        label = "";
    }

    ImGui::TableHeader(label);
}

int table_get_column_count() {
    return ImGui::TableGetColumnCount();
}

int table_get_column_index() {
    return ImGui::TableGetColumnIndex();
}

int table_get_row_index() {
    return ImGui::TableGetRowIndex();
}

const char* table_get_column_name(int column = -1) {
    return ImGui::TableGetColumnName(column);
}

ImGuiTableColumnFlags table_get_column_flags(sol::object column) {
    return ImGui::TableGetColumnFlags(column.is<int>() ? column.as<int>() : -1);
}

void table_set_bg_color(ImGuiTableBgTarget target, ImU32 color, sol::object column) {
    ImGui::TableSetBgColor(target, color, column.is<int>() ? column.as<int>() : -1);
}

void table_set_bg_color_vec4(ImGuiTableBgTarget target, Vector4f color, sol::object column) {
    ImVec4 _color = {color.x, color.y, color.z, color.w};
    ImGui::TableSetBgColor(target, ImGui::ColorConvertFloat4ToU32(_color), column.is<int>() ? column.as<int>() : -1);
}

ImGuiTableSortSpecs* table_get_sort_specs() {
    return ImGui::TableGetSortSpecs();
}

// Window Drawlist
void draw_list_path_clear() {
    if (auto dl = ImGui::GetWindowDrawList(); dl != nullptr) {
        dl->PathClear();
    }
}

void draw_list_path_line_to(sol::object pos_obj) {
    auto pos = create_imvec2(pos_obj);
    if (auto dl = ImGui::GetWindowDrawList(); dl != nullptr) {
        dl->PathLineTo(pos);
    }
}

void draw_list_path_stroke(ImU32 color, bool closed, float thickness) {
    if (auto dl = ImGui::GetWindowDrawList(); dl != nullptr) {
        dl->PathStroke(color, closed, thickness);
    }
}
} // namespace api::imgui

// Scroll APIs
namespace api::imgui {
float get_scroll_x() {
    return ImGui::GetScrollX();
}

float get_scroll_y() {
    return ImGui::GetScrollY();
}

void set_scroll_x(float scroll_x) {
    ImGui::SetScrollX(scroll_x);
}

void set_scroll_y(float scroll_y) {
    ImGui::SetScrollY(scroll_y);
}

float get_scroll_max_x() {
    return ImGui::GetScrollMaxX();
}

float get_scroll_max_y() {
    return ImGui::GetScrollMaxY();
}

void set_scroll_here_x(float center_x_ratio = 0.5f) {
    ImGui::SetScrollHereX(center_x_ratio);
}

void set_scroll_here_y(float center_y_ratio = 0.5f) {
    ImGui::SetScrollHereY(center_y_ratio);
}

void set_scroll_from_pos_x(float local_x, float center_x_ratio = 0.5f) {
    ImGui::SetScrollFromPosX(local_x, center_x_ratio);
}

void set_scroll_from_pos_y(float local_y, float center_y_ratio = 0.5f) {
    ImGui::SetScrollFromPosY(local_y, center_y_ratio);
}
} // namespace api::imgui

namespace api::draw {
std::optional<Vector2f> world_to_screen(sol::object world_pos_object) {
    // auto scene = sdk::get_current_scene();

    // if (scene == nullptr) {
    //     return std::nullopt;
    // }

    // auto context = sdk::get_thread_context();

    // static auto scene_def = sdk::find_type_definition("via.Scene");
    // auto first_transform = sdk::call_native_func_easy<RETransform*>(scene, scene_def, "get_FirstTransform");

    // if (first_transform == nullptr) {
    //     return std::nullopt;
    // }

    // if (world_pos_object.is<sol::nil_t>()) {
    //     return std::nullopt;
    // }

    // Vector4f world_pos{};

    // if (world_pos_object.is<Vector2f>()) {
    //     auto& v2f = world_pos_object.as<Vector2f&>();
    //     world_pos = Vector4f{v2f.x, v2f.y, 0.0f, 1.0f};
    // } else if (world_pos_object.is<Vector3f>()) {
    //     auto& v3f = world_pos_object.as<Vector3f&>();
    //     world_pos = Vector4f{v3f.x, v3f.y, v3f.z, 1.0f};
    // } else if (world_pos_object.is<Vector4f>()) {
    //     auto& v4f = world_pos_object.as<Vector4f&>();
    //     world_pos = Vector4f{v4f.x, v4f.y, v4f.z, v4f.w};
    // } else {
    //     return std::nullopt;
    // }

    // static auto transform_def = utility::re_managed_object::get_type_definition(first_transform);
    // static auto next_transform_method = transform_def->get_method("get_Next");
    // static auto get_gameobject_method = transform_def->get_method("get_GameObject");
    // static auto get_position_method = transform_def->get_method("get_Position");
    // static auto get_axisz_method = transform_def->get_method("get_AxisZ");
    // static auto math_t = sdk::find_type_definition("via.math");

    // auto camera = sdk::get_primary_camera();

    // if (camera == nullptr) {
    //     return std::nullopt;
    // }

    // auto main_view = sdk::get_main_view();

    // if (main_view == nullptr) {
    //     return std::nullopt;
    // }

    // auto camera_gameobject = get_gameobject_method->call<REGameObject*>(context, camera);
    // auto camera_transform = camera_gameobject->transform;

    // Vector4f camera_origin{};
    // get_position_method->call<void*>(&camera_origin, context, camera_transform);

    // camera_origin.w = 1.0f;

    // Vector4f camera_forward{};
    // get_axisz_method->call<void*>(&camera_forward, context, camera_transform);

    // camera_forward.w = 1.0f;

    // // Translate 2d position to 3d position (screen to world)
    // Matrix4x4f proj{}, view{};
    // float screen_size[2]{};
    // sdk::call_object_func<void*>(camera, "get_ProjectionMatrix", &proj, context, camera);
    // sdk::call_object_func<void*>(camera, "get_ViewMatrix", &view, context, camera);
    // sdk::call_object_func<void*>(main_view, "get_WindowSize", &screen_size, context, main_view);

    // Vector4f screen_pos{};

    // const auto delta = world_pos - camera_origin;

    // // behind camera
    // if (glm::dot(Vector3f{delta}, Vector3f{-camera_forward}) <= 0.0f) {
    //     return std::nullopt;
    // }

    // static auto world_to_screen_method = math_t->get_method("worldPos2ScreenPos(via.vec3, via.mat4, via.mat4, via.Size)"); // there are 2 of them.
    // world_to_screen_method->call<void*>(&screen_pos, context, &world_pos, &view, &proj, &screen_size);

    // return Vector2f{screen_pos.x, screen_pos.y};
    return Vector2f{0, 0};
}

void world_text(const char* text, sol::object world_pos_object, ImU32 color = 0xFFFFFFFF) {
    auto screen_pos = world_to_screen(world_pos_object);

    if (!screen_pos) {
        return;
    }

    auto draw_list = ImGui::GetBackgroundDrawList();
    draw_list->AddText(ImVec2{screen_pos->x, screen_pos->y}, color, text);
}

void text(const char* text, float x, float y, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddText(ImVec2{x, y}, color, text);
}

void filled_rect(float x, float y, float w, float h, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2{x, y}, ImVec2{x + w, y + h}, color);
}

void outline_rect(float x, float y, float w, float h, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddRect(ImVec2{x, y}, ImVec2{x + w, y + h}, color);
}

void line(float x1, float y1, float x2, float y2, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2{x1, y1}, ImVec2{x2, y2}, color);
}

void outline_circle(float x, float y, float radius, ImU32 color, sol::object num_segments) {
    auto segments = num_segments.is<sol::nil_t>() ? 32 : num_segments.as<int>();

    ImGui::GetBackgroundDrawList()->AddCircle(ImVec2{x, y}, radius, color, segments);
}

void filled_circle(float x, float y, float radius, ImU32 color, sol::object num_segments) {
    auto segments = num_segments.is<sol::nil_t>() ? 32 : num_segments.as<int>();

    ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2{x, y}, radius, color, segments);
}

void outline_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddQuad(ImVec2{x1, y1}, ImVec2{x2, y2}, ImVec2{x3, y3}, ImVec2{x4, y4}, color);
}

void filled_quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, ImU32 color) {
    ImGui::GetBackgroundDrawList()->AddQuadFilled(ImVec2{x1, y1}, ImVec2{x2, y2}, ImVec2{x3, y3}, ImVec2{x4, y4}, color);
}

void sphere(sol::object world_pos_object, float radius, ImU32 color, bool outline) {
    // Vector3f world_pos{};

    // if (world_pos_object.is<Vector2f>()) {
    //     auto& v2f = world_pos_object.as<Vector2f&>();
    //     world_pos = Vector3f{v2f.x, v2f.y, 0.0f};
    // } else if (world_pos_object.is<Vector3f>()) {
    //     auto& v3f = world_pos_object.as<Vector3f&>();
    //     world_pos = Vector3f{v3f.x, v3f.y, v3f.z};
    // } else if (world_pos_object.is<Vector4f>()) {
    //     auto& v4f = world_pos_object.as<Vector4f&>();
    //     world_pos = Vector3f{v4f.x, v4f.y, v4f.z};
    // } else {
    //     return;
    // }

    // ::imgui::draw_sphere(world_pos, radius, color, outline);
}

void capsule(sol::object start_pos_object, sol::object end_pos_object, float radius, ImU32 color, bool outline) {
    // Vector3f start_pos{};

    // if (start_pos_object.is<Vector2f>()) {
    //     auto& v2f = start_pos_object.as<Vector2f&>();
    //     start_pos = Vector3f{v2f.x, v2f.y, 0.0f};
    // } else if (start_pos_object.is<Vector3f>()) {
    //     auto& v3f = start_pos_object.as<Vector3f&>();
    //     start_pos = Vector3f{v3f.x, v3f.y, v3f.z};
    // } else if (start_pos_object.is<Vector4f>()) {
    //     auto& v4f = start_pos_object.as<Vector4f&>();
    //     start_pos = Vector3f{v4f.x, v4f.y, v4f.z};
    // } else {
    //     return;
    // }

    // Vector3f end_pos{};

    // if (end_pos_object.is<Vector2f>()) {
    //     auto& v2f = end_pos_object.as<Vector2f&>();
    //     end_pos = Vector3f{v2f.x, v2f.y, 0.0f};
    // } else if (end_pos_object.is<Vector3f>()) {
    //     auto& v3f = end_pos_object.as<Vector3f&>();
    //     end_pos = Vector3f{v3f.x, v3f.y, v3f.z};
    // } else if (end_pos_object.is<Vector4f>()) {
    //     auto& v4f = end_pos_object.as<Vector4f&>();
    //     end_pos = Vector3f{v4f.x, v4f.y, v4f.z};
    // } else {
    //     return;
    // }

    // ::imgui::draw_capsule(start_pos, end_pos, radius, color, outline);
}

sol::variadic_results gizmo(sol::this_state s, int64_t unique_id, Matrix4x4f& transform, sol::object operation_obj, sol::object mode_obj) {
    // if (!ImGui::GetIO().MouseDown[0]) {
    //     ImGuizmo::Enable(false);
    //     ImGuizmo::Enable(true);
    // }

    // ImGuizmo::OPERATION operation{};
    // ImGuizmo::MODE mode{};

    // if (mode_obj.is<sol::nil_t>()) {
    //     mode = ImGuizmo::MODE::WORLD;
    // } else if (mode_obj.is<int>()) {
    //     mode = (ImGuizmo::MODE)mode_obj.as<int>();
    // } else {
    //     throw sol::error("Invalid mode passed to gizmo");
    // }

    // if (operation_obj.is<sol::nil_t>()) {
    //     operation = ImGuizmo::OPERATION::UNIVERSAL;
    // } else if (operation_obj.is<int>()) {
    //     operation = (ImGuizmo::OPERATION)operation_obj.as<int>();
    // } else {
    //     throw sol::error("Invalid operation passed to gizmo");
    // }

    // ImGuizmo::SetID(unique_id);
    // bool changed = ::imgui::draw_gizmo(transform, operation, mode);

    // sol::variadic_results results{};

    // results.push_back(sol::make_object<bool>(s, changed));
    // results.push_back(sol::make_object<Matrix4x4f>(s, transform));

    // return results;
    sol::variadic_results results{};
    return results;
}
} // namespace api::draw


namespace api::imnodeed {

namespace ed = ax::NodeEditor;

sol::variadic_results CreateEditor(sol::this_state s, const char* name) {
    static constexpr float s_DefaultZoomLevels[] =
    {
        0.01f, 0.05f, 0.1f, 0.15f, 0.20f, 0.25f, 0.33f, 0.5f, 0.75f, 1.0f, 1.25f, 1.50f, 2.0f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f
    };
    static constexpr int s_DefaultZoomLevelCount = std::size(s_DefaultZoomLevels);

    ed::Config config;
    config.SettingsFile = name;
    for (int j = 0; j < s_DefaultZoomLevelCount; ++j) {
        config.CustomZoomLevels.push_back(s_DefaultZoomLevels[j]);
    }
    sol::variadic_results results{};
    ed::Detail::EditorContext* m_Context = reinterpret_cast<ed::Detail::EditorContext*>(ed::CreateEditor(&config));

    results.push_back(sol::make_object(s, m_Context));
    return results;
}

void DestroyEditor(sol::object context_obj) {
    if (!context_obj.is<sol::nil_t>()) {
        ed::DestroyEditor(context_obj.as<ed::EditorContext*>());
    }
}

void SetCurrentEditor(sol::object context_obj) {
    if (!context_obj.is<sol::nil_t>()) {
        ed::SetCurrentEditor(context_obj.as<ed::EditorContext*>());
    }
    else {
        ed::SetCurrentEditor(nullptr);
    }
}

void Begin(const char* name, sol::object size_obj) {
    if (name == nullptr) {
        name = "";
    }

    const auto size = api::imgui::create_imvec2(size_obj);
    ed::Begin(name, size);
}

void End() {
    ed::End();
}

void BeginNode(int id) {
    ed::BeginNode(id);
}

void BeginPin(int id, int kind) {
    ed::BeginPin(id, (ed::PinKind)kind);
}

void EndPin() {
    ed::EndPin();
}

void EndNode() {
    ed::EndNode();
}

bool Link(int id, int startPinId, int endPinId, sol::object color_obj, sol::object thickness_obj) {    
    return ed::Link(id, startPinId, endPinId);
}

void Flow(int linkId, int startPinId, int endPinId, sol::object color_obj, sol::object thickness_obj) {    
    // IMGUI_NODE_EDITOR_API void Flow(LinkId linkId, FlowDirection direction = FlowDirection::Forward);
    ed::Flow(linkId);
}

void Suspend() {
    ed::Suspend();
}
void Resume() {
    ed::Resume();
}
bool IsSuspended() {
    return ed::IsSuspended();
}
bool IsActive() {
    return ed::IsActive();
}

bool HasSelectionChanged() {
    return ed::HasSelectionChanged();
}

int GetSelectedObjectCount() {
    return ed::GetSelectedObjectCount();
}

bool BeginDelete() {
    return ed::BeginDelete();
}

sol::variadic_results QueryDeletedLink(sol::this_state s) {    
    sol::variadic_results results{};
    ed::LinkId linkId;
    bool ret = QueryDeletedLink(&linkId);

    results.push_back(sol::make_object(s, ret));
    results.push_back(sol::make_object(s, linkId.Get()));
    return results;
}
sol::variadic_results QueryDeletedNode(sol::this_state s) {    
    sol::variadic_results results{};
    ed::NodeId nodeId;
    bool ret = QueryDeletedNode(&nodeId);

    results.push_back(sol::make_object(s, ret));
    results.push_back(sol::make_object(s, nodeId.Get()));
    return results;
}

bool AcceptDeletedItem() {
    return ed::AcceptDeletedItem();
}

void RejectDeletedItem() {
    ed::RejectDeletedItem();
}

void EndDelete() {
    ed::EndDelete();
}

void SetNodePosition(int id, sol::object pos_obj) {
    const auto pos = api::imgui::create_imvec2(pos_obj);
    ed::SetNodePosition(id, pos);
}

void SetGroupSize(int id, sol::object size_obj) {
    const auto pos = api::imgui::create_imvec2(size_obj);
    ed::SetGroupSize(id, pos);
}

Vector2f GetNodePosition(int id) {
    const auto result = ed::GetNodePosition(id);

    return Vector2f{
        result.x,
        result.y,
    };
}

Vector2f GetNodeSize(int id) {
    const auto result = ed::GetNodeSize(id);

    return Vector2f{
        result.x,
        result.y,
    };
}

void CenterNodeOnScreen(int id) {
    ed::CenterNodeOnScreen(id);
}

void SetNodeZPosition(int id, float z) {
    ed::SetNodeZPosition(id, z);
}

float GetNodeZPosition(int id) {
    return ed::GetNodeZPosition(id);
}

void RestoreNodeState(int id) {
    return ed::RestoreNodeState(id);
}

void EnableShortcuts(bool enable) {
    ed::EnableShortcuts(enable);
}

auto GetHoveredNode() {
    auto id = ed::GetHoveredNode();
    return id.Get();
}

sol::variadic_results IsNodeHovered(sol::this_state s) {    
    sol::variadic_results results{};
    ed::NodeId nodeId = ed::GetHoveredNode();
    bool ret = false;
    long long id = 0;
    if (nodeId) {
        ret = true;
        id = nodeId.Get();
    }
    results.push_back(sol::make_object(s, ret));
    results.push_back(sol::make_object(s, id));
    return results;
}

}

void bindings::open_imgui(sol::state& lua) {
    auto limgui = lua.create_table();

    limgui["button"] = api::imgui::button;
    limgui["small_button"] = api::imgui::small_button;
    limgui["invisible_button"] = api::imgui::invisible_button;
    limgui["arrow_button"] = api::imgui::arrow_button;
    limgui["combo"] = api::imgui::combo;
    limgui["drag_float"] = api::imgui::drag_float;
    limgui["drag_float2"] = api::imgui::drag_float2;
    limgui["drag_float3"] = api::imgui::drag_float3;
    limgui["drag_float4"] = api::imgui::drag_float4;
    limgui["drag_int"] = api::imgui::drag_int;
    limgui["slider_float"] = api::imgui::slider_float;
    limgui["slider_int"] = api::imgui::slider_int;
    limgui["input_text"] = api::imgui::input_text;
    limgui["input_text_multiline"] = api::imgui::input_text_multiline;
    limgui["text"] = api::imgui::text;
    limgui["text_colored"] = api::imgui::text_colored;
    limgui["checkbox"] = api::imgui::checkbox;
    limgui["tree_node"] = api::imgui::tree_node;
    limgui["tree_node_ptr_id"] = api::imgui::tree_node_ptr_id;
    limgui["tree_node_str_id"] = api::imgui::tree_node_str_id;
    limgui["tree_pop"] = api::imgui::tree_pop;
    limgui["same_line"] = api::imgui::same_line;
    limgui["is_item_hovered"] = api::imgui::is_item_hovered;
    limgui["is_item_active"] = api::imgui::is_item_active;
    limgui["is_item_focused"] = api::imgui::is_item_focused;
    limgui["begin_window"] = api::imgui::begin_window;
    limgui["end_window"] = api::imgui::end_window;
    limgui["begin_child_window"] = api::imgui::begin_child_window;
    limgui["end_child_window"] = api::imgui::end_child_window;
    limgui["begin_group"] = api::imgui::begin_group;
    limgui["end_group"] = api::imgui::end_group;
    limgui["begin_rect"] = api::imgui::begin_rect;
    limgui["end_rect"] = api::imgui::end_rect;
    limgui["begin_disabled"] = api::imgui::begin_disabled;
    limgui["end_disabled"] = api::imgui::end_disabled;
    limgui["separator"] = api::imgui::separator;
    limgui["spacing"] = api::imgui::spacing;
    limgui["new_line"] = api::imgui::new_line;
    limgui["Splitter"] = api::imgui::Splitter;
    limgui["collapsing_header"] = api::imgui::collapsing_header;
    limgui["load_font"] = api::imgui::load_font;
    limgui["push_font"] = api::imgui::push_font;
    limgui["pop_font"] = api::imgui::pop_font;
    limgui["get_default_font_size"] = api::imgui::get_default_font_size;
    limgui["color_picker"] = api::imgui::color_picker;
    limgui["color_picker_argb"] = api::imgui::color_picker_argb;
    limgui["color_picker3"] = api::imgui::color_picker3;
    limgui["color_picker4"] = api::imgui::color_picker4;
    limgui["color_edit"] = api::imgui::color_edit;
    limgui["color_edit_argb"] = api::imgui::color_edit_argb;
    limgui["color_edit3"] = api::imgui::color_edit3;
    limgui["color_edit4"] = api::imgui::color_edit4;
    limgui["set_next_window_pos"] = api::imgui::set_next_window_pos;
    limgui["set_next_window_size"] = api::imgui::set_next_window_size;
    limgui["push_id"] = api::imgui::push_id;
    limgui["pop_id"] = api::imgui::pop_id;
    limgui["get_id"] = api::imgui::get_id;
    limgui["get_mouse"] = api::imgui::get_mouse;
    limgui["get_key_index"] = api::imgui::get_key_index;
    limgui["is_key_down"] = api::imgui::is_key_down;
    limgui["is_key_pressed"] = api::imgui::is_key_pressed;
    limgui["is_key_released"] = api::imgui::is_key_released;
    limgui["is_mouse_down"] = api::imgui::is_mouse_down;
    limgui["is_mouse_clicked"] = api::imgui::is_mouse_clicked;
    limgui["is_mouse_released"] = api::imgui::is_mouse_released;
    limgui["is_mouse_double_clicked"] = api::imgui::is_mouse_double_clicked;
    limgui["dummy"] = api::imgui::dummy;
    limgui["indent"] = api::imgui::indent;
    limgui["unindent"] = api::imgui::unindent;
    limgui["begin_tooltip"] = api::imgui::begin_tooltip;
    limgui["end_tooltip"] = api::imgui::end_tooltip;
    limgui["set_tooltip"] = api::imgui::set_tooltip;
    limgui["open_popup"] = api::imgui::open_popup;
    limgui["begin_popup"] = api::imgui::begin_popup;
    limgui["begin_popup_context_item"] = api::imgui::begin_popup_context_item;
    limgui["end_popup"] = api::imgui::end_popup;
    limgui["close_current_popup"] = api::imgui::close_current_popup;
    limgui["is_popup_open"] = api::imgui::is_popup_open;
    limgui["calc_text_size"] = api::imgui::calc_text_size;
    limgui["get_window_size"] = api::imgui::get_window_size;
    limgui["get_window_pos"] = api::imgui::get_window_pos;
    limgui["set_next_item_open"] = api::imgui::set_next_item_open;
    limgui["begin_list_box"] = api::imgui::begin_list_box;
    limgui["end_list_box"] = api::imgui::end_list_box;
    limgui["begin_menu_bar"] = api::imgui::begin_menu_bar;
    limgui["end_menu_bar"] = api::imgui::end_menu_bar;
    limgui["begin_main_menu_bar"] = api::imgui::begin_main_menu_bar;
    limgui["end_main_menu_bar"] = api::imgui::end_main_menu_bar;
    limgui["begin_menu"] = api::imgui::begin_menu;
    limgui["end_menu"] = api::imgui::end_menu;
    limgui["menu_item"] = api::imgui::menu_item;
    limgui["get_display_size"] = api::imgui::get_display_size;

    // Item
    limgui["push_item_width"] = api::imgui::push_item_width;
    limgui["pop_item_width"] = api::imgui::pop_item_width;
    limgui["set_next_item_width"] = api::imgui::set_next_item_width;
    limgui["calc_item_width"] = api::imgui::calc_item_width;
    limgui["item_add"] = api::imgui::item_add;
    limgui["item_size"] = api::imgui::item_size;

    limgui["push_style_color"] = api::imgui::push_style_color;
    limgui["pop_style_color"] = api::imgui::pop_style_color;
    limgui["push_style_var"] = api::imgui::push_style_var;
    limgui["pop_style_var"] = api::imgui::pop_style_var;
    limgui["get_cursor_pos"] = api::imgui::get_cursor_pos;
    limgui["set_cursor_pos"] = api::imgui::set_cursor_pos;
    limgui["get_cursor_start_pos"] = api::imgui::get_cursor_start_pos;
    limgui["get_cursor_screen_pos"] = api::imgui::get_cursor_screen_pos;
    limgui["set_cursor_screen_pos"] = api::imgui::set_cursor_screen_pos;
    limgui["set_item_default_focus"] = api::imgui::set_item_default_focus;
    limgui["set_clipboard"] = api::imgui::set_clipboard;
    limgui["get_clipboard"] = api::imgui::get_clipboard;
    limgui["progress_bar"] = api::imgui::progress_bar;

    // Draw list
    limgui["draw_list_path_clear"] = api::imgui::draw_list_path_clear;
    limgui["draw_list_path_line_to"] = api::imgui::draw_list_path_line_to;
    limgui["draw_list_path_stroke"] = api::imgui::draw_list_path_stroke;
    
    // SCROLL APIs
    limgui["get_scroll_x"] = api::imgui::get_scroll_x;
    limgui["get_scroll_y"] = api::imgui::get_scroll_y;
    limgui["set_scroll_x"] = api::imgui::set_scroll_x;
    limgui["set_scroll_y"] = api::imgui::set_scroll_y;
    limgui["get_scroll_max_x"] = api::imgui::get_scroll_max_x;
    limgui["get_scroll_max_y"] = api::imgui::get_scroll_max_y;
    limgui["set_scroll_here_x"] = api::imgui::set_scroll_here_x;
    limgui["set_scroll_here_y"] = api::imgui::set_scroll_here_y;
    limgui["set_scroll_from_pos_x"] = api::imgui::set_scroll_from_pos_x;
    limgui["set_scroll_from_pos_y"] = api::imgui::set_scroll_from_pos_y;


    // TABLE APIS
    limgui["begin_table"] = api::imgui::begin_table;
    limgui["end_table"] = api::imgui::end_table;
    limgui["table_next_row"] = api::imgui::table_next_row;
    limgui["table_next_column"] = api::imgui::table_next_column;
    limgui["table_set_column_index"] = api::imgui::table_set_column_index;
    limgui["table_setup_column"] = api::imgui::table_setup_column;
    limgui["table_setup_scroll_freeze"] = api::imgui::table_setup_scroll_freeze;
    limgui["table_headers_row"] = api::imgui::table_headers_row;
    limgui["table_header"] = api::imgui::table_header;
    limgui["table_get_sort_specs"] = api::imgui::table_get_sort_specs;
    limgui["table_get_column_count"] = api::imgui::table_get_column_count;
    limgui["table_get_column_index"] = api::imgui::table_get_column_index;
    limgui["table_get_row_index"] = api::imgui::table_get_row_index;
    limgui["table_get_column_name"] = api::imgui::table_get_column_name;
    limgui["table_get_column_flags"] = api::imgui::table_get_column_flags;
    limgui["table_set_bg_color"] = api::imgui::table_set_bg_color;
    // limgui.new_enum("ImGuizmoOperation", 
    //                 "TRANSLATE", ImGuizmo::OPERATION::TRANSLATE, 
    //                 "ROTATE", ImGuizmo::OPERATION::ROTATE,
    //                 "SCALE", ImGuizmo::OPERATION::SCALE,
    //                 "SCALEU", ImGuizmo::OPERATION::SCALEU,
    //                 "UNIVERSAL", ImGuizmo::OPERATION::UNIVERSAL);
    // limgui.new_enum("ImGuizmoMode", 
    //                 "WORLD", ImGuizmo::MODE::WORLD,
    //                 "LOCAL", ImGuizmo::MODE::LOCAL);
    limgui.new_usertype<ImGuiTableSortSpecs>("TableSortSpecs",
        "specs_dirty", &ImGuiTableSortSpecs::SpecsDirty,
        "get_specs", [](ImGuiTableSortSpecs* specs) {
            std::vector<ImGuiTableColumnSortSpecs*> out {};

            for (int i = 0; i < specs->SpecsCount; ++i) {
                out.push_back(const_cast<ImGuiTableColumnSortSpecs*>(specs->Specs + i));
            }

            return out;
        }
    );
    limgui.new_usertype<ImGuiTableColumnSortSpecs>("TableColumnSortSpecs", 
        "user_id", &ImGuiTableColumnSortSpecs::ColumnUserID,
        "column_index", &ImGuiTableColumnSortSpecs::ColumnIndex,
        "sort_order", &ImGuiTableColumnSortSpecs::SortOrder,
        "sort_direction", sol::readonly_property([](ImGuiTableColumnSortSpecs* specs){
            return specs->SortDirection;
        })
    );
    limgui.new_enum("TableFlags",
        "None", ImGuiTableFlags_None,
        "Resizable", ImGuiTableFlags_Resizable,
        "Reorderable", ImGuiTableFlags_Reorderable,
        "Hideable", ImGuiTableFlags_Hideable,
        "Sortable", ImGuiTableFlags_Sortable,
        "NoSavedSettings", ImGuiTableFlags_NoSavedSettings,
        "ContextMenuInBody", ImGuiTableFlags_ContextMenuInBody,
        "RowBg", ImGuiTableFlags_RowBg,
        "BordersInnerH", ImGuiTableFlags_BordersInnerH,
        "BordersOuterH", ImGuiTableFlags_BordersOuterH,
        "BordersInnerV", ImGuiTableFlags_BordersInnerV,
        "BordersOuterV", ImGuiTableFlags_BordersOuterV,
        "BordersH", ImGuiTableFlags_BordersH,
        "BordersV", ImGuiTableFlags_BordersV,
        "BordersInner", ImGuiTableFlags_BordersInner,
        "BordersOuter", ImGuiTableFlags_BordersOuter,
        "Borders", ImGuiTableFlags_Borders,
        "NoBordersInBody", ImGuiTableFlags_NoBordersInBody,
        "NoBordersInBodyUntilResize", ImGuiTableFlags_NoBordersInBodyUntilResize,
        "SizingFixedFit", ImGuiTableFlags_SizingFixedFit,
        "SizingFixedSame", ImGuiTableFlags_SizingFixedSame,
        "SizingStretchProp", ImGuiTableFlags_SizingStretchProp,
        "SizingStretchSame", ImGuiTableFlags_SizingStretchSame,
        "NoHostExtendX", ImGuiTableFlags_NoHostExtendX,
        "NoHostExtendY", ImGuiTableFlags_NoHostExtendY,
        "NoKeepColumnsVisible", ImGuiTableFlags_NoKeepColumnsVisible,
        "PreciseWidths", ImGuiTableFlags_PreciseWidths,
        "NoClip", ImGuiTableFlags_NoClip,
        "PadOuterX", ImGuiTableFlags_PadOuterX,
        "NoPadOuterX", ImGuiTableFlags_NoPadOuterX,
        "NoPadInnerX", ImGuiTableFlags_NoPadInnerX,
        "ScrollX", ImGuiTableFlags_ScrollX,
        "ScrollY", ImGuiTableFlags_ScrollY,
        "SortMulti", ImGuiTableFlags_SortMulti,
        "SortTristate", ImGuiTableFlags_SortTristate
    );
    limgui.new_enum("ColumnFlags",
        "None", ImGuiTableColumnFlags_None,                
        "DefaultHide", ImGuiTableColumnFlags_DefaultHide,         
        "DefaultSort", ImGuiTableColumnFlags_DefaultSort,         
        "WidthStretch", ImGuiTableColumnFlags_WidthStretch,        
        "WidthFixed", ImGuiTableColumnFlags_WidthFixed,          
        "NoResize", ImGuiTableColumnFlags_NoResize,            
        "NoReorder", ImGuiTableColumnFlags_NoReorder,           
        "NoHide", ImGuiTableColumnFlags_NoHide,              
        "NoClip", ImGuiTableColumnFlags_NoClip,              
        "NoSort", ImGuiTableColumnFlags_NoSort,              
        "NoSortAscending", ImGuiTableColumnFlags_NoSortAscending,     
        "NoSortDescending", ImGuiTableColumnFlags_NoSortDescending,    
        "NoHeaderWidth", ImGuiTableColumnFlags_NoHeaderWidth,       
        "PreferSortAscending", ImGuiTableColumnFlags_PreferSortAscending, 
        "PreferSortDescending", ImGuiTableColumnFlags_PreferSortDescending,
        "IndentEnable", ImGuiTableColumnFlags_IndentEnable,        
        "IndentDisable", ImGuiTableColumnFlags_IndentDisable,       
        "IsEnabled", ImGuiTableColumnFlags_IsEnabled,           
        "IsVisible", ImGuiTableColumnFlags_IsVisible,           
        "IsSorted", ImGuiTableColumnFlags_IsSorted,            
        "IsHovered", ImGuiTableColumnFlags_IsHovered           
    );

    lua["imgui"] = limgui;

    // auto imguizmo = lua.create_table();

    // imguizmo["is_over"] = [] { return ImGuizmo::IsOver(); };
    // imguizmo["is_using"] = [] { return ImGuizmo::IsUsing(); };

    // lua["imguizmo"] = imguizmo;

    auto draw = lua.create_table();

    draw["world_to_screen"] = api::draw::world_to_screen;
    draw["world_text"] = api::draw::world_text;
    draw["text"] = api::draw::text;
    draw["filled_rect"] = api::draw::filled_rect;
    draw["outline_rect"] = api::draw::outline_rect;
    draw["line"] = api::draw::line;
    draw["outline_circle"] = api::draw::outline_circle;
    draw["filled_circle"] = api::draw::filled_circle;
    draw["outline_quad"] = api::draw::outline_quad;
    draw["filled_quad"] = api::draw::filled_quad;
    draw["sphere"] = api::draw::sphere;
    draw["capsule"] = api::draw::capsule;
    draw["gizmo"] = api::draw::gizmo;
    // draw["cube"] = [](const Matrix4x4f& mat) { imgui::draw_cube(mat); };
    // draw["grid"] = [](const Matrix4x4f& mat, float size) { imgui::draw_grid(mat, size); };
    lua["draw"] = draw;

    // auto imnodes = lua.create_table();

    // imnodes["is_editor_hovered"] = &api::imnodes::is_editor_hovered;
    // imnodes["is_node_hovered"] = &api::imnodes::is_node_hovered;
    // imnodes["is_link_hovered"] = &api::imnodes::is_link_hovered;
    // imnodes["is_pin_hovered"] = &api::imnodes::is_pin_hovered;
    // imnodes["begin_node_editor"] = &api::imnodes::begin_node_editor;
    // imnodes["end_node_editor"] = &api::imnodes::end_node_editor;
    // imnodes["begin_node"] = &api::imnodes::begin_node;
    // imnodes["end_node"] = &api::imnodes::end_node;
    // imnodes["begin_node_titlebar"] = &api::imnodes::begin_node_titlebar;
    // imnodes["end_node_titlebar"] = &api::imnodes::end_node_titlebar;
    // imnodes["begin_input_attribute"] = &api::imnodes::begin_input_attribute;
    // imnodes["end_input_attribute"] = &api::imnodes::end_input_attribute;
    // imnodes["begin_output_attribute"] = &api::imnodes::begin_output_attribute;
    // imnodes["end_output_attribute"] = &api::imnodes::end_output_attribute;
    // imnodes["begin_static_attribute"] = &api::imnodes::begin_static_attribute;
    // imnodes["end_static_attribute"] = &api::imnodes::end_static_attribute;
    // imnodes["minimap"] = &api::imnodes::minimap;
    // imnodes["link"] = &api::imnodes::link;
    // imnodes["get_node_dimensions"] = &api::imnodes::get_node_dimensions;
    // imnodes["push_color_style"] = &api::imnodes::push_color_style;
    // imnodes["pop_color_style"] = &api::imnodes::pop_color_style;
    // imnodes["push_style_var"] = &api::imnodes::push_style_var;
    // imnodes["push_style_var_vec2"] = &api::imnodes::push_style_var_vec2;
    // imnodes["pop_style_var"] = &api::imnodes::pop_style_var;
    // imnodes["set_node_screen_space_pos"] = &api::imnodes::set_node_screen_space_pos;
    // imnodes["set_node_editor_space_pos"] = &api::imnodes::set_node_editor_space_pos;
    // imnodes["set_node_grid_space_pos"] = &api::imnodes::set_node_grid_space_pos;
    // imnodes["get_node_screen_space_pos"] = &api::imnodes::get_node_screen_space_pos;
    // imnodes["get_node_editor_space_pos"] = &api::imnodes::get_node_editor_space_pos;
    // imnodes["get_node_grid_space_pos"] = &api::imnodes::get_node_grid_space_pos;
    // imnodes["is_editor_hovered"] = &ImNodes::IsEditorHovered;
    // imnodes["is_node_selected"] = &ImNodes::IsNodeSelected;
    // imnodes["is_link_selected"] = &ImNodes::IsLinkSelected;
    // imnodes["num_selected_nodes"] = &ImNodes::NumSelectedNodes;
    // imnodes["num_selected_links"] = &ImNodes::NumSelectedLinks;
    // imnodes["clear_node_selection"] = &api::imnodes::clear_node_selection;
    // imnodes["clear_link_selection"] = &api::imnodes::clear_link_selection;
    // imnodes["select_node"] = &ImNodes::SelectNode;
    // imnodes["select_link"] = &ImNodes::SelectLink;
    // imnodes["is_attribute_active"] = &ImNodes::IsAttributeActive;
    // imnodes["snap_node_to_grid"] = &ImNodes::SnapNodeToGrid;

    // imnodes["editor_move_to_node"] = &ImNodes::EditorContextMoveToNode;
    // imnodes["editor_reset_panning"] = &api::imnodes::editor_reset_panning;
    // imnodes["editor_get_panning"] = &api::imnodes::editor_get_panning;

    // imnodes["is_link_started"] = &api::imnodes::is_link_started;
    // imnodes["is_link_dropped"] = &api::imnodes::is_link_dropped;
    // imnodes["is_link_created"] = &api::imnodes::is_link_created;
    // imnodes["is_link_destroyed"] = &api::imnodes::is_link_destroyed;

    // imnodes["get_selected_nodes"] = &api::imnodes::get_selected_nodes;
    // imnodes["get_selected_links"] = &api::imnodes::get_selected_links;

    // lua["imnodes"] = imnodes;

    auto imnodeed = lua.create_table();
    imnodeed["CreateEditor"] = &api::imnodeed::CreateEditor;
    imnodeed["DestroyEditor"] = &api::imnodeed::DestroyEditor;
    imnodeed["SetCurrentEditor"] = &api::imnodeed::SetCurrentEditor;
    imnodeed["Begin"] = &api::imnodeed::Begin;
    imnodeed["End"] = &api::imnodeed::End;
    imnodeed["BeginNode"] = &api::imnodeed::BeginNode;
    imnodeed["BeginPin"] = &api::imnodeed::BeginPin;
    imnodeed["EndPin"] = &api::imnodeed::EndPin;
    imnodeed["EndNode"] = &api::imnodeed::EndNode;

    imnodeed["Link"] = &api::imnodeed::Link;
    imnodeed["Flow"] = &api::imnodeed::Flow;
    
    imnodeed["Suspend"] = &api::imnodeed::Suspend;
    imnodeed["Resume"] = &api::imnodeed::Resume;
    imnodeed["IsSuspended"] = &api::imnodeed::IsSuspended;
    imnodeed["IsActive"] = &api::imnodeed::IsActive;
    imnodeed["HasSelectionChanged"] = &api::imnodeed::HasSelectionChanged;
    imnodeed["GetSelectedObjectCount"] = &api::imnodeed::GetSelectedObjectCount;

    imnodeed["BeginDelete"] = &api::imnodeed::BeginDelete;
    imnodeed["QueryDeletedLink"] = &api::imnodeed::QueryDeletedLink;
    imnodeed["QueryDeletedNode"] = &api::imnodeed::QueryDeletedNode;
    imnodeed["AcceptDeletedItem"] = &api::imnodeed::AcceptDeletedItem;
    imnodeed["RejectDeletedItem"] = &api::imnodeed::RejectDeletedItem;
    imnodeed["EndDelete"] = &api::imnodeed::EndDelete;
    
    imnodeed["SetNodePosition"] = &api::imnodeed::SetNodePosition;
    imnodeed["SetGroupSize"] = &api::imnodeed::SetGroupSize;
    imnodeed["GetNodePosition"] = &api::imnodeed::GetNodePosition;
    imnodeed["GetNodeSize"] = &api::imnodeed::GetNodeSize;
    imnodeed["CenterNodeOnScreen"] = &api::imnodeed::CenterNodeOnScreen;
    imnodeed["SetNodeZPosition"] = &api::imnodeed::SetNodeZPosition;
    imnodeed["GetNodeZPosition"] = &api::imnodeed::GetNodeZPosition;
    imnodeed["RestoreNodeState"] = &api::imnodeed::RestoreNodeState;
    
    imnodeed["EnableShortcuts"] = &api::imnodeed::EnableShortcuts;
    
    imnodeed["GetHoveredNode"] = &api::imnodeed::GetHoveredNode;
    imnodeed["IsNodeHovered"] = &api::imnodeed::IsNodeHovered;
    
    lua["imnodeed"] = imnodeed;
}
