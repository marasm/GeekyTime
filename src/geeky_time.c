
#include <pebble.h>

//the below 2 lines disable logging
#undef APP_LOG
#define APP_LOG(...)

static Window *window;

static TextLayer *bat_perc_layer;
static TextLayer *time_layer;
static TextLayer *second_layer;
static TextLayer *date_layer;
static TextLayer *temp_layer;
static TextLayer *weather_loc_layer;

static char temp_text[10];
static char temp_num[]= "100";
static char temp_scale[]= "F";
static char comm_text[]= "\xef\x88\x9c";

static TextLayer *bt_layer;
static TextLayer *comm_layer;
static TextLayer *battery_layer;
static TextLayer *icon_layer;

static GFont custom_font_temp_30;
static GFont custom_font_temp_40;

static bool bt_connected = 1;
static AppSync sync;
static uint8_t sync_buffer[192];
static bool bt_vibrate = 1;

enum TupleKey {
  WEATHER_ICON_KEY = 0x0,         // TUPLE_CSTRING
  WEATHER_TEMPERATURE_KEY = 0x1,  // TUPLE_CSTRING
  WEATHER_LOCATION_KEY = 0x2,     // TUPLE_CSTRING
  WEATHER_SCALE_KEY = 0x4,
  CONFIG_BT_VIBRATE = 0x64        // TUPLE_CSTRING (100 in decimal)
};

static bool is_valid_temp(const char * st)
{
  int len = strlen(st);
  int ascii_code;
  int negative_count = -1;

  for (int i = 0; i < len; i++) {
    ascii_code = (int)st[i];
    switch (ascii_code)
    {

      case 45: // Allow a negative sign.
          negative_count++;
          if (negative_count || i != 0) {
              return false;
          }
          break;

      default:
          if (ascii_code < 48 || ascii_code > 57) {
              return false;
          }
          break;
    }
  }
  return true;
}


