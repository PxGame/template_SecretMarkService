# ifndef _FILEOPERATIONS_H_
# define _FILEOPERATIONS_H_

//����ļ�ͷ
BOOL AppendHeadToFile(CString filePath);
//�Ƴ��ļ�ͷ��������ͷ���ļ�
BOOL RemoveHeadToFile(CString filePath);
//�ж��ļ��Ƿ�Ϊָ�������ļ�
BOOL JudgeFileIsOk(CString filePath);
//�ж��ļ�ͷ�ļ��Ƿ����
BOOL JudgeFileHeadIsExist(CString filePath);


# endif