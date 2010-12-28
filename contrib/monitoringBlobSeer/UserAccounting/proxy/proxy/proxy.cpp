#include "repClient.h"
using namespace std;
int main()
{
	repClient rep_client;
	std::string result = rep_client.sendRequest();
	rep_client.sendResult(result);
} 