static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  char *error_desc;
  switch (app_message_error) {
    case APP_MSG_OK:
      error_desc = "APP_MSG_OK";
      break;
    case APP_MSG_SEND_TIMEOUT:
      error_desc =  "APP_MSG_SEND_TIMEOUT";
      break;
    case APP_MSG_SEND_REJECTED:
      error_desc =  "APP_MSG_SEND_REJECTED";
      break;
    case APP_MSG_NOT_CONNECTED:
      error_desc =  "APP_MSG_NOT_CONNECTED";
      break;
    case APP_MSG_APP_NOT_RUNNING:
      error_desc =  "APP_MSG_APP_NOT_RUNNING";
      break;
    case APP_MSG_INVALID_ARGS:
      error_desc =  "APP_MSG_INVALID_ARGS";
      break;
    case APP_MSG_BUSY:
      error_desc =  "APP_MSG_BUSY";
      break;
    case APP_MSG_BUFFER_OVERFLOW:
      error_desc =  "APP_MSG_BUFFER_OVERFLOW";
      break;
    case APP_MSG_ALREADY_RELEASED:
      error_desc =  "APP_MSG_ALREADY_RELEASED";
      break;
    case APP_MSG_CALLBACK_ALREADY_REGISTERED:
      error_desc =  "APP_MSG_CALLBACK_ALREADY_REGISTERED";
      break;
    case APP_MSG_CALLBACK_NOT_REGISTERED:
      error_desc =  "APP_MSG_CALLBACK_NOT_REGISTERED";
      break;
    case APP_MSG_OUT_OF_MEMORY:
      error_desc =  "APP_MSG_OUT_OF_MEMORY";
      break;
    case APP_MSG_CLOSED:
      error_desc =  "APP_MSG_CLOSED";
      break;
    case APP_MSG_INTERNAL_ERROR:
      error_desc =  "APP_MSG_INTERNAL_ERROR";
      break;
    default:
      error_desc =  "UNKNOWN ERROR";
      break;
  }

  if (error_desc != NULL)
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %s", error_desc);
  }

  strcpy(comm_text, "\xef\x84\xa9");
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "CallBack. Key=%i", (int)key);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Callback. Tuple Value=%s", new_tuple->value->cstring);
  static char weather_text[]= "\xef\x80\xbe";

  switch (key) {
    case WEATHER_ICON_KEY:
      if (strcmp("01d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x8d");
      }
      else if (strcmp("01n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\xae");
      }
      else if (strcmp("02d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x82");
      }
      else if (strcmp("02n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\xb1");
      }
      else if (strcmp("03d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x81\x81");
      }
      else if (strcmp("03n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x81\x81");
      }
      else if (strcmp("04d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x93");
      }
      else if (strcmp("04n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x93");
      }
      else if (strcmp("09d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x89");
      }
      else if (strcmp("09n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\xb7");
      }
      else if (strcmp("10d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x88");
      }
      else if (strcmp("10n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\xb6");
      }
      else if (strcmp("11d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x85");
      }
      else if (strcmp("11n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\xb3");
      }
      else if (strcmp("13d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x8a");
      }
      else if (strcmp("13n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\xb8");
      }
      else if (strcmp("50d", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x80\x83");
      }
      else if (strcmp("50n", new_tuple->value->cstring) == 0)
      {
        strcpy(weather_text, "\xef\x81\x8a");
      }
      else
      {
        strcpy(weather_text, "\xef\x80\xbe");
      }

      text_layer_set_text(icon_layer, weather_text);
      break;

    case WEATHER_TEMPERATURE_KEY:
      if (is_valid_temp(new_tuple->value->cstring) || strncmp("--", new_tuple->value->cstring, 2) == 0)
      {
        if (strlen(new_tuple->value->cstring) > 2)
        {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "3 digit temp detected. Setting font to 30");
          text_layer_set_font(temp_layer, custom_font_temp_30);
        }
        else
        {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "2 digit temp detected. Setting font to 40");
          text_layer_set_font(temp_layer, custom_font_temp_40);
        }
        strncpy(temp_num, new_tuple->value->cstring, 3);
        strncpy(temp_text, temp_num, 3);
        strcat(temp_text, "°");
        strncat(temp_text, temp_scale, 1);
        text_layer_set_text(temp_layer, temp_text);
      }
      else
      {
        if (is_valid_temp(old_tuple->value->cstring))
        {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "invalid temp detected. Will keep current value = %s",
            old_tuple->value->cstring);
          text_layer_set_text(temp_layer, old_tuple->value->cstring);
        }
        else
        {
          APP_LOG(APP_LOG_LEVEL_DEBUG, "invalid temp detected and the previous value is bad too. Using -- ");
          text_layer_set_font(temp_layer, custom_font_temp_40);
          text_layer_set_text(temp_layer, "--");
        }
      }
      break;

    case WEATHER_LOCATION_KEY:
      text_layer_set_text(weather_loc_layer, new_tuple->value->cstring);

      //update the comm icon only once per call
      strcpy(comm_text, "");
      break;
    case WEATHER_SCALE_KEY:
      if (strcmp(new_tuple->value->cstring, "C") == 0)
      {
        strcpy(temp_scale, "C");
      }
      else
      {
        strcpy(temp_scale, "F");
      }
      strcpy(temp_text, temp_num);
      strcat(temp_text, "°");
      strcat(temp_text, temp_scale);
      text_layer_set_text(temp_layer, temp_text);
      break;

    case CONFIG_BT_VIBRATE:
      if (strcmp(new_tuple->value->cstring, "On") == 0)
      {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting BT Vibrate to On");
        bt_vibrate = 1;
      }
      else
      {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting BT Vibrate to Off");
        bt_vibrate = 0;
      }
      break;
  }
}

static void send_cmd(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending sync message to phone...");
  strcpy(comm_text, "\xef\x88\x9c");

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

static void handle_tap(AccelAxisType axis, int32_t direction)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "tap direction=%i", (int)direction);
  switch (axis) {
    case ACCEL_AXIS_X:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "tap axis=X");
      break;
    case ACCEL_AXIS_Y:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "tap axis=Y");
      break;
    case ACCEL_AXIS_Z:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "tap axis=Z");
      send_cmd();
      break;
  }
}

