/*
 *  dloglowlevel.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 16/04/09.
 *
 */

#include <Carbon/Carbon.h>
#include <cstdio>
#include <cstring>

#include "dlgconsts.h"
#include "dlglowlevel.h"
#include "dlgtool.h"

#include "soundtool.h"
#include "graphtool.h"
#include "mathutil.h"

extern Rect bg[];

extern GWorldPtr bg_gworld;
extern GWorldPtr dlg_buttons_gworld[NUM_BUTTONS][2];
extern map<char*,GWorldPtr*> dlg_gworlds;

extern dlg_t dlgs[ND];
extern dlg_item_t items[NI];

extern char text_long_str[10][256];
extern char text_short_str[140][40];
extern dlg_label_t labels[NL];
extern short dlg_bg;

extern short available_dlog_buttons[NUM_DLOG_B];
extern btn_t buttons[];

void process_new_window (short which_dlg) {
	short i = -1,j,free_slot = -1,free_item = -1,type,flag;
	//char but_str[30];
	Str255 item_str;
	bool str_stored = false;
	//Rect dlg_rect;
	Rect store_rect;
	short win_height = 0, win_width = 0;
	short the_type;
	Handle the_handle = NULL;
	Rect small_rect;
	short num_items;
	short str_offset = 1;
	WindowPtr hDlg = dlgs[which_dlg].win;
	
	free_slot = which_dlg;
	num_items = CountDITL(GetDialogFromWindow(hDlg));
	
	dlgs[free_slot].highest_item = 0;
	
	// Now, give the window its items
	for (i = 0; i < num_items; i++) {
		str_offset = 1;
		GetDialogItem( GetDialogFromWindow(hDlg), i + 1, &the_type, &the_handle, &small_rect);
		if (the_type % 128 == 8) {
			GetDialogItemText(the_handle,item_str);
			p2cstr(item_str);
			dlgs[free_slot].highest_item++;
			str_stored = false;
			if (strlen((char *)item_str) == 0) {
				sprintf((char *) item_str, "+");
				type = DLG_TEXT_BOLD;
				flag = 1;
	            str_stored = true;
			}
			else if (item_str[0] == '+') { // default is framed text
				type = DLG_TEXT_BOLD;
				flag = 1;
	            str_stored = true;
			}
			else if (item_str[0] == '*') {
				type = DLG_TEXT_BOLD;
				flag = 0;
				str_stored = true;
			}
			else if (item_str[0] == '~') {
				type = DLG_TEXT_LARGE;
				flag = 0;
				str_stored = true;
			}
			else if (item_str[0] == '!') {
				type = DLG_TEXT_PLAIN;
				flag = 0;
				str_stored = true;
			}
			else if (item_str[0] == '=') {
				type = DLG_TEXT_DEFAULT;
				flag = 1;
				str_stored = true;
			}
			else if (item_str[0] == '^') {
				type = DLG_CUSTOM_BTN;
				flag = 1;
				if (string_length((char *) item_str) > 55)
					flag = 2;
				str_stored = true;
			}
			else if (((item_str[0] >= 65) && (item_str[0] <= 122)) || (item_str[0] == '"')) {
				type = DLG_TEXT_DEFAULT;
				flag = 0;
				str_offset = 0;
				str_stored = true;
			}
			else if (item_str[0] == '&') {
				type = DLG_CUSTOM_DEF_BTN;
				flag = 1;
				if (string_length((char *) item_str) > 55)
					flag = 2;
				str_stored = true;
			}
//			else if (item_str[0] == '@') { // For potential future use
//				type = DLG_PUSH_BTN;
//				flag = 1;
//				if (string_length((char *) item_str) > 55)
//					flag = 2;
//				str_stored = true;
//			}
			else {
#ifndef EXILE_BIG_GUNS
				sscanf((char *) item_str,"%hd_%hd",&type,&flag);
#endif		
#ifdef EXILE_BIG_GUNS
				sscanf((char *) item_str,"%hd_%hd",&type,&flag);
#endif
			}
			
			free_item = -1;
			// find free item
			if(type == DLG_BUTTON || type == DLG_DEFAULT_BTN || type == DLG_LED_BUTTON ||
			   type == DLG_OLD_PICTURE || type == DLG_HIDDEN_BUTTON || type >= DLG_NEW_PICTURE){
				for (j = 150; j < NI; j++)
					if (items[j].dlg < 0) {
						free_item = j;
						break;
					}
			}else if ((type == 9) || (str_stored && (strlen((char *) item_str) > 35))) {
				for (j = 0; j < 10; j++)
					if (items[j].dlg < 0) {
						free_item = j;
						break;
					}
			}else {
				for (j = 10; j < 140; j++)
					if (items[j].dlg < 0) {
						free_item = j;
						break;
					}
			}
			
			if (free_item >= 0) {
				items[free_item].dlg = dlgs[free_slot].type;
				items[free_item].type = type;
				items[free_item].number = i + 1;
				
				items[free_item].rect = get_item_rect(hDlg,i + 1);
				
				items[free_item].flag = flag;
				
				items[free_item].active = 1;
				items[free_item].label = 0;
         		items[free_item].label_loc = -1;
           		items[free_item].key = 0;
				switch (type) {
					case DLG_BUTTON: case DLG_DEFAULT_BTN:
						GetPortBounds(dlg_buttons_gworld[buttons[flag].type][0], &store_rect);
						items[free_item].rect.right = items[free_item].rect.left + store_rect.right;
						items[free_item].rect.bottom = items[free_item].rect.top + store_rect.bottom;
						items[free_item].key = buttons[flag].def_key;
						if (type == 1)
							items[free_item].key = DLG_KEY_RETURN;
						break;
					case DLG_LED_BUTTON:
						items[free_item].rect.right = items[free_item].rect.left + 14;
						items[free_item].rect.bottom = items[free_item].rect.top + 10;
						items[free_item].key = 255;
						break;
					case DLG_TEXT_BOLD: case DLG_TEXT_PLAIN: case DLG_TEXT_LARGE:
					case DLG_TEXT_CLICKABLE: case DLG_TEXT_DEFAULT:
					case DLG_CUSTOM_BTN: case DLG_CUSTOM_DEF_BTN: 
						//sprintf(((free_item < 10) ? text_long_str[free_item] : text_short_str[free_item - 10]),"");
						if (str_stored == true) {
							if (free_item < 10)
								sprintf(text_long_str[free_item],"%s",(char *) (item_str + str_offset));
							else
								sprintf(text_short_str[free_item - 10],"%-39.39s", (char *) (item_str + str_offset));
						}
						items[free_item].key = 255; 
						if (type >= DLG_CUSTOM_BTN) {
							GetPortBounds(dlg_buttons_gworld[1][0], &store_rect);
							items[free_item].rect.right = items[free_item].rect.left + store_rect.right;
							items[free_item].rect.bottom = items[free_item].rect.top + store_rect.bottom;
							if (type == DLG_CUSTOM_DEF_BTN)
								items[free_item].key = DLG_KEY_RETURN;
						}
						break;
				}
				win_height = max(win_height, items[free_item].rect.bottom + 5);
				win_width = max(win_width, items[free_item].rect.right + 6);
			}
		}
	}
	ShortenDITL(GetDialogFromWindow(hDlg),dlgs[free_slot].highest_item);
	SizeWindow(hDlg,win_width,win_height,false);
	dlgs[free_slot].highest_item = num_items;
}

