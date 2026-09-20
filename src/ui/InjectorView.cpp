#include "InjectorView.h"

#include <algorithm>
#include <cctype>

#include "imgui.h"
#include "app/AppMetadata.h"


namespace
{
	constexpr int TitleBarHeight = 48;
	ImFont* titleFont = nullptr;

	void ApplyStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(18.0f, 14.0f);
		style.FramePadding = ImVec2(12.0f, 8.0f);
		style.ItemSpacing = ImVec2(10.0f, 9.0f);
		style.WindowRounding = 10.0f;
		style.ChildRounding = 9.0f;
		style.FrameRounding = 7.0f;
		style.FrameBorderSize = 1.0f;
		style.ScrollbarRounding = 8.0f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.91f, 0.93f, 0.98f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.54f, 0.65f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.035f, 0.045f, 0.075f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.055f, 0.070f, 0.115f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.16f, 0.22f, 0.34f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.12f, 0.18f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.075f, 0.095f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.14f, 0.23f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.12f, 0.17f, 0.28f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.18f, 0.38f, 0.78f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.48f, 0.92f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.30f, 0.65f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.38f, 0.76f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.25f, 0.68f, 1.00f, 1.00f);
	}

	void LoadFonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImFont* body = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 16.0f))
			io.FontDefault = body;
		titleFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 18.0f);
	}
}

InjectorView::InjectorView(SDL_Window* parentWindow, AppController& appController, AppState& appState)
	: window(parentWindow), controller(appController), state(appState)
{
	ApplyStyle();
	LoadFonts();
}

void InjectorView::OnFileSelected(void* userdata, const char* const* fileList, int)
{
	auto* view = static_cast<InjectorView*>(userdata);
	if (fileList == nullptr || fileList[0] == nullptr)
		return;

	std::scoped_lock lock(view->selectionMutex);
	view->selectedPath = fileList[0];
	view->hasSelectedPath = true;
}

void InjectorView::OpenModFileDialog()
{
	static const SDL_DialogFileFilter filters[] = {
		{ "Dll", "dll" },
		{ "All files", "*" }
	};
	SDL_ShowOpenFileDialog(OnFileSelected, this, window, filters, 2, nullptr, false);
	controller.Dispatch(AppController::UiAction::SelectDll);
}

void InjectorView::ApplySelectedFile()
{
	std::scoped_lock lock(selectionMutex);
	if (!hasSelectedPath)
		return;

	dllPathInput.fill('\0');
	const size_t length = (std::min)(selectedPath.size(), dllPathInput.size() - 1);
	std::copy_n(selectedPath.data(), length, dllPathInput.data());
	controller.SetDllPath(std::string(dllPathInput.data()));
	hasSelectedPath = false;
}

void InjectorView::RefreshProcesses()
{
	processes = injector::process::EnumerateProcesses();
}

