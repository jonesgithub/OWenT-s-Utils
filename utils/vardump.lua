--region vardump.lua
--Author : 文韬
--Date   : 2014/10/22
--此文件由[BabeLua]插件自动生成

require('utils.loader')

-- 额外拓展 -- vardump
_G.vardump = function (var, conf)
    local rm = {}
    local cfg = {
        show_metatable = false,
        show_table_once = false,
        ident = "    ",
        symbol_name = "symbol",
        recursive = nil, -- 递归打印的层数（nil表示不限制打印层数 -_-||）  
        ostream = io.stdout
    }
    
    if "table" == type(conf) then
        for k, v in pairs(conf) do
            cfg[k] = v
        end
    end
    
    local function pvar(obj, ident, root, recursive)
        local t = type(obj)
        local ident_symbol = cfg.ident
        
        if nil ~= recursive then
            recursive = recursive - 1
        end
        
        if nil == t then
            cfg.ostream:write("nil")
        elseif "table" == t and (nil == recursive or recursive >= 0) then
            local name = tostring(obj)
            cfg.ostream:write(name)
            if nil == rawget(rm, name) then
                -- 防止环形引用
                rawset(rm, name, "   ### [reference to " .. root .. "]")
                
                cfg.ostream:write(" {\n")
                -- metatable
                do
                    local meta = getmetatable(obj)
                    cfg.ostream:write(ident .. ident_symbol .. "__metatable : ")
                    if cfg.show_metatable then
                        pvar(meta, ident .. ident_symbol, name, recursive)
                    else
                        cfg.ostream:write(tostring(meta))
                        if "table" == type(meta) and cfg.symbol_name and nil ~= meta[cfg.symbol_name] then
                            cfg.ostream:write(" [" .. tostring(meta[cfg.symbol_name]) .. "]")
                        end
                    end
                    cfg.ostream:write("\n")
                end
                
                for k, v in pairs(obj) do
                    cfg.ostream:write(ident .. ident_symbol)
                    pvar(k, ident .. ident_symbol, name, recursive)
                    cfg.ostream:write(" : ")
                    pvar(v, ident .. ident_symbol, name, recursive)
                    cfg.ostream:write("\n")
                end
                cfg.ostream:write(ident .. "}")
                
                
                -- 如果不是强制只显示一次table则允许再显示
                if cfg.show_table_once then
                    rawset(rm, name, nil)
                end
            else
                cfg.ostream:write(rawget(rm, name))
            end
            
        else
            cfg.ostream:write(tostring(obj))
        end
    end
    
    pvar(var, "", "{root}", cfg.recursive)
    cfg.ostream:write("\n")
    cfg.ostream:flush ()
end

--  额外拓展 -- vardbg_assert
_G.vardbg_assert = function (var, check, conf, callback)
    if check then
        return
    end
    
    vardump(var, conf)
    if nil ~= callback then
        callback()
    end
end


return _G.vardump
--endregion
