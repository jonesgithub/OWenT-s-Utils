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

-- 那啥cocos2d会关闭标准输入输出函数, 会导致vardump无输出
do
    _G.vardump_default.ostream = log_stream
end

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


--require "script/testunit/mainMenu"

function test_scene_start()
--[[
    local scene = cc.Scene:create()
    scene:addChild(CreateTestMenu())
    if cc.Director:getInstance():getRunningScene() then
        cc.Director:getInstance():replaceScene(scene)
    else
        cc.Director:getInstance():runWithScene(scene)
    end
    --]]
    --lua自动绑定测试代码
    --local test = cc.MyClass:create()
    --local ccnode = test:createNode();
    --ccnode:setName('aaabbb');
    --print("lua bind: " .. test:foo(99))
    --print("lua node :" .. ccnode:getName());
    local scene_mgr = loader.load('logic.scene.manager')
    local login_scene = loader.load('logic.scene.login_scene')

    local scene_inst = scene_mgr.create(login_scene)
--[[
    local state_machine = loader.load('logic.unit.StateMachine')
    local fsm = state_machine.new();

    fsm:setupState({
    initial = "idle",
    events = {
      {name = "attackEvent", from = "idle", to = "attack"},
      {name = "dizzEvent", from = {"idle","attack","dizz","walking","cast_skill","interrupt"}, to = "dizz" },
      {name = "idleEvent", from = {"attack","walking","dizz","cast_skill","interrupt"}, to = "idle"},
      {name = "walkingEvent", from = {"idle"}, to = "walking" },
      {name = "hitEvent", from = {"idle"}, to = "behit" },
      {name = "beKilled", from = {"idle","walking","dizz","cast_skill","attack"}, to = "dead" },
    },
    callbacks = {
    onbeforeattackEvent = function(event) print("[FSM] STARTING UP") end,
    onattackEvent = function(event) print("[FSM] READY") end,
   -- onafterattackEvent = function(event) print("[FSM] FINISH") end,
    },
})
    --fsm:doEvent("start")
    fsm:doEvent("attackEvent")

    fsm:isReady()--]]
     --载入登入场景
    scene_inst:init()
    scene_mgr.load(scene_inst, game.ui.LuaScene.EN_TT_FADE, 1)
     
   return true
end


do
    log_info('global scale (%f, %f)', game.device.getGlobalScale())
    local vr = game.device.getVisibleRect()
    log_info('visualable rect: origin=(%f, %f), size=(%f, %f)', vr.origin.x, vr.origin.y, vr.size.width, vr.size.height)
end

log_info('============================= Load Lua End =============================')
