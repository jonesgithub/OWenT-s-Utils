<?php


$obj_config = array(
    'value' => array('type' => 'typename _TAttrMgr::attr_value_type', 'create' => 'AttrOprVal<_TAttrMgr>::Create(%v)'),
    'base' => array('type' => 'std::shared_ptr<AttrOprBase<_TAttrMgr> >', 'create' => '%v'),
    'attr_wrapper' => array('type' => 'const Wrapper::AttributeFormulaBuilderAttrWrapper<_TAttrMgr>&', 'create' => '%v()')
);

function gen_binary_operator($opr, $cls, $left, $right){?>

            template<typename _TAttrMgr>
            std::shared_ptr<AttrOprBase<_TAttrMgr> > operator<?php echo $opr ?>(<?php echo $left['type']; ?> l, <?php echo $right['type']; ?> r)
            {
                return <?php echo $cls; ?><_TAttrMgr>::Create(<?php echo str_replace('%v', 'l', $left['create']); ?>, <?php echo str_replace('%v', 'r', $right['create']); ?>);
            }
<?php
}

function gen_plus_function($left, $right){
    gen_binary_operator('+', 'AttrOprPlus', $left, $right);
}

function gen_minu_function($left, $right){
    gen_binary_operator('-', 'AttrOprMinu', $left, $right);
}

function gen_mult_function($left, $right){
    gen_binary_operator('*', 'AttrOprMult', $left, $right);
}

function gen_devi_function($left, $right){
    gen_binary_operator('/', 'AttrOprDevi', $left, $right);
}

function gen_all_operator_function($left, $right){
    gen_plus_function($left, $right);
    gen_minu_function($left, $right);
    gen_mult_function($left, $right);
    gen_devi_function($left, $right);
}

function gen_all_operator($left_name, $right_name){
    global $obj_config;
    if(isset($obj_config[$left_name]) && isset($obj_config[$right_name]))
        gen_all_operator_function($obj_config[$left_name], $obj_config[$right_name]);
}

?>/**
 * @file AttributeManager.h
 * @brief ͨ������ϵͳ<br />
 *        ֧���Զ�����������ϵ��ʽ
 *
 * @version 1.0
 * @author OWenT
 * @date 2013-04-19
 *
 * @history
 *
 */

#ifndef _BASELIB_HASH_H_
#define _BASELIB_HASH_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <map>
#include <set>
#include <list>
#include <vector>
#include <algorithm>
#include "std/smart_ptr.h"
#include "std/ref.h"

namespace util
{
    namespace logic
    {
        namespace Operator
        {
            /**
             * ���Թ�ϵ���ʽ--����
             */
            template<typename _TAttrMgr>
            struct AttrOprBase: public std::enable_shared_from_this< AttrOprBase<_TAttrMgr> >
            {
                typedef typename _TAttrMgr::attr_value_type attr_value_type;
                typedef typename _TAttrMgr::attr_class_type attr_class_type;
                typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;

                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;

                virtual ~AttrOprBase(){}
                virtual attr_value_type operator()(_TAttrMgr&) = 0;

                virtual void BuildFormularParam(attr_attach_set_type&) {}
            };

            /**
             * ���Թ�ϵ���ʽ��--ֵ����
             */
            template<typename _TAttrMgr>
            struct AttrOprVal : public AttrOprBase<_TAttrMgr>
            {
                typedef AttrOprVal<_TAttrMgr> self_type;
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef typename base_type::attr_value_type attr_value_type;
                typedef typename base_type::attr_class_type attr_class_type;

                attr_value_type tVal;
                AttrOprVal(attr_value_type val): tVal(val){}

                virtual attr_value_type operator()(_TAttrMgr&)
                {
                    return tVal;
                }

                static base_ptr_type Create(attr_value_type v)
                {
                    return std::shared_ptr<base_type>(new self_type(v));
                }
            };

            /**
             * ���Թ�ϵ���ʽ--��������
             */
            template<typename _TAttrMgr>
            struct AttrOprAttr : public AttrOprBase<_TAttrMgr>
            {
                typedef AttrOprAttr<_TAttrMgr> self_type;
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef typename base_type::attr_value_type attr_value_type;
                typedef typename base_type::attr_class_type attr_class_type;
                typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;

                attr_class_type tAttrType;
                AttrOprAttr(attr_class_type val): tAttrType(val){}

