#pragma once

#include "volume.h"

/*
	����� ������ ������.

	������� Win32 API � GetLogicalDrives, GetLogicalDriveStrings.
*/
void print_disks(std::unordered_map<WCHAR, Volume*> volumes)
{
	for (auto it = volumes.begin(); it != volumes.end(); it++)
		cout << (char)it->first << " ";
	cout << "\n";
}

/*
	��� ������ �� ��������� ������ ����� ���������� � ����� �
	������ ���������� ������������.

	������� Win32 API � GetDriveType, GetVolumeInformation, GetDiskFreeSpace.
*/
void print_disk_info(std::unordered_map<WCHAR, Volume*> volumes)
{
	string inp;
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
}

/*
	�������� � �������� �������� ���������.

	������� Win32 API � CreateDirectory, RemoveDirectory.
*/
void create_remove_directory()
{
	string inp;
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
}

/*
	�������� ������ � ����� ���������.

	������� Win32 API � CreateFile.
*/
void create_file()
{
	wstring f_path;
	cout << "������� ���� �� �����: ";
	wcin >> f_path;

	auto f_handle = CreateFile(f_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
		print_winapi_error();
	else
		CloseHandle(f_handle);
}

/*
	����������� � ����������� ������ ����� ���������� � ������������ 
	��������� ������� ������ � �������, �������� ����������� �����.

	������� Win32 API � CopyFile, MoveFile, MoveFileEx.
*/

void copy_move_file()
{
	string inp;
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
}

/*
	������ � ��������� ��������� ������.

	������� Win32 API � GetFileAttributes, SetFileAttributes, 
	GetFileInformationByHandle, GetFileTime, SetFileTime.
*/

void print_file_attributes(const wstring& file_path);
void change_file_attributes(const wstring& file_path);
void print_file_info(const wstring& file_path);
void print_file_time_info(const wstring& file_path);
void change_file_time_info(const wstring& file_path);

void print_change_file_attributes()
{
	string inp;
	cout << "1. ������ ��������� �����\n"
		<< "2. ��������� ��������� �����\n"
		<< "3. ������ ���������� � �����\n"
		<< "4. ������ ������� ��������, ������������� � ��������� �����\n"
		<< "5. ��������� ������� ��������, ������������� � ��������� �����\n";
	cin >> inp;
	wstring file_path;
	cout << "������� ���� �� �����: ";
	wcin >> file_path;
	switch (inp[0])
	{
	case '1':
		print_file_attributes(file_path);
		break;
	case '2':
		change_file_attributes(file_path);
		break;
	case '3':
		print_file_info(file_path);
		break;
	case '4':
		print_file_time_info(file_path);
		break;
	case '5':
		change_file_time_info(file_path);
		break;
	}
}

void print_file_attributes(const wstring& file_path)
{
	DWORD attr = GetFileAttributes(file_path.c_str());
	if (attr != INVALID_FILE_ATTRIBUTES)
		cout << "�������� �����: 0x" << hex << attr << "\n";
	else
		print_winapi_error();
}

void change_file_attributes(const wstring& file_path)
{
	DWORD attr;
	cout << "������� ����� �������� �����: 0x";
	wcin >> hex >> attr;
	if (!SetFileAttributes(file_path.c_str(), attr))
		print_winapi_error();
}

void print_file_info(const wstring& file_path)
{
	auto f_handle = CreateFile(file_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
		print_winapi_error();
	else
		CloseHandle(f_handle);
	BY_HANDLE_FILE_INFORMATION inf;
	if (!GetFileInformationByHandle(f_handle, &inf))
	{
		print_winapi_error();
	}
	else
	{
		cout << "\n������ �����: " << inf.nFileSizeHigh << inf.nFileSizeLow;
		cout << "\n������ �����: " << inf.nFileIndexHigh << inf.nFileIndexLow;
		cout << "\n���������� ������ �� ����: " << inf.nNumberOfLinks << "\n";
	}
}

void print_file_time_info(const wstring& file_path)
{
	auto f_handle = CreateFile(file_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
		print_winapi_error();
	else
		CloseHandle(f_handle);

	BY_HANDLE_FILE_INFORMATION inf;
	FILETIME creation_ft, last_access_ft, last_write_ft;
	if (!GetFileTime(f_handle, &creation_ft, &last_access_ft, &last_write_ft))
	{
		print_winapi_error();
		return;
	}

	SYSTEMTIME creation_st, last_access_st, last_write_st;
	string creation_str, last_access_str, last_write_str;
	if (!FileTimeToSystemTime(&inf.ftCreationTime, &creation_st))
	{
		print_winapi_error();
		return;
	}
	else
		creation_str = format("{}:{}:{} {}.{}.{}", creation_st.wHour, creation_st.wMinute, creation_st.wSecond,
			creation_st.wDay, creation_st.wMonth, creation_st.wYear);
	if (!FileTimeToSystemTime(&inf.ftLastWriteTime, &last_write_st))
	{
		print_winapi_error();
		return;
	}
	else
		last_write_str = format("{}:{}:{} {}.{}.{}", last_write_st.wHour, last_write_st.wMinute, last_write_st.wSecond,
			last_write_st.wDay, last_write_st.wMonth, last_write_st.wYear);
	if (!FileTimeToSystemTime(&inf.ftLastAccessTime, &last_access_st))
	{
		print_winapi_error();
		return;
	}
	else
		last_access_str = format("{}:{}:{} {}.{}.{}", last_access_st.wHour, last_access_st.wMinute, last_access_st.wSecond,
			last_access_st.wDay, last_access_st.wMonth, last_access_st.wYear);

	cout << "\n����� ��������: " << creation_str;
	cout << "\n����� ���������� �������������: " << last_access_str;
	cout << "\n����� ���������� ���������: " << last_write_str;
}

void change_file_time_info(const wstring& file_path)
{
	auto f_handle = CreateFile(file_path.c_str(), NULL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		return;
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
			return;
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
			return;
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
			return;
		}
	}
	if (!SetFileTime(f_handle, creation_ft, last_access_ft, last_write_ft))
		print_winapi_error();

	delete creation_ft, last_write_ft, last_access_ft;
}

/*
	
	��������/�������� ������. 
	������� Win32 API � CreateFile.
	(����������� ������������ ����� FILE_FLAG_NO_BUFFERING � FILE_FLAG_OVERLAPPED)

	�������� ����-����� ������� �������� ������� ��������.
	������� Win32 API � ReadFileEx, WriteFileEx.

	�������� ������������ ������ ������� ����������.
	������� Win32 API � SleepEx.

	��������� ����������������� ���������� �������� ����������� �����. 
	������� Win32 API � TimeGetTime.
*/

DWORD async_copy(const wstring& in, const wstring& out, const DWORD btc, const DWORD ovm);

void async_file_copy(unordered_map<WCHAR, Volume*> volumes)
{
	try
	{
		wstring in_file_path, out_file_path;
		DWORD buffer_multiplier, overlapped_multiplier;

		wcout << L"������� ������ ���� �� �����, ������ ����� ���������� ������������ ������: ";
		wcin >> in_file_path;
		const DWORD bytes_ps = volumes[in_file_path[0]]->bytes_ps;
		const DWORD sectors_pc = volumes[in_file_path[0]]->sectors_pc;

		wcout << L"������� ������ ���� �� �����, ���� ����� ���������� ������������ ������: ";
		wcin >> out_file_path;
		wcout << L"������ ���������� ������ ������: " << bytes_ps
			<< L"\n������� ��������� ����������� ����� ������: ";
		wcin >> buffer_multiplier;
		wcout << L"������� ���������� ������������� �������� �����/������: ";
		wcin >> overlapped_multiplier;

		DWORD time = async_copy(in_file_path, out_file_path, sectors_pc * bytes_ps * buffer_multiplier, overlapped_multiplier);

		cout << "����������� ��������� �������." << endl
			<< "����� �����������: " << time << " ��" << endl;
	}
	catch (const out_of_range exc)
	{
		cout << "�����, ������ ���������� ������, �� ����������.\n";
	}
}