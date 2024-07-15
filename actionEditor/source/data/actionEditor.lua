
require ("imgui_ext")


function G_LUA_ON_ERROR(err)
    print("LUA ERROR: ", err, debug.traceback("", 2))
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
    x = math.max(imgui.get_display_size().x, 1280),
    y = math.max(imgui.get_display_size().y, 720)
}


local m_Context = nil
local node_input = 1
local node_replace_input = 1
local action_map = {}
local action_name_map = {}
local event_map = {}
local event_name_map = {}
local condition_map = {}
local condition_name_map = {}
local selection_map = {}
local condition_selection_map = {}
local node_map = {}
local node_names = {}
local first_times = {}
local sort_dict = {}

local custom_tree = {}
local function recreate_globals()
    custom_tree = {}
    cached_node_names = {}
    cached_node_indices = {}
    action_map = {}
    action_name_map = {}
    event_map = {}
    event_name_map = {}
    condition_map = {}
    condition_name_map = {}
    selection_map = {}
    condition_selection_map = {}
    node_map = {}
    node_names = {}
    first_times = {}
    sort_dict = {}
end



local node_replacements = {

}
local selected_action_node_id = nil
local custom_tree_indices_map = {}

-- local TAB = imgui.get_key_index(0)
-- local LEFT_ARROW = imgui.get_key_index(1)
-- local RIGHT_ARROW = imgui.get_key_index(2)
-- local UP_ARROW = imgui.get_key_index(3)
-- local DOWN_ARROW = imgui.get_key_index(4)
-- local ENTER = imgui.get_key_index(13)
-- local VK_LSHIFT = 0xA0

local cfg = {
    -- view
    always_show_node_editor = false,
    show_minimap = true,
    follow_active_nodes = false,
    display_parent_of_active = true,
    parent_display_depth = 0,
    default_node = 0,
    show_side_panels = true,
    sidePanelWidth = 400,
    editorPanelWidth = 800,
    graph_closes_with_reframework = true,

    -- editor
    pan_speed = 1000,
    lerp_speed = 2.0,
    lerp_nodes = true,

    -- search
    max_search_results = 200,
    default_node_search_name = "",
    default_condition_search_name = "",
    default_action_search_name = "",
    search_allow_duplicates = true
}
local file_path = "D:/WS/Modding/DMC5_Research/pl0800_vergilPL_saved_tree.json"

local function load_action_file(filePath)
    if not filePath then
        print("filePath is nil")
        return
    end
    return json.load_file(filePath)
end

local unlock_node_positioning = false

local function draw_link(active, id, attr_start, attr_end)
    if active then
        imnodeed.Link(id, attr_start, attr_end)
        imnodeed.Flow(id)
    else
        imnodeed.Link(id, attr_start, attr_end)
    end
end

local function draw_stupid_node(name, custom_id, render_inputs_cb, render_outputs_cb, render_after_cb)
    local out = {}

    if custom_id then
        out.id = custom_id
    else
        out.id = imgui.get_id(name)
    end

    out.inputs = {}
    out.outputs = {}

    imnodeed.BeginNode(out.id)

    imgui.text(name)

    if render_inputs_cb then
        out.inputs = render_inputs_cb()
    end

    if render_outputs_cb then
        out.outputs = render_outputs_cb()
    end

    if render_after_cb then
        render_after_cb()
    end

    imnodeed.EndNode()
    if imgui.IsItemClicked() then
        selected_action_node_id = custom_id
    end

    return out
end