                virtual attr_value_type operator()(_TAttrMgr& stMgr)
                {
                    return stMgr[tAttrType];
                }

                static base_ptr_type Create(attr_class_type v)
                {
                    return std::shared_ptr<base_type>(new self_type(v));
                }

                virtual void BuildFormularParam(attr_attach_set_type& stAttachSet) 
                {
                    stAttachSet.insert(tAttrType);
                }
            };

            /**
             * ���Թ�ϵ���ʽ--˫Ŀ���������
             */
            template<typename _TAttrMgr, typename _TReal>
            struct AttrOprBinaryOperation: public AttrOprBase<_TAttrMgr>
            {
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef _TReal self_type;
                typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;

                base_ptr_type left, right;

                AttrOprBinaryOperation(base_ptr_type l, base_ptr_type r): left(l), right(r){}

                static base_ptr_type Create(base_ptr_type left, base_ptr_type right)
                {
                    return std::shared_ptr<base_type>(new self_type(left, right));
                }

                virtual void BuildFormularParam(attr_attach_set_type& stAttachSet) 
                {
                    left->BuildFormularParam(stAttachSet);
                    right->BuildFormularParam(stAttachSet);
                }
            };

            /**
             * ���Թ�ϵ���ʽ--�ӷ�����
             */
            template<typename _TAttrMgr>
            struct AttrOprPlus : public AttrOprBinaryOperation<_TAttrMgr, AttrOprPlus<_TAttrMgr> >
            {
                typedef AttrOprPlus<_TAttrMgr> self_type;
                typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef typename base_type::attr_value_type attr_value_type;

                using opr_base_type::left;
                using opr_base_type::right;

                AttrOprPlus(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

                virtual attr_value_type operator()(_TAttrMgr& stMgr)
                {
                    return (*left)(stMgr) + (*right)(stMgr);
                }
            };

            /**
             * ���Թ�ϵ���ʽ--��������
             */
            template<typename _TAttrMgr>
            struct AttrOprMinu : public AttrOprBinaryOperation<_TAttrMgr, AttrOprMinu<_TAttrMgr> >
            {
                typedef AttrOprMinu<_TAttrMgr> self_type;
                typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef typename base_type::attr_value_type attr_value_type;

                using opr_base_type::left;
                using opr_base_type::right;

                AttrOprMinu(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

                virtual attr_value_type operator()(_TAttrMgr& stMgr)
                {
                    return (*left)(stMgr) - (*right)(stMgr);
                }
            };

            /**
             * ���Թ�ϵ���ʽ--�˷�����
             */
            template<typename _TAttrMgr>
            struct AttrOprMult : public AttrOprBinaryOperation<_TAttrMgr, AttrOprMult<_TAttrMgr> >
            {
                typedef AttrOprMult<_TAttrMgr> self_type;
                typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef typename base_type::attr_value_type attr_value_type;

                using opr_base_type::left;
                using opr_base_type::right;

                AttrOprMult(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

                virtual attr_value_type operator()(_TAttrMgr& stMgr)
                {
                    return (*left)(stMgr) * (*right)(stMgr);
                }

            };

            /**
             * ���Թ�ϵ���ʽ--��������
             */
            template<typename _TAttrMgr>
            struct AttrOprDevi : public AttrOprBinaryOperation<_TAttrMgr, AttrOprDevi<_TAttrMgr> >
            {
                typedef AttrOprDevi<_TAttrMgr> self_type;
                typedef AttrOprBinaryOperation<_TAttrMgr, self_type > opr_base_type;
                typedef AttrOprBase<_TAttrMgr> base_type;
                typedef std::shared_ptr<base_type> base_ptr_type;
                typedef typename base_type::attr_value_type attr_value_type;

                using opr_base_type::left;
                using opr_base_type::right;

                AttrOprDevi(base_ptr_type l, base_ptr_type r): opr_base_type(l, r){}

                virtual attr_value_type operator()(_TAttrMgr& stMgr)
                {
                    return (*left)(stMgr) / (*right)(stMgr);
                }
            };

            template<typename _AttrMgr>
            typename AttrOprAttr<_AttrMgr>::base_ptr_type _(typename AttrOprAttr<_AttrMgr>::attr_class_type type)
            {
                return AttrOprAttr<_AttrMgr>::Create(type);
            }
        }

