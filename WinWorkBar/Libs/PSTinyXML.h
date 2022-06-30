//
//PSTinyXML
//Recordset access class for TinyXML
//08-12-2003 © Piotr Zagawa
//http://vetch.magot.pl
//---------------------------------------------------------------------------------------

#pragma once

#include <windows.h>
#include <algorithm>
#include <string>
#include <vector>
#include "TinyXML.h"


namespace PSXML
{
//Example row definition
//---------------------------------------------------------------------------------------
/*
const	FieldDef ExampleFieldList[] = 
{
	"ID",
	"Name",
	"Date",
	NULL,		//IMPORTANT: marks end of list !
};
*/


//Utils
//---------------------------------------------------------------------------------------
std::string IntToStr(int I);
std::string FloatToStr(float L);
std::string IntToHex(int I);
int StrToInt(std::string& S);
int HexToInt(std::string& S);
float StrToFloat(std::string& S);
std::string GetNewUID();
std::string DateToStr(const SYSTEMTIME& ST);
std::string GetNowDate();
bool DateFromStr(const std::string& S, SYSTEMTIME& FT);


//XML util classes
//---------------------------------------------------------------------------------------
struct	FieldDef
{
	char*	Name;
	bool	ReadOnly;
	bool	Required;
	bool	Visible;

	FieldDef(char* _Name, bool _ReadOnly = false, bool _Required = true, bool _Visible = true)
	{
		Name = _Name;
		ReadOnly = _ReadOnly;
		Required = _Required;
		Visible = _Visible;
	}
};

class	Field
{
	bool				InvalidField;
public:
	std::string	Name;
	std::string	Value;
	FieldDef		Definition;

	Field();
	Field(const FieldDef* pFieldDef);

	void	SetNowDate();
	void	Set(std::string S);
	void	Set(float L);
	void	Set(int I);

	std::string AsString();
	float AsFloat();
	int		AsInt();

	bool	IsNull();
	void	Clear();
	void	SetThisFieldAsInvalid() { InvalidField = true; };
	bool	IsInvalidField() { return InvalidField; };
};

//For cached, read-only recordset
typedef std::vector<PSXML::Field> RSFields;
struct RSRow
{
	std::string ID;
	RSFields		Fields;
};
typedef std::vector<RSRow> RSRecordset;
typedef bool (*RSFilterAcceptRow)(RSRow& Row);					//filtering records callback function
typedef bool (*RSSortRecordset)(RSRow& R1, RSRow& R2);	//sorting records callback function

//Current row
class	Row
{
	Field				NullField;				//empty field for invalid returns
	TiXmlNode*	pCurrNode;				//nodes iterator
	std::string	strID_value;			//ID attribute value
public:
	TiXmlElement* pParentElement;	//parent element: "Data"
	std::vector<Field>	Fields;		//Fields objects array

	Row();
	Row(const FieldDef* FieldDefs);

	void SetID(int ID);
	void SetID(std::string& ID);
	void SetUID(std::string UID = GetNewUID());

	const std::string& GetID();
	int GetIDAsInt();

	bool	SetDefinition(const FieldDef* FieldDefs);

	Field&	FieldByName(std::string Name);
	bool		FieldExists(std::string Name);
	void		CopyRowValues(Row& SrcRow);

	void	ClearFields();

	bool	AddRow();
	bool	GetData();		//gets values to Field objects from current element
	bool	UpdateData();	//saves new fields values to current element

	bool	First();
	bool	Last();
	bool	Next();
	bool	IsEof();
	bool	HasNoRows();

	int		RowCount();
	bool	RowExists(std::string ID);	//returns true, if node exists
	bool	Locate(std::string ID);			//as above, but yet sets pCurrNode

	bool	Remove(std::string ID);			//remove row with given ID
	bool	RemoveAll();								//remove all rows
};


//PSTinyXML
//---------------------------------------------------------------------------------------
class PSTinyXML : public Row
{
private:
	TiXmlDocument		_XmlDoc;
	bool						_Loaded;
	bool						_Saved;
	std::string			_RootName;
	std::string			_TableName;
	std::string			_DataPath;
	std::string			_DirectFileName;
	TCHAR						_ExeFileName[MAX_PATH];

	std::string			_ProgramName;
	std::string			_ProgramVersion;

	TiXmlElement*		pXmlRoot;
	TiXmlElement*		pXmlInfo;
	TiXmlElement*		pXmlData;

	FieldDef*				lFieldDefs;

	std::string ExtractPath(std::string FileName);

	TiXmlElement*	CreateElement(TiXmlNode* pElement, std::string Name);
	bool	SetElementValue(TiXmlElement* pElement, std::string Value);
	bool	SetElement(TiXmlNode* pParentElement, std::string Name, std::string Value);
	bool	SetUpdatedInfo();
public:
	TiXmlDocument&	XmlDoc;

	PSTinyXML(void);
	~PSTinyXML(void);

	void	SetDirectFileName(std::string sDirectFileName);
	bool	SetProgramData(std::string Name, std::string Version, std::string DataFolder);
	bool	SetTableDefinition(const FieldDef* FieldDefs);

	bool	LoadTable(std::string TableName, const FieldDef* FieldDefs = NULL);
	bool	LoadTable(PSTinyXML& SrcTB);

	bool	SaveTable(std::string TableName = "");
	bool	SaveTableToFile(std::string sFileName);

	std::string TableFileName();

	//Cached, read-only recordset with filtering and sorting
	RSRecordset	Recordset;
	bool LoadRecordset(RSFilterAcceptRow fnFilterAcceptRow);
	bool SortRecordset(RSSortRecordset fnSortRecordset);
};

} //namespace PSXML
