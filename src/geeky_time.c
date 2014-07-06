
#include "pebble.h"

static Window *window;

static TextLayer *bat_perc_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *temp_layer;
static TextLayer *weather_loc_layer;

static BitmapLayer *bt_layer;
static BitmapLayer *battery_layer;
static BitmapLayer *icon_layer;
static BitmapLayer *therm_layer;
static GBitmap *bt_bitmap = NULL;
static GBitmap *battery_bitmap = NULL;
static GBitmap *icon_bitmap = NULL;
static GBitmap *therm_bitmap = NULL;

static bool bt_connected = 1;
static AppSync sync;
static uint8_t sync_buffer[64];

enum WeatherKey {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_CSTRING
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
  WEATHER_LOCATION_KEY = 0x2,     // TUPLE_CSTRING
};

static const uint32_t BATTERY_ICONS[] = {
  RESOURCE_ID_IMG_BATTERY_CHRG, //0
  RESOURCE_ID_IMG_BATTERY_20,   //1
  RESOURCE_ID_IMG_BATTERY_40,   //2
  RESOURCE_ID_IMG_BATTERY_60,   //3
  RESOURCE_ID_IMG_BATTERY_80,   //4
  RESOURCE_ID_IMG_BATTERY_100,  //5
};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
  // if (icon_bitmap) 
  // {
  //   gbitmap_destroy(icon_bitmap);
  // }
  // icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_00);
  // bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
  // text_layer_set_text(temp_layer, "--");
  // text_layer_set_text(weather_loc_layer, "Unknown");
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "CallBack. Key=%i", (int)key);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Callback. Tuple Value=%s", new_tuple->value->cstring);
  switch (key) {
    case WEATHER_ICON_KEY:
      if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }
      if (strcmp("01d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_01d);
      }
      else if (strcmp("01n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_01n);
      }
      else if (strcmp("02d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_02d);
      }
      else if (strcmp("02n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_02n);
      }
      else if (strcmp("03d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_03d);
      }
      else if (strcmp("03n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_03n);
      }
      else if (strcmp("04d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_04d);
      }
      else if (strcmp("04n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_04n);
      }
      else if (strcmp("09d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_09d);
      }
      else if (strcmp("09n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_09n);
      }
      else if (strcmp("10d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_10d);
      }
      else if (strcmp("10n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_10n);
      }
      else if (strcmp("11d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_11d);
      }
      else if (strcmp("11n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_11n);
      }
      else if (strcmp("13d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_13d);
      }
      else if (strcmp("13n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_13n);
      }
      else if (strcmp("50d", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_50d);
      }
      else if (strcmp("50n", new_tuple->value->cstring) == 0)
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_50n);
      }
      else
      {
        icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_00);
      }
      
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
      layer_mark_dirty(bitmap_layer_get_layer(icon_layer));
      break;

    case WEATHER_TEMPERATURE_KEY:
      text_layer_set_text(temp_layer, new_tuple->value->cstring);
      break;

    case WEATHER_LOCATION_KEY:
      text_layer_set_text(weather_loc_layer, new_tuple->value->cstring);
      break;
  }
}

static void send_cmd(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending sync message to phone...");
  Tuplet value = TupletInteger(1, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &value);
  dict_write_end(iter);

  app_message_outbox_send();
}

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

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed) {

  if(units_changed & MINUTE_UNIT) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Minute tick");
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
    
  }
  if(units_changed & HOUR_UNIT) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Hour tick");
    send_cmd();
    
  }

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
  
  //WEATHER ICON
  icon_layer = bitmap_layer_create(GRect(5, 90, 60, 60));
  layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));

  //THERM
  therm_layer = bitmap_layer_create(GRect(65, 102, 16, 36));
  layer_add_child(window_layer, bitmap_layer_get_layer(therm_layer));
  therm_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_THERM);
  bitmap_layer_set_bitmap(therm_layer, therm_bitmap);

  //TEMP
  GFont custom_font_temp = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TEMP_40));
  temp_layer = text_layer_create(GRect(81, 95, 144-85 /* width */, 55 /* 168 max height */));
  text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
  text_layer_set_font(temp_layer, custom_font_temp);
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_text_color(temp_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(temp_layer));

  //WEATHER LOCATION
  GFont custom_font_weather_loc = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LOCATION_10));
  weather_loc_layer = text_layer_create(GRect(10, 145, 134 /* width */, 18 /* 168 max height */));
  text_layer_set_text_alignment(weather_loc_layer, GTextAlignmentCenter);
  text_layer_set_font(weather_loc_layer, custom_font_weather_loc);
  text_layer_set_background_color(weather_loc_layer, GColorClear);
  text_layer_set_text_color(weather_loc_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(weather_loc_layer));
  

  //TEST DUMMY Stuff
  // text_layer_set_text(bat_perc_layer, "100%");
  // battery_bitmap = gbitmap_create_with_resource(BATTERY_ICONS[0]);
  // bitmap_layer_set_bitmap(battery_layer, battery_bitmap);
  // icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_WEATHER_00);
  // bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
  // text_layer_set_text(time_layer, "88:88");
  // text_layer_set_text(date_layer, "Sun 12:22");
  // text_layer_set_text(temp_layer, "74");
  // text_layer_set_text(weather_loc_layer, "Denver, CO");
  

  //EVENT SUBSCRIBTIONS
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_time_tick(current_time, MINUTE_UNIT);
  handle_battery(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
  tick_timer_service_subscribe(MINUTE_UNIT|HOUR_UNIT, &handle_time_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);


  //GET WEATHER FROM THE WEB
  const int inbound_size = 64;
  const int outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  Tuplet initial_values[] = {
    TupletCString(WEATHER_ICON_KEY, "00"),
    TupletCString(WEATHER_TEMPERATURE_KEY, "--"),
    TupletCString(WEATHER_LOCATION_KEY, "Unknown"),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);
  //Make sure that the weather is refreshed at least hourly
  handle_time_tick(current_time, HOUR_UNIT);
}

static void deinit() {
  app_sync_deinit(&sync);
  text_layer_destroy(bat_perc_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(temp_layer);
  text_layer_destroy(weather_loc_layer);
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
