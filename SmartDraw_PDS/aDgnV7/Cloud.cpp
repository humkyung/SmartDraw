#include <mdl.h>
#include <cexpr.h>
#include <rscdefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmdlist.h>
#include <dlogitem.h>
#include <system.h>
#include <global.h>
#include <userfnc.h>
#include <mselems.h>
#include <refernce.h>
#include <tcb.h>
#include <scanner.h>
#include <mdldim.h>
#include <dlogman.fdf>
#include <msdim.fdf>
#include <mdlerrs.h>
#include <clipdscr.h>
#include <msfile.fdf>

#include "Cloud.h"

/*----------------------------------------------------------------------+
|									                                    |
| name		getPolygonArea						                        |
|									                                    |
| author	BSI					6/91		                            |
|									                                    |
+----------------------------------------------------------------------*/
Private double getPolygonArea(Dpoint3d* pts , int nCount)
{
	int p = 0 , q = 0;
	double area = 0.0;

	for(p = nCount - 1,q = 0;q < nCount;q++)
	{
		area += pts[p].x*pts[q].y - pts[p].y*pts[q].x;
		p = q;
	}

	return area;
}

Private boolean get_AcrCoordinates(double Amin, double Amax, DPoint3d *startSegment, DPoint3d *endSegment, double *Pjx, double *Pjy, double *Rf, DPoint3d *center)
{
	double dRandumNo;
	double dSweepAngle;
	double Rmin;
	double Rmax;	
	double R;
	double L;
	double Rc;
	double deltaL;
	double BETA;
	double h;
	double Alpha;

	//get random number
	dRandumNo = (((rand() % 10)/10.0) + 0.05);
	//sweepAngle is 110 for all arc
	dSweepAngle = 110;	

	//get minimum and maximum radius of arc
	Rmin = Amin / (dSweepAngle * fc_pi / 180);
	Rmax = Amax / (dSweepAngle * fc_pi / 180);

	//get radius of arc which is between minimum and maximum range
	R = (dRandumNo * (Rmax - Rmin)) +  Rmin;

	//divide line into sub-line and calculate the length 
	L = 2 * R * sin((dSweepAngle/2) * (fc_pi / 180));
	Rc = L / 2;

	//get the radius of arc
	//Rf[0] = max(R, Rc);	
	if (R < Rc)
		Rf[0] = Rc;
	else
		Rf[0] = R;

	//calculate delta length
	deltaL = sqrt(((endSegment[0].y - startSegment[0].y) * (endSegment[0].y - startSegment[0].y)) + ((endSegment[0].x - startSegment[0].x) * (endSegment[0].x - startSegment[0].x)) );

	//calculate Beta angle between the sun-line
	BETA = atan2((endSegment[0].y - startSegment[0].y),(endSegment[0].x - startSegment[0].x));

	//calulate the Alpha angle
	Alpha = BETA - (fc_pi / 2);

	h = sqrt((Rf[0] * Rf[0]) - (Rc * Rc));

	//
	Pjx[0] = ((L/deltaL) * (endSegment[0].x - startSegment[0].x)) + startSegment[0].x;
	Pjy[0] = ((L/deltaL) * (endSegment[0].y - startSegment[0].y)) + startSegment[0].y;

	//using the alpha angle of sub-line calculate the center of the Arc
	center[0].x = (0.5 * (Pjx[0] + startSegment[0].x)) - (h * cos(Alpha));
	center[0].y = (0.5 * (Pjy[0] + startSegment[0].y)) - (h * sin(Alpha));

	return TRUE;
}

