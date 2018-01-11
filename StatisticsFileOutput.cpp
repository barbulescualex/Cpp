#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;

struct Student{
	int studentID;
	int grade;
};

struct Dataset{
	int numStudents;
	Student* students;
};

struct Rejects{
	int numRejects;
	int* rejects;
};

struct Mode{
	int numModes;
	int* modes;
};

struct Statistics{
	int   minimum;
	float average;
	int   maximum;
	float popStdDev;
	float smplStdDev;
	Mode  mode;
	int   histogram[10];
};

int readCSV(const char filename[],const int minAcceptableID, 
				const int maxAcceptableID,Dataset& data, 
				Rejects& rejects);

int computeStatistics(Dataset& data, Statistics& stats);

int writeCSV(const char filename[], const Statistics& stats);


enum states { UNKNOWN1, UNKNOWN2, UNKNOWN3, UNKNOWN4, STUDENTID, COMMA, GRADE};
int maxNumErrors = 10;

int readCSV(const char filename[],const int minAcceptableID, 
				const int maxAcceptableID,Dataset& data, 
				Rejects& rejects){
	if (minAcceptableID > maxAcceptableID)
		return -1;

	states currentState;
	rejects.numRejects = 0;
	rejects.rejects = new int[maxNumErrors];
	const int maxLineLength = 100;
	char line[maxLineLength];

	ifstream infile;

	int stringLength = 0;

	for (int q = 0; filename[q] != 0; q++) {
		if (filename[q] == '.' && !(filename[q+1] == 'c' && filename[q+2] == 's' && filename[q+3] == 'v'))
			return -1;
		else if (filename[q] == '.' && filename[q+1] == 'c')
			break;
		else if (filename[q] == '.'){
			stringLength = q;
			break;
		} else if (filename[q+1] == 0){
			stringLength = q + 1;
			break;
		}
	}

	if (stringLength == 0) {
		infile.open(filename);
	} else {
		char newFileName[stringLength+4];
		for (int f = 0; f < stringLength; f++) {
			newFileName[f] = filename[f];
		}

		newFileName[stringLength] = '.';
		newFileName[stringLength+1] = 'c';
		newFileName[stringLength+2] = 's';
		newFileName[stringLength+3] = 'v';
		newFileName[stringLength+4] = '\0';

		infile.open(newFileName);
	}
	if (infile.fail())
		return -1;

	bool done = false;
	bool ignoreLine = false;
	bool invalidCharacter = false;
	int fileLineNumber = 0;
	int studentIndex = -1;

	data.numStudents = 1000;
	data.students = new Student[data.numStudents];

	while (!done) {
		++fileLineNumber;
		if (!infile.getline(line, maxLineLength)) {
			if (infile.eof()) {
				break;
			} else {
				return -1;
			}
		}

		++studentIndex;

		if (studentIndex >= data.numStudents) {
			data.numStudents = data.numStudents += 1;
			Student* temp = new Student[data.numStudents];
			for (int j = 0; j < studentIndex; j++) {
				temp[j] = data.students[j];
			}
			delete[] data.students;
			data.students = temp;
		}

		data.students[studentIndex].studentID = 0;
		data.students[studentIndex].grade = 0;
		invalidCharacter = false;
		ignoreLine = false;

		for (int i = 0; i < maxLineLength-1; i++) {
			if (i == 0) {
				if (line[0] >= '0' && line[0] <= '9') {
					currentState = STUDENTID;
				} else {
					currentState = UNKNOWN1;
				}
			}

			switch(currentState) {
				case UNKNOWN1:
					if (line[i] >= '0' && line[i] <= '9') {
						data.students[studentIndex].studentID = (data.students[studentIndex].studentID * 10) + (line[i]-48);
						currentState = STUDENTID;
					} else if (line[i] == '\0' || line[i] == '\r') {
						i = maxLineLength;
						ignoreLine = true;
					} else if (!(line[i] == ' ' || line[i] == '\t')) {
						invalidCharacter = true;
						i = maxLineLength;
					}
					break;
				case STUDENTID:
					if (line[i] >= '0' && line[i] <= '9') {
						data.students[studentIndex].studentID = (data.students[studentIndex].studentID * 10) + (line[i]-48);
					} else if (line[i] == ' ' || line[i] == '\t') {
						currentState = UNKNOWN2;
					} else if (line[i] == ',') {
						currentState = COMMA;
					} else {
						invalidCharacter = true;
						i = maxLineLength;
					}
					break;
				case UNKNOWN2:
					if (line[i] == ',') {
						currentState = COMMA;
					} else if (!(line[i] == ' ' || line[i] == '\t')) {
						invalidCharacter = true;
						i=maxLineLength;
					}
					break;
				case COMMA:
					if (line[i] == ' ' || line[i] == '\t') {
						currentState = UNKNOWN3;
					} else if (line[i] - 48 >= 0 && line[i]-48 <= 9){
						currentState = GRADE;
						data.students[studentIndex].grade = (data.students[studentIndex].grade * 10) + (line[i]-48);
					} else {
						invalidCharacter = true;
						i = maxLineLength;
					}
					break;
				case UNKNOWN3:
					if (line[i] - 48 >= 0 && line[i]-48 <= 9) {
						currentState = GRADE;
						data.students[studentIndex].grade = (data.students[studentIndex].grade * 10) + (line[i]-48);
					} else if (!(line[i] == ' ' || line[i] == '\t')) {
						invalidCharacter = true;
						i = maxLineLength;
					}
					break;
				case GRADE:
					if (line[i] == ' ' || line[i] == '\t') {
						currentState = UNKNOWN4;
					} else if (line[i] == '\0' || line[i] == '\r') {
						i = maxLineLength;
					} else if (line[i] - 48 >= 0 && line[i]-48 <= 9) {
						data.students[studentIndex].grade = (data.students[studentIndex].grade * 10) + (line[i]-48);
					} else {
						invalidCharacter = true;
						i = maxLineLength;
					}
					break;
				case UNKNOWN4:
					if (line[i] == '\0') {
						i = maxLineLength;
					} else if (!(line[i] == ' ' || line[i] == '\t')) {
						invalidCharacter = true;
						i = maxLineLength;
					}
					break;
				}
			}
		if ((data.students[studentIndex].studentID <= minAcceptableID || data.students[studentIndex].studentID >= maxAcceptableID || (data.students[studentIndex].grade > 100 || data.students[studentIndex].grade < 0) || invalidCharacter) && !ignoreLine) {
			rejects.rejects[rejects.numRejects] = fileLineNumber;
			rejects.numRejects++;
			if (rejects.numRejects >= maxNumErrors) {
				data.numStudents = 0;
				return -1;
			}
			studentIndex--;
		}

		if (ignoreLine) {
			studentIndex--;
		}	
	}

	data.numStudents = studentIndex + 1;

	return rejects.numRejects;

}

