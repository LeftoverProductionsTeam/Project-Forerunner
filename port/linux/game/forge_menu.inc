/*
The in-game side panel from ChupathingyCE forge_menu.c (b00dc0c98),
adapted to Forerunner's host-authoritative placement and layout operations.

SPDX-FileCopyrightText: 2026 BirchWoodGod
SPDX-FileCopyrightText: 2026 The Chupathingy contributors
SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "halo_ui_pointer.h"
#include "../../port/linux/src/guide_input.h"
#include "../../port/linux/src/ui_overlay.h"

boolean pc_menu_forge_offered(void);
boolean pc_menu_forge_guest_offered(void);
boolean pc_menu_forge_guest_placements_offered(void);
boolean pc_menu_forge_physics_offered(void);
boolean pc_menu_forge_action(char const *action);
extern boolean terminal_command_running;
void halo_linux_forge_keys(int *tools, int *flight, int *vertical);

enum
{
	FORGE_SIDE_X = 380, FORGE_SIDE_Y = 24,
	FORGE_SIDE_WIDTH = 236, FORGE_SIDE_HEIGHT = 432,
	FORGE_SIDE_LEFT = FORGE_SIDE_X + 12, FORGE_SIDE_RIGHT = FORGE_SIDE_X + FORGE_SIDE_WIDTH - 12,
	FORGE_SIDE_TABS_Y = FORGE_SIDE_Y + 42, FORGE_SIDE_TABS_HEIGHT = 18,
	FORGE_SIDE_LIST_Y = FORGE_SIDE_Y + 82, FORGE_SIDE_ROW_HEIGHT = 22, FORGE_SIDE_ROW_GAP = 3,
	FORGE_SIDE_PROMPTS_Y = FORGE_SIDE_Y + FORGE_SIDE_HEIGHT - 24,
	FORGE_SIDE_BOX_Y = FORGE_SIDE_PROMPTS_Y - 82,
	FORGE_SIDE_ROWS = (FORGE_SIDE_BOX_Y - 6 - FORGE_SIDE_LIST_Y + FORGE_SIDE_ROW_GAP) /
		(FORGE_SIDE_ROW_HEIGHT + FORGE_SIDE_ROW_GAP),
	FORGE_CATALOG_LIMIT = 256,
};

enum forge_side_tab { _forge_side_objects, _forge_side_properties, _forge_side_layouts, _forge_side_options, NUMBER_OF_FORGE_SIDE_TABS };

static char const *const forge_side_tabs[] = { "OBJECTS", "PROPERTIES", "LAYOUTS", "OPTIONS" };
static char const *const forge_side_titles[] = { "OBJECT BROWSER", "PROPERTIES", "LAYOUTS", "OPTIONS" };
static struct
{
	long group;
	char const *title;
} const forge_side_categories[] =
{
	{ 'vehi', "VEHICLES" },
	{ 'weap', "WEAPONS" },
	{ 'eqip', "EQUIPMENT" },
	{ 'scen', "SCENERY" }
};

static struct forge_side_action
{
	char const *title;
	char const *action;
} const forge_side_properties[] =
{
	{ "SELECT AIMED OBJECT", "port forge pick" },
	{ "PLACE COPY", "port forge place" },
	{ "MOVE SELECTED COPY", "port forge move" },
	{ "ROTATE COPY 90 DEGREES", "port forge rotate" },
	{ "CHANGE PHYSICS", "port forge cycle physics" },
	{ "REMOVE SELECTED COPY", "port forge remove" },
}, forge_side_layouts[] =
{
	{ "SAVE LAYOUT (.HMV)", "port forge save hmv" },
	{ "LOAD LAYOUT (.HMV)", "port forge load hmv" },
	{ "LOAD SAVED (.HMV)", "port forge load saved hmv" },
	{ "EXPORT SIDECAR (.TXT)", "port forge save" },
	{ "IMPORT SIDECAR (.TXT)", "port forge load" },
}, forge_side_options[] =
{
	{ "UNDO EDIT", "port forge undo move" },
	{ "REDO EDIT", "port forge redo move" },
	{ "UNDO LAST COPY", "port forge undo" },
	{ "RETURN TO PLAYER", NULL },
};

static struct
{
	boolean open, confirming;
	short tab, row, first, category;
	short counts[NUMBEROF(forge_side_categories)];
	long catalog[FORGE_CATALOG_LIMIT];
	char const *message;
	unsigned int held, repeat_held;
	unsigned long repeat_time;
	float tab_starts[NUMBER_OF_FORGE_SIDE_TABS], tab_ends[NUMBER_OF_FORGE_SIDE_TABS];
} forge_side;

static boolean forge_side_requested;
static boolean forge_side_f5_held;
static boolean forge_side_f5_consumed;

boolean ui_widget_request_forge_browser(void)
{
	if (!pc_menu_forge_offered() || !game_in_progress() || !ui_overlay_available())
		return FALSE;
	forge_side_requested = TRUE;
	return TRUE;
}

static void forge_side_close(void)
{
	if (forge_side.open)
	{
		forge_side.open = FALSE;
		if (!guide_active())
			platform_guide_capture(0);
	}
}

static void forge_side_reset_row(void)
{
	forge_side.row = forge_side.first = 0;
	forge_side.category = NONE;
	forge_side.confirming = FALSE;
	forge_side.message = NULL;
}

static void forge_side_tab_go(short tab)
{
	forge_side.tab = tab;
	forge_side_reset_row();
}

static void forge_side_open(void)
{
	short category;
	int tools_key_down, flight_key_down, vertical;

	halo_linux_forge_keys(&tools_key_down, &flight_key_down, &vertical);
	forge_side_f5_held = tools_key_down != 0;

	if (!ui_overlay_available() || guide_active())
		return;
	forge_side_tab_go(pc_menu_forge_guest_offered() ? _forge_side_properties : _forge_side_objects);
	for (category = 0; category < NUMBEROF(forge_side_categories); category++)
		forge_side.counts[category] = cheats_forge_catalog(forge_side_categories[category].group,
			forge_side.catalog, NUMBEROF(forge_side.catalog));
	forge_side.held = ~(unsigned int)0;
	forge_side.repeat_held = 0;
	forge_side.open = TRUE;
	platform_guide_capture(1);
}

static short forge_side_count(void)
{
	switch (forge_side.tab)
	{
	case _forge_side_objects:
		return forge_side.category == NONE ? NUMBEROF(forge_side_categories) : forge_side.counts[forge_side.category];
	case _forge_side_properties:
		return NUMBEROF(forge_side_properties);
	case _forge_side_layouts:
		return pc_menu_forge_guest_offered() ? 0 : NUMBEROF(forge_side_layouts);
	default:
		return pc_menu_forge_guest_offered() ? 0 : NUMBEROF(forge_side_options) - !director_forge_flight_active(0);
	}
}

static boolean forge_side_property_available(short row)
{
	return (row != 4 || pc_menu_forge_physics_offered()) &&
		(!pc_menu_forge_guest_offered() || row < 2 || pc_menu_forge_guest_placements_offered());
}

static void forge_side_back(void)
{
	if (forge_side.category != NONE)
	{
		short category = forge_side.category;
		forge_side_reset_row();
		forge_side.row = category;
	}
	else
		forge_side_close();
}

static void forge_side_choose(void)
{
	short row = forge_side.row;
	char const *action;

	if (!VALID_INDEX(row, forge_side_count()))
		return;
	if (forge_side.tab == _forge_side_objects)
	{
		if (pc_menu_forge_guest_offered())
			return;
		if (forge_side.category == NONE)
		{
			forge_side_reset_row();
			forge_side.category = row;
			forge_side.counts[row] = cheats_forge_catalog(forge_side_categories[row].group,
				forge_side.catalog, NUMBEROF(forge_side.catalog));
		}
		else
		{
			boolean old_terminal_state = terminal_command_running;
			terminal_command_running = TRUE;
			forge_side.message = cheats_forge_catalog_place(forge_side.catalog[row]) ?
				"Placed at your crosshair." : "Aim at open, nearby ground.";
			terminal_command_running = old_terminal_state;
		}
		return;
	}

	if (forge_side.tab == _forge_side_properties)
	{
		if (!forge_side_property_available(row))
			return;
		action = forge_side_properties[row].action;
		if (pc_menu_forge_guest_offered())
		{
			if (row == 3)
				action = "port forge guest rotate";
			else if (row == 5)
				action = "port forge guest remove";
		}
	}
	else if (forge_side.tab == _forge_side_layouts)
	{
		if ((row == 1 || row == 2 || row == 4) && !forge_side.confirming)
		{
			forge_side.confirming = TRUE;
			forge_side.message = "Select again to confirm load.";
			return;
		}
		action = forge_side_layouts[row].action;
	}
	else
	{
		action = forge_side_options[row].action;
		if (!action)
		{
			forge_side_close();
			cheats_forge_grab_cancel();
			director_toggle_forge_flight();
			return;
		}
	}
	forge_side.confirming = FALSE;
	/* The existing action delivers its result through the HUD. */
	forge_side_close();
	pc_menu_forge_action(action);
}

