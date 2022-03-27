//******************************************************************************************************
//This is implementation of Spatial-temporal tactical moving object Management Index based TPR*-tree
//Coded by Bong-hee Hong, Ji-wan Lee, Jae-gi Hong, Ki-jin Kim from Pusan National University
//May 2016
//******************************************************************************************************
#include "TPRNode.h"
#include <list>
#include <tchar.h>

using namespace std;

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

TPRNode::TPRNode(bool isNull)
{
	m_ID = -1;
	m_level = 0;
	m_MBR[0] = 0; //MIN_X
	m_MBR[1] = 0; //MIN_Y
	m_MBR[2] = 0; //MAX_X
	m_MBR[3] = 0; //MAX_Y

	m_VBR[0] = 0;
	m_VBR[1] = 0;
	m_VBR[2] = 0;
	m_VBR[3] = 0;
	m_VBR[4] = 0;
	m_VBR[5] = 0;
	//m_entry = NULL;
	m_parent = NULL;
}

TPRNode::TPRNode(double _time, TPRNode** nodes, int _treeID)
{
	treeID = _treeID;

	m_ObjectNodePositionRef = nodes; // cskim

	m_level = 0;
	m_ID = -1;
	m_MBR[0] = 0; //MIN_X
	m_MBR[1] = 0; //MIN_Y
	m_MBR[2] = 0; //MAX_X
	m_MBR[3] = 0; //MAX_Y

	m_VBR[0] = 0;
	m_VBR[1] = 0;
	m_VBR[2] = 0;
	m_VBR[3] = 0;
	m_VBR[4] = 0;
	m_VBR[5] = 0;
	m_parent = NULL;
	m_NumCntEntries = 0;
	m_NumCntChild = 0;
	m_time = _time;

	if (TREE_TYPE_MEMORY_OR_DISK == 0) // memory based
	{
		m_entry = (CEntry*)malloc((CAPACITY + 1) * sizeof(CEntry));
	}
	else
	{
		m_entry = NULL; // disk based
	}

	m_MaxBufferRadius = -99999999;
}

TPRNode::TPRNode(TPRNode** nodes) // cskim
{
	m_ObjectNodePositionRef = nodes; // cskim

	m_level = 0;
	m_ID = -1;
	m_MBR[0] = 0; //MIN_X
	m_MBR[1] = 0; //MIN_Y
	m_MBR[2] = 0; //MAX_X
	m_MBR[3] = 0; //MAX_Y

	m_VBR[0] = 0;
	m_VBR[1] = 0;
	m_VBR[2] = 0;
	m_VBR[3] = 0;
	m_VBR[4] = 0;
	m_VBR[5] = 0;
	m_parent = NULL;
	m_NumCntEntries = 0;
	m_NumCntChild = 0;
	//m_entry = NULL;
}
TPRNode::TPRNode(double _time)
{
	m_level = 0;
	m_ID = -1;
	m_MBR[0] = 0; //MIN_X
	m_MBR[1] = 0; //MIN_Y
	m_MBR[2] = 0; //MAX_X
	m_MBR[3] = 0; //MAX_Y

	m_VBR[0] = 0;
	m_VBR[1] = 0;
	m_VBR[2] = 0;
	m_VBR[3] = 0;
	m_VBR[4] = 0;
	m_VBR[5] = 0;
	m_parent = NULL;
	m_NumCntEntries = 0;
	m_NumCntChild = 0;
	m_time = _time;
	//m_entry = NULL;
	for (int i = 0; i < NUMNODE + 1; i++) {
		m_childNode[i] = NULL;
	}
}

TPRNode::~TPRNode(void)
{
	//if (TREE_TYPE_MEMORY_OR_DISK == 0)
	//	free(m_entry);

	for (int i = 0; i < NUMNODE + 1; i++)
	{
		m_childNode[i] = NULL;
	}
	m_parent = NULL;
}

bool TPRNode::Included(CEntry _input)
{
	/*
	if (_input.getX() >= m_MBR[0] && _input.getX() <= m_MBR[2] && _input.getY() >= m_MBR[1] && _input.getY() <= m_MBR[3])
		return true;

	return false;
	*/

	double offset = 0.0000001;
	if (_input.getX() >= m_MBR[0] - offset && _input.getX() <= m_MBR[2] + offset && _input.getY() >= m_MBR[1] - offset && _input.getY() <= m_MBR[3] + offset)
		return true;
	return false;

}

CEntry TPRNode::Pickworst()
{
	double myArea = (m_MBR[2] - m_MBR[0]) * (m_MBR[3] - m_MBR[1]);
	double subArea[CAPACITY + 1];
	for (int i = 0; i < CAPACITY + 1; i++)
	{
		subArea[i] = getsubArea(myArea, i);
	}
	int index = -1;
	double maxSubArea = -1;
	for (int i = 0; i < CAPACITY + 1; i++)
	{
		if (maxSubArea < subArea[i])
		{
			maxSubArea = subArea[i];
			index = i;
		}
	}

	if (index < 0 || index > CAPACITY + 1)
		getchar();

	return m_entry[index];
}

double TPRNode::getsubArea(double _NodeArea, int _delEntryIndex)
{
	double minX = DBL_MAX;
	double minY = DBL_MAX;
	double maxX = -DBL_MAX;
	double maxY = -DBL_MAX;
	for (int i = 0; i < CAPACITY + 1; i++)
	{
		if (i != _delEntryIndex)
		{
			if (minX > m_entry[i].getX())
				minX = m_entry[i].getX();

			if (maxX < m_entry[i].getX())
				maxX = m_entry[i].getX();

			if (minY > m_entry[i].getY())
				minY = m_entry[i].getY();

			if (maxY < m_entry[i].getY())
				maxY = m_entry[i].getY();
		}
	}

	double Area = abs(maxX - minX) * abs(maxY - minY);
	double subArea = _NodeArea - Area;
	return subArea;
}

