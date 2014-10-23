--region event.lua
--Author : OWenT
--Date   : 2014/10/23
--事件系统

local class = require ('utils.class')

utils.event = class.register('utils.event', class_builder.singleton)

function utils.event.add_listener(obj, bind_name, callback)
end

function utils.event.trigger(obj, bind_name, ...)
end

return utils.event
--endregion