/* Chupathingy's Guide capture and held-direction repeat. The session continues
 * while one player's input is captured for the browser. */
static unsigned int forge_side_presses(void)
{
	unsigned int held = platform_guide_held();
	unsigned int pressed = held & ~forge_side.held;
	unsigned int directions = held & (GUIDE_INPUT_UP | GUIDE_INPUT_DOWN);
	unsigned long now = system_milliseconds();

	if (directions && directions == forge_side.repeat_held)
	{
		if (now - forge_side.repeat_time >= 400)
		{
			pressed |= directions;
			forge_side.repeat_time = now - 400 + 120;
		}
	}
	else
	{
		forge_side.repeat_held = directions;
		forge_side.repeat_time = now;
	}
	forge_side.held = held;
	return pressed;
}

static void forge_side_step(short step)
{
	forge_side.row = (short)PIN(forge_side.row + step, 0, MAX(forge_side_count() - 1, 0));
	forge_side.confirming = FALSE;
	forge_side.message = NULL;
}

static void forge_side_process(void)
{
	unsigned int pressed = forge_side_presses();

	if (pressed & GUIDE_INPUT_X)
		forge_side_close();
	else if (pressed & GUIDE_INPUT_B)
		forge_side_back();
	else if (pressed & (GUIDE_INPUT_LEFT_BUMPER | GUIDE_INPUT_RIGHT_BUMPER))
		forge_side_tab_go((forge_side.tab + NUMBER_OF_FORGE_SIDE_TABS +
			((pressed & GUIDE_INPUT_RIGHT_BUMPER) ? 1 : -1)) % NUMBER_OF_FORGE_SIDE_TABS);
	else if (pressed & (GUIDE_INPUT_UP | GUIDE_INPUT_DOWN | GUIDE_INPUT_LEFT_TRIGGER | GUIDE_INPUT_RIGHT_TRIGGER))
		forge_side_step((pressed & (GUIDE_INPUT_UP | GUIDE_INPUT_LEFT_TRIGGER) ? -1 : 1) *
			(pressed & (GUIDE_INPUT_LEFT_TRIGGER | GUIDE_INPUT_RIGHT_TRIGGER) ? FORGE_SIDE_ROWS : 1));
	else if (pressed & GUIDE_INPUT_A)
		forge_side_choose();
}

