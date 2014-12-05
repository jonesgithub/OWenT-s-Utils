--region data.list.lua
--Author : OWenT
--Date   : 2014/10/29
--����������

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

-- ����
do
    cfg:load('skill.skill_cfg', function(k, v)
        return v.id
    end)

    local skills = cfg:get('skill_cfg')

    for k, v in pairs(skills:get_all()) do
        if 'table' == type(v) then
            -- ���м��ܶ�������key(id,level)
            cfg:load(string.format('skill.%d', v.id), function(ck, cv)
                return cv.id, cv.level
            end)
        end
    end
end


-- ս������
do
    cfg:load('fight.fight_scene_cfg', function(k, v)
        return v.id
    end)

    local scenes = cfg:get('fight_scene_cfg')

    for k, v in pairs(scenes:get_all()) do
        if 'table' == type(v) then
            -- ����ս������Ҳ��������key(type id,level)
            cfg:load(string.format('fight.%d', v.id), function(ck, cv)
                return cv.id, cv.level
            end)
        end
    end
end

-- ������
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
        -- ����ս������Ҳ��������key(type id,level)
        cfg:load(string.format('unit_list.%d', v), function(ck, cv)
            return cv.id
        end)
    end
end

-- buff��
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
        -- ����buff��������key(type id,level)
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