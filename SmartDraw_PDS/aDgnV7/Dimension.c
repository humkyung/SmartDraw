#include <stdio.h>
#include <tcb.h>
#include <mdldim.h>
#include <msdim.fdf>
#include "aDgnV7.h"
#include "Dimension.h"

Private boolean Draw_Dimension(Dpoint3d *ptStart, Dpoint3d *ptEnd, Dpoint3d *ptLoc, int nColor, int nLayer, int nStyle, float fScale, int nWeight, char *sDirection, char *sTxtDirc , char *pDimStyle);

DimStyle* _DIM_STYLES_ = NULL;

/**
	@brief	DIM STYLE NAME을 가지고 DimStyle을 찾는다.
	@author	HumKyung.Baek
	@param	pDimStyleName = 
	@return	DimStyle*
*/
DimStyle* Find_DimStyle( char* pDimStyleName )
{
	DimStyle* ptr = NULL;
	char szBuf[128];
	char txtString[256];

	if(NULL == _DIM_STYLES_) return NULL;
	
	for(ptr = _DIM_STYLES_;ptr;ptr = ptr->next)
	{
		if(0 == strcmpi(ptr->szName , pDimStyleName))
		{	
			return ptr;
		}	
	}

	return NULL;
}

/**
	@brief	list에 DimStyle을 추가한다.
	@author	백흠경
	@param	pDimStyle
	@return
*/
Private int Add_DimStyle( DimStyle* pDimStyle )
{
	DimStyle* ptr = NULL;

	if(NULL != pDimStyle)
	{
		if(NULL == _DIM_STYLES_)
		{
			_DIM_STYLES_ = pDimStyle;
			return SUCCESS;
		}

		for(ptr = _DIM_STYLES_;ptr && ptr->next;ptr = ptr->next);
		if(ptr) ptr->next = pDimStyle;

		return SUCCESS;
	}

	return FALSE;
}

/**
	@brief	DimStyle을 파싱한다.
	@author	HumKyung.Baek
	@param	pTokens
	@return	DimStyle*
*/
DimStyle* Parse_DimStyle(Tokens* pTokens)
{
	Tokens *pToken1 = NULL;
	DimStyle* pDimStyle = NULL;

	pToken1 = SplitCSVString( pTokens->buf[1] );
	if(pToken1 && ((17 == pToken1->nCount) || (18 == pToken1->nCount)))
	{
		pDimStyle = Find_DimStyle( pToken1->buf[0] );
		if(NULL != pDimStyle )
		{
			/// 기존의 DIM STYLE의 내용을 수정한다.
			pToken1->buf[0];
			pDimStyle->nFontName        = atoi( pToken1->buf[1] );
			pDimStyle->dTextHeight      = atof( pToken1->buf[2] );
			pDimStyle->dTextWidth       = atof( pToken1->buf[3] );
			pDimStyle->nTextWeight      = atof(pToken1->buf[4]); 
			pDimStyle->nTextLevel       = atof(pToken1->buf[5]); 
			pDimStyle->nTextColor       = GetColorNo( pToken1->buf[6] );
			pDimStyle->fTextOffset      = atof( pToken1->buf[7] );
			pDimStyle->nTextPrecision   = atoi(pToken1->buf[8]);
			pDimStyle->nDimLineColor	= atoi(pToken1->buf[9]);
			pDimStyle->fDimLineDistance	= atof(pToken1->buf[10]);
			pDimStyle->nExtLineColor    = GetColorNo( pToken1->buf[11] );
			pDimStyle->fExtLineOffset   = atof( pToken1->buf[12] ) * 1.0;
			pDimStyle->fExtLineExtension= atof( pToken1->buf[13] );
			strncpy(pDimStyle->szArrowType, pToken1->buf[14] , 16 );
			pDimStyle->fArrowWidth      = atof( pToken1->buf[15] );
			pDimStyle->fArrowHeight     = atof( pToken1->buf[16] );
			pDimStyle->bAutoSize = TRUE;
			if(18 == pToken1->nCount) pDimStyle->bAutoSize = strcmp(pToken1->buf[17] , "OFF");
		}
		else
		{
			pDimStyle = (DimStyle*)calloc(1 , sizeof(DimStyle));
			if(pDimStyle)
			{
				strncpy(pDimStyle->szName , pToken1->buf[0] , DIM_STYLE_NAME_SIZE);
				pDimStyle->nFontName        = atoi( pToken1->buf[1] );
				pDimStyle->dTextHeight      = atof( pToken1->buf[2] );
				pDimStyle->dTextWidth       = atof( pToken1->buf[3] );
				pDimStyle->nTextWeight      = atof(pToken1->buf[4]); 
				pDimStyle->nTextLevel       = atof(pToken1->buf[5]); 
				pDimStyle->nTextColor       = GetColorNo( pToken1->buf[6]);
				pDimStyle->fTextOffset      = atof( pToken1->buf[7]);
				pDimStyle->nTextPrecision   = atoi(pToken1->buf[8]);
				pDimStyle->nDimLineColor	= atoi(pToken1->buf[9]);
				pDimStyle->fDimLineDistance	= atof(pToken1->buf[10]);
				pDimStyle->nExtLineColor    = GetColorNo( pToken1->buf[11] );
				pDimStyle->fExtLineOffset   = atof( pToken1->buf[12]) * 1.0;
				pDimStyle->fExtLineExtension= atof( pToken1->buf[13] );
				strncpy(pDimStyle->szArrowType , pToken1->buf[14] , 16);
				pDimStyle->fArrowWidth      = atof( pToken1->buf[15]);
				pDimStyle->fArrowHeight     = atof( pToken1->buf[16]);
				pDimStyle->bAutoSize = TRUE;
				if(18 == pToken1->nCount) pDimStyle->bAutoSize = strcmp(pToken1->buf[17] , "OFF");

				Add_DimStyle( pDimStyle );
			}
		}
	}

	return pDimStyle;
}

