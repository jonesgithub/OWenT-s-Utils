require('utils.adaptor')
local conf = require('utils.conf')

local id_allocator = { 0, 0, 0, 0 }

local class = {
    -- ��������
    object = {
        __classname = 'utils.class.object',
        __type = 'class',   -- Ĭ������Ϊclass
    },

    -- ��ע���
    __class_reg_table = {}
}

-- �����µ�ʵ��ID
function class.alloc_id()
    local len = #id_allocator
    id_allocator[len] = id_allocator[len] + 1

    do
        local i = len
        while i >= 1 do
            if id_allocator[i] >= 65536 then
                id_allocator[i] = 0
                if i > 1 then
                    id_allocator[i - 1] = id_allocator[i - 1] + 1
                end
            else
                break
            end
            i = i - 1
        end
    end

    return string.format('%04x%04x%04x%04x', id_allocator[1], id_allocator[2], id_allocator[3], id_allocator[4])
end

-- tostring����
function class.object:__tostring()
    if self then
        local ret = '[' .. self.__type .. ' : ' .. self.__classname .. ']'
        if self.__instance_id then
            ret = ret .. ' @' .. tostring(self.__instance_id)
        end
        return ret
    end
    return '[class : object] WARNING: you should use obj:__tostring but obj.__tostring'
end

-- ��ȡ���෽��
function class.object:__parent()
    local m = getmetatable(self)
    if nil == m then
        return nil
    end

    if nil == m.__index then
        return nil;
    end

    if 'function' == type(m.__index) then
        return m:__index('__parent')
    end

    return m.__index
end

setmetatable(class.object, class.object)

-- ������ļ̳й�ϵ
-- @param child ����
-- @param father ����
function class.inherit (child, father)
    if child == father then
        return
    end

    child.__tostring = child.__tostring or father.__tostring
    setmetatable(child, child)
    rawset(child, '__index', father)
end

-- �������ֻ�ȡ��
function class.get (class_name)
    local ret = class.__class_reg_table
    for k in string.gmatch(class_name, '[%a%d_%$]+') do
        ret = ret[k] or nil
        if nil == ret then
            return ret
        end
    end

    return ret
end

-- ע����������ռ�
function class.register (class_info, base_class, class_name)
    local ret = class.__class_reg_table
    if 'string' == type(class_info) then
        local cache = nil
        for k in string.gmatch(class_info, '[%a%d_%$]+') do
            -- �´����ĸ��ڵ�ȫ���̳�namespace
            if cache and class.namespace then
                class.inherit(cache, class.namespace)
            end

            -- ���Ҽ�¼�½ڵ�
            if nil == ret[k] then
                ret[k] = {}
                cache = ret[k]
            else
                cache = nil
            end
            ret = ret[k]
        end

        if nil ~= ret.__classname and class_info ~= ret.__classname then
            if conf and conf.debug_mode then
                error(debug.traceback('class [' .. class_info .. '] has already registered to name "' .. ret.__classname .. '", cannot be registered again.'))
            else
                print(debug.traceback('class [' .. class_info .. '] has already registered to name "' .. ret.__classname .. '", cannot be registered again.'))
            end
            return ret
        end

        rawset(ret, '__classname', class_name or class_info)
    else
        ret = class_info
        if nil ~= class_name then
            rawset(ret, '__classname', class_name)
        end
    end

    base_class = base_class or class.object
    class.inherit(ret, base_class)

    -- Ĭ������new����
    -- @param inst ԭʼ����
    if nil == ret.new then
        rawset(ret, "new", function(inst)
            inst = inst or {}
            if nil == inst.__type then
                rawset(inst, '__type', 'object')    -- Ĭ������Ϊobject
            end

            rawset(inst, '__instance_id', class.alloc_id()) -- instance id
            class.inherit(inst, ret)
            return inst
        end)
    end

    return ret
end


-- ��������
do
    class.singleton = class.register('utils.class.singleton')
    rawset(class.singleton, 'new', function(self, inst)
        if nil == self then
            error('singleton class must using class:new or class:instance(class.new and class.instance is unavailable)')
            return nil
        end
        return self
    end)

    rawset(class.singleton, 'instance', class.singleton.new)
end

-- �����ռ�
class.namespace = class.register('utils.class.namespace', class.singleton)

-- ���ش���
class.native = class.register('utils.class.native', class.singleton)
rawset(class.native, '__type', 'native code')

-- �������ռ�
class.namespace = class.register('utils.class.__class_reg_table', class.namespace)

_G.utils = _G.utils or {}
_G.utils.class = class

class.register(_G.utils, class.namespace, 'utils')
class.register(_G.utils.class, class.singleton, 'utils.class')

return class