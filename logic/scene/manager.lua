--region logic.scene.manager.lua
--Author : OWenT
--Date   : 2014/10/29
--启动载入项

local class = require('utils.class')
local loader = require('utils.loader')
local manager = class.register('logic.scene.manager', class.singleton)
local worker = loader.load('logic.scene.worker')
local event = loader.load('utils.event')

manager.__data = {
    current = nil,
}

function manager.current()
    return manager.__data.current
end

function manager.load(worker, ...)
    if nil == worker or nil == worker.__scene then
        return false
    end

    if worker.__scene:show(...) < 0 then
        error('show scene failed')
        debug.traceback()
        return false
    end

    manager.__data.current = worker

    -- 覆盖具名timer
    event.global:add_timer(0.2, function(delta)
        worker:event():update(delta)
    end, event.TIMER_TYPE.LOOP, 'current_scene_update')

    return true
end

function manager.getResourcePath(id)
    return "game.scene_manager.__data.current.__resource." .. id;  
end

-- 创建场景
-- @param ... 将会全部传入scene的init方法中
function manager.create(...)
    local inst = worker.new()

    inst.__scene = game.ui.LuaScene.new()
    local res = inst.__scene:init(...)
    if res < 0 then
        error('scene init failed, ret: ' .. res)
        return nil
    end

    return inst
end

-- 注册到全局
_G.game = _G.game or class.register({}, class.namespace, "game")
_G.game.scene_manager = manager

return manager