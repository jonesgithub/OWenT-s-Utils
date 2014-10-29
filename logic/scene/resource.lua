--region logic.scene.resource.lua
--Author : OWenT
--Date   : 2014/10/29
--∆Ù∂Ø‘ÿ»ÎœÓ

local class = require('utils.class')
local resource = class.register('logic.scene.resource')
local manager = nil


function resource:path()
    local manager = manager or class.get('logic.scene.manager')
    return manager.getResourcePath(self.id .. '.body')
end

return resource