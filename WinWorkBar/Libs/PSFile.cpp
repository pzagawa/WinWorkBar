//
// File
// File system i/o class
//-----------------------------------------------------------------------------------------

#include "PSFile.h"
#include "PSUtil.h"

//-----------------------------------------------------------------------------------------

const	File::retMsg	retCodes[] =
{
	File::feOK,							"OK",
	File::feOpenError,			"Open error",
	File::feNoBuffer,				"No buffer",
	File::feWriteError,			"Write error",
	File::feReadError,			"Read error",
	File::feFileClosed,			"File closed",
	File::feSetLengthError,	"Set length error",
	File::feGetPosError,		"Get pos error",
	File::feSetPosError,		"Set pos error",
	File::feOK,							NULL
};

//-----------------------------------------------------------------------------------------

//----------------------------
//RawFile CLASS
//----------------------------
File::RawFile::RawFile(DWORD BufferSize, DWORD FinderBufferSize)
{
	hFile				= NULL;
	DataBuffer	= NULL;
	_RetCode		= feOK;
	SetBuffer(BufferSize);
	FindBufferSize = FinderBufferSize;
	FindBuffer	= new BYTE[FindBufferSize];
}

File::RawFile::~RawFile()
{
	Close();
	delete [] FindBuffer;
	if (DataBuffer)
		delete [] DataBuffer;
	DataBuffer = NULL;
}

void	File::RawFile::Close()
{
	if (hFile)
		CloseHandle(hFile);
	hFile = NULL;
}

bool	File::RawFile::FileClosed()
{
	if (hFile)
	{
		return false;
	} else {
		_RetCode	= feFileClosed;
		return true;
	}
}

void	File::RawFile::SetBuffer(DWORD Size)
{
	if (DataBuffer)
		delete [] DataBuffer;
	DataBuffer			= NULL;
	DataBufferSize	= Size;
	DataBuffer			= new BYTE[Size];
	ClearBuffer();
};

DWORD	File::RawFile::BufferSize()
{
	return DataBufferSize;
}

char*	File::RawFile::RetMsg()
{
	retMsg*	lpData = (retMsg*)&retCodes;
	while (lpData->Msg)
	{
		if (lpData->Code == _RetCode)
			break;
		lpData++;
	}
	return lpData->Msg;
}

File::retCode	File::RawFile::RetCode()
{
	return _RetCode;
}

bool	File::RawFile::NoError()
{
	return (_RetCode == feOK);
}

void	File::RawFile::Open(char* FileName, fileAccess Access, fileShare Share, DWORD dwCreateType, DWORD dwAttributes)
{
	_FileName = FileName;

	Close();
	switch (Access)
	{
		case faRead:	_Access	= (DWORD)GENERIC_READ;		break;
		case faWrite:	_Access	= (DWORD)GENERIC_WRITE;		break;
		case faRW:		_Access	= (DWORD)(GENERIC_READ | GENERIC_WRITE); break;
	}

	switch (Share)
	{
		case	fsCheck:	_Share	= 0;	break;
		case	fsRead:		_Share	= FILE_SHARE_READ;	break;
		case	fsWrite:	_Share	= FILE_SHARE_WRITE;	break;
		case	fsRW:			_Share	= (FILE_SHARE_READ | FILE_SHARE_WRITE);	break;
	}

	if (dwCreateType == CREATE_ALWAYS)
		DeleteFile(FileName);

	hFile = CreateFile(	FileName, _Access, _Share, NULL, dwCreateType, dwAttributes, NULL);

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		_RetCode	= feOpenError;
	} else {
		_RetCode	= feOK;
	}
}

std::string File::RawFile::FileName()
{
	return _FileName;
}

DWORD	File::RawFile::Write(const BYTE* Data, DWORD BufferSize)
{
	WrittenData = 0;
	if (FileClosed())
		return 0;

	if (!Data)
		Data = DataBuffer;

	if (BufferSize == -1)
		BufferSize = DataBufferSize;
		
	if (WriteFile(hFile, Data, BufferSize, &WrittenData, NULL))
	{
		_RetCode	= feOK;
	} else {
		_RetCode	= feWriteError;
	}
	return 	WrittenData;
}

DWORD	File::RawFile::WriteNewLine()
{
	char	NewLine[3] = { 13, 10, 0 };

	return WriteFile(hFile, NewLine, 2, &WrittenData, NULL);
}

