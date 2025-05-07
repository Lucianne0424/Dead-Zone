#pragma once
#include "Component.h"

enum class PERSPECTIVE_TYPE : uint8
{
	FIRST_PERSON,
	THIRD_PERSON,

	END
};

class MonoBehaviour : public Component
{
public:
	MonoBehaviour();
	virtual ~MonoBehaviour();

public:
	virtual void FinalUpdate() sealed { }

public:
	const wstring& GetName() { return _name; }

protected:
	wstring _name;
};

