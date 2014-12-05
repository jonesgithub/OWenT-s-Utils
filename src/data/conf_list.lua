--region data.list.lua
--Author : OWenT
--Date   : 2014/10/29
--启动载入项

local loader = require('utils.loader')
local cfg = loader.load('data.conf_manager')

-- role_cfg
cfg:load('unit.role_cfg', function(k, v)
    return v.id or k
end)

vardump(cfg:get('role_cfg'))

cfg:load('unit.missile_cfg', function(k, v)
    return v.id or k
end)

-- 技能
do
    cfg:load('skill.skill_cfg', function(k, v)
        return v.id
    end)

    local skills = cfg:get('skill_cfg')

    for k, v in pairs(skills:get_all()) do
        if 'table' == type(v) then
            -- 所有技能都是两个key(id,level)
            cfg:load(string.format('skill.%d', v.id), function(ck, cv)
                return cv.id, cv.level
            end)
        end
    end
end


-- 战斗场景
do
    cfg:load('fight.fight_scene_cfg', function(k, v)
        return v.id
    end)

    local scenes = cfg:get('fight_scene_cfg')

    for k, v in pairs(scenes:get_all()) do
        if 'table' == type(v) then
            -- 所有战斗场景也都是两个key(type id,level)
            cfg:load(string.format('fight.%d', v.id), function(ck, cv)
                return cv.id, cv.level
            end)
        end
    end
end

-- 出兵表
do
    cfg:load('unit_list.unit_list_cfg', function(k, v)
        return v.id
    end)

    local list = cfg:get('unit_list_cfg')
    local id_set = {}
    for k, v in pairs(list:get_all()) do
        if 'table' == type(v) then
            table.insert(id_set, v. id)
        end
    end

    for k, v in ipairs(id_set) do
        -- 所有战斗场景也都是两个key(type id,level)
        cfg:load(string.format('unit_list.%d', v), function(ck, cv)
            return cv.id
        end)
    end
end

-- buff表
do
    cfg:load('buff.buff_cfg', function(k, v)
        return v.id
    end)

    local buffs = cfg:get('buff_cfg')
    local id_set = {}
    for k, v in pairs(buffs:get_all()) do
        if 'table' == type(v) then
            table.insert(id_set, v. id)
        end
    end

    for k, v in ipairs(id_set) do
        -- 所有buff都是两个key(type id,level)
        cfg:load(string.format('buff.%d', v), function(ck, cv)
            return cv.id
        end)
    end
end

do
    cfg:load('gfx.gfx_cfg', function(k, v)
        return v.id or k
    end)
end