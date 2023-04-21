#include <Windows.h>
#include <Dshow.h>

// pragmas for libraries needed
#pragma comment(lib, "ntdll.lib") 		// bsod stuff
#pragma comment(lib, "strmiids.lib") 	// most of directshow
#pragma comment(lib, "ole32.lib") 		// CoInitialize and CoCreateInstance
#pragma comment(lib, "user32.lib")		// ShowWindow

// externs for bsod stuff
extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrThread, PBOOLEAN StatusPointer);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG Unless1, ULONG Unless2, PULONG_PTR Unless3, ULONG ValidResponseOption, PULONG ResponsePointer);

// global variables for directshow
IGraphBuilder *graph = 0; 		// filter graph manager
IMediaControl *control = 0; 	// media control interface
IMediaEvent   *event = 0; 		// media event interface
IVideoWindow  *window = 0;		// the video window

// helpful functions
void initialize_directshow(LPCWSTR path) {
	CoInitialize(NULL); // initialize the COM

	// create the filter graph manager
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&graph);
	
	// get addition interfaces for it
	graph->QueryInterface(IID_IMediaControl, (void **)&control);
	graph->QueryInterface(IID_IMediaEvent, (void **)&event);
	graph->QueryInterface(IID_IVideoWindow, (void **)&window);

	// attempt to build the graph for file playback
	graph->RenderFile(path, NULL);

	// set the video window to fullscreen mode
	window->put_FullScreenMode(OATRUE);
}
// main code here
int main() {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	initialize_directshow(L"main.wmv");

	HRESULT result = control->Run();

	if (!SUCCEEDED(result)) {
		return 1;
	}
	long code = 0;
	event->WaitForCompletion(INFINITE, &code);

	return 0;
}