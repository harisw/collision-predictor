//******************************************************************************************************
//This is implementation of Spatial-temporal tactical moving object Management Index based TPR*-tree
//Coded by Bong-hee Hong, Ji-wan Lee, Jae-gi Hong, Ki-jin Kim from Pusan National University
//May 2016
//******************************************************************************************************
#include "TPRTree.h"



#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))


bool TPRTree::RemoveEntry(TPRNode* _curNode, int _id) // cskim
{
	bool removed = false;
	removed = _curNode->RemoveEntry(_id);

	if (removed == true)
	{
		m_ObjectNodePosition[_id] = NULL; // cskim
	}
	else return false;

	return true;
}

int TPRTree::getNodeCountRecursive(TPRNode* node)
{
	if (node == NULL) return 0;

	int allNodeCount = 1;

	for (int i = 0; i < node->getNumCntChild(); i++)
	{
		allNodeCount += getNodeCountRecursive(node->m_childNode[i]);
	}

	return allNodeCount;
}


int TPRTree::getNLeafCountRecursive(TPRNode* node)
{
	if (node->getLevel() == 0) return 0;

	int NLeafCount = 1;

	for (int i = 0; i < node->getNumCntChild(); i++)
	{
		NLeafCount += getNLeafCountRecursive(node->m_childNode[i]);
	}

	return NLeafCount;

}

int TPRTree::getLeafCountRecursive(TPRNode* node)
{
	if (node->getLevel() == 0) return 1;

	int LeafCount = 0;

	for (int i = 0; i < node->getNumCntChild(); i++)
	{
		LeafCount += getLeafCountRecursive(node->m_childNode[i]);
	}

	return LeafCount;
}

int TPRTree::getObjectCountRecursive(TPRNode* node)
{
	if (node->getLevel() == 0) {
		return node->getNumEntrys();
	}

	int ObjectCount = 0;

	for (int i = 0; i < node->getNumCntChild(); i++)
	{
		ObjectCount += getObjectCountRecursive(node->m_childNode[i]);
	}

	return ObjectCount;
}

//void TPRTree::printAllObjectRecursive(TPRNode* node)
//{
//	if (node->getLevel() == 0) {
//		cout << "Node #" << node->getID() << ",  " << node->getNumEntrys() << " Entries" << endl;
//		CEntry *entries = node->getEntry();
//		if (node->getEntry() != nullptr) {
//			for (int j = 0; j < node->getNumEntrys(); j++) {
//				cout << " Entry #" << entries[j].m_id << " ( " << entries[j].m_x << ", " << entries[j].m_y << ")" << endl;
//			}
//		}
//		//return node->getNumEntrys();
//	}
//
//	//int ObjectCount = 0;
//
//	for (int i = 0; i < node->getNumCntChild(); i++)
//	{
//		//ObjectCount += getObjectCountRecursive(node->m_childNode[i]);
//		printAllObjectRecursive(node->m_childNode[i]);
//	}
//
//	//return ObjectCount;
//}



//// end CSKIM

TPRTree::TPRTree(void)
{
	m_root = NULL;
	m_level = 0;
	m_time = -1;
	m_NodeIDCtrl = 0;

	InsertedTrackList = (InsertedTrackInfo*)malloc(sizeof(InsertedTrackInfo) * MAX_TRACK_NUM); // CSK

	for (int i = 0; i < MAX_TRACK_NUM; i++)
	{
		InsertedTrackList[i].ID = -1;
	}
}

void  TPRTree::ClearNodeRecursive(TPRNode* _curNode)
{
	if (_curNode == NULL) {
		return;
	}

	if (_curNode->getLevel() == 0) {
		return;
	}

	for (int i = 0; i < _curNode->getNumCntChild(); i++)
	{
		ClearNodeRecursive(_curNode->getChildNode(i));

		delete _curNode->getChildNode(i);
	}
	return;
}

void TPRTree::PrintAllEntry()
{
	bool vesselFlag[1000];
	fill_n(vesselFlag, 1000, false);
	int count = 0;
	PrintAllEntryRecursive(m_root, vesselFlag, count);
	cout << "TOTAL entry : " << count << endl;
}