        namespace Wrapper
        {
            template<typename _TAttrMgr>
            class AttributeFormulaBuilderAttrWrapper
            {
            public:
                typedef typename _TAttrMgr::attr_class_type attr_class_type;
                typedef typename _TAttrMgr::formula_map_type formula_map_type;
                typedef typename _TAttrMgr::attr_formula_ptr_type attr_formula_ptr_type;
                typedef AttributeFormulaBuilderAttrWrapper<_TAttrMgr> self_type;

            private:
                attr_class_type m_tClass;

            public:
                AttributeFormulaBuilderAttrWrapper(attr_class_type tClass): m_tClass(tClass){}

                attr_formula_ptr_type operator()() const
                {
                    return Operator::_<_TAttrMgr>(m_tClass);
                }

                /**
                 * �ṩ�Զ�תΪ���Բ����Ĺ���
                 */
                operator attr_formula_ptr_type() const
                {
                    return (*this)(m_tClass);
                }

                /**
                 * ��ʽ��ֵ����
                 */
                self_type& operator=(attr_formula_ptr_type pFormula)
                {
                    formula_map_type& stFormulaMap = _TAttrMgr::GetFormulaMapObj();
                    stFormulaMap[m_tClass] = pFormula;
                    return *this;
                }

                /**
                 * ���Ƹ�ֵ����(֧�����Ⱥ�)
                 */
                self_type& operator=(const self_type& stFormulaWrapper)
                {
                    formula_map_type& stFormulaMap = _TAttrMgr::GetFormulaMapObj();
                    typename formula_map_type::iterator iter = stFormulaMap.find(stFormulaWrapper.m_tClass);

                    // [�Ż�] ���Ŀ�������ɹ�ʽ���ɣ������һ��ڵ�
                    if (iter == stFormulaMap.end())
                    {
                        stFormulaMap[m_tClass] = Operator::_<_TAttrMgr>(stFormulaWrapper.m_tClass);
                    }
                    else
                    {
                        stFormulaMap[m_tClass] = iter->second;
                    }

                    return *this;
                }
            };


            template<typename _TAttrMgr>
            class AttributeFormulaBuilderWrapper
            {
            public:
                typedef typename _TAttrMgr::attr_class_type attr_class_type;
                typedef typename _TAttrMgr::attr_value_type attr_value_type;
                typedef AttributeFormulaBuilderAttrWrapper<_TAttrMgr> builder_attr_type;

            public:

                builder_attr_type operator[](attr_class_type tClass)
                {
                    return builder_attr_type(tClass);
                }

                builder_attr_type operator()(attr_class_type tClass)
                {
                    return builder_attr_type(tClass);
                }
            };

            /**
             * ����ֵ��װ��
             */
            template<typename _TAttrMgr>
            class AttributeWrapper
            {
            public:
                typedef typename _TAttrMgr::attr_class_type attr_class_type;
                typedef typename _TAttrMgr::attr_value_type attr_value_type;
                typedef typename _TAttrMgr::attr_attach_set_type attr_attach_set_type;
                typedef typename _TAttrMgr::attr_attach_list_type attr_attach_list_type;
                typedef typename _TAttrMgr::formula_map_type formula_map_type;
                typedef typename _TAttrMgr::attr_attach_type attr_attach_type;

                typedef AttributeWrapper<_TAttrMgr> self_type;

            private:
                std::reference_wrapper<_TAttrMgr> m_stMgrRef;
                attr_class_type m_tIndex;

            public:
                AttributeWrapper(_TAttrMgr& stMgr, attr_class_type tIndex): m_stMgrRef(stMgr), m_tIndex(tIndex){}

                operator attr_value_type() const
                {
                    return m_stMgrRef.get().get(m_tIndex);
                }

                /**
                 * ֵ�����仯
                 * @note ͬ���ı������
                 * @param [in] tVal �ı��������
                 * @return ���ݵȺŹ淶, ����*this
                 */
                self_type& operator=(attr_value_type tVal)
                {
                    m_stMgrRef.get().get(m_tIndex) = tVal;

                    attr_attach_list_type stAttachedList;
                    GetAttachedAttributes(stAttachedList, false);

                    formula_map_type& stFormulaMap = m_stMgrRef.get().GetFormulaMap();
                    
                    typename attr_attach_list_type::iterator iter = stAttachedList.begin();
                    for(; iter != stAttachedList.end(); ++ iter)
                    {
                        typename formula_map_type::iterator itAttached = stFormulaMap.find(*iter);
                        if (itAttached == stFormulaMap.end() || *iter == m_tIndex)
                        {
                            // ���������⣬�Ǿ��ǳ�����BUG
                            // ����������ϵ�ɻ�
                            // ���߳���������
                            continue;
                        }

                        // ѭ����ֵ
                        m_stMgrRef.get()[*iter] = (*itAttached->second)(m_stMgrRef);
                    }

                    return (*this);
                }

