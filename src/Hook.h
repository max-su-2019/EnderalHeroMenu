#pragma once

namespace enderal
{

	using EventResult = RE::BSEventNotifyControl;

	// Hook to trigger hero menu close
	class MenuControlsHook
	{
	public:
		static void InstallHook()
		{
			REL::Relocation<std::uintptr_t> MenuControlsVtbl{ RE::VTABLE_MenuControls[0] };
			_ProcessEvent = MenuControlsVtbl.write_vfunc(0x1, ProcessEvent);
		}

	private:
		static EventResult ProcessEvent(RE::MenuControls* a_menuControls, RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource);

		static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;
	};

	// Hook to trigger hero menu open
	class TweenMenuOpenSubHook
	{
	public:
		static void InstallHook()
		{
			REL::Relocation<uintptr_t> hook{ REL::ID(51836) };  //1408D0F80
			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(1 << 4);
			_OpenSubMenu = trampoline.write_branch<5>(hook.address() + 0xE, OpenSubMenu);
		};

	private:
		static void OpenSubMenu(RE::IMenu* tweenMenu, std::int32_t index);

		static inline REL::Relocation<decltype(OpenSubMenu)> _OpenSubMenu;
	};

	class OpenQuickStatsMenuHook
	{
	public:
		static void InstallHook()
		{
			REL::Relocation<uintptr_t> hook{ REL::ID(39346) };  //14069ABF0
			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(1 << 4);
			_OpenQuickStatsMenu = trampoline.write_branch<5>(hook.address() + 0x6a, OpenQuickStatsMenu);
		};

	private:
		static void OpenQuickStatsMenu(RE::UIMessageQueue* queue, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data);

		static inline REL::Relocation<decltype(OpenQuickStatsMenu)> _OpenQuickStatsMenu;
	};
}