// LOW LEVEL

short cd_get_indices(short dlg_num, short item_num, short *dlg_index, short *item_index){
	if ((*dlg_index = cd_get_dlg_index(dlg_num)) < 0) {
		return -1;
	}
	if ((*item_index = cd_get_item_id(dlg_num,item_num)) < 0) {
		return -1;
	}
	return 0;
}

short cd_get_dlg_index(short dlog_num){
	short i;
	
	for (i = 0; i < ND; i++)
		if ((dlgs[i].key >= 0) && (dlgs[i].type == dlog_num))
			return i;
	return -1;
}

short cd_find_dlog(WindowPtr window, short *dlg_num, short *dlg_key){
	short i;
	for (i = 0; i < ND; i++)
		if ((dlgs[i].key >= 0) && (dlgs[i].win == window)) {
			*dlg_num = dlgs[i].type;
			*dlg_key = dlgs[i].key;
			return i;
		}
	return -1;
}

short cd_get_item_id(short dlg_num, short item_num){
	short i;
	
	for (i = 0; i < NI; i++)
		if ((items[i].dlg == dlg_num) && (items[i].number == item_num))
			return i;
	return -1;
}

Rect get_item_rect(WindowPtr hDlg, short item_num){
	short the_type;
	Handle the_handle = NULL;
	Rect small_rect;
	
	GetDialogItem( GetDialogFromWindow(hDlg), item_num, &the_type, &the_handle, &small_rect);
	
	return small_rect;
}

