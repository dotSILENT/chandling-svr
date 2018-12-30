
namespace Hooks
{
	void PreHooking();
	bool InstallHooks();
	void InstallNativeRedirects(void* pAMXFunctions);
	void UninstallHooks();
}