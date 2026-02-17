import pygame
import pygame_menu

import board
import definitions


TETRALATH_FPS = 25
TETRALATH_WINDOW_WIDTH = 1224
TETRALATH_WINDOW_HEIGHT = 691
TETRALATH_WINDOW_TITLE = "Tetralath"
TETRALATH_BACKGROUND_COLOR = (255, 255, 255)
TETRALATH_LEFT_PANEL_WIDTH = 266
TETRALATH_LEFT_PANEL_HEIGHT = 691
TETRALATH_LEFT_PANEL_TITLE = "Tetralath"
TETRALATH_LEFT_PANEL_BACKGROUND_COLOR = (224, 224, 224)
TETRALATH_LEFT_PANEL_READONLY_FONT_COLOR = (128, 128, 128)
TETRALATH_LEFT_PANEL_TITLE_BACKGROUND_COLOR = (32, 32, 32)
TETRALATH_LEFT_PANEL_TITLE_FONT_COLOR = (255, 255, 255)
TETRALATH_LEFT_PANEL_WIDGET_BACKGROUND_COLOR = (240, 240, 240)
TETRALATH_LEFT_PANEL_WIDGET_BORDER_COLOR = (192, 192, 192)
TETRALATH_LEFT_PANEL_WIDGET_BORDER_WIDTH = 2
TETRALATH_LEFT_PANEL_WIDGET_FONT_COLOR = (0, 0, 0)
TETRALATH_LEFT_PANEL_POSITION = (0, 0, False)
TETRALATH_RIGHT_PANEL_WIDTH = 266
TETRALATH_RIGHT_PANEL_HEIGHT = 691
TETRALATH_RIGHT_PANEL_BACKGROUND_COLOR = (224, 224, 224)
TETRALATH_RIGHT_PANEL_READONLY_FONT_COLOR = (128, 128, 128)
TETRALATH_RIGHT_PANEL_WIDGET_BACKGROUND_COLOR = (240, 240, 240)
TETRALATH_RIGHT_PANEL_WIDGET_BORDER_COLOR = (192, 192, 192)
TETRALATH_RIGHT_PANEL_WIDGET_BORDER_WIDTH = 2
TETRALATH_RIGHT_PANEL_WIDGET_FONT_COLOR = (0, 0, 0)
TETRALATH_RIGHT_PANEL_POSITION = (958, 0, False)
TETRALATH_BOARD_HEXAGON_RADIUS = 43
TETRALATH_BOARD_PIECE_RADIUS = 29
TETRALATH_BOARD_PIECE_COLORS = {
    definitions.TetralathColor.WHITE: (255, 255, 255),
    definitions.TetralathColor.BLACK: (0, 0, 0),
}


def initialize_game_ui() -> tuple[pygame.surface.Surface, pygame.time.Clock]:
    pygame.init()
    game_window = pygame.display.set_mode((TETRALATH_WINDOW_WIDTH, TETRALATH_WINDOW_HEIGHT))
    pygame.display.set_caption(TETRALATH_WINDOW_TITLE)
    clock = pygame.time.Clock()
    return game_window, clock


def refresh_game_ui(game_window: pygame.surface.Surface, clock: pygame.time.Clock, menus: list[pygame_menu.Menu], game_board: list[definitions.TetralathColor]) -> None:
    game_window.fill(TETRALATH_BACKGROUND_COLOR)
    draw_board(game_window, game_board)
    for menu in menus:
        menu.draw(game_window)
    pygame.display.flip()
    clock.tick(TETRALATH_FPS)


def get_events(pending_tetralath_ui_events: list[definitions.TetralathUIEvent]) -> tuple[definitions.TetralathUIEvent | None, list[pygame.event.Event]]:
    tetralath_event: definitions.TetralathUIEvent | None = None
    pygame_events = pygame.event.get()
    for event in pygame_events:
        if event.type == pygame.QUIT:
            ui_event: definitions.TetralathUIEvent = {
                "type": definitions.TetralathEventType.QUIT,
            }
            pending_tetralath_ui_events.append(ui_event)
        elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            click_x = event.pos[0]
            click_y = event.pos[1]
            board_position_index = board.click_in_an_hexagon(
                click_x,
                click_y,
                board.TETRALATH_BOARD_NUMBER_OF_HEXAGONS,
                board.TETRALATH_BOARD_ROW_LENGTHS,
                board.TETRALATH_BOARD_MIDDLE_ROW_INDEX,
                TETRALATH_BOARD_HEXAGON_RADIUS,
                round(TETRALATH_WINDOW_WIDTH / 2),
                round(TETRALATH_WINDOW_HEIGHT / 2),
            )
            if board_position_index is not None:
                ui_event: definitions.TetralathUIEvent = {
                    "type": definitions.TetralathEventType.BOARD_POSITION_CLICKED,
                    "board_position_index": board_position_index,
                }
                pending_tetralath_ui_events.append(ui_event)
    if len(pending_tetralath_ui_events) >= 1:
        tetralath_event = pending_tetralath_ui_events.pop(0)
    return tetralath_event, pygame_events


