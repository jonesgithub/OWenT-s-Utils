--region data.list.lua
--Author : OWenT
--Date   : 2014/10/29
--∆Ù∂Ø‘ÿ»ÎœÓ

local loader = require('utils.loader')
local cfg = loader.load('data.conf_manager')

-- role_cfg
cfg:load('unit.role_cfg', function(k, v)
    return v.id or k
end)
