
local class = {}

-- ��������
class.object = {
    __classname = 'utils.class.object',
    __type = 'class',   -- Ĭ������Ϊclass
}

-- tostring����
function class.object:__tostring()
    if self then
        return '[' .. self.__type .. ' : ' .. self.__classname .. ']'
    end
    return '[class : object]'
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

-- ע����������ռ�
function class.register (class_name, base_class)
    local ret = _G
    for k in string.gmatch(class_name, '[%a%d_%$]+') do
        local cache = ret[k]
        ret[k] = ret[k] or {}
        ret = ret[k]
    end
    
    rawset(ret, '__classname', class_name)
    base_class = base_class or class.object
    ret.__tostring = ret.__tostring or base_class.__tostring
    ret.__index = base_class
    setmetatable(ret, ret)
    

    -- Ĭ������create����
    -- @param inst ԭʼ����
    if nil == ret.create then
        rawset(ret, "create", function(inst)
            inst = inst or {}
            if nil == inst.__type then
                rawset(inst, '__type', 'object')    -- Ĭ������Ϊobject
            end
            inst.__tostring = inst.__tostring or ret.__tostring
            inst.__index = ret
            setmetatable(inst, inst)
            return inst
        end)
    end

    return ret
end


-- ��������
do
    class.singleton = class.register('utils.class.singleton')
    rawset(class.singleton, 'create', function(self, inst)
        if nil == self then
            error('singleton class must using class:create or class:instance(class.create and class.instance is unavailable)')
            return nil
        end
        return self
    end)

    rawset(class.singleton, 'instance', class.singleton.create)
end


-- �����ռ�
class.namespace = class.register('utils.class.namespace', class.singleton)

_G.utils = _G.utils or {}
_G.utils.class = class

class.register('utils', class.namespace)
class.register('utils.class', class.singleton)

return class