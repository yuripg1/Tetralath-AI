from enum import Enum
import pygame
import pygame_menu

import definitions


TETRALATH_WINDOW_WIDTH = 1224
TETRALATH_WINDOW_HEIGHT = 691
TETRALATH_WINDOW_TITLE = "Tetralath"
TETRALATH_BACKGROUND_COLOR = (255, 255, 255)
TETRALATH_LEFT_PANEL_WIDTH = 300
TETRALATH_LEFT_PANEL_HEIGHT = 691
TETRALATH_LEFT_PANEL_TITLE = "Tetralath"


def initialize_game_ui() -> tuple[pygame.Surface, pygame.time.Clock]:
    pygame.init()
    game_window = pygame.display.set_mode(
        (TETRALATH_WINDOW_WIDTH, TETRALATH_WINDOW_HEIGHT)
    )
    pygame.display.set_caption(TETRALATH_WINDOW_TITLE)
    clock = pygame.time.Clock()
    return game_window, clock


def refresh_game_ui(
    game_window: pygame.Surface, clock: pygame.time.Clock, menu: pygame_menu.Menu
) -> None:
    game_window.fill(TETRALATH_BACKGROUND_COLOR)
    menu.draw(game_window)
    pygame.display.flip()
    clock.tick(10)


def get_events(pending_tetralath_events: list[definitions.TetralathEvent]) -> tuple[definitions.TetralathEvent | None, list[pygame.event.Event]]:
    tetralath_event: definitions.TetralathEvent | None = None
    pygame_events = pygame.event.get()
    for event in pygame_events:
        if event.type == pygame.QUIT:
            pending_tetralath_events.append(definitions.TetralathEvent.QUIT)
    if len(pending_tetralath_events) > 0:
        tetralath_event = pending_tetralath_events.pop(0)
    return tetralath_event, pygame_events


def draw_left_panel(game: definitions.TetralathGame, pending_tetralath_events: list[definitions.TetralathEvent]) -> tuple[pygame_menu.Menu, pygame_menu.widgets.Selector, pygame_menu.widgets.Selector, pygame_menu.widgets.Button]:
    theme = pygame_menu.themes.THEME_DEFAULT.copy()
    menu = pygame_menu.Menu(
        width=TETRALATH_LEFT_PANEL_WIDTH,
        height=TETRALATH_LEFT_PANEL_HEIGHT,
        title=TETRALATH_LEFT_PANEL_TITLE,
        theme=theme,
        position=(0, 0, False),
        screen_dimension=(TETRALATH_WINDOW_WIDTH, TETRALATH_WINDOW_HEIGHT),
    )

    def on_ai_mode_change(selected: tuple, index: int) -> None:
        game["ai_mode"] = selected[0][1]

    ai_mode_options = [
        ("Ruthless", definitions.TetralathAIMode.RUTHLESS),
        ("Friendly", definitions.TetralathAIMode.FRIENDLY),
    ]
    ai_mode_default_index = 0
    game["ai_mode"] = ai_mode_options[ai_mode_default_index][1]
    ai_mode_selector = menu.add.selector(
        "AI mode",
        ai_mode_options,
        default=ai_mode_default_index,
        onchange=on_ai_mode_change,
    )

    def on_player_color_change(selected: tuple, index: int) -> None:
        game["player_color"] = selected[0][1]

    player_color_options = [
        ("White", definitions.TetralathColor.WHITE),
        ("Black", definitions.TetralathColor.BLACK),
    ]
    player_color_default_index = 0
    game["player_color"] = player_color_options[player_color_default_index][1]
    player_color_selector = menu.add.selector(
        "Player color",
        player_color_options,
        default=player_color_default_index,
        onchange=on_player_color_change,
    )

    def on_start_game() -> None:
        pending_tetralath_events.append(definitions.TetralathEvent.START_GAME)

    start_game_button = menu.add.button("Start game", on_start_game)

    return menu, ai_mode_selector, player_color_selector, start_game_button


def update_left_panel(menu: pygame_menu.Menu, events: list[pygame.event.Event]) -> None:
    menu.update(events)


def disable_left_panel(ai_mode_selector: pygame_menu.widgets.Selector, player_color_selector: pygame_menu.widgets.Selector, start_game_button: pygame_menu.widgets.Button) -> None:
    ai_mode_selector.readonly = True
    player_color_selector.readonly = True
    start_game_button.readonly = True

def destroy_game_ui() -> None:
    pygame.quit()