/**
	@brief	Dimension을 파싱한다.
	@author	HumKyung.Baek
	@param	pTokens
	@return	DimStyle*
*/
Public int Parse_Dimension(Tokens* pTokens)
{
	Tokens *pToken1=NULL , *pToken2=NULL;
	char *sText, *sArrowType, *sDirection, *sTextDirection, *sCellLibPath , *pDimStyleName = NULL;
	Dpoint3d ptStart, ptEnd, ptLoc, ptCenter;
	int nColor, nLayer, nStyle, nWeight = 0, nJustify, nNumSide, Obliquing, i;
	double fScale = 0.;

	pToken1 = SplitCSVString( pTokens->buf[1] );
	if(pToken1 && (7 == pToken1->nCount))
	{
		ptStart.x = atof(pToken1->buf[0]);
		ptStart.y = atof(pToken1->buf[1]);
		ptStart.z = 0;
		
		ptEnd.x = atof(pToken1->buf[2]);
		ptEnd.y = atof(pToken1->buf[3]);
		ptEnd.z = 0;
		
		ptLoc.x = atof(pToken1->buf[4]);
		ptLoc.y = atof(pToken1->buf[5]);
		ptLoc.z = 0;
		
		fScale = atof(pToken1->buf[6]);
	}
	
	pToken2 = SplitCSVString( pTokens->buf[2] );
	if(pToken2 && (4 == pToken2->nCount))
	{				
		nLayer = GetLevel(pToken2->buf[0]);
		sDirection = pToken2->buf[1];
		sTextDirection = pToken2->buf[2];
		pDimStyleName = pToken2->buf[3];
	}
	
	if(Draw_Dimension(&ptStart, &ptEnd, &ptLoc, nColor, nLayer, nStyle, fScale, nWeight, sDirection, sTextDirection , pDimStyleName) == FALSE)
		PrintError("<DIMENSION> drawing fail");
	
	return SUCCESS;
}