void frame_dlog_rect(GrafPtr hDlg, Rect rect, short val, short med_or_lt){
	static RGBColor lt_gray = {57344,57344,57344},dk_gray = {12287,12287,12287},med_gray = {24574,24574,24574};
	GrafPtr old_port;
	
	GetPort(&old_port);
	SetPort(hDlg);
	
	InsetRect(&rect,-1 * val,-1 * val);
	
	RGBForeColor(&dk_gray);
	MoveTo(rect.left,rect.top);
	LineTo(rect.right,rect.top);
	if (med_or_lt) // hDlg == GetWindowPort(mainPtr) // ie for the pc editor only
		RGBForeColor(&med_gray);
	else RGBForeColor(&lt_gray);
	LineTo(rect.right,rect.bottom);
	LineTo(rect.left,rect.bottom);
	RGBForeColor(&dk_gray);
	LineTo(rect.left,rect.top);
	ForeColor(blackColor);
	SetPort(old_port);
}

// win_or_gworld: 0 - window  1 - gworld
// 0 - 300   number of terrain graphic -> PICT_TER (1), PICT_TER_ANIM (2)
// 400 + x - monster graphic num (uses index from scenario) -> PICT_MONST
//   400 is 1st monster from monster index, 401 is 2nd from m. index, and so on
// 700 + x  dlog graphic -> PICT_DLG
// 800 + x  pc graphic -> PICT_PC
// 900 + x  B&W graphic -> PICT_BW
// 950 null item -> 0, PICT_BLANK (0)
// 1000 + x  Talking face -> PICT_TALK
// 1100 - item info help -> PICT_INFO
// 1200 - pc screen help -> PICT_PC_HELP
// 1300 - combat ap -> PICT_COMBAT_AP
// 1400-1402 - button help -> PICT_HELP
// 1500 - stat symbols help -> PICT_STAT
// 1600 + x - scen graphics -> PICT_SCEN
// 1700 + x - anim / field graphic -> PICT_FIELD
// 1800 + x  item graphic -> PICT_ITEM
// 2000 + x - custom graphics up to 2399
// 2400 + x - custom graphics up to 2799, BUT it's a split graphic ...
//    it looks at the size of rect, and places a 32 x 32 or 36 x 36 graphic drawn
//    from the custom gworld, depending on the size of rect. half of graphic is
//    drawn from one custom slot, and half is drawn from next one.
// PICT_OLD (-1) to revert to the old behaviour
GrafPtr hDialog;
short w__gw;
bool fr;
void draw_dialog_graphic(GrafPtr hDlg, Rect rect, short which_g, short type_g, bool do_frame,short win_or_gworld){
	RGBColor store_color;
	
	if (which_g < 0)
		return;
	
	if (which_g >= 2000)
		do_frame = false;
	which_g = which_g % 2000;
	
	if (type_g == PICT_OLD)
		convert_pict(which_g,type_g,rect); // pass by reference
	printf("Drawing graphic %i of type %i.\n",which_g,type_g);
	
	if (type_g == PICT_BLANK) { // Empty. Maybe clear space.
		if (win_or_gworld == 0) {
			InsetRect(&rect, -3, -3);
			tileImage(rect,bg_gworld,bg[dlg_bg]);
		}	
		return;
	}
	GetBackColor(&store_color);
	
	BackColor(whiteColor);	
	
	hDialog = hDlg;
	w__gw = win_or_gworld;
	fr = do_frame;
	switch (type_g) {
		case PICT_TER: // terrain
			draw_preset_ter_pic(which_g,rect);
			break;
		case PICT_TER_ANIM: // animated terrain
			draw_preset_anim_ter_pic(which_g,rect);
			break;
		case PICT_MONST: // monster ... needs use index   small_monst_rect
			// There are 4 different ways to draw, depending on size of monster
			if ((m_pic_index[which_g].x == 1) && (m_pic_index[which_g].y == 1))
				draw_preset_monst_pic_small(which_g,rect);
			else if ((m_pic_index[which_g].x == 2) && (m_pic_index[which_g].y == 1))
				draw_preset_monst_pic_wide(which_g,rect);
			else if ((m_pic_index[which_g].x == 1) && (m_pic_index[which_g].y == 2))
				draw_preset_monst_pic_tall(which_g,rect);
			else if ((m_pic_index[which_g].x == 2) && (m_pic_index[which_g].y == 2))
				draw_preset_monst_pic_large(which_g,rect);
			break;
			
		case PICT_ITEM: // item
			draw_preset_item_pic(which_g,rect);
			break;
		case PICT_DLG: // dialog
			draw_preset_dlg_pic(which_g,rect);
			break;
		case PICT_DLG_LARGE:
			draw_preset_dlg_pic_large(which_g,rect);
			break;
		case PICT_PC: // PC
			draw_preset_pc_pic(which_g,rect);
			break;
		case PICT_TALK: // talk face
			draw_preset_talk_pic(which_g,rect);
			break;
		case PICT_INFO_HELP: // item info help  
			draw_item_info_help_pic(rect);
			break;
		case PICT_PC_HELP: // item info help  
			draw_pc_info_help_pic(rect);
			break;
		case PICT_HELP: // button help
			draw_help_pic(which_g,rect);
			break;
		case PICT_COMBAT_AP: // combat ap help  
			draw_combat_ap_help_pic(rect);
			break;
		case PICT_STAT: // stat symbols help  
			draw_pc_stat_help_pic(rect);
			break;
		case PICT_SCEN:
			draw_preset_scen_pic(which_g,rect);
			break;
		case PICT_SCEN_LARGE:
			draw_preset_scen_pic_large(which_g,rect);
			break;
		case PICT_FIELD: // dialog
			draw_preset_field_pic(which_g,rect);
			break;
		case PICT_CUSTOM + PICT_MONST + PICT_WIDE_MONSTER:
			draw_custom_monst_wide_pic(which_g,rect);
			break;
		case PICT_CUSTOM + PICT_MONST + PICT_TALL_MONSTER:
			draw_custom_monst_tall_pic(which_g,rect);
			break;
		case PICT_CUSTOM + PICT_MONST + PICT_WIDE_MONSTER + PICT_TALL_MONSTER:
			draw_custom_monst_large_pic(which_g,rect);
			break;
		case PICT_CUSTOM + PICT_DLG: // dialog, split
			draw_custom_dlg_pic_split(which_g,rect);
			break;
		case PICT_CUSTOM + PICT_TALK: // facial graphic, split
		case PICT_CUSTOM + PICT_SCEN: // Not currently possible; not sure it ever will be
			draw_custom_talk_pic_split(which_g,rect);
			break;
		default: // PICT_CUSTOM + PICT_TER, PICT_TER_ANIM, PICT_MONST, PICT_ITEM, PICT_PC, PICT_FIELD, etc
			if(type_g < PICT_CUSTOM) break; // not all of which are necessarily valid...
			draw_custom_space_pic(which_g,rect);
			break;
	}
	w__gw = 0;
	hDialog = NULL;
	fr = false;
	
	if (do_frame == true) {
		rect.bottom--;
		rect.right--;
		frame_dlog_rect(hDlg,rect,3);
	}
	RGBBackColor(&store_color);
}

