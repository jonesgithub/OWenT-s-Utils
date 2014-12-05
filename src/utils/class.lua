require('utils.adaptor')
local conf = require('utils.conf')

local id_allocator = { 0, 0, 0, 0 }

local class = {
    -- 公共基类
    object = {
        __classname = 'utils.class.object',
        __type = 'class',   -- 默认类型为class
    },

    -- 类注册表
    __class_reg_table = {}
}

-- 创建新的实例ID
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

-- tostring方法
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

-- 根据名字获取类
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

-- 注册类和命名空间
function class.register (class_info, base_class, class_name)
    local ret = class.__class_reg_table
    if 'string' == type(class_info) then
        local cache = nil
        for k in string.gmatch(class_info, '[%a%d_%$]+') do
            -- 新创建的父节点全部继承namespace
            if cache and class.namespace then
                class.inherit(cache, class.namespace)
            end

            -- 查找记录新节点
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

    -- 默认生成new方法
    -- @param inst 原始数据
    if nil == ret.new then
        rawset(ret, "new", function(inst)
            inst = inst or {}
            if nil == inst.__type then
                rawset(inst, '__type', 'object')    -- 默认类型为object
            end

            rawset(inst, '__instance_id', class.alloc_id()) -- instance id
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

-- 根命名空间
class.namespace = class.register('utils.class.__class_reg_table', class.namespace)

_G.utils = _G.utils or {}
_G.utils.class = class

class.register(_G.utils, class.namespace, 'utils')
class.register(_G.utils.class, class.singleton, 'utils.class')

return class