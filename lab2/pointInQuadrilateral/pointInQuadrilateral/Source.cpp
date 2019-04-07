#include <iostream>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <vector>
#include <windows.h>

#define INF 10000

using namespace std;

const unsigned int count_of_coordinates = 2;
char input_file[] = "input.txt";
char output_file[] = "output.txt";

struct Point {
	double x;
	double y;

	Point(double _x, double _y) : x(_x), y(_y) {}

	Point() {}

	friend ostream &operator<<(ostream &os, const Point &point) {
		os << "(" << point.x << ", " << point.y << ")";
		return os;
	}

	//Учитывая три коллинеарные точки p, q, r, функция проверяет, лежит ли точка q на отрезке 'pr'
	static bool onSegment(Point p, Point q, Point r)
	{
		if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
			q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
			return true;
		return false;
	}

	// Найти ориентацию упорядоченного триплета (p, q, r). 
	// Функция возвращает следующие значения 
	// 0 -> p, q и r коллинеарны 
	// 1 -> По часовой стрелке 
	// 2 -> против часовой стрелки
	static int orientation(Point p, Point q, Point r)
	{
		double val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0; // коллинеарны
		return (val > 0) ? 1 : 2; // по или против часовой
	}

	// Функция, которая возвращает true, если отрезок линии 'p1q1' и 'p2q2' пересекаются
	static bool doIntersect(Point p1, Point q1, Point p2, Point q2)
	{
		// Поиск ориентации, необходимых для общего и особых случаев
		int o1 = orientation(p1, q1, p2);
		int o2 = orientation(p1, q1, q2);
		int o3 = orientation(p2, q2, p1);
		int o4 = orientation(p2, q2, q1);

		// Общий случай
		if (o1 != o2 && o3 != o4)
			return true;

		// Особые случаев
		// p1, q1 и p2 коллинеарны, а p2 лежит на отрезке p1q1
		if (o1 == 0 && onSegment(p1, p2, q1)) return true;
		// p1, q1 и p2 коллинеарны, а q2 лежит на отрезке p1q1
		if (o2 == 0 && onSegment(p1, q2, q1)) return true;
		// p2, q2 и p1 коллинеарны, а p1 лежит на отрезке p2q2
		if (o3 == 0 && onSegment(p2, p1, q2)) return true;
		// p2, q2 и q1 коллинеарны, а q1 лежит на отрезке p2q2
		if (o4 == 0 && onSegment(p2, q1, q2)) return true;

		return false; // Не попадает ни в один из вышеперечисленных случаев
	}

};

struct Data {
	static const unsigned int count = 4;

	Point *quadrilateral;
	Point point;

	Data(Point *_quadrilateral, Point _point) : quadrilateral(_quadrilateral), point(_point) {}

	friend ostream &operator<<(ostream &os, const Data& data) {
		os << "quadrilateral: ";
		for (int i = 0; i < count; i++) {
			os << data.quadrilateral[i] << " ";
		}
		os << "\npoint: " << data.point;
		return os;
	}

	Data() { quadrilateral = new Point[count]; }

	~Data() {
	}

	bool isInside()
	{
		// Создаем точку для отрезка линии от p до бесконечности(условно)
		Point extreme(INF, point.y);

		// Считаем пересечения указанной линии со сторонами четырехугольника
		int count = 0, i = 0;
		do
		{
			int next = (i + 1) % 4;

			// Проверяем, пересекается ли отрезок линии от 'p' до 'extreme' 
			// с отрезком линии от 'quadrilateral[i]' до 'quadrilateral[next]'
			if (Point::doIntersect(quadrilateral[i], quadrilateral[next], point, extreme))
			{
				// Если точка 'p' является коллинеарной с отрезком линии 'i-next', 
				// проверяем, лежит ли он на отрезке.
				if (Point::orientation(quadrilateral[i], point, quadrilateral[next]) == 0)
					return Point::onSegment(quadrilateral[i], point, quadrilateral[next]);

				count++;
			}
			i = next;
		} while (i != 0);

		// Возвращаем true, если count нечетно, false, иначе
		return (count % 2 == 1); 
	}


};

Data read_from_file(char *input_file) {
	Data data;
	try {
		ifstream fin(input_file);

		if (!fin.is_open()) {
			throw "Input file doesn't exist";
		}

		double x, y;
		for (int i = 0; i < Data::count; i++) {	
			fin >> x;
			fin >> y;
			data.quadrilateral[i] = Point(x, y);
		}
		fin >> x;
		fin >> y;
		data.point = Point(x, y);

		fin.close();

	}
	catch (const char *str) {
		cout << str << endl;
	}
	catch (...) {                                                          
		cout << "Unknown error" << endl;
	}

	return data;
}

void solution(char *input_file, char *output_file) {
	unsigned int begin = GetTickCount();

	Data data = read_from_file(input_file);

	try {
		ofstream fout(output_file);

		if (!fout.good()) {
			throw "Output file can't be opened";
		}

		fout << data.isInside() << endl;
		
		fout.close();
	}
	catch (const char *str) {
		cout << str << endl;
	}
	catch (...) {                                                         
		cout << "Unknown error" << endl;
	}
	
	cout << ">> Time: " << ((double)GetTickCount() - begin) / 1000 << " seconds spent" << endl;
	cout << data << endl;
}


DWORD WINAPI run(LPVOID iNum);

int main()
{
	HANDLE hThread;
	unsigned long idThread;

	hThread = CreateThread(NULL, 0, run, NULL, 0, &idThread);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	system("pause");
	return 0;
}

DWORD WINAPI run(LPVOID iNum) {
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
	solution(input_file, output_file);
	return 0;
}
