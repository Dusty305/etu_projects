#pragma once

#include "volume.h"

/*
	Вывод списка дисков.

	Функции Win32 API – GetLogicalDrives, GetLogicalDriveStrings.
*/
void print_disks(std::unordered_map<WCHAR, Volume*> volumes)
{
	for (auto it = volumes.begin(); it != volumes.end(); it++)
		cout << (char)it->first << " ";
	cout << "\n";
}

/*
	Для одного из выбранных дисков вывод информации о диске и
	размер свободного пространства.

	Функции Win32 API – GetDriveType, GetVolumeInformation, GetDiskFreeSpace.
*/
void print_disk_info(std::unordered_map<WCHAR, Volume*> volumes)
{
	string inp;
	try
	{
		cout << "Введите букву диска, о котором вы хотите получить информацию: ";
		cin >> inp;

		const auto& v = volumes.at(inp[0]);
		wcout << L"Тип диска: " << v->drive_type << "\n\n";

		if (v->name_buffer)
			wcout << L"Имя раздела: " << v->name_buffer
			<< L"\nИмя файловой системы: " << v->sys_name_buffer
			<< L"\nСерийный номер тома: " << v->vs_number
			<< L"\nМаксимальная длина имени файла: " << v->mc_length << "\n";

		if (v->total_cn)
			wcout << L"\nКоличество секторов на кластер: " << v->sectors_pc
			<< L"\nКоличество байтов на сектор: " << v->bytes_ps
			<< L"\nКоличество свободных кластеров: " << v->free_cn
			<< L"\nКоличество кластеров: " << v->total_cn
			<< L"\nКоличество свободного места: " << v->free_space << L"GB\n";
	}
	catch (const out_of_range exc)
	{
		cout << "Такого диска не существует.\n";
	}
}

/*
	Создание и удаление заданных каталогов.

	Функции Win32 API – CreateDirectory, RemoveDirectory.
*/
void create_remove_directory()
{
	string inp;
	cout << "1. Создать каталог\n"
		<< "2. Удалить каталог\n";
	cin >> inp;

	wstring dir_path;
	cout << "Введите путь директории: ";
	wcin >> dir_path;

	if (inp[0] == '1')
		if (!CreateDirectory(dir_path.c_str(), NULL))
			print_winapi_error();
		else if (inp[0] == '2')
			if (!RemoveDirectory(dir_path.c_str()))
				print_winapi_error();
}