void	File::RawFile::SetLength(LONG	Value)
{
	if (FileClosed())
		return;

	DWORD Result = SetFilePointer(hFile, Value, NULL, FILE_BEGIN);
	if (Result == INVALID_SET_FILE_POINTER)
	{
		_RetCode	= feWriteError;
	} else {
		if (SetEndOfFile(hFile))
		{
			_RetCode	= feOK;
		} else {
			_RetCode	= feSetLengthError;
		}
	}
}

DWORD	File::RawFile::GetPos()
{
	if (FileClosed())
		return 0;

	DWORD	Pointer	= SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	if (Pointer == INVALID_SET_FILE_POINTER)
	{
		_RetCode	= feGetPosError;
	} else {
		_RetCode	= feOK;
	}
	return Pointer;
}

DWORD	File::RawFile::SetPos(LONG Value, DWORD From)
{
	if (FileClosed())
		return 0;

	DWORD	Pointer	= SetFilePointer(hFile, Value, NULL, From);
	if (Pointer == INVALID_SET_FILE_POINTER)
	{
		_RetCode	= feSetPosError;
	} else {
		_RetCode	= feOK;
	}
	return Pointer;
}

DWORD	File::RawFile::IncPos(DWORD Step)
{
	DWORD Pos = GetPos() + Step; 
	SetPos(Pos);
	return Pos;
}

DWORD	File::RawFile::DecPos(DWORD Step)
{
	DWORD Pos = GetPos() - Step;
	SetPos(Pos);
	return Pos;
}

DWORD	File::RawFile::Find(BYTE* Sequence, DWORD SequenceSize)
{
	if (FileClosed())
		return -1;

	_RetCode = feFindNone;
	DWORD SavePointer	= GetPos();
	DWORD CmpSavePointer = -1;
	ZeroMemory(FindBuffer, FindBufferSize);

	while (true)
	{
		CmpSavePointer = GetPos();
		if (!NoError())
			return -1;

		DWORD BytesReaded = Read(FindBuffer, FindBufferSize);
		if (BytesReaded == 0)
		{
			CmpSavePointer = -1;
			break;
		}

		DWORD Pos = Misc::FindMemory(FindBuffer, FindBufferSize, Sequence, SequenceSize);
		if (Pos != -1)
		{
			CmpSavePointer += Pos;
			_RetCode = feOK;
			break;
		}

		if (EndOfFile())
			return -1;

		DecPos(SequenceSize * 2);
	}

	SetPos(SavePointer);
	return CmpSavePointer;
}

void	File::RawFile::SetBegin()
{
	SetPos(0);
}

void	File::RawFile::SetEnd()
{
	SetPos(0, FILE_END);
}

bool	File::RawFile::EndOfFile()
{
	return (GetPos() >= Size());
}

DWORD	File::RawFile::Size()
{
	if (FileClosed())
		return 0;
	return GetFileSize(hFile, NULL);
}

DWORD	File::RawFile::Attributes()
{
	_RetCode = feOK;
	DWORD ATTR = GetFileAttributes(_FileName.c_str());
	if (ATTR == INVALID_FILE_ATTRIBUTES)
	{
		ATTR = FILE_ATTRIBUTE_NORMAL;
		_RetCode = feGetAttribError;
	}
	return ATTR;
}

bool	File::RawFile::GetTime(LPFILETIME ftCreation, LPFILETIME ftLastAccess, LPFILETIME ftLastWrite)
{
	bool OK = (GetFileTime(hFile, ftCreation, ftLastAccess, ftLastWrite) != 0);
	_RetCode = (OK)?feOK:feGetTimeError;
	if (!OK)
	{
		SYSTEMTIME	SysTime;
		GetSystemTime(&SysTime);
		SystemTimeToFileTime(&SysTime, ftCreation);
		SystemTimeToFileTime(&SysTime, ftLastAccess);
		SystemTimeToFileTime(&SysTime, ftLastWrite);
	}
	return OK;
}

SYSTEMTIME File::RawFile::GetLastWriteTime()
{
	SYSTEMTIME	time;
	GetLocalTime(&time);
  FILETIME ftCreate, ftAccess, ftWrite, ftLocal;
	if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
    if (FileTimeToLocalFileTime(&ftWrite, &ftLocal))
	    FileTimeToSystemTime(&ftLocal, &time);
	return time;
}

bool	File::RawFile::SetTime(LPFILETIME ftCreation, LPFILETIME ftLastAccess, LPFILETIME ftLastWrite)
{
	return (SetFileTime(hFile, ftCreation, ftLastAccess, ftLastWrite) != 0);
}

