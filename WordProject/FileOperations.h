# ifndef _FILEOPERATIONS_H_
# define _FILEOPERATIONS_H_

//填加文件头
BOOL AppendHeadToFile(CString filePath);
//移除文件头，并保存头到文件
BOOL RemoveHeadToFile(CString filePath);
//判断文件是否为指定类型文件
BOOL JudgeFileIsOk(CString filePath);
//判断文件头文件是否存在
BOOL JudgeFileHeadIsExist(CString filePath);


# endif