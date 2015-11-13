#pragma once

class RendererComponent {
public:
	enum class ResetPriority {
		Low, Medium, High
	};
	ResetPriority mResetPriority;
	static vector<RendererComponent*> msComponentList;
	explicit RendererComponent();
	virtual ~RendererComponent();
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
};