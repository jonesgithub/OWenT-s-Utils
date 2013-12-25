/**
 * @file TQueryString.h
 * @brief ֧��Ƕ�׽ṹ��Web Querystring�࣬���ɺͽ�����ʽ����PHP <br />
 *        ��������ָ��� <br />
 *        ע: ���������ַ�����ʱ�򣬳����*[]=...������з��Ͻṹ��ȫ������ΪObject���� <br />
 *            ���ݺ�key�в��������[��]
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT
 * @date 2012.08.01
 *
 * @history
 *
 */

#ifndef _UTIL_URI_TQUERYSTRING_H_
#define _UTIL_URI_TQUERYSTRING_H_

#include <cstddef>
#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "std/smart_ptr.h"

namespace util
{
    namespace uri
    {
        /**
         * @brief ����URI������Javascript��encodeURI����
         * @param [in] strContent ����������ָ��
         * @param [in] uSize      �������ݴ�С��Ĭ�ϵ����ַ�����
         * @return �������ַ���
         */
        std::string EncodeUri(const char* strContent, std::size_t uSize = 0);

        /**
         * @brief ����URI������Javascript��decodeURI����
         * @param [in] strUri     ����������ָ��
         * @param [in] uSize      �������ݴ�С��Ĭ�ϵ����ַ�����
         * @return �������ַ���
         */
        std::string DecodeUri(const char* strUri, std::size_t uSize = 0);

        /**
         * @brief ���벢ת��URI������Javascript��decodeURIComponent����
         * @param [in] strContent ����������ָ��
         * @param [in] uSize      �������ݴ�С��Ĭ�ϵ����ַ�����
         * @return �������ַ���
         */
        std::string EncodeUriComponent(const char* strContent, std::size_t uSize = 0);

        /**
         * @brief ����ת���URI������Javascript��encodeURIComponent����
         * @param [in] strUri     ����������ָ��
         * @param [in] uSize      �������ݴ�С��Ĭ�ϵ����ַ�����
         * @return �������ַ���
         */
        std::string DecodeUriComponent(const char* strUri, std::size_t uSize = 0);

        /**
         * @brief �ַ���ת��Ϊ��������
         * @param [in] str     �ַ�����ʾ����������
         * @return ��������
         */
        template<typename T>
        T QueryStringToAny(const char* str)
        {
            T ret;
            std::stringstream ss;
            ss << str;
            ss >> ret;
            return ret;
        }

        /**
         * @brief ��������ת��Ϊ�ַ���
         * @param [in] val     ������������
         * @return ��Ӧ���ַ���
         */
        template<typename T>
        std::string AnyToQueryString(const T& val)
        {
            std::stringstream ss;
            ss << val;
            return ss.str();
        }
    }


    namespace types
    {
        /**
         * @brief ��������ö�٣�С��ITEM_TYPE_QUERYSTRING��ΪԪ����
         */
        enum ITEM_TYPE
        {
            ITEM_TYPE_STRING        = 0,
            ITEM_TYPE_QUERYSTRING   = 1,
            ITEM_TYPE_ARRAY         = 2,
            ITEM_TYPE_OBJECT        = 3
        };

        /**
         * @brief �������ͳ���ӿ�
         */
        class ItemImpl
        {
        protected:
            ItemImpl(){}

            /**
             * @brief ��ӵ��ַ���
             * @param [in] strTar   ���Ŀ��
             * @param [in] key      keyֵ
             * @param [in] value    valueֵ
             */
            void appendTo(std::string& strTar, const std::string& key, const std::string& value) const;

        public:
            /**
             * @brief ��Ӧ�������͵�����ָ������
             */
            typedef std::shared_ptr<ItemImpl> ptr_type;

            virtual ~ItemImpl(){}

            /**
             * @brief ��ȡ��������
             * @return ���ݳ���
             */
            virtual std::size_t GetSize() const = 0;

