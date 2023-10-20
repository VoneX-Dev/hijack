
namespace DirectX9Interface
{
	IDirect3D9Ex* p_object = NULL;
	IDirect3DDevice9Ex* p_device = NULL;
	D3DPRESENT_PARAMETERS p_params = { NULL };
	RECT game_rect = { NULL };
	MSG message = { NULL };
	HWND hijacked_hwnd = NULL;
	HWND game_wnd = NULL;
}

__forceinline VOID directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9Interface::p_object)))
		exit(3);

	ZeroMemory(&DirectX9Interface::p_params, sizeof(DirectX9Interface::p_params));
	DirectX9Interface::p_params.Windowed = TRUE;
	DirectX9Interface::p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	DirectX9Interface::p_params.hDeviceWindow = DirectX9Interface::hijacked_hwnd;
	DirectX9Interface::p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	DirectX9Interface::p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	DirectX9Interface::p_params.BackBufferWidth = Width;
	DirectX9Interface::p_params.BackBufferHeight = Height;
	DirectX9Interface::p_params.EnableAutoDepthStencil = TRUE;
	DirectX9Interface::p_params.AutoDepthStencilFormat = D3DFMT_D16;
	DirectX9Interface::p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(DirectX9Interface::p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, DirectX9Interface::hijacked_hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &DirectX9Interface::p_params, 0, &DirectX9Interface::p_device)))
	{
		DirectX9Interface::p_object->Release();
		Sleep(1000);
		exit(1);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(DirectX9Interface::hijacked_hwnd);
	ImGui_ImplDX9_Init(DirectX9Interface::p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	static const ImWchar icons_ranges[] = { 0xf000, 0xf5ff, 0 };
	ImFontConfig icons_config;

	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	ImFontConfig config;

	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.OversampleH = 1;
	icons_config.OversampleV = 1;
	ImFontConfig CustomFont;
	CustomFont.FontDataOwnedByAtlas = false;

	ESPFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 15.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
	bold_segu = io.Fonts->AddFontFromMemoryTTF(&bold_segue, sizeof bold_segue, 30, NULL, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 14.0f, &CustomFont);
	io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 13.0f, &icons_config, icons_ranges);
	titleicon = io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 25.0f, &icons_config, icons_ranges);
	subtitle = io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 13.0f, &CustomFont);

	DirectX9Interface::p_object->Release();
}

namespace hijack
{
	bool amd = false;
	bool nvidia = false;

	__forceinline void hijack_init()
	{
		if (LI_FN(FindWindowA)(skCrypt("CEF-OSC-WIDGET"), skCrypt("NVIDIA GeForce Overlay")))
		{
			nvidia = true;
			DirectX9Interface::hijacked_hwnd = LI_FN(FindWindowA)(skCrypt("CEF-OSC-WIDGET"), skCrypt("NVIDIA GeForce Overlay"));
			NOTIFYICONDATA nid = {};
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = NULL;
			nid.uID = 1;
			nid.uFlags = NIF_INFO;
			nid.dwInfoFlags = NIIF_INFO;
			strcpy_s(nid.szInfoTitle, "External Notification");
			strcpy_s(nid.szInfo, "Found NVIDIA GeFore Experience Overlay");
			Shell_NotifyIcon(NIM_ADD, &nid);
			Sleep(5000);
			Shell_NotifyIcon(NIM_DELETE, &nid);
		}
		else if (LI_FN(FindWindowA)(skCrypt("AMDDVROVERLAYWINDOWCLASS"), skCrypt("amd dvr overlay")))
		{
			amd = true;
			DirectX9Interface::hijacked_hwnd = LI_FN(FindWindowA)(skCrypt("AMDDVROVERLAYWINDOWCLASS"), skCrypt("amd dvr overlay"));
			NOTIFYICONDATA nid = {};
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = NULL;
			nid.uID = 1;
			nid.uFlags = NIF_INFO;
			nid.dwInfoFlags = NIIF_INFO;
			strcpy_s(nid.szInfoTitle, xorstr("External Notification").c_str());
			strcpy_s(nid.szInfo, xorstr("Found Radeon AMD Overlay").c_str());
			Shell_NotifyIcon(NIM_ADD, &nid);
			Sleep(5000);
			Shell_NotifyIcon(NIM_DELETE, &nid);
		}
		else
		{
			NOTIFYICONDATA nid = {};
			nid.cbSize = sizeof(NOTIFYICONDATA);
			nid.hWnd = NULL;
			nid.uID = 1;
			nid.uFlags = NIF_INFO;
			nid.dwInfoFlags = NIIF_INFO;
			strcpy_s(nid.szInfoTitle, xorstr("External Notification").c_str());
			strcpy_s(nid.szInfo, xorstr("NVIDIA/AMD not found!").c_str());
			Shell_NotifyIcon(NIM_ADD, &nid);
			Sleep(8000);
			Shell_NotifyIcon(NIM_DELETE, &nid);
			exit(1);
		}

		LI_FN(ShowWindow)(DirectX9Interface::hijacked_hwnd, SW_SHOW);

		LI_FN(SetWindowLongA)(DirectX9Interface::hijacked_hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		LI_FN(SetWindowLongA)(
			DirectX9Interface::hijacked_hwnd,
			-20,
			static_cast<LONG_PTR>(
				static_cast<int>(LI_FN(GetWindowLongA)(DirectX9Interface::hijacked_hwnd, -20)) | 0x20
				)
		);

		if (amd) LI_FN(MoveWindow)(DirectX9Interface::hijacked_hwnd, 0, 0, Width, Height, false); // only when amd

		//transparency
		MARGINS margin = { -1, -1, -1, -1 };
		LI_FN(DwmExtendFrameIntoClientArea)(
			DirectX9Interface::hijacked_hwnd,
			&margin
		);

		LI_FN(SetLayeredWindowAttributes)(
			DirectX9Interface::hijacked_hwnd,
			NULL,
			0xFF,
			0x02
		);

		// top most
		LI_FN(SetWindowPos)(
			DirectX9Interface::hijacked_hwnd,
			HWND_TOPMOST,
			0, 0, 0, 0,
			0x0002 | 0x0001
		);


		LI_FN(UpdateWindow)(DirectX9Interface::hijacked_hwnd);

	}

}

__forceinline void draw_rect(int x, int y, int w, int h, ImColor color, int thickness) {
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 3.f, 15, thickness);
}

__forceinline void draw_rect_filled(int x, int y, int w, int h, ImColor color, int thickness) {
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 3.f, 15);
}
 // example 
__forceinline void box_esp(float OffsetW, float OffsetH, int X, int Y, int W, int H, ImU32 Color, int thickness, bool filled, bool outlined) {
	if (filled) {
		ImU32 sdfg = ImColor(0, 0, 0, 100);
		draw_rect_filled(X, Y, W, H, sdfg, thickness);
	}

	if (outlined) {
		draw_rect(X, Y, W, H, ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), thickness + 1);
	}

	draw_rect(X, Y, W, H, Color, thickness);
}
