



function G_LUA_ON_ERROR(err)
    print("LUA ERROR: ", err, debug.traceback(""))
end

-- 枚举ID生成器创建函数
function enumIDGeneratorCreator(initID, initStep)
    local baseID = tonumber(initID) or 0
    local step = tonumber(initStep) or 1
    local function enumIDGenerator(newID)
        local newID = tonumber(newID)
        if newID and newID <= baseID then
            print("enumIDGeneratorCreator newID <= baseID")
        end
        local nowID = newID or (baseID + step)
        baseID = nowID
        return nowID
    end

    return enumIDGenerator
end


-- 枚举ID生成器
local enumID = enumIDGeneratorCreator()



local ImGuiStyleVar =
{
    --Enum name --------------------- --Member in ImGuiStyle structure (see ImGuiStyle for descriptions)
    ImGuiStyleVar_Alpha = enumID(),                   -- // float     Alpha
    ImGuiStyleVar_DisabledAlpha = enumID(),            -- // float     DisabledAlpha
    ImGuiStyleVar_WindowPadding = enumID(),            -- // ImVec2    WindowPadding
    ImGuiStyleVar_WindowRounding = enumID(),           -- // float     WindowRounding
    ImGuiStyleVar_WindowBorderSize = enumID(),         -- // float     WindowBorderSize
    ImGuiStyleVar_WindowMinSize = enumID(),            -- // ImVec2    WindowMinSize
    ImGuiStyleVar_WindowTitleAlign = enumID(),         -- // ImVec2    WindowTitleAlign
    ImGuiStyleVar_ChildRounding = enumID(),            -- // float     ChildRounding
    ImGuiStyleVar_ChildBorderSize = enumID(),          -- // float     ChildBorderSize
    ImGuiStyleVar_PopupRounding = enumID(),            -- // float     PopupRounding
    ImGuiStyleVar_PopupBorderSize = enumID(),          -- // float     PopupBorderSize
    ImGuiStyleVar_FramePadding = enumID(),             -- // ImVec2    FramePadding
    ImGuiStyleVar_FrameRounding = enumID(),            -- // float     FrameRounding
    ImGuiStyleVar_FrameBorderSize = enumID(),          -- // float     FrameBorderSize
    ImGuiStyleVar_ItemSpacing = enumID(),              -- // ImVec2    ItemSpacing
    ImGuiStyleVar_ItemInnerSpacing = enumID(),         -- // ImVec2    ItemInnerSpacing
    ImGuiStyleVar_IndentSpacing = enumID(),            -- // float     IndentSpacing
    ImGuiStyleVar_CellPadding = enumID(),              -- // ImVec2    CellPadding
    ImGuiStyleVar_ScrollbarSize = enumID(),            -- // float     ScrollbarSize
    ImGuiStyleVar_ScrollbarRounding = enumID(),        -- // float     ScrollbarRounding
    ImGuiStyleVar_GrabMinSize = enumID(),              -- // float     GrabMinSize
    ImGuiStyleVar_GrabRounding = enumID(),             -- // float     GrabRounding
    ImGuiStyleVar_TabRounding = enumID(),              -- // float     TabRounding
    ImGuiStyleVar_TabBarBorderSize = enumID(),         -- // float     TabBarBorderSize
    ImGuiStyleVar_ButtonTextAlign = enumID(),          -- // ImVec2    ButtonTextAlign
    ImGuiStyleVar_SelectableTextAlign = enumID(),      -- // ImVec2    SelectableTextAlign
    ImGuiStyleVar_SeparatorTextBorderSize = enumID(),  -- // float  SeparatorTextBorderSize
    ImGuiStyleVar_SeparatorTextAlign = enumID(),       -- // ImVec2    SeparatorTextAlign
    ImGuiStyleVar_SeparatorTextPadding = enumID(),     -- // ImVec2    SeparatorTextPadding
    ImGuiStyleVar_COUNT = enumID()
};

local PinKind = 
{
    Input = 0,
    Output = 1
};


local EDITOR_SIZE = {
    x = math.max(imgui.get_display_size().x / 4, 640),
    y = math.max(imgui.get_display_size().y / 2, 480)
}


local m_Context = nil
local actionTree = nil
local file_path = "pl0800_vergilPL_saved_tree.json"

local function load_action_file(filePath)
    if not filePath then
        print("filePath is nil")
        return
    end
    local data = json.load_file(filePath)
    if data then
        actionTree = data
    end
end


local function draw_node_action(id, name)
    imnodeed.BeginNode(id)
    imgui.text(name)
    imnodeed.EndNode()
end

local function draw_tree_node(node)
    if not node then
        return
    end
    draw_node_action(node.id, node.name)
    local children = node.children
    if children then
        local node_pos = imnodeed.GetNodePosition(node.id)
        local node_dims = imnodeed.GetNodeSize(node.id)
        for j, child_id in ipairs(children) do
            local child = actionTree.nodes[child_id + 1]
            -- Y needs to be dynamic
            local child_render_pos = {
                x = node_pos.x + 50,
                y = node_pos.y + j*50
                --y = node_pos.y - ((#node_descriptor.children - 1) * (node_dims.y / 2)) + node_dim_requirements.y + ((j-1) * node_dims.y)
            }

            imnodeed.SetNodePosition(child.id, {child_render_pos.x, child_render_pos.y})
            draw_tree_node(child)
        end
    end
end

local function draw_tree()
    if not actionTree then
        imgui.text("actionTree is nil")
        return
    end
    local rootNode = actionTree.nodes[1]
    if rootNode then
        -- imnodeed.SetNodePosition(rootNode.id, {0, 0})
        draw_tree_node(rootNode)
    end
end

local function draw_editor(name)
    if not imgui.begin_window(name, true, 1 << 10) then return end

    if not m_Context then
        m_Context = imnodeed.CreateEditor("Simple.json")
    end
    -- local ret, v = imgui.input_text("file path", file_path)
    -- if ret then
    --     file_path = v
    -- end
    if imgui.button("load") then
        load_action_file(file_path)
    end
    xpcall(function()
        imnodeed.SetCurrentEditor(m_Context)
        imnodeed.Begin("Action Tree Editor", Vector2f.new(0, 0))
        draw_tree()
        imnodeed.End()
        imnodeed.SetCurrentEditor(nil)
    end, G_LUA_ON_ERROR)


    imgui.end_window()
end



actionEditorFunc = function()
    imgui.push_style_var(ImGuiStyleVar.ImGuiStyleVar_WindowRounding, 10.0)

    local disp_size = imgui.get_display_size()
    imgui.set_next_window_size({EDITOR_SIZE.x, EDITOR_SIZE.y}, 1 << 1) -- ImGuiCond_Once
    imgui.set_next_window_pos({disp_size.x / 2 - (EDITOR_SIZE.x / 2), disp_size.y / 2 - (EDITOR_SIZE.y / 2)}, 1 << 1)
    
    xpcall(function()
        draw_editor("Action Editor")
    end, G_LUA_ON_ERROR)

    last_time = os.clock()

    imgui.pop_style_var()
end