DWORD	File::RawFile::Read(BYTE* Data, DWORD BufferSize)
{	
	if (FileClosed())
		return 0;

	if (!Data)
		Data = DataBuffer;

	if (BufferSize == -1)
		BufferSize = DataBufferSize;
		
	if (ReadFile(hFile, Data, BufferSize, &ReadedData, NULL))
	{
		_RetCode	= feOK;
	} else {
		_RetCode	= feReadError;
	}
	return	ReadedData;
}

BYTE*	File::RawFile::Buffer()
{
	return DataBuffer;
}

DWORD	File::RawFile::BytesReaded()
{
	return ReadedData;
}

DWORD	File::RawFile::BytesWritten()
{
	return WrittenData;
}

void	File::RawFile::ClearBuffer()
{
	if (DataBuffer)
		ZeroMemory(DataBuffer, DataBufferSize);
}

void	File::RawFile::Flush()
{
	FlushFileBuffers(hFile);
}


//----------------------------
//TextFile CLASS
//----------------------------
DWORD	File::TextFile::WriteLine(char* Line)
{
	DWORD	I = Write((BYTE*)Line, (DWORD)strlen(Line));
	WriteNewLine();
	return I;
}

DWORD File::TextFile::WriteLine(std::string Line)
{
	DWORD	I = Write((BYTE*)Line.c_str(), (DWORD)Line.length());
	WriteNewLine();
	return I;
}

DWORD	File::TextFile::ReadLine()
{
	DWORD	Start		= GetPos();
	DWORD	Size		= Read();
	BYTE* lpPtr		= DataBuffer;

	//search for #13 code
	while (*lpPtr != 13)
	{
		if (lpPtr == (DataBuffer + DataBufferSize))
		{
			lpPtr-=3;
			break;
		}
		lpPtr++;
	}

	//check #10 code
	*lpPtr++;
	if (*lpPtr == 10)
	{
		*lpPtr++;
	}

	//terminate string
	*lpPtr	= 0;
	Size = (DWORD)strlen((char*)DataBuffer);
	SetPos(Start + Size);
	return Size;
}

char*	File::TextFile::StringBuffer(bool RemoveNewLine)
{
	if (RemoveNewLine)
	{
		char* lpPtr = (char*)DataBuffer;
		int	Count = DataBufferSize;
		while (Count--)
		{
			if ((*lpPtr == 13) || (*lpPtr == 10))
				*lpPtr = 0;
			lpPtr++;
		}
	}
	return (char*)DataBuffer;
}

int	File::TextFile::LinesCount()
{
	if (FileClosed())
		return 0;
	int Counter = 0;
	SetBegin();
	while (!EndOfFile())
	{
		ReadLine();
		Counter++;
	}
	SetBegin();
	return Counter;
}

std::string File::TextFile::GetLine(bool ReplaceInvisibleChars)
{
	std::string S = StringBuffer();
	if (!S.empty())
		if (ReplaceInvisibleChars)
			S = Text::Replace(S, "\\n", "\n");
	return S;
}

void File::TextFile::PutLine(std::string S, bool ReplaceInvisibleChars)
{			
	if (ReplaceInvisibleChars)
		S = Text::Replace(S, "\n", "\\n");
	int MaxLength = (int)S.length();
	if (MaxLength > (int)DataBufferSize)
		MaxLength = DataBufferSize;
	lstrcpyn((char*)DataBuffer, S.c_str(), MaxLength);
}


//----------------------------
//RecursiveDir CLASS
//----------------------------
File::RecursiveDir::RecursiveDir(LPVOID UserData)
{
	fnEvent = NULL;
	ResetData();
	_UserData = UserData;
}

void	File::RecursiveDir::ResetData()
{
	_Exclude = false;
	_Cancel = false;
	_Size	= 0;
	_Count = 0;
}