bool TPRNode::RemoveAllEntry()
{

	for (int i = 0; i < m_NumCntEntries; i++)
	{
		//file test
		m_entry[i] = CEntry();
	}
	m_NumCntEntries = 0;

	return true;
}



bool TPRNode::RemoveEntry(int _ID)
{
	for (int i = 0; i < m_NumCntEntries; i++)
	{
		if (m_entry[i].getID() == _ID)
		{
			m_entry[i] = m_entry[m_NumCntEntries - 1];
			m_entry[m_NumCntEntries - 1] = CEntry();
			m_NumCntEntries--;

			return true;
		}
	}
	return false;
}


void TPRNode::NodetoStructCopy(indexXY* _dest, TPRNode* _start)
{
	_dest->id = _start->getID();
	_dest->time = -1;
	_dest->x = ((_start->getMBR()[0] + _start->getMBR()[2]) / 2);
	_dest->y = ((_start->getMBR()[1] + _start->getMBR()[3]) / 2);
	_dest->z = NULL;
	_dest->vx = NULL;
	_dest->vy = NULL;
	_dest->vz = NULL;
}

void TPRNode::UpdateParentNode()
{
	if (m_level > 0)
	{
		for (int i = 0; i < m_NumCntChild; i++)
		{
			m_childNode[i]->setParent(this);
		}
	}
}

void TPRNode::NonleafNodeSplit(TPRNode* _curNode, double _time, TPRNode* _newNode)
{
	indexXY NodeX[NUMNODE + 1] = {};		// Sunghoon CAPACITY -> NUMNODE
	indexXY NodeY[NUMNODE + 1] = {};		// Sunghoon CAPACITY -> NUMNODE
	indexXY* NodeList;	// Sunghoon CAPACITY -> NUMNODE
	SplitAxis _splitAxis = SetNodeSplitAxis(NodeX, NodeY, _curNode);

	TPRNode _existingNode = *_curNode;

	if (_splitAxis.isX == true)
		NodeList = NodeX;
	else
		NodeList = NodeY;

	for (int i = 0; i < _splitAxis.splitIndex; i++)
	{
		_curNode->updateChildNode(_existingNode.getChildNode(NodeList[i].index), i);
	}
	for (int i = _splitAxis.splitIndex; i < NUMNODE + 1; i++)
	{
		//	Node *temp = new Node(_time);
		//	_curNode->updateChildNode(NULL,i); // nulling splitted
		_curNode->setNumCntChild(_curNode->getNumCntChild() - 1);
		// delete temp;
		_curNode->updateChildNode(NULL, i);
	}

	for (int i = 0; i < NUMNODE + 1; i++) // CSK 170425 nulling
	{
		_newNode->updateChildNode(NULL, i);
	}

	for (int i = _splitAxis.splitIndex; i < NUMNODE + 1; i++)
	{
		_newNode->setChildNode(_existingNode.getChildNode(NodeList[i].index));
	}

	// Update MBR & VBR
	double curMBR[4];
	double newMBR[4];
	for (int i = 0; i < 4; i++) {
		curMBR[i] = _splitAxis.pre[i];
		newMBR[i] = _splitAxis.next[i];
	}
	_curNode->setMBR(curMBR);
	_newNode->setMBR(newMBR);

	double curVBR[6];
	double newVBR[6];
	for (int i = 0; i < 6; i++) {
		curVBR[i] = _splitAxis.preV[i];
		newVBR[i] = _splitAxis.nextV[i];
	}
	_curNode->setVBR(curVBR);
	_newNode->setVBR(newVBR);

	_curNode->setTime(_time);
	_newNode->setTime(_time);
}

bool xCmp(indexXY a, indexXY b) { return a.x < b.x; }
bool yCmp(indexXY a, indexXY b) { return a.y < b.y; }

SplitAxis TPRNode::SetNodeSplitAxis(indexXY* _mbrX, indexXY* _mbrY, TPRNode* _OverflowNode)
{
	// Sunghoon
	int n;
	if (_OverflowNode->getLevel() == 0)
		n = CAPACITY;
	else
		n = NUMNODE;
	//
	for (int i = 0; i < n + 1; i++)
	{
		NodetoStructCopy(&_mbrX[i], _OverflowNode->getChildNode(i));
		_mbrX[i].index = i;
		_mbrY[i] = _mbrX[i];
	}
	sort(_mbrX, _mbrX + n, xCmp);
	sort(_mbrY, _mbrY + n, yCmp);

	SplitAxis splitAxis = minNodePerimeter(_mbrX, _mbrY, _OverflowNode);

	return splitAxis;
}

