#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <list>
#include <time.h>
#include <windows.h>
#include <iomanip>

using namespace std;

#define INF 10000

char output_file[] = "output.txt";

double RANDOM_BEGIN = -5;
double RANDOM_END = 5;

CHAR nameFirstMutex[] = "MutexQueue";
CHAR nameSecondMutex[] = "MutexStat";

list<string> taskAnswers;

unsigned int minTime = INFINITE, maxTime = 0;
unsigned int solvedTrue = 0, solvedFalse = 0;
unsigned int* arrayOfCount = NULL;
unsigned int* arrayOfTime = NULL;

HANDLE hMutexQueue, hMutexStat;
unsigned int THREAD_ID = 0;

struct Point {
	double x;
	double y;

	Point(double _x, double _y) : x(_x), y(_y) {}

	Point() {}

	friend ostream &operator<<(ostream &os, const Point &point) {
		os << "(" << point.x << ", " << point.y << ")";
		return os;
	}
	string toString() {
		return "(" + to_string(x) + ", " + to_string(y) + ")";
	}

	static bool onSegment(Point p, Point q, Point r)
	{
		if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
			q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
			return true;
		return false;
	}

	static int orientation(Point p, Point q, Point r)
	{
		double val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0; 
		return (val > 0) ? 1 : 2; 
	}

	static bool doIntersect(Point p1, Point q1, Point p2, Point q2)
	{
		int o1 = orientation(p1, q1, p2);
		int o2 = orientation(p1, q1, q2);
		int o3 = orientation(p2, q2, p1);
		int o4 = orientation(p2, q2, q1);

		if (o1 != o2 && o3 != o4)
			return true;

		if (o1 == 0 && onSegment(p1, p2, q1)) return true;
		if (o2 == 0 && onSegment(p1, q2, q1)) return true;
		if (o3 == 0 && onSegment(p2, p1, q2)) return true;
		if (o4 == 0 && onSegment(p2, q1, q2)) return true;

		return false;
	}

	static Point randomPoint(double begin, double end) {
		double x = ((double)rand() / RAND_MAX) * (end - begin) + begin;
		double y = ((double)rand() / RAND_MAX) * (end - begin) + begin;

		return Point(x, y);
	}

};

struct Data {
	static const unsigned int count = 4;

	Point *quadrilateral;
	Point point;

	Data() {
		quadrilateral = new Point[count];
	}

	Data(Point *_quadrilateral, Point _point) : quadrilateral(_quadrilateral), point(_point) {}

	friend ostream &operator<<(ostream &os, const Data& data) {
		os << "quadrilateral: ";
		for (int i = 0; i < count; i++) {
			os << data.quadrilateral[i] << " ";
		}
		os << "\npoint: " << data.point;
		return os;
	}

	string toString() {
		string s ="";
		for (int i = 0; i < count; i++) {
			s += quadrilateral[i].toString() + " ";
		}
		s += point.toString();
		return s;
	}

	~Data() {
	}

	static Data randomData(double begin, double end) {
		Data random = Data();
		for (int i = 0; i < count; ++i) {
			random.quadrilateral[i] = Point::randomPoint(begin, end);
		}
		random.point = Point::randomPoint(begin, end);
		return random;
	}

	bool isInside()
	{
		Point extreme(INF, point.y);

		int count = 0, i = 0;
		do
		{
			int next = (i + 1) % 4;

			if (Point::doIntersect(quadrilateral[i], quadrilateral[next], point, extreme))
			{
				if (Point::orientation(quadrilateral[i], point, quadrilateral[next]) == 0)
					return Point::onSegment(quadrilateral[i], point, quadrilateral[next]);

				count++;
			}
			i = next;
		} while (i != 0);

		return (count % 2 == 1);
	}

};

queue<Data> randomTasks(int count, double begin, double end) {
	queue<Data> tasks;
	for (int i = 0; i < count; i++) {
		tasks.push(Data::randomData(begin, end));
	}
	return tasks;
}

