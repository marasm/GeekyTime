/*

   How to use a custom non-system font.

 */

#include "pebble.h"

static Window *window;

static TextLayer *bat_perc_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *temp_layer;

static BitmapLayer *bt_layer;
static BitmapLayer *battery_layer;
static BitmapLayer *icon_layer;
static BitmapLayer *therm_layer;
static GBitmap *bt_bitmap = NULL;
static GBitmap *battery_bitmap = NULL;
static GBitmap *icon_bitmap = NULL;
static GBitmap *therm_bitmap = NULL;

static bool bt_connected = 1;

static const uint32_t BATTERY_ICONS[] = {
  RESOURCE_ID_IMG_BATTERY_CHRG, //0
  RESOURCE_ID_IMG_BATTERY_20,   //1
  RESOURCE_ID_IMG_BATTERY_40,   //2
  RESOURCE_ID_IMG_BATTERY_60,   //3
  RESOURCE_ID_IMG_BATTERY_80,   //4
  RESOURCE_ID_IMG_BATTERY_100,  //5
};

static void handle_bluetooth(bool connected) {
  if (bt_bitmap)
  {
    gbitmap_destroy(bt_bitmap);
  }
  if (connected)
  {
    bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BT_ON);
    if (!bt_connected)
    {
      vibes_double_pulse();
      bt_connected = 1;
    }
  }
  else
  {
    bt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BT_OFF);
    if (bt_connected)
    {
      bt_connected = 0;
      vibes_long_pulse();
    }
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_bluetooth connected=%i", connected);
  bitmap_layer_set_bitmap(bt_layer, bt_bitmap);
  layer_mark_dirty(bitmap_layer_get_layer(bt_layer));
}

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";
  if (battery_bitmap) {
    gbitmap_destroy(battery_bitmap);
  }
  if (charge_state.is_charging || charge_state.is_plugged) {

    battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[0]);
  } 
  else {
    if (charge_state.charge_percent > 80) //80 - 100% charge
    {
      battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[5]);
    } 
    else if (charge_state.charge_percent > 60 && charge_state.charge_percent <= 80) //60 - 80% charge
    {
      battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[4]);
    }
    else if (charge_state.charge_percent > 40 && charge_state.charge_percent <= 60) //40 - 60% charge
    {
      battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[3]);
    }
    else if (charge_state.charge_percent > 20 && charge_state.charge_percent <= 40) //20 - 40% charge
    {
      battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[2]);
    }
    else  //less than 20% charge
    {
      battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[1]);
    }   
    
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_battery: %i remaining", charge_state.charge_percent);
  snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  text_layer_set_text(bat_perc_layer, battery_text);
  bitmap_layer_set_bitmap(battery_layer, battery_bitmap);
  layer_mark_dirty(bitmap_layer_get_layer(battery_layer));
}

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00"; // Needs to be static because it's used by the system later.
  static char date_text[] = "Sun 01/01"; // Needs to be static because it's used by the system later.
  char *time_format;
  time_format = "%I:%M";
  if (clock_is_24h_style())
  {
    time_format = "%R";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Current time: %s", time_text);

  strftime(date_text, sizeof(date_text), "%a %m-%d", tick_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Current date: %s", date_text);

  text_layer_set_text(time_layer, time_text);
  text_layer_set_text(date_layer, date_text);

  //update the battery
  handle_battery(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
}


static void init() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Entering Init");
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  //BLUETOOTH
  bt_layer = bitmap_layer_create(GRect(55, 3, 10, 10));
  layer_add_child(window_layer, bitmap_layer_get_layer(bt_layer));

  //BATTERY PERCENT
  GFont custom_font_bat_perc = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BAT_PERC_10));
  bat_perc_layer = text_layer_create(GRect(65, 1, 33, 15));
  text_layer_set_font(bat_perc_layer, custom_font_bat_perc);
  text_layer_set_text_color(bat_perc_layer, GColorWhite);
  text_layer_set_background_color(bat_perc_layer, GColorClear);
  text_layer_set_text_alignment(bat_perc_layer, GTextAlignmentRight);

  layer_add_child(window_layer, text_layer_get_layer(bat_perc_layer));

  //BATTERY
  battery_layer = bitmap_layer_create(GRect(144-44, 3, 36, 10));
  layer_add_child(window_layer, bitmap_layer_get_layer(battery_layer));

  //TIME
  GFont custom_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_42));
  time_layer = text_layer_create(GRect(2, 15, 144-2 /* width */, 45 /* 168 max height */));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, custom_font_time);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  //DATE
  GFont custom_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_22));
  date_layer = text_layer_create(GRect(2, 65, 144-2 /* width */, 25 /* 168 max height */));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_font(date_layer, custom_font_date);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  //THERM
  therm_layer = bitmap_layer_create(GRect(65, 112, 16, 36));
  layer_add_child(window_layer, bitmap_layer_get_layer(therm_layer));
  therm_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_THERM);
  bitmap_layer_set_bitmap(therm_layer, therm_bitmap);

  //TEMP
  GFont custom_font_temp = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TEMP_40));
  temp_layer = text_layer_create(GRect(81, 105, 144-85 /* width */, 60 /* 168 max height */));
  text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
  text_layer_set_font(temp_layer, custom_font_temp);
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_text_color(temp_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(temp_layer));
  
  //WEATHER ICON
  icon_layer = bitmap_layer_create(GRect(5, 100, 60, 60));
  layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));

  //TEST DUMMY Stuff
  // text_layer_set_text(bat_perc_layer, "100%");
  // battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[0]);
  // bitmap_layer_set_bitmap(battery_layer, battery_bitmap);
  icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_00);
  bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
   // text_layer_set_text(time_layer, "88:88");
   // text_layer_set_text(date_layer, "Sun 12:22");
  text_layer_set_text(temp_layer, "74");
  

  //EVENT SUBSCRIBTIONS
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
}

static void deinit() {
  text_layer_destroy(bat_perc_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(temp_layer);
  gbitmap_destroy(bt_bitmap);
  bitmap_layer_destroy(bt_layer);
  gbitmap_destroy(battery_bitmap);
  bitmap_layer_destroy(battery_layer);
  gbitmap_destroy(icon_bitmap);
  bitmap_layer_destroy(icon_layer);
  gbitmap_destroy(therm_bitmap);
  bitmap_layer_destroy(therm_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
