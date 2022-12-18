#include "virtmem.h"

int main()
{
	setlocale(LC_ALL, "ru");
	string inp;
	while (true)
	{
		cout << "1. Информация о вычислительной системе" << endl
			<< "2. Статус виртуальной памяти" << endl
			<< "3. Состояние участка памяти" << endl
			<< "4. Резервирование региона виртуальной памяти" << endl
			<< "5. Резервирование региона виртуальной памяти и передача ему физической памяти" << endl
			<< "6. Запись данных в ячейку памяти" << endl
			<< "7. Установка защиты доступа для региона памяти" << endl
			<< "8. Возврат физической памяти и освобождение региона адресного пространства" << endl
			<< "9. Закрыть программу" << endl;
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