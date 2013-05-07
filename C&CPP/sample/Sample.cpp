
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "String/TQueryString.h"
#include "Logic/AttributeManager.h"
#include "Algorithm/Hash.h"

//=======================================================================================================
void TQueryStringSample()
{
    printf("\n");
    printf("===============begin querystring sample==============\n");

    util::TQueryString encode, decode;

    encode.Set("a", "wulala");
    encode.Set("page", "ok!");

    std::string output;
    encode.Encode(output);

    util::types::ItemArray::ptr_type arr = encode.CreateArray();

    arr->Append("blablabla...");
    arr->Append("a and b is ab");
    util::types::ItemObject::ptr_type obj = encode.CreateObject();
    obj->Set("so", "");
    arr->Append(obj);

    encode.Set("c", arr);

    std::cout<< "Encode => "<< encode.ToString()<< std::endl;
    std::cout<< "Encode (old) => "<< output<< std::endl;
    std::cout<< "Array => "<< arr->ToString()<< std::endl;

    decode.Decode(encode.ToString().c_str());
    std::cout<< "Decode => "<< decode.ToString()<< std::endl;

    printf("===============end querystring sample================\n");
}

//=======================================================================================================


//=======================================================================================================
enum EN_SAMPLE_ATTR
{
    ESA_UNKNOWN = 0,
    ESA_STRENTH = 1,
    ESA_BLABLAB = 2,
    ESA_BASIC_ATTACK = 3,
    ESA_ATTACK = 4,
    ESA_MAX_HP = 5,
};

struct AttributeManagerSampleValid
{
    typedef util::logic::AttributeManager<EN_SAMPLE_ATTR, ESA_MAX_HP + 1, AttributeManagerSampleValid, int> mt;

    static void GenAttrFormulaMap(mt::formula_builder_type& stFormulas)
    {
        using namespace util::logic::Operator;

        /* ��ʽ��������� = ���� * 2 + BLABLABLA */
        stFormulas[ESA_MAX_HP] = stFormulas[ESA_STRENTH] * 2 + _<mt>(ESA_BLABLAB);
        /* ��ʽ�������� = 100 * �������� - ���� */
        stFormulas[ESA_UNKNOWN] = stFormulas[ESA_ATTACK] = 100 * _<mt>(ESA_BASIC_ATTACK) - _<mt>(ESA_STRENTH) + _<mt>(ESA_STRENTH) * _<mt>(ESA_BASIC_ATTACK);
        /* ��ʽ���������� = BLABLABLA / 5 */
        stFormulas[ESA_BASIC_ATTACK] = _<mt>(ESA_BLABLAB) / 5;

    }
};


struct AttributeManagerSampleInvalid
{
    typedef util::logic::AttributeManager<EN_SAMPLE_ATTR, ESA_MAX_HP + 1, AttributeManagerSampleInvalid, int> mt;

    static void GenAttrFormulaMap(mt::formula_builder_type& stFormulas)
    {
        using namespace util::logic::Operator;

        /* ��ʽ��������� = 2 * ���� + BLABLABLA / �������� */
        stFormulas[ESA_MAX_HP] = 2 * stFormulas[ESA_STRENTH] + _<mt>(ESA_BLABLAB) / stFormulas[ESA_BASIC_ATTACK];
        /* ��ʽ��ESA_UNKNOWN = ������ = 100 * �������� - ���� + ���� * �������� */
        stFormulas[ESA_UNKNOWN] = stFormulas[ESA_ATTACK] = 100 * _<mt>(ESA_BASIC_ATTACK) - stFormulas[ESA_STRENTH] + stFormulas[ESA_STRENTH] * stFormulas[ESA_BASIC_ATTACK];
        /* ��ʽ���������� = BLABLABLA / 5 + ������� */
        stFormulas[ESA_BASIC_ATTACK] = _<mt>(ESA_BLABLAB) / 5 + stFormulas[ESA_MAX_HP];

        /* ��ʽѭ����ESA_BLABLAB = ESA_BLABLAB + ������� / 100 */
        stFormulas[ESA_BLABLAB] = stFormulas[ESA_BLABLAB]() + stFormulas[ESA_MAX_HP] / 100;

        /* ��ʽѭ�������� = ������� / 10 - �������� * ESA_BLABLAB */
        stFormulas[ESA_STRENTH] = _<mt>(ESA_MAX_HP) / 10 - stFormulas[ESA_BASIC_ATTACK] * stFormulas[ESA_BLABLAB];

        /* �����ǲ��ӡ���л��ģ����������ϵʽ�������ˡ�Ŀ��ò�����еĻ�����ȷ����� */
    }
};


template<typename _T>
void print_stl(_T& stContainer)
{
    std::string strPrefix = "\t";

    for (typename _T::iterator iter = stContainer.begin();
        iter != stContainer.end();
        ++ iter)
    {
        std::cout<< strPrefix<< *iter;
        strPrefix = ", ";
    }

    std::cout<< std::endl;
}