local function draw_standard_node(name, custom_id, render_after_cb)
    local out = draw_stupid_node(name, custom_id,
        function()
            local out2 = {}

            if custom_id then
                table.insert(out2, imgui.get_id(tostring(custom_id) .. "parent"))
            else
                table.insert(out2, imgui.get_id(name .. "parent"))
            end
        
            imnodeed.BeginPin(out2[1], PinKind.Input);
            imgui.text("-> parent")
            imnodeed.EndPin();

            return out2
        end,
        function()
            local out2 = {}

            if custom_id then
                table.insert(out2, imgui.get_id(tostring(custom_id) .. "children"))
            else
                table.insert(out2, imgui.get_id(name .. "children"))
            end
            imgui.same_line()
            imgui.dummy({math.max(imgui.calc_text_size(name).x + 100), 0})
            imgui.same_line()
            imnodeed.BeginPin(out2[1], PinKind.Output);
            imgui.text("children ->")
            imnodeed.EndPin();

            return out2
        end,
        function()
            if render_after_cb then
                render_after_cb()
            end
        end
    )

    return out
end

local updated_tree = false
local node_is_hovered = false
local node_hovered_id = 0
local node_map = {}


local active_tree = nil
local last_time = 0.0
local delta_time = 0.0

local function get_tree_node(tree, id)
    return tree.nodes[id+1]
end

