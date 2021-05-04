#include "utility.h"

Private	int	g_nTokenCount;
Private	Tokens	g_Tokens[MAX_TOKEN_COUNT];

/**
	@brief	주어진 문자열을 delimiter로 나누어 버퍼에 집어 넣는다.

	@author	백흠경

	@param	pText = 주어진 문자열
	@param	delimiter = 나눌 delimiter

	@return	
*/
Public Tokens* Split( char* pText , char delimiter )
{
	char* lhs = NULL;
	char* rhs = NULL;
	Tokens* res = NULL;

	memset(&(g_Tokens[g_nTokenCount++]) , 0 , sizeof(Tokens));
	res = &(g_Tokens[g_nTokenCount - 1]);
	if(res)
	{
		res->nCount = 0;

		lhs = pText;
		rhs = strchr(lhs , delimiter);
		if(NULL != rhs)
		{
			strncpy(res->buf[(res->nCount)++] , lhs , rhs - lhs);
			while(NULL != rhs)
			{
				lhs = rhs + 1;
				rhs = strchr(lhs , delimiter);
				if(NULL != rhs)
				{
					strncpy(res->buf[(res->nCount)++] , lhs , rhs - lhs );
				}
				else if(NULL != lhs)
				{
					strcpy(res->buf[(res->nCount)++] , lhs );
				}
			}
		}
	}

	return res;
}

/**
	@brief	CSV형식의 문자열을 분리하여 버퍼에 집어 넣는다.

	@author	humkyung

	@date	2011.08.04

	@param	pText = 주어진 문자열

	@return	
*/
Public Tokens* SplitCSVString( char* pText )
{
	char* lhs = NULL;
	char* buffer = NULL;
	char* ptr = NULL;
	Tokens* res = NULL;
	int idx = 0;
	int flag = 0;

	memset(&(g_Tokens[g_nTokenCount++]) , 0 , sizeof(Tokens));
	res = &(g_Tokens[g_nTokenCount - 1]);
	if(res)
	{
		res->nCount = 0;
		buffer = res->buf[res->nCount];
		for(ptr = pText;(*ptr) != '\0';++ptr)
		{
			if(('"' == (*ptr)) && ('\0' != (*ptr)))
			{
				//! if field begins with "
				if(0 == idx)
				{
					flag = 0;
					while(0 == flag)
					{
						ptr++;
						while('"' != (*ptr))
						{
							buffer[idx++] = *ptr;
							ptr++;
						}

						if(('\0' != (*(ptr + 1))) && ('"' == (*(ptr + 1))))
						{
							buffer[idx++] = *(ptr + 1);
							ptr++;
						}
						else if(('\0' != (*(ptr + 1))) && (',' == (*(ptr + 1))))
						{
							flag = 1;
						}
						else
						{
							flag = 1;
						}
					}
				}
				//! up to here
				else
				{
					++ptr;
					buffer[idx++] = *ptr;
				}
			}
			else if(',' == (*ptr))
			{
				buffer = res->buf[++(res->nCount)];
				memset(buffer , '\0' , TOKEN_BUF_SIZ);
				idx = 0;
			}
			else if('"' != (*ptr))
			{
				buffer[idx++] = *ptr;
			}
		}
		if('\0' != res->buf[res->nCount][0]) (res->nCount)++;
	}

	return res;
}

/**
	@brief	set g_nTokenCount to zero
	@author	HumKyung Baek
	@return	
*/
Public void ResetTokens()
{
	g_nTokenCount = 0;
}

/**
	@brief

	@author	

	@date
*/
Public int GetTextJustify(char* sJustify)
{
	if (strcmp(sJustify, "Left Top") == 0)
		return TXTJUST_LT;
	else if ((strcmp(sJustify, "Left Center") == 0) || (0 == strcmp(sJustify, "Left Center")))
		return TXTJUST_LC;
	else if (strcmp(sJustify, "Left Bottom") == 0)
		return TXTJUST_LB;
	else if (strcmp(sJustify, "Center Top") == 0)
		return TXTJUST_CT;
	else if (strcmp(sJustify, "Center Center") == 0)
		return TXTJUST_CC;
	else if (strcmp(sJustify, "Center Bottom") == 0)
		return TXTJUST_CB;
	else if (strcmp(sJustify, "Right Top") == 0)
		return TXTJUST_RT;
	else if (strcmp(sJustify, "Right Center") == 0)
		return TXTJUST_RC;
	else if (strcmp(sJustify, "Right Bottom") == 0)
		return TXTJUST_RB;

	return TXTJUST_LB;
}

/**
	@brief	level string을 파싱한다.

	@author	전호상,백흠경
*/
Public int GetLevel(char* sLayer)
{
	int i = 0,nLen = 0;

	if(strcmp(sLayer, "ANNOTATION") == 0)
		return 1;
	else
	{
		nLen = strlen( sLayer );
		for(i = 0;i < nLen;i++)
		{
			if(!isdigit( sLayer[i] ))
			{
				return 1;
			}
		}

		return atoi(sLayer);
	}

	return 1;
}

