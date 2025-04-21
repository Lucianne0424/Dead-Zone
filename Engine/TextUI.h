#pragma once
class TextUI
{
public:
	TextUI();
	virtual ~TextUI();

public:
	void Update();
	void Render();

	void SetText(const wstring& text) { _text = text; }
	wstring GetText() { return _text; }

	void SetRect(D2D1_RECT_F rect) { _rect = rect; }
	D2D1_RECT_F GetRect() { return _rect; }

	void SetBrush(ComPtr<ID2D1SolidColorBrush> brush) { _brush = brush; }
	void SetTextFormat(ComPtr<IDWriteTextFormat> textFormat) { _textFormat = textFormat; }

private:
	ComPtr<ID2D1SolidColorBrush> _brush;
	ComPtr<IDWriteTextFormat> _textFormat;

	wstring _text;
	D2D1_RECT_F _rect;
};

class AmmoUI : public TextUI
{
public:
	AmmoUI();
	virtual ~AmmoUI();

public:

};

class TimerUI : public TextUI
{
public:
	TimerUI();
	virtual ~TimerUI();

public:

private:
};