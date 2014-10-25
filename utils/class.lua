local conf = require('conf')

local class = {}

-- 公共基类
class.object = {
    __classname = 'utils.class.object',
    __type = 'class',   -- 默认类型为class
}

-- tostring方法
function class.object:__tostring()
    if self then
        return '[' .. self.__type .. ' : ' .. self.__classname .. ']'
    end
    return '[class : object] WARNING: you should use obj:__tostring but obj.__tostring'
end

-- 获取父类方法
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

-- 设置类的继承关系
-- @param child 子类
-- @param father 父类
function class.inherit (child, father)
    if child == father then
        return
    end

    child.__tostring = child.__tostring or father.__tostring
    setmetatable(child, child)
    rawset(child, '__index', father)
end

-- 注册类和命名空间
function class.register (class_name, base_class)
    local ret = _G
    if 'string' == type(class_name) then
        local cache = nil
        for k in string.gmatch(class_name, '[%a%d_%$]+') do
            local cache = ret[k] or {}
            ret = cache
        end

        if nil ~= ret.__classname and class_name ~= ret.__classname then
            if conf and conf.debug_mode then
                error(debug.traceback('class [' .. class_name .. '] has already registered to name "' .. ret.__classname .. '", cannot be registered again.'))
            else
                print(debug.traceback('class [' .. class_name .. '] has already registered to name "' .. ret.__classname .. '", cannot be registered again.'))
            end
            return ret
        end

        rawset(ret, '__classname', class_name)
    else
        ret = class_name
    end

    base_class = base_class or class.object
    class.inherit(ret, base_class)
    

    -- 默认生成new方法
    -- @param inst 原始数据
    if nil == ret.new then
        rawset(ret, "new", function(inst)
            inst = inst or {}
            if nil == inst.__type then
                rawset(inst, '__type', 'object')    -- 默认类型为object
            end
            class.inherit(inst, ret)
            return inst
        end)
    end

    return ret
end


-- 单例基类
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

-- 命名空间
class.namespace = class.register('utils.class.namespace', class.singleton)

-- 本地代码
class.native = class.register('utils.class.native', class.singleton)
rawset(class.native, '__type', 'native code')

_G.utils = _G.utils or {}
_G.utils.class = class

class.register('utils', class.namespace)
class.register('utils.class', class.singleton)

return class