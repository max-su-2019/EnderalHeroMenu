#include "Hook.h"
#include "HeroMenu.h"

namespace enderal
{
	EventResult MenuControlsHook::ProcessEvent(RE::MenuControls* a_menuControls, RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
	{
		using EventType = RE::INPUT_EVENT_TYPE;
		using DeviceType = RE::INPUT_DEVICE;
		using Flag = RE::IMenu::Flag;
		using Context = RE::IMenu::Context;
		using INPUT_ID = RE::UserEvents::INPUT_CONTEXT_ID;

		const auto ui = RE::UI::GetSingleton();
		auto userEventHolder = RE::UserEvents::GetSingleton();
		auto heroMenu = ui->GetMenu<RE::TweenMenu>(HeroMenu::menu_name);

		auto IsTopMostInteractionMenu = [](const RE::UI* ui, const RE::GPtr<RE::IMenu>& iMenu) -> bool {
			for (auto iter = ui->menuStack.end() - 1; iter >= ui->menuStack.begin(); iter--) {
				auto menu = iter->get();
				if (menu && (menu->inputContext.get() < Context::kTotal || menu->PausesGame() || menu->UsesCursor())) {
					return menu == iMenu.get();
				}
			}

			return false;
		};

		if (!a_menuControls->remapMode && !a_menuControls->isProcessing && heroMenu && heroMenu->OnStack() && IsTopMostInteractionMenu(ui, heroMenu)) {
			for (auto event = *a_event; event; event = event->next) {
				if (event->eventType != EventType::kButton)
					continue;

				const auto button = event->AsButtonEvent();
				if (!button || !button->IsDown())
					continue;

				RE::BSFixedString menuCloseEventList[] = {
					userEventHolder->cancel,
					userEventHolder->pause,
					userEventHolder->back,
					userEventHolder->journal,
					userEventHolder->tweenMenu
				};

				for (const auto& closeEvent : menuCloseEventList) {
					if (strcmpi(button->QUserEvent().c_str(), closeEvent.c_str()) == 0) {
						HeroMenu::close();
						return EventResult::kStop;
					}
				}
			}
		}

		return _ProcessEvent(a_menuControls, a_event, a_eventSource);
	}

	void TweenMenuOpenSubHook::OpenSubMenu(RE::IMenu* tweenMenu, std::int32_t index)
	{
		if (index == 1) {
			RE::TweenMenu::CloseTweenMenu();
			HeroMenu::open();
		} else
			_OpenSubMenu(tweenMenu, index);
	}

	void OpenQuickStatsMenuHook::OpenQuickStatsMenu(RE::UIMessageQueue* queue, const RE::BSFixedString& a_menuName, RE::UI_MESSAGE_TYPE a_type, RE::IUIMessageData* a_data)
	{
		return _OpenQuickStatsMenu(queue, HeroMenu::menu_name, a_type, a_data);
	}

}