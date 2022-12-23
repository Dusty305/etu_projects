#pragma once

#include "volume.h"
#include "fileattr.h"

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

string get_sf_string(DWORD flags);

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
		{
			wcout << L"\nКоличество секторов на кластер: " << v->sectors_pc
				<< L"\nКоличество байтов на сектор: " << v->bytes_ps
				<< L"\nКоличество свободных кластеров: " << v->free_cn
				<< L"\nКоличество кластеров: " << v->total_cn
				<< L"\nКоличество свободного места: " << v->free_space << L"GB\n";
			cout << get_sf_string(v->file_sf);
		}
	}
	catch (const out_of_range exc)
	{
		cout << "Такого диска не существует.\n";
	}
}

string get_sf_string(DWORD flags)
{
	stringstream ss;
	if (flags & FILE_CASE_SENSITIVE_SEARCH)
		ss << "- Диск поддерживает имена файлов с учетом регистра.\n";
	if (flags & FILE_CASE_PRESERVED_NAMES)
		ss << "- Диск поддерживает сохраненный регистр имен файлов при расположении имени на диске..\n";
	if (flags & FILE_UNICODE_ON_DISK)
		ss << "- Диск поддерживает Юникод в именах файлов, как они отображаются на диске.\n";
	if (flags & FILE_PERSISTENT_ACLS)
		ss << "- Диск сохраняет и применяет списки управления доступом (ACL).\n";
	if (flags & FILE_FILE_COMPRESSION)
		ss << "- Диск поддерживает сжатие на основе файлов.\n";
	if (flags & FILE_VOLUME_QUOTAS)
		ss << "- Диск поддерживает квоты дисков.\n";
	if (flags & FILE_SUPPORTS_SPARSE_FILES)
		ss << "- Диск поддерживает разреженные файлы.\n";
	if (flags & FILE_SUPPORTS_REPARSE_POINTS)
		ss << "- Диск поддерживает точки повторного анализа.\n";
	if (flags & FILE_SUPPORTS_REMOTE_STORAGE)
		ss << "- Диск поддерживает удаленные хранилища.\n";
	if (flags & FILE_RETURNS_CLEANUP_RESULT_INFO)
		ss << "- Диск возвращает информацию о результатах очистки.\n";
	if (flags & FILE_VOLUME_IS_COMPRESSED)
		ss << "- Диск является сжатым томом (например томом DoubleSpace).\n";
	if (flags & FILE_SUPPORTS_OBJECT_IDS)
		ss << "- Диск поддерживает идентификаторы объектов.\n";
	if (flags & FILE_SUPPORTS_ENCRYPTION)
		ss << "- Диск поддерживает зашифрованную файловую систему (EFS).\n";
	if (flags & FILE_NAMED_STREAMS)
		ss << "- Диск поддерживает именованные потоки.\n";
	if (flags & FILE_READ_ONLY_VOLUME)
		ss << "- Диск доступен только для чтения.\n";
	if (flags & FILE_SEQUENTIAL_WRITE_ONCE)
		ss << "- Диск поддерживает одну последовательную запись.\n";
	if (flags & FILE_SUPPORTS_TRANSACTIONS)
		ss << "- Диск поддерживает транзакции.\n";
	if (flags & FILE_SUPPORTS_HARD_LINKS)
		ss << "- Диск поддерживает жесткие связи.\n";
	if (flags & FILE_SUPPORTS_EXTENDED_ATTRIBUTES)
		ss << "- Диск поддерживает расширенные атрибуты.\n";
	if (flags & FILE_SUPPORTS_OPEN_BY_FILE_ID)
		ss << "- Файловая система диска поддерживает открытие с помощью FileID.\n";
	if (flags & FILE_SUPPORTS_USN_JOURNAL)
		ss << "- Диск поддерживает журналы обновления порядкового номера (USN).\n";
	if (flags & FILE_SUPPORTS_BLOCK_REFCOUNTING)
		ss << "- Диск поддерживает совместное использование логических кластеров между файлами на одном томе.\n";
	if (flags & FILE_SUPPORTS_SPARSE_VDL)
		ss << "- Диск поддерживает разреженный VDL.\n";
	if (flags & FILE_DAX_VOLUME)
		ss << "- Диск является томом прямого доступа (DAX).\n";
	return ss.str();
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

void print_change_file_attributes()
{
	string inp;
	cout << "1. Анализ атрибутов файла\n"
		 << "2. Изменение атрибутов файла\n"
		 << "3. Анализ информации о файле\n"
		 << "4. Анализ времени создания, использования и изменение файла\n"
		 << "5. Изменение времени создания, использования и изменение файла\n";
	cin >> inp;
	if (inp[0] < '1' or inp[0] > '5')
		return;
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

		wcout << L"Введите полный путь до файла, куда будут асинхронно копироваться данные: ";
		wcin >> out_file_path;
		wcout << L"Размер копируемых блоков данных: " << bytes_ps
			<< L"\nВведите множитель копируемого блока данных: ";
		wcin >> buffer_multiplier;
		wcout << L"Введите количество перекрывающих операций ввода/вывода: ";
		wcin >> overlapped_multiplier;

		DWORD time = async_copy(in_file_path, out_file_path, bytes_ps * buffer_multiplier, overlapped_multiplier);
		if (time == -1)
			cout << "Копирование завершено с ошибкой." << endl;
		else
			cout << "Копирование завершено успешно." << endl
				 << "Время копирования: " << dec << time << " мс" << endl;
	}
	catch (const out_of_range exc)
	{
		cout << "Файла, откуда копируются данные, не существует.\n";
	}
}