                self_type& operator=(const self_type& tVal)
                {
                    m_stMgrRef = std::ref(tVal.m_stMgrRef);
                    m_tIndex = tVal.m_tIndex;

                    return (*this);
                }

                /**
                 * ��ȡ���б���ǰ��������������<br />
                 * ������ǰ�����ǻ�õ����ԵĲ���
                 * @param [in, out] stAttrList ������Լ�
                 * @param [in] bRecursion �Ƿ�ݹ����������
                 */
                void GetAttachedAttributes(attr_attach_list_type& stAttrList, bool bRecursion = false)
                {
                    _TAttrMgr& stMgr = m_stMgrRef;
                    attr_attach_type& stAttachMap = stMgr.GetAttachAttrMap();
                    typename attr_attach_type::iterator iter = stAttachMap.find(m_tIndex);

                    // û�ҵ���ϵʽ
                    if (iter == stAttachMap.end())
                    {
                        return;
                    }

                    // �������
                    stAttrList.insert(stAttrList.end(), iter->second.begin(), iter->second.end());
                    if ( false == bRecursion || false == _TAttrMgr::CheckValid())
                    {
                        return;
                    }

                    // ��Ҫ�ݹ����
                    typedef typename attr_attach_type::mapped_type attach_mapped;
                    typename attach_mapped::iterator itm = iter->second.begin();
                    for (; itm != iter->second.end(); ++ itm)
                    {
                        self_type stTmpObj(stMgr, *itm);
                        stTmpObj.GetAttachedAttributes(stAttrList, bRecursion);
                    }
                }

                /**
                 * ��ȡ���е�ǰ���Եļ����������<br />
                 * ������õ������ǵĵ�ǰ���Բ���
                 * @param [in, out] stAttrSet ������Լ�
                 * @param [in] bRecursion �Ƿ�ݹ���Ҳ���
                 */
                void GetAttachAttributes(attr_attach_set_type& stAttrSet, bool bRecursion = false)
                {
                    _TAttrMgr& stMgr = m_stMgrRef;
                    formula_map_type& stFormulaMap = stMgr.GetFormulaMap();
                    typename formula_map_type::iterator iter = stFormulaMap.find(m_tIndex);

                    // û�ҵ���ʽ
                    if (iter == stFormulaMap.end())
                    {
                        return;
                    }

                    // �������
                    attr_attach_set_type stCurrentSet;
                    iter->second->BuildFormularParam(stCurrentSet);
                    stAttrSet.insert(stCurrentSet.begin(), stCurrentSet.end());

                    if ( false == bRecursion || false == _TAttrMgr::CheckValid())
                    {
                        return;
                    }

                    // ��Ҫ�ݹ����
                    typename attr_attach_set_type::iterator itm = stCurrentSet.begin();
                    for (; itm != stCurrentSet.end(); ++ itm)
                    {
                        self_type stTmpObj(stMgr, *itm);
                        stTmpObj.GetAttachAttributes(stAttrSet, bRecursion);
                    }
                }
            };
        }

        /**
         * ���Թ�����
         * @note Ӧ�ð�������һ�����Թ�ʽ
         * @note ������_TOwner�������GenAttrFormulaMap(formula_builder_type&)�����Գ�ʼ�����Թ�ʽ��
         * @note Ϊ����߲�ѯЧ��, ���Ա����������, �ʶ�_MAXCOUNT ���˹���
         */
        template<typename _TAttrType, int _MAXCOUNT, typename _TOwner, typename _TAttr = int>
        class AttributeManager
        {
        public:
            typedef _TAttrType attr_class_type;
            typedef _TAttr attr_value_type;
            typedef AttributeManager<_TAttrType, _MAXCOUNT, _TOwner, _TAttr> self_type;

