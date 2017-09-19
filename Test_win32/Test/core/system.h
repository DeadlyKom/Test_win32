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
	// Конструктор
							System();
	// Деструктор
							~System();
	// Запуск
	int						Launch();
	// Освобождение ресурсов
	void					Release();

	// Установка размера окна
	void					SetRectWindow(LPRECT new_rect);
	// Обработка левого клика мыши
	void					LeftClickMouse(uint32_t x, uint32_t y);

private:

	// Регистрация класса окна
	void					Register();
	// Создание окна
	void					Create();
	// Запуск цикла обработки сообщений
	int						Run();
	// Простой приложения
	void					Idle();
	// Обновление 
	void					Update(float delta);
	// Отрисовка окна
	void					Render();

	// Создать буфер контекста 
	void					CreateBufferDC();
	// Сброс буфера контекста
	void					ResetBufferDC();
	// Очистить контекст устройства
	void					ClearDC();
	// Произвести подмену буферов
	void					SwapBuffers();

	// Изменение скорости
	void					RandSpeedball();

	// Нарисовать круг
	void					DrawCircle(uint32_t x, uint32_t y, uint32_t size, HBRUSH color);

	HINSTANCE				m_hInstance;			// дескриптор экземпляра
	ATOM					m_atomClass;			// атом зарегистрированного класса
	HWND					m_hwndSystem;			// дескриптор окна
	HDC						m_hdcSystem;			// контест устройства окна
	HDC						m_hdcBuffer;			// контекст устройства буфера
	HBITMAP					m_memoryBM;				// дескриптор участка памяти выделенный под хранение буфера
	HGDIOBJ					m_gdiObjectBuffer_old;	// предыдущий объект контекста буфера
	RECT					m_rect_window;			// размер окна
	HBRUSH					m_red_brush;			// красная кисть

	uint32_t				m_screen_width;			// ширина экрана
	uint32_t				m_screen_height;		// высота экрана

	SystemTime				m_time;					// стороний класс для подсчёта времени

	SIZE					m_position_ball;		// координаты мяча
	uint32_t				m_size_ball;			// размер мяча

	SIZE					m_direction_vector;		// направляющий вектор движения

	std::vector<InfoBlock*>	m_list_block;			// список блокирующих кругов
};