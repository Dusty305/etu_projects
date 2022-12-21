#include <iostream>
#include <Windows.h>


using namespace std;

void print_winapi_error() noexcept
{
	DWORD error = GetLastError();
	string message;
	if (error == 0)
		message = string();
	else
	{
		LPSTR message_buffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPSTR)&message_buffer, 0, NULL);
		message = string(message_buffer, size);
		LocalFree(message_buffer);
	}

	cout << "������: " << message << "(��� ������: 0x" << hex << GetLastError() << ")\n";
}


/*
	��������� ���������� � �������������� �������.

	������� Win32 API � GetSystemInfo.
*/

string get_processor_architecture(WORD arch);

void get_system_info()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	string arch = get_processor_architecture(si.wProcessorArchitecture);

	cout
		<< "����������� ����������: " << arch << endl
		<< "������ ��������: " << si.dwPageSize << endl
		<< "������ ������� ������� (base �����): " << si.lpMinimumApplicationAddress << endl
		<< "������� ������� ������� (bound �����): " << si.lpMaximumApplicationAddress << endl
		<< "������� ����������� ���������� ������ ����������� ������: " << si.dwAllocationGranularity << endl
		<< "���������� ���������� ����������� � ������� ������: " << si.dwNumberOfProcessors << endl\
		<< "������� ����������: " << si.wProcessorLevel << endl;
	
	cout << "������� \"����� ����������\"-\"�������� �� �� � �������\"\n";
	for (int i = 0; i < 32; i++)
		cout << i + 1 << '\t' << (si.dwActiveProcessorMask & (1 << i) ? '+' : '-') << endl;
}

string get_processor_architecture(WORD arch)
{
	switch (arch)
	{
	case PROCESSOR_ARCHITECTURE_AMD64:
		return "x64 (AMD or Intel)";
	case PROCESSOR_ARCHITECTURE_ARM:
		return "ARM";
	case PROCESSOR_ARCHITECTURE_ARM64:
		return "ARM64";
	case PROCESSOR_ARCHITECTURE_IA64:
		return "Intel Itanium-based";
	case PROCESSOR_ARCHITECTURE_INTEL:
		return "x86";
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		return "Unknown architecture";
	}
}

/*
	����������� ������� ����������� ������.

	������� Win32 API � GlobalMemoryStatus.
*/
void get_vm_status()
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	if (!GlobalMemoryStatusEx(&ms))
	{
		print_winapi_error();
		return;
	}

	cout
		<< "������������ " << ms.dwMemoryLoad << "% ���������� ������" << endl
		<< "����� ���������� ������: " << ms.ullTotalPhys << " ����\n"
		<< "�������� ���������� ������: " << ms.ullAvailPhys << " ����\n"
		<< "������ ������ ��� ��������: " << ms.ullTotalPageFile << " ����\n"
		<< "������������ ����� ������, ������� ����� �������������: " << ms.ullAvailPageFile << " ����\n"
		<< "������ ��� ��� ��������: " << ms.ullTotalVirtual << " ����\n" 
		<< "����� �������������������� � ������������������ ������ ������ � ��� ��� ��������: " << ms.ullAvailVirtual << " ����\n";
}

/*
	����������� ��������� ����������� ������� ������
	�� ��������� � ���������� ������.

	������� Win32 API � VirtualQuery.
*/
void get_pm_status()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address;
	cout << "������� ����� ������� ������, ��������� �������� ����� ����������: ";
	wcin >> hex >> address >> dec;
	if (!VirtualQuery(address, &mbi, sizeof(mbi)))
	{
		print_winapi_error();
		return;
	}

	string state;
	switch (mbi.State)
	{
	case MEM_COMMIT:
		state = "���������� ������ �������� ��� �������";
		break;
	case MEM_FREE:
		state = "������������� ��������";
		break;
	case MEM_RESERVE:
		state = "������ ����������������";
		break;
	}

	string type;
	switch (mbi.Type)
	{
	case MEM_IMAGE:
		type = "�������� �������������� � �������������� ������� �����������";
		break;
	case MEM_MAPPED:
		type = "�������� �������������� � �������������� �������";
		break;
	case MEM_PRIVATE:
		type = "�������� �������� ��������";
		break;
	default:
		type = "������";
	}

	cout
		<< "��������� �� ������� ����� ������� �������: " << mbi.BaseAddress << endl
		<< "��������� �� ������� ����� ��������� �������: " << mbi.AllocationBase << endl
		<< "�������� ������ ������ ��� �������������� ��������� �������: 0x" << hex << mbi.AllocationProtect << endl
		<< "��������� �������� ������ ������: 0x" << hex << mbi.Protect << endl
		<< "������ ������� �������: " << dec << mbi.RegionSize << " ������" << endl
		<< "��������� ������� � �������: " << state << endl
		<< "��� ������� � �������: " << type << endl;

	if (0x66 & mbi.Protect and mbi.State == MEM_COMMIT)
	{
		cout << "���������� �������:\n";
		for (int i = 0; i < mbi.RegionSize; i++)
			cout << (unsigned)(*((uint8_t*)mbi.BaseAddress + i)) << " ";
		cout << endl;
	}
}

