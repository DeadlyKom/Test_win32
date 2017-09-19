#pragma once

#include "pch.h"
#include "system_time.h"

struct InfoBlock
{
	SIZE		position;
	uint32_t	size;
};

class System
{
public:
	// �����������
							System();
	// ����������
							~System();
	// ������
	int						Launch();
	// ������������ ��������
	void					Release();

	// ��������� ������� ����
	void					SetRectWindow(LPRECT new_rect);
	// ��������� ������ ����� ����
	void					LeftClickMouse(uint32_t x, uint32_t y);

private:

	// ����������� ������ ����
	void					Register();
	// �������� ����
	void					Create();
	// ������ ����� ��������� ���������
	int						Run();
	// ������� ����������
	void					Idle();
	// ���������� 
	void					Update(float delta);
	// ��������� ����
	void					Render();

	// ������� ����� ��������� 
	void					CreateBufferDC();
	// ����� ������ ���������
	void					ResetBufferDC();
	// �������� �������� ����������
	void					ClearDC();
	// ���������� ������� �������
	void					SwapBuffers();

	// ��������� ��������
	void					RandSpeedball();

	// ���������� ����
	void					DrawCircle(uint32_t x, uint32_t y, uint32_t size, HBRUSH color);

	HINSTANCE				m_hInstance;			// ���������� ����������
	ATOM					m_atomClass;			// ���� ������������������� ������
	HWND					m_hwndSystem;			// ���������� ����
	HDC						m_hdcSystem;			// ������� ���������� ����
	HDC						m_hdcBuffer;			// �������� ���������� ������
	HBITMAP					m_memoryBM;				// ���������� ������� ������ ���������� ��� �������� ������
	HGDIOBJ					m_gdiObjectBuffer_old;	// ���������� ������ ��������� ������
	RECT					m_rect_window;			// ������ ����
	HBRUSH					m_red_brush;			// ������� �����

	uint32_t				m_screen_width;			// ������ ������
	uint32_t				m_screen_height;		// ������ ������

	SystemTime				m_time;					// �������� ����� ��� �������� �������

	SIZE					m_position_ball;		// ���������� ����
	uint32_t				m_size_ball;			// ������ ����

	SIZE					m_direction_vector;		// ������������ ������ ��������

	std::vector<InfoBlock*>	m_list_block;			// ������ ����������� ������
};