/**
	@brief	DIMENSION을 생성한다.
	@author	HoSang.Jeon,HumKyung.Baek
	@date	????.??.??
*/
Private boolean Draw_Dimension(Dpoint3d *ptStart, Dpoint3d *ptEnd, Dpoint3d *ptLoc, int nColor, int nLayer, int nStyle, float fScale, int nWeight, char *sDirection, char *sTxtDirc , char *pDimStyleName)
{
	MSElementUnion   dim;

	RotMatrix    rMatrix;
	AssocPoint   assocPoint;
	DimParamText dimText;
	DimParamGeometry dimGeo;
	DimParamFormat   dimParam;
	DimParamMisc     dimMisc;
	DimParamFlags    dimFlags;
	DimParamTemplate dimTemplate;
	Dpoint3d ptLine1[2];
	MSElementUnion newLine1; 
	Dpoint3d ptLine2[2];
	MSElementUnion newLine2;
	DimStrings dimStrings;
	DimStringConfig config;

	DPoint3d     xVector, yVector, zVector, yPoint , dVec[2] , normVec , intsecPoint , perpendicularVector;
	double height, margin , distance , factor,dDist = 0;
	DVector3d vector1 , vector2;
	DimStyle *pDimStyle = NULL;
	char szBuf[512];
	char msg[256];
	double unitVal = tcb->uorpersub;

	ptStart->x = ptStart->x * unitVal;
	ptStart->y = ptStart->y * unitVal;
	ptStart->z = 0;
	ptEnd->x   = ptEnd->x   * unitVal;
	ptEnd->y   = ptEnd->y   * unitVal;
	ptEnd->z   = 0;
	ptLoc->x   = ptLoc->x   * unitVal;
	ptLoc->y   = ptLoc->y   * unitVal;
	ptLoc->z   = 0;
	
	ptLine1[0].x = ptEnd->x;
	ptLine1[0].y = ptEnd->y;
	ptLine1[0].z = ptEnd->z;

	ptLine1[1].x = ptLoc->x;
	ptLine1[1].y = ptLoc->y;
	ptLine1[1].z = ptLoc->z;
	

	/*-------------------------------------------------------------------
	If lineTag IS NOT zero, it was set by writeBaseElement() and we need
	to create and insert associative points.
	If lineTag IS zero, there is no tag for associations and we need to
	insert standard points into the dimension.
	
	  The required points for a size dimension are:
	  0 - Base of first witness line
	  1 - Base of second witness line
	-------------------------------------------------------------------*/
	if (mdlDim_create (&dim, NULL, NULL, DIMTYPE_SIZE_ARROW, 0))
		return (ERROR);
	{
		if(strcmpi(sDirection,"HOR") == 0)
		{
			if (strcmp(sTxtDirc,"UR") == 0)
			{
				//ptLoc->y += (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptEnd->y = ptLoc->y - (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptStart->y = ptLoc->y - (mdlVec_distance(ptLoc,ptEnd) * 0.1);
			}
			else if (strcmp(sTxtDirc,"DR") == 0)
			{
				//ptLoc->y -= (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptEnd->y = ptLoc->y + (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptStart->y = ptLoc->y + (mdlVec_distance(ptLoc,ptEnd) * 0.1);
			}
		}
		
		if(strcmpi(sDirection,"VER") == 0)
		{
			if (strcmp(sTxtDirc,"UR") == 0)
			{
				//ptLoc->x -= (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptEnd->x = ptLoc->x + (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptStart->x = ptLoc->x + (mdlVec_distance(ptLoc,ptEnd) * 0.1);
			}
			else if (strcmp(sTxtDirc,"DR") == 0)
			{
				//ptLoc->y += (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptEnd->x = ptLoc->x - (mdlVec_distance(ptLoc,ptEnd) * 0.1);
				ptStart->x = ptLoc->x - (mdlVec_distance(ptLoc,ptEnd) * 0.1);
			}
		}
		mdlDim_insertPoint (&dim, ptStart , -1, POINT_STD);
		mdlDim_insertPoint (&dim, ptEnd   , -1  , POINT_STD);
	}

	/*mdlDim_getStrings(&dimStrings , &config , &dim , 0);
	if(DIMSTRING_DUAL == config.dual)
	{
		dimStrings.secondary.upper
		mdlDim_setStrings(&dim , 0 , dimStrings);
	}*/

	/*-------------------------------------------------------------------
	We want the dimension to follow the line in a logical manner and we
	also want it to react properly to the DIMENSION AXIS (alignment)
	setting. So, rather than using the inverse of the view rotation matrix
	as the dimension rotation matrix, use mdlDim_defineRotMatrix(). To do
	this we need a third point, on the dimension Y axis (witness line).
	Use the two points of the line to define an X axis. Use the Z vector
	from the current view to define the Z axis. Then take the cross
	product of the Z and X to determine the Y axis. 
	-------------------------------------------------------------------*/

	mdlVec_computeNormal (&xVector, ptStart , ptEnd);
	
	dVec[0].x = ptEnd->x - ptStart->x;
	dVec[0].y = ptEnd->y - ptStart->y;
	dVec[0].z = 0;
	dVec[1].x = ptLoc->x - ptStart->x;
	dVec[1].y = ptLoc->y - ptStart->y;
	dVec[1].z = 0;
	
	if(0 == strncmp( sDirection, "Hor", 3))
	{
		perpendicularVector.x = 0;
		if(dVec[1].y < 0.)
		{
			perpendicularVector.y = -1;
		}else	perpendicularVector.y = 1;
		perpendicularVector.z = 0;
	}
	else if(0 == strncmp( sDirection, "Ver", 3))
	{
		if(dVec[1].x < 0.)
		{
			perpendicularVector.x = -1;
		}else	perpendicularVector.x = 1;
		perpendicularVector.y = 0;
		perpendicularVector.z = 0;
	}

	vector1.org = (*ptStart);
	vector1.end.x = ptStart->x - perpendicularVector.y;
	vector1.end.y = ptStart->y + perpendicularVector.x;
	vector2.org = (*ptLoc);
	vector2.end.x = ptLoc->x + perpendicularVector.x;
	vector2.end.y = ptLoc->y + perpendicularVector.y;
	vector2.end.z = 0;

	if(SUCCESS == mdlVec_intersect(&intsecPoint , &vector1 , &vector2))
	{
		distance = mdlVec_distance(&intsecPoint , ptLoc);
	}else	distance = 1.;
	
	mdlView_getParameters (NULL, NULL, NULL, &rMatrix, NULL, 0);
	mdlRMatrix_getRowVector (&zVector, &rMatrix, 2);
	zVector.x = 0;
	zVector.y = 0;
	zVector.z = 1;
	
	yVector.x = ptLoc->x - intsecPoint.x;
	yVector.y = ptLoc->y - intsecPoint.y;
	yVector.z = 0;

	/*-------------------------------------------------------------------
	Create a point on the dimension Y axis by projecting any distance
	out the Y vector defined above. Pass this point and the line points
	to mdlDim_defineRotMatrix. The proper rotation matrix is created and
	inserted into the dimension and a copy is returned in rMatrix;
	-------------------------------------------------------------------*/

	mdlVec_projectPoint (&yPoint, ptStart, &yVector, fc_1);
	mdlDim_defineRotMatrix (&rMatrix, &dim, ptStart, &yPoint, ptEnd);
	/*-------------------------------------------------------------------
	Now set the height so that the dimension does not interfere with the
	line. Make the shortest witness line at least three times the length
	of the dimension text height.
	-------------------------------------------------------------------*/

	mdlDim_getParam(&dimText, &dim, DIMPARAM_TEXT);
	margin = dimText.height * 160.0;
	if(pDimStyleName)
	{
		pDimStyle = Find_DimStyle( pDimStyleName );
		if(pDimStyle)
		{
			mdlElement_setSymbology(&dim, &(pDimStyle->nDimLineColor) , &nWeight , NULL);
			mdlElement_setProperties(&dim , &(pDimStyle->nTextLevel) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);

			factor = pDimStyle->dTextWidth / pDimStyle->dTextHeight;
			dimText.height   = pDimStyle->dTextHeight * unitVal;
			dimText.width    = pDimStyle->dTextWidth  * unitVal;
			dimText.font     = pDimStyle->nFontName;
			dimText.color    = pDimStyle->nTextColor;
			dimText.useColor = TRUE;
			dimText.weight   = pDimStyle->nTextWeight;
			dimText.useWeight= TRUE;
			dDist = mdlVec_distance(ptStart,ptEnd);
			if((strcmp(sTxtDirc,"UR") == 0 || strcmp(sTxtDirc,"DR") == 0) && (TRUE == pDimStyle->bAutoSize) && (dimText.height > dDist))
			{
				dimText.height *= 0.5;
				dimText.width  *= 0.5;
			}
			mdlDim_setParam( &dim , &dimText , DIMPARAM_TEXT );
			
			mdlDim_getParam(&dimGeo , &dim , DIMPARAM_GEOMETRY);
			dimGeo.witOffset  = pDimStyle->fExtLineOffset * unitVal;
			dimGeo.witExtend  = pDimStyle->fExtLineExtension * unitVal;
			///Suhas
			if(strcmp(sTxtDirc,"U") == 0)
			{
				dimGeo.textLift   = pDimStyle->fTextOffset * unitVal;
			}
			else if(strcmp(sTxtDirc,"D") == 0)
			{
				dimGeo.textLift =(0 - dimText.height) - (pDimStyle->fTextOffset * unitVal);
			}
			else if(strcmp(sTxtDirc,"UR") == 0 || strcmp(sTxtDirc,"DR") == 0)
			{
				dDist = mdlVec_distance(ptStart,ptEnd);
				if(strcmpi(sDirection,"VER") == 0)
				{
					if(ptStart->y > ptEnd->y)
						dimGeo.textLift = (dDist/2) - dimText.height/2;
					else
						dimGeo.textLift = -(dDist/2)- dimText.height/2;
				}
				if(strcmpi(sDirection,"HOR") == 0)
				{
					if(ptStart->x > ptEnd->x)
						dimGeo.textLift = -(dDist/2) - dimText.height/2;
					else
						dimGeo.textLift = (dDist/2) - dimText.height/2;
				}
			}
			///up to here

			dimGeo.textMargin = 0 * unitVal ;
			dimGeo.termWidth  = pDimStyle->fArrowWidth * unitVal;
			if((0 == strcmp("DOT" , pDimStyle->szArrowType)) || (0 == strcmp("DOTSMALL" , pDimStyle->szArrowType)))	/// dot
			{
				dimGeo.termHeight = (pDimStyle->fArrowWidth) * unitVal;
			}
			else
			{
				dimGeo.termHeight = (pDimStyle->fArrowWidth / 3.) * unitVal;
			}
			mdlDim_setParam( &dim , &dimGeo , DIMPARAM_GEOMETRY );
			
			mdlDim_getParam(&dimParam , &dim , DIMPARAM_FORMAT);
			dimParam.scale		 = tcb->subpermast * fScale ;	/* dimension scale을 적용시킨다. */
			dimParam.metric          = TRUE;
			dimParam.englishAccuracy = DIMACC_0;
			dimParam.metricAccuracy  = DIMACC_0;
			dimParam.showSubunits = FALSE;
			dimParam.showUnits = FALSE;
			mdlDim_setParam(&dim , &dimParam , DIMPARAM_FORMAT);

			mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
			//dimFlags.boxText = TRUE;
			dimFlags.horizontal = TRUE;
			mdlDim_setParam(&dim , &dimFlags , DIMPARAM_FLAGS);

			mdlDim_getParam( &dimMisc , &dim , DIMPARAM_MISC );
			dimMisc.extensionColor = pDimStyle->nExtLineColor;
			dimMisc.noAutoTextLift = TRUE;
			///Suhas : Added code to set dimension line inside.
			dimMisc.termMode = 2;
			dimMisc.verticalText = 0;	/// Standard
			if(strcmp(sTxtDirc,"UR") == 0 || strcmp(sTxtDirc,"DR") == 0)
			{
				dimMisc.verticalText = 1;
			}
			///up to here
			mdlDim_setParam( &dim , &dimMisc , DIMPARAM_MISC );
			
			if(0 == strcmp("NONE" , pDimStyle->szArrowType))	/// NONE
			{
				mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
				dimTemplate.firstTerm = 0;
				dimTemplate.leftTerm  = 0;
				dimTemplate.rightTerm = 0;
				dimTemplate.jointTerm = 0;
				mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
			}
			else if(0 == strcmp("CLOSE" , pDimStyle->szArrowType))	/// FILLED ARROW
			{
				mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
				dimFlags.arrowhead = 2;
				dimFlags.horizontal = FALSE;
				dimFlags.aec = 0;
				dimFlags.embedded = FALSE;
				mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );

				mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
				dimTemplate.firstTerm = 1;
				dimTemplate.leftTerm  = 1;
				dimTemplate.rightTerm = 1;
				dimTemplate.jointTerm = 1;
				mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
			}
			else if(0 == strcmp("OPEN30" , pDimStyle->szArrowType)) /// ARROW
			{
				mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
				dimFlags.horizontal = FALSE;
				dimFlags.arrowhead = 0;
				dimFlags.aec = 0;
				dimFlags.embedded = FALSE;
				mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );
				
				mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
				dimTemplate.firstTerm = 1;
				dimTemplate.leftTerm  = 1;
				dimTemplate.rightTerm = 1;
				dimTemplate.jointTerm = 1;
				mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
			}
			else if((0 == strcmp("HOLLOWSMALL" , pDimStyle->szArrowType)) || (0 == strcmp("DOTSMALL" , pDimStyle->szArrowType))) /// DOT
			{
				mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
				dimFlags.horizontal = FALSE;
				dimFlags.arrowhead = 2;
				dimFlags.aec = 0;
				dimFlags.embedded = FALSE;
				mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );
				
				mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
				dimTemplate.firstTerm = 4;
				dimTemplate.leftTerm  = 4;
				dimTemplate.rightTerm = 4;
				dimTemplate.jointTerm = 4;
				mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
			}
			else if(0 == strcmp("SLASH" , pDimStyle->szArrowType)) /// STROKE
			{
				mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
				dimFlags.horizontal = FALSE;
				dimFlags.arrowhead = 0;
				dimFlags.aec = 0;
				dimFlags.embedded = FALSE;
				mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );

				mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
				dimTemplate.firstTerm = 2;
				dimTemplate.leftTerm  = 2;
				dimTemplate.rightTerm = 2;
				dimTemplate.jointTerm = 2;
				mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
			}
		}
	}
	else
	{
		mdlDialog_dmsgsPrint("Can't find dimension style");
		return FALSE;
	}
	
	/*mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
	dimTemplate.ExtLineOffset    = 5 * -1.0;
	mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );*/
	/*-------------------------------------------------------------------
	Find the distance along the dimension Y axis between the two end
	points of the line.	Use the distance + "margin" as the dimension height
	making sure that the minimum is "margin".
	-------------------------------------------------------------------*/

	mdlRMatrix_getColumnVector (&yVector, &rMatrix, 1);
	mdlVec_subtractPoint (&yPoint, ptEnd , ptStart);
	height = mdlVec_dotProduct (&yVector, &ptLoc) + margin;
	if (height <= margin)
		height = margin;
	
	mdlDim_setHeight (&dim, distance , 1);
		
	mdlDim_validate(&dim); 
	
	mdlElement_setProperties(&dim , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
	mdlElement_display(&dim, NORMALDRAW); 

	mdlElement_add(&dim); 

	/*if(strcmpi(sDirection,"HOR") == 0)
	{
		if (strcmp(sTxtDirc,"UR") == 0 || strcmp(sTxtDirc,"DR") == 0)
		{		
			mdlLine_create(&newLine1, NULL, ptLine1);
			mdlElement_setSymbology(&newLine1, &(pDimStyle->nExtLineColor) , &nWeight , NULL);
			mdlElement_display(&newLine1, NORMALDRAW); 
			mdlElement_add(&newLine1);
		}
	}

	if(strcmpi(sDirection,"VER") == 0)
	{	
		if (strcmp(sTxtDirc,"UR") == 0 || strcmp(sTxtDirc,"DR") == 0)
		{
			mdlLine_create(&newLine1, NULL, ptLine1);
			mdlElement_setSymbology(&newLine1, &(pDimStyle->nExtLineColor) , &nWeight , NULL);			
			mdlElement_display(&newLine1, NORMALDRAW); 
			mdlElement_add(&newLine1);
		}
	}*/

	return (TRUE);
}

