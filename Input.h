#pragma once

class Engine;

class Input : public ruInput {
private:
	Engine * const mEngine;

	LPDIRECTINPUT8 pInput;
	LPDIRECTINPUTDEVICE8 pKeyboard;
	LPDIRECTINPUTDEVICE8 pMouse;
	DIMOUSESTATE mouseData;
	HWND hwnd;

	char g_keys[256] = {0};
	char g_lastKeys[256] = {0};
	char g_keysHit[256] = {0};
	char g_keysUp[256] = {0};

	char g_mouseUp[4] = {0};
	char g_mouseHit[4] = {0};
	char g_mousePressed[4] = {0};

	int mouseX = 0;
	int mouseY = 0;
	int mouseWheel = 0;
	int windowOffsetX = 0;
	int windowOffsetY = 0;
	RECT windowRect;
public:
	Input(Engine * engine, HWND window);
	~Input();

	virtual string GetKeyName(ruInput::Key key) override final;
	virtual void Update() override final;
	virtual bool IsKeyDown(Key key) override final;
	virtual bool IsKeyHit(Key key) override final;
	virtual bool IsKeyUp(Key key) override final;
	virtual bool IsMouseDown(MouseButton button) override final;
	virtual bool IsMouseHit(MouseButton button) override final;
	virtual int GetMouseX() override final;
	virtual int GetMouseY() override final;
	virtual int GetMouseWheel() override final;
	virtual int GetMouseXSpeed() override final;
	virtual int GetMouseYSpeed() override final;
	virtual int GetMouseWheelSpeed() override final;
};
