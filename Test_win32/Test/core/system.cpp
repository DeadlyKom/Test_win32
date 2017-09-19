#include "system.h"

// Обработчик сообщений главного диалогового окна
static LONG_PTR CALLBACK SystemProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static System*	system = nullptr;

	switch (msg)
	{
	// Обработка сообщения создания окна
	case WM_CREATE:
		
		// Сохраним указатель на базовый класс
		system = (System*)((LPCREATESTRUCT)lParam)->lpCreateParams;		break;

	// Обработка изменения размеров окна
	case WM_SIZING:

		// Сохраним новые размеры окна
		system->SetRectWindow((LPRECT)lParam);							break;

	// Обработка нажатия левой кнопки мыши
	case WM_LBUTTONDOWN:

		// Сформируем круг по координатам клика
		system->LeftClickMouse(LOWORD(lParam), HIWORD(lParam));			break;

	// Обработка завершения работы окна
	case WM_DESTROY:

		// Отправим сообщение о прекражения работы цикла обработки сообщений
		PostQuitMessage(0);												break;

	// Дефолтная обработка сообщений
	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	// Сообщим что мы обрабатывает сообщения
	return TRUE;
}

// Конструктор
System::System()
{
	// Инициализируем дополнительный класс подсчёта времени
	m_time.Initialize();

	// Установим значение для случайности генерации чисел 
	srand((uint32_t)m_time.GetCurrentTick());

	// Получим размер экрана по горизонтали
	m_screen_width	= GetSystemMetrics(SM_CXSCREEN);

	// Получим размер экрана по вертикали
	m_screen_height	= GetSystemMetrics(SM_CYSCREEN);

	// Расчитаем начальное положение мяча 
	m_position_ball.cx	= m_screen_width >> 3;
	m_position_ball.cy	= m_screen_height >> 3;

	// Сгенерируем случайный размер мяча
	m_size_ball			= Rand(MIN_SIZE_BALL, MAX_SIZE_BALL);

	// Определим напровление движения мяча
	m_direction_vector.cx = 1;
	m_direction_vector.cy = 1;

	// Сформируем случайную скорость мяча
	RandSpeedball();

	// создадим красную кисть
	m_red_brush = CreateSolidBrush(COLOR_RED);

	m_hdcSystem = nullptr;
}

// Деструктор
System::~System()
{
	// Освобождение ресурсов
	Release();
}

// Запуск
int System::Launch()
{
	// Регистрируем класс окна
	Register();

	// Создаём окно
	Create();

	// Запускаем цикл обработки сообщений
	return Run();
}

// Освобождение ресурсов
void System::Release()
{
	// Отменим ранее зарегистрированный класс окна
	if (m_atomClass != 0)			{ UnregisterClass((LPCWSTR)m_atomClass, m_hInstance); }

	// Удалим созданную кисть
	if (m_red_brush != nullptr)		{ DeleteObject(m_red_brush); }

	//if(m_hdcSystem != nullptr) { ReleaseDC() }
}

// Установка размера окна
void System::SetRectWindow(LPRECT new_rect)
{
	// Получим новые размеры клиенской области окна
	GetClientRect(m_hwndSystem, &m_rect_window);

	// Переформируем старый контекст отрисовки окна
	ResetBufferDC();

	// Вызовим метод для обновления картинки
	Idle();
}

// Регистрация класса окна
void System::Register()
{
	WNDCLASSEX wcex;
	// Получим дескриптор данного экземпляра
	m_hInstance			= GetModuleHandle(NULL);

	// Формируем структуру класса окна
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)SystemProc;
	wcex.cbClsExtra		=
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= CLASS_NAME;
	wcex.hIconSm		= NULL;

	// Регистрируем ранее описанный класс окна
	m_atomClass			= RegisterClassEx(&wcex);
}