void convert_pict(short& which_g, short& type_g,Rect& rect){
	unsigned short r = which_g / 100;
	printf("Parsing old-style graphic %i.\t",which_g);
	if (r < 3){
		type_g = PICT_TER;
	}else if(r == 4){
		type_g = PICT_TER_ANIM;
		which_g -= 400;
	}else if(r < 7){
		type_g = PICT_MONST;
		which_g -= 700;
	}else if(r >= 20 && r <= 23){
		type_g = PICT_CUSTOM + PICT_TER;
		// It may not be a terrain graphic, but it's at least a terrain-sized graphic, so it matters not
		which_g -= 2000;
	}else if(r >= 24 && r <= 27){
		type_g = PICT_CUSTOM;
		which_g -= 2400;
		if(rect.right - rect.left == 32){
			type_g += PICT_TALK;
		}else if(rect.right - rect.left == 36){
			type_g += PICT_DLG;
		}else{
			type_g += PICT_TER; // This should never happen
		}
	}else if(which_g == 950){
		type_g = PICT_BLANK;
	}else switch(r){
		case 7:
			type_g = PICT_DLG;
			which_g -= 700;
			break;
		case 10:
			type_g = PICT_TALK;
			which_g -= 1000;
			break;
		case 11:
			type_g = PICT_INFO_HELP;
			which_g -= 1100;
			break;
		case 12:
			type_g = PICT_PC_HELP;
			which_g -= 1200;
			break;
		case 13:
			type_g = PICT_COMBAT_AP;
			which_g -= 1300;
			break;
		case 14:
			if(which_g > 10){
				type_g = PICT_SCEN_LARGE;
			}else{
				type_g = PICT_HELP;
			}
			which_g -= 1400;
			break;
		case 15:
			type_g = PICT_STAT;
			which_g -= 1500;
			break;
		case 16:
			type_g = PICT_SCEN;
			which_g -= 1600;
			break;
		case 17:
			type_g = PICT_FIELD;
			which_g -= 1700;
		case 18: case 19:
			type_g = PICT_ITEM;
			which_g -= 1800;
			break;
	}
	printf("Reduced to type = %i, num = %i\n",type_g,which_g);
}

