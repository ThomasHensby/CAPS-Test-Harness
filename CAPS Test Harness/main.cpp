#include <iostream>
#include "TCPClient.h"
#include <thread>
#include <vector>
#include<chrono>
#include<conio.h>
#include<time.h>
#include<string>
#include<shared_mutex>


#define DEFAULT_PORT 12345
#define IP_ADDRESS "127.0.0.1"
#define READER_THREAD 1
#define POSTER_THREAD 1
#define TIME_PERIOD 10



using namespace std;

shared_mutex shared;

void readThreadFunction(int threadNum, vector<string> postId);
void postThreadFunction(int threadNum, vector<string> postId);


int main()
{
	
	vector<thread> readerThreads;
	vector<thread> posterThreads;



	TCPClient client(IP_ADDRESS, DEFAULT_PORT);
	

	vector<string> postId;

	for (int i = 0; i < POSTER_THREAD; i++)
		posterThreads.emplace_back(postThreadFunction, i, postId);

	for (int i = 0; i < READER_THREAD; i++)
		readerThreads.emplace_back(readThreadFunction, i, postId);

	for (auto& th : posterThreads)
		th.join();

	for (auto& th : readerThreads)
		th.join();



	return 0;
}


void postThreadFunction(int threadNum, vector<string> postId)
{
	TCPClient client(IP_ADDRESS, DEFAULT_PORT);
	int requests_managed = 0;
	client.OpenConnection();

	chrono::steady_clock::time_point start = chrono::steady_clock::now();
	do {
		string random1;
		string random2;
		string random3;
		string random4;
		string random5;
		string request;

		//Creating random letters for post
		unsigned seed = time(0);
		srand(seed);
		random1 = 'a' + rand() % 26;
		random2 = 'a' + rand() % 26;
		random3 = 'a' + rand() % 26;
		random4 = 'a' + rand() % 26;
		random5 = 'a' + rand() % 26;
		request =  random1 + random2 + random3 + random4 + random5;
		client.send("POST@" + request + "#message");
		requests_managed++;

		shared.lock();
		postId.emplace_back(request);
		shared.unlock();

	/*	if (chrono::steady_clock::now() - start == std::chrono::seconds(0))
			cout << "total post requests: " + to_string(requests_managed) + " ";
		if (chrono::steady_clock::now() - start == std::chrono::seconds(1))
			cout << "total post requests: " + to_string(requests_managed) + " ";*/


		if (chrono::steady_clock::now() - start > std::chrono::seconds(TIME_PERIOD))
		{
			break;
		}
		
	} while (requests_managed != 0);
		
	cout << to_string(threadNum) + " Total POST Requests: " + to_string(requests_managed) + "\n";
	cout << to_string(threadNum) + " POST Requests per second: " + to_string(requests_managed / TIME_PERIOD) + "\n";
	client.CloseConnection();

}

void readThreadFunction(int threadNum, vector<string> postId)
{
	TCPClient client(IP_ADDRESS, DEFAULT_PORT);
	int requests_managed = 0;
	client.OpenConnection();

	chrono::steady_clock::time_point start = chrono::steady_clock::now();
	do {
		string random;
		string request;

		//getting random letters
		unsigned seed = time(0);
		srand(seed);
		random = 'a' + rand() % 26;
		int num = rand() % 10;
		request = "READ@" + random + "#" + to_string(num);
		client.send(request);
		requests_managed++;

		if (chrono::steady_clock::now() - start > std::chrono::seconds(TIME_PERIOD))
		{
			break;
		}

	} while (requests_managed != 0);

	cout << to_string(threadNum) +  " Total READ Requests: " + to_string(requests_managed) + "\n";
	cout << to_string(threadNum) + " READ Requests per second: " + to_string(requests_managed / TIME_PERIOD) + "\n";


	client.CloseConnection();
}