            typedef std::set<_TAttrType> attr_attach_set_type;
            typedef std::list<_TAttrType> attr_attach_list_type;
            typedef std::map<_TAttrType, attr_attach_list_type > attr_attach_type;

            typedef Operator::AttrOprBase<self_type> attr_formula_type;
            typedef typename attr_formula_type::base_ptr_type attr_formula_ptr_type;
            typedef std::map<_TAttrType, attr_formula_ptr_type > formula_map_type;
            typedef Wrapper::AttributeFormulaBuilderWrapper<self_type> formula_builder_type;

            typedef Wrapper::AttributeWrapper<self_type> attr_wrapper_type;

            typedef std::list< std::list<int> > formula_loops_type;

            friend class Wrapper::AttributeWrapper<self_type>;
            friend class Wrapper::AttributeFormulaBuilderWrapper<self_type>;
            friend class Wrapper::AttributeFormulaBuilderAttrWrapper<self_type>;

        private:
            _TAttr m_arrAttrs[_MAXCOUNT];

            static attr_attach_type& GetAttachAttrMapObj()
            {
                static attr_attach_type functor;
                return functor;
            }

            static formula_map_type& GetFormulaMapObj()
            {
                static formula_map_type functor;
                return functor;
            }

            static attr_attach_type& GetAttachAttrMap()
            {
                // ��֤�ѳ�ʼ��
                GetFormulaMap();

                return GetAttachAttrMapObj();
            }

            static formula_map_type& GetFormulaMap()
            {
                formula_map_type& stFormula = GetFormulaMapObj();
                if (stFormula.begin() == stFormula.end())
                {
                    // �������Թ�ʽ
                    formula_builder_type stBuilder;
                    _TOwner::GenAttrFormulaMap(stBuilder);

                    // �������Թ�ϵ�ڽӱ�
                    attr_attach_type& stAttrAttach = GetAttachAttrMapObj();
                    stAttrAttach.clear();
                    for(typename formula_map_type::iterator iter = stFormula.begin();
                        iter != stFormula.end();
                        ++ iter)
                    {
                        attr_attach_set_type stAttachAttrs;

                        iter->second->BuildFormularParam(stAttachAttrs);
                        for (typename attr_attach_set_type::iterator itAttr = stAttachAttrs.begin();
                            itAttr != stAttachAttrs.end();
                            ++ itAttr
                            )
                        {
                            typename attr_attach_type::mapped_type& stList = stAttrAttach[*itAttr];

                            stList.push_back(iter->first);
                        }
                    }
                }

                return stFormula;
            }

        public:

            void Construct()
            {
                memset(m_arrAttrs, 0, sizeof(m_arrAttrs));
            }

            /**
             * ��ȡ����ԭʼ���ö���
             * @param [in] uIndex ��������
             * @return ԭʼ��������
             */
            _TAttr& get(_TAttrType uIndex)
            {
                return m_arrAttrs[uIndex];
            }

            /**
             * ��ȡ����ԭʼ���ö���
             * @param [in] uIndex ��������
             * @return ԭʼ��������
             */
            const _TAttr& get(_TAttrType uIndex) const
            {
                return m_arrAttrs[uIndex];
            }

            /**
             * ��ȡ����
             * @param [in] uIndex ��������
             * @return ���Զ���
             */
            attr_wrapper_type operator[](_TAttrType uIndex)
            {
                return attr_wrapper_type(*this, uIndex);
            }


        private:
            struct CheckValidNode
            {
                int iLeftDstEdge; /** ʣ�ൽ�ﵱǰ�ڵ�ı��� **/
                std::list<int> stSrcList; /** �Ե�ǰ�ڵ�Ϊ���ı��б�(�ڽӱ�) **/
                CheckValidNode(): iLeftDstEdge(0){}
            };

