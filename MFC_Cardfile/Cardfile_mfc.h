
// Cardfile_mfc.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CCardfilemfcApp:
// Сведения о реализации этого класса: Cardfile_mfc.cpp
//

class CCardfilemfcApp : public CWinApp
{
public:
	CCardfilemfcApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
		
		
};

extern CCardfilemfcApp theApp;