SplitAxis TPRNode::minNodePerimeter(indexXY* _XGroup, indexXY* _YGroup, TPRNode* _OverflowNode)
{
	double minPerimeter_X = DBL_MAX;
	double splitIndex_X = -1;
	double minPerimeter_Y = DBL_MAX;
	double splitIndex_Y = -1;

	double pre_X[4];
	double next_X[4];
	double pre_Y[4];
	double next_Y[4];

	double pre_VX[6];
	double next_VX[6];
	double pre_VY[6];
	double next_VY[6];

	for (int i = NUMNODE * UNDERFLOW_RATIO; i < NUMNODE * (1 - UNDERFLOW_RATIO); i++)
	{
		double pre[4] = { DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX };
		double next[4] = { DBL_MAX, DBL_MAX, -DBL_MAX,-DBL_MAX };
		double preV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };
		double nextV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

		double prePerimeter = 0;
		double nextPerimeter = 0;

		for (int j = 0; j < i + 1; j++)
		{
			pre[0] = min(pre[0], _OverflowNode->getChildNode(_XGroup[j].index)->getMBR()[0]);
			pre[1] = min(pre[1], _OverflowNode->getChildNode(_XGroup[j].index)->getMBR()[1]);
			pre[2] = max(pre[2], _OverflowNode->getChildNode(_XGroup[j].index)->getMBR()[2]);
			pre[3] = max(pre[3], _OverflowNode->getChildNode(_XGroup[j].index)->getMBR()[3]);

			preV[0] = min(preV[0], _OverflowNode->getChildNode(_XGroup[j].index)->getVBR()[0]);
			preV[1] = min(preV[1], _OverflowNode->getChildNode(_XGroup[j].index)->getVBR()[1]);
			preV[2] = min(preV[2], _OverflowNode->getChildNode(_XGroup[j].index)->getVBR()[2]);
			preV[3] = max(preV[3], _OverflowNode->getChildNode(_XGroup[j].index)->getVBR()[3]);
			preV[4] = max(preV[4], _OverflowNode->getChildNode(_XGroup[j].index)->getVBR()[4]);
			preV[5] = max(preV[5], _OverflowNode->getChildNode(_XGroup[j].index)->getVBR()[5]);
		}

		for (int k = i + 1; k < NUMNODE + 1; k++)	// Sunghoon CAPACITY -> NUMNODE
		{
			next[0] = min(next[0], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[0]);
			next[1] = min(next[1], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[1]);
			next[2] = max(next[2], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[2]);
			next[3] = max(next[3], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[3]);

			nextV[0] = min(next[0], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[0]);
			nextV[1] = min(next[1], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[1]);
			nextV[2] = min(next[2], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[2]);
			nextV[3] = max(next[3], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[3]);
			nextV[4] = max(next[4], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[4]);
			nextV[5] = max(next[5], _OverflowNode->getChildNode(_XGroup[k].index)->getMBR()[5]);
		}

		prePerimeter = (abs(pre[2] - pre[0]) * 2) + (abs(pre[3] - pre[1]) * 2);
		nextPerimeter = (abs(next[2] - next[0]) * 2) + (abs(next[3] - next[1]) * 2);
		if (minPerimeter_X > (prePerimeter + nextPerimeter))
		{
			minPerimeter_X = (prePerimeter + nextPerimeter);
			splitIndex_X = i + 1;
			for (int i = 0; i < 4; i++) {
				pre_X[i] = pre[i];
				next_X[i] = next[i];
			}
			for (int i = 0; i < 6; i++) {
				pre_VX[i] = preV[i];
				next_VX[i] = nextV[i];
			}
		}
	}

	for (int i = NUMNODE * UNDERFLOW_RATIO; i < NUMNODE * (1 - UNDERFLOW_RATIO); i++)
	{
		double pre[4] = { DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX };
		double next[4] = { DBL_MAX, DBL_MAX, -DBL_MAX,-DBL_MAX };
		double preV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };
		double nextV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

		double prePerimeter = 0;
		double nextPerimeter = 0;

		for (int j = 0; j < i + 1; j++)
		{
			pre[0] = min(pre[0], _OverflowNode->getChildNode(_YGroup[j].index)->getMBR()[0]);
			pre[1] = min(pre[1], _OverflowNode->getChildNode(_YGroup[j].index)->getMBR()[1]);
			pre[2] = max(pre[2], _OverflowNode->getChildNode(_YGroup[j].index)->getMBR()[2]);
			pre[3] = max(pre[3], _OverflowNode->getChildNode(_YGroup[j].index)->getMBR()[3]);

			preV[0] = min(preV[0], _OverflowNode->getChildNode(_YGroup[j].index)->getVBR()[0]);
			preV[1] = min(preV[1], _OverflowNode->getChildNode(_YGroup[j].index)->getVBR()[1]);
			preV[2] = min(preV[2], _OverflowNode->getChildNode(_YGroup[j].index)->getVBR()[2]);
			preV[3] = max(preV[3], _OverflowNode->getChildNode(_YGroup[j].index)->getVBR()[3]);
			preV[4] = max(preV[4], _OverflowNode->getChildNode(_YGroup[j].index)->getVBR()[4]);
			preV[5] = max(preV[5], _OverflowNode->getChildNode(_YGroup[j].index)->getVBR()[5]);
		}

		for (int k = i + 1; k < NUMNODE + 1; k++)	// Sunghoon CAPACITY -> NUMNODE
		{
			next[0] = min(next[0], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[0]);
			next[1] = min(next[1], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[1]);
			next[2] = max(next[2], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[2]);
			next[3] = max(next[3], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[3]);

			nextV[0] = min(next[0], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[0]);
			nextV[1] = min(next[1], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[1]);
			nextV[2] = min(next[2], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[2]);
			nextV[3] = max(next[3], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[3]);
			nextV[4] = max(next[4], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[4]);
			nextV[5] = max(next[5], _OverflowNode->getChildNode(_YGroup[k].index)->getMBR()[5]);
		}

		prePerimeter = (abs(pre[2] - pre[0]) * 2) + (abs(pre[3] - pre[1]) * 2);
		nextPerimeter = (abs(next[2] - next[0]) * 2) + (abs(next[3] - next[1]) * 2);
		if (minPerimeter_Y > (prePerimeter + nextPerimeter))
		{
			minPerimeter_Y = (prePerimeter + nextPerimeter);
			splitIndex_Y = i + 1;
			for (int i = 0; i < 4; i++) {
				pre_Y[i] = pre[i];
				next_Y[i] = next[i];
			}
			for (int i = 0; i < 6; i++) {
				pre_VY[i] = preV[i];
				next_VY[i] = nextV[i];
			}
		}
	}

	if (minPerimeter_X <= minPerimeter_Y)
	{
		SplitAxis result;
		result.isX = true;
		result.splitIndex = splitIndex_X;
		for (int i = 0; i < 4; i++) {
			result.pre[i] = pre_X[i];
			result.next[i] = next_X[i];
		}
		for (int i = 0; i < 6; i++) {
			result.preV[i] = pre_VX[i];
			result.nextV[i] = next_VX[i];
		}
		return result;
	}
	else
	{
		SplitAxis result;
		result.isX = false;
		result.splitIndex = splitIndex_Y;
		for (int i = 0; i < 4; i++) {
			result.pre[i] = pre_Y[i];
			result.next[i] = next_Y[i];
		}
		for (int i = 0; i < 6; i++) {
			result.preV[i] = pre_VY[i];
			result.nextV[i] = next_VY[i];
		}
		return result;
	}
}

