#pragma once

namespace eclass{

	//����������������һ�����ƹ��캯��
	void Copy(void);

	LPVOID __stdcall TAlloc(int nSize);
	void __stdcall memcpy(void *dest, void *src, int size);

	LPVOID SetFunctionHookAddr(LPVOID lpSrcAddr, LPVOID lpHookAddr);

	extern DWORD m_pAccessorVfptr;

	void SetAccessorClass(DWORD **pNewClass);
}

//������������ṹ
#define IMP_NEWECLASS(LocalName,Object,Vfptr) \
	ECLASS_VFPTR LocalName##T, *LocalName##TT, **LocalName; \
	LocalName##T.dwVfTable = Vfptr; \
	LocalName##T.pObjMySelf = (LPVOID)Object; \
	LocalName##TT = &LocalName##T; \
	LocalName = &LocalName##TT;