imgui = imgui or {}


imgui.ImGuiTreeNodeFlags_None                 = 0
imgui.ImGuiTreeNodeFlags_Selected             = 1 << 0   -- // Draw as selected
imgui.ImGuiTreeNodeFlags_Framed               = 1 << 1   -- // Draw frame with background (e.g. for CollapsingHeader)
imgui.ImGuiTreeNodeFlags_AllowOverlap         = 1 << 2   -- // Hit testing to allow subsequent widgets to overlap this one
imgui.ImGuiTreeNodeFlags_NoTreePushOnOpen     = 1 << 3   -- // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
imgui.ImGuiTreeNodeFlags_NoAutoOpenOnLog      = 1 << 4   -- // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
imgui.ImGuiTreeNodeFlags_DefaultOpen          = 1 << 5   -- // Default node to be open
imgui.ImGuiTreeNodeFlags_OpenOnDoubleClick    = 1 << 6   -- // Need double-click to open node
imgui.ImGuiTreeNodeFlags_OpenOnArrow          = 1 << 7   -- // Only open when clicking on the arrow part. If ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
imgui.ImGuiTreeNodeFlags_Leaf                 = 1 << 8   -- // No collapsing, no arrow (use as a convenience for leaf nodes).
imgui.ImGuiTreeNodeFlags_Bullet               = 1 << 9   -- // Display a bullet instead of arrow. IMPORTANT: node can still be marked open/close if you don't set the _Leaf flag!
imgui.ImGuiTreeNodeFlags_FramePadding         = 1 << 10  -- // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
imgui.ImGuiTreeNodeFlags_SpanAvailWidth       = 1 << 11  -- // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
imgui.ImGuiTreeNodeFlags_SpanFullWidth        = 1 << 12  -- // Extend hit box to the left-most and right-most edges (bypass the indented area).
imgui.ImGuiTreeNodeFlags_SpanAllColumns       = 1 << 13  -- // Frame will span all columns of its container table (text will still fit in current column)
imgui.ImGuiTreeNodeFlags_NavLeftJumpsBackHere = 1 << 14  -- // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
-- imgui.ImGuiTreeNodeFlags_NoScrollOnOpen       = 1 << 15 -- // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
imgui.ImGuiTreeNodeFlags_CollapsingHeader     = imgui.ImGuiTreeNodeFlags_Framed | imgui.ImGuiTreeNodeFlags_NoTreePushOnOpen | imgui.ImGuiTreeNodeFlags_NoAutoOpenOnLog



local ComboBoxPackMap = {}
local ShowNameMap = {}

function imgui.h_getValueString(v)
    if type(v) == "string" then
        v = "\"" .. v .. "\""
    end
    return tostring(v)
end

function imgui.h_checkTableHasSub(v_t)
    if type(v_t) ~= "table" then
        return false;
    end

    for key, value in pairs(v_t) do
        return  true
    end

    return false
end

function imgui.h_dumpVisitLuaValue(lvalue, desciption, uid_prefix)
    desciption = desciption or "<var>"
    uid_prefix = uid_prefix or "uid_"
    local text = desciption .. " = " .. imgui.h_getValueString(lvalue)
    local node_flags = imgui.ImGuiTreeNodeFlags_OpenOnArrow | imgui.ImGuiTreeNodeFlags_OpenOnDoubleClick

    local uid = uid_prefix .. desciption
    imgui.PushID(uid)
    imgui.AlignTextToFramePadding();
    local typeStr = type(lvalue)
    if typeStr == "table" or typeStr == "userdata" then
        local showItems = {}
        if typeStr == "table" then
            for key, v in pairs(lvalue) do
                table.insert(showItems, {tostring(key), v})
            end
        end
        if imgui._config_visualDump_showMetaTable then
            table.insert(showItems, {"_MetaTable_", getmetatable(lvalue)})
        end
        if imgui._config_visualDump_showPeerTable then
            table.insert(showItems, {"_PeerTable_", tolua.getpeer(lvalue)})
        end
        if imgui._config_visualDump_sortKey then
            table.sort(showItems, function(a, b) return a[1] < b[1] end)
        end

        local hasSub = #showItems>0
        if hasSub then
            local node_open = imgui.TreeNodeEx(uid, node_flags, text)
            if (imgui.BeginPopupContextItem("##item context menu"..uid)) then
                imgui.inputText("##edit"..uid, text, imgui.ImGuiInputTextFlags_ReadOnly)
                imgui.EndPopup()
            end
            if node_open then
                for _, showItem in ipairs(showItems) do
                    local key = showItem[1]
                    local value = showItem[2]
                    imgui.h_dumpVisitLuaValue(value, imgui.h_getValueString(key), uid_prefix.."_"..desciption)
                end
                imgui.TreePop()
            end
        else
            node_flags = node_flags | imgui.ImGuiTreeNodeFlags_Leaf |imgui.ImGuiTreeNodeFlags_NoTreePushOnOpen
            imgui.TreeNodeEx(uid, node_flags, text)
            if (imgui.BeginPopupContextItem("##item context menu"..uid)) then
                imgui.inputText("##edit"..uid, text, imgui.ImGuiInputTextFlags_ReadOnly)
                imgui.EndPopup()
            end
        end
    else
        node_flags = node_flags | imgui.ImGuiTreeNodeFlags_Leaf |imgui.ImGuiTreeNodeFlags_NoTreePushOnOpen
        imgui.TreeNodeEx(uid, node_flags, text)
        if (imgui.BeginPopupContextItem("##item context menu"..uid)) then
            imgui.inputText("##edit"..uid, text, imgui.ImGuiInputTextFlags_ReadOnly)
            imgui.EndPopup()
        end
    end

    imgui.PopID()
end

function imgui.h_dumpVisitLuaValueWithConfig(lvalue, desciption, uid_prefix)
    local ret, v = imgui.checkbox("Sort Key", imgui._config_visualDump_sortKey)
    if ret then
        imgui._config_visualDump_sortKey = v
    end
    imgui.SameLine()
    local ret, v = imgui.checkbox("Metatable", imgui._config_visualDump_showMetaTable)
    if ret then
        imgui._config_visualDump_showMetaTable = v
    end
    imgui.SameLine()
    local ret, v = imgui.checkbox("Peertable", imgui._config_visualDump_showPeerTable)
    if ret then
        imgui._config_visualDump_showPeerTable = v
    end
    imgui.h_dumpVisitLuaValue(lvalue, desciption, uid_prefix)
end