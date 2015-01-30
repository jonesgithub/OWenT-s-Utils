--region adaptor.lua
--Author : OWenT
--Date   : 2014/10/22
--适配系统

local conf = require('utils.conf')

-- 禁用5.1用法
if nil == _G.module then
    _G.module = function(name)
        log_error("[ERROR] module function disabled.")
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

-- 高级适配，日志输出重定向
if nil ~= _G.lua_log_level_t then
    conf.LOG_LEVEL = _G.lua_log_level_t
    _G.lua_log_level_t = nil
end

_G.lua_log_check = function(l)
    return l >= conf.log_min_level and l <= conf.log_max_level
end

if nil == _G.lua_log then
    _G.lua_log = function(log_level, fmt, ...)
        if lua_log_check(log_level) then
            print(string.format(fmt, ...))
        end
    end

else
    local log_fn = _G.lua_log
    _G.lua_log = function(log_level, fmt, ...)
        if lua_log_check(log_level) then
            log_fn(log_level, string.format(fmt, ...))
        end
    end
    
    _G.print = function(...)
        local args = {...}
        for k, v in ipairs(args) do
            _G.lua_log(conf.LOG_LEVEL.FATAL, "%s", tostring(v))
        end
    end
end

do
    _G.log_fatel = function(fmt, ...)
        fmt = fmt or ''
        lua_log(conf.LOG_LEVEL.FATAL, '[FATAL]: ' .. fmt, ...)
    end

    _G.log_debug = function(fmt, ...)
        fmt = fmt or ''
        lua_log(conf.LOG_LEVEL.DEBUG, '[DEBUG]: ' .. fmt, ...)
    end

    _G.log_info = function(fmt, ...)
        fmt = fmt or ''
        lua_log(conf.LOG_LEVEL.INFO, '[INFO]: ' .. fmt, ...)
    end

    _G.log_warn = function(fmt, ...)
        fmt = fmt or ''
        lua_log(conf.LOG_LEVEL.WARNING, '[WARN]: ' .. fmt, ...)
    end

    _G.log_error = function(fmt, ...)
        fmt = fmt or ''
        lua_log(conf.LOG_LEVEL.ERROR, '%s\n[ERROR]: ' .. fmt, debug.traceback('[ERROR]: ', 2), ...)
    end
    
    _G.log_stream = {}
    do
        local data_cache = ''
        function _G.log_stream:write(...)
            local args = {...}
            data_cache = data_cache .. table.unpack(args)
        end

        function _G.log_stream:flush()
            log_debug(data_cache)
            data_cache = ''
        end
    end
end


-- math 扩展
-- eps in c
math.epsilon = 0.0000001192092896

-- table扩展

--转入c实现
--function table.extend(dst, src, ...)
--    if nil == dst or nil == src then
--        return dst
--    end

--    for k, v in pairs(src) do
--        dst[k] = v
--    end
--    table.extend(dst, ...)
--    return dst
--end

--转入c实现
--function table.extend_r(dst, src, ...)
--    if nil == dst or nil == src then
--        return dst
--    end

--    for k, v in pairs(src) do
--        if 'table' == type(v) then
--            dst[k] = table.extend_r(dst[k] or {}, v)
--        else
--            dst[k] = v
--        end
--    end

--    table.extend_r(dst, ...)
--    return dst
--end

--转入c实现
--function table.clone(src)
--    if nil == src or 'table' ~= type(src) then
--        return nil
--    end

--    local ret = {}
--    for k, v in pairs(src) do
--        if 'table' == type(v) then
--            ret[k] = table.clone(v)
--        else
--            ret[k] = v
--        end
--    end

--    local mtb = getmetatable(src)
--    if src == mtb then
--        setmetatable(ret, ret)
--    else
--        setmetatable(ret, mtb)
--    end

--    return ret
--end

-- 二分查找
-- @param src table
-- @param v 比较目标
-- @param comp 比较函数(默认采用<号)
-- 比较函数调用式为comp(src[m], v)
-- @return 返回的索引值r，满足 not comp(src[r], v)
-- @note 如果所有元素都不满足条件not comp(src[r], v) 返回 #src + 1
function table.lower_bound(src, v, comp)
    if 0 == #src then
        log_error('binary search table must has more than 1 element(s)')
        return 0
    end

    comp = comp or function(cl, cr)
        return cl < cr
    end

    local l = 1
    local r = #src
    local m = 0
    while l < r do
        m = math.floor((l + r) / 2)
        if comp(src[m], v) then
            l = m + 1
        else
            r = m
        end
    end

    if comp(src[l], v) then
        return l + 1
    end

    return l
end

--endregion
