import definitions
import ui


def graphical_game() -> None:
    game: definitions.TetralathGame = {
        "is_started": False,
        "player_color": definitions.TetralathColor.NONE,
        "ai_mode": definitions.TetralathAIMode.NONE,
    }
    game_window, clock = ui.initialize_game_ui()
    pending_tetralath_events: list[definitions.TetralathEvent] = []
    left_panel, ai_mode_selector, player_color_selector, start_game_button = ui.draw_left_panel(game, pending_tetralath_events)
    running = True
    while running:
        tetralath_event, pygame_events = ui.get_events(pending_tetralath_events)
        ui.update_left_panel(left_panel, pygame_events)
        if tetralath_event is not None:
            if tetralath_event == definitions.TetralathEvent.QUIT:
                if running is not False:
                    running = False
            elif tetralath_event == definitions.TetralathEvent.START_GAME:
                if game["is_started"] is not True:
                    game["is_started"] = True
                    ui.disable_left_panel(ai_mode_selector, player_color_selector, start_game_button)
                running = True
        ui.refresh_game_ui(game_window, clock, left_panel)
        print(game)
    if running is False:
        ui.destroy_game_ui()


def main() -> None:
    graphical_game()


if __name__ == "__main__":
    main()
