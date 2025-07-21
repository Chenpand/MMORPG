/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	���ݱ��������
 */
#ifndef _AVALON_DATAENTRY_MGR_H_
#define _AVALON_DATAENTRY_MGR_H_

#include <sstream>
#include <AvalonSingleton.h>
#include <AvalonDataTable.h>
#include <list>

namespace Avalon
{
	class DataTable;

	/**
	 *@brief �����������
	 */
	template<typename DataEntryT>
	class DataEntryVisitor
	{
	public:
		virtual ~DataEntryVisitor(){}

		virtual bool operator()(DataEntryT* dataEntry) = 0;
	};

	/**
	 *@brief ���������������
	 */
	template<typename DataEntryT>
	class DataEntryMgrBase
	{
		typedef HashMap<UInt32, DataEntryT*> DataEntryMap;

	public:
		DataEntryMgrBase(){}
		virtual ~DataEntryMgrBase()
		{
			ClearDataEntries();
		}

	public:
		/**
		 *@brief ��������
		 */
		bool LoadData(const std::string& filename)
		{
			DataTable table;
			if (!table.LoadFile(filename))
			{
				std::stringstream ss;
				ss << "load table(" << filename << ") failed.";
				throw ss.str();
				return false;
			}

			DataRow row;
			while (table.GetNextRow(row))
			{
				DataEntryT* entry = new DataEntryT();
				if (!entry->Read(row)
					|| !AddEntry(entry))
				{
					std::stringstream ss;
					ss << "read table(" << filename << ") key(" << entry->GetKey() << ") failed.";
					delete entry;
					throw ss.str();
					return false;
				}
			}
			return true;
		}

		/**
		*@brief ���¼�������
		*/
		virtual bool ReloadData(const std::string& filename)
		{
			DataTable table;
			if (!table.LoadFile(filename))
			{
				std::stringstream ss;
				ss << "reload table(" << filename << ") failed.";
				throw ss.str();
				return false;
			}

			DataRow row;
			DataEntryMap tmpDataEntries;
			while (table.GetNextRow(row))
			{
				DataEntryT* entry = new DataEntryT();
				if (!entry->Read(row)
					|| !tmpDataEntries.insert(std::make_pair(entry->GetKey(), entry)).second)
				{
					std::stringstream ss;
					ss << "reload read table(" << filename << ") key(" << entry->GetKey() << ") failed.";
					delete entry;
					throw ss.str();
					return false;
				}
			}

			ClearDataEntries();
			m_DataEntries = tmpDataEntries;
			OnReload();
			return true;
		}

		/**
		 *@brief ����һ������
		 */
		DataEntryT* FindEntry(UInt32 key) const
		{
			typename DataEntryMap::const_iterator iter = m_DataEntries.find(key);
			if(iter != m_DataEntries.end())
			{
				return iter->second;
			}
			return NULL;
		}

		/**
		 *@brief ����������
		 */
		void Visit(DataEntryVisitor<DataEntryT>& visitor)
		{
			for(typename DataEntryMap::iterator iter = m_DataEntries.begin();
				iter != m_DataEntries.end(); ++iter)
			{
				if(!visitor(iter->second)) return;
			}
		}

	public:

		/**
		*@brief ���¼��������¼�
		*/
		virtual void OnReload() { }

	protected:
		/**
		 *@brief ������������б���
		 */
		virtual bool AddEntry(DataEntryT* entry)
		{
			return m_DataEntries.insert(std::make_pair(entry->GetKey(),entry)).second;
		}

	private:

		/**
		*@brief ���������
		*/
		void ClearDataEntries()
		{
			for (typename DataEntryMap::iterator iter = m_DataEntries.begin()
				; iter != m_DataEntries.end(); ++iter)
			{
				delete iter->second;
			}
			m_DataEntries.clear();
		}

	private:
		//������map
		DataEntryMap	m_DataEntries;
	};

	/**
	 *@brief ���������������
	 */
	template<typename DataEntryT>
	class DataEntryMgr : public Avalon::Singleton<DataEntryMgr<DataEntryT> >, public DataEntryMgrBase<DataEntryT>
	{
	public:
	};