void draw_preset_ter_pic(short which_g,Rect& rect){ // terrain
	short draw_dest = (w__gw == 1) ? 0 : 2;
	printf("Getting terrain icon from sheet %i.\n",which_g / 50);
	GWorldPtr from_gworld = dlg_gworlds["ter"][which_g / 50];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	which_g = which_g % 50;
	Rect from_rect = calc_rect(which_g % 10, which_g / 10);
	if (rect.right - rect.left > 28) 
		InsetRect(&rect,4,0);
	rect_draw_some_item(from_gworld,from_rect, to_gworld, rect);
}

void draw_preset_anim_ter_pic(short which_g,Rect& rect){ // animated terrain
	short draw_dest = (w__gw == 1) ? 0 : 2;
	Rect from_rect = calc_rect(4 * (which_g / 5), which_g % 5);
	GWorldPtr from_gworld = *dlg_gworlds["anim"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	printf("Getting animated terrain graphic %i from sheet 20", which_g);
	//printf(" at {%i,%i,%i,%i}",from_rect.top,from_rect.left,from_rect.bottom,from_rect.right);
	//printf(" – column %i, row %i.\n",from_rect.left/28,from_rect.top/36);
	if (rect.right - rect.left > 28) {
		InsetRect(&rect,4,0);
		rect.right = rect.left + 28;
	}
	rect_draw_some_item(from_gworld ,from_rect, to_gworld,rect);
}

void draw_preset_monst_pic_small(short which_g,Rect& rect){ // monster
	short draw_dest = (w__gw == 1) ? 0 : 2;
	short m_start_pic = m_pic_index[which_g].i;
	GWorldPtr from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	rect.right = rect.left + 28; rect.bottom = rect.top + 36;
	if (w__gw == 0)
		PaintRect(&rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect,transparent);
}

void draw_preset_monst_pic_wide(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	short m_start_pic = m_pic_index[which_g].i;
	Rect small_monst_rect = {0,0,18,14};
	rect.right = rect.left + 28; rect.bottom = rect.top + 36;
	if (w__gw == 0)
		PaintRect(&rect);
	GWorldPtr from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,rect.left,rect.top + 7);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	m_start_pic = m_pic_index[which_g].i + 1;
	from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,14,0);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
}

void draw_preset_monst_pic_tall(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	short m_start_pic = m_pic_index[which_g].i;
	Rect small_monst_rect = {0,0,18,14};
	rect.right = rect.left + 28; rect.bottom = rect.top + 36;
	if (w__gw == 0)
		PaintRect(&rect);
	GWorldPtr from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,rect.left + 7,rect.top);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	m_start_pic = m_pic_index[which_g].i + 1;
	from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,0,18);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
}

void draw_preset_monst_pic_large(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	short m_start_pic = m_pic_index[which_g].i;
	Rect small_monst_rect = {0,0,18,14};
	rect.right = rect.left + 28; rect.bottom = rect.top + 36;
	if (w__gw == 0)
		PaintRect(&rect);
	GWorldPtr from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	m_start_pic = m_start_pic % 20;
	Rect from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,rect.left,rect.top);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	m_start_pic = m_pic_index[which_g].i + 1;
	from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,14,0);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	m_start_pic = m_pic_index[which_g].i + 2;
	from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,-14,18);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	m_start_pic = m_pic_index[which_g].i + 3;
	from_gworld = dlg_gworlds["monst"][m_start_pic / 20];
	m_start_pic = m_start_pic % 20;
	from_rect = calc_rect(2 * (m_start_pic / 10), m_start_pic % 10);
	OffsetRect(&small_monst_rect,14,0);
	if (w__gw == 0)
		PaintRect(&small_monst_rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
}
	