def draw_left_panel(game: definitions.TetralathGame, pending_tetralath_ui_events: list[definitions.TetralathUIEvent]) -> tuple[pygame_menu.Menu, pygame_menu.widgets.Selector, pygame_menu.widgets.Selector, pygame_menu.widgets.Button]:
    theme = pygame_menu.themes.THEME_DEFAULT.copy()
    theme.border_width = 0
    theme.widget_border_width = 0
    theme.widget_border_color = TETRALATH_LEFT_PANEL_WIDGET_BORDER_COLOR
    theme.background_color = TETRALATH_LEFT_PANEL_BACKGROUND_COLOR
    theme.widget_background_color = TETRALATH_LEFT_PANEL_BACKGROUND_COLOR
    theme.title_background_color = TETRALATH_LEFT_PANEL_TITLE_BACKGROUND_COLOR
    theme.widget_font_color = TETRALATH_LEFT_PANEL_WIDGET_FONT_COLOR
    theme.title_font_color = TETRALATH_LEFT_PANEL_TITLE_FONT_COLOR
    theme.readonly_color = TETRALATH_LEFT_PANEL_READONLY_FONT_COLOR
    theme.readonly_selected_color = TETRALATH_LEFT_PANEL_READONLY_FONT_COLOR
    theme.title_bar_style = pygame_menu.widgets.MENUBAR_STYLE_TITLE_ONLY_DIAGONAL
    theme.widget_selection_effect = pygame_menu.widgets.NoneSelection()
    menu = pygame_menu.Menu(
        width=TETRALATH_LEFT_PANEL_WIDTH,
        height=TETRALATH_LEFT_PANEL_HEIGHT,
        title=TETRALATH_LEFT_PANEL_TITLE,
        theme=theme,
        position=TETRALATH_LEFT_PANEL_POSITION,
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
    menu.add.label("AI mode")
    ai_mode_selector = menu.add.selector(
        "",
        ai_mode_options,
        default=ai_mode_default_index,
        onchange=on_ai_mode_change,
        background_color=TETRALATH_LEFT_PANEL_WIDGET_BACKGROUND_COLOR,
        border_width=TETRALATH_LEFT_PANEL_WIDGET_BORDER_WIDTH,
    )

    menu.add.label("")

    def on_player_color_change(selected: tuple, index: int) -> None:
        game["player_color"] = selected[0][1]

    player_color_options = [
        ("White", definitions.TetralathColor.WHITE),
        ("Black", definitions.TetralathColor.BLACK),
    ]
    player_color_default_index = 0
    game["player_color"] = player_color_options[player_color_default_index][1]
    menu.add.label("Player color")
    player_color_selector = menu.add.selector(
        "",
        player_color_options,
        default=player_color_default_index,
        onchange=on_player_color_change,
        background_color=TETRALATH_LEFT_PANEL_WIDGET_BACKGROUND_COLOR,
        border_width=TETRALATH_LEFT_PANEL_WIDGET_BORDER_WIDTH,
    )

    menu.add.label("")

    def on_start_game() -> None:
        ui_event: definitions.TetralathUIEvent = {
            "type": definitions.TetralathEventType.START_GAME,
        }
        pending_tetralath_ui_events.append(ui_event)

    start_game_button = menu.add.button(
        "Start game",
        on_start_game,
        background_color=TETRALATH_LEFT_PANEL_WIDGET_BACKGROUND_COLOR,
        border_width=TETRALATH_LEFT_PANEL_WIDGET_BORDER_WIDTH,
    )

    return menu, ai_mode_selector, player_color_selector, start_game_button


def disable_left_panel(menu: pygame_menu.Menu, ai_mode_selector: pygame_menu.widgets.Selector, player_color_selector: pygame_menu.widgets.Selector, start_game_button: pygame_menu.widgets.Button) -> None:
    ai_mode_selector.readonly = True
    player_color_selector.readonly = True
    start_game_button.readonly = True
    menu.render()


def draw_right_panel(game: definitions.TetralathGame, pending_tetralath_ui_events: list[definitions.TetralathUIEvent]) -> tuple[pygame_menu.Menu, pygame_menu.widgets.Button]:
    theme = pygame_menu.themes.THEME_DEFAULT.copy()
    theme.title = False
    theme.border_width = 0
    theme.widget_border_width = 0
    theme.widget_border_color = TETRALATH_RIGHT_PANEL_WIDGET_BORDER_COLOR
    theme.background_color = TETRALATH_RIGHT_PANEL_BACKGROUND_COLOR
    theme.widget_background_color = TETRALATH_RIGHT_PANEL_BACKGROUND_COLOR
    theme.widget_font_color = TETRALATH_RIGHT_PANEL_WIDGET_FONT_COLOR
    theme.readonly_color = TETRALATH_RIGHT_PANEL_READONLY_FONT_COLOR
    theme.readonly_selected_color = TETRALATH_RIGHT_PANEL_READONLY_FONT_COLOR
    theme.widget_selection_effect = pygame_menu.widgets.NoneSelection()
    menu = pygame_menu.Menu(
        width=TETRALATH_RIGHT_PANEL_WIDTH,
        height=TETRALATH_RIGHT_PANEL_HEIGHT,
        title="",
        theme=theme,
        position=TETRALATH_RIGHT_PANEL_POSITION,
        screen_dimension=(TETRALATH_WINDOW_WIDTH, TETRALATH_WINDOW_HEIGHT),
    )

    def on_undo_last_move() -> None:
        ui_event: definitions.TetralathUIEvent = {
            "type": definitions.TetralathEventType.UNDO_LAST_MOVE,
        }
        pending_tetralath_ui_events.append(ui_event)

    undo_last_move_button = menu.add.button(
        "Undo last move",
        on_undo_last_move,
        background_color=TETRALATH_RIGHT_PANEL_WIDGET_BACKGROUND_COLOR,
        border_width=TETRALATH_RIGHT_PANEL_WIDGET_BORDER_WIDTH,
    )

    return menu, undo_last_move_button


def enable_right_panel(menu: pygame_menu.Menu, undo_last_move_button: pygame_menu.widgets.Button) -> None:
    undo_last_move_button.readonly = False
    menu.render()


def disable_right_panel(menu: pygame_menu.Menu, undo_last_move_button: pygame_menu.widgets.Button) -> None:
    undo_last_move_button.readonly = True
    menu.render()


def update_panels(menus: pygame_menu.Menu, events: list[pygame.event.Event]) -> None:
    for menu in menus:
        menu.update(events)


def draw_hexagons(game_window: pygame.surface.Surface, hexagon_radius: float, board_center_x: int, board_center_y: int) -> None:
    hexagon_centers = board.get_all_hexagon_centers(board.TETRALATH_BOARD_ROW_LENGTHS, board.TETRALATH_BOARD_MIDDLE_ROW_INDEX, hexagon_radius, board_center_x, board_center_y)
    for center_x, center_y in hexagon_centers:
        hexagon_vertices = board.get_hexagon_vertices(center_x, center_y, hexagon_radius)
        pygame.draw.polygon(game_window, board.TETRALATH_BOARD_POSITION_FILL_COLOR, hexagon_vertices)
        pygame.draw.polygon(game_window, board.TETRALATH_BOARD_POSITION_BORDER_COLOR, hexagon_vertices, board.TETRALATH_BOARD_POSITION_BORDER_WIDTH)


def draw_board_edges(game_window: pygame.surface.Surface, hexagon_radius: float, board_center_x: int, board_center_y: int) -> None:
    for index, edges in board.TETRALATH_BOARD_EDGES.items():
        center_x, center_y = board.get_single_hexagon_center_from_index(index, board.TETRALATH_BOARD_ROW_LENGTHS, board.TETRALATH_BOARD_MIDDLE_ROW_INDEX, hexagon_radius, board_center_x, board_center_y)
        hexagon_vertices = board.get_hexagon_vertices(center_x, center_y, hexagon_radius)
        for start_vertex, end_vertex in edges:
            line_start: tuple[int, int] = (round(hexagon_vertices[start_vertex][0]), round(hexagon_vertices[start_vertex][1]))
            line_end: tuple[int, int] = (round(hexagon_vertices[end_vertex][0]), round(hexagon_vertices[end_vertex][1]))
            pygame.draw.line(game_window, board.TETRALATH_BOARD_EDGE_BORDER_COLOR, line_start, line_end, board.TETRALATH_BOARD_EDGE_BORDER_WIDTH)


def draw_pieces(game_window: pygame.surface.Surface, hexagon_radius: float, board_center_x: int, board_center_y: int, game_board: list[definitions.TetralathColor], piece_radius: float) -> None:
    for position, color in enumerate(game_board):
        if color != definitions.TetralathColor.NONE:
            hexagon_center_x, hexagon_center_y = board.get_single_hexagon_center_from_index(position, board.TETRALATH_BOARD_ROW_LENGTHS, board.TETRALATH_BOARD_MIDDLE_ROW_INDEX, hexagon_radius, board_center_x, board_center_y)
            pygame.draw.circle(game_window, TETRALATH_BOARD_PIECE_COLORS[color], (hexagon_center_x, hexagon_center_y), piece_radius)


def draw_board(game_window: pygame.surface.Surface, game_board: list[definitions.TetralathColor]) -> None:
    draw_hexagons(game_window, TETRALATH_BOARD_HEXAGON_RADIUS, round(TETRALATH_WINDOW_WIDTH / 2), round(TETRALATH_WINDOW_HEIGHT / 2))
    draw_board_edges(game_window, TETRALATH_BOARD_HEXAGON_RADIUS, round(TETRALATH_WINDOW_WIDTH / 2), round(TETRALATH_WINDOW_HEIGHT / 2))
    draw_pieces(game_window, TETRALATH_BOARD_HEXAGON_RADIUS, round(TETRALATH_WINDOW_WIDTH / 2), round(TETRALATH_WINDOW_HEIGHT / 2), game_board, TETRALATH_BOARD_PIECE_RADIUS)


def destroy_game_ui() -> None:
    pygame.quit()