void AttributeManagerSample()
{
    puts("");
    puts("===============begin attribute manager sample==============");
    AttributeManagerSampleValid::mt foo;

    std::cout<< "Formula check: "<< (foo.CheckValid()? "OK": "Failed")<< std::endl;
    AttributeManagerSampleValid::mt::PrintInvalidLoops(std::cout);
    std::cout<< "Formula check: "<< (AttributeManagerSampleInvalid::mt::CheckValid()? "OK": "Failed")<< std::endl;
    AttributeManagerSampleInvalid::mt::PrintInvalidLoops(std::cout);

    if (false == foo.CheckValid())
    {
        return;
    }

    // ��ʼ�ÿ�
    foo.Construct();
    foo[ESA_STRENTH] = 1000;
    foo[ESA_BLABLAB] = 512;

    // ��鱻������ϵ
    AttributeManagerSampleValid::mt::attr_attach_list_type stList;

    // ���� ESA_STRENTH ������
    foo[ESA_STRENTH].GetAttachedAttributes(stList, false);
    std::cout<< "Attached: ESA_STRENTH => should be 3, real is "<< stList.size()<< std::endl;
    print_stl(stList);


    // ���� ESA_BLABLAB ������
    stList.clear();
    foo[ESA_BLABLAB].GetAttachedAttributes(stList, false);
    std::cout<< "Attached: ESA_BLABLAB => should be 2, real is "<< stList.size()<< std::endl;
    print_stl(stList);

    // ���� ESA_BLABLAB ������ - Recursion
    stList.clear();
    foo[ESA_BLABLAB].GetAttachedAttributes(stList, true);
    std::cout<< "Attached: ESA_BLABLAB - Recursion => should be 4, real is "<< stList.size()<< std::endl;
    print_stl(stList);

    // ���� ESA_ATTACK ������
    stList.clear();
    foo[ESA_ATTACK].GetAttachedAttributes(stList, true);
    std::cout<< "Attached: ESA_ATTACK => should be 0, real is "<< stList.size()<< std::endl;

    // ���� ESA_UNKNOWN ������
    stList.clear();
    foo[ESA_UNKNOWN].GetAttachedAttributes(stList, true);
    std::cout<< "Attached: ESA_UNKNOWN => should be 0, real is "<< stList.size()<< std::endl;

    // ��������
    AttributeManagerSampleValid::mt::attr_attach_set_type stSet;

    // ESA_STRENTH �Ĺ�������
    foo[ESA_STRENTH].GetAttachAttributes(stSet, true);
    std::cout<< "Attach: ESA_STRENTH => should be 0, real is "<< stSet.size()<< std::endl;

    // ESA_MAX_HP �Ĺ�������
    stSet.clear();
    foo[ESA_MAX_HP].GetAttachAttributes(stSet, true);
    std::cout<< "Attach: ESA_MAX_HP => should be 2, real is "<< stSet.size()<< std::endl;
    print_stl(stSet);

    // ESA_ATTACK �Ĺ�������
    stSet.clear();
    foo[ESA_ATTACK].GetAttachAttributes(stSet, false);
    std::cout<< "Attach: ESA_ATTACK => should be 2, real is "<< stSet.size()<< std::endl;
    print_stl(stSet);

    // ESA_ATTACK �Ĺ������� - Recursion
    stSet.clear();
    foo[ESA_ATTACK].GetAttachAttributes(stSet, true);
    std::cout<< "Attach: ESA_ATTACK - Recursion => should be 3, real is "<< stSet.size()<< std::endl;
    print_stl(stSet);

    // ���ֵ����ʽ������
    std::cout<< "ESA_UNKNOWN => "<< foo[ESA_UNKNOWN]<< "(ESA_UNKNOWN = ESA_ATTACK)"<< std::endl;
    std::cout<< "ESA_STRENTH => "<< foo[ESA_STRENTH]<< std::endl;
    std::cout<< "ESA_BLABLAB => "<< foo[ESA_BLABLAB]<< std::endl;
    std::cout<< "ESA_BASIC_ATTACK => "<< foo[ESA_BASIC_ATTACK]<< "([ESA_BLABLAB] / 5: "<< foo[ESA_BLABLAB] / 5<< ")"<< std::endl;
    std::cout<< "ESA_ATTACK => "<< foo[ESA_ATTACK]<<
        "(100 * [ESA_BASIC_ATTACK] - [ESA_STRENTH] + [ESA_STRENTH] * [ESA_BASIC_ATTACK]: "<<
        100 * foo[ESA_BASIC_ATTACK] - foo[ESA_STRENTH] + foo[ESA_STRENTH] * foo[ESA_BASIC_ATTACK]<<
        ")"<< std::endl;
    std::cout<< "ESA_MAX_HP => "<< foo[ESA_MAX_HP]<<
        "([ESA_STRENTH] * 2 + [ESA_BLABLAB]: "<<
        foo[ESA_STRENTH] * 2 + foo[ESA_BLABLAB]<<
        ")"<< std::endl;

    puts("===============end attribute manager sample==============");
}

//=======================================================================================================

//=======================================================================================================
void HashSample()
{
    puts("");
    puts("===============begin hash sample==============");
    char strBuff[] = "Hello World!\nI'm OWenT\n";
    printf("Hashed String: \n%s\n", strBuff);

    std::cout<< "FNV-1:   "<< util::hash::HashFNV1<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "FNV-1A:  "<< util::hash::HashFNV1A<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "SDBM:    "<< util::hash::HashSDBM<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "RS:      "<< util::hash::HashRS<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "JS:      "<< util::hash::HashJS<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "PJW:     "<< util::hash::HashPJW<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "ELF:     "<< util::hash::HashELF<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "BKDR:    "<< util::hash::HashBKDR<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "DJB:     "<< util::hash::HashDJB<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    std::cout<< "AP:      "<< util::hash::HashAP<uint32_t>(strBuff, strlen(strBuff)) <<std::endl;
    puts("===============end hash sample==============");
}
//=======================================================================================================

int main(int argc, char** argv)
{
    TQueryStringSample();
    AttributeManagerSample();
	HashSample();
    return 0;
}
