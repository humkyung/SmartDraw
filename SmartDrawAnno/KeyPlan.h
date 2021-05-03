#pragma once

class CKeyPlan
{
	CKeyPlan(const CKeyPlan&){}
	CKeyPlan& operator=(const CKeyPlan&){return (*this);}
public:
	CKeyPlan(void);
	~CKeyPlan(void);

	int Write(OFSTREAM_T& ofile , const double& dBorderScale , const double& dDrawingScale , const STRING_T& sIniFilePath);
};
