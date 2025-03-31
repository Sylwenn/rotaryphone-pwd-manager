#include "includes.h"

// Anonymous namespace for internal linkage
namespace {
	// Direct3D device and context pointers
	ID3D11Device* g_pd3d_device = nullptr;
	ID3D11DeviceContext* g_pd3d_device_context = nullptr;
	IDXGISwapChain* g_p_swap_chain = nullptr;
	bool g_swap_chain_occluded = false;
	UINT g_resize_width = 0, g_resize_height = 0;
	ID3D11RenderTargetView* g_main_render_target_view = nullptr;
	int selected_index = -1;
	std::string password;
	std::string new_password;
	const char* c_password = nullptr; // Pointer to c_str of generated password
	const char* c_new_password = nullptr;
	int password_length = 12; // Default password length
	auto input_password = std::make_unique<char[]>(64); // Use smart pointer for memory management

	// Handle website click event
	string handle_website_click(const int index, std::vector<std::string> passwordelements) {
		selected_index = index;
		string decryptedpassword = decrypt_password(index);
		std::cout << "Website clicked at index: " << index << ", Name: " << passwordelements[index] << "\n";
		std::cout << "Decrypted password: " << decryptedpassword << "\n";
		return decryptedpassword;
	}
}

// Forward declarations of helper functions
static LRESULT WINAPI wnd_proc(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param);  // NOLINT(misc-use-anonymous-namespace)
static bool create_device_d_3d(HWND h_wnd);  // NOLINT(misc-use-anonymous-namespace)
static void cleanup_device_d_3d(); // NOLINT(misc-use-anonymous-namespace)
static void create_render_target(); // NOLINT(misc-use-anonymous-namespace)
static void cleanup_render_target(); // NOLINT(misc-use-anonymous-namespace)

