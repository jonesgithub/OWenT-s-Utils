--region adaptor.lua
--Author : OWenT
--Date   : 2014/10/22
--适配系统

if nil == _G.module then
    _G.module = function(name)
        error("[ERROR] module function disabled.")
    end
end

--endregion