SplitAxis TPRNode::SetSplitAxis(indexXY* _mbrX, indexXY* _mbrY, TPRNode* _OverflowNode)
{
	for (int i = 0; i < CAPACITY + 1; i++)
	{
		EntrytoStructCopy(&_mbrX[i], &_OverflowNode->getEntry()[i]);
		_mbrX[i].index = i;
		_mbrY[i] = _mbrX[i];
	}

	sort(_mbrX, _mbrX + CAPACITY + 1, xCmp);
	sort(_mbrY, _mbrY + CAPACITY + 1, yCmp);
	SplitAxis splitAxis = minPerimeter(_mbrX, _mbrY);
	return splitAxis;
}

void TPRNode::NodeSplit(TPRNode* _curNode, double _time, TPRNode* _newNode)
{
	indexXY _mbrX[CAPACITY + 1] = {};
	indexXY _mbrY[CAPACITY + 1] = {};
	SplitAxis _splitAxis = SetSplitAxis(_mbrX, _mbrY, _curNode);

	//_newNode->m_NumCntEntries = _curNode->getNumEntrys() - _splitAxis.splitIndex; // CSK170424

	SetExistingNode(_curNode, _splitAxis, _mbrX, _mbrY);
	SetNewNode(_splitAxis, _mbrX, _mbrY, _time, _newNode);

	_curNode->m_NumCntEntries = _splitAxis.splitIndex;
	_newNode->m_NumCntEntries = CAPACITY + 1 - _splitAxis.splitIndex;

	// Update MBR & VBR
	double curMBR[4];
	double newMBR[4];
	double curVBR[6];
	double newVBR[6];
	for (int i = 0; i < 4; i++) {
		curMBR[i] = _splitAxis.pre[i];
		newMBR[i] = _splitAxis.next[i];
	}
	_curNode->setMBR(curMBR);
	_newNode->setMBR(newMBR);

	for (int i = 0; i < 6; i++) {
		curVBR[i] = _splitAxis.preV[i];
		newVBR[i] = _splitAxis.nextV[i];
	}
	_curNode->setVBR(curVBR);
	_newNode->setVBR(newVBR);

	_curNode->setTime(_time);
	_newNode->setTime(_time);
}

int TPRNode::getNumEntrys()
{
	return m_NumCntEntries;
}

bool TPRNode::Insert(CEntry _InsertEntry)
{
	memcpy(&m_entry[m_NumCntEntries++], &_InsertEntry, sizeof(_InsertEntry));
	
	int lastIndex = m_NumCntEntries - 1;
	int newInsertedID = m_entry[lastIndex].m_id;
	for (int j = 0; j < m_NumCntEntries - 1; j++) {
		if (!(m_entry[lastIndex].m_MBR[0] > m_entry[j].m_MBR[2] || m_entry[lastIndex].m_MBR[2] < m_entry[j].m_MBR[0] ||
			m_entry[lastIndex].m_MBR[3] < m_entry[j].m_MBR[1] || m_entry[lastIndex].m_MBR[1] > m_entry[j].m_MBR[3])) {

			overlappingPairs.push_back(make_pair(m_entry[lastIndex].m_id, m_entry[j].m_id));
		}
	}
	return true;
}

void TPRNode::UpdateMBR(double _time)
{
	if (getLevel() == 0)
	{
		UpdateMBRbyEntry(_time);
	}
	else
	{
		UpdateMBRbyNode(_time);
	}
}