            /**
             * @brief ��ȡ��������
             * @return ��ʾ���͵�ö������
             */
            virtual ITEM_TYPE GetType() const = 0;

            /**
             * @brief ������תΪ�ַ���
             * @param [in] strPrefix ������ǰ׺
             * @return �ַ�����ʾ
             */
            virtual std::string ToString(const char* strPrefix = "") const = 0;

            /**
             * @brief ���벢׷��Ŀ���ַ���β��
             * @param [out] strOutput �������
             * @param [in] strPrefix ��������ǰ׺
             * @return ����ɹ�������true�����򷵻�false
             */
            virtual bool Encode(std::string& strOutput, const char* strPrefix = "") const = 0;

            /**
             * @brief ���ݽ���
             * @param [in] stKeys   key�б�
             * @param [in] index    ��ǰ����λ���±�
             * @param [in] strValue Ŀ��ֵ
             * @return ����ɹ�������true�����򷵻�false
             */
            virtual bool parse(const std::vector<std::string>& stKeys, std::size_t index, const std::string& strValue) = 0;
        };

        /**
         * @brief ��ͨ�ַ�������
         */
        class ItemString: public ItemImpl
        {
        protected:
            std::string m_strData;

        public:
            /**
             * @brief ��Ӧ�������͵�����ָ������
             */
            typedef std::shared_ptr<ItemString> ptr_type;

            ItemString();
            
            ItemString(const std::string& strData);

            virtual ~ItemString();

            /**
             * @brief �����������͵�ʵ��
             * @return ��ʵ��������ָ��
             */
            static inline ptr_type Create() { return std::shared_ptr<ItemString>(new ItemString()); }

            /**
             * @brief �����������͵�ʵ��
             * @param [in] strData ��ʼ����
             * @return ��ʵ��������ָ��
             */
            static inline ptr_type Create(const std::string& strData) { return std::shared_ptr<ItemString>(new ItemString(strData)); }

            virtual std::size_t GetSize() const;

            virtual types::ITEM_TYPE GetType() const;

            virtual std::string ToString(const char* strPrefix = "") const;

            virtual bool Encode(std::string& strOutput, const char* strPrefix = "") const;

            virtual bool parse(const std::vector<std::string>& stKeys, std::size_t index, const std::string& strValue);

            /**
             * @breif ����ת������
             */
            inline operator std::string() { return Get(); };

            /**
             * @breif ���ݸ�ֵ����
             * @param [in] strData ԭʼ����
             * @return ���ݵȺŲ��������򷵻���������
             */
            inline ItemString& operator=(const std::string& strData) { Set(strData); return (*this); };

            /**
             * @breif ��������
             * @param [in] strData ԭʼ����
             */
            inline void Set(const std::string& strData) { m_strData = strData; };

            /**
             * @breif ��ȡ����
             * @return ��������
             */
            inline std::string& Get() { return m_strData; };
        };

        /**
         * @brief ��������
         */
        class ItemArray: public ItemImpl
        {
        protected:
            std::vector<std::shared_ptr<ItemImpl> > m_stData;

        public:
            /**
             * @brief ��Ӧ�������͵�����ָ������
             */
            typedef std::shared_ptr<ItemArray> ptr_type;

            ItemArray();

            virtual ~ItemArray();

            /**
             * @brief �����������͵�ʵ��
             * @param [in] strData ��ʼ����
             * @return ��ʵ��������ָ��
             */
            static inline ptr_type Create() { return std::shared_ptr<ItemArray>(new ItemArray()); }

            virtual std::size_t GetSize() const;

            virtual types::ITEM_TYPE GetType() const;

            virtual std::string ToString(const char* strPrefix = "") const;

            virtual bool Encode(std::string& strOutput, const char* strPrefix = "") const;

            virtual bool parse(const std::vector<std::string>& stKeys, std::size_t index, const std::string& strValue);