local function draw_node(idx, seen, depth)
    seen = seen or {}
    depth = depth or 0
    if seen[idx] then return end
    if not custom_tree[idx] then return end
    
    local custom_id = nil

    if active_tree ~= nil then
        local node = get_tree_node(active_tree, idx)
        if node == nil then
            return
        end

        custom_id = node.id
    end
    local node_descriptor = custom_tree[idx]
    local node = draw_standard_node(
        "[" .. tostring(idx) .. "]" .. node_descriptor.name, 
        custom_id,
        function()
            if not node_map[idx] then return end

            if active_tree then
                --imgui.text(tostring(active_tree:get_node(i)))
                --if imgui.begin_child_window("Test" .. tostring(i), 100, 100) then
                    -- display_node(active_tree, active_tree:get_node(i))
                    --imgui.end_child_window()
                --end
            end
        end
    )

    node_map[idx] = node

    local active = false

    -- if active_tree ~= nil then
    --     local real_node = active_tree:get_node(i)

    --     active = real_node:get_status1() == 2 or real_node:get_status2() == 2
    -- end
    
    -- Draw children and compute space requirements
    local node_pos = imnodeed.GetNodePosition(node.id)
    local node_dims = imnodeed.GetNodeSize(node.id)

    local size = { x=0, y=0 }

    for j, child_id in ipairs(node_descriptor.children) do
        local child, node_dim_requirements, child_active = draw_node(child_id, seen, depth + 1)
        if child then
            -- Y needs to be dynamic
            local child_render_pos = {
                x = node_pos.x + node_dims.x + 20,
                y = node_pos.y + size.y
                --y = node_pos.y - ((#node_descriptor.children - 1) * (node_dims.y / 2)) + node_dim_requirements.y + ((j-1) * node_dims.y)
            }
    
            if not unlock_node_positioning then
                if cfg.lerp_nodes then
                    local current_child_pos = imnodeed.GetNodePosition(child.id)
    
                    local crp = Vector2f.new(child_render_pos.x, child_render_pos.y)
                    local dist = (current_child_pos - crp):length()
    
                    if dist < 20 then
                        crp = current_child_pos + ((crp - current_child_pos) * math.min(delta_time, 0.5))
                    else
                        crp = current_child_pos + ((crp - current_child_pos):normalized() * math.min(math.min(delta_time, 0.5) * dist * cfg.lerp_speed * 10.0, dist))
                    end
    
                    imnodeed.SetNodePosition(child.id, {crp.x, crp.y})
                else
                    imnodeed.SetNodePosition(child.id, {child_render_pos.x, child_render_pos.y})
                end
            end
    
            local link_id = imgui.get_id(node_descriptor.name .. custom_tree[child_id].name .. "LINK")
    
            -- if node_is_hovered then
            --     if node_hovered_id == node_map[idx].id then
            --         draw_link(false, link_id, node_map[idx].outputs[1], node_map[child_id].inputs[1])
            --     end
    
            --     if node_hovered_id == node_map[child_id].id then
            --         draw_link(false, link_id, node_map[idx].outputs[1], node_map[child_id].inputs[1])
            --     end
            -- elseif active and child_active then
            --     draw_link(active, link_id, node_map[idx].outputs[1], node_map[child_id].inputs[1])
            -- end
            draw_link(active, link_id, node_map[idx].outputs[1], node_map[child_id].inputs[1])
    
            size.x = size.x + node_dim_requirements.x
            size.y = size.y + node_dim_requirements.y            
        end
    end

    -- Only add the node dimensions to the out dim requirements
    -- if the node has no children, meaning it's the end of the chain
    if #node_descriptor.children == 0 then
        size.y = size.y + node_dims.y + 5
    else
        if node_dims.y > size.y then
            size.y = node_dims.y + 5
        end
    end

    return node, size, active
end

local function draw_tree()
    if not active_tree then
        imgui.text("actionTree is nil")
        return
    end
    -- draw_node draws all children, so only draw the root node
    if cfg.default_node == 0 then
        if custom_tree[0] then
            if node_map[0] and not unlock_node_positioning then
                imnodeed.SetNodePosition(node_map[0].id, {0, 0})
            end

            local node, req, active = draw_node(0)
        else
            if node_map[1] and not unlock_node_positioning then
                imnodeed.SetNodePosition(node_map[1].id, {0, 0})
            end

            draw_node(1)
        end
    else
        draw_node(cfg.default_node)
    end

    node_is_hovered, node_hovered_id = imnodeed.IsNodeHovered()
    node_hovered_id = node_hovered_id & 0xFFFFFFFF
end


local function reconstruct_node(active_tree, node)
    if not node then
        return
    end
    if node.actions and not node.actions_r then
        node.actions_r = {}
        for index, value in ipairs(node.actions) do
            node.actions_r[index] = active_tree.actions[value]
        end
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
    xpcall(function()
        if imgui.button("Load") then
            local tree = load_action_file(file_path)
            if tree then
                recreate_globals()
        
                custom_tree = {}
                updated_tree = true
                active_tree = tree
                custom_tree_indices_map = {}
                
                for i, node in ipairs(active_tree.nodes) do    
                    local insertion = {
                        name = node.name,
                        children = node.children
                    }
                    reconstruct_node(active_tree, node)
    
                    custom_tree[i-1] = insertion
                    custom_tree_indices_map[node.id] = i-1
                end
            end
        end
        imnodeed.SetCurrentEditor(m_Context)
        imnodeed.Begin("Action Tree Editor", Vector2f.new(0, 0))
        draw_tree()
        imnodeed.End()
        imnodeed.SetCurrentEditor(nil)
    end, G_LUA_ON_ERROR)


    imgui.end_window()
end



actionEditorFunc = function()
    -- imgui.push_style_var(ImGuiStyleVar.ImGuiStyleVar_WindowRounding, 10.0)
    
    delta_time = os.clock() - last_time

    imgui.begin_window("SidePanel", true)
    xpcall(function()
        imgui.h_dumpVisitLuaValueWithConfig(active_tree, "active_tree")
        imgui.text(string.format("selected_action_node_id %s", tostring(selected_action_node_id)))
        if selected_action_node_id and custom_tree_indices_map[selected_action_node_id] then
            local node = get_tree_node(active_tree, custom_tree_indices_map[selected_action_node_id])
            if node then
                imgui.h_dumpVisitLuaValueWithConfig(node, "selectedActionNode")
            end
        end
    end, G_LUA_ON_ERROR)
    imgui.end_window()
    local disp_size = imgui.get_display_size()
    imgui.set_next_window_size({EDITOR_SIZE.x, EDITOR_SIZE.y}, 1 << 1) -- ImGuiCond_Once
    imgui.set_next_window_pos({disp_size.x / 2 - (EDITOR_SIZE.x / 2), disp_size.y / 2 - (EDITOR_SIZE.y / 2)}, 1 << 1)
    
    draw_editor("Action Tree Editor")

    last_time = os.clock()

    -- imgui.pop_style_var()
end