void TPRNode::UpdateMBRbyEntry(double _time)
{

	double tempMBR[4] = { DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX };

	for (int i = 0; i < m_NumCntEntries; i++)
	{
		m_entry[i].extrapolation(_time);
		tempMBR[0] = min(m_entry[i].getX(), tempMBR[0]);
		tempMBR[1] = min(m_entry[i].getY(), tempMBR[1]);
		tempMBR[2] = max(m_entry[i].getX(), tempMBR[2]);
		tempMBR[3] = max(m_entry[i].getY(), tempMBR[3]);
		//Dr. Jung
	}

	if (hasBufferZone) {
		//expand MBR by buffer radius
		tempMBR[0] -= m_MaxBufferRadius;
		tempMBR[1] -= m_MaxBufferRadius;
		tempMBR[2] += m_MaxBufferRadius;
		tempMBR[3] += m_MaxBufferRadius;
	}

	setMBR(tempMBR);

	double tempVBR[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

	for (int i = 0; i < m_NumCntEntries; i++)
	{
		tempVBR[0] = min(m_entry[i].getVX(), tempVBR[0]);
		tempVBR[1] = min(m_entry[i].getVY(), tempVBR[1]);
		tempVBR[2] = min(m_entry[i].getVZ(), tempVBR[2]);
		tempVBR[3] = max(m_entry[i].getVX(), tempVBR[3]);
		tempVBR[4] = max(m_entry[i].getVY(), tempVBR[4]);
		tempVBR[5] = max(m_entry[i].getVZ(), tempVBR[5]);
	}

	setVBR(tempVBR);
}

void TPRNode::UpdateMBRbyNode(double _time)
{

	double tempMBR[4] = { DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX };
	double extMBR[4];

	for (int i = 0; i < m_NumCntChild; i++)
	{
		extfuture_mbr_of_node(extMBR, m_childNode[i], _time, 0);

		tempMBR[0] = min(extMBR[0], tempMBR[0]);
		tempMBR[1] = min(extMBR[1], tempMBR[1]);
		tempMBR[2] = max(extMBR[2], tempMBR[2]);
		tempMBR[3] = max(extMBR[3], tempMBR[3]);

	}
	if (hasBufferZone) {
		//expand MBR by buffer radius
		tempMBR[0] -= m_MaxBufferRadius;
		tempMBR[1] -= m_MaxBufferRadius;
		tempMBR[2] += m_MaxBufferRadius;
		tempMBR[3] += m_MaxBufferRadius;
	}
	setMBR(tempMBR);

	double tempVBR[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

	for (int i = 0; i < m_NumCntChild; i++)
	{
		tempVBR[0] = min(m_childNode[i]->getVBR()[0], tempVBR[0]);
		tempVBR[1] = min(m_childNode[i]->getVBR()[1], tempVBR[1]);
		tempVBR[2] = min(m_childNode[i]->getVBR()[2], tempVBR[2]);
		tempVBR[3] = max(m_childNode[i]->getVBR()[3], tempVBR[3]);
		tempVBR[4] = max(m_childNode[i]->getVBR()[4], tempVBR[4]);
		tempVBR[5] = max(m_childNode[i]->getVBR()[5], tempVBR[5]);
	}

	setVBR(tempVBR);
}


void TPRNode::SortChildNode()
{

	if (m_level > 0)
	{
		int cnt = 0;
		TPRNode* tmpNode[NUMNODE + 1];
		for (int i = 0; i < m_NumCntChild; i++)
		{
			if (m_childNode[i] != NULL)
			{
				tmpNode[cnt] = m_childNode[i];
				cnt++;
			}
		}

		for (int i = 0; i < cnt; i++)
		{
			m_childNode[i] = tmpNode[i];
		}

		for (int i = cnt; i < NUMNODE + 1; i++)
		{
			m_childNode[i] = NULL;
		}
	}
}


void TPRNode::MBRCopy(indexXY* _dest, indexXY* _start)
{
	_dest->id = _start->id;
	_dest->time = _start->time;
	_dest->x = _start->x;
	_dest->y = _start->y;
	_dest->z = _start->z;
	_dest->vx = _start->vx;
	_dest->vy = _start->vy;
	_dest->vz = _start->vz;
	_dest->index = _start->index;

	_dest->uiTOVCreated = _start->uiTOVCreated;
	_dest->uiTOVCreated_us = _start->uiTOVCreated_us;
	_dest->m_uiTOVUpdated = _start->m_uiTOVUpdated;
	_dest->m_uiTOVUpdated_us = _start->m_uiTOVUpdated_us;
	_dest->byTrackStatus = _start->byTrackStatus;


}

void TPRNode::StructtoEntryCopy(CEntry* _dest, indexXY* _start)
{
	_dest->setID(_start->id);
	_dest->setTime(_start->time);
	_dest->setX(_start->x);
	_dest->setY(_start->y);
	_dest->setZ(_start->z);
	_dest->setVX(_start->vx);
	_dest->setVY(_start->vy);
	_dest->setVZ(_start->vz);
	_dest->m_byEnvironment = _start->m_byEnvironment;
}

void TPRNode::EntrytoStructCopy(indexXY* _dest, CEntry* _start)
{
	_dest->id = _start->getID();
	_dest->time = _start->getTime();
	_dest->x = _start->getX();
	_dest->y = _start->getY();
	_dest->z = _start->getZ();
	_dest->vx = _start->getVX();
	_dest->vy = _start->getVY();
	_dest->vz = _start->getVZ();
	_dest->m_byEnvironment = _start->m_byEnvironment;
}

SplitAxis TPRNode::minPerimeter(indexXY* _XGroup, indexXY* _YGroup)
{
	double minPerimeter_X = DBL_MAX;
	double splitIndex_X = -1;
	double minPerimeter_Y = DBL_MAX;
	double splitIndex_Y = -1;

	double pre_X[4];
	double next_X[4];
	double pre_Y[4];
	double next_Y[4];

	double pre_VX[6];
	double next_VX[6];
	double pre_VY[6];
	double next_VY[6];

	// Sunghoon
	for (int i = CAPACITY * UNDERFLOW_RATIO; i < CAPACITY * (1 - UNDERFLOW_RATIO); i++)
	{
		double pre[4] = { DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX };
		double next[4] = { DBL_MAX, DBL_MAX, -DBL_MAX,-DBL_MAX };
		double preV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };
		double nextV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

		double prePerimeter = 0;
		double nextPerimeter = 0;

		for (int j = 0; j < i + 1; j++)
		{
			pre[0] = min(pre[0], _XGroup[j].x);
			pre[1] = min(pre[1], _XGroup[j].y);
			pre[2] = max(pre[2], _XGroup[j].x);
			pre[3] = max(pre[3], _XGroup[j].y);

			preV[0] = min(preV[0], _XGroup[j].vx);
			preV[1] = min(preV[1], _XGroup[j].vy);
			preV[2] = min(preV[2], _XGroup[j].vz);
			preV[3] = max(preV[3], _XGroup[j].vx);
			preV[4] = max(preV[4], _XGroup[j].vy);
			preV[5] = max(preV[5], _XGroup[j].vz);
		}

		for (int k = i + 1; k < CAPACITY + 1; k++)
		{
			next[0] = min(next[0], _XGroup[k].x);
			next[1] = min(next[1], _XGroup[k].y);
			next[2] = max(next[2], _XGroup[k].x);
			next[3] = max(next[3], _XGroup[k].y);

			nextV[0] = min(nextV[0], _XGroup[k].vx);
			nextV[1] = min(nextV[1], _XGroup[k].vy);
			nextV[2] = min(nextV[2], _XGroup[k].vz);
			nextV[3] = max(nextV[3], _XGroup[k].vx);
			nextV[4] = max(nextV[4], _XGroup[k].vy);
			nextV[5] = max(nextV[5], _XGroup[k].vz);
		}

		prePerimeter = (abs(pre[2] - pre[0]) * 2) + (abs(pre[3] - pre[1]) * 2);
		nextPerimeter = (abs(next[2] - next[0]) * 2) + (abs(next[3] - next[1]) * 2);
		if (minPerimeter_X > (prePerimeter + nextPerimeter))
		{
			minPerimeter_X = (prePerimeter + nextPerimeter);
			splitIndex_X = i + 1;
			for (int j = 0; j < 4; j++) {
				pre_X[j] = pre[j];
				next_X[j] = next[j];
			}
			for (int j = 0; j < 6; j++) {
				pre_VX[j] = preV[j];
				next_VX[j] = nextV[j];
			}
		}
	}

	// Sunghoon
	for (int i = CAPACITY * UNDERFLOW_RATIO; i < CAPACITY * (1 - UNDERFLOW_RATIO); i++)
	{
		double pre[4] = { DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX };
		double next[4] = { DBL_MAX, DBL_MAX, -DBL_MAX,-DBL_MAX };
		double preV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };
		double nextV[6] = { DBL_MAX, DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

		double prePerimeter = 0;
		double nextPerimeter = 0;

		for (int j = 0; j < i + 1; j++)
		{
			pre[0] = min(pre[0], _YGroup[j].x);
			pre[1] = min(pre[1], _YGroup[j].y);
			pre[2] = max(pre[2], _YGroup[j].x);
			pre[3] = max(pre[3], _YGroup[j].y);

			preV[0] = min(preV[0], _YGroup[j].vx);
			preV[1] = min(preV[1], _YGroup[j].vy);
			preV[2] = min(preV[2], _YGroup[j].vz);
			preV[3] = max(preV[3], _YGroup[j].vx);
			preV[4] = max(preV[4], _YGroup[j].vy);
			preV[5] = max(preV[5], _YGroup[j].vz);
		}

		for (int k = i + 1; k < CAPACITY + 1; k++)
		{
			next[0] = min(next[0], _YGroup[k].x);
			next[1] = min(next[1], _YGroup[k].y);
			next[2] = max(next[2], _YGroup[k].x);
			next[3] = max(next[3], _YGroup[k].y);

			nextV[0] = min(nextV[0], _YGroup[k].vx);
			nextV[1] = min(nextV[1], _YGroup[k].vy);
			nextV[2] = min(nextV[2], _YGroup[k].vz);
			nextV[3] = max(nextV[3], _YGroup[k].vx);
			nextV[4] = max(nextV[4], _YGroup[k].vy);
			nextV[5] = max(nextV[5], _YGroup[k].vz);
		}

		prePerimeter = (abs(pre[2] - pre[0]) * 2) + (abs(pre[3] - pre[1]) * 2);
		nextPerimeter = (abs(next[2] - next[0]) * 2) + (abs(next[3] - next[1]) * 2);
		if (minPerimeter_Y > (prePerimeter + nextPerimeter))
		{
			minPerimeter_Y = (prePerimeter + nextPerimeter);
			splitIndex_Y = i + 1;
			for (int j = 0; j < 4; j++) {
				pre_Y[j] = pre[j];
				next_Y[j] = next[j];
			}
			for (int j = 0; j < 6; j++) {
				pre_VY[j] = preV[j];
				next_VY[j] = nextV[j];
			}
		}
	}

	if (minPerimeter_X <= minPerimeter_Y)
	{
		SplitAxis result;
		result.isX = true;
		result.splitIndex = splitIndex_X;
		for (int i = 0; i < 4; i++) {
			result.pre[i] = pre_X[i];
			result.next[i] = next_X[i];
		}
		for (int i = 0; i < 6; i++) {
			result.preV[i] = pre_VX[i];
			result.nextV[i] = next_VX[i];
		}
		return result;
	}
	else
	{
		SplitAxis result;
		result.isX = false;
		result.splitIndex = splitIndex_Y;
		for (int i = 0; i < 4; i++) {
			result.pre[i] = pre_Y[i];
			result.next[i] = next_Y[i];
		}
		for (int i = 0; i < 6; i++) {
			result.preV[i] = pre_VY[i];
			result.nextV[i] = next_VY[i];
		}
		return result;
	}
}

TPRNode* TPRNode::ChooseChildPath(CEntry _InsertEntry, double _time)
{
	struct enlargeAreaStruct
	{
		double Area;
		int index;
		bool operator <  (const enlargeAreaStruct& rhs) const
		{
			return Area < rhs.Area;
		}
	};
	double enlargeArea = DBL_MAX;
	int resultIndex = 0;
	for (int i = 0; i < m_NumCntChild; i++)
	{
		enlargeAreaStruct temp;
		temp.Area = m_childNode[i]->CalcEnlargeMBR(_InsertEntry, _time);
		temp.index = i;

		if (enlargeArea > temp.Area)
		{
			enlargeArea = temp.Area;
			resultIndex = i;
		}
	}
	return m_childNode[resultIndex];
}

double TPRNode::CalcEnlargeMBR(CEntry _InsertEntry, double _time)
{
	double extMBR[4];


	// CSK -->
	extfuture_mbr(extMBR, m_MBR, m_VBR, _time, 0);

	double nodeMinX = extMBR[0];
	double nodeMaxX = extMBR[2];
	double nodeMinY = extMBR[1];
	double nodeMaxY = extMBR[3];

	// <--- CSK
	/*
	double nodeMinX = m_MBR[0] + m_VBR[0] * (_time - m_time);
	double nodeMaxX = m_MBR[2] + m_VBR[3] * (_time - m_time);
	double nodeMinY = m_MBR[1] + m_VBR[1] * (_time - m_time);
	double nodeMaxY = m_MBR[3] + m_VBR[4] * (_time - m_time);
	*/

	double nodePreArea = (nodeMaxX - nodeMinX) * (nodeMaxY - nodeMinY);

	nodeMinX = min(nodeMinX, _InsertEntry.getX());
	nodeMinY = min(nodeMinY, _InsertEntry.getY());
	nodeMaxX = max(nodeMaxX, _InsertEntry.getX());
	nodeMaxY = max(nodeMaxY, _InsertEntry.getY());

	double nodeNextArea = (nodeMaxX - nodeMinX) * (nodeMaxY - nodeMinY);
	double enlargeArea = nodeNextArea - nodePreArea;
	return enlargeArea;
}

void TPRNode::SetNewNode(SplitAxis _splitAxis, indexXY* _mbrX, indexXY* _mbrY, double _time, TPRNode* _newNode)
{
	indexXY* mbrList;
	if (_splitAxis.isX == true)
		mbrList = _mbrX;
	else
		mbrList = _mbrY;

	for (int i = _splitAxis.splitIndex; i < CAPACITY + 1; i++)
	{
		int k = i - _splitAxis.splitIndex;
		CEntry* temp = new CEntry();
		StructtoEntryCopy(temp, &mbrList[i]);
		_newNode->setEntry(*temp, k);

		m_ObjectNodePositionRef[temp->getID()] = _newNode; // cskim

		delete temp;
	}
	m_time = _time;
}

void TPRNode::SetExistingNode(TPRNode* _curNode, SplitAxis _splitAxis, indexXY* _mbrX, indexXY* _mbrY)
{
	indexXY* mbrList;
	if (_splitAxis.isX == true)
		mbrList = _mbrX;
	else
		mbrList = _mbrY;

	for (int i = 0; i < _splitAxis.splitIndex; i++)
	{
		CEntry* temp = new CEntry();
		StructtoEntryCopy(temp, &mbrList[i]);
		_curNode->setEntry(*temp, i);
		m_ObjectNodePositionRef[temp->getID()] = _curNode; // cskim

		delete temp;
	}
	for (int i = _splitAxis.splitIndex; i < CAPACITY + 1; i++)
	{
		CEntry* temp = new CEntry();
		_curNode->setEntry(*temp, i);
		delete temp;
	}
}

void TPRNode::delChildNode(TPRNode* _childNode)
{
	for (int i = 0; i < m_NumCntChild; i++)
	{
		if (_childNode->getID() == m_childNode[i]->getID())
		{
			m_childNode[i] = m_childNode[m_NumCntChild - 1];
			m_childNode[m_NumCntChild - 1] = NULL;
			m_NumCntChild--;
			break;
		}
	}
}

bool TPRNode::RectangleInsect(double minX, double minY, double maxX, double maxY, double centralX, double centralY, double radius, int id)
{
	return !((minX > centralX + radius) || (maxX < centralX - radius) || (minY > centralY + radius) || (maxY < centralY - radius));
}

void TPRNode::extfuture_mbr(double* _extMbr, double* _mbr, double* _vbr, double _time, int _dim)
{
	double diffTime = _time - m_time; // sec.
	_extMbr[0] = _mbr[0] + _vbr[0] * diffTime;
	_extMbr[1] = _mbr[1] + _vbr[1] * diffTime;
	_extMbr[2] = _mbr[2] + _vbr[3] * diffTime;
	_extMbr[3] = _mbr[3] + _vbr[4] * diffTime;
	/*
	// changed by CSK

	double divTime = 3600;
	int Int_treeTime = (int)_time;
	int INt_m_uiTOVUpdated = (int)m_time;
	int DiffTime = Int_treeTime - INt_m_uiTOVUpdated;

	if (DiffTime > 0) {
		double Diff = ((Int_treeTime - INt_m_uiTOVUpdated) / divTime);
		double bearing = CICMSCalculator::GetCourseFromVelocity(_vbr[0], _vbr[1], 0);
		double distance = CICMSCalculator::GetSpeedFromVelocity(_vbr[0], _vbr[1], 0) * Diff;
		CICMSCalculator::ChangeBearingNRangeToLatLong(_mbr[1], _mbr[0], bearing, distance, &_extMbr[1], &_extMbr[0]); // X,Y 는 위경도 좌표임 - CSK

		bearing = CICMSCalculator::GetCourseFromVelocity(_vbr[3], _vbr[4], 0);
		distance = CICMSCalculator::GetSpeedFromVelocity(_vbr[3], _vbr[4], 0) * Diff;
		CICMSCalculator::ChangeBearingNRangeToLatLong(_mbr[3], _mbr[2], bearing, distance, &_extMbr[3], &_extMbr[2]); // X,Y 는 위경도 좌표임 - CSK
	}
	else {
		for (int i=0; i<4; i++){
			_extMbr[i]=_mbr[i];
		}
	}
	*/
}

void TPRNode::extfuture_mbr_of_node(double* _extMbr, TPRNode* _node, double _time, int _dim)
{
	double diffTime = _time - _node->m_time;
	_extMbr[0] = _node->m_MBR[0] + _node->m_VBR[0] * diffTime;
	_extMbr[1] = _node->m_MBR[1] + _node->m_VBR[1] * diffTime;
	_extMbr[2] = _node->m_MBR[2] + _node->m_VBR[3] * diffTime;
	_extMbr[3] = _node->m_MBR[3] + _node->m_VBR[4] * diffTime;
	if (hasBufferZone) {
		_extMbr[0] -= m_MaxBufferRadius;
		_extMbr[0] -= m_MaxBufferRadius;
		_extMbr[0] += m_MaxBufferRadius;
		_extMbr[0] += m_MaxBufferRadius;
	}
	/*
	// changed by CSK

	double divTime = 3600;
	int Int_treeTime = (int)_time;
	int INt_m_uiTOVUpdated = (int)_node->m_time;
	int DiffTime = Int_treeTime - INt_m_uiTOVUpdated;

	if (DiffTime > 0) {
		double Diff = ((Int_treeTime - INt_m_uiTOVUpdated) / divTime);
		double bearing = CICMSCalculator::GetCourseFromVelocity(_node->m_VBR[0], _node->m_VBR[1], 0);
		double distance = CICMSCalculator::GetSpeedFromVelocity(_node->m_VBR[0], _node->m_VBR[1], 0) * Diff;
		CICMSCalculator::ChangeBearingNRangeToLatLong(_node->m_MBR[1], _node->m_MBR[0], bearing, distance, &_extMbr[1], &_extMbr[0]); // X,Y 는 위경도 좌표임 - CSK

		bearing = CICMSCalculator::GetCourseFromVelocity(_node->m_VBR[3], _node->m_VBR[4], 0);
		distance = CICMSCalculator::GetSpeedFromVelocity(_node->m_VBR[3], _node->m_VBR[4], 0) * Diff;
		CICMSCalculator::ChangeBearingNRangeToLatLong(_node->m_MBR[3], _node->m_MBR[2], bearing, distance, &_extMbr[3], &_extMbr[2]); // X,Y 는 위경도 좌표임 - CSK
	}
	else {
		for (int i=0; i<4; i++){
			_extMbr[i] = _node->m_MBR[i];
		}
	}
	*/
}

void TPRNode::rangeQueryKNN(double centralX, double centralY, double radius, unsigned long _st, unsigned long _ed, int& _rsltcnt, vector<CEntry>& _CEntryList, double _queryTime)
{
	bool find = false;

	node_visit_count = 1;
	founded_obj_count = 0;

	if (m_level > 0)
	{
		double extMBR[4];

		for (int i = 0; i < m_NumCntChild; i++)
		{
			bool isIncluded = false;

			extfuture_mbr(extMBR, m_childNode[i]->getMBR(), m_childNode[i]->getVBR(), _queryTime, 0);

			isIncluded = this->RectangleInsect(extMBR[0], extMBR[1], extMBR[2], extMBR[3], centralX, centralY, radius, m_ID);
			if (isIncluded)
			{
				m_childNode[i]->rangeQueryKNN(centralX, centralY, radius, _st, _ed, _rsltcnt, _CEntryList, _queryTime);

				node_visit_count += m_childNode[i]->node_visit_count;
				founded_obj_count += m_childNode[i]->founded_obj_count;
			}
		}
	}
	if (m_level == 0)
	{
		this->allocEntryMemory();
		this->readEntryFile();

		for (int j = 0; j < m_NumCntEntries; j++)
		{
			double extXY[2];
			m_entry[j].extfuture_location(extXY, _queryTime);
			bool isIntersect = false;

			double dist = sqrt(pow(extXY[0] - centralX, 2) + pow(extXY[1] - centralY, 2));

			if (dist <= radius)
			{
				founded_obj_count++;
				_CEntryList.push_back(m_entry[j]);
			}

		}
		this->freeEntryMemory();
	}
}

void TPRNode::UpdateMBRbyExt(double _time)
{
	this->UpdateMBR(_time);
	m_time = _time;
}

void TPRNode::writeEntryFile()
{
	if (TREE_TYPE_MEMORY_OR_DISK == 0) return;

	if (m_entry == NULL) return;

	char fname[100];
	sprintf_s(fname, "%d_%d.txt", treeID, this->getID());
	ofstream fout;


	fout.open(fname, ios::out | ios::binary);
	fout.write((char*)&m_entry[0], this->getNumEntrys() * sizeof(CEntry));


	fout.close();

}

void TPRNode::readEntryFile()
{
	if (TREE_TYPE_MEMORY_OR_DISK == 0) return;

	if (m_entry == NULL) return;

	std::ifstream fin;
	char fname[100];

	sprintf_s(fname, "%d_%d.txt", treeID, this->getID());


	fin.open(fname, ios::in | ios::binary);
	if (fin.fail())
	{
		return;
	}

	fin.read((char*)&m_entry[0], this->getNumEntrys() * sizeof(CEntry));

	fin.close();


}

void TPRNode::allocEntryMemory()
{
	if (TREE_TYPE_MEMORY_OR_DISK == 0) return;


	if (m_entry != NULL)
		this->freeEntryMemory();

	m_entry = (CEntry*)malloc((CAPACITY + 1) * sizeof(CEntry));

}
void TPRNode::freeEntryMemory()
{
	if (TREE_TYPE_MEMORY_OR_DISK == 0) return;

	if (m_entry != NULL)
		free(m_entry);
	m_entry = NULL;

}

void TPRNode::CheckOverlappingArea()
{

}

void TPRNode::FindOverlappingRecursive(vector< pair<int, int> >& result)
{
	if (this == NULL)
		return;
	for (int j = 0; j < overlappingPairs.size(); j++) {
		result.push_back(overlappingPairs[j]);
	}
	if (m_level > 0) {
		for (int j = 0; j < m_NumCntChild; j++) {
			m_childNode[j]->FindOverlappingRecursive(result);
		}
	}
}