static void handle_bluetooth(bool connected) {
  static char bt_text[5]= "";
  if (connected)
  {
    strcpy(bt_text, "\xef\x84\x96");
    if (!bt_connected)
    {
      bt_connected = 1;
      if (bt_vibrate)
      {
        vibes_double_pulse();
      }
    }
  }
  else
  {
    strcpy(bt_text, "");
    if (bt_connected)
    {
      bt_connected = 0;
      if (bt_vibrate)
      {
        vibes_long_pulse();
      }
    }
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_bluetooth connected=%i", connected);
  text_layer_set_text(bt_layer, bt_text);
}

static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";
  static char battery_icon[] = "\xef\x84\x93";
    if (charge_state.charge_percent > 80) //80 - 100% charge
    {
      strcpy(battery_icon, "\xef\x84\x93");
    }
    else if (charge_state.charge_percent > 50 && charge_state.charge_percent <= 80) //50 - 80% charge
    {
      strcpy(battery_icon, "\xef\x84\x94");
    }
    else if (charge_state.charge_percent > 20 && charge_state.charge_percent <= 50) //20 - 50% charge
    {
      strcpy(battery_icon, "\xef\x84\x95");
    }
    else  //less than 20% charge
    {
      strcpy(battery_icon, "\xef\x84\x92");
    }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_battery: %i remaining", charge_state.charge_percent);
  snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  text_layer_set_text(bat_perc_layer, battery_text);
  text_layer_set_text(battery_layer, battery_icon);
}

