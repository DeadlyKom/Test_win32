#include "system.h"

// ���������� ��������� �������� ����������� ����
static LONG_PTR CALLBACK SystemProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static System*	system = nullptr;

	switch (msg)
	{
	// ��������� ��������� �������� ����
	case WM_CREATE:
		
		// �������� ��������� �� ������� �����
		system = (System*)((LPCREATESTRUCT)lParam)->lpCreateParams;		break;

	// ��������� ��������� �������� ����
	case WM_SIZING:

		// �������� ����� ������� ����
		system->SetRectWindow((LPRECT)lParam);							break;

	// ��������� ������� ����� ������ ����
	case WM_LBUTTONDOWN:

		// ���������� ���� �� ����������� �����
		system->LeftClickMouse(LOWORD(lParam), HIWORD(lParam));			break;

	// ��������� ���������� ������ ����
	case WM_DESTROY:

		// �������� ��������� � ����������� ������ ����� ��������� ���������
		PostQuitMessage(0);												break;

	// ��������� ��������� ���������
	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	// ������� ��� �� ������������ ���������
	return TRUE;
}

// �����������
System::System()
{
	// �������������� �������������� ����� �������� �������
	m_time.Initialize();

	// ��������� �������� ��� ����������� ��������� ����� 
	srand((uint32_t)m_time.GetCurrentTick());

	// ������� ������ ������ �� �����������
	m_screen_width	= GetSystemMetrics(SM_CXSCREEN);

	// ������� ������ ������ �� ���������
	m_screen_height	= GetSystemMetrics(SM_CYSCREEN);

	// ��������� ��������� ��������� ���� 
	m_position_ball.cx	= m_screen_width >> 3;
	m_position_ball.cy	= m_screen_height >> 3;

	// ����������� ��������� ������ ����
	m_size_ball			= Rand(MIN_SIZE_BALL, MAX_SIZE_BALL);

	// ��������� ����������� �������� ����
	m_direction_vector.cx = 1;
	m_direction_vector.cy = 1;

	// ���������� ��������� �������� ����
	RandSpeedball();

	// �������� ������� �����
	m_red_brush = CreateSolidBrush(COLOR_RED);

	m_hdcSystem = nullptr;
}

// ����������
System::~System()
{
	// ������������ ��������
	Release();
}

// ������
int System::Launch()
{
	// ������������ ����� ����
	Register();

	// ������ ����
	Create();

	// ��������� ���� ��������� ���������
	return Run();
}

// ������������ ��������
void System::Release()
{
	// ������� ����� ������������������ ����� ����
	if (m_atomClass != 0)			{ UnregisterClass((LPCWSTR)m_atomClass, m_hInstance); }

	// ������ ��������� �����
	if (m_red_brush != nullptr)		{ DeleteObject(m_red_brush); }

	//if(m_hdcSystem != nullptr) { ReleaseDC() }
}

// ��������� ������� ����
void System::SetRectWindow(LPRECT new_rect)
{
	// ������� ����� ������� ��������� ������� ����
	GetClientRect(m_hwndSystem, &m_rect_window);

	// ������������� ������ �������� ��������� ����
	ResetBufferDC();

	// ������� ����� ��� ���������� ��������
	Idle();
}

// ����������� ������ ����
void System::Register()
{
	WNDCLASSEX wcex;
	// ������� ���������� ������� ����������
	m_hInstance			= GetModuleHandle(NULL);

	// ��������� ��������� ������ ����
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

	// ������������ ����� ��������� ����� ����
	m_atomClass			= RegisterClassEx(&wcex);
}

// �������� ����
void System::Create()
{
	// ��������� ������� ����
	uint32_t window_width		= m_screen_width	>> 1;
	uint32_t window_height		= m_screen_height	>> 1;

	// ��������� ��������� ����
	uint32_t window_position_x	= m_screen_width	>> 2;
	uint32_t window_position_y	= m_screen_height	>> 2;

	// �������� ����
	m_hwndSystem = CreateWindowEx(NULL, (LPCWSTR)m_atomClass, WINDOW_NAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, window_position_x, window_position_y, window_width, window_height, NULL, NULL, m_hInstance, this);

	// ������� ���
	UpdateWindow(m_hwndSystem);

	// ������� ������� ��������� ������� ����
	GetClientRect(m_hwndSystem, &m_rect_window);

	// �������� ����� ��������� ���������
	CreateBufferDC();
}

// ������ ����� ��������� ���������
int System::Run()
{
	MSG		msg;

	bool run = true;

	// ���� ��������� ��������� 
	while (run)
	{
		// �������� �� ������������� ��������� � ����� ������ (��� �������� �� �������)
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			// ������� ������ ���������
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// ���� ��� ��������� � ������, ��������� ���...
			else { run = false; break; }
		}

		// ������� ����������� ���������
		Idle();
	}

	// ����� ������ ������
	return msg.wParam;
}

// ������� ����������
void System::Idle()
{
	static int64_t	time_old = m_time.GetCurrentTick();

	double	delta_time;

	// ������� ������� �����
	int64_t time_current = m_time.GetCurrentTick();

	// ��������� ������� ����� ��������
	delta_time = m_time.TimeBetweenTicks(time_old, time_current);

	// ������������ ��������, ��� ���������� �������� ������� �� �������
	time_old = time_current;

	// ������� ������� �� ������
	Update((float)delta_time);

	// ��������� ��������
	Render();

	// ��������� ����������� ������� ������ �� �������� �����
	SwapBuffers();
}

