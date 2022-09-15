#include <Windows.h>
#include <unordered_map>
#include <exception>
#include <iostream>
#include <utility>
#include <stdexcept>

using namespace std;

struct Volume
{
	char volume_char;
	LPCWSTR volume_name;

	// Volume name buffer
	LPWSTR name_buffer;

	// The name of the file system 
	// (for example, the FAT file system or the NTFS file system)
	LPWSTR sys_name_buffer;  

	// Volume serial number
	DWORD vs_number;
	
	// The maximum length of a file name component that a specified file system supports
	DWORD mc_length;

	// Flag associated with the specified file system
	DWORD file_sf;

	// Sectors per cluster
	DWORD sectors_pc;

	// Bytes per sector
	DWORD bytes_ps;

	// Number of free clusters
	DWORD free_cn;

	// Total number of clusters
	DWORD total_cn;

	LPCWSTR drive_type;

	Volume()
	{
		
	}

	Volume(wstring vol_name) : volume_name(vol_name.c_str())
	{
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
		{
			throw exception();
		}
		if (drive_type == L"������ ����")
		{
			name_buffer = nullptr;
			sys_name_buffer = nullptr;
		}
		else
		{
			name_buffer = new WCHAR[MAX_PATH + 1];
			sys_name_buffer = new WCHAR[MAX_PATH + 1];
			if (!GetVolumeInformation(volume_name, name_buffer, sizeof(name_buffer),
				&vs_number, &mc_length, &file_sf, sys_name_buffer, sizeof(sys_name_buffer)))
			{
				throw exception();
			}
		}
	}

	~Volume()
	{
		delete[] name_buffer;
		delete[] sys_name_buffer;
	}
};

int main()
{
	setlocale(LC_ALL, "Russian");
	
	unordered_map<WCHAR, unique_ptr<Volume>> volumes;
	wstring vol = L"A:\\";
	for (auto list = GetLogicalDrives(); list; list /= 2, vol[0]++)
		if (list % 2)
			volumes[vol[0]] = unique_ptr<Volume>(new Volume(vol));

	while (true)
	{
		cout << "1. ������ ������.\n"
			 << "2. ���������� � ��������� �����.\n"
			 << "3. ��������/�������� ���������\n"
			 << "4. �������� ����� � ��������\n"
			 << "5. �����������/����������� ������\n"
			 << "6. ������/��������� ��������� ������\n"
			 << "7. ����� �� ���������\n";
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
				cout << "������� ����� �����, � ������� �� ������ �������� ����������: ";
				cin >> inp;
				try 
				{
					const auto& v = volumes.at(inp[0]);
					if (v->name_buffer)
						wcout << L"��� �������: " << v->name_buffer
							  << L"\n��� �������� �������: " << v->sys_name_buffer
							  << L"\n�������� ����� ����: " << v->vs_number
							  << L"\n������������ ����� ����� �����: " << v->mc_length;

					wcout << L"\n��� �����: "						 << v->drive_type
						  << L"\n\n���������� �������� �� �������: " << v->sectors_pc
						  << L"\n���������� ������ �� ������: "		 << v->bytes_ps
						  << L"\n���������� ��������� ���������: "	 << v->free_cn
						  << L"\n���������� ���������: "			 << v->total_cn
						  << L"\n���������� ���������� �����: "		 << (float)v->free_cn / 1024 / 1024 * v->sectors_pc * v->bytes_ps << L"MB\n";
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
				if (inp[0] == '1')
				{
					cout << "������� ���� ��� ����������� ����������: ";
					wstring dir_path;
					wcin >> dir_path;
					if(!CreateDirectory(dir_path.c_str(), NULL))
						cout << "��������� ������ �� ����� �������� ����������\n";
				}
				else if (inp[0] == '2')
				{
					wstring dir_path;
					cout << "������� ���� ��� ��������� ����������: ";
					wcin >> dir_path;
					if(!RemoveDirectory(dir_path.c_str()))
						cout << "��������� ������ �� ����� �������� ����������\n";
				}
				break;
			}
			// �������� ������ � ����� ��������� (������� Win32 API � CreateFile)
			case '4': 
			{
				wstring file_path;
				cout << "������� ���� �� �����: ";
				wcin >> file_path;
				if(CreateFile(file_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) == INVALID_HANDLE_VALUE)
					cout << "��������� ������ �� ����� �������� �����\n";
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
					wstring file_path, new_file_path;
					cout << "������� ���� �����, ������� ������ �����������: ";
					wcin >> file_path;
					cout << "������� ����, ���� �� ������ ����������� ����: ";
					wcin >> new_file_path;
					if(!CopyFile(file_path.c_str(), new_file_path.c_str(), false))
						cout << "��������� ������\n";
				}
				if (inp[0] == '2')
				{
					wstring file_path, new_file_path;
					cout << "������� ���� �����, ������� ������ �����������: ";
					wcin >> file_path;
					cout << "������� ����, ���� �� ������ ����������� ����: ";
					wcin >> new_file_path;
					if(!MoveFile(file_path.c_str(), new_file_path.c_str()))
						cout << "��������� ������\n";
				}
				break;
			}
			// ������ � ��������� ��������� ������ (������� Win32 API �
			// GetFileAttributes, SetFileAttributes, GetFileInformationByHandle,
			// GetFileTime, SetFileTime).
			case '6': 
			{
				cout << "1. ������ ��������� �����\n"
					 << "2. ��������� ��������� �����\n";
				cin >> inp;
				wstring f_path;
				cout << "������� ���� �� �����: ";
				wcin >> f_path;
				if (inp[0] == '1')
				{
					DWORD attr = GetFileAttributes(f_path.c_str());
					if (attr != INVALID_FILE_ATTRIBUTES)
						cout << "�������� �����: 0x" << hex << attr << "\n";
					else
						cout << "��������� ������\n";
				}
				else if (inp[0] == '2')
				{
					DWORD attr;
					cout << "������� ����� �������� �����: 0x";
					wcin >> hex >> attr;
					if(!SetFileAttributes(f_path.c_str(), attr))
						cout << "��������� ������\n";
				}
				break;
			}
			case '7': 
			{
				return 0;
			}
		}
	}
}