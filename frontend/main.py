import definitions
import ui


def graphical_game() -> None:
    game: definitions.TetralathGame = {
        "is_started": False,
        "player_color": definitions.TetralathColor.NONE,
        "ai_mode": definitions.TetralathAIMode.NONE,
    }
    game_window, clock = ui.initialize_game_ui()
    pending_tetralath_ui_events: list[definitions.TetralathUIEvent] = []
    left_panel, ai_mode_selector, player_color_selector, start_game_button = (
        ui.draw_left_panel(game, pending_tetralath_ui_events)
    )
    running = True
    while running:
        tetralath_ui_event, pygame_events = ui.get_events(pending_tetralath_ui_events)
        ui.update_left_panel(left_panel, pygame_events)
        if tetralath_ui_event is not None:
            if tetralath_ui_event["type"] == definitions.TetralathEventType.QUIT:
                if running is not False:
                    running = False
            elif (
                tetralath_ui_event["type"] == definitions.TetralathEventType.START_GAME
            ):
                if game["is_started"] is not True:
                    game["is_started"] = True
                    ui.disable_left_panel(
                        left_panel,
                        ai_mode_selector,
                        player_color_selector,
                        start_game_button,
                    )
                running = True
            elif (
                tetralath_ui_event["type"]
                == definitions.TetralathEventType.BOARD_POSITION_CLICKED
            ):
                board_position_index = tetralath_ui_event["board_position_index"]
        ui.refresh_game_ui(game_window, clock, left_panel)
    if running is False:
        ui.destroy_game_ui()


def main() -> None:
    graphical_game()


if __name__ == "__main__":
    main()
