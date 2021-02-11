#include <iostream>
#include <string>
#include <windows.h>
#include <TlHelp32.h>

class Proc32
{
private:
	const char* procName;
	HANDLE procHandler;
	DWORD procID;

public:
	Proc32(const char* procName)
		: procName(procName)
	{
	}

	static bool listAll() {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		if (snapshot == INVALID_HANDLE_VALUE) {

			std::cout << "\nError getting a snapshot of all processes: " << GetLastError << std::endl;

			return 0;
		}

		Process32First(snapshot, &procEntry);
		do {
			// Get current process name
			WCHAR* name = procEntry.szExeFile;
			std::wstring wsname(name);
			std::string strname(wsname.begin(), wsname.end());

			// Get current process id
			DWORD id = procEntry.th32ProcessID;

			// Print process
			std::cout << id << ": " << strname << std::endl;
		} while (Process32Next(snapshot, &procEntry));

		CloseHandle(snapshot);
		return 1;
	}

	bool attach()
	{
		// Setup variable for snapshot entry
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		// Create snapshot of all processes
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		// Check for errors
		if (snapshot == INVALID_HANDLE_VALUE)
		{
			std::cout << "\nError getting a snapshot of all processes: " << GetLastError << std::endl;

			return 0;
		}

		// Print all processes and attach to desired one
		Process32First(snapshot, &procEntry);
		do
		{
			// Get current process name
			WCHAR* name = procEntry.szExeFile;
			std::wstring wsname(name);
			std::string strname(wsname.begin(), wsname.end());

			// Get current process id
			DWORD id = procEntry.th32ProcessID;

			// Print process
			std::cout << strname << ": " << id << std::endl;

			// If desired one, attach
			if (strname.compare(procName) == 0) {
				// Attach
				procHandler = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procEntry.th32ProcessID);

				procID = id;

				if (procHandler == NULL) {
					std::cout << "\nFailed to attach to specified process." << std::endl;
					CloseHandle(snapshot);
					return 0;
				}
				else {
					std::cout << "\nAttached to process " << strname << " (" << id << ") sucessfully." << std::endl;
					CloseHandle(snapshot);
					return 1;
				}

			}
		} while (Process32Next(snapshot, &procEntry));

		std::cout << "Process " << procName << " not found" << std::endl;
		CloseHandle(snapshot);
		return 0;
	}

	template <typename T>
	void writeMemory(DWORD address, T value) {
		unsigned long bytesWritten;

		WriteProcessMemory(procHandler, (void*)address, &value, sizeof(T), &bytesWritten);

		std::cout << "\nChanged: " << std::hex << "0x" << address << std::dec << "\nTo: " << value << " (" << bytesWritten << " bytes)" << std::endl;
	}

	template <typename T>
	T readMemory(DWORD address) {
		T buffer;
		unsigned long bytesReaded;

		ReadProcessMemory(procHandler, (void*)address, &buffer, sizeof(T), &bytesReaded);

		std::cout << "\n" << std::hex << "0x" << address << std::dec << ": " << buffer << " (" << bytesReaded << " bytes)" << std::endl;
		return buffer;
	}
};

int main(int argc, char *argv[])
{
	// Debug
	std::cout << argc << std::endl;
	for (int i = 0; i < argc; i++) {
		std::cout << argv[i] << std::endl;
	}


	// Help
	if (argc == 2)
	{
		// Help
		if (strcmp("-h", argv[1]) == 0 || strcmp("-help", argv[1]) == 0 || strcmp("--help", argv[1]) == 0 || strcmp("/?", argv[1]) == 0)
		{
			std::cout << "Help" << std::endl;
		}
		else if (strcmp("-l", argv[1]) == 0) {
			Proc32::listAll();
		}
	}
	else if (argc == 3) {
		if (strcmp("-a", argv[1]) == 0) {
			Proc32 proc = Proc32(argv[2]);
			proc.attach();
		}
	}

	//Debug
	//Proc32::listAll();
	Proc32 proc = Proc32("Torchlight2.exe");
	proc.attach();
	proc.readMemory<int>(0x09EE0590);
	proc.writeMemory<int>(0x09EE0590, 99999999);
	std::cin.get();

}