            /**
             * @breif �����±��ȡ����
             * @param [in] uIndex �±�
             * @return �������ݵ�����ָ��
             */
            inline std::shared_ptr<ItemImpl> Get(std::size_t uIndex) { return m_stData[uIndex]; };

            /**
             * @breif �����±��ȡ���ݵ��ַ���ֵ
             * @param [in] uIndex �±�
             * @return �������ݵ��ַ�����ʾ
             */
            inline std::string GetString(std::size_t uIndex) const { return m_stData[uIndex]->ToString(); };

            /**
             * @breif ����ֵ
             * @param [in] uIndex �±�
             * @param [in] value ֵ������ָ��
             */
            inline void Set(std::size_t uIndex, const std::shared_ptr<ItemImpl>& value) { m_stData[uIndex] = value; };

            /**
             * @breif �����ַ���ֵ
             * @param [in] uIndex �±�
             * @param [in] value �ַ���ֵ
             */
            inline void Set(std::size_t uIndex, const std::string& value) { m_stData[uIndex] = std::shared_ptr<ItemString>(new ItemString(value)); };

            /**
             * @breif ���ֵ
             * @param [in] uIndex �±�
             * @param [in] value ֵ������ָ��
             */
            inline void Append(const std::shared_ptr<ItemImpl>& value) { m_stData.push_back(value); };

            /**
             * @breif ����ַ���ֵ
             * @param [in] uIndex �±�
             * @param [in] value �ַ���ֵ
             */
            inline void Append(const std::string& value) { m_stData.push_back(std::shared_ptr<ItemString>(new ItemString(value))); };

            /**
             * @breif ������һ������
             */
            inline void PopBack() { m_stData.pop_back(); };

            /**
             * @breif �������
             */
            inline void Clear() { m_stData.clear(); };
        };

        /**
         * @brief ӳ������
         */
        class ItemObject: public ItemImpl
        {
        protected:
            std::map<std::string, std::shared_ptr<ItemImpl> > m_stData;
            typedef std::map<std::string, std::shared_ptr<ItemImpl> >::iterator data_iterator;
            typedef std::map<std::string, std::shared_ptr<ItemImpl> >::const_iterator data_const_iterator;

        public:
            /**
             * @brief ��Ӧ�������͵�����ָ������
             */
            typedef std::shared_ptr<ItemObject> ptr_type;

            /**
             * @brief �����������͵�ʵ��
             * @param [in] strData ��ʼ����
             * @return ��ʵ��������ָ��
             */
            static inline ptr_type Create() { return std::shared_ptr<ItemObject>(new ItemObject()); }

            ItemObject();

            virtual ~ItemObject();

            virtual std::size_t GetSize() const;

            virtual types::ITEM_TYPE GetType() const;

            virtual std::string ToString(const char* strPrefix = "") const;

            virtual bool Encode(std::string& strOutput, const char* strPrefix = "") const;

            virtual bool parse(const std::vector<std::string>& stKeys, std::size_t index, const std::string& strValue);

            std::vector<std::string> GetKeys() const;

            /**
             * @breif ����Key��ȡ����
             * @param [in] key Key
             * @return �������ݵ�����ָ��
             */
            inline std::shared_ptr<ItemImpl> Get(const std::string& key) 
            { 
                data_iterator iter = m_stData.find(key);
                return iter == m_stData.end()? std::shared_ptr<ItemImpl>(): iter->second; 
            };

            /**
             * @breif ����Key��ȡ���ݵ��ַ���ֵ
             * @param [in] key Key
             * @return �������ݵ��ַ�����ʾ
             */
            inline std::string GetString(const std::string& key) const 
            { 
                data_const_iterator iter = m_stData.find(key);
                return iter == m_stData.end()? "": iter->second->ToString(); 
            };

