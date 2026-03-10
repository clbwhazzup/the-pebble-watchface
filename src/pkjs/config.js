module.exports = [
  {
    "type": "heading",
    "defaultValue": "Watchface Settings"
  },
  {
    "type": "text",
    "defaultValue": "Customize your watchface appearance and preferences."
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
        "defaultValue": "Preferences"
      },
      {
        "type": "toggle",
        "messageKey": "TemperatureUnit",
        "label": "Use Fahrenheit",
        "defaultValue": false
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
        "type": "select",
        "messageKey": "SecondsChoice",
        "defaultValue": "show",
        "label": "Show Seconds",
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
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
