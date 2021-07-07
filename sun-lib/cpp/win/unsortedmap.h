/* unsortedmap.h
   2013-5-26
   Version: 0.1

Copyright (c) 2012- SUN Junwen

�� std::list<std::pair<_Key, _Tp> > �ķ�װ
Ŭ��ʵ�� std::map �����ԣ����ṩ FIFO ������
����Ԫ�ص����� ���� ɾ�� �������� O(n) ��
ʱ�临�Ӷ�
���ܻ�����һ�� std::list 
������������ std::map
*/
#ifndef _UNSORTED_MAP_H_
#define _UNSORTED_MAP_H_

#include <list>
#include <map>

template<typename _Key, typename _Tp>
class UnsortedMap
{
public:
	typedef _Key									key_type;
	typedef _Tp										mapped_type;
	typedef std::pair<_Key, _Tp>					pair_type;
	typedef std::list<pair_type>					unsorted_map;
	typedef typename unsorted_map::iterator			iterator;
	typedef typename unsorted_map::const_iterator	const_iterator;
	typedef typename unsorted_map::size_type		size_type;

	UnsortedMap()
	{}
	virtual ~UnsortedMap()
	{}

	// �����ӿڶ��Ƕ� std::list �ķ�װ
	inline size_type size()
	{ return m_list.size(); }

	void push_front(const pair_type& pair)
	{ 
		erase(pair.first);
		m_list.push_front(pair); 
	}

	void push_back(const pair_type& pair)
	{
		erase(pair.first);
		m_list.push_back(pair); 
	}

	iterator insert(iterator itr, const pair_type& pair)
	{
		erase(pair.first);
		return m_list.insert(itr, pair); 
	}

	iterator erase(const key_type& key)
	{
		typename unsorted_map::iterator itr = find(key);
		if(itr != end())
			itr = erase(itr);

		return itr;
	}

	inline iterator erase(iterator itr)
	{ return m_list.erase(itr); }
	inline void clear()
	{ m_list.clear(); }

	inline const_iterator begin() const
	{ return m_list.begin(); }
	inline iterator begin()
	{ return m_list.begin(); }

	inline const_iterator end() const
	{ return m_list.end(); }
	inline iterator end()
	{ return m_list.end(); }

	iterator find(const key_type& key)
	{
        typename unsorted_map::iterator itr = m_list.begin();
        for(; itr != m_list.end(); ++itr)
        {
            if(key == itr->first)
                return itr;
        }
        
        return m_list.end();
	}

	// ֻ���ṩ O(n) ������
	mapped_type& operator[](const key_type& key)
	{
		typename unsorted_map::iterator itr = find(key);
		if(itr == m_list.end())
		{
			// û��, ����һ��
			m_list.push_back(pair_type(key, mapped_type()));
			itr = m_list.end();
			--itr;
		}
		return (*itr).second;
	}

private:
	unsorted_map m_list;
};

#endif
