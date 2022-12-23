#pragma once

#include "core.h"

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

	// Bytes per logical sector
	DWORD bytes_ps = 0;

	// Number of free clusters
	DWORD free_cn = 0;

	// Total number of clusters
	DWORD total_cn = 0;

	double free_space = 0;

	LPCWSTR drive_type;

	Volume(std::wstring vol_name) : volume_name(vol_name.c_str())
	{
		std::wcout << L"Обнаружен диск " << volume_name << L"\n";

		switch (GetDriveType(volume_name))
		{
		case DRIVE_UNKNOWN:   drive_type = L"не удалось определить тип диска";  break;
		case DRIVE_REMOVABLE: drive_type = L"гибкий диск";					   break;
		case DRIVE_FIXED:	  drive_type = L"жесткий диск";					   break;
		case DRIVE_REMOTE:	  drive_type = L"сетевой диск";					   break;
		case DRIVE_CDROM:	  drive_type = L"компакт диск";				       break;
		case DRIVE_RAMDISK:   drive_type = L"RAM диск";					       break;

		case DRIVE_NO_ROOT_DIR: throw std::exception();
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