// Создание окна
void System::Create()
{
	// Расчитаем размеры окна
	uint32_t window_width		= m_screen_width	>> 1;
	uint32_t window_height		= m_screen_height	>> 1;

	// Расчитаем положение окна
	uint32_t window_position_x	= m_screen_width	>> 2;
	uint32_t window_position_y	= m_screen_height	>> 2;

	// Создадим окно
	m_hwndSystem = CreateWindowEx(NULL, (LPCWSTR)m_atomClass, WINDOW_NAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, window_position_x, window_position_y, window_width, window_height, NULL, NULL, m_hInstance, this);

	// Обновим его
	UpdateWindow(m_hwndSystem);

	// Получим размеры клиенской области окна
	GetClientRect(m_hwndSystem, &m_rect_window);

	// Создадим буфер контекста отрисовки
	CreateBufferDC();
}

// Запуск цикла обработки сообщений
int System::Run()
{
	MSG		msg;

	bool run = true;

	// Цикл обработки сообщений 
	while (run)
	{
		// Проверим на существование сообщений в общем списке (без удаления из очереди)
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			// Получим первое сообщение
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Если это сообщение о выходе, произведём его...
			else { run = false; break; }
		}

		// Простой обработчика сообщений
		Idle();
	}

	// Вернём прчину выхода
	return msg.wParam;
}

// Простой приложения
void System::Idle()
{
	static int64_t	time_old = m_time.GetCurrentTick();

	double	delta_time;

	// Получим текущее время
	int64_t time_current = m_time.GetCurrentTick();

	// Расчитаем разницу между замерами
	delta_time = m_time.TimeBetweenTicks(time_old, time_current);

	// Скоректируем значения, для правельных расчётов разницы ао времени
	time_old = time_current;

	// Обновим дейстия на экране
	Update((float)delta_time);

	// Отобразим элементы
	Render();

	// Произведём копирование заднего буфера на основной экран
	SwapBuffers();
}

// Обновление 
void System::Update(float delta)
{
	static float delta_time = 0;

	// Движение мяча каждые 0.015 сек
	if (delta_time > 0.015)
	{
		delta_time = 0;

		m_position_ball.cx += m_direction_vector.cx;
		m_position_ball.cy += m_direction_vector.cy;
	}

	// Получим итератор первого элемента блокирующих кругов
	std::vector<InfoBlock*>::iterator it_block = m_list_block.begin();

	// Сформируем цмкл обхода блокирующих кругов
	for (; it_block != m_list_block.end(); it_block++)
	{
		// Расчитаем растояние между центрами мяча и блокирующего круга
		float distance = (float)sqrt(pow(m_position_ball.cx - (*it_block)->position.cx, 2) + pow(m_position_ball.cy - (*it_block)->position.cy, 2));

		// Расчитаем сумму радиусов мяча и блокирующего круга
		float sum_radius = (float)((m_size_ball >> 1) + ((*it_block)->size >> 1));

		// Удалим блокирующий круг, если они пересикаются
		if (distance <= sum_radius)
		{
			// Удалим текущий блокирующий круг и скоректируем следующий элемент за ним
			it_block = m_list_block.erase(it_block);

			// Если после удаления элемента из списка он был последний, дальнейший обход не нужен
			if (it_block == m_list_block.end()) { break; }
		}
	}

	// Проверка пересечения мяча с краями по горихонтали
	if (m_rect_window.left   >= (m_position_ball.cx - (int)(m_size_ball >> 1))) { m_direction_vector.cx =  1; RandSpeedball(); }
	if (m_rect_window.right  <= (m_position_ball.cx + (int)(m_size_ball >> 1))) { m_direction_vector.cx = -1; RandSpeedball(); }

	// Проверка пересечения мяча с краями по вертикали
	if (m_rect_window.top    >= (m_position_ball.cy - (int)(m_size_ball >> 1))) { m_direction_vector.cy =  1; RandSpeedball(); }
	if (m_rect_window.bottom <= (m_position_ball.cy + (int)(m_size_ball >> 1))) { m_direction_vector.cy = -1; RandSpeedball(); }

	
	delta_time += delta;
}

