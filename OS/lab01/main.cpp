#include <Windows.h>
#include <unordered_map>
#include <exception>
#include <iostream>
#include <utility>
#include <stdexcept>
#include <format>

using namespace std;

string get_last_error_as_string()
{
	DWORD error = GetLastError();
	if (error == 0)
		return std::string();

	LPSTR message_buffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPSTR)&message_buffer, 0, NULL);

	std::string message(message_buffer, size);

	LocalFree(message_buffer);

	return message;
}

inline void print_winapi_error() noexcept
{
	cout << "��������� ������: " << get_last_error_as_string() << "(��� ������: 0x" << hex << GetLastError() << ")\n";
}

// ������������ �����������

VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED out_lpOv);
VOID WINAPI async_write(DWORD code, DWORD bytes, LPOVERLAPPED in_lpOv);
void async_end(LPOVERLAPPED in_overlapped, LPOVERLAPPED out_overlapped);

unordered_map<HANDLE, LPOVERLAPPED>* in_to_out_lpOv, * out_to_in_lpOv;
unordered_map<HANDLE, char*>* in_buffers, * out_buffers;

HANDLE* async_events;
LPOVERLAPPED in_overlapped;
LPOVERLAPPED out_overlapped;

HANDLE in_file, out_file;
LARGE_INTEGER file_size;
LARGE_INTEGER in_pos;

DWORD bytes_tc, ov_multiplier;
DWORD start_time, end_time;
bool read_is_over;

