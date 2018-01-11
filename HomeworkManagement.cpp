#include <time.h>
#include <limits.h>
#include <iostream>
using namespace std;

enum COURSE
{
	CHE102,
	MATH115,
	MATH117,
	ECE105,
	ECE150,
	ECE190,
	Null
};

struct Assignment
{
	COURSE course;
	int assnNum;         // Assignment number
	int dueMonth;        // 1-12
	int dueDay;          // 1-31
	char*  description;  // Assignment description
};


struct HQ
{
	Assignment* assn;
	HQ* nextInQueue;
};


class HomeworkQueue
{
public:
bool                enqueue (const Assignment& assignment);
const Assignment*   dequeue ();
int                 daysTillDue (const COURSE course);
const Assignment*   dueIn (const int numDays);


HomeworkQueue ();       //constructor
~HomeworkQueue ();      //destructor

private:

bool isEarlier (const Assignment a1, const Assignment a2);
Assignment* copyAssn (const Assignment& assignment);
void getTime (int &month, int &day);

HQ* queue;
};

HomeworkQueue::HomeworkQueue (){
	queue = NULL;
}
bool HomeworkQueue::isEarlier (const Assignment a1, const Assignment a2)
{
	if (a1.dueMonth < a2.dueMonth) {
		return true;
	}
	if (a1.dueDay < a2.dueDay) {
		return true;
	}
	return false;
}

Assignment* HomeworkQueue::copyAssn (const Assignment& assignment){
	Assignment* n = new Assignment;
	n->course = assignment.course;
	n->assnNum = assignment.assnNum;
	n->dueDay = assignment.dueDay;
	n->dueMonth = assignment.dueMonth;
	if (assignment.description == 0) {
		n->description = 0;
	} else {
		char* desc = new char[256];
		int i = 0;
		while (assignment.description[i] && i < 255) {
			desc[i] = assignment.description[i];
			i++;
		}
		desc[i] = 0;
		n->description = desc;
	}
	return n;
}

void HomeworkQueue::getTime (int &month, int &day){
	time_t rawtime;
	struct tm *timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	month = timeinfo->tm_mon + 1;
	day = timeinfo->tm_mday;
}

bool HomeworkQueue::enqueue (const Assignment& assignment)
{
	if (assignment.course == Null)
		return false;
	if (assignment.dueMonth < 1 || assignment.dueMonth > 12)
		return false;
	if (assignment.dueDay < 1 || assignment.dueDay > 31)
		return false;
	if (assignment.description == NULL || assignment.description == '\0')
		return false;
	int i = 0;
	if (queue == NULL) {
		HQ* n = new HQ;
		n->assn = copyAssn (assignment);
		n->nextInQueue = 0;
		queue = n;
		return true;
	}
	if (queue->assn == NULL) {
		queue->assn = copyAssn (assignment);
		return true;
	}
	if (isEarlier ( assignment, *(queue->assn))) {
		HQ* n = new HQ;
		n->assn = copyAssn (assignment);
		n->nextInQueue = queue;
		queue = n;
		return true;
	}

	HQ *q = queue;
	while (q->nextInQueue && i++ < 9000) {
		if (q->nextInQueue->assn == NULL) {
			return false;
		}
		if (q->assn->assnNum == assignment.assnNum &&
		    q->assn->course == assignment.course) {
			return false;
		}
		if (isEarlier (assignment, *(q->nextInQueue->assn))) {
			HQ* n = new HQ;
			n->assn = copyAssn (assignment);
			n->nextInQueue = q->nextInQueue->nextInQueue;
			q->nextInQueue->nextInQueue = n;
			return true;
		}
		q = q->nextInQueue;
	}
	if (q->nextInQueue == NULL) {
		HQ* n = new HQ;
		n->assn = copyAssn (assignment);
		n->nextInQueue = 0;
		q->nextInQueue = n;
		return true;
	}
	return false;
}

const Assignment* HomeworkQueue::dequeue ()
{
	if (!queue) {
		return NULL;
	}

	Assignment* rmAssn = queue->assn;
	HQ* rmNode = queue;
	queue = queue->nextInQueue;
	delete rmNode;
	return rmAssn;
}

int HomeworkQueue::daysTillDue (const COURSE course)
{
	int month = 13;
	int day = 32;
	getTime (month, day);
	int assnMonth = -1;
	int assnDay = -1;

	int count = 0;
	const HQ *pointerino = queue;
	while (pointerino && count++ < 9000) {
		if (pointerino->assn == NULL) {
			return -1;
		}

		if (pointerino->assn->course == course) {
			assnMonth = pointerino->assn->dueMonth;
			assnDay = pointerino->assn->dueDay;
			if (assnMonth < 1 || assnMonth > 12)
				return -1;
			if (assnDay < 1 || assnDay > 31)
				return -1;
			pointerino = NULL;
		}
		if (pointerino != NULL) {
			if (pointerino->nextInQueue == NULL)
				pointerino = 0;
			else
				pointerino = pointerino->nextInQueue;
		}
	}

	//no assn found
	if (assnMonth == -1 && assnDay == -1) {
		return INT_MAX;
	}

	if (assnMonth < month) {
		return -1;
	}
	if (assnMonth == month && assnDay < day) {
		return -1;
	}
	return (assnMonth - month) * 30 + (assnDay - day);
}

const Assignment* HomeworkQueue::dueIn (const int numDays)
{
	int count = 0;
	int month = 13;
	int day = 32;
	getTime (month, day);

	int fail = 0;

	const HQ *pointerino = queue;
	while (pointerino && fail++ < 9000) {
		if (pointerino->assn == NULL) {
			break;
		}
		int assnMonth = pointerino->assn->dueMonth;
		int assnDay = pointerino->assn->dueDay;
		if (assnMonth < 1 || assnMonth > 12)
			break;
		if (assnDay < 1 || assnDay > 31)
			break;
		if (assnMonth < month) {
			break;
		}
		if (assnMonth == month && assnDay < day) {
			break;
		}
		if ((assnMonth * 30 + assnDay) - (month * 30 + day) <= numDays) {
			count++;
		}
		pointerino = pointerino->nextInQueue;
	}
	Assignment* dues = new Assignment[count + 1];
	int i = 0;
	pointerino = queue;
	for (int i = 0; i < count; i++) {
		dues[i] = *copyAssn (*(pointerino->assn));
		pointerino = pointerino->nextInQueue;
	}
	dues[count].course = Null;
	return dues;
}

HomeworkQueue::~HomeworkQueue ()
{
	int i = 0;
	while (queue && i++ < 1000) {
		dequeue ();
	}
}


int main (const int argc, const char* const args[])
{
	HomeworkQueue hwq;

	char description[] = "This is a test";

	Assignment someAssignment =
	{
		.course = CHE102,
		.assnNum = 1000000,
		.dueMonth = 12,
		.dueDay = 4,
		.description = description,
	};

	if (!hwq.enqueue (someAssignment))
	{
		cout << "enqueue() unsuccessful" << endl;
	}
	else
	{
		cout << "enqueue() successful" << endl;
	}

	const Assignment* assnInTheQueue = hwq.dequeue ();

	if (!assnInTheQueue)
	{
		cout << "dequeue() unsuccessful" << endl;

	}
	else
	{
		cout << "dequeue() successful" << endl;
	}

	return 0;
}