void TPRTree::PrintAllEntryRecursive(TPRNode* node, bool* vesselFlag, int &count)
{
	if (node->getLevel() == 0) {
		cout << "Leaf Node #" << node->m_ID << ": ";
		for (int j = 0; j < node->getNumEntrys(); j++) {
			int ent_id = node->getEntry()[j].m_id;
			if (vesselFlag[ent_id])
				cout << "DUPLICATE " << ent_id << "!! ";
			else
				vesselFlag[ent_id] = true;
			cout << "Entry " << ent_id << endl;
			count++;
		}
	}
	else {
		for (int j = 0; j < node->getNumCntChild(); j++)
			PrintAllEntryRecursive(node->m_childNode[j], vesselFlag, count);
	}
}

void TPRTree::GetOverlappingObject(vector<CEntry*>& outputList, int time)
{

	if (m_root == NULL)
		return;
	GetOverlappingRecursive(m_root, outputList, time);

}

void TPRTree::GetOverlappingRecursive(TPRNode* node, vector<CEntry*>& outputList, int time)
{
	if (node->getLevel() == 0) {
		//cout << "Node #" << node->getID() << ",  " << node->getNumEntrys() << " Entries" << endl;
		CEntry *entries = node->getEntry();
		if (node->getEntry() != nullptr) {

			//for (int j = 0; j < node->m_ObservableEntriesID.size(); j++)
			//	outputList[j] = entries;
//				cout << " Entry #" << entries[j].m_id << " ( " << entries[j].m_x << ", " << entries[j].m_y << ")" << endl;
		}
		return;
	}

	for (int i = 0; i < node->getNumCntChild(); i++)
		GetOverlappingRecursive(node->m_childNode[i], outputList, time);
	return;
}

bool TPRTree::Clear(void)
{
	bool empty = true;

	if (m_root != NULL) {
		empty = false;
		ClearNodeRecursive(m_root);
	}
	m_root = NULL;
	m_level = 0;
	m_time = -1;
	m_NodeIDCtrl = 0;

	InsertedTrackInfo* temp = new InsertedTrackInfo();
	for (int i = 0; i < MAX_TRACK_NUM; i++)
	{
		InsertedTrackList[i] = *temp;
	}
	delete temp;

	return empty;
}


TPRTree::~TPRTree(void)
{
	delete m_root;
}

void TPRTree::TreeCheck(TPRNode* node) {
	if (node == NULL) {
		printf("invalid\n");
	}
	for (int i = 0; i < node->getNumCntChild(); i++) {
		TreeCheck(node->getChildNode(i));
	}
}

