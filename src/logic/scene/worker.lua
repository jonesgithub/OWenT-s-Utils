--region logic.scene.worker.lua
--Author : OWenT
--Date   : 2014/10/29
--∆Ù∂Ø‘ÿ»ÎœÓ

local class = require('utils.class')
local worker = class.register('logic.scene.worker')
local resource = nil
local event_mgr = class.get('utils.event')

function worker.newId()
    return class.alloc_id()
end

local normal_new = worker.new
function worker.new()
    return normal_new(
        event_mgr.init({
            __scene = nil,
            __resource = {}
        })
    )
end

function worker:scene()
    return self.__scene or nil
end

function worker:addResource(any_thing)
    if nil == self or nil == self.__resource then
        return nil
    end

    resource = resource or class.get('logic.scene.resource')
    local ret = resource.new({
        id = worker.newId(),
        body = any_thing
    })
    self.__resource[ret.id] = ret
    return ret
end

function worker:removeResource(id)
    if nil == self or nil == self.__resource then
        return false
    end

    if nil ~= self.__resource[id] then
        self.__resource[id] = nil
    end
end

function worker:getResource(id)
    if nil == self or nil == self.__resource then
        return nil
    end

    return self.__resource[id] or nil
end


return worker