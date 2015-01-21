# include "stdafx.h"
# include "FileOperations.h"

BOOL AppendHeadToFile(CString filePath)
{
	OutputDebugString(_T("BaiYi#AppendHeadToFile"));
	_Longlong length = 0;
	vector<char> buf;
	char ch;

	ifstream iFile(filePath, ios::binary | ios::in);
	if (!iFile.is_open())
	{
		return FALSE;
	}
	while (!iFile.eof())
	{
		iFile.read(&ch, 1);
		buf.push_back(ch);
	}
	buf.pop_back();

	iFile.close();
	//------------------------------------------------
	CString headFilePath;
	headFilePath.Format(_T("%s.baiyi"), filePath);
	ofstream oFile(filePath, ios::binary | ios::out | ios::trunc);
	ifstream iHFile(headFilePath, ios::binary | ios::in);
	if (!oFile.is_open() || !iHFile.is_open())
	{
		return FALSE;
	}

	//oFile << "xiangmu";//头部添加信息
	//读取头
	char headBuf[HEADSIZE];
	iHFile.read(headBuf, HEADSIZE);//读取头
	iHFile.close();

	oFile.write(headBuf, HEADSIZE);//写入头
	oFile.seekp(HEADSIZE, ios::beg);

	vector<char>::iterator begin, end;
	for (begin = buf.begin(), end = buf.end(); begin < end; begin++)
	{
		oFile << *begin;
	}
	oFile.close();
	return TRUE;
}

BOOL RemoveHeadToFile(CString filePath)
{
	OutputDebugString(_T("BaiYi#RemoveHeadToFile"));
	_Longlong length = 0;
	vector<char> buf;
	char ch;

	//--------------------------------------
	CString headFilePath;
	headFilePath.Format(_T("%s.baiyi"), filePath);
	ifstream iFile(filePath, ios::binary | ios::in);
	ofstream oHFile(headFilePath, ios::binary | ios::out);
	if (!iFile.is_open())
	{
		OutputDebugString(_T("BaiYi#RemoveHeadToFile iFile.is_open() is FALSE"));
		return FALSE;
	}
	if (!oHFile.is_open())
	{
		OutputDebugString(_T("BaiYi#RemoveHeadToFile oHFile.is_open() is FALSE"));
		return FALSE;
	}

	//存储头
	//iFile.seekg(1024, ios::beg);//忽略头
	char headBuf[HEADSIZE];
	ZeroMemory(headBuf, HEADSIZE);
	iFile.read(headBuf, HEADSIZE);//读取头信息
	oHFile.write(headBuf, HEADSIZE);//将头信息写入到新的文件中
	oHFile.close();

	//读取可执行的word文件
	while (!iFile.eof())
	{
		iFile.read(&ch, 1);
		buf.push_back(ch);
	}
	buf.pop_back();//去除文件结束符

	iFile.close();

	//---------------------------------------
	ofstream oFile(filePath, ios::binary | ios::out);
	if (!oFile.is_open())
	{
		OutputDebugString(_T("BaiYi#RemoveHeadToFile oFile.is_open() is FALSE"));
		return FALSE;
	}
	vector<char>::iterator begin = buf.begin();
	vector<char>::iterator end = buf.end();
	for (; begin < end; begin++)
	{
		oFile << *begin;
	}

	oFile.close();
	return TRUE;
}
BOOL JudgeFileIsOk(CString filePath)
{
	OutputDebugString(L"BaiYi#执行JudgeFileIsOk");
	int ext = filePath.ReverseFind('.') + 1;
	int name = filePath.ReverseFind('\\') + 1;

	if (ext == -1 || name == -1)
	{
		OutputDebugString(L"BaiYi#filePath isn`t found!");
		return FALSE;
	}

	CString extFile = filePath.Mid(ext);
	CString nameFile = filePath.Mid(name);

	OutputDebugString(extFile);
	OutputDebugString(nameFile);

	if ((extFile.CompareNoCase(_T("doc")) == 0 ||
		extFile.CompareNoCase(_T("docx")) == 0 )&&
		nameFile.Left(2).CompareNoCase(_T("~$")) != 0
		)
	{
		return TRUE;
	}
	return FALSE;
}
BOOL JudgeFileHeadIsExist(CString filePath)
{
	OutputDebugString(L"BaiYi#执行JudgeFileHeadIsExist");
	filePath.Append(_T(".baiyi"));
	return PathFileExists(filePath);
}