bool TPRTree::InsertRecursive(TPRNode* _curNode, CEntry _input, double _insertTime)
{

	if (_curNode->getLevel() == 0)
	{
		_curNode->allocEntryMemory();
		_curNode->readEntryFile();
	}


	// New MBR at the insert time
	_curNode->UpdateMBRbyExt(_insertTime);

	if (_curNode->getLevel() == 0)
	{
		_curNode->Insert(_input);
		if (hasBufferZone) {
			if (_curNode->m_MaxBufferRadius < _input.m_BufferRadius)		//UPDATE BUFFER RADIUS
				_curNode->m_MaxBufferRadius = _input.m_BufferRadius;
		}

		_curNode->UpdateMBRbyExt(_insertTime);

		m_ObjectNodePosition[_input.getID()] = _curNode; // cskim
		m_InputEntry.setID(-1); //위아래 순서변경? yk

		if (_curNode->getNumEntrys() > CAPACITY && m_status == NORMAL)
		{
			m_InputEntry = _curNode->Pickworst();
			_curNode->RemoveEntry(m_InputEntry.getID());
			_curNode->UpdateMBRbyExt(_insertTime);
			m_PreNodeid = _curNode->getID();
		}
		else if (_curNode->getNumEntrys() > CAPACITY && m_status == REINSERT)	// Sunghoon NUMNODE -> CAPACITY
		{
			if (m_PreNodeid == _curNode->m_ID)
			{
				_curNode->UpdateMBRbyExt(_insertTime);

				TPRNode* tmp = new TPRNode(_insertTime, m_ObjectNodePosition, this->getTreeID());
				if (hasBufferZone)
					m_root->hasBufferZone = true;
				tmp->allocEntryMemory();

				double parentMBR[4] = { _curNode->getMBR()[0], _curNode->getMBR()[1], _curNode->getMBR()[2], _curNode->getMBR()[3] };
				double parentVBR[6] = { _curNode->getVBR()[0], _curNode->getVBR()[1], _curNode->getVBR()[2], _curNode->getVBR()[3], _curNode->getVBR()[4], _curNode->getVBR()[5] };

				_curNode->NodeSplit(_curNode, _insertTime, tmp);
				tmp->setID(m_NodeIDCtrl++);

				if (_curNode == m_root)
				{
					TPRNode* tmpRoot = new TPRNode(_insertTime, m_ObjectNodePosition, this->getTreeID());
					if (hasBufferZone)
						m_root->hasBufferZone = true;
					tmpRoot->setID(m_NodeIDCtrl++);

					tmpRoot->setChildNode(_curNode);
					tmpRoot->setChildNode(tmp);
					_curNode->setParent(tmpRoot);
					tmp->setParent(tmpRoot);

					tmp->setLevel(_curNode->getLevel());
					tmpRoot->setLevel(_curNode->getLevel() + 1);

					m_root = tmpRoot;
					m_root->setMBR(parentMBR);
					m_root->setVBR(parentVBR);
					//m_root->UpdateMBRbyExt(_insertTime);//
				}
				else
				{
					tmp->setParent(_curNode->getParent());
					_curNode->getParent()->setChildNode(tmp);
					_curNode->getParent()->setMBR(parentMBR);
					_curNode->getParent()->setVBR(parentVBR);
					//_curNode->getParent()->UpdateMBRbyExt(_insertTime);//
				}

				_curNode->getParent()->setLevel(_curNode->getLevel() + 1);
				m_status = NORMAL;

				tmp->writeEntryFile();
				tmp->freeEntryMemory();
			}
			else
			{
				if (_curNode->getNumEntrys() > CAPACITY)	// Sunghoon NUMNODE -> CAPACITY
				{
					m_InputEntry = _curNode->Pickworst();
					m_PreNodeid = _curNode->getID();
					_curNode->RemoveEntry(m_InputEntry.getID());
					_curNode->UpdateMBRbyExt(_insertTime);
					m_status = REINSERT;

					m_ObjectNodePosition[m_InputEntry.getID()] = NULL; // cskim
				}
			}
		}

		if (m_InputEntry.getID() != -1)
		{
			m_status = REINSERT;
		}
		else
		{
			m_status = NORMAL;
			_curNode->UpdateMBRbyExt(_insertTime);
		}

	}
	else
	{
		TPRNode* ChooseNode = _curNode->ChooseChildPath(_input, _insertTime);
		InsertRecursive(ChooseNode, _input, _insertTime);
		_curNode->UpdateMBRbyExt(_insertTime);

		if (_curNode->getNumCntChild() > NUMNODE)
		{
			TPRNode* tmp = new TPRNode(_insertTime, m_ObjectNodePosition, this->getTreeID());
			if (hasBufferZone)
				m_root->hasBufferZone = true;
			double parentMBR[4] = { _curNode->getMBR()[0], _curNode->getMBR()[1], _curNode->getMBR()[2], _curNode->getMBR()[3] };
			double parentVBR[6] = { _curNode->getVBR()[0], _curNode->getVBR()[1], _curNode->getVBR()[2], _curNode->getVBR()[3], _curNode->getVBR()[4], _curNode->getVBR()[5] };

			_curNode->NonleafNodeSplit(_curNode, _insertTime, tmp);
			tmp->setID(m_NodeIDCtrl++);
			//_curNode->UpdateMBRbyExt(_insertTime);//
			//tmp->UpdateMBRbyExt(_insertTime);//
			if (_curNode == m_root)
			{
				TPRNode* tmpRoot = new TPRNode(_insertTime, m_ObjectNodePosition, this->getTreeID());
				if (hasBufferZone)
					m_root->hasBufferZone = true;
				tmpRoot->setID(m_NodeIDCtrl++);
				tmpRoot->setChildNode(_curNode);
				tmpRoot->setChildNode(tmp);
				_curNode->setParent(tmpRoot);
				tmp->setParent(tmpRoot);

				tmp->setLevel(_curNode->getLevel());
				tmpRoot->setLevel(_curNode->getLevel() + 1);

				m_root = tmpRoot;
				m_level = m_root->getLevel();
				m_root->setMBR(parentMBR);
				m_root->setVBR(parentVBR);
				//m_root->UpdateMBRbyExt(_insertTime);//
			}
			else
			{
				tmp->setParent(_curNode->getParent());
				_curNode->getParent()->setChildNode(tmp);
				tmp->setLevel(_curNode->getLevel());
				_curNode->getParent()->setMBR(parentMBR);
				_curNode->getParent()->setVBR(parentVBR);
				//_curNode->getParent()->UpdateMBRbyExt(_insertTime);//
			}

			//tmp->setLevel(_curNode->getChildNode(0)->getLevel()+1);
			//_curNode->setLevel(_curNode->getChildNode(0)->getLevel()+1);
			//_curNode->getParent()->setLevel(_curNode->getLevel() + 1);
			//_curNode->getParent()->UpdateMBRbyExt(_insertTime);

			tmp->UpdateParentNode();
			_curNode->UpdateParentNode();
			_curNode->UpdateMBRbyExt(_insertTime);//
			tmp->UpdateMBRbyExt(_insertTime);//

			m_status = NORMAL;
		}

		//_curNode->setLevel(_curNode->getChildNode(0)->getLevel() + 1); 
		//_curNode->UpdateMBRbyExt(_insertTime);

		m_level = m_root->getLevel();
	}
	//_curNode->UpdateMBRbyExt(_insertTime);

	if (_curNode->getLevel() == 0)
	{
		_curNode->writeEntryFile();
		_curNode->freeEntryMemory();
	}

	return true;
}

