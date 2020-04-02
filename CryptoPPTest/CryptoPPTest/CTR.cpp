// g++ -g3 -ggdb -O0 -DDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread
// g++ -g -O2 -DNDEBUG -I/usr/include/cryptopp Driver.cpp -o Driver.exe -lcryptopp -lpthread

#include "osrng.h"
using namespace CryptoPP;
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <numeric>
using namespace std;
using namespace std::chrono;

#include "cryptlib.h"
using CryptoPP::Exception;

#include "hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "aes.h"
using CryptoPP::AES;

#include "ccm.h"
using CryptoPP::CTR_Mode;

#include "assert.h"

class CRT
{
public:

	CRT(string &data_):plain(data_)
	{
	}

	bool encrypt()
	{
		try
		{
			CTR_Mode< AES >::Encryption e;
			e.SetKeyWithIV(key, sizeof(key), iv);

			// The StreamTransformationFilter adds padding
			//  as required. ECB and CBC Mode must be padded
			//  to the block size of the cipher.
			StringSource(plain, true,
				new StreamTransformationFilter(e,
					new StringSink(cipher)
				) // StreamTransformationFilter      
			); // StringSource
			return true;
		}
		catch (const CryptoPP::Exception& e)
		{
			cerr << e.what() << endl;
			return false;
		}
	}

	bool decrypt()
	{
		try
		{
			CTR_Mode< AES >::Decryption d;
			d.SetKeyWithIV(key, sizeof(key), iv);

			// The StreamTransformationFilter removes
			//  padding as required.
			StringSource s(cipher, true,
				new StreamTransformationFilter(d,
					new StringSink(recovered)
				) // StreamTransformationFilter
			); // StringSource

			

		}
		catch (const CryptoPP::Exception& e)
		{
			cerr << e.what() << endl;
			return false;
		}
	}


private:
	string plain;

	string cipher, encoded, recovered;

	byte key[AES::DEFAULT_KEYLENGTH] = "iscas";

	byte iv[AES::BLOCKSIZE] = "tianji";

};

int main(int argc, char* argv[])
{
	//size of the data
	for (int sz = 1; sz <= 512; sz*=2)
	{
		const int times = 100;

		vector<double> encryptVec;
		vector<double> decryptVec;

		//to calc average time
		for (int i = 0; i < times; ++i)
		{
			string data(1024 * sz, 'A');
			CRT test(data);

			
			auto start = high_resolution_clock::now();

			auto ret = test.encrypt();

			auto end = high_resolution_clock::now();

			encryptVec.push_back(duration_cast<microseconds>(end - start).count());

			if (ret)
			{
				start = high_resolution_clock::now();

				test.decrypt();

				end = high_resolution_clock::now();

				decryptVec.push_back(duration_cast<microseconds>(end - start).count());
			}
			else
			{
				break;
			}
		}

		cout << "date length: " << sz << " KB" << endl;
		cout << "encrypt average use time: " << accumulate(encryptVec.begin(), encryptVec.end(), 0.0) / encryptVec.size() << " microsecond " << endl;
		
		cout << "decrypt average use time: " << accumulate(decryptVec.begin(), decryptVec.end(), 0.0) / decryptVec.size() << " microsecond " << endl;
	}
	return 0;
}