// ���������� 
void System::Update(float delta)
{
	static float delta_time = 0;

	// �������� ���� ������ 0.015 ���
	if (delta_time > 0.015)
	{
		delta_time = 0;

		m_position_ball.cx += m_direction_vector.cx;
		m_position_ball.cy += m_direction_vector.cy;
	}

	// ������� �������� ������� �������� ����������� ������
	std::vector<InfoBlock*>::iterator it_block = m_list_block.begin();

	// ���������� ���� ������ ����������� ������
	for (; it_block != m_list_block.end(); it_block++)
	{
		// ��������� ��������� ����� �������� ���� � ������������ �����
		float distance = (float)sqrt(pow(m_position_ball.cx - (*it_block)->position.cx, 2) + pow(m_position_ball.cy - (*it_block)->position.cy, 2));

		// ��������� ����� �������� ���� � ������������ �����
		float sum_radius = (float)((m_size_ball >> 1) + ((*it_block)->size >> 1));

		// ������ ����������� ����, ���� ��� ������������
		if (distance <= sum_radius)
		{
			// ������ ������� ����������� ���� � ������������ ��������� ������� �� ���
			it_block = m_list_block.erase(it_block);

			// ���� ����� �������� �������� �� ������ �� ��� ���������, ���������� ����� �� �����
			if (it_block == m_list_block.end()) { break; }
		}
	}

	// �������� ����������� ���� � ������ �� �����������
	if (m_rect_window.left   >= (m_position_ball.cx - (int)(m_size_ball >> 1))) { m_direction_vector.cx =  1; RandSpeedball(); }
	if (m_rect_window.right  <= (m_position_ball.cx + (int)(m_size_ball >> 1))) { m_direction_vector.cx = -1; RandSpeedball(); }

	// �������� ����������� ���� � ������ �� ���������
	if (m_rect_window.top    >= (m_position_ball.cy - (int)(m_size_ball >> 1))) { m_direction_vector.cy =  1; RandSpeedball(); }
	if (m_rect_window.bottom <= (m_position_ball.cy + (int)(m_size_ball >> 1))) { m_direction_vector.cy = -1; RandSpeedball(); }

	
	delta_time += delta;
}

// ��������� ����
void System::Render()
{
	if (m_hdcSystem == nullptr) { return; }

	// ������� ������� ����
	ClearDC();

	// �������� ���
	DrawCircle(m_position_ball.cx, m_position_ball.cy, m_size_ball, (HBRUSH)GetStockObject(BLACK_BRUSH));

	// �������� ��� ����������� �����
	for (auto& it_block : m_list_block)		{ DrawCircle(it_block->position.cx, it_block->position.cy, it_block->size, m_red_brush); }
}

// ������� ����� ��������� 
void System::CreateBufferDC()
{
	// ������� �������� �������� ����
	m_hdcSystem = GetDC(m_hwndSystem);

	// �������� ��� �����
	m_hdcBuffer = CreateCompatibleDC(m_hdcSystem);

	// ������� ������� ������ ��� ���� �� ��������
	m_memoryBM = CreateCompatibleBitmap(m_hdcSystem, m_rect_window.right, m_rect_window.bottom);

	// ������� ���
	m_gdiObjectBuffer_old = SelectObject(m_hdcBuffer, m_memoryBM);
}

// ����� ������ ���������
void System::ResetBufferDC()
{
	// ������� ������ ��������
	SelectObject(m_hdcSystem, m_gdiObjectBuffer_old);

	// ��������� �������� ������
	ReleaseDC(m_hwndSystem, m_hdcBuffer);
	
	// ������ ���
	DeleteDC(m_hdcBuffer);

	// ������ ������� ������ ��������� ���������
	DeleteObject(m_memoryBM);

	// �������� ���� �������� ������
	CreateBufferDC();
}

// �������� �������� ����������
void System::ClearDC()
{
	BitBlt(m_hdcBuffer, m_rect_window.left, m_rect_window.top, m_rect_window.right, m_rect_window.bottom, 0, 0, 0, WHITENESS);
}

// ���������� ������� �������
void System::SwapBuffers()
{
	BitBlt(m_hdcSystem, m_rect_window.left, m_rect_window.top, m_rect_window.right, m_rect_window.bottom, m_hdcBuffer, m_rect_window.left, m_rect_window.top, SRCCOPY);
}

// ��������� ��������
void System::RandSpeedball()
{
	int32_t m_speed_ball_x = Rand(MIN_SPEED_BALL, MAX_SPEED_BALL);
	int32_t m_speed_ball_y = Rand(MIN_SPEED_BALL, MAX_SPEED_BALL);

	m_direction_vector.cx = (m_direction_vector.cx > 0) ? m_speed_ball_x : - m_speed_ball_x;
	m_direction_vector.cy = (m_direction_vector.cy > 0) ? m_speed_ball_y : - m_speed_ball_y;
}

// ��������� ������ ����� ����
void System::LeftClickMouse(uint32_t x, uint32_t y)
{
	InfoBlock* info_block = new InfoBlock();

	info_block->position.cx = x;
	info_block->position.cy = y;

	int d = rand();

	info_block->size		= Rand(MIN_SIZE_BLOCK, MAX_SIZE_BLOCK);
	
	m_list_block.push_back(info_block);
}

// ���������� ����
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