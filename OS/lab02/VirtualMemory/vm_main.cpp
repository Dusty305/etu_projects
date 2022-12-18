#include "virtmem.h"

int main()
{
	setlocale(LC_ALL, "ru");
	string inp;
	while (true)
	{
		cout << "1. ���������� � �������������� �������" << endl
			<< "2. ������ ����������� ������" << endl
			<< "3. ��������� ������� ������" << endl
			<< "4. �������������� ������� ����������� ������" << endl
			<< "5. �������������� ������� ����������� ������ � �������� ��� ���������� ������" << endl
			<< "6. ������ ������ � ������ ������" << endl
			<< "7. ��������� ������ ������� ��� ������� ������" << endl
			<< "8. ������� ���������� ������ � ������������ ������� ��������� ������������" << endl
			<< "9. ������� ���������" << endl;
		cin >> inp;
		switch (inp[0])
		{
		case '1':
			get_system_info();
			break;
		case '2':
			get_vm_status();
			break;
		case '3':
			get_pm_status();
			break;
		case '4':
			alloc_vm(MEM_RESERVE);
			break;
		case '5':
			alloc_vm(MEM_COMMIT);
			break;
		case '6':
			write_to_pm();
			break;
		case '7':
			set_pm_protection();
			break;
		case '8':
			free_pm();
			break;
		case '9':
			return 0;
		}
		system("pause");
	}
}