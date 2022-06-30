
#include "PSTinyXML.h"
#include "PSFile.h"

namespace PSXML
{

//Utils
//---------------------------------------------------------------------------------------
std::string IntToStr(int I)
{
	char _Buff[16] = {0};
	itoa(I, _Buff, 10);
	return _Buff;
}

std::string FloatToStr(float L)
{
	char _Buff[32] = {0};
	_gcvt(L, 16, _Buff);
	return _Buff;
}

std::string IntToHex(int I)
{
	char _Buff[16] = {0};
	itoa(I, _Buff, 16);
	return _Buff;
}

int StrToInt(std::string& S)
{
	if (S.empty())
		return 0;
	return atoi((char*)S.c_str());
}

float StrToFloat(std::string& S)
{
	if (S.empty())
		return 0;
	return (float)atof((char*)S.c_str());
}

int HexToInt(std::string& S)
{
	static const char List[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	if (S.empty())
		return 0;
	int		Sum = 0;
	char C[2] = { 0, 0 };
	DWORD	Mask = 0x1;
	int Len = (int)S.length();
	for (int i = 0; i < Len; i++)
	{
		char C = toupper(S[Len - 1 - i]);
		int D = 0;
		for (int j = 0; j < 16; j++)
		{
			if (C == List[j])
			{
				D = j;
				break;
			}
		}
		Sum += (D * (Mask));
		Mask = Mask << 4;
	}
	return Sum;
}

std::string GetNewUID()
{
	GUID	MyUID;
  CoCreateGuid(&MyUID);
	char _Buff[32] = {0};

	itoa(MyUID.Data1, _Buff, 16);
	std::string S = _Buff;
	S+= "-";
	itoa(MyUID.Data2, _Buff, 16);
	S += _Buff;
	S += "-";
	itoa(MyUID.Data3, _Buff, 16);
	S += _Buff;
	S += "-";
	for (int i = 0; i < 8; i++)
	{
		_itoa(MyUID.Data4[i], _Buff, 16);
		S += _Buff;
	}
	return S;
}

std::string GetNowDate()
{
	SYSTEMTIME ST;
	GetLocalTime(&ST);
	return DateToStr(ST);
}

std::string DateToStr(const SYSTEMTIME& ST)
{
	char Out[32] = { '0' };
	wsprintf(Out, "%0.2d-%0.2d-%0.4d %0.2d:%0.2d.%0.2d",
		ST.wDay, ST.wMonth, ST.wYear, ST.wHour, ST.wMinute, ST.wSecond);
	return Out;
}

//DD-MM-YYYY HH:MM:SS
//0123456789012345678
//10-01-2004 14:20.58
bool DateFromStr(const std::string& S, SYSTEMTIME& ST)
{
	if (S.empty())
		return false;
	if ((int)S.length() == 19)
	{
		ST.wDay			= StrToInt(S.substr(0, 2));
		ST.wMonth		= StrToInt(S.substr(3, 2));
		ST.wYear		= StrToInt(S.substr(6, 4));
		ST.wHour		= StrToInt(S.substr(11, 2));
		ST.wMinute	= StrToInt(S.substr(14, 2));
		ST.wSecond	= StrToInt(S.substr(17, 2));
		ST.wMilliseconds = 0;
		ST.wDayOfWeek = 0;
		return true;
	}
	return false;
}


//Field
//---------------------------------------------------------------------------------------
Field::Field()
	:	Definition("", false, true, true)
{
	Value.clear();
	InvalidField = false;
}

Field::Field(const FieldDef* pFieldDef)
	:	Definition(pFieldDef->Name, pFieldDef->ReadOnly, pFieldDef->Required, pFieldDef->Visible)
{
	Value.clear();
	Name = pFieldDef->Name;
	InvalidField = false;
}

void Field::Set(std::string S)
{
	if (!Definition.ReadOnly)
		Value = S;
}

void Field::Set(float L)
{
	if (!Definition.ReadOnly)
		Value = FloatToStr(L);
}

void Field::Set(int I)
{
	if (!Definition.ReadOnly)
		Value = IntToStr(I);
}

std::string Field::AsString()
{
	return Value;
}

float Field::AsFloat()
{
	return StrToFloat(Value);
}

int	Field::AsInt()
{
	return StrToInt(Value);
}

bool Field::IsNull()
{
	return (Value.empty());
}

void Field::Clear()
{
	if (!Definition.ReadOnly)
		Value.clear();
}

void Field::SetNowDate()
{
	Set(GetNowDate());
}


//Row
//---------------------------------------------------------------------------------------
Row::Row()
{
	pParentElement = NULL;
	pCurrNode = NULL;
	NullField.SetThisFieldAsInvalid();
	SetDefinition(NULL);
	ClearFields();
}

Row::Row(const FieldDef* FieldDefs)
{
	pParentElement = NULL;
	pCurrNode = NULL;
	NullField.SetThisFieldAsInvalid();
	SetDefinition(FieldDefs);
	ClearFields();
}

void Row::SetID(int ID)
{
	strID_value = IntToStr(ID);
}

void Row::SetID(std::string& ID)
{
	strID_value = ID;
}

void Row::SetUID(std::string UID)
{
	strID_value = UID;
}

const std::string& Row::GetID()
{
	return strID_value;
}

int Row::GetIDAsInt()
{
	return StrToInt(strID_value);
}

Field&	Row::FieldByName(std::string Name)
{
	for (int i = 0; i < (int)Fields.size(); i++)
	{
		if (Name == Fields[i].Name)
			return Fields[i];
	}
	return NullField;
}

bool	Row::FieldExists(std::string Name)
{
	for (int i = 0; i < (int)Fields.size(); i++)
	{
		if (Name == Fields[i].Name)
			return true;
	}
	return false;
}

void	Row::CopyRowValues(Row& SrcRow)
{
	for (int i = 0; i < (int)Fields.size(); i++)
	{
		if (SrcRow.FieldExists(Fields[i].Name))
		{
			Fields[i].Set(SrcRow.FieldByName(Fields[i].Name).AsString());
		}
	}
}

bool	Row::SetDefinition(const FieldDef* FieldDefs)
{
	if (FieldDefs)
	{
		int i = 0;
		Fields.clear();
		while (FieldDefs[i].Name)
		{
			Field	MyField(&FieldDefs[i]);
			Fields.push_back(MyField);
			i++;
		}
		return true;
	} else {
		return false;
	}
}

void	Row::ClearFields()
{
	for (int i = 0; i < (int)Fields.size(); i++)
		Fields[i].Value.clear();
	strID_value.clear();
}

bool	Row::AddRow()
{
	if (strID_value.empty())
		return NULL;

	TiXmlElement	XmlRow("Row");
	XmlRow.SetAttribute("ID", strID_value.c_str());
	TiXmlText			XmlText("");

	for (int i = 0; i < (int)Fields.size(); i++)
	{
		TiXmlElement	XmlField(Fields[i].Name.c_str());
		XmlText.SetValue(Fields[i].Value.c_str());
		XmlField.InsertEndChild(XmlText);
		XmlRow.InsertEndChild(XmlField);
	}

	if (pParentElement)
		return (pParentElement->InsertEndChild(XmlRow) != NULL);
	else
		return NULL;
}

bool	Row::First()
{
	pCurrNode = NULL;
	if (pParentElement)
		pCurrNode = pParentElement->FirstChildElement();
	return (pCurrNode != NULL);
}

bool	Row::Last()
{
	if (pParentElement)
		pCurrNode = pParentElement->LastChild();
	return (pCurrNode != NULL);
}

bool	Row::Next()
{
	if (pCurrNode)
		pCurrNode = pCurrNode->NextSibling();
	return (pCurrNode != NULL);
}

bool	Row::IsEof()
{
	return (pCurrNode == NULL);
}

bool	Row::HasNoRows()
{
	if (pParentElement)
		return (pParentElement->NoChildren());
	return false;
}

int		Row::RowCount()
{
	int Sum = 0;
	if (pParentElement)
	{
		for (TiXmlNode* pRow = pParentElement->FirstChild(); pRow; pRow = pRow->NextSibling())
			Sum++;
	}
	return Sum;
}	

bool	Row::GetData()
{
	if (pCurrNode)
	{
		strID_value.clear();
		TiXmlElement* pElement = pCurrNode->ToElement();
		if (pElement)
			strID_value = pElement->Attribute("ID");

		for (TiXmlNode* pFieldNode = pCurrNode->FirstChild(); pFieldNode; pFieldNode = pFieldNode->NextSibling())
		{
			Field& pField = FieldByName(pFieldNode->Value());
			if (!pField.IsInvalidField())
			{
				pField.Clear();
				TiXmlNode* pFieldNodeValue = pFieldNode->FirstChild();
				if (pFieldNodeValue)
				{
					pField.Set(pFieldNodeValue->Value());
				}
			}
		}
		return true;
	}
	return false;
}

bool	Row::RowExists(std::string ID)
{
	if (pParentElement)
	{
		std::string AttrValue;
		for (TiXmlNode* pRow = pParentElement->FirstChild(); pRow; pRow = pRow->NextSibling())
		{
			TiXmlElement* pElement = pRow->ToElement();
			if (pElement)
			{
				AttrValue = pElement->Attribute("ID");
				if (AttrValue == ID)
					return true;
			}
		}
	}
	return false;
}

bool	Row::Locate(std::string ID)
{
	pCurrNode = NULL;
	if (pParentElement)
	{
		std::string AttrValue;
		for (pCurrNode = pParentElement->FirstChild(); pCurrNode; pCurrNode = pCurrNode->NextSibling())
		{
			TiXmlElement* pElement = pCurrNode->ToElement();
			if (pElement)
			{
				AttrValue = pElement->Attribute("ID");
				if (AttrValue == ID)
				{
					return true;
				}
			}
		}
	}
	pCurrNode = NULL;
	return false;
}

bool	Row::UpdateData()
{
	if (pCurrNode)
	{
		std::string FieldName;
		TiXmlText ElementValue("");
		for (int i = 0; i < (int)Fields.size(); i++)
		{
			FieldName = Fields[i].Name;
			ElementValue.SetValue(Fields[i].AsString().c_str());

			TiXmlNode* pNode = pCurrNode->FirstChild(FieldName.c_str());
			if (pNode)
			{
				pNode->Clear();
				pNode->InsertEndChild(ElementValue);
			} else {
				TiXmlElement Element(FieldName.c_str());
				Element.InsertEndChild(ElementValue);
				pCurrNode->InsertEndChild(Element);
			}
		}
		return true;
	}
	return false;
}

bool	Row::RemoveAll()
{
	if (pParentElement)
		pParentElement->Clear();
	return true;
}

bool	Row::Remove(std::string ID)
{
	if (Locate(ID))
	{
		if ((pParentElement) && (pCurrNode))
		{
			pParentElement->RemoveChild(pCurrNode);
			return true;
		}
	}
	return false;
}


//PSTinyXML
//---------------------------------------------------------------------------------------

const	std::string	XmlElInfo	= "Information";
const	std::string	XmlElData	= "Data";

PSTinyXML::PSTinyXML(void)
	: XmlDoc(_XmlDoc)
{
	_Loaded = false;
	_Saved = false;
	pParentElement = NULL;
}

PSTinyXML::~PSTinyXML(void)
{
}

std::string PSTinyXML::ExtractPath(std::string FileName)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
	std::string Drive = drive;
	FileName = dir;
	return Drive + FileName;
}

TiXmlElement*	PSTinyXML::CreateElement(TiXmlNode* pElement, std::string Name)
{
	if (!pElement->FirstChildElement(Name.c_str()))
	{
		TiXmlElement XmlElement(Name.c_str());
		pElement->InsertEndChild(XmlElement);
	}
	return pElement->FirstChildElement(Name.c_str());
}

bool PSTinyXML::SetElementValue(TiXmlElement* pElement, std::string Value)
{
	if (pElement)
	{
		TiXmlText	XmlText(Value.c_str());
		pElement->Clear();
		pElement->InsertEndChild(XmlText);
		return true;
	}
	return false;
}

bool	PSTinyXML::SetElement(TiXmlNode* pParentElement, std::string Name, std::string Value)
{
	if (!pParentElement->FirstChildElement(Name.c_str()))
	{
		TiXmlElement XmlElement(Name.c_str());
		pParentElement->InsertEndChild(XmlElement);
	}
	TiXmlElement* pElement = pParentElement->FirstChildElement(Name.c_str());
	if (pElement)
	{
		TiXmlText	XmlText(Value.c_str());
		pElement->Clear();
		pElement->InsertEndChild(XmlText);
		return true;
	}
	return false;
}

bool	PSTinyXML::SetTableDefinition(const FieldDef* FieldDefs)
{
	lFieldDefs = (FieldDef*)FieldDefs;
	return SetDefinition(FieldDefs);
}

bool	PSTinyXML::LoadTable(PSTinyXML& SrcTB)
{
	return LoadTable(SrcTB._RootName, SrcTB.lFieldDefs);
}

bool	PSTinyXML::LoadTable(std::string TableName, const FieldDef* FieldDefs)
{
	pParentElement = NULL;
	_RootName = TableName;
	_TableName = TableName + ".xml";

	if (_DirectFileName.empty())
	{
		_Loaded = XmlDoc.LoadFile(TableFileName().c_str());
	} else {
		_Loaded = XmlDoc.LoadFile(_DirectFileName.c_str());
	}

	lFieldDefs = (FieldDef*)FieldDefs;
	SetDefinition(FieldDefs);

	//Add declaration
	if (XmlDoc.NoChildren())
	{
		TiXmlDeclaration	XmlDec("1.0", "windows-1250", "yes");
		XmlDoc.InsertEndChild(XmlDec);
	}
	//Add root
	pXmlRoot = CreateElement(&XmlDoc, _RootName);
	if (pXmlRoot)
	{
		//Add information element
		pXmlInfo = CreateElement(pXmlRoot, XmlElInfo);
		if (pXmlInfo)
		{
			SetElement(pXmlInfo, "Name", _ProgramName);
			SetElement(pXmlInfo, "Version", _ProgramVersion);
			SetElement(pXmlInfo, "Saved", GetNowDate());
		}
		//Add data element
		pXmlData = CreateElement(pXmlRoot, XmlElData);
		pParentElement = pXmlData;
	}
	return _Loaded;
}

void	PSTinyXML::SetDirectFileName(std::string sDirectFileName)
{
	_DirectFileName = sDirectFileName;
}

bool	PSTinyXML::SetProgramData(std::string Name, std::string Version, std::string DataFolder)
{
	_ProgramName = Name;
	_ProgramVersion = Version;
	_DataPath = DataFolder + "Data\\";
	return true;
}

bool	PSTinyXML::SetUpdatedInfo()
{
	if (pXmlInfo)
	{
		return SetElement(pXmlInfo, "Updated", GetNowDate());
	}
	return false;
}

std::string PSTinyXML::TableFileName()
{
	File::CreatePathIfNotExists(_DataPath);
	return _DataPath + _TableName;
}

bool	PSTinyXML::SaveTable(std::string TableName)
{
	if (_DirectFileName.empty())
	{
		if (TableName.empty())
		{
			_Saved = XmlDoc.SaveFile(TableFileName().c_str());
		} else {
			_Saved = XmlDoc.SaveFile((_DataPath + TableName).c_str());
		}
	} else {
		_Saved = XmlDoc.SaveFile(_DirectFileName.c_str());
	}
	return _Saved;
}

bool	PSTinyXML::SaveTableToFile(std::string sFileName)
{
	_Saved = XmlDoc.SaveFile(sFileName.c_str());
	return _Saved;
}

bool PSTinyXML::LoadRecordset(RSFilterAcceptRow fnFilterAcceptRow)
{
	RSRow Row;
	Recordset.clear();
	First();
	while (!IsEof())
	{
		if (GetData())
		{
			Row.ID = GetID();
			Row.Fields = Fields;
			if (fnFilterAcceptRow)
			{
				if (fnFilterAcceptRow(Row))
					Recordset.push_back(Row);
			} else {
				Recordset.push_back(Row);
			}
		}
		Next();
	}
	return true;
}

bool PSTinyXML::SortRecordset(RSSortRecordset fnSortRecordset)
{
	std::sort(Recordset.begin(), Recordset.end(), fnSortRecordset);
	return true;
}

}	//namespace PSXML