/**
	@brief	create a angular dimension

	@author	Suhas

	@date	????.??.??

	@return	boolean
*/
Public boolean Draw_DimAngular(Dpoint3d *ptCenter,Dpoint3d *ptStart, Dpoint3d *ptEnd, int nLayer, char *pDimStyleName)
{
	DimStyle *pDimStyle = NULL;
	DimParamGeometry dimGeo;
	double unitVal = tcb->uorpersub,factor;
	DimParamText dimText;
	MSElementUnion   dim;
	DimParamFlags    dimFlags;
	DimParamTemplate dimTemplate;
	char msg[256];

	ptStart->x = ptStart->x * unitVal;
	ptStart->y = ptStart->y * unitVal;
	ptStart->z = 0;

	ptEnd->x   = ptEnd->x * unitVal;
	ptEnd->y   = ptEnd->y * unitVal;
	ptEnd->z   = 0;

	ptCenter->x   = ptCenter->x  * unitVal;
	ptCenter->y   = ptCenter->y  * unitVal;
	ptCenter->z   = 0;


	if(pDimStyleName)
	{
		pDimStyle = Find_DimStyle( pDimStyleName );
		if(pDimStyle)
		{

			pDimStyle->fExtLineOffset = pDimStyle->fExtLineOffset * -1.0;	
			//if (mdlDim_create (&dim, NULL, NULL, DIMTYPE_ANGLE_LINES, 0) != SUCCESS)
			if (mdlDim_create (&dim, NULL, NULL, DIMTYPE_ANGLE_SIZE, 0) != SUCCESS)
			{
				PrintError("Error creating graphic element!");
				return FALSE;
			}
			else
			{
				//Added By Suchita
				mdlDim_getParam(&dimText, &dim, DIMPARAM_TEXT);	

				mdlElement_setSymbology(&dim, &(pDimStyle->nExtLineColor) , NULL , NULL);

				factor = pDimStyle->dTextHeight / dimText.height;
				dimText.height   = dimText.height * factor * unitVal;
				dimText.width    = dimText.width  * factor * unitVal;
				dimText.font     = pDimStyle->nFontName;
				dimText.color    = pDimStyle->nTextColor;
				dimText.useColor = TRUE;
				//dimText.useWeight= FALSE;
				dimText.useWeight= pDimStyle->nTextWeight;
				mdlDim_setParam(&dim , &dimText , DIMPARAM_TEXT );

				//added by suchita
				mdlDim_getParam(&dimGeo , &dim , DIMPARAM_GEOMETRY);
				dimGeo.witOffset  = pDimStyle->fExtLineOffset * unitVal;
				dimGeo.witExtend  = pDimStyle->fExtLineExtension * unitVal;
				sprintf(msg, "dimGeo.witOffset = %f", dimGeo.witOffset);

				//dimGeo.textMargin = 0 * unitVal ;
				dimGeo.textMargin = 0.5 * unitVal ;
				dimGeo.termWidth  = pDimStyle->fArrowWidth * unitVal;
				dimGeo.termHeight = (pDimStyle->fArrowWidth / 3.) * unitVal;
				mdlDim_setParam( &dim , &dimGeo , DIMPARAM_GEOMETRY );
				//********
				if(0 == strcmp("NONE" , pDimStyle->szArrowType))	/// NONE
				{
					mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
					dimTemplate.firstTerm = 0;
					dimTemplate.leftTerm  = 0;
					dimTemplate.rightTerm = 0;
					dimTemplate.jointTerm = 0;
					mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
				}
				else if(0 == strcmp("CLOSE" , pDimStyle->szArrowType))	/// FILLED ARROW
				{
					mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
					dimFlags.arrowhead = 2;
					dimFlags.horizontal = FALSE;
					dimFlags.aec = 0;
					dimFlags.embedded = FALSE;
					mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );

					mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
					dimTemplate.firstTerm = 1;
					dimTemplate.leftTerm  = 1;
					dimTemplate.rightTerm = 1;
					dimTemplate.jointTerm = 1;
					mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
				}
				else if(0 == strcmp("OPEN30" , pDimStyle->szArrowType)) /// ARROW
				{
					mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
					dimFlags.horizontal = FALSE;
					dimFlags.arrowhead = 0;
					dimFlags.aec = 0;
					dimFlags.embedded = FALSE;
					mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );

					mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
					dimTemplate.firstTerm = 1;
					dimTemplate.leftTerm  = 1;
					dimTemplate.rightTerm = 1;
					dimTemplate.jointTerm = 1;
					mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
				}
				else if((0 == strcmp("HOLLOWSMALL" , pDimStyle->szArrowType)) || (0 == strcmp("DOTSMALL" , pDimStyle->szArrowType))) /// DOT
				{
					mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
					dimFlags.horizontal = FALSE;
					dimFlags.arrowhead = 2;
					dimFlags.aec = 0;
					dimFlags.embedded = FALSE;
					mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );

					mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
					dimTemplate.firstTerm = 4;
					dimTemplate.leftTerm  = 4;
					dimTemplate.rightTerm = 4;
					dimTemplate.jointTerm = 4;
					mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
				}
				else if(0 == strcmp("SLASH" , pDimStyle->szArrowType)) /// STROKE
				{
					mdlDim_getParam( &dimFlags , &dim , DIMPARAM_FLAGS );
					dimFlags.horizontal = FALSE;
					dimFlags.arrowhead = 0;
					dimFlags.aec = 0;
					dimFlags.embedded = FALSE;
					mdlDim_setParam( &dim , &dimFlags , DIMPARAM_FLAGS );

					mdlDim_getParam( &dimTemplate , &dim , DIMPARAM_TEMPLATE );
					dimTemplate.firstTerm = 2;
					dimTemplate.leftTerm  = 2;
					dimTemplate.rightTerm = 2;
					dimTemplate.jointTerm = 2;
					mdlDim_setParam( &dim , &dimTemplate , DIMPARAM_TEMPLATE );
				}


				//**********
				//end addition
				mdlElement_setProperties(&dim , &(nLayer) , NULL , NULL , NULL , NULL , NULL , NULL , NULL);
				mdlDim_insertPoint (&dim, ptCenter , -1, POINT_STD);
				mdlDim_insertPoint (&dim, ptEnd, -1, POINT_STD);
				mdlDim_insertPoint (&dim, ptStart, -1, POINT_STD);
				//mdlDim_insertPoint (&dim, ptEnd, -1, POINT_STD);

				if( SUCCESS == mdlDim_validate(&dim))
				{
					mdlElement_add(&dim);
				}
				else
				{
					PrintError("Error creating graphic element!");
				}
			}
		}
	}

	return TRUE;
}