	/**
	*@brief ��������������࣬LISTʵ��
	*/
	template<typename DataEntryT>
	class DataEntryListMgrBase
	{
		typedef std::list<DataEntryT*> DataEntryList;

	public:
		DataEntryListMgrBase(){ m_DataBackUp = NULL; }
		virtual ~DataEntryListMgrBase()
		{
			ClearDataEntries();
		}

	public:
		/**
		*@brief ��������
		*/
		bool LoadData(const std::string& filename)
		{
			DataTable table;
			if (!table.LoadFile(filename))
			{
				return false;
			}

			DataRow row;
			while (table.GetNextRow(row))
			{
				DataEntryT* entry = new DataEntryT();
				if (!entry->Read(row)
					|| !AddEntry(entry))
				{
					delete entry;
					return false;
				}
			}
			return true;
		}

		/**
		*@brief ���¼�������
		*/
		virtual bool ReloadData(const std::string& filename)
		{
			DataTable table;
			if (!table.LoadFile(filename))
			{
				return false;
			}

			DataRow row;

			DoBackUp();
			ClearDataEntries();
			while (table.GetNextRow(row))
			{
				DataEntryT* entry = new DataEntryT();
				if (!entry->Read(row)
					|| !AddEntry(entry))
				{
					DoRollBack();
					return false;
				}
			}

			DeleteOldData();
			OnReload();
			return true;
		}

		/**
		*@brief ����һ������
		*/
		DataEntryT* FindEntry(UInt32 key) const
		{
			typename DataEntryList::const_iterator iter = m_DataEntries.begin();
			while (iter != m_DataEntries.end())
			{
				DataEntryT* entry = *iter;
				if (entry && entry->GetKey() == key)
				{
					return entry;
				}
				
				++iter;
			}
			return NULL;
		}

		/**
		*@brief ����������
		*/
		void Visit(DataEntryVisitor<DataEntryT>& visitor)
		{
			for (typename DataEntryList::iterator iter = m_DataEntries.begin();
				iter != m_DataEntries.end(); ++iter)
			{
				if (!visitor(*iter)) return;
			}
		}

	public:

		/**
		*@brief ���¼��������¼�
		*/
		virtual void OnReload()
		{

		}

	protected:
		/**
		*@brief ������������б���
		*/
		virtual bool AddEntry(DataEntryT* entry)
		{
			m_DataEntries.push_back(entry);
			return true;
		}

	protected:

		/**
		*@brief ���������
		*/
		virtual void ClearDataEntries()
		{
			m_DataEntries.clear();
		}

		void DeleteOldData()
		{
			if (m_DataBackUp)
			{
				typename DataEntryList::iterator iter = m_DataBackUp->begin();
				while (iter != m_DataBackUp->end())
				{
					DataEntryT* entry = *iter;
					iter = m_DataBackUp->erase(iter);
					delete entry;
				}

				delete m_DataBackUp;
				m_DataBackUp = NULL;
			}
		}

		/**
		*@brief ��������ǰ���ֳ�
		*/
		virtual void DoBackUp()
		{
			m_DataBackUp = new DataEntryList();

			(*m_DataBackUp) = m_DataEntries;
		}

		/**
		*@brief �ع�����ǰ���ֳ�
		*/
		virtual void DoRollBack()
		{
			typename DataEntryList::iterator iter = m_DataEntries.begin();
			while (iter != m_DataEntries.end())
			{
				DataEntryT* entry = *iter;
				iter = m_DataEntries.erase(iter);
				delete entry;
			}

			if (m_DataBackUp)
			{
				m_DataEntries = *m_DataBackUp;
				delete m_DataBackUp;
				m_DataBackUp = NULL;
			}
		}

	private:
		//������list
		DataEntryList	m_DataEntries;
		//�������ݵı���
		DataEntryList*	m_DataBackUp;
	};

	/**
	*@brief ���������������
	*/
	template<typename DataEntryT>
	class DataEntryListMgr : public Avalon::Singleton<DataEntryListMgr<DataEntryT> >, public DataEntryListMgrBase<DataEntryT>
	{
	public:
	};
}

#endif
