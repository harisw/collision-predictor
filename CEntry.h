//******************************************************************************************************
//This is implementation of Spatial-temporal tactical moving object Management Index based TPR*-tree
//Coded by Bong-hee Hong, Ji-wan Lee, Jae-gi Hong, Ki-jin Kim from Pusan National University
//May 2016
//******************************************************************************************************
#pragma once

#include "ICMSCalculator.h"
#include "time.h"
#include "Util.h"
#include <stdio.h>
#include <algorithm>
using namespace std;
struct CPosition
{
	double x;				//�浵 ��ġ
	double y;				//���� ��ġ
	CPosition() {};
	CPosition(double _x, double _y) {
		x = _x;
		y = _y;
	}
};

class CEntry
{
public:
	CEntry(void);
	~CEntry(void);

	CEntry(int _id, unsigned long _time, double _x, double _y, double _z, double _vx, double _vy, double _vz, double _radius = 0) {
		m_x = _x;
		m_y = _y;
		m_z = _z;
		m_vx = _vx;
		m_vy = _vy;
		m_vz = _vz;
		m_id = _id;
		m_time = _time;
		m_BufferRadius = _radius;
		
		double dest_x = m_x + Util::interval * m_vx;
		double dest_y = m_y + Util::interval * m_vy;

		m_MBR[0] = min(m_x, dest_x) - _radius;
		m_MBR[1] = min(m_y, dest_y) - _radius;
		m_MBR[2] = max(m_x, dest_x) + -_radius;
		m_MBR[3] = max(m_y, dest_y) + -_radius;


	}

	CEntry* getEntry() {
		return new CEntry(m_id, m_time, m_x, m_y, m_z, m_vx, m_vy, m_vz, m_BufferRadius);
	}

	int getID() { return m_id; }
	void setID(int _id) { m_id = _id; }

	double getTime() { return m_time; }
	void setTime(double _time) { m_time = _time; }

	double getX() { return m_x; }
	void setX(double _x) { m_x = _x; }

	double getY() { return m_y; }
	void setY(double _y) { m_y = _y; }

	double getZ() { return m_z; }
	void setZ(double _z) { m_z = _z; }

	double getVX() { return m_vx; }
	void setVX(double _vx) { m_vx = _vx; }

	double getVY() { return m_vy; }
	void setVY(double _vy) { m_vy = _vy; }

	double getVZ() { return m_vz; }
	void setVZ(double _vz) { m_vz = _vz; }

	void extrapolation(double _time);
	void extfuture_location(double* XY, double _treeTime);

	void updateCurrentLoc();
public:
	int m_id;
	double m_time;
	double m_x, m_y, m_z; //���浵, ��
	double m_extX, m_extY, m_extZ;
	double  m_vx, m_vy, m_vz;
	int m_byEnvironment;
	//hari
	double m_BufferRadius;
	double m_MBR[4];
	int m_IsCandidate = false;

public: // for test
	bool updated = false;
	bool created = false;
	bool deleted = false;
	bool updated_for_display = false;
	bool created_for_display = false;
	int update_count;
	int update_rate;
};