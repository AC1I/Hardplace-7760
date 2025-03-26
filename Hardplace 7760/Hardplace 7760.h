
// Hardplace 7760.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHardplace7760App:
// See Hardplace 7760.cpp for the implementation of this class
//

class CHardplace7760App : public CWinApp
{
public:
	CHardplace7760App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CHardplace7760App theApp;