// Отрисовка окна
void System::Render()
{
	if (m_hdcSystem == nullptr) { return; }

	// Очистим рабочее окно
	ClearDC();

	// Отрисуем мяч
	DrawCircle(m_position_ball.cx, m_position_ball.cy, m_size_ball, (HBRUSH)GetStockObject(BLACK_BRUSH));

	// Отрисуем все блокирующие круги
	for (auto& it_block : m_list_block)		{ DrawCircle(it_block->position.cx, it_block->position.cy, it_block->size, m_red_brush); }
}

// Создать буфер контекста 
void System::CreateBufferDC()
{
	// Получим контекст главного окна
	m_hdcSystem = GetDC(m_hwndSystem);

	// Создадим его копию
	m_hdcBuffer = CreateCompatibleDC(m_hdcSystem);

	// Выделим участок памяти для него по размерам
	m_memoryBM = CreateCompatibleBitmap(m_hdcSystem, m_rect_window.right, m_rect_window.bottom);

	// Выберим его
	m_gdiObjectBuffer_old = SelectObject(m_hdcBuffer, m_memoryBM);
}

// Сброс буфера контекста
void System::ResetBufferDC()
{
	// Выберим старый контекст
	SelectObject(m_hdcSystem, m_gdiObjectBuffer_old);

	// Освободим контекст буфера
	ReleaseDC(m_hwndSystem, m_hdcBuffer);
	
	// Удалим его
	DeleteDC(m_hdcBuffer);

	// Удалим участок памяти буферного контекста
	DeleteObject(m_memoryBM);

	// Создадим новй контекст буфера
	CreateBufferDC();
}

// Очистить контекст устройства
void System::ClearDC()
{
	BitBlt(m_hdcBuffer, m_rect_window.left, m_rect_window.top, m_rect_window.right, m_rect_window.bottom, 0, 0, 0, WHITENESS);
}

// Произвести подмену буферов
void System::SwapBuffers()
{
	BitBlt(m_hdcSystem, m_rect_window.left, m_rect_window.top, m_rect_window.right, m_rect_window.bottom, m_hdcBuffer, m_rect_window.left, m_rect_window.top, SRCCOPY);
}

// Изменение скорости
void System::RandSpeedball()
{
	int32_t m_speed_ball_x = Rand(MIN_SPEED_BALL, MAX_SPEED_BALL);
	int32_t m_speed_ball_y = Rand(MIN_SPEED_BALL, MAX_SPEED_BALL);

	m_direction_vector.cx = (m_direction_vector.cx > 0) ? m_speed_ball_x : - m_speed_ball_x;
	m_direction_vector.cy = (m_direction_vector.cy > 0) ? m_speed_ball_y : - m_speed_ball_y;
}

// Обработка левого клика мыши
void System::LeftClickMouse(uint32_t x, uint32_t y)
{
	InfoBlock* info_block = new InfoBlock();

	info_block->position.cx = x;
	info_block->position.cy = y;

	int d = rand();

	info_block->size		= Rand(MIN_SIZE_BLOCK, MAX_SIZE_BLOCK);
	
	m_list_block.push_back(info_block);
}

// Нарисовать круг
void System::DrawCircle(uint32_t x, uint32_t y, uint32_t size, HBRUSH color)
{
	RECT rect_circle;

	HGDIOBJ old_object = SelectObject(m_hdcBuffer, color);

	rect_circle.left	= x - (size >> 1);
	rect_circle.right	= x + (size >> 1);

	rect_circle.top		= y - (size >> 1);
	rect_circle.bottom	= y + (size >> 1);


	Ellipse(m_hdcBuffer, rect_circle.left, rect_circle.top, rect_circle.right, rect_circle.bottom);

	SelectObject(m_hdcBuffer, old_object);
}