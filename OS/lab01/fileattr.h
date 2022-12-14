#include "core.h"

string get_string_attributes(DWORD attr)
{
	stringstream ss;

	if (attr & FILE_ATTRIBUTE_ARCHIVE)
		ss << "- Файл является архивом\n";
	if (attr & FILE_ATTRIBUTE_COMPRESSED)
		ss << "- Файл является сжатым\n";
	if (attr & FILE_ATTRIBUTE_DIRECTORY)
		ss << "- Файл является каталогом\n";
	if (attr & FILE_ATTRIBUTE_ENCRYPTED)
		ss << "- Файл зашифрован\n";
	if (attr & FILE_ATTRIBUTE_HIDDEN)
		ss << "- Файл скрыт\n";
	if (attr & FILE_ATTRIBUTE_INTEGRITY_STREAM)
		ss << "- Файл настраивается с целостностью\n";
	if (attr & FILE_ATTRIBUTE_NORMAL)
		ss << "- Файл не имеет особых аттрибутов\n";
	if (attr & FILE_ATTRIBUTE_NO_SCRUB_DATA)
		ss << "- Файл не будет считываться с помощью средства проверки целостности фоновых данных\n";
	if (attr & FILE_ATTRIBUTE_OFFLINE)
		ss << "- Данные файла недоступны немедленно\n";
	if (attr & FILE_ATTRIBUTE_READONLY)
		ss << "- Файл доступен только для чтения\n";
	if (attr & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
		ss << "- Файл не индексируются службой индексирования содержимого\n";
	if (attr & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS)
		ss << "- Файл не полностью присутствует локально\n";
	if (attr & FILE_ATTRIBUTE_RECALL_ON_OPEN)
		ss << "- Файл не имеет физического представления в локальной системе (является виртуальным)\n";
	if (attr & FILE_ATTRIBUTE_REPARSE_POINT)
		ss << "- Файл является символьной ссылкой\n";
	if (attr & FILE_ATTRIBUTE_SPARSE_FILE)
		ss << "- Файл является разреженным файлом\n";
	if (attr & FILE_ATTRIBUTE_SYSTEM)
		ss << "- Операционная система использует только часть файла или файл используется исключительно.\n";
	if (attr & FILE_ATTRIBUTE_TEMPORARY)
		ss << "- Файл используется для временного хранилища\n";
	if (attr & FILE_ATTRIBUTE_PINNED)
		ss << "- Файл должнен быть полностью представлен локально (даже если доступ к ней не выполняется активно)\n";
	if (attr & FILE_ATTRIBUTE_UNPINNED)
		ss << "- Файл или каталог не должны храниться локально (за исключением случаев активного доступа)\n";

	return ss.str();
}

void input_time(FILETIME* ft)
{
	SYSTEMTIME st = { 0 };
	DWORD value;
	cout << "Введите час, минуты, секунды, день, месяц и год через пробел : ";
	
	wcin >> value; st.wHour   = value; 
	wcin >> value; st.wMinute = value; 
	wcin >> value; st.wSecond = value; 
	wcin >> value; st.wDay	  = value; 
	wcin >> value; st.wMonth  = value; 
	wcin >> value; st.wYear   = value;
	st.wMilliseconds = 0; st.wDayOfWeek = 0;
	SystemTimeToFileTime(&st, ft);
}

void print_file_attributes(const wstring& file_path)
{
	DWORD attr = GetFileAttributes(file_path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES)
		print_winapi_error();
	else
	{
		cout << "Аттрибут файла: 0x" << hex << attr << "\n";
		cout << get_string_attributes(attr) << "\n";
	}
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
	auto f_handle = CreateFile(file_path.c_str(), NULL, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		return;
	}

	BY_HANDLE_FILE_INFORMATION inf;
	if (!GetFileInformationByHandle(f_handle, &inf))
	{
		print_winapi_error();
		CloseHandle(f_handle);
	}
	else
	{
		cout << "\nРазмер файла: " << inf.nFileSizeHigh << inf.nFileSizeLow << " байт";
		cout << "\nИндекс файла: " << inf.nFileIndexHigh << inf.nFileIndexLow;
		cout << "\nКоличество ссылок на файл: " << inf.nNumberOfLinks << "\n";
	}

	CloseHandle(f_handle);
}

void print_file_time_info(const wstring& file_path)
{
	auto f_handle = CreateFile(file_path.c_str(), NULL, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		return;
	}

	BY_HANDLE_FILE_INFORMATION inf;
	if (!GetFileInformationByHandle(f_handle, &inf))
	{
		print_winapi_error();
		CloseHandle(f_handle);
		return;
	}

	SYSTEMTIME creation_st, last_access_st, last_write_st;
	string creation_str, last_access_str, last_write_str;
	if (!FileTimeToSystemTime(&(inf.ftCreationTime), &creation_st))
	{
		print_winapi_error();
		CloseHandle(f_handle);
		return;
	}
	else
		creation_str = format("{}:{}:{} {}.{}.{}", creation_st.wHour, creation_st.wMinute, creation_st.wSecond,
			creation_st.wDay, creation_st.wMonth, creation_st.wYear);
	if (!FileTimeToSystemTime(&inf.ftLastWriteTime, &last_write_st))
	{
		print_winapi_error();
		CloseHandle(f_handle);
		return;
	}
	else
		last_write_str = format("{}:{}:{} {}.{}.{}", last_write_st.wHour, last_write_st.wMinute, last_write_st.wSecond,
			last_write_st.wDay, last_write_st.wMonth, last_write_st.wYear);
	if (!FileTimeToSystemTime(&inf.ftLastAccessTime, &last_access_st))
	{
		print_winapi_error();
		CloseHandle(f_handle);
		return;
	}
	else
		last_access_str = format("{}:{}:{} {}.{}.{}", last_access_st.wHour, last_access_st.wMinute, last_access_st.wSecond,
			last_access_st.wDay, last_access_st.wMonth, last_access_st.wYear);

	cout << "\nВремя создание: " << creation_str;
	cout << "\nВремя последнего использования: " << last_access_str;
	cout << "\nВремя последнего изменения: " << last_write_str << endl;

	CloseHandle(f_handle);
}

void change_file_time_info(const wstring& file_path)
{
	auto f_handle = CreateFile(file_path.c_str(),
		GENERIC_READ | FILE_WRITE_ATTRIBUTES,
		FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (f_handle == INVALID_HANDLE_VALUE)
	{
		print_winapi_error();
		return;
	}

	FILETIME* creation_ft = nullptr,
		* last_write_ft = nullptr,
		* last_access_ft = nullptr;
	bool yes;

	cout << "Хотите изменить время создания файла? (1 - да, 0 - нет)\n";
	cin >> yes;
	if (yes)
	{
		creation_ft = new FILETIME;
		input_time(creation_ft);
		if (!SetFileTime(f_handle, creation_ft, NULL, NULL))
		{
			print_winapi_error();
			CloseHandle(f_handle);
			delete creation_ft;
			return;
		}
	}
	cout << "Хотите изменить время последней записи файла? (1 - да, 0 - нет)\n";
	cin >> yes;
	if (yes)
	{
		last_write_ft = new FILETIME;
		input_time(last_write_ft);
		if (!SetFileTime(f_handle, NULL, last_write_ft, NULL))
		{
			print_winapi_error();
			CloseHandle(f_handle);
			delete creation_ft, last_write_ft;
			return;
		}
	}
	cout << "Хотите изменить время создания файла? (1 - да, 0 - нет)\n";
	cin >> yes;
	if (yes)
	{
		last_access_ft = new FILETIME;
		input_time(last_access_ft);
		if (!SetFileTime(f_handle, NULL, NULL, last_access_ft))
		{
			print_winapi_error();
			CloseHandle(f_handle);
			delete creation_ft, last_write_ft, last_access_ft;
			return;
		}
	}

	delete creation_ft, last_write_ft, last_access_ft;
	CloseHandle(f_handle);
}