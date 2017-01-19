#include <Windows.h>
#include <stdio.h>
#include <string>
#include <cstring>

#define QWORD unsigned __int64

using namespace std;

const int MAX_COMMAND_LENGTH = 30;
const char* PROMPT = "ds3cmd> ";
const QWORD SOULS_ADDR = 0x7FF5A08814A4;
const QWORD EMBER_ADDR = 0x7FF5A0882188;

void clear()
{
	for (int i = 0; i < 50; i++)
		printf("\n");
}

void getCommand(char* cmd)
{
	char c = getchar();
	int i = 0;
	while (c != '\n')
	{
		*(cmd + sizeof(char) * i) = c;
		c = getchar();
		i++;
	}
	*(cmd + sizeof(char) * i) = '\0';
}

int main(int argc, char** args)
{
	//Check argc
	if (argc < 2)
	{
		printf("You must enter PID as argument.\n");
		return -1;
	}

	//Clear console
	clear();

	char command[MAX_COMMAND_LENGTH];
	char* currentToken;
	DWORD error;

	//Attach to Dark Souls III
	HANDLE darkSouls = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ, FALSE, stoi(args[1]));
	if (darkSouls == NULL)
	{
		error = GetLastError();
		printf("Unable to attach to process.\nError code: %d\n", GetLastError());
		return -1;
	}

	printf(PROMPT);
	while (true)
	{
		//Get command
		getCommand(command);

		//Write memory
		currentToken = strtok(command, " ");

		//Check for no input
		if (strcmp(command, "") == 0)
		{
			printf("You actually have to type something for this program to work.\n");
		}
		else if (strcmp(currentToken, "setsouls") == 0)
		{
			//Update token to second arg
			currentToken = strtok(NULL, " ");

			int numberOfSouls = stoi(currentToken);
			if (WriteProcessMemory(darkSouls, (void*)SOULS_ADDR, &numberOfSouls, sizeof(int), NULL) == 0)
				printf("Error: %d\n", GetLastError());
		}
		else if (strcmp(currentToken, "getsouls") == 0)
		{
			int numberOfSouls;
			if (ReadProcessMemory(darkSouls, (void*)SOULS_ADDR, &numberOfSouls, sizeof(int), NULL) == 0)
				printf("Error: %d\n", GetLastError());
			
			printf("Souls: %d\n", numberOfSouls);
		}
		else if (strcmp(currentToken, "exit") == 0)
		{
			return 0;
		}
		else if (strcmp(currentToken, "clear") == 0)
		{
			clear();
		}
		else
		{
			printf("Unrecognized command: \"%s\"\n", currentToken);
		}

		//Reset display
		printf("\n");
		printf(PROMPT);
	}

	return 0;
}