/*
	�������������� ������� (� �������� ��� ���������� ������, ���� commit == true)
	� �������������� ������ � � ������ ����� ������ ������ �������.

	������� Win32 API � VirtualAlloc.
*/
void alloc_vm(DWORD alloc_type)
{
	LPVOID address = nullptr;
	SIZE_T size = 0;

	string answer;
	cout << "���� �� ������ ������ ��������� ����� ������� ��� ��������������, ������� 1, ����� 0.\n";
	cin >> answer;
	if (answer[0] == '1')
	{
		cout << "������� ��������� �����: ";
		cin >> hex >> address >> dec;
	}

	cout << "������� ������ ������� ��� ��������������: ";
	wcin >> size;

	address = VirtualAlloc(address, size, alloc_type, PAGE_READWRITE);
	if (!address)
		print_winapi_error();
	else
		cout << "�������������� ������� (�� ������ " << hex << address << ") ������ �������.\n";

}

/*
	������ ������ � ������ ������ �� �������� � ����������
	�������.
*/
void write_to_pm()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address = nullptr;

	cout << "������� ����� ������ ������, ���� �� ������� �������� ������: ";
	wcin >> hex >> address >> dec;

	VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	if (mbi.State != MEM_COMMIT)
	{
		cout << "������ �� ���� ��������. ������ ������ ����������.\n";
		return;
	}

	SIZE_T max_size = mbi.RegionSize - ((SIZE_T)address - (SIZE_T)mbi.BaseAddress);
	SIZE_T size = 0;
	cout << "������������ ����� ������, ��������� ��� ������: " << max_size << endl;
	cout << "������� ���������� ������ ��� �����: ";
	wcin >> size;

	unsigned b;
	byte* data = (byte*)address;
	cout << "������� ����� ������ ����� ��� ������: ";
	for (SIZE_T i = 0; i < size; i++)
	{
		cin >> b;
		*(data++) = b;
	}

	cout << "�� ����� ";
	for (SIZE_T i = 0; i < size; i++)
	{
		b = *((byte*)address + i);
		cout << b << " ";
	}
}

/*
	��������� ������ ������� ��� ��������� (� ����������) �������
	������ � �� ��������.

	������� Win32 API � VirtualProtect.
*/
void set_pm_protection()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address = nullptr;
	DWORD new_protection, old_protection;

	cout << "������� ����� ������� ��� �������� ����� �������� ������ �������: ";
	wcin >> hex >> address >> dec;

	VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	//error check?

	cout << "������� ���� ��� ������� ������: ";
	wcin >> hex >> new_protection >> dec;
	if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, new_protection, &old_protection))
		print_winapi_error();
	else
		cout << "���� ������� ������ ��� ������� �������.\n";
}

/*
	������� ���������� ������ � ������������ ������� ���������
	������������ ��������� (� ����������) ������� ������.

	������� Win32 API � VirtualFree.
*/
void free_pm()
{
	MEMORY_BASIC_INFORMATION mbi;
	LPVOID address = nullptr;

	cout << "������� ����� �������, ������� ���� ����������: ";
	wcin >> hex >> address >> dec;

	VirtualQuery(address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	if (!VirtualFree(mbi.BaseAddress, 0, MEM_RELEASE))
		print_winapi_error();
	else
		cout << "������ ���� ������� �����������.\n";
}