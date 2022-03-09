
#pragma once

#include "../Ass.h"
#include "tinyxml.h"

namespace UG{
	namespace xml{

		struct XMLData
		{
			std::string sKey;
			std::string sValue;
		};

		struct XMLCleanItem
		{
			XMLCleanItem* FindItemBylanID(int iFindLanID)
			{
				if (iFindLanID == iLanID) return this;

				for (auto &it : vecCleanItem)
				{
					XMLCleanItem *pItem = it.FindItemBylanID(iFindLanID);
					if (pItem != NULL) return pItem;
				}

				return NULL;
			}

			void SetCheckStatus(int iSetCheckStatus)
			{
				if (iCheckStatus == iSetCheckStatus) return;

				iCheckStatus = iSetCheckStatus;
				for (auto &it : vecCleanItem)
				{
					it.SetCheckStatus(iSetCheckStatus);
				}
			}

			void UpdateCheckStatus()
			{
				if (vecCleanItem.empty()) return;

				int nCheckCount = 0;
				bool bCheckLittle = false;
				for (auto &it : vecCleanItem)
				{
					it.UpdateCheckStatus();
					if (it.iCheckStatus == 2)
						bCheckLittle = true;
					else if (it.iCheckStatus == 1)
						++nCheckCount;
				}

				if (bCheckLittle)
					iCheckStatus = 2;
				else if (nCheckCount == (int)vecCleanItem.size())
					iCheckStatus = 1;
				else if (nCheckCount == 0)
					iCheckStatus = 0;
				else
					iCheckStatus = 2;
			}

			unsigned long long GetTotalFileSize()
			{
				if (vecCleanItem.empty()) return ullSize;

				ullSize = 0;
				for (auto &it : vecCleanItem)
				{
					unsigned long long ullSubSize = it.GetTotalFileSize();

					if (it.iLayer == 0 && it.bReg) continue;
					ullSize += ullSubSize;
				}

				return ullSize;
			}

			unsigned long long GetTotalCheckFileSize()
			{
				if (vecCleanItem.empty())
				{
					ullCheckSize = iCheckStatus == 0 ? 0 : ullSize;
					return ullCheckSize;
				}

				ullCheckSize = 0;
				for (auto &it : vecCleanItem)
				{
					if (it.iLayer == 0 && it.bReg)
					{
						it.GetTotalCheckFileSize();
						continue;
					}

					unsigned long long ullSubSize = it.GetTotalCheckFileSize();
					if (it.iCheckStatus != 0)
						ullCheckSize += ullSubSize;
				}

				return ullCheckSize;
			}

			int iLayer;			//几层
			int iLanID;			//语言ID
			int iTipID;			//TipId
			int iCheckStatus;	//选择状态
			bool bReg;			//是否注册表
			unsigned long long ullSize;				//总大小
			unsigned long long ullCheckSize;		//所有选择中大小
			std::string strIcon;	//ICON名字
			std::vector<XMLCleanItem> vecCleanItem; //子集
		};

		class APDDLL_DECLSPEC UGXml
		{
		public:
			virtual ~UGXml(){}
			bool LoadXml(const std::string& sXmlPath);
		private:
			bool Parse(int iLayer, void* pParentNode, TiXmlNode* pNode, XMLCleanItem &cleanItem);
			virtual bool BuilderControls(int iLayer, void* pParent, const std::string& sTag, std::vector<XMLData>& vData, XMLCleanItem &cleanItem, void*& ret) = 0;

		public:
			XMLCleanItem m_xmlCleanItem;
		};

		class APDDLL_DECLSPEC UGReadXml
		{
		public:
			virtual ~UGReadXml(){}

			virtual bool LoadXml(const std::string& sXmlPath);
		protected:
			bool Parse(int iLayer, void* pParentNode, TiXmlNode* pNode);
			virtual bool BuilderControls(int iLayer, void* pParent, const std::string& sTag, std::vector<XMLData>& vData, void*& ret) = 0;
			virtual void NodeFinish(const std::string& sTag){}

		public:
			XMLCleanItem m_xmlCleanItem;
		};

	}
}