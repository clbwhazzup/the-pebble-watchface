module.exports = [
  {
    "type": "heading",
    "defaultValue": "Settings"
  },
  {
    "type": "text",
    "defaultValue": "Some settings affect battery without restart"
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
        "defaultValue": "humid",
        "options": [
          { "label": "Precipitation", "value": "precip"},
          { "label": "Humidity", "value": "humid"},
          { "label": "Hide", "value": "hide"}
        ]
      },
      {
        "type": "select",
        "messageKey": "BatteryChoice",
        "label": "Battery Slot",
        "description": "Requires watchface restart. Choose to show the bar, the percentage, both, or neither",
        "defaultValue": "both",
        "options": [
          { "label": "Show Both", "value": "both"},
          { "label": "Show Bar", "value": "bar"},
          { "label": "Show Pecentage", "value": "percent"},
          { "label": "Hide Both", "value": "hide"}
        ]
      },
      {
        "type": "select",
        "messageKey": "SecondsChoice",
        "defaultValue": "show",
        "label": "Seconds Slot",
        "description": "Requires watchface restart",
        "options": [
          { "label": "Show Always", "value": "show" },
          { "label": "Show on Tap", "value": "tap" },
          { "label": "Hide", "value": "hide" }
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
      },
      {
        "type": "toggle",
        "messageKey": "ShowHealth",
        "label": "Show Health",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "ShowConditions",
        "label": "Show Conditions",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "ShowHighLow",
        "label": "Show High/Low Temperatures",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "ShowTemp",
        "label": "Show Current Temperature",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "ShowRiseSet",
        "label": "Show Sunrise and Sunset",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