bool TPRTree::Insert(CEntry _input)
{

	InsertedTrackInfo inputTrack = getTrackInfo(_input.getID());

	if (inputTrack.ID != -1) {
		return false;
	}


	m_status = NORMAL;
	m_InputEntry.setID(-1);
	double insertTime = _input.getTime();// time(NULL);
	_input.extrapolation(insertTime);
	setTrackInfo(_input);

	/*if (_input.getID() == 30)
		getchar();*/

	if (m_root == NULL)
	{
		m_root = new TPRNode(insertTime, m_ObjectNodePosition, this->getTreeID()); // cskim
		if (hasBufferZone)
			m_root->hasBufferZone = true;
		m_root->allocEntryMemory();

		m_root->Insert(_input);
		m_root->setID(m_NodeIDCtrl++);
		m_root->UpdateMBRbyExt(insertTime);

		m_ObjectNodePosition[_input.getID()] = m_root; // cskim

		m_root->writeEntryFile();
		m_root->freeEntryMemory();
		if(hasBufferZone)
			m_root->setMaxBufferRadius(_input.m_BufferRadius);
	}
	else
	{
		while (true)
		{
			bool bResult = InsertRecursive(m_root, _input, insertTime);

			_input = m_InputEntry;
			if (m_status == NORMAL) {
				break;
			}
		}
	}
	return true;
}

double TPRTree::CalcNodeEnlargeMBR(TPRNode* _underflowNode, TPRNode* _borrowNode)
{
	double underflowNodeMBR[4];
	double borrowNodeMBR[4];
	double enlargeMBR[4];
	//double* underflowNodeMBR;
	//double* borrowNodeMBR;
	//double* enlargeMBR;
	////////////////////////////////////////

	double underflowNodeArea;
	double enlargeArea;

	// Sunghoon
	//underflowNodeMBR = _underflowNode->getMBR();
	//enlargeMBR = _underflowNode->getMBR();
	//borrowNodeMBR = _borrowNode->getMBR();


	for (int i = 0; i < 4; i++)
	{
		underflowNodeMBR[i] = _underflowNode->getMBR()[i];
		enlargeMBR[i] = _underflowNode->getMBR()[i];
		borrowNodeMBR[i] = _borrowNode->getMBR()[i];
	}

	/////////////////////////////////////////
	underflowNodeArea = (underflowNodeMBR[2] - underflowNodeMBR[0]) * (underflowNodeMBR[3] - underflowNodeMBR[1]);

	enlargeMBR[0] = min(enlargeMBR[0], borrowNodeMBR[0]);
	enlargeMBR[1] = min(enlargeMBR[1], borrowNodeMBR[1]);
	enlargeMBR[2] = max(enlargeMBR[2], borrowNodeMBR[2]);
	enlargeMBR[3] = max(enlargeMBR[3], borrowNodeMBR[3]);

	enlargeArea = ((enlargeMBR[2] - enlargeMBR[0]) * (enlargeMBR[3] - enlargeMBR[1])) - underflowNodeArea;

	return enlargeArea;
}

