
#include "UGXml.h"

namespace UG{
	namespace xml{

		bool UGXml::LoadXml(const std::string& sXmlPath)
		{
			TiXmlDocument doc(sXmlPath.c_str());
			doc.LoadFile(TIXML_ENCODING_UTF8);

			bool bRet = false;
			TiXmlElement* root = doc.RootElement();
			if (root)
			{
				m_xmlCleanItem.iLanID = 0;
				m_xmlCleanItem.iTipID = 0;
				m_xmlCleanItem.iCheckStatus = 0;
				m_xmlCleanItem.iLayer = -1;
				int iLayer = 0;
				bRet = Parse(iLayer, nullptr, root->FirstChildElement(), m_xmlCleanItem);
			}

			return bRet;
		}

		bool UGXml::Parse(int iLayer, void* pParentNode, TiXmlNode* pNode, XMLCleanItem &cleanItem)
		{
			bool bNode = false;

			if (pNode != NULL)
			{
				int iType = pNode->Type();
				if (iType == TiXmlNode::TINYXML_ELEMENT)
				{
					int i = 0;
					TiXmlNode* node = pNode;
					while (node)
					{
						TiXmlAttribute* pAttr = node->ToElement()->FirstAttribute();

						std::string sTag = std::string(node->Value());
						{
							std::vector<XMLData> temData;

							if (pAttr)
							{
								while (pAttr)
								{
									XMLData nodeData;
									nodeData.sKey = std::string(pAttr->Name());
									nodeData.sValue = std::string(pAttr->Value());

									temData.push_back(nodeData);

									pAttr = pAttr->Next();
								}
							}

							void* pNode = nullptr;
							if (BuilderControls(iLayer, pParentNode, sTag, temData, cleanItem, pNode))
							{
								if (!Parse(iLayer + 1, pNode, node->FirstChildElement(), cleanItem.vecCleanItem[i]))
									return false;
							}
							else
							{
								return false;
							}
						}

						node = node->NextSiblingElement();
						++i;
					}

				}
				else if (iType == TiXmlNode::TINYXML_TEXT)
				{
					std::cout << pNode->Value() << std::endl;
				}
			}

			return true;
		}


		bool UGReadXml::LoadXml(const std::string& sXmlPath)
		{
			TiXmlDocument doc(sXmlPath.c_str());
			doc.LoadFile(TIXML_ENCODING_UTF8);

			bool bRet = false;
			TiXmlElement* root = doc.RootElement();
			if (root)
			{
				int iLayer = 0;
				bRet = Parse(iLayer, nullptr, root);
			}

			return bRet;
		}

		bool UGReadXml::Parse(int iLayer, void* pParentNode, TiXmlNode* pNode)
		{
			bool bNode = false;

			if (pNode != NULL)
			{
				int iType = pNode->Type();
				if (iType == TiXmlNode::TINYXML_ELEMENT)
				{
					TiXmlNode* node = pNode;
					while (node)
					{
						TiXmlAttribute* pAttr = node->ToElement()->FirstAttribute();

						std::string sTag = std::string(node->Value());
						{
							std::vector<XMLData> temData;

							if (pAttr)
							{
								while (pAttr)
								{
									XMLData nodeData;
									nodeData.sKey = std::string(pAttr->Name());
									nodeData.sValue = std::string(pAttr->Value());

									temData.push_back(nodeData);

									pAttr = pAttr->Next();
								}
							}

							void* pNode = nullptr;
							if (BuilderControls(iLayer, pParentNode, sTag, temData, pNode))
							{
								if (!Parse(iLayer + 1, pNode, node->FirstChild()))
									return false;
							}
							else
							{
								return false;
							}

							NodeFinish(sTag);
						}

						node = node->NextSiblingElement();
					}

				}
				else if (iType == TiXmlNode::TINYXML_TEXT)
				{
					std::cout << pNode->Value() << std::endl;
				}
			}

			return true;
		}
	}
}