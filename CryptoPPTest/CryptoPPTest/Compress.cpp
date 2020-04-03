#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
using namespace std;
using namespace std::chrono;
#include "cryptlib.h"
#include "filters.h"
#include "gzip.h"
using namespace CryptoPP;

class Compress
{

public:
	Compress(string &data) :data_(data)
	{
	}

	void zip()
	{
		auto start = high_resolution_clock::now();
		StringSource ss(data_, true,
			new Gzip(
				new StringSink(compressed_)
			));
		auto end = high_resolution_clock::now();
		cout << "zip use time: " << duration_cast<microseconds>(end - start).count() << " microseconds" << endl;
	}

	void unZip()
	{
		auto start = high_resolution_clock::now();

		StringSource ssr(compressed_, true,
			new Gunzip(
				new StringSink(recoverd_)
			));
		auto end = high_resolution_clock::now();
		cout << "unzip use time: " << duration_cast<microseconds>(end - start).count() << " microseconds" << endl;
	}

	bool check()
	{
		return !data_.compare(recoverd_) ? true : false;
	}

	double ratio()
	{
		int temp = int(data_.length() - compressed_.length());
		double d = (double)temp / data_.length();
		return d * 100;
	}

private:
	string data_, compressed_, recoverd_;
};



int main()
{



	for (auto& p : experimental::filesystem::directory_iterator("../files"))
	{
		ifstream ifs(p.path());
		std::string data((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));

		cout << "file name: " << p.path() << endl;
		cout << "file size: " << data.size() << " bytes" << endl;

		Compress task(data);

		task.zip();
		task.unZip();

		if (task.check())
		{
			cout << "ratio: " << task.ratio() << "%" << endl;
		}
		else
		{
			cout << "error occurd" << endl;
		}

	}

	return 0;
}