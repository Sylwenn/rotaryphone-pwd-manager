#include "includes.h"
//DATA
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
int selectedIndex = -1;
static std::string password;
static const char* output = nullptr;
static int passwordLength;
// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
string HandleWebsiteClick(int index, std::vector<std::string> passwordelements)
{
	selectedIndex = index;
	std::cout << "Website clicked at index: " << index << ", Name: " << passwordelements[index] << std::endl;
	string decryptedpassword = decryptPassword(index);
	std::cout << "Decrypted password: " << decryptedpassword << std::endl;
	return decryptedpassword;
}
void GetWindowSize(HWND hWnd, int& width, int& height)
{
	RECT rect;
	if (GetClientRect(hWnd, &rect)) {
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
}


int main()
{
	initChecks();
	std::vector<std::string> keys = loadKeysToMemory();
	std::vector<std::string> passwords = loadPasswordsToMemory();
	std::vector<std::string> passwordelements = loadPasswordElementsToMemory();

	// Create application window
   //ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Rotary Password Manager", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST, _T("Rotary Password Manager"), NULL, WS_POPUP, 0, 0, 1920, 1080, NULL, NULL, wc.hInstance, NULL);
	//HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Rotary Password Manager", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.WantCaptureKeyboard = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != nullptr);

	// menu state
	bool show_demo_window = true;
	bool show_main_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{

		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window being minimized or screen locked
		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			static float f = 0.0f;
			static int counter = 0;

			if (show_main_window) {
				ImGui::SetNextWindowSize(ImVec2(815, 0));
				ImGui::Begin("Rotary Password Manager", &show_main_window, ImGuiWindowFlags_NoCollapse);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::Separator();
				ImGui::Columns(1);
				if (ImGui::BeginTabBar("#tabs")) {
					{
						if (ImGui::BeginTabItem("Passwords")) {
							{
								centerNextImGui();
								ImGui::BeginChild("PasswordListMain", ImVec2(800, 400), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
								ImGui::Columns(2, "Password Collumns", false);
								ImGui::SetColumnOffset(1, 210);
								ImGui::BeginChild("PasswordListLeft", ImVec2(200, 350), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
								static int item_current3 = 0;
								std::vector<std::string> passwordelements = loadPasswordElementsToMemory();
								std::vector<const char*> items;
								for (const std::string& str : passwordelements) {
									items.push_back(str.c_str());
								}
								ImGui::PushItemWidth(150);
								if (ImGui::ListBox("##", &item_current3, items.data(), static_cast<int>(items.size()), 10)) {
									HandleWebsiteClick(item_current3, passwordelements);
								}
								ImGui::EndChild();
								ImGui::NextColumn();
								ImGui::BeginChild("PasswordListRight", ImVec2(400, 200), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
								if (selectedIndex >= 0) {
									ImGui::Text("Password for %s:", passwordelements[selectedIndex].c_str());
									ImGui::Text("%s", decryptPassword(selectedIndex).c_str());
									if (ImGui::Button("Delete Password")) {
										deletePassword("passwordelements.txt", selectedIndex);
										deletePassword("passwords.txt", selectedIndex);
									}
								}
								else {
									ImGui::Text("Select a website to see the password.");
								}
								static char websiteInput[128] = "";   // Input buffer for the new website
								static char passwordInput[128] = "";  // Input buffer for the new password

								ImGui::InputText("URL", websiteInput, IM_ARRAYSIZE(websiteInput));
								ImGui::InputText("PASS", passwordInput, IM_ARRAYSIZE(passwordInput));
								if (ImGui::Button("Add Password")) {
									if (strlen(websiteInput) == 0 || strlen(passwordInput) == 0) {
										printf("Error: Website or Password is empty!\n");
									}
									else {
										addPassword(websiteInput, passwordInput);
										printf("Added: %s -> %s\n", websiteInput, passwordInput);
										websiteInput[0] = '\0';
										passwordInput[0] = '\0';
									}
								}
							}
							ImGui::EndChild();
							ImGui::EndChild();
							ImGui::Columns(1);
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Generator"))
						{
							PasswordGenerator generator;
							passwordLength = 12;

							ImGui::Text("Password Generator");
							ImGui::InputInt("Length", &passwordLength);

							if (ImGui::Button("Generate")) {
								password = generator.generate_password(passwordLength);
								output = password.c_str();
								std::cout << "Generated: " << output << std::endl;
							}

							if (password.empty() != true) {
								ImGui::Text(output);
								if (ImGui::Button("Copy to Clipboard")) {
									ImGui::SetClipboardText(output);
								}
							}
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Settings"))
						{
							centerNextImGui();
							ImGui::BeginChild("SettingsMain", ImVec2(800, 400), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
							ImGui::Text("TODO");
							ImGui::EndChild();
							ImGui::EndTabItem();
						}
						if (ImGui::BeginTabItem("Password Strenghtener"))
						{
							
							centerNextImGui();
							ImGui::BeginChild("SettingsMain", ImVec2(800, 400), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
							ImGui::Text("Password Strenghtener");
							
							ImGui::Text("TODO");
							ImGui::EndChild();
							ImGui::EndTabItem();
						}
					}
					ImGui::EndTabBar();
				}
			}
			else
			{
				PostQuitMessage(0);
			}
			ImGui::End();

		}
		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present
		HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
		//HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}




bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
