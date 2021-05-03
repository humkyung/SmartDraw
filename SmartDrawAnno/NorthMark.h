#pragma once

class CNorthMark
{
	CNorthMark(const CNorthMark&){}
	CNorthMark& operator=(const CNorthMark&){ return *(this); }
public:
	CNorthMark(const double& dAngle);
	~CNorthMark(void);

	int Write(OFSTREAM_T& ofile , const double& dBorderScale , const double& dDrawingScale , const STRING_T& sIniFilePath);
private:
	double m_dAngle;
};