queue<Data> tasks;

DWORD WINAPI run(LPVOID lpParameters) {

	Data* data;
	//---------------------------begin of synchronized section
	DWORD dwWaitResult = WaitForSingleObject(hMutexQueue, INFINITE);

	int ID = THREAD_ID++;

	while (!tasks.empty()) {
		unsigned int timeBegin = GetTickCount();
		data = &tasks.front();
		string str = data->toString();
		if (data->isInside()) {
			str += " 1";
			solvedTrue++;
		}
		else {
			str += " 0";
			solvedFalse++;
		}
		tasks.pop();
		unsigned int timeEnd = GetTickCount() - timeBegin;

		ReleaseMutex(hMutexQueue);
		dwWaitResult = WaitForSingleObject(hMutexStat, INFINITE);

		if (maxTime < timeEnd) {
			maxTime = timeEnd;
		}
		if (minTime > timeEnd) {
			minTime = timeEnd;
		}

		arrayOfCount[ID]++;
		arrayOfTime[ID] += timeEnd;
		taskAnswers.push_back(str);

		ReleaseMutex(hMutexStat);
		DWORD dwWaitResult = WaitForSingleObject(hMutexQueue, INFINITE);
	}
	ReleaseMutex(hMutexQueue);
	//---------------------------end of synchronized section

	return 0;
}

int main()
{
	HANDLE* hThreads;
	DWORD idThread;

	int taskCount = 0;
	int threadCount = 0;
	
	cout << "Enter task count: ";
	cin >> taskCount;
	
	cout << "Enter thread count: ";
	cin >> threadCount;
	
	if (taskCount <= 0 || threadCount <= 0) {
		perror("Wrong params!\n");
		return 2;
	}

	srand(time(NULL));
	tasks = randomTasks(taskCount, RANDOM_BEGIN, RANDOM_END);

	hMutexQueue = CreateMutex(NULL, FALSE, NULL);
	hMutexStat = CreateMutex(NULL, FALSE, NULL);

	arrayOfCount = new unsigned int[threadCount];
	arrayOfTime = new unsigned int[threadCount];
	for (int i = 0; i < threadCount; i++) {
		arrayOfCount[i] = 0;
		arrayOfTime[i] = 0;
	}

	hThreads = new HANDLE[threadCount];

	for (int i = 0; i < threadCount; i++) {
		hThreads[i] = CreateThread(NULL, NULL, run, NULL, NULL, &idThread);
		if (hThreads[i] == NULL) {
			printf("Can't create thread. Error - %d", GetLastError());
			return 1;
		}
	}
	WaitForMultipleObjects(threadCount, hThreads, TRUE, INFINITE);
	for (int i = 0; i < threadCount; i++) {
		CloseHandle(hThreads[i]);
	}
	CloseHandle(hMutexQueue);

	unsigned int writeTime = GetTickCount();
	ofstream fout(output_file);
	fout << taskAnswers.size() << endl;
	for (list<string>::iterator it = taskAnswers.begin(); it != taskAnswers.end(); ++it) {
		fout << *it << endl;
	}
	writeTime = GetTickCount() - writeTime;

	cout << "Task count: " << taskCount << "\tThread count: " << threadCount << endl;
	cout << "Min time solving task: " << minTime << " ms" << endl;
	cout << "Max time solving task: " << maxTime << " ms" << endl;
	cout << "Count of tasks ends with true: " << solvedTrue << endl;
	cout << "Count of tasks ends with false: " << solvedFalse << endl << endl;
	cout << "Time for writing to file = " << writeTime << " ms" << endl;
 	for (int i = 0; i < threadCount; i++) {
		cout << "Thread " << i << " solved " << arrayOfCount[i] << " tasks for " << arrayOfTime[i] << " ms" << endl;
	}
	cout << endl << endl;

	system("pause");
	delete[] arrayOfCount;
	delete[] arrayOfTime;
	return 0;
}