int min(Dataset &data) {
	int min = data.students[0].grade;
	for (int i = 1; i < data.numStudents; i++) {
		if (data.students[i].grade < min)
			min = data.students[i].grade;
	}
	return min;
}

float avg(Dataset &data) {
	float sum = 0;
	for (int i = 0; i < data.numStudents; i++) {
		sum += data.students[i].grade;
	}
	return sum / (float)data.numStudents;
}

int max(Dataset &data) {
	int max = data.students[0].grade;
	for (int i = 1; i < data.numStudents; i++) {
		if (data.students[i].grade > max)
			max = data.students[i].grade;
	}
	return max;
}

float populationStdDev(Dataset &data) {
	float average = avg(data);
	float sum = 0;
	
	for (int i = 0; i < data.numStudents; i++) {
		sum += pow((data.students[i].grade - average),2);
	}
	
	return sqrt(sum / (float)data.numStudents);
}

float sampleStdDev(Dataset &data) {	
	float average = avg(data);
	float sum = 0;
	
	for (int i = 0; i < data.numStudents; i++) {
		sum += pow((data.students[i].grade - average),2);
	}
	
	return sqrt(sum / (float)(data.numStudents-1));
}

void computeHistogram(int histogram[], Dataset &data) {
	for (int i = 0; i < 10; i++) {
		histogram[i] = 0;
	}

	for (int i = 0; i < data.numStudents; i++) {
		if (data.students[i].grade == 100)
			histogram[9] = histogram[9] + 1;
		else
			histogram[(data.students[i].grade)/10] = histogram[(data.students[i].grade)/10] + 1;
	}
}

void quicksort(int dataset[], int low, int high) {
    int left = low, right = high;
    int temp;
    int pivot = dataset[(low + high) / 2];

    while (left <= right) {
		while (dataset[left] < pivot)
            left++;
        while (dataset[right] > pivot)
            right--;
        if (left <= right) {
            temp = dataset[left];
            dataset[left] = dataset[right];
            dataset[right] = temp;
            left++;
            right--;
        }
    };
	  
    if (low < right)
        quicksort(dataset, low, right);
    if (left < high)
        quicksort(dataset, left, high);
}

void computeMode(Dataset &data, Statistics &stats){
	int grades[data.numStudents];
	
	for (int i = 0; i < data.numStudents; i++) {
		grades[i] = data.students[i].grade;
	}

	quicksort(grades, 0, data.numStudents-1);

	int tempNum = grades[0];
	int currentCount = 0;
	int maxCount = 0;
	stats.mode.numModes = 0;
	int tempModes[data.numStudents];
	
	for (int j = 0; j < data.numStudents; j++) {
		if (tempNum == grades[j]) {
			++currentCount;
		} else {
			if (currentCount > maxCount) {
				for (int k = 0; k < stats.mode.numModes; k++) {
					tempModes[k] = 0;
				}
				stats.mode.numModes = 0;
				tempModes[stats.mode.numModes] = tempNum;
				++stats.mode.numModes;
				maxCount = currentCount;
			} else if (currentCount == maxCount) {
				tempModes[stats.mode.numModes] = tempNum;
				++stats.mode.numModes;
			}
			tempNum = grades[j];
			currentCount = 1;
		}
	}
	
	if (currentCount > maxCount) {
		for (int k = 0; k < stats.mode.numModes; k++) {
			tempModes[k] = 0;
		}
		stats.mode.numModes = 0;
		maxCount = currentCount;
		tempModes[stats.mode.numModes] = tempNum;
		++stats.mode.numModes;
	} else if (currentCount == maxCount) {
		tempModes[stats.mode.numModes] = tempNum;
		++stats.mode.numModes;
	}

	stats.mode.modes = new int[stats.mode.numModes-1];

	for (int l = 0; l < stats.mode.numModes; l++) {
		stats.mode.modes[l] = tempModes[l];
	}
	
}