            /**
             * ��鹫ʽ�Ƿ�ɻ�
             * @note �������ͼ���Ƿ���ڻ�
             * @note ��Ҫ�㷨����������, ����ʱ�临�Ӷ�[O(n+2m)], �����ռ临�Ӷ�[O(2n+m)]
             * @note { nΪ���й�ʽ���漰������ID�ܸ���, mΪ���ԵĻ�ֱ��Ӱ���Ŀ�������ܸ��� }
             * @param [in, out] stRelationMap ��ϵ�ڵ㻺����(������ڻ�, �������еı��б�������������ӵıߺͽڵ�)
             * @return �����ڻ�����true
             */
            static bool CheckFormulaNoLoop(CheckValidNode stRelationMap[_MAXCOUNT])
            {
                std::list<int> stWaitForReach;

                // Step 1. ��ͼ, ʱ�临�Ӷ�[O(m)], �ռ临�Ӷ�[O(n + m)]
                attr_attach_type& stAttachedMap = GetAttachAttrMap();
                for(typename attr_attach_type::iterator iter = stAttachedMap.begin();
                    iter != stAttachedMap.end();
                    ++ iter)
                {
                        attr_attach_list_type& stAttachList = iter->second;
                        int iSrcIndex = static_cast<int>(iter->first);

                        for (typename attr_attach_list_type::iterator itAttr = stAttachList.begin();
                            itAttr != stAttachList.end();
                            ++ itAttr
                            )
                        {
                            int iDstIndex = static_cast<int>(*itAttr);
                            ++ stRelationMap[iDstIndex].iLeftDstEdge;
                            stRelationMap[iSrcIndex].stSrcList.push_back(iDstIndex);
                        }
                }

                // Step 2. ��ʼ����������ڵ�, �ɵ���ڵ������������, ʱ�临�Ӷ�[O(n)], �ռ临�Ӷ�[O(n)]
                int iLeftUnreached = _MAXCOUNT;
                for (int i = 0; i < _MAXCOUNT; ++ i)
                {
                    if (stRelationMap[i].iLeftDstEdge <= 0)
                    {
                        stWaitForReach.push_back(i);
                    }
                }

                // Step 3. ��������, ʱ�临�Ӷ�[O(m)], �ռ临�Ӷ�[O(n)][�����湲�þ�������]
                while(false == stWaitForReach.empty())
                {
                    -- iLeftUnreached;
                    int iSrcIndex = stWaitForReach.front();
                    stWaitForReach.pop_front();

                    std::list<int>& stEdgeList =  stRelationMap[iSrcIndex].stSrcList;
                    while(!stEdgeList.empty())
                    {
                        int iDstIndex = stEdgeList.front();
                        stEdgeList.pop_front();

                        -- stRelationMap[iDstIndex].iLeftDstEdge;
                        if (stRelationMap[iDstIndex].iLeftDstEdge == 0)
                        {
                            stWaitForReach.push_back(iDstIndex);
                        }
                    }
                }

                // Step 4. ������нڵ���ѵ�����޻�
                return iLeftUnreached <= 0;
            }

            // ��ȡѭ����ϵ��--���������������
            static void GetInvalidLoopsDFS(CheckValidNode stRelationMap[_MAXCOUNT], int iPos, std::vector<bool>& stReachedFlags, std::vector<bool>& stIsInRoute, std::vector<int>& stRoute, formula_loops_type& stResult)
            {
                // ����Ѿ����������ǰ�ڵ�Ļ�����������
                if(stReachedFlags[iPos])
                {
                    return;
                }

                // ����ѭ����
                if (stIsInRoute[iPos])
                {
                    // ��������¼
                    stResult.push_back(std::list<int>());
                    stResult.back().assign(
                        std::find(stRoute.begin(), stRoute.end(), iPos),
                        stRoute.end()
                    );

                    return;
                }

                stRoute.push_back(iPos); // ��ջ
                stIsInRoute[iPos] = true;
                for (std::list<int>::iterator iter = stRelationMap[iPos].stSrcList.begin();
                    iter != stRelationMap[iPos].stSrcList.end();
                    ++ iter)
                {
                    GetInvalidLoopsDFS(stRelationMap, *iter, stReachedFlags, stIsInRoute, stRoute, stResult);
                }
                stIsInRoute[iPos] = false;
                stRoute.pop_back(); // ��ջ

                // ���Ϊ�Ѿ�������ǰ�ڵ�
                stReachedFlags[iPos] = true;
            }

        public:
            /**
             * ����ϵ���Ƿ�Ϸ�(���ܳ��� ����ͼ ��ϵ)
             * @return ��ϵ���Ϸ�����true
             */
            static bool CheckValid()
            {
                // ��鹫ʽ��ȷ�Ļ���
                static bool bCheckRes = false;
                if (bCheckRes)
                {
                    return bCheckRes;
                }

                // �������ͼ���Ƿ���ڻ�
                CheckValidNode stRelationMap[_MAXCOUNT];
                return bCheckRes = CheckFormulaNoLoop(stRelationMap);
            }

