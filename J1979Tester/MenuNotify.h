#pragma once
#include "framework.h"

class CMenuNotify : public INotifyUI
{
public:
	CMenuNotify(void);
	~CMenuNotify(void);
protected:
	void Notify(TNotifyUI& msg);
};

