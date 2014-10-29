local class_builder = require('utils.class')

local loader = class_builder.register('utils.loader', class_builder.singleton)
loader.sysmod = {}

for k, v in pairs(_G) do
    if nil ~= v then
        loader.sysmod[k] = true
    end
end

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
        print(debug.traceback())
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
                class_builder.register(prefix .. k, class_builder.namespace)
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
        error('[ERROR] load list file "' .. prefix .. modname .. '" failed')
        return false
    end

    loader.load_table(ret, prefix, root)
    return true
end

-- ������з�Ԥ��ģ��
function loader.clear()
    for k, v in pairs(_G) do
        if nil ~= v and loader.sysmod[k] then
            _G[k] = nil
        end
    end
    -- ��ֹutils���ȼ��أ�����utils�����µ�����
    if nil ~= _G['utils'] then
        _G['utils'] = nil
    end

    -- ��պ�Ҫ��֤loader���ȱ����أ�����ʼ��utils��
    require('utils.loader')
end

function loader.reload()
    for k, v in pairs(_G) do
        if nil ~= v and loader.sysmod[k] then
            _G[k] = nil
        end
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
