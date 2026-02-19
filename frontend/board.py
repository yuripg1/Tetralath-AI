import math


TETRALATH_BOARD_NUMBER_OF_HEXAGONS = 61
TETRALATH_BOARD_ROW_LENGTHS = [5, 6, 7, 8, 9, 8, 7, 6, 5]
TETRALATH_BOARD_MIDDLE_ROW_INDEX = 4
TETRALATH_BOARD_POSITION_FILL_COLOR = (255, 255, 0)
TETRALATH_BOARD_POSITION_BORDER_COLOR = (128, 128, 128)
TETRALATH_BOARD_POSITION_BORDER_WIDTH = 3
TETRALATH_BOARD_EDGE_BORDER_COLOR = (0, 0, 0)
TETRALATH_BOARD_EDGE_BORDER_WIDTH = 5
TETRALATH_BOARD_EDGES: dict[int, list[tuple[int, int]]] = {
    1: [(5, 0), (0, 1)],
    2: [(5, 0), (0, 1)],
    3: [(5, 0), (0, 1)],
    5: [(0, 1), (1, 2)],
    10: [(5, 0), (4, 5)],
    11: [(0, 1), (1, 2)],
    17: [(5, 0), (4, 5)],
    18: [(0, 1), (1, 2)],
    25: [(5, 0), (4, 5)],
    35: [(1, 2), (2, 3)],
    42: [(4, 5), (3, 4)],
    43: [(1, 2), (2, 3)],
    49: [(4, 5), (3, 4)],
    50: [(1, 2), (2, 3)],
    55: [(4, 5), (3, 4)],
    57: [(2, 3), (3, 4)],
    58: [(2, 3), (3, 4)],
    59: [(2, 3), (3, 4)],
    0: [(5, 0), (0, 1), (1, 2)],
    4: [(5, 0), (0, 1), (4, 5)],
    26: [(0, 1), (1, 2), (2, 3)],
    34: [(5, 0), (4, 5), (3, 4)],
    56: [(1, 2), (2, 3), (3, 4)],
    60: [(2, 3), (3, 4), (4, 5)],
}
TETRALATH_BOARD_PIECE_BORDER_WIDTH = 1


def get_hexagon_vertices(center_x: float, center_y: float, radius: float) -> list[tuple[float, float]]:
    hexagon_vertices: list[tuple[float, float]] = []
    for i in range(6):
        angle_in_radians = math.radians(90 + (60 * i))
        vertex_x = center_x + (radius * math.cos(angle_in_radians))
        vertex_y = center_y - (radius * math.sin(angle_in_radians))
        hexagon_vertices.append((vertex_x, vertex_y))
    return hexagon_vertices


def get_axial_coordinates(row: int, column: int, number_of_rows: int, middle_row_index: int) -> tuple[int, int]:
    axial_column_offset = min(row, number_of_rows - row - 1)
    if row > middle_row_index:
        axial_column_offset += row - middle_row_index
    axial_column = column - axial_column_offset
    axial_row = row - middle_row_index
    return (axial_row, axial_column)


def get_row_and_column_from_index(index: int, row_lengths: list[int]) -> tuple[int, int]:
    for row in range(len(row_lengths)):
        if index < row_lengths[row]:
            return (row, index)
        index -= row_lengths[row]
    return (-1, -1)


def get_single_hexagon_center(row: int, column: int, number_of_rows: int, middle_row_index: int, hexagon_radius: float, board_center_x: int, board_center_y: int) -> tuple[float, float]:
    axial_row, axial_column = get_axial_coordinates(row, column, number_of_rows, middle_row_index)
    center_x = math.sqrt(3) * hexagon_radius * (axial_column + (axial_row / 2))
    center_y = (3 / 2) * hexagon_radius * axial_row
    return (board_center_x + center_x, board_center_y + center_y)


def get_single_hexagon_center_from_index(index: int, row_lengths: list[int], middle_row_index: int, hexagon_radius: float, board_center_x: int, board_center_y: int) -> tuple[int, int]:
    row, column = get_row_and_column_from_index(index, row_lengths)
    return get_single_hexagon_center(row, column, len(row_lengths), middle_row_index, hexagon_radius, board_center_x, board_center_y)


def get_all_hexagon_centers(row_lengths: list[int], middle_row_index: int, hexagon_radius: float, board_center_x: int, board_center_y: int) -> list[tuple[float, float]]:
    number_of_rows = len(row_lengths)
    hexagon_centers: list[tuple[float, float]] = []
    for row in range(number_of_rows):
        for column in range(row_lengths[row]):
            hexagon_center = get_single_hexagon_center(row, column, number_of_rows, middle_row_index, hexagon_radius, board_center_x, board_center_y)
            hexagon_centers.append(hexagon_center)
    return hexagon_centers


def is_click_in_hexagon(click_x: int, click_y: int, hexagon_radius: float, hexagon_center_x: int, hexagon_center_y: int) -> bool:
    hexagon_vertices = get_hexagon_vertices(hexagon_center_x, hexagon_center_y, hexagon_radius)
    number_of_vertices = len(hexagon_vertices)
    for i in range(number_of_vertices):
        start_vertex_x, start_vertex_y = hexagon_vertices[i]
        end_vertex_x, end_vertex_y = hexagon_vertices[(i + 1) % number_of_vertices]
        cross_product = (end_vertex_x - start_vertex_x) * (click_y - start_vertex_y) - (end_vertex_y - start_vertex_y) * (click_x - start_vertex_x)
        if cross_product >= 0:
            return False
    return True


def click_in_an_hexagon(click_x: int, click_y: int, number_of_hexagons: int, row_lengths: list[int], middle_row_index: int, hexagon_radius: float, board_center_x: int, board_center_y: int) -> int | None:
    for i in range(number_of_hexagons):
        row, column = get_row_and_column_from_index(i, row_lengths)
        hexagon_center_x, hexagon_center_y = get_single_hexagon_center(row, column, len(row_lengths), middle_row_index, hexagon_radius, board_center_x, board_center_y)
        if is_click_in_hexagon(click_x, click_y, hexagon_radius, hexagon_center_x, hexagon_center_y):
            return i
    return None
