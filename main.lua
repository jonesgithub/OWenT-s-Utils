print('============================= Lua Start =============================')

local loader = require('utils.loader')

-- 加载bootstrap
loader.load_list('bootstrap')

-- 注册所有game下的table为命名空间
do
    local class = loader.load('utils.class')
    local function reg_game_namespace(tb, prefix, name)
        local this_path = prefix .. name
        prefix = this_path .. '.'

        for k, v in pairs(tb) do
            -- 命名空间注册基类
            if 'table' == type(v) and nil == getmetatable(v) then
                reg_game_namespace(v, prefix, k)
            end

            -- native类注册基类
            if 'table' == type(v) and nil ~= getmetatable(v) and nil == v.__index then
                class.register(prefix .. k, class.native)
            end
        end

        class.register(this_path, class.namespace)
    end

    reg_game_namespace(game, '', 'game')
end

vardump(game)


game.ui.LuaScene.print()

local test_var1 = game.ui.LuaScene.new()
local test_var2 = game.ui.LuaScene.new()
local test_var3 = game.ui.LuaScene.new()
local test_var4 = game.ui.LuaScene.new()


test_var1.print()
test_var2:print()
test_var3:print()
test_var4:print()

test_var1:init()
test_var2:init()
test_var3:init()
test_var4:init()


test_var3 = nil
test_var4 = nil
collectgarbage()

vardump(test_var1)
vardump(test_var1:__tostring())
vardump(test_var1.__tostring())
vardump(tostring(test_var1))
vardump(getmetatable(test_var1))

vardump(game.ui.LuaScene:__tostring())
vardump(game.ui.LuaScene.__tostring())
vardump(tostring(game.ui.LuaScene))
