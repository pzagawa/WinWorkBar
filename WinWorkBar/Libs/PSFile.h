//
// PSFile
// File i/o class
// Copyright by Piotr Zagawa. 2002 All rights reserved.
// Wszystkie prawa zastrze¿one
//
// ver.1.0:	23-09-2002
//

/* -----------------------------------------------------------------------------------------
dwCreateType:
-------------
CREATE_NEW				Creates a new file. The function fails if the specified file already exists. 

CREATE_ALWAYS			Creates a new file. If the file exists, the function overwrites the file, clears the existing attributes,
									and combines the file attributes and flags specified by dwFlagsAndAttributes with FILE_ATTRIBUTE_ARCHIVE.

OPEN_EXISTING			Opens the file. The function fails if the file does not exist. For a discussion of why you should use the
									OPEN_EXISTING flag if you are using the CreateFile function for devices, see Remarks.

OPEN_ALWAYS				Opens the file, if it exists. If the file does not exist, the function creates
									the file as if dwCreationDisposition were CREATE_NEW. 

TRUNCATE_EXISTING	Opens the file. Once opened, the file is truncated so that its size is zero bytes. The calling process
									must open the file with at least GENERIC_WRITE access. The function fails if the file does not exist.
----------------------------------------------------------------------------------------- */

#pragma once

#include	<string>
#include	<vector>
#include	<Windows.h>

//------------------------------------------------------------------------------------------------

namespace File
{
	//UTILS
	//-----------------------------------------------------------------------------
	std::string ParentDirectory(std::string FileName);
	std::string RelativePath(std::string BasePath, std::string FileName);
	std::string SubDirectory(std::string ParentDirectory, std::string FileName, bool& PathExists); //returns subdirectory of FileName in ParentDirectory
	std::string ExcludeBackSlash(std::string S);
	std::string IncludeBackSlash(std::string S);
	std::string ExtractPath(std::string FileName);	//extracts DRIVE + DIRECTORY
	std::string ExtractPathWithoutDrive(std::string FileName); //extracts DIRECTORY
	std::string ExtractFileNameWithoutDrive(std::string FileName, bool RemoveRootSlash); //extracts Dir + FileName without drive
	std::string ExtractFileName(std::string FileName);
	std::string ExtractDriveLetter(std::string FileName);
	std::string	FileExtension(std::string FileName);
	std::string	RemoveFileExtension(std::string FileName);

	bool	IsFileName(std::string FileName);				//checks for BACKSLASH on string's end
	bool	ForceCreateDirs(std::string Path);

	DWORD	GetFreeSpaceInKB(std::string FileName);
	__int64	GetFreeSpaceInBytes(std::string FileName);

	std::string GetLargestFreeDrive();

	//uDriveType
	//DRIVE_UNKNOWN The drive type cannot be determined. 
	//DRIVE_NO_ROOT_DIR The root path is invalid. For example, no volume is mounted at the path. 
	//DRIVE_REMOVABLE The disk can be removed from the drive. 
	//DRIVE_FIXED The disk cannot be removed from the drive. 
	//DRIVE_REMOTE The drive is a remote (network) drive. 
	//DRIVE_CDROM The drive is a CD-ROM drive. 
	//DRIVE_RAMDISK 
	void GetDrivesByType(std::vector<std::string>& vDrives, UINT uDriveType);

	bool IsPathWritable(std::string sPath, std::string sAppName);

	bool	IsDriveRemovable(std::string FileName);
	bool	IsDriveFixed(std::string FileName);
	bool	IsDriveNetwork(std::string FileName);

	bool	FileExists(std::string FileName);
	bool	DirectoryExists(std::string FileName);

	void CreatePathIfNotExists(std::string sPath);

	std::string MergePaths(std::string Left, std::string Right);

	std::string LoadFileToString(std::string sFileName, bool* Result);


	//Data
	//-----------------------------------------------------------------------------

	//Default data buffer size for one read/write operation (16KB)
	const int DefaultBufferSize = 16 * 1024;

	//Buffer size for data finding (1MB)
	const int DefaultFindBufferSize	= 1 * 1024 * 1024;

	enum	fileAccess	
	{	
		faRead, faWrite, faRW	
	};

	enum	fileShare		
	{	
		fsCheck, fsRead, fsWrite, fsRW 
	};

	enum	retCode			
	{	
		feOK = 0, 
		feOpenError, 
		feNoBuffer, 
		feWriteError,
		feReadError,
		feFileClosed, 
		feSetLengthError,
		feGetPosError,
		feSetPosError,
		feFindNone,
		feGetAttribError,
		feGetTimeError,
		feNotExists,
		feDirError,
	};

