#pragma once


//추후 define 값 이용
#define ENV_AIR			1// 공중
#define ENV_SURFACE		2// 수상
#define ENV_LAND		3// 육상
#define ENV_SUBSURFACE	4// 수중
#define ENV_UNKNOWN		5// 미확인

#define PI 				3.1415926535897932384626	//π값 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DOUBLE_EQUAL(a,b) ((((a)<(b))?(b)-(a):(a)-(b)) < (double)0.000000000000001)   // TRUE : equal,  FALSE : not equal//130225 정두환 추가

#define RAD2MIL			1018.59163578813			//radian값을 mil로 변환 하는 상수
#define RAD2DEG			(180.0/PI)					//radian값을 degree로 변환 하는 상수
#define RAD2MIN 		(10800./PI)					//radina값을 mimnute로 변환 하는 상수
#define MIL2DEG			0.05625						//mil값을 degree값으로 변환 하는 상수
#define DEG2MIL			17.777777777777				//degree값을 mil값으로 변환 하는 상수

#define KPH2MS			0.27777777					//m/hour를 m/sec로 변환 하는 상수

#define DEG2RAD			(PI/180.0)					//degree값을 radian값으로 변환 하는 상수	
#define MIL2RAD			0.000981747705				//mil값을 radian값으로 변환 하는 상수

#define MIN2DEG			(1./60.)					//minute값을 degree값으로 변환 하는 상수
#define	DEG2MIN			60.							//degree값을 minute값으로 변환 하는 상수 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define POUND2KG		(0.453592)					//pound를 kg로 변환 하는 상수 
#define SQ2KG			(0.13)						//square를 kg로 변환 하는 상수 (탄도 계산)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MBARTONM2		(100.)						//mbar를 newton/m^2로 변환 하는 상수
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define KT2MS			(1852./3600.)				//kts를 m/s로 변환 하는 상수 
#define MS2KT			(3600./1852.)				//m/s를 kts로 변환 하는 상수
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
// 101126 최병철 DM(Data Mile)     = 1828.8  meter  //
//               NM(Nautical Mile) = 1852    meter  //
//				 MILE(Mile)        = 1609.34 meter  //
//                                                  //
//          **** 단위 혼동 주의 ****                //
//                                                  //
//////////////////////////////////////////////////////

// 101206 김용환
// 참고사항 : 1DM(Data Mile) - 레이다나 전술데이터링크관련한 시스템에서 주로 사용되는 단위, 6000 feet를 말하며, 1.8288 km으로 사용
//            MILE - 1DM * 0.88, 약 1609.3m   

#define DmToM 1828.8								// DM -> m								//ticket:1320 101126 김영산
#define MToDM 1/1828.8								// m -> DM								//ticket:1320 101126 김영산					
#define DmToNM 1828.8/1852;							// DM -> NM								//ticket:1320 101126 김영산 //PKX-B ticket:133 140225 김영산
#define NMToDM 1852/1828.8;							// NM -> DM								//ticket:1320 101126 김영산 //PKX-B ticket:133 140225 김영산

#define INCH2METER		(0.0254)					//inch를 m로 변환 하는 상수
#define NM2M			1852.						//nautical mile을 m로 변환 하는 상수 
#define M2NM			(1./1852.)					//m를 nautical mile로 변환 하는 상수

#define FT2M 0.3048									// feet  -> meter						//ticket:1272 101123 송정훈
#define M2FT  3.28084								// meter -> feet						//ticket:1272 101123 송정훈

#define YD2M			0.91440183					//yard를 m로 변환 하는 상수
#define M2YD			(1./YD2M)					//m를 yard로 변환 하는 상수				// ticket:1049 20101022 송정훈, 이흥배 		

#define	KNOT2MPS		0.51444444444444			//knot를 m/sec값으로 변환 하는 상수
#define MPS2KNOT		1.9438444924406				//m/sec를 knot값으로 변환 하는 상수
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TORAD(d)	((d)/57.2957795131)
#define TODEG(r)	((r)*57.2957795131)

