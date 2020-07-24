#include <iostream>
#include <windows.h>
#include <TlHelp32.h>

class Proc32 {
private:
	const char* procName;
	PROCESSENTRY32 procEntry;

public:
	Proc32(const char* procName)
		: procName(procName)
	{
	}

	bool attach() {
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snapshot == INVALID_HANDLE_VALUE) {
			std::cout << "Error getting a snapshot of all processes: " << GetLastError << std::endl;

			return 0;
		}

		Process32First(snapshot, &procEntry);
		do {
			WCHAR* name = procEntry.szExeFile;
			std::wstring ws(name);
			std::string string(ws.begin(), ws.end());
			std::cout << string << ": " << procEntry.th32ProcessID << std::endl;
		} while (Process32Next(snapshot, &procEntry));
	}

};

int main(int argc, char* argv[]) {

	// Debug
	for (int i = 0; i < argc; i++) {
		std::cout << i << ": " << argv[i] << std::endl;
	}

	// Help
	if (argc == 2) 
	{
		// Help
		if (strcmp("-h", argv[1]) == 0
			|| strcmp("-help", argv[1]) == 0
			|| strcmp("--help", argv[1]) == 0
			|| strcmp("/?", argv[1]) == 0) 
		{
			std::cout << "Help" << std::endl;
		}
		else {
			Proc32 proc = Proc32(argv[1]);
		}
	}

	Proc32 proc = Proc32("test.exe");
	proc.attach();

	std::cin.get();

}
