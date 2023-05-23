#pragma once
#include "DKUtil/Hook.hpp"

namespace enderal
{
	using namespace DKUtil::Hook;

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
			REL::Relocation<uintptr_t> hook{ REL::ID(51400) };  //1408AA140
			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(1 << 4);
			_OpenQuickStatsMenu = trampoline.write_call<5>(hook.address() + 0x483, OpenQuickStatsMenu);
		};

	private:
		static void OpenQuickStatsMenu(RE::UIMessageQueue* queue, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data);

		static inline REL::Relocation<decltype(OpenQuickStatsMenu)> _OpenQuickStatsMenu;
	};

	class StatsMenuBlackScreenHook
	{
	public:
		static void InstallHook()
		{
			REL::Relocation<uintptr_t> hook{ REL::ID(51400) };  //1408AA140
			auto& trampoline = SKSE::GetTrampoline();
			SKSE::AllocTrampoline(1 << 4);
			trampoline.write_call<5>(hook.address() + 0x436, ProcessMenuBlackScreen);
		};

	private:
		static std::int32_t ProcessMenuBlackScreen(char a1, char a2, std::int32_t a3, char a4, std::int32_t a5)
		{
			return 0;
		}
	};

}