            /**
             * ��ȡ���Ϸ��Ĺ�ϵ��
             * @note ��Ҫ�㷨��DFS { ʱ�临�Ӷ�O(mn), �ռ临�Ӷ�O(3n) }
             * @param [in, out] stRelationMap ��ϵ�ڵ㻺����
             * @param [out] stLoops ���еĻ��б�
             */
            static void GetInvalidLoops(CheckValidNode stRelationMap[_MAXCOUNT], formula_loops_type& stLoops)
            {
                stLoops.clear();
                bool bCheckRes = CheckFormulaNoLoop(stRelationMap);
                if (bCheckRes)
                {
                    return;
                }

                int iStartIndex = 0;
                std::vector<bool> stReachedFlags, stIsInRoute;
                stReachedFlags.assign(_MAXCOUNT, false);
                stIsInRoute.assign(_MAXCOUNT, false);
                std::vector<int> stRoute;

                // ö�����
                for (; iStartIndex < _MAXCOUNT; ++ iStartIndex )
                {
                    // �ѱ������������
                    if (stReachedFlags[iStartIndex])
                    {
                        continue;
                    }

                    // �޺����ڵ㣬����
                    if (stRelationMap[iStartIndex].stSrcList.size() <= 0)
                    {
                        continue;
                    }

                    // ��������������������еĻ�
                    stRoute.clear();
                    GetInvalidLoopsDFS(stRelationMap, iStartIndex, stReachedFlags, stIsInRoute, stRoute, stLoops);
                }

                
            }

            /**
             * ������Ϸ��Ĺ�ϵ���͹�ʽѭ���������
             * @param [in, out] ostream ���Ŀ��
             */
            template<typename _TOStream>
            static void PrintInvalidLoops(_TOStream& ostream)
            {
                formula_loops_type stLoops;
                CheckValidNode stRelationMap[_MAXCOUNT];
                GetInvalidLoops(stRelationMap, stLoops);
                ostream<< "All node links:\n";

                // ��ӡ��ϵͼ��
                for (int i = 0; i < _MAXCOUNT; ++ i)
                {
                    if (stRelationMap[i].iLeftDstEdge <= 0)
                    {
                        continue;
                    }

                    ostream<< "\t"<< i<< " -> ";
                    for (std::list<int>::iterator iter = stRelationMap[i].stSrcList.begin();
                        iter != stRelationMap[i].stSrcList.end();
                        ++ iter)
                    {
                        ostream<< " "<< *iter;
                    }
                    ostream<< "\n";
                }

                // ��ӡ���еĹ�ʽ����ѭ��
                ostream<< "All formula loops:\n";
                for (typename formula_loops_type::iterator iter = stLoops.begin();
                    iter != stLoops.end();
                    ++ iter)
                {
                    // �Թ����б�
                    if (iter->empty())
                    {
                        continue;
                    }

                    ostream<< "\t"<< *(iter->begin());
                    for (std::list<int>::reverse_iterator riter = iter->rbegin();
                        riter != iter->rend();
                        ++ riter)
                    {
                        ostream<<" -> "<< *riter;
                    }
                    ostream<< "\n";
                }
            }

        };


        // ==================== ���������� ====================
        namespace Operator
        {
            // ==================== ����������: ���ʽ -- ���ʽ ====================
            <?php gen_all_operator('base', 'base'); ?>

            // ==================== ����������: ���ʽ -- ���� ====================
            <?php gen_all_operator('base', 'value'); ?>

            // ==================== ����������: ���� -- ���ʽ ====================
            <?php gen_all_operator('value', 'base'); ?>

            // ==================== ����������: ���� -- ���� ====================
            <?php gen_all_operator('attr_wrapper', 'attr_wrapper'); ?>

            // ==================== ����������: ���� -- ���� ====================
            <?php gen_all_operator('attr_wrapper', 'value'); ?>

            // ==================== ����������: ���� -- ���� ====================
            <?php gen_all_operator('value', 'attr_wrapper'); ?>

            // ==================== ����������: ���� -- ���ʽ ====================
            <?php gen_all_operator('attr_wrapper', 'base'); ?>

            // ==================== ����������: ���ʽ -- ���� ====================
            <?php gen_all_operator('base', 'attr_wrapper'); ?>
        }
    }
}

#endif
