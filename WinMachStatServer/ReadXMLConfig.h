#pragma once

#include <string>
#include <ole2.h>
#include <xmllite.h> 
#include <shlwapi.h>
#include <unordered_map>
#include <stack>

#include "Exception.h"
#include "Logger.h"

namespace WinMachStat {
	class ReadXMLConfig {
		std::wstring cfile;
		IXmlReader *pReader = NULL;
		IStream *pFileStream = NULL;

		HRESULT hr = S_OK;

#define MAX_ELEMENT 256
		typedef enum {
			_text,
			_element
		} vtype_t;

		struct Element;
		typedef std::vector<Element *> XMLNodes_t;
		typedef XMLNodes_t* pXMLNodes_t;
		typedef std::unordered_map<std::wstring, pXMLNodes_t> XMLElement_t;
		typedef XMLElement_t *pXMLElement_t;

		struct Element {
			std::wstring name;
			std::unordered_map<std::wstring, std::wstring> attr;

			class NodeValue {
			public:
				virtual vtype_t getValueType() = 0;
			};

			class NodeValueText : public NodeValue {
			public:
				std::wstring v;
				
				vtype_t getValueType() { return _text; }

				NodeValueText() {};
				~NodeValueText() {};
			} ;

			class NodeValueElement : public NodeValue {
			public:
				XMLElement_t v;

				vtype_t getValueType() { return _element;  }

				NodeValueElement() {};

				~NodeValueElement() {
				};
			};
						
			NodeValue *value;

			Element() {}

			~Element() {
				if(value) delete value;
			}
		};

		typedef Element XMLRoot_t;
		typedef XMLRoot_t* pXMLRoot_t;

		typedef Element::NodeValueText XMLTextNode_t;
		typedef Element::NodeValueElement XMLElementNode_t;

		typedef XMLTextNode_t* pXMLTextNode_t;
		typedef XMLElementNode_t* pXMLElementNode_t;


		pXMLRoot_t rootXML = NULL;

		void parseElements(pXMLRoot_t pXMLRoot) {
			XmlNodeType nodeType;
			const WCHAR* pQName;
			const WCHAR* pValue;
			Element *pElement = NULL;
			std::stack<pXMLRoot_t> stXMLRoot;

			bool nodeStart = false; 

			while (S_OK == (hr = pReader->Read(&nodeType)))
			{
				switch (nodeType)
				{
				case XmlNodeType_Element:
				{
					Element::NodeValueElement *pNVElement;
					if (pReader->IsEmptyElement())
					{
						continue;
					}
					else
					{
						if (FAILED(hr = pReader->GetQualifiedName(&pQName, NULL)))
						{
							LOG(WARNING) << "Error reading element name, error";
							return;
						}

						if (pXMLRoot) {
							//LOG(DEBUG) << "Pusing " << pXMLRoot->name << "Root to Stack" ;
							stXMLRoot.push(pXMLRoot);
							pXMLRoot = NULL;
						}

						if (pXMLRoot == NULL) {
							pXMLRoot = new XMLRoot_t;
							pXMLRoot->value = new XMLElementNode_t;
							pXMLRoot->name = pQName;
						}

						//LOG(DEBUG) << "Starting Sub/Root " << pQName << ":";
#if 0
						if ((*pXMLElement).find(pRootElement->name) == (*pXMLElement).end()) {
						}
						else {
						}
						(*pXMLElement).insert(std::make_pair(pRootElement->name, &pNVElement->v));
#endif
					}
				}

				break;
				case XmlNodeType_Text:
				{
					
					if (FAILED(hr = pReader->GetValue(&pValue, NULL)))
					{
						LOG(WARNING) << "Error reading value, error: " << hr;
						continue;
					}
					if (!pValue || L'\0' == (*pValue)) {
						LOG(WARNING) << "Error reading value name, error: E_UNEXPECTED";
						continue;
					}
					//LOG(DEBUG) << "Creating Text Value:" << pXMLRoot->name << "==>" << pQName << "=" << pValue << ":" ;

					pElement = new Element();
					pElement->name = pXMLRoot->name;

					pXMLTextNode_t pNodeValText;
					pNodeValText = new XMLTextNode_t;
					pNodeValText->v = std::wstring(pValue);
					pElement->value = pNodeValText;

					{
						pXMLElement_t pXMLElement = &(dynamic_cast<pXMLElementNode_t>(pXMLRoot->value))->v;

						if (pXMLElement != NULL) { // downcast successful
							if (pXMLElement->find(pXMLRoot->name) == pXMLElement->end()) {
								(*pXMLElement)[pXMLRoot->name] = new XMLNodes_t;
							}
							(*pXMLElement)[pXMLRoot->name]->push_back(pElement);
						}
					}
				}

				break;
				case XmlNodeType_EndElement:
				{
#if 0
					const WCHAR* pwszPrefix;
					const WCHAR* pwszLocalName;
					const WCHAR* pwszValue;
					UINT cwchPrefix;

					if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
					{
						LOG(DEBUG) << "Error getting prefix, error";
						continue;
					}
					if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
					{
						LOG(DEBUG) << "Error local name, error ";
						continue;
					}
					if (cwchPrefix > 0)
						LOG(DEBUG) << "End Element :" <<  pwszPrefix << ":" << pwszLocalName;
					else
						LOG(DEBUG) << "End Element :" << pwszLocalName;
#endif
					if (!stXMLRoot.empty()) {
						pXMLRoot_t pTmpXMLRoot = pXMLRoot;
						pXMLRoot = stXMLRoot.top(); stXMLRoot.pop();

						{
							pXMLElement_t pXMLElement = &((dynamic_cast<pXMLElementNode_t>(pXMLRoot->value))->v);
							if (pXMLElement != NULL) { // downcast successful
								if (pXMLElement->find(pXMLRoot->name) == pXMLElement->end()) {
									(*pXMLElement)[pXMLRoot->name] = new XMLNodes_t;
								}
								(*pXMLElement)[pXMLRoot->name]->push_back(pTmpXMLRoot);
							}
						}
					}
					else {
						LOG(DEBUG) << "Empty stack, I'm the root";
					}
				}

				break;
				default:
				{

				}
				break;
				}
			}
			rootXML = pXMLRoot;
			LOG(DEBUG) << "-==-=-=-=-=-=-=-==-=-=-=-=-=-=---=-=-=-==-=--=-=" ;
			printXML(rootXML);
		}

