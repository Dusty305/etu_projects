#include "asynccopy.h"

int main()
{
	setlocale(LC_ALL, "Russian");

	AsyncCopyHandle** ach = new AsyncCopyHandle*[10];

	cout << "���� ���������� � ������...\n";
	unordered_map<WCHAR, Volume*> volumes;
	wstring vol = L"A:\\";
	for (auto list = GetLogicalDrives(); list; list /= 2, vol[0]++)
		if (list % 2)
			volumes[vol[0]] = new Volume(vol);
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
		case '1':
			print_disks(volumes);
			break;
		case '2':
			print_disk_info(volumes);
			break;
		case '3':
			create_remove_directory();
			break;
		case '4':
			create_file();
			break;
		case '5':
			copy_move_file();
			break;
		case '6':
			print_change_file_attributes();
			break;
		case '7':
			async_file_copy(volumes);
			break;
		case '8':
			for (auto it = volumes.begin(); it != volumes.end(); it++)
				delete it->second;
			return 0;
		}
		system("pause");
	}
}