int computeStatistics(Dataset& data, Statistics& stats){
	if (data.numStudents < 2)
		return -1;

	stats.average = avg(data);
	stats.minimum = min(data);
	stats.maximum = max(data);
	stats.popStdDev = populationStdDev(data);
	stats.smplStdDev = sampleStdDev(data);
	computeHistogram(stats.histogram, data);
	computeMode(data, stats);
	return 0;
}

int writeCSV(const char filename[], const Statistics& stats){
	ofstream outfile;

	int stringLength = 0;

	for (int q = 0; filename[q] != 0; q++) {
		if (filename[q] == '.' && !(filename[q+1] == 'c' && filename[q+2] == 's' && filename[q+3] == 'v')) {
			return -1;
		} else if (filename[q] == '.' && filename[q+1] == 'c') {
			stringLength = q + 6;
			break;
		} else if (filename[q+1] == 0) {
			stringLength = q + 7;
			break;
		}
	}

	char newFileName[stringLength];

	for (int f = 0; f < stringLength - 5; f++) {
		newFileName[f] = filename[f];
	}

	newFileName[stringLength-6] = '.';
	newFileName[stringLength-5] = 's';
	newFileName[stringLength-4] = 't';
	newFileName[stringLength-3] = 'a';
	newFileName[stringLength-2] = 't';
	newFileName[stringLength-1] = 0;

	outfile.open(newFileName);
	if(!outfile.is_open())
		return -1;

	outfile <<
	"Minimum: " << stats.minimum <<
	"\nAverage: " << stats.average <<
	"\nMaximum: " << stats.maximum <<
	"\nPopulation Standard Deviation: " << stats.popStdDev <<
	"\nSample Standard Deviation: " << stats.smplStdDev;
	
	outfile << "\nModes: ";

	if (stats.mode.numModes > 0) {
		for (int j = 0; j < stats.mode.numModes; j++) {
			if (j != stats.mode.numModes-1)
				outfile << stats.mode.modes[j] << ", ";
			else
				outfile << stats.mode.modes[j];
		}
	}

	outfile << "\nHistogram: " <<
	"\n[0-9]: " << stats.histogram[0] <<
	"\n[10-19]: " << stats.histogram[1] <<
	"\n[20-29]: " << stats.histogram[2] <<
	"\n[30-39]: " << stats.histogram[3] <<
	"\n[40-49]: " << stats.histogram[4] <<
	"\n[50-59]: " << stats.histogram[5] <<
	"\n[60-69]: " << stats.histogram[6] <<
	"\n[70-79]: " << stats.histogram[7] <<
	"\n[80-89]: " << stats.histogram[8] <<
	"\n[90-100]: " << stats.histogram[9];

	outfile.close();

	return 0;
}



#define isNaN(X) (X != X)

int main(const int argc, const char* const argv[]) {
  	
	int minAcceptableID = 0;
	int maxAcceptableID = 24000000;

	Dataset data = {0};
	Rejects rejects = {0};
	Statistics stats = {0};


	if(readCSV(argv[1], minAcceptableID, maxAcceptableID, data, rejects) < 0)
	{
		cout << ".csv file could not be read" << endl;
	}

	if (computeStatistics(data, stats) < 0)
	{
		std::cout << "Stats could not be computed" << endl;
	}

	if (writeCSV(argv[1], stats) < 0)
	{
		cout << ".stat file could not be written" << endl;
	}

	cout << "Average: " << stats.average << endl;
	cout << "Minimum: " << stats.minimum << endl;
	cout << "Maximum: " << stats.maximum << endl;
	cout << "Population standard deviation: " << stats.popStdDev << endl;
	cout << "Sample standard deviation: " << stats.smplStdDev << endl;
	
	cout << "NUM MODES: " << stats.mode.numModes << endl;
	for (unsigned char i = 0; i < stats.mode.numModes; i++)
	{
		cout << "Mode: " << stats.mode.modes[i] << endl;
	}

	for (unsigned char i = 0; i < 10; i++)
	{
		cout << "Histogram bin [" << (i*10) << ", " << ((((i+1)*10) - 1) + i/9) << "]: " << stats.histogram[i] << endl;
	}
	

	return 0;
}
