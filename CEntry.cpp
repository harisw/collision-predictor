//******************************************************************************************************
//This is implementation of Spatial-temporal tactical moving object Management Index based TPR*-tree
//Coded by Bong-hee Hong, Ji-wan Lee, Jae-gi Hong, Ki-jin Kim from Pusan National University
//May 2016
//******************************************************************************************************
#include "CEntry.h"

CEntry::CEntry(void)
{
	m_id = -1;
	m_time = -1;
	m_x = -1;
	m_y = -1;
	m_z = -1;
	m_vx = 1000;
	m_vy = 1000;
	m_vz = 1000;

}

CEntry::~CEntry(void)
{
}

void CEntry::extrapolation(double _treeTime)
{
	double diffTime = _treeTime - m_time; // millisec.
	m_x += m_vx * diffTime;// / 1000; // CSK
	m_y += m_vy * diffTime;// / 1000;
	m_z += m_vz * diffTime;// / 1000;
	m_time = _treeTime;
	/*
	_extMbr[0] = _mbr[0] + _vbr[0] * diffTime / 1000;
	_extMbr[1] = _mbr[1] + _vbr[1] * diffTime / 1000;
	_extMbr[2] = _mbr[2] + _vbr[3] * diffTime / 1000;
	_extMbr[3] = _mbr[3] + _vbr[4] * diffTime / 1000;

	double extX = 0;
	double extY = 0;
	double divTime = 3600;
	int DiffTime = _treeTime - m_time;

	if(DiffTime > 0)
	{
		double Diff = ((_treeTime - m_time)/divTime);
		double bearing = CICMSCalculator::GetCourseFromVelocity(m_vx,m_vy,m_vz);
		double distance = CICMSCalculator::GetSpeedFromVelocity(m_vx,m_vy,m_vz) * Diff;
		CICMSCalculator::ChangeBearingNRangeToLatLong(m_y,m_x,bearing,distance,&extY,&extX);
		m_x = extX;
		m_y = extY;

		m_z = m_z + (m_vz * Diff);

		m_time = _treeTime;
	}
	*/
}

void CEntry::extfuture_location(double* XY, double _treeTime)
{
	double diffTime = _treeTime - m_time; // millisec.
	XY[0] = m_x + m_vx * diffTime;// / 1000;
	XY[1] = m_y + m_vy * diffTime;// / 1000;

	/*
	XY[0] = m_x;
	XY[1] = m_y;
	double extX = (double)m_x;
	double extY = (double)m_y;

	double* temp_extX = &extX;
	double* temp_extY = &extY;

	double divTime = 3600;
	int Int_treeTime = (int)_treeTime;
	int INt_m_uiTOVUpdated = (int)m_time;
	int DiffTime = Int_treeTime - INt_m_uiTOVUpdated;

	if (DiffTime > 0)
	{
		double Diff = ((Int_treeTime - INt_m_uiTOVUpdated) / divTime);
		double bearing = CICMSCalculator::GetCourseFromVelocity(m_vx, m_vy, m_vz);
		double distance = CICMSCalculator::GetSpeedFromVelocity(m_vx, m_vy, m_vz) * Diff;
		CICMSCalculator::ChangeBearingNRangeToLatLong(m_y, m_x, bearing, distance, temp_extY, temp_extX);
		XY[0] = (double)*temp_extX;
		XY[1] = (double)*temp_extY;
	}
	*/
}

void CEntry::updateCurrentLoc()
{
	currentLoc->x += m_vx;
	currentLoc->y += m_vy;
}