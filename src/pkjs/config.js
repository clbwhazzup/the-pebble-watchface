module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "BackgroundColor",
        "defaultValue": "0x000000",
        "label": "Background Color"
      },
      {
        "type": "color",
        "messageKey": "TextColor",
        "defaultValue": "0xFFFFFF",
        "label": "Text Color"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Display Settings"
      },
      {
        "type": "select",
        "messageKey": "TemperatureUnit",
        "label": "Temperature Unit",
        "description": "Choose Fahrenheit or Celcuis as your temperature unit",
        "defaultValue": "false",
        "options": [
          { "label": "Fahrenheit", "value": "true"},
          { "label": "Celcius", "value": "false"}
        ]
      },
      {
        "type": "select",
        "messageKey": "PercentChoice",
        "label": "Percent Slot",
        "description": "Choose to show current humidity or daily max preciptation",
        "defaultValue": "false",
        "options": [
          { "label": "Precipitation", "value": "true"},
          { "label": "Humidity", "value": "false"}
        ]
      },
      {
        "type": "select",
        "messageKey": "SecondsChoice",
        "defaultValue": "show",
        "label": "Seconds Slot",
        "description": "Requires watchface restart",
        "options": [
          { "label": "Hide", "value": "hide" },
          { "label": "Show on Tap", "value": "tap" },
          { "label": "Show Normally", "value": "show" }
        ]
      },
      {
        "type": "slider",
        "messageKey": "SecondsLimit",
        "defaultValue": 5,
        "label": "Seconds Display Limit (0-60)",
        "min": 0,
        "max": 60,
        "step": 1
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Display Toggles"
      },
      {
        "type": "toggle",
        "messageKey": "ShowDate",
        "label": "Show Date",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "ShowCity",
        "label": "Show City",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
