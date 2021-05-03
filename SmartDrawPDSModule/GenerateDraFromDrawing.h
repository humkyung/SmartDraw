#pragma once

#ifdef ADRAW_PDSMODULE_EXPORTS
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllexport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern
#else
	#define ON_PDSMODULE_EXT_DLL		__declspec(dllimport)
	#define	ON_PDSMODULE_EXT_DECL		extern "C" __declspec(dllexport)
	#define ON_PDSMODULE_EXTERN_DECL	extern __declspec(dllimport)
#endif

#include <IsTools.h>
#include <aDraw_View.h>

class ON_PDSMODULE_EXT_DLL CGenerateDraFromDrawing
{
public:
	CGenerateDraFromDrawing(const double& dProjectUOR=80.0);
	~CGenerateDraFromDrawing(void);
public:
	int Generate(const STRING_T& sOutDraFilePath , const STRING_T& sDrawingFilePath, const STRING_T& sDatabaseFilePath);
private:
	CaDraw_View::ROTATE_T GetRotFrom( const double& dAngle );
	int GetViewDirAndRot(CaDraw_View::DIRECTIONT_T& dir , CaDraw_View::ROTATE_T& rot , const STRING_T& sViewDirection , double* pTransMatrix);
	CIsPoint3d GetViewOrigin(const CaDraw_View::DIRECTIONT_T& dir , const CaDraw_View::ROTATE_T& rot , 
		const double& scale , const CIsVolume& vol , const CIsPoint3d ptDestOrigin);

	double m_dProjectUOR;
};
