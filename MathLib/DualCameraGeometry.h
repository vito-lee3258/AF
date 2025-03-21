#ifndef __DUAL_CAMERA_GEOMETRY_H_
#define __DUAL_CAMERA_GEOMETRY_H_

#include "Matrix.h"
#include "Vector.h"

bool DLT_Homograph(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &H);
void Homograph2Rotation(CMatrix &H, CMatrix &R, bool bAvgS=true);

bool FundamentalMatrix_8Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F);
bool FundamentalMatrix_5Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F);	
bool FundamentalMatrix_3Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F);	//only scale, offsetY and rotation around Z
bool FundamentalMatrix_2Pt(float pMainXYList[][2], float pSecondXYList[][2], int nNum, CMatrix &F);	//only scale and offsetY

void Fundamental2Homograph2D(CMatrix &F, CMatrix &H);
bool Fundamental2RotationXY(CMatrix &F, CMatrix &R);

void DecomposeRotation(CMatrix &R, float &fAngleX, float &fAngleY, float &fAngleZ);
void Angle2Rx(float fAngleX, CMatrix &Rx);
void Angle2Ry(float fAngleY, CMatrix &Ry);
void Angle2Rz(float fAngleZ, CMatrix &Rz);

#endif