void InjectorView::RenderProcessFinder()
{
	if (!showProcessFinder)
		return;

	ImGui::SetNextWindowSize(ImVec2(420.0f, 360.0f), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Find process", &showProcessFinder))
	{
		ImGui::End();
		return;
	}

	if (ImGui::Button("Refresh"))
		RefreshProcesses();
	ImGui::SameLine();
	ImGui::TextDisabled("%zu running processes", processes.size());
	ImGui::SetNextItemWidth(-1.0f);
	ImGui::InputTextWithHint("##process-search", "Search process name or PID...", processSearch.data(), processSearch.size());
	ImGui::Separator();

	std::string query(processSearch.data());
	std::transform(query.begin(), query.end(), query.begin(),
		[](unsigned char character) { return static_cast<char>(std::tolower(character)); });

	if (ImGui::BeginChild("##process-list", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders))
	{
		for (const injector::process::ProcessEntry& process : processes)
		{
			const std::string label = process.name + "  (" + std::to_string(process.id) + ")";
			std::string searchable = label;
			std::transform(searchable.begin(), searchable.end(), searchable.begin(),
				[](unsigned char character) { return static_cast<char>(std::tolower(character)); });
			if (!query.empty() && searchable.find(query) == std::string::npos)
				continue;
			if (ImGui::Selectable(label.c_str()))
			{
				const std::string id = std::to_string(process.id);
				targetInput.fill('\0');
				std::copy_n(id.data(), (std::min)(id.size(), targetInput.size() - 1), targetInput.data());
				state.targetName = process.name;
				controller.SetTarget(id);
				controller.Dispatch(AppController::UiAction::ValidateTarget);
				showProcessFinder = false;
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void InjectorView::Render(bool& shouldClose)
{
	controller.Dispatch(AppController::UiAction::Update);
	ApplySelectedFile();
	RenderProcessFinder();
	const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(displaySize);
	ImGui::Begin("LEAK Injector", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 position = ImGui::GetWindowPos();
	const ImVec2 size = ImGui::GetWindowSize();
	drawList->AddRectFilled(position, ImVec2(position.x + size.x, position.y + TitleBarHeight), ImGui::GetColorU32(ImVec4(0.055f, 0.075f, 0.13f, 1.00f)));
	drawList->AddRectFilled(ImVec2(position.x, position.y), ImVec2(position.x + 170.0f, position.y + 2.0f), ImGui::GetColorU32(ImVec4(0.30f, 0.68f, 1.00f, 1.00f)));
	drawList->AddLine(ImVec2(position.x, position.y + TitleBarHeight), ImVec2(position.x + size.x, position.y + TitleBarHeight), ImGui::GetColorU32(ImGuiCol_Border));

	if (titleFont) ImGui::PushFont(titleFont);
	ImGui::TextColored(ImVec4(0.88f, 0.94f, 1.0f, 1.0f), "LEAK");
	if (titleFont) ImGui::PopFont();
	ImGui::SameLine(0.0f, 8.0f);
	ImGui::TextDisabled("Injector");

	constexpr float titleButtonSize = 28.0f;
	ImGui::SetCursorPos(ImVec2(size.x - 70.0f, 10.0f));
	const ImVec2 minimizePosition = ImGui::GetCursorScreenPos();
	ImGui::InvisibleButton("##minimize", ImVec2(titleButtonSize, titleButtonSize));
	if (ImGui::IsItemClicked()) SDL_MinimizeWindow(window);
	if (ImGui::IsItemHovered())
		drawList->AddRectFilled(minimizePosition, ImVec2(minimizePosition.x + titleButtonSize, minimizePosition.y + titleButtonSize), ImGui::GetColorU32(ImVec4(0.17f, 0.22f, 0.29f, 1.00f)), 5.0f);
	drawList->AddLine(ImVec2(minimizePosition.x + 9.0f, minimizePosition.y + 16.0f), ImVec2(minimizePosition.x + 19.0f, minimizePosition.y + 16.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), 1.5f);

	ImGui::SetCursorPos(ImVec2(size.x - 36.0f, 10.0f));
	const ImVec2 closePosition = ImGui::GetCursorScreenPos();
	ImGui::InvisibleButton("##close", ImVec2(titleButtonSize, titleButtonSize));
	if (ImGui::IsItemClicked()) shouldClose = true;
	if (ImGui::IsItemHovered())
		drawList->AddRectFilled(closePosition, ImVec2(closePosition.x + titleButtonSize, closePosition.y + titleButtonSize), ImGui::GetColorU32(ImVec4(0.55f, 0.19f, 0.23f, 1.00f)), 5.0f);
	drawList->AddLine(ImVec2(closePosition.x + 10.0f, closePosition.y + 10.0f), ImVec2(closePosition.x + 18.0f, closePosition.y + 18.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), 1.5f);
	drawList->AddLine(ImVec2(closePosition.x + 18.0f, closePosition.y + 10.0f), ImVec2(closePosition.x + 10.0f, closePosition.y + 18.0f), ImGui::GetColorU32(ImGuiCol_TextDisabled), 1.5f);

	ImGui::SetCursorPos(ImVec2(18.0f, TitleBarHeight + 18.0f));
	if (ImGui::BeginChild("##config-card", ImVec2(size.x - 36.0f, 248.0f), true,
		ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::TextColored(ImVec4(0.38f, 0.76f, 1.0f, 1.0f), "CONFIGURATION");
		ImGui::SameLine();
		ImGui::TextDisabled("Choose a target and payload");
		ImGui::Spacing();

		ImGui::TextDisabled("TARGET PROCESS");
		ImGui::SetNextItemWidth(-102.0f);
		ImGui::InputTextWithHint("##target", "Process name or PID", targetInput.data(), targetInput.size());
		controller.SetTarget(std::string(targetInput.data()));
		ImGui::SameLine();
		if (ImGui::Button("Find", ImVec2(82.0f, 0.0f)))
		{
			RefreshProcesses();
			showProcessFinder = true;
		}
		if (!state.targetName.empty())
		{
			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.45f, 0.85f, 0.65f, 1.0f), "TARGET SELECTED");
			ImGui::SameLine();
			ImGui::TextDisabled("%s  •  PID %s", state.targetName.c_str(), targetInput.data());
		}

		ImGui::TextDisabled("DLL FILE");
		ImGui::SetNextItemWidth(-102.0f);
		ImGui::InputText("##dll-file", dllPathInput.data(), dllPathInput.size(), ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("Browse", ImVec2(82.0f, 0.0f))) OpenModFileDialog();
		ImGui::Spacing();
		ImGui::TextDisabled("METHOD OF INJECTION");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.55f, 0.78f, 1.0f, 1.0f), "%s", state.injectionMethod.c_str());
		ImGui::EndChild();
	}

	ImGui::SetCursorPos(ImVec2(18.0f, TitleBarHeight + 282.0f));
	if (ImGui::BeginChild("##action-card", ImVec2(size.x - 36.0f, 156.0f), true,
		ImGuiWindowFlags_NoScrollbar))
	{
		const bool ready = targetInput.front() != '\0' && dllPathInput.front() != '\0';
		ImGui::TextColored(ImVec4(0.38f, 0.76f, 1.0f, 1.0f), "OPERATION");
		ImGui::SameLine();
		const ImVec4 statusColor = state.busy ? ImVec4(1.0f, 0.75f, 0.25f, 1.0f) :
			(state.status == "Injected successfully" ? ImVec4(0.45f, 0.85f, 0.65f, 1.0f) : ImVec4(0.60f, 0.67f, 0.78f, 1.0f));
		ImGui::TextColored(statusColor, "%s", state.status.c_str());
		ImGui::Spacing();
		ImGui::TextDisabled(state.busy ? "The selected payload is being prepared..." : "Ready when you are.");
		ImGui::Spacing();
		ImGui::BeginDisabled(!ready || state.busy);
		if (ImGui::Button(state.busy ? "Working..." : "Inject", ImVec2(132.0f, 34.0f)))
			controller.Dispatch(AppController::UiAction::Inject);
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Clear", ImVec2(86.0f, 34.0f)))
		{
			targetInput.fill('\0');
			dllPathInput.fill('\0');
			controller.Dispatch(AppController::UiAction::Clear);
		}
		ImGui::SameLine();
		if (state.busy || state.totalBytes > 0)
		{
			ImGui::SetNextItemWidth(-1.0f);
			ImGui::ProgressBar(state.progress, ImVec2(-1.0f, 10.0f), "");
		}
		if (state.totalBytes > 0)
			ImGui::TextDisabled("Payload transfer  •  %zu / %zu bytes", state.bytesWritten, state.totalBytes);
		ImGui::EndChild();
	}

	ImGui::SetCursorPos(ImVec2(18.0f, size.y - 30.0f));
	ImGui::TextDisabled("LEAK  /  %s", leak::app::Metadata.build);
	ImGui::End();
}
