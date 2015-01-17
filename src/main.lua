do
    local info = '============================= Load Lua Start ============================='
    if _G.jit then
        info = info .. '\n========== Lua JIT Version:' .. tostring(_G.jit.version) .. ' ==========\n\n\n'
    end

    if _G.lua_log then
        lua_log(info)
    elseif _G.LuaLog then
        LuaLog(info)
    else
        print(info)
    end
end

math.randomseed(os.time())

local loader = require('utils.loader')
require('utils.event')

-- ��ɶcocos2d��رձ�׼�����������, �ᵼ��vardump�����
do
    _G.vardump_default.ostream = log_stream
end

-- ע������game�µ�tableΪ�����ռ�
do
    local class = loader.load('utils.class')
    local function reg_game_namespace(tb, prefix, name, base_type)
        local this_path = prefix .. name
        prefix = this_path .. '.'

        for k, v in pairs(tb) do
            -- �����ռ�ע�����
            if 'table' == type(v) and nil == getmetatable(v) then
                reg_game_namespace(v, prefix, k, class.namespace)
            end

            -- native��ע�����
            if 'table' == type(v) and nil ~= getmetatable(v) and nil == v.__index then
                reg_game_namespace(v, prefix, k, class.native)
            end
        end

        class.register(tb, base_type, this_path)
    end

    reg_game_namespace(game, '', 'game', class.namespace)
end

-- ����bootstrap
loader.load_list('bootstrap')


--require "script/testunit/mainMenu"
log_info('============================= Load Lua End =============================')
