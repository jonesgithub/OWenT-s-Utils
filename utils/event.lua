--region event.lua
--Author : OWenT
--Date   : 2014/10/23
--事件系统

local class = require ('utils.class')

local event = class.register('utils.event', class.singleton)
event.TIMER_TYPE = {
    ONCE = 0,
    LOOP = 1,
}

local dispatcher = class.register('utils.event.dispatcher')

-- 注入new事件，采用预分配table，减少rehash
do
    local old_new = dispatcher.new
    dispatcher.new = function()
        return old_new({
            listener = {},
            once = {},
            timer = {},
        })
    end
end

function dispatcher:add_listener(bind_name, callback)
    if nil == bind_name or nil == callback then
        return nil
    end

    local id = class.alloc_id()

    self.listener[bind_name] = selfself.listener[bind_name] or {}
    self.listener[bind_name][id] = callback
    return id
end

function dispatcher:add_once(bind_name, callback)
    if nil == bind_name or nil == callback then
        return nil
    end

    local id = class.alloc_id()

    self.once[bind_name] = selfself.once[bind_name] or {}
    self.once[bind_name][id] = callback
    return id
end

function dispatcher:add_timer(time_, callback, type_, timer_name)
    if nil == callback then
        return nil
    end

    type_ = type_ or event.TIMER_TYPE.ONCE
    local id = timer_name or class.alloc_id()

    self.timer[id] = {
        type = type_,
        fn = callback,
        time = time_,
        left = time_,
    }
    return id
end

function dispatcher:update(delta)
    local km = {}
    for k, v in pairs(self.timer) do
        v.left = v.left - delta
        if v.left <= 0 then
            v.fn(v.time - delta)
        end
        if v.type == event.TIMER_TYPE.ONCE then
            table.insert(km, k)
        else
            v.left = v.time
        end
    end

    -- 移除只触发一次的定时器
    for k, v in ipairs(km) do
        self.timer[k] = nil
    end
end


-- 移除监听事件
function dispatcher:remove_listener(bind_name, id)
    -- 先尝试移除临时监听器
    if nil ~= self.once[bind_name] then
        local ret = self.once[bind_name][id] or nil
        self.once[bind_name][id] = nil
        return ret
    end

    -- 再尝试移除永久监听器
    if nil ~= self.listener[bind_name] then
        local ret = self.listener[bind_name][id] or nil
        self.listener[bind_name][id] = nil
        return ret
    end

    return nil
end

function dispatcher:trigger(bind_name, ...)
    if nil ~= self.listener[bind_name] then
        for k, v in pairs(self.listener) do
            v(...)
        end
    end

    if nil ~= self.once[bind_name] then
        for k, v in pairs(self.once) do
            v(...)
        end

        self.once[bind_name] = {}
    end
end


local init_obj_event_listener = function (obj)

end

-- 事件管理器
function event.init(obj)
    rawset(obj, '__event', dispatcher.new())
    rawset(obj, 'event', function(self)
        return self.__event
    end)

    return obj
end


function event.add_listener(obj, bind_name, callback)

    if nil == rawget(obj, '__event') then
        event.init(obj)
    end

    return obj.__event:add_listener(bind_name, callback)
end

function event.add_once(obj, bind_name, callback)

    if nil == rawget(obj, '__event') then
        event.init(obj)
    end

    return obj.__event:add_once(bind_name, callback)
end

function event.remove_listener(obj, bind_name, id)
    if nil == obj or nil == obj.__event then
        return nil
    end

    return obj.__event:remove_listener(bind_name, id)
end

function event.trigger(obj, bind_name, ...)
    if nil == obj or nil == obj.__event then
        return
    end

    obj.__event:trigger(bind_name, ...)
end

function event.add_timer(obj, ...)
    if nil == rawget(obj, '__event') then
        event.init(obj)
    end

    return obj.__event:add_timer(...)
end

-- 全局事件对象
event.global = dispatcher.new()
function event.global:add_listener(bind_name, callback)
    LogError('add listener to utils.event.global is deny to reduce memory leak')
    return nil
end

-- 不允许全局定时器必须是具名定时器，防止滥用导致内存泄露
function event.global:add_timer(time_, callback, type_, timer_name)
    if timer_name then
        return dispatcher.add_timer(self, time_, callback, type_, timer_name)
    end

    LogError('add timer to utils.event.global require timer name')
    return nil
end


function event.update(delta)
    event.global:update(delta)
end

-- 注册到全局
_G.utils = _G.utils or {}
_G.utils.event = event

return event
--endregion
