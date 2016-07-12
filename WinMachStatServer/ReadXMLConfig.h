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
			_value,
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
			vtype_t vtype;
			union _values {
				std::wstring vstr;
				XMLNodes_t elmt;
				_values() {};
				~_values() {};
			} ;

			_values values;

			operator std::wstring() {
			}

			operator const XMLElement_t &() {
			}

			Element() {}

			~Element() {
				if (vtype == _element) {
					for (auto it : values.elmt) {
						delete it;
					}
				}
			}
		};

		pXMLElement_t elmts = NULL;

		void parseElements(pXMLElement_t pXMLElement) {
			XmlNodeType nodeType;
			const WCHAR* pQName;
			const WCHAR* pValue;
			Element *pElement = NULL;
			Element *pRootElement = NULL;
			std::stack<Element *> stElements;
			
			if (!pXMLElement) {
				pXMLElement = new XMLElement_t;
			}
			bool nodeStart = false; 
			bool valStart = false;

			while (S_OK == (hr = pReader->Read(&nodeType)))
			{
				switch (nodeType)
				{
				case XmlNodeType_Element:
				{
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

						if (!valStart) { /* Root or SubRoot */
							if (pRootElement) {
								stElements.push(pRootElement);
							}

							pRootElement = new Element();
							pRootElement->name = pQName;
							pRootElement->vtype = _element;
							(*pXMLElement)[pRootElement->name] = &pRootElement->values.elmt;
						}
						else { /* May be value */
							pElement = new Element();
							pElement->name = pQName;
						}

						nodeStart = true;
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

					pElement->vtype = _value;
					pElement->values.vstr = pValue;
					valStart = true;
				}

				break;
				case XmlNodeType_EndElement:
				{
					if (!valStart) {
						if (!stElements.empty()) {
							Element *pTmpElement = pRootElement;
							pRootElement = stElements.top(); stElements.pop();
							(*pXMLElement)[pRootElement->name] = &pTmpElement->values.elmt;
						}
					}
					else {
						(*pXMLElement)[pRootElement->name]->push_back(pElement);
					}
					valStart = false;
					pElement = NULL;
				}

				break;
				default:
				{

				}
				break;
				}
			}
		}

	public:
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
		}
		~ReadXMLConfig() {}
		std::string getValue(std::string _tag) {

		}
		std::string getAtrribute(std::string _attr) {

		}
		
	};
}
