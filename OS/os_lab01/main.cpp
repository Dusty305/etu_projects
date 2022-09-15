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
			case DRIVE_UNKNOWN:   drive_type = L"не удалось определить тип диска";  break;
			case DRIVE_REMOVABLE: drive_type = L"гибкий диск";					   break;
			case DRIVE_FIXED:	  drive_type = L"жесткий диск";					   break;
			case DRIVE_REMOTE:	  drive_type = L"сетевой диск";					   break;
			case DRIVE_CDROM:	  drive_type = L"компакт диск";				       break;
			case DRIVE_RAMDISK:   drive_type = L"RAM диск";					       break;

			case DRIVE_NO_ROOT_DIR: throw exception();
		}
		
		if (!GetDiskFreeSpace(volume_name, &sectors_pc, &bytes_ps, &free_cn, &total_cn))
		{
			throw exception();
		}
		if (drive_type == L"гибкий диск")
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
		cout << "1. Список дисков.\n"
			 << "2. Информация о выбранном диске.\n"
			 << "3. Создание/удаление каталогов\n"
			 << "4. Создание файла в каталоге\n"
			 << "5. Копирование/перемещение файлов\n"
			 << "6. Анализ/изменение атрибутов файлов\n"
			 << "7. Выйти из программы\n";
		string inp;
		cin >> inp;
		switch (inp[0])
		{
			// вывод списка дисков (функции Win32 API – GetLogicalDrives,
			// GetLogicalDriveStrings);
			case '1': 
			{

				for (auto it = volumes.begin(); it != volumes.end(); it++)
					cout << (char)it->first << " ";
				cout << "\n";
				break;
			}
			// для одного из выбранных дисков вывод информации о диске и
			// размер свободного пространства(функции Win32 API –
			//	GetDriveType, GetVolumeInformation, GetDiskFreeSpace);
			case '2': 
			{
				cout << "Введите букву диска, о котором вы хотите получить информацию: ";
				cin >> inp;
				try 
				{
					const auto& v = volumes.at(inp[0]);
					if (v->name_buffer)
						wcout << L"Имя раздела: " << v->name_buffer
							  << L"\nИмя файловой системы: " << v->sys_name_buffer
							  << L"\nСерийный номер тома: " << v->vs_number
							  << L"\nМаксимальная длина имени файла: " << v->mc_length;

					wcout << L"\nТип диска: "						 << v->drive_type
						  << L"\n\nКоличество секторов на кластер: " << v->sectors_pc
						  << L"\nКоличество байтов на сектор: "		 << v->bytes_ps
						  << L"\nКоличество свободных кластеров: "	 << v->free_cn
						  << L"\nКоличество кластеров: "			 << v->total_cn
						  << L"\nКоличество свободного места: "		 << (float)v->free_cn / 1024 / 1024 * v->sectors_pc * v->bytes_ps << L"MB\n";
				}
				catch (const out_of_range exc)
				{
					cout << "Такого диска не существует.\n";
				}
				
				break;
			}
			// создание и удаление заданных каталогов (функции Win32 API –
			// CreateDirectory, RemoveDirectory);
			case '3': 
			{
				cout << "1. Создать каталог\n"
					 << "2. Удалить каталог\n";
				cin >> inp;
				if (inp[0] == '1')
				{
					cout << "Введите путь для создаваемой директории: ";
					wstring dir_path;
					wcin >> dir_path;
					if(!CreateDirectory(dir_path.c_str(), NULL))
						cout << "Произошла ошибка во время создания директории\n";
				}
				else if (inp[0] == '2')
				{
					wstring dir_path;
					cout << "Введите путь для удаляемой директории: ";
					wcin >> dir_path;
					if(!RemoveDirectory(dir_path.c_str()))
						cout << "Произошла ошибка во время удаление директории\n";
				}
				break;
			}
			// создание файлов в новых каталогах (функция Win32 API – CreateFile)
			case '4': 
			{
				wstring file_path;
				cout << "Введите путь до файла: ";
				wcin >> file_path;
				if(CreateFile(file_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) == INVALID_HANDLE_VALUE)
					cout << "Произошла ошибка во время создания файла\n";
				break;
			}
			// копирование и перемещение файлов между каталогами с
			// возможностью выявления попытки работы с файлами, имеющими
			// совпадающие имена(функции Win32 API – CopyFile, MoveFile, MoveFileEx);
			case '5': 
			{
				cout << "1. Скопировать файл\n"
					 << "2. Переместить файл\n";
				cin >> inp;
				if (inp[0] == '1')
				{
					wstring file_path, new_file_path;
					cout << "Введите путь файла, которые хотите скопировать: ";
					wcin >> file_path;
					cout << "Введите путь, куда вы хотите скопировать файл: ";
					wcin >> new_file_path;
					if(!CopyFile(file_path.c_str(), new_file_path.c_str(), false))
						cout << "Произошла ошибка\n";
				}
				if (inp[0] == '2')
				{
					wstring file_path, new_file_path;
					cout << "Введите путь файла, которые хотите переместить: ";
					wcin >> file_path;
					cout << "Введите путь, куда вы хотите переместить файл: ";
					wcin >> new_file_path;
					if(!MoveFile(file_path.c_str(), new_file_path.c_str()))
						cout << "Произошла ошибка\n";
				}
				break;
			}
			// анализ и изменение атрибутов файлов (функции Win32 API –
			// GetFileAttributes, SetFileAttributes, GetFileInformationByHandle,
			// GetFileTime, SetFileTime).
			case '6': 
			{
				cout << "1. Анализ атрибутов файла\n"
					 << "2. Изменение атрибутов файла\n";
				cin >> inp;
				wstring f_path;
				cout << "Введите путь до файла: ";
				wcin >> f_path;
				if (inp[0] == '1')
				{
					DWORD attr = GetFileAttributes(f_path.c_str());
					if (attr != INVALID_FILE_ATTRIBUTES)
						cout << "Аттрибут файла: 0x" << hex << attr << "\n";
					else
						cout << "Произошла ошибка\n";
				}
				else if (inp[0] == '2')
				{
					DWORD attr;
					cout << "Введите новый аттрибут файла: 0x";
					wcin >> hex >> attr;
					if(!SetFileAttributes(f_path.c_str(), attr))
						cout << "Произошла ошибка\n";
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