bool TPRTree::isUnderflow(TPRNode* _curNode)
{
	if (_curNode->getNumCntChild() < (NUMNODE * UNDERFLOW_RATIO))		// Sunghoon CAPACITY -> NUMNODE
		return true;

	return false;
}


bool TPRTree::isUnderflowEntry(TPRNode* _curNode)
{
	if (_curNode->getNumEntrys() < (CAPACITY * UNDERFLOW_RATIO)) {
		return true;
	}

	return false;
}

bool TPRTree::Update(CEntry _input) // by CSK
{
	bool ret;

	ret = Delete(_input.getID());

	if (ret == false) return false; // can't find object to update



	return Insert(_input);
}



bool TPRTree::Delete(int _id)
{
	InsertedTrackInfo inputTrack = getTrackInfo(_id);

	if (inputTrack.ID == -1) {
		return false;
	}

	if (m_root == NULL) {
		m_time = -1;
		return false;
	}

	double deleteTime = m_root->getTime();//inputTrack.time;// time(NULL);

	CEntry _input = CEntry();
	_input.setID(inputTrack.ID);
	_input.setX(inputTrack.x);
	_input.setY(inputTrack.y);
	_input.setZ(inputTrack.z);
	_input.setVX(inputTrack.vx);
	_input.setVY(inputTrack.vy);
	_input.setVZ(inputTrack.vz);
	_input.setTime(inputTrack.time);

	_input.extrapolation(deleteTime);

	if (m_root == NULL)
	{
		return false;
	}
	else
	{
		TPRNode* _curNode = m_root;
		bool bUnderflow = false;
		bool deleteResult = false;
		list<CEntry> CEntryList;
		list<CEntry>::iterator it;

		deleteResult = DeleteRecursive(_input, _curNode, bUnderflow, CEntryList, deleteTime);

		if (bUnderflow == true)
		{
			for (it = CEntryList.begin(); it != CEntryList.end(); it++)
			{
				Insert(*it);
			}
		}
		CEntryList.clear();
		if (_curNode->getID() < 0)
			delete _curNode;

		return deleteResult;
	}
}