static char const *forge_side_tag_name(long definition, char *text, short capacity)
{
	char const *name, *leaf;
	short i;

	if (definition == NONE)
		return "";
	name = tag_get_name(definition);
	if (!name)
		return "";
	leaf = strrchr(name, '\\');
	snprintf(text, capacity, "%s", leaf ? leaf + 1 : name);
	for (i = 0; text[i]; i++)
		if (text[i] >= 'a' && text[i] <= 'z')
			text[i] -= 'a' - 'A';
	return text;
}

/* The upstream side-column geometry, row cards, detail box and prompt bar. */
static void forge_side_render(void)
{
	struct ui_style_prompt prompts[2] =
	{
		{ UI_BUTTON_A, "ENTER", "SELECT" },
		{ UI_BUTTON_B, "ESC", "CLOSE" }
	};
	short i, count = forge_side_count();
	int previous_theme;
	float y = FORGE_SIDE_LIST_Y;
	char tag_text[64], count_text[16];
	char const *detail = forge_side.message;

	if (!forge_side.open || !ui_overlay_available() || guide_active())
		return;

	ui_overlay_layer(4);
	previous_theme = ui_style_theme_current();
	ui_style_theme(UI_THEME_GREEN);
	ui_style_clock((unsigned int)system_milliseconds());
	ui_style_panel(FORGE_SIDE_X, FORGE_SIDE_Y, FORGE_SIDE_WIDTH, FORGE_SIDE_HEIGHT, 1.f);
	ui_style_text(UI_TEXT_TITLE, FORGE_SIDE_LEFT, FORGE_SIDE_Y + 10, UI_ALIGN_LEFT, forge_side_titles[forge_side.tab]);
	ui_overlay_rect(FORGE_SIDE_LEFT - 4, FORGE_SIDE_Y + 34, FORGE_SIDE_RIGHT - FORGE_SIDE_LEFT + 8, 1, 0,
		ui_style_color(UI_COLOR_RULE));
	ui_style_tabs(FORGE_SIDE_LEFT, FORGE_SIDE_RIGHT, FORGE_SIDE_TABS_Y, FORGE_SIDE_TABS_HEIGHT, forge_side_tabs,
		NUMBER_OF_FORGE_SIDE_TABS, forge_side.tab, "Q", "E", forge_side.tab_starts, forge_side.tab_ends);
	ui_style_text(UI_TEXT_HEADING, FORGE_SIDE_LEFT, FORGE_SIDE_Y + 68, UI_ALIGN_LEFT,
		forge_side.category == NONE ? forge_side_tabs[forge_side.tab] : forge_side_categories[forge_side.category].title);

	forge_side.first = (short)PIN(forge_side.first, MAX(forge_side.row - FORGE_SIDE_ROWS + 1, 0), forge_side.row);
	for (i = forge_side.first; i < count && i < forge_side.first + FORGE_SIDE_ROWS; i++)
	{
		char const *title, *tag = NULL;
		boolean available = TRUE;
		float text_size = ui_style_text_size(UI_TEXT_NAME);

		if (forge_side.tab == _forge_side_objects)
		{
			if (forge_side.category == NONE)
			{
				title = forge_side_categories[i].title;
				snprintf(count_text, sizeof(count_text), "%d", forge_side.counts[i]);
				tag = count_text;
			}
			else
				title = forge_side_tag_name(forge_side.catalog[i], tag_text, sizeof(tag_text));
		}
		else if (forge_side.tab == _forge_side_properties)
		{
			title = forge_side_properties[i].title;
			available = forge_side_property_available(i);
		}
		else if (forge_side.tab == _forge_side_layouts)
			title = forge_side_layouts[i].title;
		else
			title = forge_side_options[i].title;
		ui_style_row(FORGE_SIDE_LEFT, y, FORGE_SIDE_RIGHT - FORGE_SIDE_LEFT, FORGE_SIDE_ROW_HEIGHT, i == forge_side.row);
		while (text_size > 7.f && ui_overlay_text_width(UI_FONT_MENU, text_size, title) >
			FORGE_SIDE_RIGHT - FORGE_SIDE_LEFT - (tag ? 44 : 20))
			text_size -= .5f;
		ui_overlay_text(UI_FONT_MENU, text_size, FORGE_SIDE_LEFT + 10, y + (FORGE_SIDE_ROW_HEIGHT - text_size) * .5f,
			UI_ALIGN_LEFT, ui_style_color(available ? UI_COLOR_NAME : UI_COLOR_LABEL), title);
		if (tag)
			ui_overlay_text(UI_FONT_MENU, 9.5f, FORGE_SIDE_RIGHT - 10, y + 6, UI_ALIGN_RIGHT,
				ui_style_color(UI_COLOR_STATUS), tag);
		y += FORGE_SIDE_ROW_HEIGHT + FORGE_SIDE_ROW_GAP;
	}
	ui_style_glass(FORGE_SIDE_LEFT, FORGE_SIDE_BOX_Y, FORGE_SIDE_RIGHT - FORGE_SIDE_LEFT, 76, 7,
		ui_style_color(UI_COLOR_PANE), ui_style_color(UI_COLOR_ROW_EDGE));

	if (forge_side.tab == _forge_side_properties)
	{
		char const *selection = forge_side_tag_name(cheats_forge_selection(), tag_text, sizeof(tag_text));
		ui_style_text(UI_TEXT_LINE, FORGE_SIDE_LEFT + 10, FORGE_SIDE_BOX_Y + 9, UI_ALIGN_LEFT,
			selection[0] ? selection : "NO LOCAL OBJECT SELECTED");
	}
	if (!detail)
	{
		if (forge_side.tab == _forge_side_objects)
			detail = pc_menu_forge_guest_offered() ? "Object catalog is available to the host." :
				forge_side.category == NONE ? "Select a category." :
				count ? "Place on clear ground at the crosshair." : "No compatible objects in this map.";
		else if (forge_side.tab == _forge_side_properties)
			detail = "Select an object at the crosshair.";
		else if (forge_side.tab == _forge_side_layouts)
			detail = count ? "Layouts are saved against this base map." : "Only the host can manage layouts.";
		else
			detail = count ? "M / F7: Monitor or player." : "These options are available to the host.";
	}
	ui_style_text(UI_TEXT_LINE, FORGE_SIDE_LEFT + 10, FORGE_SIDE_BOX_Y + 30, UI_ALIGN_LEFT, detail);
	if (forge_side.category != NONE)
	{
		prompts[0].word = "PLACE";
		prompts[1].word = "BACK";
	}
	else if (forge_side.confirming)
		prompts[0].word = "CONFIRM";
	ui_style_prompts(FORGE_SIDE_RIGHT, FORGE_SIDE_PROMPTS_Y, 13.f, prompts, NUMBEROF(prompts), NULL, NULL);
	ui_style_theme(previous_theme);
	ui_overlay_layer(0);
}

