// ===================================================================================================================
// Widget Example
// Drawing standard ImGui widgets inside the node body
//
// First, some unsorted notes about which widgets do and don't draw well inside nodes.  Run the examples to see all the allowed widgets.
//
// - Child windows with scrolling doesn't work in the node.  The child window appears in a normal node,
//   and scrolls, but its contents are floating around in the wrong location, and they are not scaled.
//   Note that you can put scrolling child windows into "deferred popups" (see next item).
// - Listboxes and combo-boxes only work in nodes with a work-around: deferring the popup calls until after the node drawing is
//   completed. Look to the popup-demo for an example.
// - Headers and trees work inside the nodes only with hacks.  This is because they attempt to span the "avaialbe width"
//   and the nodes can't tell these widgets how wide it is. The work-around is to set up a fake
//   table with a static column width, then draw your header and tree widgets in that column.
// - Clickable tabs don't work in nodes.  Tabs appear, but you cannot actually change the tab, so they're functionally useless.
// - Editable text areas work, but you have to manually manage disabling the editor shorcuts while typing is detected.
//   Look around for the call to ed::EnableShortcuts() for an example.
// - Most of the cool graph widgets can't be used because they are hard-coded in ImGui to spawn tooltips, which don't work.

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_node_editor.h>
# include <application.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <fmt/core.h>

#include <fstream>
#include <iostream>

#include "Math.hpp"

#include "bindings/ImGui.hpp"
#include "bindings/FS.hpp"
#include "bindings/Json.hpp"

namespace ed = ax::NodeEditor;

# ifdef _MSC_VER
# define portable_strcpy    strcpy_s
# define portable_sprintf   sprintf_s
# else
# define portable_strcpy    strcpy
# define portable_sprintf   sprintf
# endif