DWORD async_copy(const wstring& in, const wstring& out, const DWORD btc, const DWORD ovm)
{
	//INIT VARIABLES
	bytes_tc = btc;
	ov_multiplier = ovm;

	in_overlapped = new OVERLAPPED[ov_multiplier];
	out_overlapped = new OVERLAPPED[ov_multiplier];
	async_events = new HANDLE[ov_multiplier * 2];

	in_to_out_lpOv = new unordered_map<HANDLE, LPOVERLAPPED>;
	out_to_in_lpOv = new unordered_map<HANDLE, LPOVERLAPPED>;

	in_buffers = new unordered_map<HANDLE, char*>;
	out_buffers = new unordered_map<HANDLE, char*>;
	for (int i = 0; i < ov_multiplier; i++)
	{
		(*in_buffers)[in_overlapped[i].hEvent] = new char[bytes_tc];
		(*out_buffers)[out_overlapped[i].hEvent] = (*in_buffers)[&in_overlapped[i]];
	}


	file_size = { 0 };
	in_pos = { 0 };

	//OPEN FILES
	in_file = CreateFile(in.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	out_file = CreateFile(out.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);
	if (out_file == INVALID_HANDLE_VALUE || in_file == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		async_end(in_overlapped, out_overlapped);
		return -1;
	}

	GetFileSizeEx(in_file, &file_size);

	read_is_over = false;

	//INIT EVENTS
	for (int i = 0; i < ov_multiplier; i++)
	{
		async_events[i] = CreateEvent(NULL, true, false, nullptr);
		async_events[i + ov_multiplier] = CreateEvent(NULL, true, false, nullptr);
		if (!async_events[i] or !async_events[i + ov_multiplier])
		{
			print_winapi_error();
			async_end(in_overlapped, out_overlapped);
			return -1;
		}
		in_overlapped[i].hEvent = async_events[i];
		out_overlapped[i].hEvent = async_events[i + ov_multiplier];

		(*in_to_out_lpOv)[in_overlapped[i].hEvent] = &out_overlapped[i];
		(*out_to_in_lpOv)[out_overlapped[i].hEvent] = &in_overlapped[i];

		in_overlapped[i].Offset = in_pos.LowPart;
		in_overlapped[i].OffsetHigh = in_pos.HighPart;

		in_pos.QuadPart += bytes_tc;
	}

	//START
	start_time = GetTickCount64();

	for (int i = 0; i < ov_multiplier; i++)
	{
		ReadFileEx(in_file, (*in_buffers)[in_overlapped[i].hEvent], bytes_tc, &in_overlapped[i], async_write);
	}

	while (!read_is_over)
		SleepEx(INFINITE, true);
	WaitForMultipleObjects(ov_multiplier, async_events, true, 0);

	//END
	end_time = GetTickCount64();
	async_end(in_overlapped, out_overlapped);

	return end_time - start_time;
}

VOID WINAPI async_read(DWORD code, DWORD bytes, LPOVERLAPPED out_lpOv)
{
	if (!read_is_over)
	{
		LPOVERLAPPED in_lpOv = (*out_to_in_lpOv)[out_lpOv];
		char* buffer = (*out_buffers)[out_lpOv];

		LARGE_INTEGER next_pos;
		next_pos.LowPart = out_lpOv->Offset;
		next_pos.HighPart = out_lpOv->OffsetHigh;
		next_pos.QuadPart += (LONGLONG)(ov_multiplier * bytes_tc);

		if (next_pos.QuadPart < file_size.QuadPart)
		{
			in_lpOv->Offset = next_pos.LowPart;
			in_lpOv->OffsetHigh = next_pos.HighPart;

			ReadFileEx(in_file, buffer, bytes_tc, in_lpOv, async_write);
		}
		else
		{
			read_is_over = true;
		}
	}

}

VOID WINAPI async_write(DWORD code, DWORD bytes, LPOVERLAPPED in_lpOv)
{
	LPOVERLAPPED out_lpOv = (*in_to_out_lpOv)[in_lpOv];
	char* buffer = (*out_buffers)[in_lpOv];

	out_lpOv->Offset = in_lpOv->Offset;
	out_lpOv->OffsetHigh = in_lpOv->OffsetHigh;

	WriteFileEx(out_file, buffer, bytes_tc, out_lpOv, async_read);
}

void async_end(LPOVERLAPPED in_overlapped, LPOVERLAPPED out_overlapped)
{
	if (in_file != INVALID_HANDLE_VALUE)
		CloseHandle(in_file);
	if (out_file != INVALID_HANDLE_VALUE)
		CloseHandle(out_file);

	for (int i = 0; i < ov_multiplier * 2; i++)
		if (async_events[i])
			CloseHandle(async_events[i]);

	for (int i = 0; i < ov_multiplier; i++)
		delete[](*in_buffers)[&in_overlapped[i]];

	delete[] in_overlapped;
	delete[] out_overlapped;
	delete[] async_events;

	delete in_to_out_lpOv;
	delete out_to_in_lpOv;
}

struct Volume
{
	LPCWSTR volume_name;

	// Volume name buffer
	LPWSTR name_buffer;

	// The name of the file system 
	// (for example, the FAT file system or the NTFS file system)
	LPWSTR sys_name_buffer;

	// Volume serial number
	DWORD vs_number = 0;

	// The maximum length of a file name component that a specified file system supports
	DWORD mc_length = 0;

	// Flag associated with the specified file system
	DWORD file_sf = 0;

	// Sectors per cluster
	DWORD sectors_pc = 0;

	// Bytes per sector
	DWORD bytes_ps = 0;

	// Number of free clusters
	DWORD free_cn = 0;

	// Total number of clusters
	DWORD total_cn = 0;

	double free_space = 0;

	LPCWSTR drive_type;

	Volume(wstring vol_name) : volume_name(vol_name.c_str())
	{
		wcout << L"��������� ���� " << volume_name << L"\n";
		switch (GetDriveType(volume_name))
		{
		case DRIVE_UNKNOWN:   drive_type = L"�� ������� ���������� ��� �����";  break;
		case DRIVE_REMOVABLE: drive_type = L"������ ����";					   break;
		case DRIVE_FIXED:	  drive_type = L"������� ����";					   break;
		case DRIVE_REMOTE:	  drive_type = L"������� ����";					   break;
		case DRIVE_CDROM:	  drive_type = L"������� ����";				       break;
		case DRIVE_RAMDISK:   drive_type = L"RAM ����";					       break;

		case DRIVE_NO_ROOT_DIR: throw exception();
		}

		if (!GetDiskFreeSpace(volume_name, &sectors_pc, &bytes_ps, &free_cn, &total_cn))
			print_winapi_error();
		else
			free_space = (double)free_cn / pow(2, 30) * sectors_pc * bytes_ps;

		name_buffer = new WCHAR[MAX_PATH + 1];
		sys_name_buffer = new WCHAR[MAX_PATH + 1];
		if (!GetVolumeInformation(volume_name, name_buffer, sizeof(WCHAR) * (MAX_PATH + 1),
			&vs_number, &mc_length, &file_sf, sys_name_buffer, sizeof(WCHAR) * (MAX_PATH + 1)))
		{
			print_winapi_error();
			delete[] name_buffer;
			delete[] sys_name_buffer;
			name_buffer = sys_name_buffer = nullptr;
		}
	}

	~Volume()
	{
		delete[] name_buffer;
		delete[] sys_name_buffer;
	}
};

bool input_time(FILETIME* ft)
{
	SYSTEMTIME st;
	cout << "������� ���, ������, �������, ����, ����� � ��� ����� ������: ";
	cin >> st.wHour >> st.wMinute >> st.wSecond >> st.wDay >> st.wMonth >> st.wYear;
	return SystemTimeToFileTime(&st, ft);
}

int main()
{
	setlocale(LC_ALL, "Russian");

	cout << "���� ���������� � ������...\n";
	unordered_map<WCHAR, unique_ptr<Volume>> volumes;
	wstring vol = L"A:\\";
	for (auto list = GetLogicalDrives(); list; list /= 2, vol[0]++)
		if (list % 2)
			volumes[vol[0]] = unique_ptr<Volume>(new Volume(vol));
	cout << "���� ���������� ��������.\n\n";

	while (true)
	{
		cout << "1. ������ ������.\n"
			<< "2. ���������� � ��������� �����.\n"
			<< "3. ��������/�������� ���������\n"
			<< "4. �������� ����� � ��������\n"
			<< "5. �����������/����������� ������\n"
			<< "6. ������/��������� ��������� ������\n"
			<< "7. ������������ ����������� �����\n"
			<< "8. ����� �� ���������\n";
		string inp;
		cin >> inp;

		switch (inp[0])
		{
			// ����� ������ ������ (������� Win32 API � GetLogicalDrives,
			// GetLogicalDriveStrings);
		case '1':
		{
			for (auto it = volumes.begin(); it != volumes.end(); it++)
				cout << (char)it->first << " ";
			cout << "\n";

			break;
		}
		// ��� ������ �� ��������� ������ ����� ���������� � ����� �
		// ������ ���������� ������������(������� Win32 API �
		//	GetDriveType, GetVolumeInformation, GetDiskFreeSpace);
		case '2':
		{
			try
			{
				cout << "������� ����� �����, � ������� �� ������ �������� ����������: ";
				cin >> inp;

				const auto& v = volumes.at(inp[0]);
				wcout << L"��� �����: " << v->drive_type << "\n\n";

				if (v->name_buffer)
					wcout << L"��� �������: " << v->name_buffer
					<< L"\n��� �������� �������: " << v->sys_name_buffer
					<< L"\n�������� ����� ����: " << v->vs_number
					<< L"\n������������ ����� ����� �����: " << v->mc_length << "\n";

				if (v->total_cn)
					wcout << L"\n���������� �������� �� �������: " << v->sectors_pc
					<< L"\n���������� ������ �� ������: " << v->bytes_ps
					<< L"\n���������� ��������� ���������: " << v->free_cn
					<< L"\n���������� ���������: " << v->total_cn
					<< L"\n���������� ���������� �����: " << v->free_space << L"GB\n";
			}
			catch (const out_of_range exc)
			{
				cout << "������ ����� �� ����������.\n";
			}

			break;
		}
		// �������� � �������� �������� ��������� (������� Win32 API �
		// CreateDirectory, RemoveDirectory);
		case '3':
		{
			cout << "1. ������� �������\n"
				<< "2. ������� �������\n";
			cin >> inp;

			wstring dir_path;
			cout << "������� ���� ����������: ";
			wcin >> dir_path;

			if (inp[0] == '1')
				if (!CreateDirectory(dir_path.c_str(), NULL))
					print_winapi_error();

				else if (inp[0] == '2')
					if (!RemoveDirectory(dir_path.c_str()))
						print_winapi_error();

			break;
		}
		// �������� ������ � ����� ��������� (������� Win32 API � CreateFile)
		case '4':
		{
			wstring f_path;
			cout << "������� ���� �� �����: ";
			wcin >> f_path;

			auto f_handle = CreateFile(f_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (f_handle == INVALID_HANDLE_VALUE)
				print_winapi_error();
			else
				CloseHandle(f_handle);

			break;
		}
		// ����������� � ����������� ������ ����� ���������� �
		// ������������ ��������� ������� ������ � �������, ��������
		// ����������� �����(������� Win32 API � CopyFile, MoveFile, MoveFileEx);
		case '5':
		{
			cout << "1. ����������� ����\n"
				<< "2. ����������� ����\n";
			cin >> inp;
			if (inp[0] == '1')
			{
				bool fail_if_exists;
				wstring f_path, new_file_path;
				cout << "������� ���� �����, ������� ������ �����������: ";
				wcin >> f_path;
				cout << "������� ����, ���� �� ������ ����������� ����: ";
				wcin >> new_file_path;
				cout << "������� 1, ���� ������ ����������� ���� ���� ���� ���� � ����� ������ ��� ����������, ����� 0: ";
				cin >> fail_if_exists;

				if (!CopyFile(f_path.c_str(), new_file_path.c_str(), fail_if_exists))
					print_winapi_error();
			}
			if (inp[0] == '2')
			{
				bool fail_if_exists;
				wstring file_path, new_file_path;
				cout << "������� ���� �����, ������� ������ �����������: ";
				wcin >> file_path;
				cout << "������� ����, ���� �� ������ ����������� ����: ";
				wcin >> new_file_path;
				cout << "������� 1, ���� ������ ����������� ���� ���� ���� ���� � ����� ������ ��� ����������, ����� 0: ";
				cin >> fail_if_exists;

				if (!MoveFileEx(file_path.c_str(), new_file_path.c_str(), MOVEFILE_COPY_ALLOWED | (fail_if_exists * MOVEFILE_REPLACE_EXISTING)))
					print_winapi_error();
			}
			break;
		}
		// ������ � ��������� ��������� ������ (������� Win32 API �
		// GetFileAttributes, SetFileAttributes, GetFileInformationByHandle,
		// GetFileTime, SetFileTime).
		case '6':
		{
			cout << "1. ������ ��������� �����\n"
				<< "2. ��������� ��������� �����\n"
				<< "3. ������ ���������� � �����\n"
				<< "4. ������ ������� ��������, ������������� � ��������� �����\n"
				<< "5. ��������� ������� ��������, ������������� � ��������� �����\n";
			cin >> inp;
			wstring f_path;
			cout << "������� ���� �� �����: ";
			wcin >> f_path;
			switch (inp[0])
			{
			case '1':
			{
				DWORD attr = GetFileAttributes(f_path.c_str());
				if (attr != INVALID_FILE_ATTRIBUTES)
					cout << "�������� �����: 0x" << hex << attr << "\n";
				else
					print_winapi_error();

				break;
			}
			case '2':
			{
				DWORD attr;
				cout << "������� ����� �������� �����: 0x";
				wcin >> hex >> attr;
				if (!SetFileAttributes(f_path.c_str(), attr))
					print_winapi_error();
			}
			case '3':
			{
				auto f_handle = CreateFile(f_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (f_handle == INVALID_HANDLE_VALUE)
					print_winapi_error();
				else
					CloseHandle(f_handle);
				BY_HANDLE_FILE_INFORMATION inf;
				if (!GetFileInformationByHandle(f_handle, &inf))
				{
					print_winapi_error();
					break;
				}

				cout << "\n������ �����: " << inf.nFileSizeHigh << inf.nFileSizeLow;
				cout << "\n������ �����: " << inf.nFileIndexHigh << inf.nFileIndexLow;
				cout << "\n���������� ������ �� ����: " << inf.nNumberOfLinks << "\n";

				break;
			}
			case '4':
			{
				auto f_handle = CreateFile(f_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (f_handle == INVALID_HANDLE_VALUE)
					print_winapi_error();
				else
					CloseHandle(f_handle);

				BY_HANDLE_FILE_INFORMATION inf;
				FILETIME creation_ft, last_access_ft, last_write_ft;
				if (!GetFileTime(f_handle, &creation_ft, &last_access_ft, &last_write_ft))
				{
					print_winapi_error();
					break;
				}

				SYSTEMTIME creation_st, last_access_st, last_write_st;
				string creation_str, last_access_str, last_write_str;
				if (!FileTimeToSystemTime(&inf.ftCreationTime, &creation_st))
				{
					print_winapi_error();
					break;
				}
				else
					creation_str = format("{}:{}:{} {}.{}.{}", creation_st.wHour, creation_st.wMinute, creation_st.wSecond,
						creation_st.wDay, creation_st.wMonth, creation_st.wYear);
				if (!FileTimeToSystemTime(&inf.ftLastWriteTime, &last_write_st))
				{
					print_winapi_error();
					break;
				}
				else
					last_write_str = format("{}:{}:{} {}.{}.{}", last_write_st.wHour, last_write_st.wMinute, last_write_st.wSecond,
						last_write_st.wDay, last_write_st.wMonth, last_write_st.wYear);
				if (!FileTimeToSystemTime(&inf.ftLastAccessTime, &last_access_st))
				{
					print_winapi_error();
					break;
				}
				else
					last_access_str = format("{}:{}:{} {}.{}.{}", last_access_st.wHour, last_access_st.wMinute, last_access_st.wSecond,
						last_access_st.wDay, last_access_st.wMonth, last_access_st.wYear);

				cout << "\n����� ��������: " << creation_str;
				cout << "\n����� ���������� �������������: " << last_access_str;
				cout << "\n����� ���������� ���������: " << last_write_str;

				break;
			}
			case '5':
			{
				auto f_handle = CreateFile(f_path.c_str(), NULL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (f_handle == INVALID_HANDLE_VALUE)
				{
					print_winapi_error();
					break;
				}
				else
					CloseHandle(f_handle);

				FILETIME* creation_ft = nullptr,
					* last_write_ft = nullptr,
					* last_access_ft = nullptr;
				bool yes;

				cout << "������ �������� ����� �������� �����? (1 - ��, 0 - ���)\n";
				cin >> yes;
				if (yes)
				{
					creation_ft = new FILETIME;
					if (!input_time(creation_ft))
					{
						print_winapi_error();
						delete creation_ft;
						break;
					}
				}
				cout << "������ �������� ����� ��������� ������ �����? (1 - ��, 0 - ���)\n";
				cin >> yes;
				if (yes)
				{
					last_write_ft = new FILETIME;
					if (!input_time(last_write_ft))
					{
						print_winapi_error();
						delete creation_ft, last_write_ft;
						break;
					}
				}
				cout << "������ �������� ����� �������� �����? (1 - ��, 0 - ���)\n";
				cin >> yes;
				if (yes)
				{
					last_access_ft = new FILETIME;
					if (!input_time(last_access_ft))
					{
						print_winapi_error();
						delete creation_ft, last_write_ft, last_access_ft;
						break;
					}
				}
				if (!SetFileTime(f_handle, creation_ft, last_access_ft, last_write_ft))
					print_winapi_error();

				delete creation_ft, last_write_ft, last_access_ft;
				break;
			}
			}

			break;
		}
		case '7':
		{
			try
			{
				wstring in_file_path, out_file_path;
				DWORD buffer_multiplier, overlapped_multiplier;

				wcout << L"������� ������ ���� �� �����, ������ ����� ���������� ������������ ������: ";
				wcin >> in_file_path;
				const auto& bytes_ps = volumes[in_file_path[0]]->bytes_ps;

				wcout << L"������� ������ ���� �� �����, ���� ����� ���������� ������������ ������: ";
				wcin >> out_file_path;
				wcout << L"������ ���������� ������ ������: " << bytes_ps
					<< L"\n������� ��������� ����������� ����� ������: ";
				wcin >> buffer_multiplier;
				wcout << L"������� ���������� ������������� �������� �����/������: ";
				wcin >> overlapped_multiplier;

				async_copy(in_file_path, out_file_path, bytes_ps * buffer_multiplier, overlapped_multiplier);
			}
			catch (const out_of_range exc)
			{
				cout << "�����, ������ ���������� ������, �� ����������.\n";
			}
		}
		case '8':
		{
			return 0;
		}
		}
		system("pause");
	}
}