void draw_preset_item_pic(short which_g,Rect& rect){ // item
	short draw_dest = (w__gw == 1) ? 0 : 2;
	rect.right = rect.left + 28; rect.bottom = rect.top + 36;
	Rect to_rect = rect;
	if (w__gw == 0)
		PaintRect(&rect);
	GWorldPtr from_gworld;
	Rect from_rect = {0,0,18,18};
	if (which_g < 45) {
		from_gworld = *dlg_gworlds["items"];
		from_rect = calc_rect(which_g % 5, which_g / 5);
	}
	else {
		from_gworld = *dlg_gworlds["tiny-obj"];
		//rect.top += 4; rect.bottom -= 4;
		InsetRect(&to_rect,5,9);
		OffsetRect(&from_rect,18 * (which_g % 10), 18 * (which_g / 10));
	}
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect,transparent);
}

void draw_preset_dlg_pic(short which_g,Rect& rect){ // dialog
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = *dlg_gworlds["dlog"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,36,36};
	OffsetRect(&from_rect,36 * (which_g % 4),36 * (which_g / 4));
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
}

void draw_preset_pc_pic(short which_g,Rect& rect){ // PC
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld;
	if (*dlg_gworlds["pc"] != NULL)
		from_gworld = *dlg_gworlds["pc"];
	else from_gworld = load_pict(902);
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	//which_g -= 800;
	Rect from_rect = calc_rect(2 * (which_g / 8), which_g % 8);
	PaintRect(&rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect,transparent);
	if (*dlg_gworlds["pc"] == NULL)
		DisposeGWorld(from_gworld);
}

void draw_preset_talk_pic(short which_g,Rect& rect){ // talk face
	rect.right = rect.left + 32; rect.bottom = rect.top + 32;
	GWorldPtr from_gworld = *dlg_gworlds["talkfaces"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,32,32};
	OffsetRect(&from_rect,32 * ((which_g - 1) % 10),32 * ((which_g - 1) / 10));
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
}

void draw_item_info_help_pic(Rect& rect){ // item info help
	short draw_dest = (w__gw == 1) ? 0 : 2;
	Rect from_rect = {0,174,113,312};
	GWorldPtr from_gworld = *dlg_gworlds["mixed"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr)hDialog: *dlg_gworlds["mixed"]);
	GWorldPtr to_gworld = (GWorldPtr)hDialog;
	rect.right = rect.left + from_rect.right - from_rect.left;
	rect.bottom = rect.top + from_rect.bottom - from_rect.top;
	rect_draw_some_item(from_gworld ,from_rect, to_gworld ,rect);
}

void draw_pc_info_help_pic(Rect& rect){ // item info help 
	short draw_dest = (w__gw == 1) ? 0 : 2;
	Rect from_rect = {130,0,157,106};
	GWorldPtr from_gworld = *dlg_gworlds["mixed"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	rect.right = rect.left + from_rect.right - from_rect.left;
	rect.bottom = rect.top + from_rect.bottom - from_rect.top;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
}

void draw_preset_scen_pic_large(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = load_pict(900 + which_g);
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,64,64};
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
	DisposeGWorld(from_gworld);
}

void draw_help_pic(short which_g,Rect& rect){ // button help
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = load_pict(1401);
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,125,320};
	rect.right = rect.left + from_rect.right;
	rect.bottom = rect.top + from_rect.bottom;
	OffsetRect(&from_rect,0,125 * which_g);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
	DisposeGWorld(from_gworld);
}

void draw_combat_ap_help_pic(Rect& rect){ // combat ap help
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = load_pict(1402);
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,100,275};
	rect.right = rect.left + from_rect.right;
	rect.bottom = rect.top + from_rect.bottom;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
	DisposeGWorld(from_gworld);
}

void draw_pc_stat_help_pic(Rect& rect){ // stat symbols help  
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = load_pict(1400);
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,94,386};
	rect.right = rect.left + from_rect.right;
	rect.bottom = rect.top + from_rect.bottom;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
	DisposeGWorld(from_gworld);
}

void draw_preset_scen_pic(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = load_pict(851);
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,32,32};
	OffsetRect(&from_rect,32 * (which_g % 5),32 * (which_g / 5));
	rect.right = rect.left + 32;
	rect.bottom = rect.top + 32;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
	DisposeGWorld(from_gworld);
}

