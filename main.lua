print('============================= Load Lua Start =============================')

local loader = require('utils.loader')
require('utils.event')

-- 注册所有game下的table为命名空间
do
    local class = loader.load('utils.class')
    local function reg_game_namespace(tb, prefix, name, base_type)
        local this_path = prefix .. name
        prefix = this_path .. '.'

        for k, v in pairs(tb) do
            -- 命名空间注册基类
            if 'table' == type(v) and nil == getmetatable(v) then
                reg_game_namespace(v, prefix, k, class.namespace)
            end

            -- native类注册基类
            if 'table' == type(v) and nil ~= getmetatable(v) and nil == v.__index then
                reg_game_namespace(v, prefix, k, class.native)
            end
        end

        class.register(tb, base_type, this_path)
    end

    reg_game_namespace(game, '', 'game', class.namespace)
end

-- 加载bootstrap
loader.load_list('bootstrap')


vardump(game)

-- 加载配置
do
    local cfg = loader.load('data.conf_manager')
    vardump(cfg.__data)
end
print('============================= Load Lua End =============================')
