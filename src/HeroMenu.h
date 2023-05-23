#pragma once

namespace enderal
{
	class HeroMenu final : public RE::IMenu
	{
	public:
		static constexpr std::string_view menu_name = "00E_heromenu";
		static constexpr std::string_view file_name = menu_name;
		static RE::stl::owner<RE::IMenu*> creator() { return new HeroMenu(); }

		static void Register()
		{
			RE::UI::GetSingleton()->Register(menu_name, creator);
			INFO("Registered {}", menu_name);
		}

		static void open()
		{
			if (!is_menu_open()) {
				DEBUG("Open Menu {}", menu_name);
				RE::UIMessageQueue::GetSingleton()->AddMessage(menu_name, RE::UI_MESSAGE_TYPE::kShow, nullptr);
			}
		}

		static void close()
		{
			if (is_menu_open()) {
				DEBUG("Close Menu {}", menu_name);
				RE::UIMessageQueue::GetSingleton()->AddMessage(menu_name, RE::UI_MESSAGE_TYPE::kHide, nullptr);
			}
		}

		static bool is_menu_open()
		{
			auto is_open = RE::UI::GetSingleton()->IsMenuOpen(menu_name);
			if (is_open) {
				DEBUG("Menu {} is open {}", menu_name, is_open);
			}
			return is_open;
		}

		HeroMenu(const HeroMenu&) = delete;
		HeroMenu(HeroMenu&&) = delete;

		HeroMenu& operator=(const HeroMenu&) = delete;
		HeroMenu& operator=(HeroMenu&&) = delete;

	protected:
		HeroMenu()
		{
			using context = RE::UserEvents::INPUT_CONTEXT_ID;
			using flag = RE::UI_MENU_FLAGS;

			const auto a_menu = static_cast<IMenu*>(this);
			const auto scaleform_manager = RE::BSScaleformManager::GetSingleton();
			[[maybe_unused]] const auto success = scaleform_manager->LoadMovieEx(a_menu,
				file_name,
				RE::BSScaleformManager::ScaleModeType::kExactFit,
				[&](RE::GFxMovieDef* a_def) -> void {
					fxDelegate.reset(new RE::FxDelegate);
					fxDelegate->RegisterHandler(this);
					a_def->SetState(RE::GFxState::StateType::kExternalInterface, fxDelegate.get());
					fxDelegate->Release();

					DEBUG("SWF FPS: {}, Width: {}, Height: {}",
						a_def->GetFrameRate(),
						a_def->GetWidth(),
						a_def->GetHeight());
					a_def->SetState(RE::GFxState::StateType::kLog, RE::make_gptr<Logger>().get());
				});
			DEBUG("Loading Menu {} was successful {}", file_name, success);
			assert(success);

			view_ = a_menu->uiMovie;
			a_menu->menuFlags.set(flag::kPausesGame,
				flag::kUsesCursor,
				flag::kUsesMenuContext,
				flag::kDisablePauseMenu,
				flag::kUpdateUsesCursor,
				flag::kTopmostRenderedMenu);

			a_menu->depthPriority = 5;
			a_menu->inputContext = context::kNone;

			is_active_ = true;
			view_->SetVisible(true);
		}

		void PostCreate() override { on_open(); }

		RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override
		{
			switch (*a_message.type) {
			case RE::UI_MESSAGE_TYPE::kHide:
				on_close();
				return RE::UI_MESSAGE_RESULTS::kHandled;
			default:
				return IMenu::ProcessMessage(a_message);
			}
		}

		void AdvanceMovie(const float a_interval, const uint32_t a_current_time) override
		{
			IMenu::AdvanceMovie(a_interval, a_current_time);
		}

		void Accept(CallbackProcessor* a_processor) override
		{
			a_processor->Process("CloseMenu", close_menu);
		}

	private:
		class Logger : public RE::GFxLog
		{
		public:
			void LogMessageVarg(LogMessageType, const char* a_fmt, const std::va_list a_arg_list) override
			{
				std::string fmt(a_fmt ? a_fmt : "");
				while (!fmt.empty() && fmt.back() == '\n') {
					fmt.pop_back();
				}

				std::va_list args;
				va_copy(args, a_arg_list);
				std::vector<char> buf(
					static_cast<std::size_t>(std::vsnprintf(nullptr, 0, fmt.c_str(), a_arg_list) + 1));
				std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
				va_end(args);

				INFO("{}: {}", menu_name, buf.data());
			}
		};

		void on_open()
		{
			RE::UIBlurManager::GetSingleton()->IncrementBlurCount();
		}

		void on_close()
		{
			RE::UIBlurManager::GetSingleton()->DecrementBlurCount();
		}

		static void close_menu([[maybe_unused]] const RE::FxDelegateArgs& a_params)
		{
			DEBUG("GUI Close Button Pressed");
			close();
		}

		RE::GPtr<RE::GFxMovieView> view_;
		bool is_active_ = false;
	};
}