bool	File::RecursiveDir::GetFiles(std::string Directory)
{
	std::string FileName;
	HANDLE			hSearch; 
	Directory = IncludeBackSlash(Directory);
	hSearch = FindFirstFile((Directory + "*.*").c_str(), &_FileData); 
	if (hSearch != INVALID_HANDLE_VALUE) 
	{ 
		while (true)
		{
			if (FindNextFile(hSearch, &_FileData))
			{
				FileName = _FileData.cFileName;		
				if ((_FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				{
					if ((FileName != ".") && (FileName != ".."))
						GetFiles(Directory + FileName);
				} else {
					_Exclude = false;
					if (fnEvent)
						fnEvent(_UserData, Directory, _FileData, (bool)_Cancel, _Exclude);
					if (!_Exclude)
					{
						_Size += _FileData.nFileSizeLow;
						_Count++;
					}
					if (_Cancel)
						break;
				}
			} else {
				break;
			}
		}
		FindClose(hSearch);
	} else {
		return false;
	}
	return true;
}

bool	File::RecursiveDir::Get(std::string Directory)
{
	_Error = feOK;
	ResetData();
	bool Status = GetFiles(Directory);
	if (!Status)
		_Error = feDirError;
	return Status;
}


//Tools
//----------------------------
namespace File
{
	std::string ParentDirectory(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );

		std::string Path = dir;
		int LastSlash = (int)Path.find_last_of("\\");
		Path = Path.erase(LastSlash + 1);
		Path = File::ExcludeBackSlash(Path);
		return drive + Path;
	}

	std::string SubDirectory(std::string ParentDirectory, std::string FileName, bool& PathExists)
	{
		ParentDirectory = (ParentDirectory.empty())?ParentDirectory:IncludeBackSlash(ParentDirectory);
		std::string S = ExtractPathWithoutDrive(FileName);
		int Pos = (int)S.find(ParentDirectory);
		if (Pos == 0)
		{
			S = S.substr(ParentDirectory.length());
			Pos = (int)S.find("\\");
			if (Pos >= 0)
				S = S.substr(0, Pos + 1);
			PathExists = true;
		} else {
			PathExists = false;
			S = "";
		}
		return S;
	}

	std::string RelativePath(std::string BasePath, std::string FileName)
	{
		BasePath = IncludeBackSlash(BasePath);
		if (FileName.find(BasePath) == 0)
			FileName = FileName.substr(BasePath.length());
		return ExcludeBackSlash(FileName);
	}

	std::string ExcludeBackSlash(std::string S)
	{
		std::string::size_type IDX = S.find_last_of("\\");
		if (IDX == S.length() - 1)
			S = S.substr(0, IDX);
		return S;
	}

	std::string IncludeBackSlash(std::string S)
	{
		std::string::size_type IDX = S.find_last_of("\\");
		if (IDX != S.length() - 1)
			S = S + "\\";
		if (S.length() == 0)
			S = S + "\\";
		return S;
	}

	bool	ForceCreateDirs(std::string Path)
	{
		std::string S = IncludeBackSlash(Path);
		std::string DirToCreate = "";
		int IDX = (int)std::string::npos;
		if (S.length() > 0)
		{
			std::string T = S;		
			while (true)
			{
				if ((IDX = (int)S.find("\\")) == std::string::npos)
				{
					break;
				} else {
					S[IDX] = ',';
				}
				DirToCreate = IncludeBackSlash((char*)T.substr(0, IDX).c_str());
				CreateDirectory((char*)DirToCreate.c_str(), NULL);
			}
		}
		return true;
	};

	bool	IsFileName(std::string FileName)
	{
		return (FileName.find_last_of("\\") != FileName.length() - 1);
	}

	std::string ExtractPath(std::string FileName)
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

	std::string ExtractPathWithoutDrive(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		std::string Path = dir;
		if ((int)Path.length() > 0)
			if (Path.at(0) == '\\')
				Path = Path.substr(1);
		return Path;
	}

	std::string ExtractDriveLetter(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		std::string Drive = drive;
		if (Drive.find(":") == 1)
			Drive.erase(1);
		return Drive;
	}

	std::string ExtractFileNameWithoutDrive(std::string FileName, bool RemoveRootSlash)
	{
		if (FileName.length() > 1)
			if (FileName.at(1) == ':')
			{
				FileName = FileName.substr(2);
				if (FileName.length() > 0)
					if (FileName.at(0) == '\\')
						FileName = FileName.substr(1);
			}
		return FileName;
	}

	std::string ExtractFileName(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		return (std::string)fname + (std::string)ext;
	}

	std::string	FileExtension(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		FileName = ext;
		if ((FileName.find_first_of(".") == 0) && (FileName.length() > 0))
			FileName = FileName.substr(1);
		return FileName;
	}

	std::string	RemoveFileExtension(std::string FileName)
	{
		std::basic_string<char>::size_type pos = FileName.find_last_of(".");
		if (pos != -1)
			FileName = FileName.erase(pos);
		return FileName;
	}

	DWORD	GetFreeSpaceInKB(std::string FileName)
	{
		FileName = ExtractPath(FileName);
		ULARGE_INTEGER	i64FreeBytesToCaller;
		ULARGE_INTEGER	i64TotalBytes;
		ULARGE_INTEGER	i64FreeBytes;
		GetDiskFreeSpaceEx(FileName.c_str(), &i64FreeBytesToCaller, &i64TotalBytes, &i64FreeBytes);
		__int64 I = i64FreeBytes.QuadPart;
		return DWORD(I >> 10); //divide by 1024
	}

	__int64	GetFreeSpaceInBytes(std::string FileName)
	{
		FileName = ExtractPath(FileName);
		ULARGE_INTEGER	i64FreeBytesToCaller;
		ULARGE_INTEGER	i64TotalBytes;
		ULARGE_INTEGER	i64FreeBytes;
		GetDiskFreeSpaceEx(FileName.c_str(), &i64FreeBytesToCaller, &i64TotalBytes, &i64FreeBytes);
		return i64FreeBytes.QuadPart;
	}

	std::string GetLargestFreeDrive()
	{
		DWORD List[128];
		char  Drive[] = "C:\\";
		ZeroMemory(&List, sizeof(List));

		for (char idx = 'C'; idx < 'Z'; idx++)
		{
			Drive[0] = idx;
			if (GetDriveType(Drive) == DRIVE_FIXED)
			{
				List[idx] = GetFreeSpaceInKB(Drive);
			}
		}

		Drive[0] = 'C';
		DWORD Size = 0;
		for (char idx = 'C'; idx < 'Z'; idx++)
		{
			if (List[idx] > Size)
			{
				Drive[0] = idx;
				Size = List[idx];
			}
		}
		return Drive;
	}

	bool	IsDriveRemovable(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		FileName = drive;
		FileName = IncludeBackSlash(FileName);
		return (GetDriveType(FileName.c_str()) == DRIVE_REMOVABLE);
	}

	bool	IsDriveFixed(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		FileName = drive;
		FileName = IncludeBackSlash(FileName);
		return (GetDriveType(FileName.c_str()) == DRIVE_FIXED);
	}

	bool	IsDriveNetwork(std::string FileName)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath((char*)FileName.c_str(), drive, dir, fname, ext );
		FileName = drive;
		FileName = IncludeBackSlash(FileName);
		return (GetDriveType(FileName.c_str()) == DRIVE_REMOTE);
	}

	bool	FileExists(std::string FileName)
	{
		DWORD dwFileAttr = GetFileAttributes(FileName.c_str());
		if ((dwFileAttr != INVALID_FILE_ATTRIBUTES) && !(dwFileAttr & FILE_ATTRIBUTE_DIRECTORY))
			return TRUE;
		else
			return FALSE;
	}

	bool DirectoryExists(std::string FileName)
	{
		DWORD dwFileAttr = GetFileAttributes(FileName.c_str());
		if ((dwFileAttr != INVALID_FILE_ATTRIBUTES) && (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY))
			return TRUE;
		else
			return FALSE;
	}

	std::string MergePaths(std::string Left, std::string Right)
	{
		std::string S = "";
		if (Left.empty())
		{
			S = Right;
		} else {
			S = Left;
			if ((int)S.find_last_of("\\") == S.length() - 1)
				S += Right;
		}
		return S;
	}

	void CreatePathIfNotExists(std::string sPath)
	{
		if (!DirectoryExists(sPath))
			ForceCreateDirs(sPath);
	}

	void GetDrivesByType(std::vector<std::string>& vDrives, UINT uDriveType)
	{
		char Drive[] = "C:\\";
		for (char idx = 'C'; idx < 'Z'; idx++)
		{
			Drive[0] = idx;
			UINT uType = GetDriveType(Drive);
			if (uType == uDriveType)
				vDrives.push_back(Drive);
		}
	}

	bool IsPathWritable(std::string sPath, std::string sAppName)
	{
		bool bResult = false;
		sPath = sPath + sAppName + "_TestTempFile.tmp";
		HANDLE hFile = CreateFile(sPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN | FILE_FLAG_DELETE_ON_CLOSE, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bResult = true;
			CloseHandle(hFile);
		}
		return bResult;
	}

	std::string LoadFileToString(std::string sFileName, bool* Result)
	{
		if (Result)
			*Result = false;
		std::string S;
		if (File::FileExists(sFileName))
		{
			File::TextFile TF;
			TF.Open((char*)sFileName.c_str(), File::faRead, File::fsRead, OPEN_EXISTING);
			if (TF.NoError())
			{
				TF.Read();
				S = TF.StringBuffer(false);
				TF.Close();
				if (Result)
					*Result = true;
			}
		}
		return S;
	}

}