            /**
             * @breif ��ӻ�����ֵ
             * @param [in] key Key
             * @param [in] value ֵ������ָ��
             */
            inline void Set(const std::string& key, const std::shared_ptr<ItemImpl>& value) 
            {
                data_iterator iter = m_stData.find(key);
                if (iter == m_stData.end())
                {
                    m_stData.insert(std::make_pair(key, value));
                }
                else
                {
                    iter->second = value;
                }
            };

            /**
             * @breif ��ӻ������ַ���ֵ
             * @param [in] key Key
             * @param [in] value �ַ���ֵ
             */
            inline void Set(const std::string& key, const std::string& value) { Set(key, std::shared_ptr<ItemString>(new ItemString(value))); };

            /**
             * @breif ɾ������
             * @param [in] key Key
             */
            inline void Del(const std::string& key) { m_stData.erase(key); };

            /**
             * @breif �������
             */
            inline void Clear() { m_stData.clear(); };
        };
    }

    class TQueryString: public types::ItemObject
    {
    protected:
        typedef std::map<std::string, std::shared_ptr<types::ItemImpl> >::iterator data_iterator;
        typedef std::map<std::string, std::shared_ptr<types::ItemImpl> >::const_iterator data_const_iterator;

        std::string m_strSpliter;

        bool decodeRecord(const char* strContent, std::size_t uSize);

    public:
        /**
         * @brief ��Ӧ�������͵�����ָ������
         */
        typedef std::shared_ptr<TQueryString> ptr_type;

        TQueryString();

        TQueryString(const std::string& strSpliter);

        virtual ~TQueryString();

        /**
         * @brief �����������͵�ʵ��
         * @return ��ʵ��������ָ��
         */
        static inline ptr_type Create() { return std::shared_ptr<TQueryString>(new TQueryString()); }

        /**
         * @brief �����������͵�ʵ��
         * @param [in] strSpliter Ĭ�Ϸָ���
         * @return ��ʵ��������ָ��
         */
        static inline ptr_type Create(const std::string& strSpliter) { return std::shared_ptr<TQueryString>(new TQueryString(strSpliter)); }

        virtual std::size_t GetSize() const;
        
        virtual types::ITEM_TYPE GetType() const;

        virtual std::string ToString(const char* strPrefix = "") const;

        virtual bool Encode(std::string& strOutput, const char* strPrefix = "") const;

        /**
         * @breif ��������
         * @param [in] strContent ����ָ��
         * @param [in] uSize      ���ݳ���
         * @return �ɹ�����true
         */
        bool Decode(const char* strContent, std::size_t uSize = 0);

        /**
         * @breif ����ID��ȡ����
         * @param [in] key Key
         * @return ���ڷ��ض�Ӧ������ָ�룬���򷵻ؿ�����ָ��
         */
        std::shared_ptr<types::ItemImpl> operator[](const std::string& key);

        /**
         * @breif �������ݷָ���
         * @param [in] strSpliter �ָ����ÿ���ַ����ǵ����ķָ���
         */
        inline void SetSpliter(const std::string& strSpliter) { m_strSpliter = strSpliter; };

        /**
         * @breif �����ַ���ʵ��
         * @return ��ʵ��ָ��
         */
        static inline types::ItemString::ptr_type CreateString() { return types::ItemString::Create(); };

        /**
         * @breif �����ַ���ʵ��
         * @param [in] val ��ʼֵ
         * @return ��ʵ��ָ��
         */
        static inline types::ItemString::ptr_type CreateString(const std::string& val) { return types::ItemString::Create(val); };

        /**
         * @breif ��������ʵ��
         * @return ��ʵ��ָ��
         */
        static inline types::ItemArray::ptr_type CreateArray() { return types::ItemArray::Create(); };

        /**
         * @breif ����Objectʵ��
         * @return ��ʵ��ָ��
         */
        static inline types::ItemObject::ptr_type CreateObject() { return types::ItemObject::Create(); };
    };

}

#endif
