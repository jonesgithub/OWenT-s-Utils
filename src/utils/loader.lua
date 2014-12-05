local class_builder = require('utils.class')

local loader = class_builder.register('utils.loader', class_builder.singleton)
loader.loaded_mods = {}

-- �Ƴ�ģ��
function loader.remove(modname)
    local ret = package.loaded[modname]
    package.loaded[modname] = nil
    return ret
end

-- ��ȫ����ģ��, ���������userdata
function loader.load(modname)
    local ret = require(modname)
    if 'userdata' == type(ret) then
        package.loaded[modname] = nil
        ret = nil
        log_error('try to load userdata using utils.loader.load(%s)', tostring(modname))
    else
--        if not loader.loaded_mods[modname] then
--            log_debug('load new module %s', modname)
--        end

        loader.loaded_mods[modname] = true
    end

    return ret
end

-- ��ȫ����table, ���������userdata
function loader.load_table(table_obj, prefix, root)
    assert('table' == type(table_obj))
    prefix = prefix or ''
    root = root or _G
    for k, v in pairs(table_obj) do
        -- �ӽṹ
        if 'table' == type(v) then
            if nil == root[k] then
                root[k] = {}
            end
            loader.load_table(v, prefix .. k .. '.', root[k])

        -- �б��ļ�
        elseif 'string' == type(k) and 'string' == type(v) then
            loader.load_list(v, prefix .. k .. '.', root)

        -- ��ͨ�ļ�: table list
        elseif 'number' == type(k) and 'string' == type(v) then
            loader.load(prefix .. v)
            
        -- ��ͨ�ļ�: key-value����
        else
            root[k] = loader.load(prefix .. k)
        end
    end
end

-- ��ȫ�����б�, ���������userdata
function loader.load_list(modname, prefix, root)
    prefix = prefix or ''
    root = root or _G

    local ret = loader.load(prefix .. modname)
    if 'userdata' == type(ret) then
        log_error('load list file "' .. prefix .. modname .. '" failed')
        return false
    end

    loader.load_table(ret, prefix, root)
    return true
end

-- ������з�Ԥ��ģ��
function loader.clear()
    for k, v in pairs(loader.loaded_mods) do
        package.loaded[k] = nil
        -- log_debug('remove package %s', k)
    end
    package.loaded['utils.adaptor'] = nil
    package.loaded['utils.vardump'] = nil
    package.loaded['utils.conf'] = nil
    package.loaded['utils.class'] = nil
    package.loaded['utils.loader'] = nil
    package.loaded['utils.event'] = nil
    
    -- vardump(package.loaded, {ostream = log_stream, recursive = 1})

    -- ��պ�Ҫ��֤loader���ȱ����أ�����ʼ��utils��
    class_builder = require('utils.class')
    loader = require('utils.loader')
    require('utils.event')
end

function loader.reload(...)
    loader.clear()
    local args = {...}
    for k, v in ipairs(args) do
        loader.load_list(v)
    end
end


-- ��ȫԤ����ģ��, ���������userdata
-- ���ᱻclear���
function loader.preload(modname)
    local ret = loader.load(modname)
    if 'userdata' ~= type(ret) then
        package.preload[modname] = ret
    end

    return ret
end

return loader