int main() {
	init_checks();
	std::vector<std::string> keys = load_keys_to_memory();
	std::vector<std::string> passwords = load_passwords_to_memory();

	// Create application window
	const WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, wnd_proc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Rotary Password Manager", nullptr };
	RegisterClassExW(&wc);
	const HWND hwnd = ::CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST, _T("Rotary Password Manager"), nullptr, WS_POPUP, 0, 0, 1920, 1080, nullptr, nullptr, wc.hInstance, nullptr);
	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	// Initialize Direct3D
	if (!create_device_d_3d(hwnd)) {
		cleanup_device_d_3d();
		UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable GamePad Controls
	io.WantCaptureKeyboard = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3d_device, g_pd3d_device_context);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).

	// Menu state
	bool show_main_window = true;

	// Main loop
	bool done = false;
	while (!done) {
		// Poll and handle messages (inputs, window resize, etc.)
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		// Handle window being minimized or screen locked
		if (g_swap_chain_occluded && g_p_swap_chain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
			Sleep(10);
			continue;
		}
		g_swap_chain_occluded = false;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_resize_width != 0 && g_resize_height != 0) {
			cleanup_render_target();
			if (const HRESULT hr = g_p_swap_chain->ResizeBuffers(0, g_resize_width, g_resize_height, DXGI_FORMAT_UNKNOWN, 0); FAILED(hr)) {
				std::cerr << "Failed to resize buffers. HRESULT: " << hr << "\n";
			}
			g_resize_width = g_resize_height = 0;
			create_render_target();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// Main window
		if (show_main_window) {
			ImGui::SetNextWindowSize(ImVec2(815, 0));
			ImGui::Begin("Rotary Password Manager", &show_main_window, ImGuiWindowFlags_NoCollapse);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::Separator();
			ImGui::Columns(1);
			if (ImGui::BeginTabBar("#tabs")) {
				if (ImGui::BeginTabItem("Passwords")) {
					center_next_im_gui();
					ImGui::BeginChild("PasswordListMain", ImVec2(800, 400), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					ImGui::Columns(2, "Password Columns", false);
					ImGui::SetColumnOffset(1, 210);
					ImGui::BeginChild("PasswordListLeft", ImVec2(200, 350), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					static int item_current3 = 0;
					std::vector<std::string> passwordelements = load_password_elements_to_memory();
					std::vector<const char*> items;
					items.reserve(passwordelements.size());
					for (const std::string& str : passwordelements) {
						items.push_back(str.c_str());
					}
					ImGui::PushItemWidth(150);
					if (ImGui::ListBox("##", &item_current3, items.data(), static_cast<int>(items.size()), 10)) {
						handle_website_click(item_current3, passwordelements);
					}
					ImGui::EndChild();
					ImGui::NextColumn();
					ImGui::BeginChild("PasswordListRight", ImVec2(400, 200), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					if (selected_index >= 0) {
						ImGui::Text("Password for %s:", passwordelements[selected_index].c_str());
						ImGui::Text("%s", decrypt_password(selected_index).c_str());
						if (ImGui::Button("Delete Password")) {
							delete_password("passwordelements.txt", selected_index);
							delete_password("passwords.txt", selected_index);
						}
					}
					else {
						ImGui::Text("Select a website to see the password.");
					}
					static char website_input[128] = "";   // Input buffer for the new website
					static char password_input[128] = "";  // Input buffer for the new password

					ImGui::InputText("URL", website_input, IM_ARRAYSIZE(website_input));
					ImGui::InputText("PASS", password_input, IM_ARRAYSIZE(password_input));
					if (ImGui::Button("Add Password")) {
						if (strlen(website_input) == 0 || strlen(password_input) == 0) {
							printf("Error: Website or Password is empty!\n");
						}
						else {
							add_password(website_input, password_input);
							printf("Added: %s -> %s\n", website_input, password_input);
							website_input[0] = '\0';
							password_input[0] = '\0';
						}
					}
					ImGui::EndChild();
					ImGui::EndChild();
					ImGui::Columns(1);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Generator")) {
					password_length = 12;
					password_length = std::clamp(password_length, 0, 64);

					ImGui::Text("Password Generator");
					ImGui::InputInt("Length", &password_length);

					if (ImGui::Button("Generate")) {
						password_generator generator;
						password = generator.generate_password(password_length);
						c_password = password.c_str();
					}

					if (!password.empty()) {
						ImGui::Text("%s", c_password);
						if (ImGui::Button("Copy to Clipboard")) {
							ImGui::SetClipboardText(c_password);
						}
					}
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Settings")) {
					center_next_im_gui();
					ImGui::BeginChild("SettingsMain", ImVec2(800, 400), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					ImGui::Text("TODO");
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Password Strengthener")) {
					center_next_im_gui();
					ImGui::BeginChild("SettingsMain", ImVec2(800, 400), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
					ImGui::Text("Password Strengthener");

					ImGui::InputText("Password", input_password.get(), 64 * sizeof(char));
					if (ImGui::Button("Strengthen")) {
						new_password = std::string(input_password.get());
						strengthen_password(new_password);
						c_new_password = new_password.c_str();
						std::cout << new_password << "\n";
					}
					if (c_new_password != nullptr) {
						ImGui::Text("%s", c_new_password);
					}
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		else {
			PostQuitMessage(0);
		}
		ImGui::End();

		// Rendering
		ImGui::Render();
		constexpr float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		g_pd3d_device_context->OMSetRenderTargets(1, &g_main_render_target_view, nullptr);
		g_pd3d_device_context->ClearRenderTargetView(g_main_render_target_view, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present
		const HRESULT hr = g_p_swap_chain->Present(1, 0);   // Present with vsync
		g_swap_chain_occluded = hr == DXGI_STATUS_OCCLUDED;
	}

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device_d_3d();
	DestroyWindow(hwnd);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Create Direct3D device and swap chain
bool create_device_d_3d(const HWND h_wnd) {
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
	sd.OutputWindow = h_wnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	constexpr UINT create_device_flags = 0;
	D3D_FEATURE_LEVEL feature_level;
	constexpr D3D_FEATURE_LEVEL feature_level_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain, &g_pd3d_device, &feature_level, &g_pd3d_device_context);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, create_device_flags, feature_level_array, 2, D3D11_SDK_VERSION, &sd, &g_p_swap_chain, &g_pd3d_device, &feature_level, &g_pd3d_device_context);
	if (res != S_OK)
		return false;

	create_render_target();
	return true;
}

// Cleanup Direct3D device and swap chain
void cleanup_device_d_3d() {
	cleanup_render_target();
	if (g_p_swap_chain) { g_p_swap_chain->Release(); g_p_swap_chain = nullptr; }
	if (g_pd3d_device_context) { g_pd3d_device_context->Release(); g_pd3d_device_context = nullptr; }
	if (g_pd3d_device) { g_pd3d_device->Release(); g_pd3d_device = nullptr; }
}

// Create render target view
void create_render_target() {
	ID3D11Texture2D* p_back_buffer = nullptr;
	// Attempt to get the back buffer from the swap chain
	if (const HRESULT hr = g_p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&p_back_buffer)); FAILED(hr)) {
		std::cerr << "Failed to get buffer from swap chain. HRESULT: " << hr << "\n";
		return;
	}
	if (g_pd3d_device == nullptr) {
		std::cerr << "g_pd3d_device is not initialized.\n";
		p_back_buffer->Release(); // Ensure proper cleanup
		return;
	}
	// Release the previous render target view if it exists to avoid memory leaks
	if (g_main_render_target_view) {
		g_main_render_target_view->Release();
		g_main_render_target_view = nullptr;
	}
	// Create the render target view
	if (const HRESULT hr = g_pd3d_device->CreateRenderTargetView(p_back_buffer, nullptr, &g_main_render_target_view); FAILED(hr)) {
		std::cerr << "Failed to create render target view. HRESULT: " << hr << "\n";
		p_back_buffer->Release(); // Ensure proper cleanup
		return;
	}
	// Release the back buffer after creating the render target view
	p_back_buffer->Release();
}

// Cleanup render target view
void cleanup_render_target() {
	if (g_main_render_target_view) { g_main_render_target_view->Release(); g_main_render_target_view = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT im_gui_impl_win32_wnd_proc_handler(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param);

// Win32 message handler
LRESULT WINAPI wnd_proc(const HWND h_wnd, const UINT msg, const WPARAM w_param, const LPARAM l_param) {
	if (im_gui_impl_win32_wnd_proc_handler(h_wnd, msg, w_param, l_param))
		return true;

	switch (msg) {
	case WM_SIZE:
		if (w_param == SIZE_MINIMIZED)
			return 0;
		g_resize_width = static_cast<UINT>(LOWORD(l_param)); // Queue resize
		g_resize_height = static_cast<UINT>(HIWORD(l_param));
		return 0;
	case WM_SYSCOMMAND:
		if ((w_param & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProcW(h_wnd, msg, w_param, l_param);
}
