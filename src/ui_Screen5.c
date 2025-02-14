// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 9.1.0
// Project name: SquareLine_Project

#include "ui.h"

void ui_Screen5_screen_init(void)
{
    ui_Screen5 = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_Screen5, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Calendar1 = lv_calendar_create(ui_Screen5);
    lv_calendar_set_today_date(ui_Calendar1, 2024, 12, 2);
    lv_calendar_set_showed_date(ui_Calendar1, 2024, 12);
    lv_obj_t * ui_Calendar1_header = lv_calendar_header_arrow_create(ui_Calendar1);
    lv_obj_set_width(ui_Calendar1, 189);
    lv_obj_set_height(ui_Calendar1, 178);
    lv_obj_set_x(ui_Calendar1, 52);
    lv_obj_set_y(ui_Calendar1, -2);
    lv_obj_set_align(ui_Calendar1, LV_ALIGN_CENTER);

    ui_Set_alarm = lv_button_create(ui_Screen5);
    lv_obj_set_width(ui_Set_alarm, 76);
    lv_obj_set_height(ui_Set_alarm, 50);
    lv_obj_set_x(ui_Set_alarm, -98);
    lv_obj_set_y(ui_Set_alarm, -15);
    lv_obj_set_align(ui_Set_alarm, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Set_alarm, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_remove_flag(ui_Set_alarm, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label11 = lv_label_create(ui_Screen5);
    lv_obj_set_width(ui_Label11, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label11, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label11, -98);
    lv_obj_set_y(ui_Label11, -14);
    lv_obj_set_align(ui_Label11, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label11, "Alarm");

    ui_Set_Wifi = lv_button_create(ui_Screen5);
    lv_obj_set_width(ui_Set_Wifi, 76);
    lv_obj_set_height(ui_Set_Wifi, 50);
    lv_obj_set_x(ui_Set_Wifi, -98);
    lv_obj_set_y(ui_Set_Wifi, -85);
    lv_obj_set_align(ui_Set_Wifi, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Set_Wifi, LV_OBJ_FLAG_SCROLL_ON_FOCUS);     /// Flags
    lv_obj_remove_flag(ui_Set_Wifi, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Label15 = lv_label_create(ui_Screen5);
    lv_obj_set_width(ui_Label15, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label15, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label15, -99);
    lv_obj_set_y(ui_Label15, -76);
    lv_obj_set_align(ui_Label15, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label15, "Wifi\n");

    ui_Container1 = lv_obj_create(ui_Screen5);
    lv_obj_remove_style_all(ui_Container1);
    lv_obj_set_width(ui_Container1, 100);
    lv_obj_set_height(ui_Container1, 89);
    lv_obj_set_x(ui_Container1, -101);
    lv_obj_set_y(ui_Container1, 62);
    lv_obj_set_align(ui_Container1, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_Container1, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_color(ui_Container1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Image4 = lv_image_create(ui_Screen5);
    lv_image_set_src(ui_Image4, &ui_img_1895770859);
    lv_obj_set_width(ui_Image4, LV_SIZE_CONTENT);   /// 512
    lv_obj_set_height(ui_Image4, LV_SIZE_CONTENT);    /// 512
    lv_obj_set_x(ui_Image4, -133);
    lv_obj_set_y(ui_Image4, 36);
    lv_obj_set_align(ui_Image4, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_Image4, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_remove_flag(ui_Image4, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_image_set_scale(ui_Image4, 15);

    ui_Label14 = lv_label_create(ui_Screen5);
    lv_obj_set_width(ui_Label14, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label14, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label14, -110);
    lv_obj_set_y(ui_Label14, 68);
    lv_obj_set_align(ui_Label14, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label14, "T: 38^ C");
    lv_obj_set_style_text_color(ui_Label14, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label14, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label14, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Label19 = lv_label_create(ui_Screen5);
    lv_obj_set_width(ui_Label19, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label19, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label19, -115);
    lv_obj_set_y(ui_Label19, 92);
    lv_obj_set_align(ui_Label19, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label19, "H: 34%");
    lv_obj_set_style_text_color(ui_Label19, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_Label19, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui_Label19, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(ui_Set_alarm, ui_event_Set_alarm, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Set_Wifi, ui_event_Set_Wifi, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(ui_Screen5, ui_event_Screen5, LV_EVENT_ALL, NULL);

}
