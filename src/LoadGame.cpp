#include "HeroMenu.h"
#include "Hook.h"

namespace enderal
{
	void EventCallback(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kPostLoad) {
			static constexpr auto enderalDLLName = "EnderalSE.dll";
			auto enderalPlugin = GetModuleHandleA(enderalDLLName);
			if (enderalPlugin) {
				enderal::HeroMenu::Register();
				enderal::TweenMenuOpenSubHook::InstallHook();
				enderal::OpenQuickStatsMenuHook::InstallHook();
				enderal::StatsMenuBlackScreenHook::InstallHook();
				enderal::MenuControlsHook::InstallHook();
			} else {
				ERROR("Not \"{}\" Loaded!", enderalDLLName);
			}
		}
	}
}