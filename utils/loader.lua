local class_builder = require('utils.class')

local loader = class_builder.register('utils.loader', class_builder.singleton)
loader.sysmod = {}

for k, v in pairs(_G) do
    if nil ~= v then
        loader.sysmod[k] = true
    end
end

-- 移除模块
function loader.remove(modname)
    local ret = package.loaded[modname]
    package.loaded[modname] = nil
    return ret
end

-- 安全加载模块, 不允许加载userdata
function loader.load(modname)
    local ret = require(modname)
    if 'userdata' == type(ret) then
        package.loaded[modname] = nil
        ret = nil
        print(debug.traceback())
    end

    return ret
end

-- 安全加载table, 不允许加载userdata
function loader.load_table(table_obj, prefix, root)
    assert('table' == type(table_obj))
    prefix = prefix or ''
    root = root or _G
    for k, v in pairs(table_obj) do
        -- 子结构
        if 'table' == type(v) then
            if nil == root[k] then
                root[k] = {}
                class_builder.register(prefix .. k, class_builder.namespace)
            end
            
            loader.load_table(v, prefix .. k .. '.', root[k])

        -- 列表文件
        elseif 'string' == type(k) and 'string' == type(v) then
            loader.load_list(v, prefix .. k .. '.', root)

        -- 普通文件: table list
        elseif 'number' == type(k) and 'string' == type(v) then
            loader.load(prefix .. v)
            
        -- 普通文件: key-value配置
        else
            root[k] = loader.load(prefix .. k)
        end
    end
end

-- 安全加载列表, 不允许加载userdata
function loader.load_list(modname, prefix, root)
    prefix = prefix or ''
    root = root or _G

    local ret = loader.load(prefix .. modname)
    if 'userdata' == type(ret) then
        error('[ERROR] load list file "' .. prefix .. modname .. '" failed')
        return false
    end

    loader.load_table(ret, prefix, root)
    return true
end

-- 清空所有非预置模块
function loader.clear()
    for k, v in pairs(_G) do
        if nil ~= v and loader.sysmod[k] then
            _G[k] = nil
        end
    end
    -- 防止utils被先加载，导致utils不更新的问题
    if nil ~= _G['utils'] then
        _G['utils'] = nil
    end

    -- 清空后要保证loader首先被加载，并初始化utils包
    require('utils.loader')
end

function loader.reload()
    for k, v in pairs(_G) do
        if nil ~= v and loader.sysmod[k] then
            _G[k] = nil
        end
    end
end


-- 安全预加载模块, 不允许加载userdata
-- 不会被clear清空
function loader.preload(modname)
    local ret = loader.load(modname)
    if 'userdata' ~= type(ret) then
        package.preload[modname] = ret
    end

    return ret
end

return loader