/*
	Создание файлов в новых каталогах.

	Функция Win32 API – CreateFile.
*/
void create_file()
{
	wstring f_path;
	cout << "Введите путь до файла: ";
	wcin >> f_path;

	auto f_handle = CreateFile(f_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
		print_winapi_error();
	else
		CloseHandle(f_handle);
}

/*
	Копирование и перемещение файлов между каталогами с возможностью 
	выявления попытки работы с файлами, имеющими совпадающие имена.

	Функции Win32 API – CopyFile, MoveFile, MoveFileEx.
*/

void copy_move_file()
{
	string inp;
	cout << "1. Скопировать файл\n"
		<< "2. Переместить файл\n";
	cin >> inp;
	if (inp[0] == '1')
	{
		bool fail_if_exists;
		wstring f_path, new_file_path;
		cout << "Введите путь файла, которые хотите скопировать: ";
		wcin >> f_path;
		cout << "Введите путь, куда вы хотите скопировать файл: ";
		wcin >> new_file_path;
		cout << "Введите 1, если хотите скопировать файл даже если файл с таким именем уже существует, иначе 0: ";
		cin >> fail_if_exists;

		if (!CopyFile(f_path.c_str(), new_file_path.c_str(), fail_if_exists))
			print_winapi_error();
	}
	if (inp[0] == '2')
	{
		bool fail_if_exists;
		wstring file_path, new_file_path;
		cout << "Введите путь файла, которые хотите переместить: ";
		wcin >> file_path;
		cout << "Введите путь, куда вы хотите переместить файл: ";
		wcin >> new_file_path;
		cout << "Введите 1, если хотите переместить файл даже если файл с таким именем уже существует, иначе 0: ";
		cin >> fail_if_exists;

		if (!MoveFileEx(file_path.c_str(), new_file_path.c_str(), MOVEFILE_COPY_ALLOWED | (fail_if_exists * MOVEFILE_REPLACE_EXISTING)))
			print_winapi_error();
	}
}

/*
	Анализ и изменение атрибутов файлов.

	Функции Win32 API – GetFileAttributes, SetFileAttributes, 
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
	cout << "1. Анализ атрибутов файла\n"
		<< "2. Изменение атрибутов файла\n"
		<< "3. Анализ информации о файле\n"
		<< "4. Анализ времени создания, использования и изменение файла\n"
		<< "5. Изменение времени создания, использования и изменение файла\n";
	cin >> inp;
	wstring file_path;
	cout << "Введите путь до файла: ";
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
		cout << "Аттрибут файла: 0x" << hex << attr << "\n";
	else
		print_winapi_error();
}

void change_file_attributes(const wstring& file_path)
{
	DWORD attr;
	cout << "Введите новый аттрибут файла: 0x";
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
		cout << "\nРазмер файла: " << inf.nFileSizeHigh << inf.nFileSizeLow;
		cout << "\nИндекс файла: " << inf.nFileIndexHigh << inf.nFileIndexLow;
		cout << "\nКоличество ссылок на файл: " << inf.nNumberOfLinks << "\n";
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

	cout << "\nВремя создание: " << creation_str;
	cout << "\nВремя последнего использования: " << last_access_str;
	cout << "\nВремя последнего изменения: " << last_write_str;
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

	cout << "Хотите изменить время создания файла? (1 - да, 0 - нет)\n";
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
	cout << "Хотите изменить время последней записи файла? (1 - да, 0 - нет)\n";
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
	cout << "Хотите изменить время создания файла? (1 - да, 0 - нет)\n";
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
	
	Открытие/создание файлов. 
	Функция Win32 API – CreateFile.
	(обязательно использовать флаги FILE_FLAG_NO_BUFFERING и FILE_FLAG_OVERLAPPED)

	Файловый ввод-вывод блоками кратными размеру кластера.
	Функции Win32 API – ReadFileEx, WriteFileEx.

	Ожидание срабатывания вызова функции завершения.
	функция Win32 API – SleepEx.

	Измерение продолжительности выполнения операции копирования файла. 
	Функция Win32 API – TimeGetTime.
*/

DWORD async_copy(const wstring& in, const wstring& out, const DWORD btc, const DWORD ovm);

void async_file_copy(unordered_map<WCHAR, Volume*> volumes)
{
	try
	{
		wstring in_file_path, out_file_path;
		DWORD buffer_multiplier, overlapped_multiplier;

		wcout << L"Введите полный путь до файла, откуда будут асинхронно копироваться данные: ";
		wcin >> in_file_path;
		const DWORD bytes_ps = volumes[in_file_path[0]]->bytes_ps;
		const DWORD sectors_pc = volumes[in_file_path[0]]->sectors_pc;

		wcout << L"Введите полный путь до файла, куда будут асинхронно копироваться данные: ";
		wcin >> out_file_path;
		wcout << L"Размер копируемых блоков данных: " << bytes_ps
			<< L"\nВведите множитель копируемого блока данных: ";
		wcin >> buffer_multiplier;
		wcout << L"Введите количество перекрывающих операций ввода/вывода: ";
		wcin >> overlapped_multiplier;

		DWORD time = async_copy(in_file_path, out_file_path, sectors_pc * bytes_ps * buffer_multiplier, overlapped_multiplier);

		cout << "Копирование завершено успешно." << endl
			<< "Время копирования: " << time << " мс" << endl;
	}
	catch (const out_of_range exc)
	{
		cout << "Файла, откуда копируются данные, не существует.\n";
	}
}