void draw_preset_field_pic(short which_g,Rect& rect){ // dialog
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = *dlg_gworlds["fields"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = calc_rect(which_g % 8, which_g / 8);
	if (w__gw == 0)
		PaintRect(&rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect,transparent);
}

void draw_custom_space_pic(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	printf("Drawing graphic %i as a custom space pic.\n",which_g);
	rect.right = rect.left + 28;
	rect.bottom = rect.top + 32;
	GWorldPtr from_gworld = *dlg_gworlds["custom"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = get_custom_rect(which_g);
	if ((w__gw == 0) && (fr == true))
		PaintRect(&rect);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect,transparent);
}

void draw_custom_monst_wide_pic(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	Rect small_monst_rect = {0,0,18,14};
	rect.right = rect.left + 28;
	rect.bottom = rect.top + 36;
	if ((w__gw == 0) && (fr == true))
		PaintRect(&rect);
	GWorldPtr from_gworld = *dlg_gworlds["custom"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = get_custom_rect(which_g);
	OffsetRect(&small_monst_rect,rect.left,rect.top + 7);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	from_rect = get_custom_rect(which_g+1);
	OffsetRect(&small_monst_rect,14,0);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
}

void draw_custom_monst_tall_pic(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	Rect small_monst_rect = {0,0,18,14};
	rect.right = rect.left + 28;
	rect.bottom = rect.top + 36;
	if ((w__gw == 0) && (fr == true))
		PaintRect(&rect);
	GWorldPtr from_gworld = *dlg_gworlds["custom"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = get_custom_rect(which_g);
	OffsetRect(&small_monst_rect,rect.left + 7,rect.top);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	OffsetRect(&small_monst_rect,0,18);
	from_rect = get_custom_rect(which_g+1);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
}

void draw_custom_monst_large_pic(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	Rect small_monst_rect = {0,0,18,14};
	rect.right = rect.left + 28;
	rect.bottom = rect.top + 36;
	if ((w__gw == 0) && (fr == true))
		PaintRect(&rect);
	GWorldPtr from_gworld = *dlg_gworlds["custom"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = get_custom_rect(which_g);
	OffsetRect(&small_monst_rect,rect.left,rect.top);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	OffsetRect(&small_monst_rect,14,0);
	from_rect = get_custom_rect(which_g+1);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	OffsetRect(&small_monst_rect,-14,18);
	from_rect = get_custom_rect(which_g+2);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
	OffsetRect(&small_monst_rect,14,0);
	from_rect = get_custom_rect(which_g+3);
	rect_draw_some_item(from_gworld,from_rect,to_gworld,small_monst_rect,transparent);
}

void draw_custom_dlg_pic_split(short which_g,Rect& rect){ // dialog, split
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = *dlg_gworlds["custom"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = get_custom_rect(which_g);
	Rect to_rect = rect;
	to_rect.right = to_rect.left + 18;
	to_rect.bottom = to_rect.top + 36;
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect);
	from_rect = get_custom_rect(which_g + 1);
	OffsetRect(&to_rect,18,0);
	from_rect.right = from_rect.left + 18;
	from_rect.bottom = from_rect.top + 36;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect);
}

void draw_custom_talk_pic_split(short which_g,Rect& rect){ // facial graphic, split
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = *dlg_gworlds["custom"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = get_custom_rect(which_g);
	Rect to_rect = rect;
	to_rect.right = to_rect.left + 16;
	to_rect.bottom = to_rect.top + 32;
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect);
	from_rect = get_custom_rect(which_g + 1);
	OffsetRect(&to_rect,16,0);
	from_rect.right = from_rect.left + 16;
	from_rect.bottom = from_rect.top + 32;
	rect_draw_some_item(from_gworld,from_rect,to_gworld,to_rect);
}

void draw_preset_dlg_pic_large(short which_g,Rect& rect){
	short draw_dest = (w__gw == 1) ? 0 : 2;
	GWorldPtr from_gworld = *dlg_gworlds["dlog"];
	//GWorldPtr to_gworld = (GWorldPtr) ((w__gw == 1) ? (GWorldPtr) (hDialog): from_gworld);
	GWorldPtr to_gworld = (GWorldPtr) hDialog;
	Rect from_rect = {0,0,72,72};
	OffsetRect(&from_rect,36 * (which_g % 4),36 * (which_g / 4));
	rect_draw_some_item(from_gworld,from_rect,to_gworld,rect);
}
