/**
 *@author	 
 *@email	 
 *@date		2011-04-19		
 *@brief	���ݱ�
 */
#ifndef _AVALON_DATATABLE_H_
#define _AVALON_DATATABLE_H_

#include <AvalonDefine.h>
#include <fstream>
#include "AvalonDataRow.h"

namespace Avalon
{
	/**
	 *@brief ���ݱ�
	 */
	class DataTable
	{
	public:
		DataTable();
		~DataTable();

	public:
		/**
		 *@brief ����
		 */
		bool LoadFile(const std::string& filename);

		/**
		 *@brief ��ȡ��һ��
		 */
		bool GetNextRow(DataRow& row);

	private:
		//�ļ���
		std::fstream m_Stream;
	};
}

#endif