Public int GetFontNo(char* sFontname)
{
	int i = 0 , nLen = 0;

	///sFontname[strlen(sFontname)-1] = 0x00;
	if(strcmp(sFontname, "Standard") == 0)
		return 0;
	else
	{
		nLen = strlen( sFontname );
		for(i = 0;i < nLen;i++)
		{
			if(!isdigit( sFontname[i] ))
			{
				return 0;
			}
		}

		return atoi( sFontname );
	}

	return 0;
}

/**
	@brief	색상 문자열에서 색상 번호를 리턴한다.

	@author	전호상,백흠경

	@return	색상 번호
*/
Public int GetColorNo(char* sColor)
{
	int i = 0 , nLen = 0;

	if(strcmp(sColor, "White") == 0)
		return 0;
	else if(strcmp(sColor, "Yellow") == 0)
		return 4;
	else if(strcmp(sColor, "Red") == 0)
		return 3;
	else if(strcmp(sColor, "Green") == 0)
		return 2;
	else if(strcmp(sColor, "Cyan") == 0)
		return 7;
	else if(strcmp(sColor, "Blue") == 0)
		return 1;
	else if(strcmp(sColor, "Magenta") == 0)
		return 5;
	else
	{
		nLen = strlen( sColor );
		for(i = 0;i < nLen;i++)
		{
			if(!isdigit( sColor[i] ))
			{
				return 0;
			}
		}

		return atoi( sColor );
	}

	return 0;
}

/**
	@brief	Type 문자열에서 Type 번호를 리턴한다.

	@author	전호상,백흠경

	@return	색상 번호
*/
Public int GetLineType(char* sLineType)
{
	int i = 0 , nLen = 0;
	if(strcmp(sLineType, "ByLayer") == 0)
		return 0;
	else
	{
		nLen = strlen( sLineType );
		for(i = 0;i < nLen;i++)
		{
			if(!isdigit(sLineType[i]))
			{
				return 0;
			}
		}

		return atoi(sLineType);
	}

	return 0;
}

/**
	@brief	Weight 문자열에서 Weight 번호를 리턴한다.

	@author	전호상,백흠경

	@return	색상 번호
*/
Public int GetLineWeight(char* sLineWeight)
{
	int i = 0,nLen = 0;
	if(strcmp(sLineWeight, "Default") == 0)
		return 0;
	else
	{
		nLen = strlen( sLineWeight );
		for(i = 0;i < nLen;i++)
		{
			if(!isdigit(sLineWeight[i]))
			{
				return 1;
			}
		}

		return atoi(sLineWeight);
	}
	return 0;
}

/*----------------------------------------------------------------------+
| Utility routines |
+----------------------------------------------------------------------*/
// p는 원래의 점, angle 은 회전할 각도(라디안으로) 
Public Dpoint3d Rotate(Dpoint3d *p, double angle) 
{ 
	Dpoint3d newPoint; 

	newPoint.x = (p->x * cos(angle)) - (p->y * sin(angle)); 
	newPoint.y = (p->x * sin(angle)) + (p->y * cos(angle)); 

	return newPoint; 
} 

// 점을 평행이동하기 위한 함수 
Public Dpoint3d Move(Dpoint3d *p,Dpoint3d *offset) 
{ 
	Dpoint3d newPoint; 

	newPoint.x = p->x - -(offset->x); 
	newPoint.y = p->y - -(offset->y); 

	return newPoint;
} 

Public Dpoint3d GetIntPoint(Dpoint3d *p) 
{ 
	Dpoint3d iPoint; 

	iPoint.x = p->x; 
	iPoint.y = p->y; 

	iPoint.x += (p->x - iPoint.x > 0.5 ? 1 : 0); 
	iPoint.y += (p->y - iPoint.y > 0.5 ? 1 : 0); 

	return iPoint; 
} 

Public double GetRadian(double nDegree)
{
	return fc_pi/180.0*nDegree;
}

Public double GetDegree(int nRadian)
{
	return nRadian*180.0/fc_pi;
}

/**
	@brief	print errorMsg

	@author

	@date	????.??.??

	@return
*/
Public void PrintError(char *errorMsg)
{
	mdlDialog_dmsgsPrint(errorMsg);
}

Public void SetOneView()
{
	ULong mask[8];
	char r[8];
	int i;

	/* Attach the reference diagram to the background */
	if (r != '\0')
		mdlRefFile_attachCoincident(r,r,NULL,NULL,TRUE,TRUE);

	mdlView_turnOn(0);
	/* Turn off views from 2 to 8 */
	for ( i = 1; i <= 7; i++)
	{
		mask[i] = 0;
		mdlView_turnOff(i);
	}
	/* Fit the diagram into the view 1 */
	//mask[0] = 3;
	//mdlView_fit(0,mask);
	mdlView_updateSingle(0);
}