--region data.conf_manager.lua
--Author : OWenT
--Date   : 2014/10/29
--启动载入项

local class = require('utils.class')
local loader = require('utils.loader')

local conf_set = class.register('data.conf_data_set')
function conf_set:get(key)
    if nil == self then
        return nil
    end

    return self[key] or nil
end


local conf_manager = class.register('data.conf_manager', class.singleton)

conf_manager.__data = {}

function conf_manager:load(path, kv_fn)
    local tb = loader.load('data.' .. path)
    if nil == tb then
        LogError('load cfg [%s] failed', path)
        return false
    end

    kv_fn = kv_fn or function(k, v)
        return k
    end

    for k,v in pairs(tb) do
        if 'number' == type(k) then
            LogInfo('load cfg [%s] success, ver=%s, count=%d', path, v.data_ver, v.count)
        else
            conf_manager.__data[k] = conf_manager.__data[k] or conf_set.new()
            local cfg = conf_manager.__data[k]
            for ck, cv in ipairs(v) do
                local rk = kv_fn(ck, cv)
                if cfg:get(rk) then
                    LogWarn('config [%s] already has key %s, old record will be covered', path, rk)
                end

                cfg[rk] = cv
            end
        end
    end

    -- 释放资源
    loader.remove('data.' .. path)
end


function conf_manager:reload()
    loader.remove('data.conf_list')
    loader.load('data.conf_list')
end

return conf_manager