#define Sin(x)			sin(TORAD(x))
#define Cos(x)			cos(TORAD(x))
#define Tan(x)			tan(TORAD(x))
#define Asin(x)			TODEG(asin(x))
#define Acos(x)			TODEG(acos(x))
#define Atan(x)			TODEG(atan(x))
#define Atan2(x1,x2)	TODEG(atan2(x1,x2))

#define RADIUS_G 6378137.
#define RADIUS_G_NM (RADIUS_G*PI/180.0/1852.)

#define WGS84_FF	(1.0/298.257223563)
#define WGS84_FF_EQ	(sqrt(2.0*WGS84_FF-WGS84_FF*WGS84_FF))
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// - FFX를 위한 TM인 경우(default)
#include "math.h"



//***************************************************************************
// 파일명        : ICMSCalculator.h
// 프로젝트명    : ICMS 공통 계산 모듈
// 최초 작성일   : 2009/01/03
// 마지막 수정일 : 2009/01/03
// 최초 작성자   : SW팀 SW2그룹/PKX SW 김천환
// 마지막 수정자 : SW팀 SW2그룹/PKX SW 김천환 
// 클래스명      : CICMSCalculator
// 클래스 설명   : ICMS에서 사용하는 좌표계산 관련 
// 비고          : 
//***************************************************************************
class CICMSCalculator
{
	// Construction
public:
	CICMSCalculator();																	// 생성자
	virtual ~CICMSCalculator();															// 소멸자

	static double GetBearing(double dLat_1, double dLon_1, double dLat_2, double dLon_2);		// 2개 위경도의 방위 구하는 함수
	static double GetDistance(double dLat_1, double dLon_1, double dLat_2, double dLon_2);		// 2개 위경도의 거리 구하는 함수
	static void GetBearingNDistance(double dLat_1, double dLon_1, double dLat_2, double dLon_2, double* dBearing, double* dDistance); // 2개의 위경도로 방위, 거리 구하는 함수
	static void ChangeBearingNRangeToLatLong(double dLat_1, double dLon_1, double dBearing, double dDistance, double* dLat_2, double* dLon_2); // 기준 위경도로부터 방위, 거리에 해당하는 위경도 구하는 함수
	static void GetXYPos(double i_dblOwnShipLongitude, double i_dblOwnShipLatitude, double i_dblTargetLongitude, double i_dblTargetLatitude, double* i_pdblTargetPosX, double* i_pdblTargetPosY); // 위경도로부터 X,Y,Z구하는 함수

	static double GetSpeedFromVelocity(double fXVelocity, double fYVelocity, double fZVelocity);	// 속도성분으로부터 X,Y,Z의 속도 구하기
	static double GetRangeFromPosition(double fXRelPosition, double fYRelPosition, double fZRelPosition); // 위치정보로부터 거리 구하기
	static double GetElevation(double fXRelPosition, double fYRelPosition, double fZRelPosition);	// 위치정보로부터 고각 구하기
	static double GetCourseFromVelocity(double fXVelocity, double fYVelocity, double fZVelocity);	// 속도성분으로 침로 구하기
	static double GetBearingFromPosition(double fXRelPosition, double fYRelPosition);				// 위치정보로 방위 구하기
	static void Sphere2Orthogonal(double i_dblBearing, double i_dblRange, double i_dblElevation, double* i_pdblPosX, double* i_pdblPosY, double* i_pdblPosZ); // 구좌표를 직교좌표로 변환
	static void Orthogonal2Sphere(double i_dblPosX, double i_dblPosY, double i_dblPosZ, double* i_pdblBearing, double* i_pdblRange, double* i_pdblElevation); // 직교 좌표를 구좌표로 변환

private:
	static double	meridional_dist(double lat);												// meridional_dist
	static double	meridional_part(double lat);												// meridional_part
	static double recurs(double FromLatt, double dBearing, double dDistance);					// recurs

public:
	static double GetTimeDiff(unsigned int uiEndTimeSec, unsigned int uiEndTimeMicroSec, unsigned int uiStartTimeSec, unsigned int uiStartTimeMicroSec);
};