/**
	@brief	
	@author	
	@date	
	@return	boolean
*/
Public boolean Draw_Cloud(Dpoint3d *ptCloud, int nColor, int nLayer, int nStyle,  float fScale, int nWeight, float fWidth, int nLineCount, float fArcMin, float fArcMax)
{
	int i;
	DPoint3d arcPts[3],elmLine[2],ptsCenter;
	//	MSElement out;
	Dpoint3d *ptsMid;
	double dDistance = 0,a = 0, R = 0,dAngle,dHeight;
	int iRandomLen[6];
	double unitVal = tcb->uorpersub;
	char msg[256];

	///Added By Suchita
	int status;
	double dSweepAngle;
	double Amin;
	double Amax;
	double Rf;
	double deltaL;
	double Pjx = 0.0;
	double Pjy = 0.0;
	double dStartAngle;
	double dEndAngle;
	double dStartDist;
	double constDeltaL;

	MSElement	out;
	DPoint3d	startPoint;
	DPoint3d	endPoint;
	DPoint3d	startSegment;
	DPoint3d	endSegment;
	DPoint3d	center;
	MSElementUnion cellElm;
	MSElementDescr* cellElP;
	MSElementDescr* arcElP = NULL;

	double dRandumNo;
	double Rmin;
	double Rmax;	
	double L;
	double Rc;
	double BETA;
	double h;
	double Alpha;
	int mid = nLineCount * 0.5;
	Dpoint3d tmp;

	for( i = 0;i <= nLineCount ; i++)
	{
		ptCloud[i].x = ptCloud[i].x  * unitVal;
		ptCloud[i].y = ptCloud[i].y  * unitVal;
		ptCloud[i].z = fc_zero;
	}

	if(ptCloud && nLineCount > 2)
	{
		double area = getPolygonArea(ptCloud , nLineCount);
		if(area < 0.0)
		{
			/// reverse
			for(i = 0;i < mid;i++)
			{
				tmp = ptCloud[ nLineCount - i - 1];
				ptCloud[nLineCount - i - 1] = ptCloud[i];
				ptCloud[i] = tmp;
			}
		}
	}

	//Added By Suchita
	Amin = fArcMin * unitVal;
	Amax = fArcMax * unitVal;

	Pjx = ptCloud[0].x;
	Pjy = ptCloud[0].y;

	status = mdlCell_create (&cellElm , NULL, &ptCloud[0] , FALSE);
	status = mdlElmdscr_new (&cellElP, NULL, &cellElm);
	//End Addition

	for(i = 0 ; i<(nLineCount - 1); i++)
	{		
		//Added By Suchita
		startPoint.x = Pjx;
		startPoint.y = Pjy;		

		if (i+1 == nLineCount)
			endPoint = ptCloud[0];
		else
			endPoint = ptCloud[i+1];

		startSegment.x = Pjx;
		startSegment.y = Pjy;		
		endSegment.x = endPoint.x;
		endSegment.y = endPoint.y;

		//get the distance of the line
		constDeltaL = sqrt(((endSegment.y - startSegment.y) * (endSegment.y - startSegment.y)) + ((endSegment.x - startSegment.x) * (endSegment.x - startSegment.x)) );
		do
		{
			//function to calculate center point and radius of the arc
			get_AcrCoordinates(Amin, Amax, &startSegment, &endSegment, &Pjx, &Pjy, &Rf, &center);
			//get the length of the sub-line
			dStartDist = sqrt(((Pjy - startPoint.y) * (Pjy - startPoint.y)) + ((Pjx - startPoint.x) * (Pjx - startPoint.x))) ;

			//condition to find the last arc on the line
			if ((dStartDist - constDeltaL) > 0)
			{										
				dSweepAngle = 180;	
				if (i+1 == nLineCount)
				{
					Pjx = ptCloud[0].x;
					Pjy = ptCloud[0].y;
				}
				else
				{
					Pjx = ptCloud[i+1].x;
					Pjy = ptCloud[i+1].y;
				}

				center.x = (startSegment.x + Pjx) / 2;
				center.y = (startSegment.y + Pjy) / 2;

				Rf = sqrt ((Pjy - center.y) * (Pjy - center.y) + (Pjx - center.x) * (Pjx - center.x));		
			}
			else
				dSweepAngle = 110;	

			dEndAngle = dSweepAngle * 0.0174532925;
			dStartAngle = (atan2(startSegment.y - center.y, startSegment.x - center.x));

			if(Rf > 0)	/// Need Rf is greater then 0
			{
				//create arc element
				status = mdlArc_create (&out, NULL, &center, Rf, Rf, NULL, dStartAngle, dEndAngle); 
				mdlElement_setSymbology(&out, &nColor, &nWeight, &nStyle);
				mdlElement_setProperties(&out , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
				//create element descriptor of cell element
				status = mdlElmdscr_new (&arcElP, NULL, &out);
				//append arc element descriptor into cell element
				mdlElmdscr_appendDscr (cellElP, arcElP);
			}
			//set start of the next arc point to the end of previous arc point
			startSegment.x = Pjx;
			startSegment.y = Pjy;
		}while((dStartDist - constDeltaL) < 0);

		//End Addition By suchita
	}

	mdlElmdscr_add(cellElP);

	if (cellElP != NULL)
		mdlElmdscr_freeAll (&cellElP); 
	if (arcElP != NULL)
		mdlElmdscr_freeAll (&arcElP);

	return TRUE;
}