static void forge_side_pointer(struct halo_ui_pointer const *pointer)
{
	short tab, clicked_row;
	float y = pointer->click_y;

	if (pointer->right_clicks)
		forge_side_back();
	else if (pointer->wheel_steps)
		forge_side_step((short)-pointer->wheel_steps);
	else if (pointer->left_clicks)
	{
		if (y >= FORGE_SIDE_TABS_Y && y < FORGE_SIDE_TABS_Y + FORGE_SIDE_TABS_HEIGHT)
		{
			for (tab = 0; tab < NUMBER_OF_FORGE_SIDE_TABS; tab++)
				if (pointer->click_x >= forge_side.tab_starts[tab] && pointer->click_x < forge_side.tab_ends[tab])
				{
					forge_side_tab_go(tab);
					return;
				}
		}
		if (y < FORGE_SIDE_LIST_Y || pointer->click_x < FORGE_SIDE_LEFT || pointer->click_x >= FORGE_SIDE_RIGHT)
			return;
		clicked_row = (short)(y - FORGE_SIDE_LIST_Y) / (FORGE_SIDE_ROW_HEIGHT + FORGE_SIDE_ROW_GAP);
		if (clicked_row < FORGE_SIDE_ROWS && VALID_INDEX(forge_side.first + clicked_row, forge_side_count()) &&
			((short)y - FORGE_SIDE_LIST_Y) % (FORGE_SIDE_ROW_HEIGHT + FORGE_SIDE_ROW_GAP) < FORGE_SIDE_ROW_HEIGHT)
		{
			if (forge_side.row != forge_side.first + clicked_row)
				forge_side.confirming = FALSE;
			forge_side.row = forge_side.first + clicked_row;
			forge_side_choose();
		}
	}
}
