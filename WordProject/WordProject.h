// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� WORDPROJECT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// WORDPROJECT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef WORDPROJECT_EXPORTS
#define WORDPROJECT_API __declspec(dllexport)
#else
#define WORDPROJECT_API __declspec(dllimport)
#endif

//// �����Ǵ� WordProject.dll ������
//class WORDPROJECT_API CWordProject {
//public:
//	CWordProject(void);
//	// TODO:  �ڴ�������ķ�����
//};
//
//extern WORDPROJECT_API int nWordProject;
//
//WORDPROJECT_API int fnWordProject(void);
