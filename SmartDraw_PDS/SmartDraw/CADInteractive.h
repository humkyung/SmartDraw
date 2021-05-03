#pragma once

class CCADInteractive
{
public:
	CCADInteractive(void);
	static int GetInputPoint(double& x , double& y , const STRING_T& sBorderFilePath , const STRING_T& sCommandFilePath);
};
