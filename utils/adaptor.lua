--region adaptor.lua
--Author : OWenT
--Date   : 2014/10/22
--适配系统

local conf = require('utils.conf')

-- 禁用5.1用法
if nil == _G.module then
    _G.module = function(name)
        error("[ERROR] module function disabled.")
    end
end

-- 5.1适配5.2
if nil == table.unpack then
    table.unpack = unpack or nil
end

-- 5.1适配5.2
if nil == package.searchers then
    package.searchers = package.loaders or nil
end


-- 高级适配，输出重定向
if nil == _G.LuaLog then
    _G.LuaLog = function(fmt, ...)
        print(string.format(fmt, ...))
    end

else
    --_G.print = function(...)
    --    LuaLog(table.unpack(...))
    --end

end

do
    
    _G.LogFatel = function(fmt, ...)
        LuaLog('[FATEL]: ' .. fmt, ...)
    end

    _G.LogDebug = function(fmt, ...)
        if conf.log < conf.LOG_LEVEL.DEBUG then
            return
        end
        LuaLog('[DEBUG]: ' .. fmt, ...)
    end

    _G.LogInfo = function(fmt, ...)
        if conf.log < conf.LOG_LEVEL.INFO then
            return
        end
        LuaLog('[INFO]: ' .. fmt, ...)
    end

    _G.LogWarn = function(fmt, ...)
        if conf.log < conf.LOG_LEVEL.WARN then
            return
        end
        LuaLog('[WARN]: ' .. fmt, ...)
    end

    _G.LogError = function(fmt, ...)
        if conf.log < conf.LOG_LEVEL.ERROR then
            return
        end
        LuaLog('%s\n[ERROR]: ' .. fmt, debug.traceback('[ERROR]: ', 2), ...)
    end
end

--endregion