struct App:
    public Application
{
    using Application::Application;

    struct LinkInfo
    {
        ed::LinkId Id;
        ed::PinId  InputId;
        ed::PinId  OutputId;
    };

    void OnStart() override
    {
        ed::Config config;
        config.SettingsFile = "Widgets.json";
        m_Context = ed::CreateEditor(&config);
        
        m_lua.open_libraries(
            sol::lib::base,
            sol::lib::package,
            sol::lib::string,
            sol::lib::math,
            sol::lib::table,
            sol::lib::bit32,
            sol::lib::utf8,
            sol::lib::os,
            sol::lib::debug,
            sol::lib::coroutine
        );

        bindings::open_imgui(m_lua);
        bindings::open_json(m_lua);
        bindings::open_fs(m_lua);

        // clang-format off
    // add vec2 usertype
    m_lua.new_usertype<Vector2f>("Vector2f",
        sol::meta_function::construct, sol::constructors<Vector4f(float, float)>(),
        "clone", [](Vector2f& v) -> Vector2f { return v; },
        "x", &Vector2f::x, 
        "y", &Vector2f::y, 
        "dot", [](Vector2f& v1, Vector2f& v2) { return glm::dot(v1, v2); },
        "length", [](Vector2f& v) { return glm::length(v); },
        "normalize", [](Vector2f& v) { v = glm::normalize(v); },
        "normalized", [](Vector2f& v) { return glm::normalize(v); },
        sol::meta_function::addition, [](Vector2f& lhs, Vector2f& rhs) { return lhs + rhs; },
        sol::meta_function::subtraction, [](Vector2f& lhs, Vector2f& rhs) { return lhs - rhs; },
        sol::meta_function::multiplication, [](Vector2f& lhs, float scalar) { return lhs * scalar; },
        "to_vec3", [](Vector2f& v) { return Vector3f{v.x, v.y, 0.0f}; },
        "to_vec4", [](Vector2f& v) { return Vector4f{v.x, v.y, 0.0f, 0.0f}; });

    // add vec3 usertype
    m_lua.new_usertype<Vector3f>("Vector3f",
        sol::meta_function::construct, sol::constructors<Vector4f(float, float, float)>(),
        "clone", [](Vector3f& v) -> Vector3f { return v; },
        "x", &Vector3f::x,
        "y", &Vector3f::y,
        "z", &Vector3f::z,
        "dot", [](Vector3f& v1, Vector3f& v2) { return glm::dot(v1, v2); },
        "cross", [](Vector3f& v1, Vector3f& v2) { return glm::cross(v1, v2); },
        "length", [](Vector3f& v) { return glm::length(v); },
        "normalize", [](Vector3f& v) { v = glm::normalize(v); },
        "normalized", [](Vector3f& v) { return glm::normalize(v); },
        "reflect", [](Vector3f& v, Vector3f& normal) { return glm::reflect(v, normal); },
        "refract", [](Vector3f& v, Vector3f& normal, float eta) { return glm::refract(v, normal, eta); },
        "lerp", [](Vector3f& v1, Vector3f& v2, float t) { return glm::lerp(v1, v2, t); },
        sol::meta_function::addition, [](Vector3f& lhs, Vector3f& rhs) { return lhs + rhs; },
        sol::meta_function::subtraction, [](Vector3f& lhs, Vector3f& rhs) { return lhs - rhs; },
        sol::meta_function::multiplication, [](Vector3f& lhs, float scalar) { return lhs * scalar; },
        "to_vec2", [](Vector3f& v) { return Vector2f{v.x, v.y}; },
        "to_vec4", [](Vector3f& v) { return Vector4f{v.x, v.y, v.z, 0.0f}; },
        "to_mat", [](Vector3f& v) { return glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, v, Vector3f{0.0f, 1.0f, 0.0f})); },
        "to_quat", [](Vector3f& v) { 
            auto mat = glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, v, Vector3f{0.0f, 1.0f, 0.0f}));

            return glm::quat{mat};
        });

    // add vec4 usertype
    m_lua.new_usertype<Vector4f>("Vector4f",
        sol::meta_function::construct, sol::constructors<Vector4f(float, float, float, float)>(),
        "clone", [](Vector4f& v) -> Vector4f { return v; },
        "x", &Vector4f::x,
        "y", &Vector4f::y,
        "z", &Vector4f::z,
        "w", &Vector4f::w,
        "dot", [](Vector4f& v1, Vector4f& v2) { return glm::dot(v1, v2); },
        "cross", [](Vector4f& v1, Vector4f& v2) { return glm::cross(Vector3f{v1.x, v1.y, v1.z}, Vector3f{v2.x, v2.y, v2.z}); },
        "length", [](Vector4f& v) { return glm::length(v); },
        "normalize", [](Vector4f& v) { v = glm::normalize(v); },
        "normalized", [](Vector4f& v) { return glm::normalize(v); },
        "reflect", [](Vector4f& v, Vector4f& normal) { return glm::reflect(v, normal); },
        "refract", [](Vector4f& v, Vector4f& normal, float eta) { return glm::refract(v, normal, eta); },
        "lerp", [](Vector4f& v1, Vector4f& v2, float t) { return glm::lerp(v1, v2, t); },
        sol::meta_function::addition, [](Vector4f& lhs, Vector4f& rhs) { return lhs + rhs; },
        sol::meta_function::subtraction, [](Vector4f& lhs, Vector4f& rhs) { return lhs - rhs; },
        sol::meta_function::multiplication, [](Vector4f& lhs, float scalar) { return lhs * scalar; },
        "to_vec2", [](Vector4f& v) { return Vector2f{v.x, v.y}; },
        "to_vec3", [](Vector4f& v) { return Vector3f{v.x, v.y, v.z}; },
        "to_mat", [](Vector4f& v) { return glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{v.x, v.y, v.z}, Vector3f{0.0f, 1.0f, 0.0f})); },
        "to_quat", [](Vector4f& v) { 
            auto mat = glm::rowMajor4(glm::lookAtLH(Vector3f{0.0f, 0.0f, 0.0f}, Vector3f{v.x, v.y, v.z}, Vector3f{0.0f, 1.0f, 0.0f}));

            return glm::quat{mat};
        });

    // add Matrix4x4f (glm::mat4) usertype
    m_lua.new_usertype<Matrix4x4f>("Matrix4x4f",
        sol::meta_function::construct, 
         sol::constructors<
         Matrix4x4f(),
         Matrix4x4f(const Vector4f&, const Vector4f&, const Vector4f&, const Vector4f&),
         Matrix4x4f(float, float, float, float,
                    float, float, float, float,
                    float, float, float, float,
                    float, float, float, float)
        >(),
        "clone", [](Matrix4x4f& m) -> Matrix4x4f { return m; },
        "identity", []() { return glm::identity<Matrix4x4f>(); },
        "to_quat", [] (Matrix4x4f& m) { return glm::quat(m); },
        "inverse", [] (Matrix4x4f& m) { return glm::inverse(m); },
        "invert", [] (Matrix4x4f& m) { m = glm::inverse(m); },
        "interpolate", [](Matrix4x4f& m1, Matrix4x4f& m2, float t) { return glm::interpolate(m1, m2, t); },
        "matrix_rotation", [](Matrix4x4f& m) { return glm::extractMatrixRotation(m); },
        sol::meta_function::multiplication, sol::overload(
            [](Matrix4x4f& lhs, Matrix4x4f& rhs) {
                return lhs * rhs;
            },
            [](Matrix4x4f& lhs, Vector4f& rhs) {

                return lhs * rhs;
            }
        ),
        sol::meta_function::index, [](sol::this_state s, Matrix4x4f& lhs, sol::object index_obj) -> sol::object {
            if (!index_obj.is<int>()) {
                return sol::make_object(s, sol::lua_nil);
            }

            const auto index = index_obj.as<int>();

            if (index >= 4) {
                return sol::make_object(s, sol::lua_nil);
            }

            return sol::make_object(s, &lhs[index]);
        },
        sol::meta_function::new_index, [](Matrix4x4f& lhs, int index, Vector4f& rhs) {
            lhs[index] = rhs;
        }
    );

    // add glm::quat usertype
    m_lua.new_usertype<glm::quat>("Quaternion",
        sol::meta_function::construct, sol::constructors<glm::quat(), glm::quat(float, float, float, float), glm::quat(const Vector3f&)>(),
        "clone", [](glm::quat& q) -> glm::quat { return q; },
        "identity", []() { return glm::identity<glm::quat>(); },
        "x", &glm::quat::x,
        "y", &glm::quat::y,
        "z", &glm::quat::z,
        "w", &glm::quat::w,
        "to_mat4", [](glm::quat& q) { return Matrix4x4f{q}; },
        "inverse", [](glm::quat& q) { return glm::inverse(q); },
        "invert", [](glm::quat& q) { q = glm::inverse(q); },
        "normalize", [](glm::quat& q) { q = glm::normalize(q); },
        "normalized", [](glm::quat& q) { return glm::normalize(q); },
        "slerp", [](glm::quat& q1, glm::quat& q2, float t) { return glm::slerp(q1, q2, t); },
        "dot", [](glm::quat& q1, glm::quat& q2) { return glm::dot(q1, q2); },
        "length", [](glm::quat& q) { return glm::length(q); },
        "conjugate", [](glm::quat& q) { return glm::conjugate(q); },
        sol::meta_function::multiplication, sol::overload( 
            [](glm::quat& lhs, glm::quat& rhs) -> glm::quat {
                return lhs * rhs;
            },
            [](glm::quat& lhs, Vector3f& rhs) -> Vector3f {
                return lhs * rhs;
            },
            [](glm::quat& lhs, Vector4f& rhs) -> Vector4f {
                return lhs * rhs;
            }
        ),
        sol::meta_function::index, [](sol::this_state s, glm::quat& lhs, sol::object index_obj) -> sol::object {
            if (!index_obj.is<int>()) {
                return sol::make_object(s, sol::lua_nil);
            }

            const auto index = index_obj.as<int>();

            if (index >= 4) {
                return sol::make_object(s, sol::lua_nil);
            }

            return sol::make_object(s, lhs[index]);
        },
        sol::meta_function::new_index, [](glm::quat& lhs, int index, float rhs) {
            lhs[index] = rhs;
        }
    );

        m_lua.script_file("data/actionEditor.lua");
    }

    void OnStop() override
    {
        ed::DestroyEditor(m_Context);
    }

    void OnFrame(float deltaTime) override
    {
        auto& io = ImGui::GetIO();

        // FPS Counter Ribbon
        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
        ImGui::Separator();
        m_lua["actionEditorFunc"]();
    }

    ed::EditorContext* m_Context = nullptr;
    sol::state m_lua{};
};

int Main(int argc, char** argv)
{
    App app("Widgets", argc, argv);

    if (app.Create())
        return app.Run();

    return 0;
}