	struct	retMsg
	{
		retCode	Code;
		char*		Msg;
	};
	
	//----------------------------
	//RawFile CLASS
	//----------------------------
	class	RawFile
	{
	protected:
		HANDLE	hFile; 
		DWORD		_Access;
		DWORD		_Share;
		retCode	_RetCode;
		BYTE*		DataBuffer;
		DWORD		DataBufferSize;	
		DWORD		WrittenData;
		DWORD		ReadedData;
		BYTE*		FindBuffer;
		DWORD		FindBufferSize;
		std::string	_FileName;
	public:
		RawFile(DWORD BufferSize = DefaultBufferSize, DWORD FinderBufferSize = DefaultFindBufferSize);
		~RawFile();

		char*		RetMsg();
		retCode	RetCode();	

		void	SetBuffer(DWORD Size);
		void	ClearBuffer();
		DWORD	BufferSize();

		void	Open(char* FileName, fileAccess Access, fileShare Share, DWORD dwCreateType, DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL);
		void	Close();

		std::string FileName();

		//Returns true, if file handle is invalid
		bool	FileClosed();

		//Returns true, if prev op was ok
		bool	NoError();

		//Writes data from given buffer
		//if Data is NULL, then writes data from internal DataBuffer
		//returns size of written data
		DWORD	Write(const BYTE* Data = NULL, DWORD BufferSize = -1);

		//Reads data to buffer
		//if Data is NULL, then reads data to internal DataBuffer
		//returns size of readed data
		DWORD	Read(BYTE* Data = NULL, DWORD BufferSize = -1);

		//Sets file length
		void	SetLength(LONG	Value);

		//Returns current file pos
		DWORD	GetPos();

		//Sets current file pos
		DWORD	SetPos(LONG Value, DWORD From = FILE_BEGIN);

		//Sets pos...
		DWORD	IncPos(DWORD Step);
		DWORD	DecPos(DWORD Step);

		//Sets file pointer to begin
		void	SetBegin();

		//Sets file pointer to end
		void	SetEnd();

		//Returns true, if end of file
		bool	EndOfFile();

		//Finds sequence in file
		DWORD	Find(BYTE* Sequence, DWORD SequenceSize);

		//Returns opened file size
		DWORD	Size();

		//Returns attribs
		DWORD	Attributes();

		//Returns file times
		bool	GetTime(LPFILETIME ftCreation, LPFILETIME ftLastAccess, LPFILETIME ftLastWrite);
		//Sets file times
		bool	SetTime(LPFILETIME ftCreation, LPFILETIME ftLastAccess, LPFILETIME ftLastWrite);

		SYSTEMTIME GetLastWriteTime();

		//Returns pointer to buffer
		BYTE*	Buffer();
		//Returns bytes count of readed data
		DWORD	BytesReaded();
		//Returns bytes count of written data
		DWORD	BytesWritten();

		//Flush buffers after write !!!
		void	Flush();

		//Misc
		DWORD	WriteNewLine();
	};

	//----------------------------
	//TextFile CLASS
	//----------------------------
	class	TextFile : public RawFile
	{
	public:
		TextFile(DWORD LineBuffer = DefaultBufferSize) : RawFile(LineBuffer) {};
		DWORD	WriteLine(char* Line);
    DWORD WriteLine(std::string Line);
		DWORD	ReadLine();
		char*	StringBuffer(bool RemoveNewLine = true);
		int		LinesCount();

		std::string GetLine(bool ReplaceInvisibleChars);
		void PutLine(std::string S, bool ReplaceInvisibleChars);
	};

	//RecursiveDir
	//----------------------------
	typedef	void (*evOnNextFile)(LPVOID UserData, const std::string Dir, const WIN32_FIND_DATA& FileData, bool& Cancel, bool& Exclude);

	class RecursiveDir
	{
		volatile bool	_Cancel;
		bool		_Exclude;
		__int64	_Size;
		int			_Count;
		LPVOID	_UserData;
		WIN32_FIND_DATA	_FileData;
		retCode	_Error;

		void	ResetData();
		bool	GetFiles(std::string Directory);
	public:
		RecursiveDir(LPVOID UserData = NULL);
		__int64	Size() { return _Size; };
		int			Count() { return _Count; };
		bool		Get(std::string Directory);
		bool		Aborted() { return _Cancel; };

		evOnNextFile	fnEvent;
	};

}