static void handle_time_tick(struct tm* tick_time, TimeUnits units_changed) {

  if(units_changed & SECOND_UNIT) {
    static char time_text[] = "00:00"; // Needs to be static because it's used by the system later.
    static char second_text[] = "00";
    static char date_text[] = "Wednesday\n1970-01-01"; // Needs to be static because it's used by the system later.
    char *time_format;
    time_format = "%I:%M";
    if (clock_is_24h_style())
    {
      time_format = "%H:%M";
    }

    strftime(time_text, sizeof(time_text), time_format, tick_time);
    strftime(second_text, sizeof(second_text), "%S", tick_time);

    strftime(date_text, sizeof(date_text), "%A\n%F", tick_time);

    text_layer_set_text(time_layer, time_text);
    text_layer_set_text(second_layer, second_text);
    text_layer_set_text(date_layer, date_text);

  }
  //if the temp has not been refreshed yet (", --") do it no
  if (units_changed & MINUTE_UNIT)
  {
      if(temp_layer &&
         text_layer_get_text(temp_layer) != NULL &&
         strncmp("--", text_layer_get_text(temp_layer), 2) == 0)
      {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Default temp of -- detected during minute tick. Request weather refresh");
        send_cmd();
      }
  }
  //Make sure that the weather is refreshed at least hourly
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

  //PHONE COMM
  GFont custom_font_status = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_STATUS_14));
  comm_layer = text_layer_create(GRect(35, 1, 15, 15));
  text_layer_set_font(comm_layer, custom_font_status);
  text_layer_set_text_color(comm_layer, GColorWhite);
  text_layer_set_background_color(comm_layer, GColorClear);
  //text_layer_set_text_alignment(comm_layer, GTextAlignmentRight);

  layer_add_child(window_layer, text_layer_get_layer(comm_layer));

  text_layer_set_text(comm_layer, comm_text);

  //BLUETOOTH
  bt_layer = text_layer_create(GRect(55, 1, 15, 15));
  text_layer_set_font(bt_layer, custom_font_status);
  text_layer_set_text_color(bt_layer, GColorWhite);
  text_layer_set_background_color(bt_layer, GColorClear);
  text_layer_set_text_alignment(bt_layer, GTextAlignmentRight);

  layer_add_child(window_layer, text_layer_get_layer(bt_layer));

  //BATTERY PERCENT
  GFont custom_font_bat_perc = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BAT_PERC_12));
  bat_perc_layer = text_layer_create(GRect(85, 1, 33, 15));
  text_layer_set_font(bat_perc_layer, custom_font_bat_perc);
  text_layer_set_text_color(bat_perc_layer, GColorWhite);
  text_layer_set_background_color(bat_perc_layer, GColorClear);
  text_layer_set_text_alignment(bat_perc_layer, GTextAlignmentRight);

  layer_add_child(window_layer, text_layer_get_layer(bat_perc_layer));

  //BATTERY
  battery_layer = text_layer_create(GRect(120, 1, 15, 15));
  text_layer_set_font(battery_layer, custom_font_status);
  text_layer_set_text_color(battery_layer, GColorWhite);
  text_layer_set_background_color(battery_layer, GColorClear);
  text_layer_set_text_alignment(battery_layer, GTextAlignmentRight);

  layer_add_child(window_layer, text_layer_get_layer(battery_layer));

  //TIME
  GFont custom_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_42));
  time_layer = text_layer_create(GRect(2, 5, 144-20 /* width */, 45 /* 168 max height */));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, custom_font_time);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  //SECOND
  GFont custom_font_second = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SECOND_10));
  second_layer = text_layer_create(GRect(124, 37, 20 /* width */, 20 /* 168 max height */));
  text_layer_set_text_alignment(second_layer, GTextAlignmentCenter);
  text_layer_set_font(second_layer, custom_font_second);
  text_layer_set_background_color(second_layer, GColorClear);
  text_layer_set_text_color(second_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(second_layer));

  //DATE
  GFont custom_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATE_14));
  date_layer = text_layer_create(GRect(2, 55, 144-2 /* width */, 35 /* 168 max height */));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_font(date_layer, custom_font_date);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(date_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  //WEATHER ICON
  GFont custom_font_weather_icon = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_ICON_30));
  icon_layer = text_layer_create(GRect(5, 95, 60, 60));
  text_layer_set_text_alignment(icon_layer, GTextAlignmentCenter);
  text_layer_set_font(icon_layer, custom_font_weather_icon);
  text_layer_set_background_color(icon_layer, GColorClear);
  text_layer_set_text_color(icon_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(icon_layer));

  //TEMP
  custom_font_temp_30 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TEMP_30));
  custom_font_temp_40 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TEMP_30));

  temp_layer = text_layer_create(GRect(60, 95, 144-65 /* width */, 60 /* 168 max height */));
  text_layer_set_font(temp_layer, custom_font_temp_40);
  text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_text_color(temp_layer, GColorWhite);

  layer_add_child(window_layer, text_layer_get_layer(temp_layer));

  //WEATHER LOCATION
  GFont custom_font_weather_loc = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LOCATION_10));
  weather_loc_layer = text_layer_create(GRect(2, 145, 142 /* width */, 18 /* 168 max height */));
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
  handle_time_tick(current_time, SECOND_UNIT);
  handle_battery(battery_state_service_peek());
  handle_bluetooth(bluetooth_connection_service_peek());
  tick_timer_service_subscribe(SECOND_UNIT|MINUTE_UNIT|HOUR_UNIT, &handle_time_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  accel_tap_service_subscribe(&handle_tap);


  //GET WEATHER FROM THE WEB
  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);

  char *bt_vibrate_str;
  if (bt_vibrate)
  {
    bt_vibrate_str = "On";
  }
  else
  {
    bt_vibrate_str = "Off";
  }

  Tuplet initial_values[] = {
    TupletCString(WEATHER_ICON_KEY, "00"),
    TupletCString(WEATHER_TEMPERATURE_KEY, "--"),
    TupletCString(WEATHER_LOCATION_KEY, "Unknown"),
    TupletCString(WEATHER_SCALE_KEY, "F"),
    TupletCString(CONFIG_BT_VIBRATE, bt_vibrate_str)
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values,
      ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);

  send_cmd();
}

static void deinit() {
  app_sync_deinit(&sync);
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  accel_tap_service_unsubscribe();
  text_layer_destroy(bat_perc_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(second_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(temp_layer);
  text_layer_destroy(weather_loc_layer);
  text_layer_destroy(comm_layer);
  text_layer_destroy(bt_layer);
  text_layer_destroy(battery_layer);
  text_layer_destroy(icon_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