	public:

		/*void printXML(Element *node,int tabs=0) {
			if (node->value->getValueType() == _element) {
				Element::NodeValueElement *val = dynamic_cast<Element::NodeValueElement*>((*it_val)->value);
				for (auto it_valE = val->v.begin(); it_valE != val->v.end(); ++it_val) {
					printXML(*it_valE, tabs++);
				}
			}
			else if (node->value->getValueType() == _text) {
				Element::NodeValueText *val = dynamic_cast<Element::NodeValueText*>((*it_val)->value);
				LOG(DEBUG) << "Value: " << val->v;
			}
		}*/

		void printXML(pXMLRoot_t node,int tabs=0) {
			if (NULL== node) { return; }

			LOG(DEBUG) << std::string(tabs, '\t') << "Root: " << node->name;
			tabs++;
			if (node->value->getValueType() == _element) {
				pXMLElementNode_t val = dynamic_cast<pXMLElementNode_t>(node->value);
				for (auto it_elements = val->v.begin(); it_elements != val->v.end(); ++it_elements) {
					//LOG(DEBUG) << std::string(tabs,'\t') << "Root: " << it_elements->first;
					pXMLNodes_t nodes = it_elements->second;
					for (auto it_elem : *nodes) {
						printXML(it_elem,tabs);
					}
				}
			}
			else if (node->value->getValueType() == _text) {
				pXMLTextNode_t val = dynamic_cast<pXMLTextNode_t>(node->value);
				LOG(DEBUG) << std::string(tabs, '\t') << node->name << "=> Value: " << val->v;
			}
		}


		pXMLRoot_t getXMLElement() { return rootXML; }

		ReadXMLConfig(std::wstring _file) : cfile(_file) {
			if (FAILED(hr = SHCreateStreamOnFile(cfile.c_str(), STGM_READ, &pFileStream))) {
				throw Exception(hr,ExceptionFormatter()<<"Error XML file: "<<cfile.c_str());
			}
			if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL))) {
				throw Exception(hr, ExceptionFormatter() << "Error parsing XML file: " << cfile.c_str());
			}
			if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit)))
			{
				throw Exception(hr, ExceptionFormatter() << "Error setting DTD probibit file: " << cfile.c_str());
			}
			if (FAILED(hr = pReader->SetInput(pFileStream))) {
				throw Exception(hr, ExceptionFormatter() << "Error failed to set XML Input stream: " << cfile.c_str());
			}
			parseElements(NULL);
		}
		~ReadXMLConfig() {}

		std::string getValue(std::string _tag) {

		}
		std::string getAtrribute(std::string _attr) {

		}
		
	};
}
