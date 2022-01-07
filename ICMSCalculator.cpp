// CICMSCalculator.cpp : implementation file
//
// 김천환 2006.02.22 하윤철 책임 코드로 교체
//#include "stdafx.h"
#include "ICMSCalculator.h"

#include <math.h>


//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : CICMSCalculator
// 설명          : 생성자
// 입력값        : 해당사항 없음
// 리턴값        : 해당사항 없음
// 비고          :
//***************************************************************************
CICMSCalculator::CICMSCalculator()
{
	//내용 없음.
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : ~CICMSCalculator
// 설명          : 소멸자
// 입력값        : 해당사항 없음
// 리턴값        : 해당사항 없음
// 비고          :
//***************************************************************************
CICMSCalculator::~CICMSCalculator()
{
	//내용 없음.
}

//하윤철 검토+
//계산한계 임계값 정의 
#define DISTANCE_TOLERANCE 0.000005 // 1cm

//반올림 처리 표준화  
//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : round
// 설명          : 반올림 처리
// 입력값        : [IN] dValue - 반올림 대상
// 입력값        : [IN] pres - 정확도
// 리턴값        : double - 반올림 결과
// 비고          :
//***************************************************************************
static double round(double dValue, int pres)
{
	if (pres < 0)
	{
		return dValue;
	}

	double dblTemp = dValue;
	double dblFactor = 1.;
	dblFactor *= ::pow(10., pres);

	dblTemp *= dblFactor;

	dblTemp = (dblTemp > 0) ? floor(dblTemp + 0.5) : ceil(dblTemp - 0.5);

	dblTemp /= dblFactor;
	return dblTemp;
}
//하윤철 검토-


//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 김천환
// 함수명        : GetBearingNDistance
// 설명          : 위경도 값을 방위 거리 값으로 변환
// 입력값        : [IN] dLat_1 : 시작 위도 (단위 : deg)
//				   [IN] dLon_1 : 시각 경도 (단위 : deg)
//				   [IN] dLat_2 : 끝 위도 (단위 : deg)
//				   [IN] dLon_2 : 끝 경도 (단위 : deg)
//				   [OUT] dBearing : 시작위치에서의 끝위치 방위각 (단위 : deg)
//				   [OUT] dDistance : 시작위치에서 끝위치까지 거리 (단위 : m)
// 리턴값        : void
// 비고          : 출력값을 Nautical mile에서 m로 수정
//***************************************************************************
void CICMSCalculator::GetBearingNDistance(double dLat_1, double dLon_1, double dLat_2, double dLon_2, double* dBearing, double* dDistance)
{
	/**
	경도를 0 ~ 360도 값으로 변환한다.
	위도를 -180 ~ 180도 값으로 변환한다.
	dDiffX = 두 경도 차이
	dDiffY = 두 위도 차이
	if(입력된 위도가 같으면)
	{
		dCourse = dDiffX > 0.0 ? 90.0 : 270.0;
		dPartial = WGS84_FF_EQ * Sin(dLat_2);
		dDist = RADIUS_G_NM * dDiffX * Cos(dLat_2) / sqrt(1 - dPartial * dPartial);
	}
	else if(입력된 경도가 같으면)
	{
		delta_m = meridional_dist(dLat_2) - meridional_dist(dLat_1);
		dDist =	fabs(delta_m);
		dCourse = dDiffY > 0.0 ? 0.0: 180.0;
	}
	else
	{
		delta_M = meridional_part(dLat_2) - meridional_part(dLat_1);
		delta_m = meridional_dist(dLat_2) - meridional_dist(dLat_1);
		dCourse	= Atan2(dDiffX, delta_M);
		dDist =	delta_m/Cos(dCourse);
	}

	dDist를 양수로 바꾼다.

	dDiffX	= dLon_2 - dLon_1;
	dPartial = Sin(dLat_2)*Sin(dLat_1)+Cos(dLat_2)*Cos(dLat_1)*Cos(dDiffX);
	g_distance = 60.0 * Acos(dPartial);

	dPartial = Cos(dLat_1)*Tan(dLat_2) - Sin(dLat_1)*Cos(dDiffX);
	angle =	fabs(Atan(Sin(dDiffX)/dPartial));

	dCourse를 0 ~ 180도 값으로 바꾼다.

	*dDistance 에 dDist * NM2M를 입력한다
	*dBearing 에 dCourse 값을 입력한다.
	**/

	double delta_m = 0., delta_M = 0.;
	double dDiffX = 0., dDiffY = 0.;
	double dPartial = 0., dCourse = 0., dDist = 0.;
	//calculate great circle distance and initial course
	double g_distance = 0., g_course = 0., angle = 0.;

	if (dLon_1 < 0.)
	{
		dLon_1 += 360.0;
	}

	if (dLon_2 < 0.)
	{
		dLon_2 += 360.0;
	}

	dDiffX = dLon_2 - dLon_1;
	if (dDiffX > 180.0)
	{
		dDiffX -= 360.0;
	}

	if (dDiffX < -180.0)
	{
		dDiffX += 360.0;
	}

	dDiffY = dLat_2 - dLat_1;

	//하윤철 검토+
		// 고질적인 부동소수점 오류때문에 의미있는 수준에서
		// 반올림해서 같은 값이라고 판단해야 함..
	dDiffY = round(dDiffY, 5);
	//하윤철 검토-

	//하윤철 검토+
	//	if(fabs(dLat_2 - dLat_1) < 0.0000001)
	if (fabs(dLat_2 - dLat_1) < DISTANCE_TOLERANCE)
		//하윤철 검토-
	{
		dCourse = dDiffX > 0.0 ? 90.0 : 270.0;
		dPartial = WGS84_FF_EQ * Sin(dLat_2);
		dDist = (RADIUS_G_NM * dDiffX * Cos(dLat_2)) / sqrt(1 - (dPartial * dPartial));
	}
	//하윤철 검토+
	//	else if(fabs(dLon_2 - dLon_1) < 0.0000001)
	else if (fabs(dLon_2 - dLon_1) < DISTANCE_TOLERANCE)
		//하윤철 검토-
	{
		delta_m = meridional_dist(dLat_2) - meridional_dist(dLat_1);
		dDist = fabs(delta_m);
		dCourse = dDiffY > 0.0 ? 0.0 : 180.0;
	}
	else
	{
		delta_M = meridional_part(dLat_2) - meridional_part(dLat_1);
		delta_m = meridional_dist(dLat_2) - meridional_dist(dLat_1);
		dCourse = Atan2(dDiffX, delta_M);
		if (dCourse < 0.0)
		{
			dCourse += 360;
		}
		dDist = delta_m / Cos(dCourse);
	}

	//하윤철 검토+
	dDist = round(dDist, 5);
	//하윤철 검토-

	if (dDist < 0)
	{
		dDist *= -1;
	}

	//하윤철 검토+
	if (dDist < DISTANCE_TOLERANCE)		dDist = 0;
	//하윤철 검토-

	dDiffX = dLon_2 - dLon_1;
	dPartial = Sin(dLat_2) * Sin(dLat_1) + Cos(dLat_2) * Cos(dLat_1) * Cos(dDiffX);
	g_distance = 60.0 * Acos(dPartial);

	dPartial = Cos(dLat_1) * Tan(dLat_2) - Sin(dLat_1) * Cos(dDiffX);
	angle = fabs(Atan(Sin(dDiffX) / dPartial));

	if (dCourse > 180)
	{
		g_course = 180.0 + angle;
	}
	else
	{
		g_course = 180.0 - angle;
	}


	//하윤철 검토+
	// m로 변환하는것이 일반적인지 ?
	//하윤철 검토-
	//	*dDistance = dDist;
	*dDistance = dDist * NM2M;	//Nautical mile에서 m로 수정 김천환	2006.02.22 
	*dBearing = dCourse;
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : GetBearing
// 설명          : 위경도 값을 방위 값으로 변환
// 입력값        : [IN] dLat_1 : 시작 위도 (단위 : deg)
//				   [IN] dLon_1 : 시각 경도 (단위 : deg)
//				   [IN] dLat_2 : 끝 위도 (단위 : deg)
//				   [IN] dLon_2 : 끝 경도 (단위 : deg)
// 리턴값        : 시작위치에서의 끝위치 방위각 (단위 : deg)
// 비고          :
//***************************************************************************
double CICMSCalculator::GetBearing(double dLat_1, double dLon_1, double dLat_2, double dLon_2)
{
	/**
	GetBearingNDistance() 함수를 호출한다.
	계산된 방위각을 반환한다.
	**/
	double dBearing;
	double dDistance;
	GetBearingNDistance(dLat_1, dLon_1, dLat_2, dLon_2, &dBearing, &dDistance);

	return dBearing;
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : GetDistance
// 설명          : 위경도 값을 방위 값으로 변환
// 입력값        : [IN] dLat_1 : 시작 위도 (단위 : deg)
//				   [IN] dLon_1 : 시각 경도 (단위 : deg)
//				   [IN] dLat_2 : 끝 위도 (단위 : deg)
//				   [IN] dLon_2 : 끝 경도 (단위 : deg)
// 리턴값        : 시작위치에서 끝위치까지 거리 (단위 : m)
// 비고          :
//***************************************************************************
double CICMSCalculator::GetDistance(double dLat_1, double dLon_1, double dLat_2, double dLon_2)
{
	/**
	GetBearingNDistance() 함수를 호출한다.
	계산된 거리를 반환한다.
	**/
	double dBearing;
	double dDistance;
	GetBearingNDistance(dLat_1, dLon_1, dLat_2, dLon_2, &dBearing, &dDistance);

	return dDistance;
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 김천환
// 함수명        : ChangeBearingNRangeToLatLong
// 설명          : 방위값 및 거리 값을 위경도로 변환
// 입력값        : [IN] dLat_1 : 시작 위도 (단위 : deg)
//				   [IN] dLon_1 : 시각 경도 (단위 : deg)
//				   [IN] dBearing : 시작위치에서의 끝위치 방위각 (단위 : deg)
//				   [IN] dDistance : 시작위치에서 끝위치까지 거리 (단위 : m)
//				   [OUT] dLat_2 : 끝 위도 (단위 : deg)
//				   [OUT] dLon_2 : 끝 경도 (단위 : deg)
// 리턴값        : void
// 비고          :
//***************************************************************************
void   CICMSCalculator::ChangeBearingNRangeToLatLong(double dLat_1, double dLon_1, double dBearing, double dDistance, double* dLat_2, double* dLon_2)
{

	/**
	dDistance를 Nautical mile로 변환 한다.

	dCourse 에 dBearing을 0 ~ 360도 값으로 변환하여 입력한다.
	dDist 에 dDistance을 입력한다.
	dPtval_X 에 dLon_1를 입력한다.

	dCourse를 0에서 360도 값으로 변환한다.

	if(dCourse 가 0도 이거나 180도 이면)
	{
		dPtval_Y = recurs(dLat_1, dCourse, dDist);
	}
	else if(dCourse가 90도 이거나 270도 이면)
	{
		dpartial	=	WGS84_FF_EQ * Sin(dLat_1);
		dDiffValue	=	dDist* sqrt(1 - dpartial * dpartial) / (RADIUS_G_NM * Cos(dLat_1));
		dPtval_X += (dCourse==90.0) ? dDiffValue : -dDiffValue;
		dPtval_Y = dLat_1;
	}
	else
	{
		dPtval_Y = recurs(dLat_1, dCourse, dDist);
		delta_M = meridional_part(dPtval_Y) - meridional_part(dLat_1);
		dDiffValue	= fabs(delta_M*Tan(dCourse));
		dPtval_X +=	(dCourse< 180.0) ? dDiffValue : -dDiffValue;
	}

	*dLon_2 에 dPtval_X를 입력한다.
	*dLat_2 에 dPtval_Y를 입력한다.
	**/

	//하윤철 검토+
	dDistance = round(dDistance, 5);
	dBearing = round(dBearing, 5);

	*dLat_2 = dLat_1;
	*dLon_2 = dLon_1;

	if (dDistance < DISTANCE_TOLERANCE)
	{
		return;// dPtFrom;
	}
	//이 계산식은 거리가 일정 수준이 되지 않으면 예상하지 못한 결과가 발생됨 
	//1cm 으로 범위 제한 
	//하윤철 검토-

	dDistance = dDistance * M2NM;	//m를 Nautical mile로 변환 // 김천환 추가 2006.02.22

	double dPtval_X = 0;
	double dPtval_Y = 0;


	double dDiffValue = 0., dCourse = 0.;
	double dpartial = 0., dDist = 0.;
	double delta_M = 0.;

	int C = 0;

	dCourse = dBearing;
	dDist = dDistance;
	dPtval_X = dLon_1;

	if ((dCourse < 0.0) || (dCourse >= 360.0))
	{
		C = (int)dCourse;
		dCourse = C % 360 + dCourse - C;
		if (dCourse < 0.0)
		{
			dCourse += 360.0;
		}
	}

	if (DOUBLE_EQUAL(dCourse, 0.0) || DOUBLE_EQUAL(dCourse, 180.0))
	{
		dPtval_Y = recurs(dLat_1, dCourse, dDist);
	}
	else if (DOUBLE_EQUAL(dCourse, 90.0) || DOUBLE_EQUAL(dCourse, 270.0))
	{
		dpartial = WGS84_FF_EQ * Sin(dLat_1);

		//		if(fabs(Cos(dLat_1)) < 0.0000000001)
		//하윤철 검토+
		//1cm 으로 범위 제한 
		//하윤철 검토-
		if (fabs(Cos(dLat_1)) < 0.00000001)
		{
			dDiffValue = dDist * sqrt(1 - dpartial * dpartial);
		}
		else
		{
			dDiffValue = dDist * sqrt(1 - dpartial * dpartial) / (RADIUS_G_NM * Cos(dLat_1));
		}

		dPtval_X += (DOUBLE_EQUAL(dCourse, 90.0)) ? dDiffValue : -dDiffValue;
		dPtval_Y = dLat_1;
	}
	else
	{
		dPtval_Y = recurs(dLat_1, dCourse, dDist);
		delta_M = meridional_part(dPtval_Y) - meridional_part(dLat_1);
		dDiffValue = fabs(delta_M * Tan(dCourse));
		dPtval_X += (dCourse < 180.0) ? dDiffValue : -dDiffValue;
	}

	*dLon_2 = dPtval_X;
	*dLat_2 = dPtval_Y;
	//	return dPtval;
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : recurs
// 설명          : 수식 #3
// 입력값        : [IN] FromLatt : 시작 위도 (단위 : deg)
//				 : [IN] dBearing : 방위각 (단위 : deg)
//				 : [IN] dDistance : 거리 (단위 : NM)
// 리턴값        : recurs() 함수 계산 결과
// 비고          :
//***************************************************************************
double CICMSCalculator::recurs(double FromLatt, double dBearing, double dDistance)
{
	/**
	dfrom = meridional_dist(FromLatt);
	dguess = dfrom + dDistance * Cos(dBearing);
	dlat = FromLatt + (dguess - dfrom) / 60.0;
	for(iteration = 0; iteration < 10; iteration++)
	{
		dpartial = meridional_dist(dlat);
		dlat = dlat + (dguess - dpartial) / 60.0;
	}
	return dlat;
	**/

	register int iteration;
	double dfrom = 0., dguess = 0., dpartial = 0., dlat = 0.;

	dfrom = meridional_dist(FromLatt);
	dguess = dfrom + dDistance * Cos(dBearing);
	dlat = FromLatt + (dguess - dfrom) / 60.0;
	for (iteration = 0; iteration < 10; iteration++)
	{
		dpartial = meridional_dist(dlat);
		dlat = dlat + (dguess - dpartial) / 60.0;
	}
	return dlat;
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : meridional_dist
// 설명          : 수식 #1
// 입력값        : [IN] lat - 위도 (단위 : deg)
// 리턴값        : meridional_dist() 함수 계산 결과
// 비고          :
//***************************************************************************
double CICMSCalculator::meridional_dist(double lat)
{
	/**
	e2 = WGS84_FF_EQ * WGS84_FF_EQ;
	e4 = e2 * e2;
	e6 = e4 * e2;
	double v1 = PI/180.0 * (1.0 - e2 / 4.0 - 3.0 * e4 / 64.0 - 5.0 * e6 / 256.0);
	double v2 = 3.0 /8.0 * (e2 - e4 / 4.0 + 15.0 * e6 / 128.0);
	double v3 = 15.0 / 256.0 * (e4 + 3.0 * e6 / 4.0);
	double v4 = RADIUS_G / 1852.0;
	retrun v4 * (v1 * lat - v2 * Sin(2.0 * lat) + v3 * Sin(4.0 * lat));
	**/

	double md, e2, e4, e6;

	e2 = WGS84_FF_EQ * WGS84_FF_EQ;
	e4 = e2 * e2;
	e6 = e4 * e2;
	double v1 = PI / 180.0 * (1.0 - e2 / 4.0 - 3.0 * e4 / 64.0 - 5.0 * e6 / 256.0);
	double v2 = 3.0 / 8.0 * (e2 - e4 / 4.0 + 15.0 * e6 / 128.0);
	double v3 = 15.0 / 256.0 * (e4 + 3.0 * e6 / 4.0);
	double v4 = RADIUS_G / 1852.0;
	md = v4 * (v1 * lat - v2 * Sin(2.0 * lat) + v3 * Sin(4.0 * lat));

	return(md);
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 하윤철
// 함수명        : meridional_part
// 설명          : 수식 #2
// 입력값        : [IN] lat - 위도 (단위 : deg)
// 리턴값        : meridional_part() 함수 계산 결과
// 비고          :
//***************************************************************************
double CICMSCalculator::meridional_part(double lat)
{
	/**
	return TODEG(log(Tan(45.0 + lat / 2.0)) - 0.5 * WGS84_FF_EQ * log((1 + WGS84_FF_EQ * Sin(lat)) / (1 - WGS84_FF_EQ * Sin(lat))));
	**/

	double mp, part;

	part = WGS84_FF_EQ * Sin(lat);
	mp = TODEG(log(Tan(45.0 + lat / 2.0))
		- 0.5 * WGS84_FF_EQ * log((1 + part) / (1 - part)));
	return(mp);
}

//***************************************************************************
// 파일명        : CICMSCalculator.cpp
// 최초 작성일   : 2006/02/22
// 마지막 수정일 : 2006/02/22
// 최초 작성자   : SW팀 SW2그룹/PKX SW 하윤철
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 김천환
// 함수명        : GetXYPos
// 설명          : 위경도 값을 X,Y 값으로 변환
// 입력값        : [IN] i_dbl
// Longitude : 시각 경도 (단위 : deg)
//				   [IN] i_dblOwnShipLatitude : 시작 위도 (단위 : deg)
//				   [IN] i_dblTargetLongitude : 끝 경도 (단위 : deg)
//				   [IN] i_dblTargetLatitude : 끝 위도 (단위 : deg)
//				   [OUT] i_pdblTargetPosX : 시작위치에서 끝위치까지 X(동쪽)거리 (단위 : m)
//				   [OUT] i_pdblTargetPosY : 시작위치에서 끝위치까지 Y(북쪽)거리 (단위 : m)
// 리턴값        : void
// 비고          :
//***************************************************************************
void CICMSCalculator::GetXYPos(double i_dblOwnShipLongitude, double i_dblOwnShipLatitude, double i_dblTargetLongitude, double i_dblTargetLatitude, double* i_pdblTargetPosX, double* i_pdblTargetPosY)
{
	/**
	GetBearingNDistance() 함수를 호출한다.
	*i_pdblTargetPosX = 계산된 거리 * Sin(계산된 방위)
	*i_pdblTargetPosY = 계산된 거리 * Cos(계산된 방위)
	**/
	double dblBearing, dblDistance;

	GetBearingNDistance(i_dblOwnShipLatitude, i_dblOwnShipLongitude, i_dblTargetLatitude, i_dblTargetLongitude, &dblBearing, &dblDistance);

	*i_pdblTargetPosX = dblDistance * Sin(dblBearing);
	*i_pdblTargetPosY = dblDistance * Cos(dblBearing);

}

//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/05/09
// 마지막 수정일: 2005/05/09
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: GetSpeedFromEachVelocity
// 설명			: 각축의 속력정보로부터 속도값을 구함
// 입력값		: X, Y, Z축 속력
// 리턴값		: 속도
// 비고			:
//*********************************************************************************************
double CICMSCalculator::GetSpeedFromVelocity(double fXVelocity, double fYVelocity, double fZVelocity)
{
	/**
	double dblSpeed;
	dblSpeed = GetRangeFromPosition(fXVelocity, fYVelocity, fZVelocity);
	return (double)dblSpeed;
	**/
	double dblSpeed;
	dblSpeed = GetRangeFromPosition(fXVelocity, fYVelocity, fZVelocity);
	return (double)dblSpeed;
}


//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/05/09
// 마지막 수정일: 2005/05/09
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: GetRangeFromPosition
// 설명			: 위치정보로부터 표적의 Range값을 구함
// 입력값		: X, Y, Z 위치 값
// 리턴값		: 거리
// 비고			:
//*********************************************************************************************
double CICMSCalculator::GetRangeFromPosition(double fXRelPosition, double fYRelPosition, double fZRelPosition)
{
	/**
	double dblRange;

	dblRange = (fXRelPosition * fXRelPosition + fYRelPosition * fYRelPosition + fZRelPosition * fZRelPosition);
	dblRange = sqrt(dblRange);

	return (double) dblRange;
	**/
	double dblRange;

	dblRange = (fXRelPosition * fXRelPosition + fYRelPosition * fYRelPosition + fZRelPosition * fZRelPosition);
	dblRange = sqrt(dblRange);

	return (double)dblRange;
}

//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/05/09
// 마지막 수정일: 2005/05/09
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: GetElevation
// 설명			: X, Y, Z으로부터 고각 계산
// 입력값		: X, Y, Z축 위치
// 리턴값		: 속도
// 비고			:
//*********************************************************************************************
double CICMSCalculator::GetElevation(double fXRelPosition, double fYRelPosition, double fZRelPosition)
{
	/**
	double fElevation;
	double fTemp;

	fTemp = fXRelPosition*fXRelPosition + fYRelPosition*fYRelPosition;
	fElevation = (double)Atan2(fZRelPosition,sqrt(fTemp));

	return fElevation;
	**/
	double fElevation;
	double fTemp;

	// ticket 2568 fTemp = fXRelPosition*fXRelPosition + fYRelPosition*fYRelPosition + fZRelPosition*fZRelPosition; 수식오류 2012.03.30
	fTemp = fXRelPosition * fXRelPosition + fYRelPosition * fYRelPosition;
	fElevation = (double)Atan2(fZRelPosition, sqrt(fTemp));

	return fElevation;
}


//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/11/24
// 마지막 수정일: 2005/11/24
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: GetCourseFromVelocity
// 설명			: 각축의 속도값으로부터 침로를 구함
// 입력값		: X, Y, Z축 속도
// 리턴값		: 침로
// 비고			:
//*********************************************************************************************
double CICMSCalculator::GetCourseFromVelocity(double fXVelocity, double fYVelocity, double fZVelocity)
{
	/// return GetBearingFromPosition(fXVelocity, fYVelocity);
	return GetBearingFromPosition(fXVelocity, fYVelocity);
}


//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/05/09
// 마지막 수정일: 2005/05/09
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: GetBearingFromPosition
// 설명			: 위치정보로부터 표적의 Bearing값을 구함
// 입력값		: 표적의 X, Y축 거리
// 리턴값		: 해당사항 없음
// 비고			: 진북기준의 Bearing을 Return(0 - 359.9999)
//*********************************************************************************************
double CICMSCalculator::GetBearingFromPosition(double fXRelPosition, double fYRelPosition)
{
	/**
	double fBearing = 0;

	fBearing = (double)Atan2(fXRelPosition,fYRelPosition);

	if ( fBearing < 0.0)
	{
		fBearing += 360;
	}

	return fBearing;
	**/
	double fBearing = 0;

	fBearing = (double)Atan2(fXRelPosition, fYRelPosition);

	if (fBearing < 0.0)
	{
		fBearing += 360;
	}

	return fBearing;
}

//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/05/09
// 마지막 수정일: 2005/05/09
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: GetBearingFromPosition
// 설명			: 구좌표를 직교좌표로 변환
// 입력값		: [IN] i_dblBearing - 방위각 (단위 : deg)
//                [IN] i_dblRange - 거리 (단위 : m)
//				  [IN] i_dblElevation - 고각 (단위 : deg)
//				  [OUT] i_pdblPosX - X 위치 (단위 : m)
//				  [OUT] i_pdblPosY - Y 위치 (단위 : m)
//				  [OUT] i_pdblPosZ - Z 위치 (단위 : m)
// 리턴값		: 해당사항 없음
// 비고			: 
//*********************************************************************************************
void CICMSCalculator::Sphere2Orthogonal(double i_dblBearing, double i_dblRange, double i_dblElevation, double* i_pdblPosX, double* i_pdblPosY, double* i_pdblPosZ)
{
	/**
	double dblElevationRad;
	double dblBearingRad;
	dblElevationRad = i_dblElevation * DEG2RAD;
	dblBearingRad = i_dblBearing * DEG2RAD;

	if(i_pdblPosX)
	{
		* i_pdblPosX = i_dblRange * cos(dblElevationRad) * sin(dblBearingRad);
	}
	if(i_pdblPosY)
	{
		* i_pdblPosY = i_dblRange * cos(dblElevationRad) * cos(dblBearingRad);
	}
	if(i_pdblPosZ)
	{
		* i_pdblPosZ = i_dblRange * sin(dblElevationRad);
	}
	**/
	double dblElevationRad;
	double dblBearingRad;
	dblElevationRad = i_dblElevation * DEG2RAD;
	dblBearingRad = i_dblBearing * DEG2RAD;

	if (i_pdblPosX)
	{
		*i_pdblPosX = i_dblRange * cos(dblElevationRad) * sin(dblBearingRad);
	}
	if (i_pdblPosY)
	{
		*i_pdblPosY = i_dblRange * cos(dblElevationRad) * cos(dblBearingRad);
	}
	if (i_pdblPosZ)
	{
		*i_pdblPosZ = i_dblRange * sin(dblElevationRad);
	}
}

//*********************************************************************************************
// 파일명		: CICMSCalculator.cpp
// 최초 작성일	: 2005/05/09
// 마지막 수정일: 2005/05/09
// 최초 작성자	: 시스템팀 SW그룹/PKX SW 김남인
// 마지막 수정자: 시스템팀 SW그룹/PKX SW 김남인
// 함수명		: Orthogonal2Sphere
// 설명			: 직교 좌표를 구좌표로 변환
// 입력값		: [IN] i_dblPosX - X 위치 (단위 : m)
//				  [IN] i_dblPosY - Y 위치 (단위 : m)
//				  [IN] i_dblPosZ - Z 위치 (단위 : m)
//				  [OUT] i_pdblBearing - 방위각 (단위 : deg)
//                [OUT] i_pdblRange - 거리 (단위 : m)
//				  [OUT] i_pdblElevation - 고각 (단위 : deg)
// 리턴값		: 해당사항 없음
// 비고			: 
//*********************************************************************************************
void CICMSCalculator::Orthogonal2Sphere(double i_dblPosX, double i_dblPosY, double i_dblPosZ, double* i_pdblBearing, double* i_pdblRange, double* i_pdblElevation)
{
	/**
	double dblTemp;
	dblTemp = i_dblPosX * i_dblPosX + i_dblPosY * i_dblPosY;

	if(i_pdblElevation)
	{
		*i_pdblElevation = atan2(i_dblPosZ,sqrt(dblTemp)) * RAD2DEG;
	}
	if(i_pdblBearing)
	{
		*i_pdblBearing = atan2(i_dblPosX,i_dblPosY) * RAD2DEG;
	}
	if(i_pdblRange)
	{
		*i_pdblRange = sqrt(dblTemp + i_dblPosZ * i_dblPosZ);
	}
	**/
	double dblTemp;
	dblTemp = i_dblPosX * i_dblPosX + i_dblPosY * i_dblPosY;

	if (i_pdblElevation)
	{
		*i_pdblElevation = atan2(i_dblPosZ, sqrt(dblTemp)) * RAD2DEG;
	}
	if (i_pdblBearing)
	{
		*i_pdblBearing = atan2(i_dblPosX, i_dblPosY) * RAD2DEG;
	}
	if (i_pdblRange)
	{
		*i_pdblRange = sqrt(dblTemp + i_dblPosZ * i_dblPosZ);
	}
}

double CICMSCalculator::GetTimeDiff(unsigned int uiEndTimeSec, unsigned int uiEndTimeMicroSec, unsigned int uiStartTimeSec, unsigned int uiStartTimeMicroSec)
{
	//start PKXB ticket: 8 y.san.kim 20141125 [수동표적 DR 시 튀는 현상]
	int iTimeDiff = 0;
	int iTimeDiff_us = 0;
	//end PKXB ticket: 8 y.san.kim 20141125 [수동표적 DR 시 튀는 현상]
	double dblDeltaTimeGap = 0.0f;

	iTimeDiff = uiEndTimeSec - uiStartTimeSec;				// sec.
	iTimeDiff_us = uiEndTimeMicroSec - uiStartTimeMicroSec;		// micro sec.

	if (iTimeDiff_us < 0)
	{
		iTimeDiff--;
		iTimeDiff_us = iTimeDiff_us + 1000000;

	}
	dblDeltaTimeGap = (double)((double)iTimeDiff + (double)((iTimeDiff_us) / 1000000.0));
	return dblDeltaTimeGap;
}