bool TPRTree::DeleteRecursive(CEntry _input, TPRNode* _curNode, bool& _bUnderflow, list<CEntry>& _CEntryList, double _deleteTime)
{
	bool includedResult = false;
	if (_curNode != NULL)
	{
		if (_curNode->getLevel() == 0)
		{
			_curNode->allocEntryMemory();
			_curNode->readEntryFile();
		}

		// New MBR at the delete time
		_curNode->UpdateMBRbyExt(_deleteTime);

		bool isIncluded = _curNode->Included(_input);
		if (isIncluded)
		{
			if (_curNode->getLevel() == 0)
			{
				for (int i = 0; i < _curNode->getNumEntrys(); i++)
				{
					if (_input.getID() == _curNode->getEntry()[i].getID())
					{
						if (_curNode == m_root && _curNode->getNumEntrys() == 1)
						{
							_curNode->RemoveAllEntry();

							_curNode->writeEntryFile();
							_curNode->freeEntryMemory();

							delete m_root;
							m_root = NULL;

							InsertedTrackList[_input.getID()].ID = -1;

							return true;
						}

						_curNode->getEntry()[i] = _curNode->getEntry()[_curNode->getNumEntrys() - 1];
						CEntry* temp = new CEntry();
						_curNode->getEntry()[_curNode->getNumEntrys() - 1] = *temp;
						delete temp;
						_curNode->setNumEntrys(_curNode->getNumEntrys() - 1);

						m_ObjectNodePosition[_input.getID()] = NULL; // cskim

						InsertedTrackList[_input.getID()].ID = -1;

						if (_curNode->getNumEntrys() < (CAPACITY * UNDERFLOW_RATIO) && _curNode->getID() != m_root->getID())
						{
							_bUnderflow = true;
							int numEntry = _curNode->getNumEntrys();

							for (int k = 0; k < numEntry; k++)
							{
								_CEntryList.push_back(_curNode->getEntry()[k]);

								InsertedTrackList[_curNode->getEntry()[k].getID()].ID = -1;

								m_ObjectNodePosition[_curNode->getEntry()[k].getID()] = NULL; // cskim

							}

							_curNode->RemoveAllEntry();
							_curNode->getParent()->delChildNode(_curNode);

							_curNode->writeEntryFile();
							_curNode->freeEntryMemory();

							delete _curNode;
							_curNode = NULL;
						}
						else {
							// update MBR
							_curNode->UpdateMBRbyExt(_deleteTime);
						}

						includedResult = true;
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < _curNode->getNumCntChild(); i++)
				{
					includedResult = DeleteRecursive(_input, _curNode->getChildNode(i), _bUnderflow, _CEntryList, _deleteTime);

					if (includedResult) {
						_curNode->UpdateMBRbyExt(_deleteTime);
						break;
					}
				}

				if (includedResult == true && _curNode->getNumCntChild() == 1 && _curNode == m_root)
				{
					m_root = _curNode->getChildNode(0);
					m_root->setParent(NULL);
					return true;
				}

				if (includedResult == true && isUnderflow(_curNode) && _curNode != m_root)
				{
					int tmpIndex = -1;

					for (int i = 0; i < _curNode->getParent()->getNumCntChild(); i++)
					{
						if (_curNode != _curNode->getParent()->getChildNode(i))
						{
							if (_curNode->getParent()->getChildNode(i)->getNumCntChild() > (NUMNODE * UNDERFLOW_RATIO) + 1)
							{
								tmpIndex = i;
								break;
							}
						}
					}

					// no borrowable node
					if (tmpIndex == -1)
					{
						int chooseIndex = -1;
						double EnlargeArea = DBL_MAX;
						if (_curNode != m_root)
						{
							for (int i = 0; i < _curNode->getParent()->getNumCntChild(); i++)
							{
								if (_curNode->getParent()->getChildNode(i)->getID() != _curNode->getID())
								{
									double tmpEnlargeArea = CalcNodeEnlargeMBR(_curNode, _curNode->getParent()->getChildNode(i));

									if (tmpEnlargeArea < EnlargeArea)
									{
										EnlargeArea = tmpEnlargeArea;
										chooseIndex = i;
									}
								}
							}

							for (int i = 0; i < _curNode->getNumCntChild(); i++)
							{
								_curNode->getChildNode(i)->setParent(_curNode->getParent()->getChildNode(chooseIndex));
								_curNode->getParent()->getChildNode(chooseIndex)->setChildNode(_curNode->getChildNode(i));
							}
							_curNode->getParent()->getChildNode(chooseIndex)->UpdateMBRbyExt(_deleteTime);

							_curNode->getParent()->delChildNode(_curNode);
							delete _curNode;
							_curNode = NULL;
						}
					}
					else
					{
						int chooseIndex = -1;
						double EnlargeArea = DBL_MAX;
						for (int i = 0; i < _curNode->getParent()->getChildNode(tmpIndex)->getNumCntChild(); i++)
						{
							double tmpEnlargeArea = CalcNodeEnlargeMBR(_curNode, _curNode->getParent()->getChildNode(tmpIndex)->getChildNode(i));
							if (tmpEnlargeArea < EnlargeArea)
							{
								EnlargeArea = tmpEnlargeArea;
								chooseIndex = i;
							}
						}

						_curNode->setChildNode(_curNode->getParent()->getChildNode(tmpIndex)->getChildNode(chooseIndex));
						_curNode->getParent()->getChildNode(tmpIndex)->getChildNode(chooseIndex)->setParent(_curNode);
						_curNode->UpdateMBRbyExt(_deleteTime);

						for (int j = 0; j < _curNode->getNumCntChild(); j++)
						{
							_curNode->getChildNode(j)->setParent(_curNode);
						}

						_curNode->getParent()->getChildNode(tmpIndex)->delChildNode(_curNode->getParent()->getChildNode(tmpIndex)->getChildNode(chooseIndex));
						_curNode->getParent()->getChildNode(tmpIndex)->UpdateMBRbyExt(_deleteTime);
					}
				}
			}
		}

		if (_curNode != NULL && _curNode->getLevel() == 0)
		{
			_curNode->writeEntryFile();
			_curNode->freeEntryMemory();
		}
	}

	return includedResult;
}




void TPRTree::rangeQueryKNN(double centralX, double centralY, double centralZ, double radius, vector<EntryKNN>& KNNEntryList)
{
	vector<CEntry> _CEntryList;
	unsigned long _st = 0;
	unsigned long _ed = 0;
	double minDistanceXYZ = DBL_MAX;
	int _rsltcnt = 0;
	vector<CEntry>::iterator myEntry;
	CEntry CloseEntry;

	double queryTime = m_root->getTime();// time(NULL); // CSK

	m_root->rangeQueryKNN(centralX, centralY, radius, _st, _ed, _rsltcnt, _CEntryList, queryTime);
	int count = 0;

	vector<EntryKNN> result;

	for (vector<CEntry>::iterator it = _CEntryList.begin(); it != _CEntryList.end(); ++it)
	{
		EntryKNN tempStruct;
		myEntry = it;

		double entryDistanceXY = CICMSCalculator::GetDistance(centralX, centralY, myEntry->getX(), myEntry->getY());
		double entryDistanceZ;
		if (centralZ - myEntry->getZ() < 0)
			entryDistanceZ = -1 * (centralZ - myEntry->getZ());
		else
			entryDistanceZ = centralZ - myEntry->getZ();
		double xy2 = pow(entryDistanceXY, 2);
		double z2 = pow(entryDistanceZ, 2);
		double entryDistanceXYZ = sqrt(xy2 + z2);


		tempStruct.cEntry = *it;
		tempStruct.DistanceXYZ = entryDistanceXYZ;
		KNNEntryList.push_back(tempStruct);


		if (minDistanceXYZ > entryDistanceXYZ)
		{
			minDistanceXYZ = entryDistanceXYZ;
			CloseEntry = *myEntry;
		}
		count++;
	}

	//	KNNEntryList.sort(KNNCompare);
	result = KNNEntryList;
}

void TPRTree::rangeQueryKNN4(double centralX, double centralY, double centralZ, double radius, vector<CEntry>& KNNEntryList, double queryTime)
{
	unsigned long _st = 0;
	unsigned long _ed = 0;
	int _rsltcnt = 0;
	if (m_root == NULL) return;
	m_root->rangeQueryKNN(centralX, centralY, radius, _st, _ed, _rsltcnt, KNNEntryList, queryTime); // for test CSK.
}

void TPRTree::setTrackInfo(CEntry _input)
{
	InsertedTrackList[_input.getID()].ID = _input.getID();
	InsertedTrackList[_input.getID()].x = _input.getX();
	InsertedTrackList[_input.getID()].y = _input.getY();
	InsertedTrackList[_input.getID()].z = _input.getZ();
	InsertedTrackList[_input.getID()].vx = _input.getVX();
	InsertedTrackList[_input.getID()].vy = _input.getVY();
	InsertedTrackList[_input.getID()].vz = _input.getVZ();
	InsertedTrackList[_input.getID()].time = _input.getTime();
}

InsertedTrackInfo TPRTree::getTrackInfo(int id)
{
	return InsertedTrackList[id];
}

void TPRTree::FindOverlapping(vector<CEntry*>& result, vector<CEntry*>& vesselResult, TPRTree* targetTree, double queryTime)
{
	if (m_root == NULL || targetTree->m_root == NULL)
		return;

	//TPRNode* myNode = m_root;
	//TPRNode* targetNode = targetTree->m_root;
	//checking from the root
	double myMBR[4], targetMBR[4];
	m_root->extfuture_mbr_of_node(myMBR, m_root, queryTime, 0);
	targetTree->m_root->extfuture_mbr_of_node(targetMBR, targetTree->m_root, queryTime, 0);

	bool isOverlapping = !((myMBR[0] > targetMBR[2] || targetMBR[0] > myMBR[2]) || (myMBR[1] > targetMBR[3] || targetMBR[1] > myMBR[3]));
	if (isOverlapping) {
		for (int j = 0; j < m_root->getNumCntChild(); j++) {
			for (int k = 0; k < targetTree->m_root->getNumCntChild(); k++) {
				m_root->m_childNode[j]->FindOverlappingRecursive(result, vesselResult, targetTree->m_root->m_childNode[k], queryTime);
			}
		}
	}
	return;
}