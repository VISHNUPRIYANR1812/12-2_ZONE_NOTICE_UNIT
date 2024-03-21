#include <WiFi.h>
#include <WebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Font_Data.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_ZONES 2
#define ZONE_SIZE 8
#define MAX_DEVICES (MAX_ZONES * ZONE_SIZE)

#define CLK_PIN   13
#define DATA_PIN  23
#define CS_PIN    5

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
WebServer server(80);
bool invertUpperZone = false;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

const char *defaultMsg[] =
{
    "ECE"
};
const char **msg = defaultMsg;
String dynamicMsg = "Default Message";
int SCROLL_SPEED = 30;
int BRIGHTNESS_LEVEL = 5;

// Define Parola effects
enum ParolaEffect 
{
    SCROLL_LEFT,
    SCROLL_RIGHT,
    FADE,
    SLIDE_IN,
    SLIDE_OUT,
    BLINK
};

ParolaEffect currentEffect = SCROLL_LEFT;  // Default effect

void handleRoot()
{
    String message = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta name='theme-color' content='#007AFF'><title>MAX7219 Dynamic Notice Board</title><style>body{background-color:#f4f4f4;color:#333;font-family:'Helvetica',sans-serif;margin:0;padding:0;text-align:center;}h1{color:#007AFF;}form{margin-top:20px;}input[type='text'],input[type='range'],input[type='submit'],select{padding:8px;margin:5px;border:1px solid #007AFF;border-radius:5px;}input[type='submit']{background-color:#007AFF;color:#fff;cursor:pointer;}input[type='range']{width:80%;}</style></head><body><h1>MAX7219 Dynamic Notice Board</h1><p>Current Message: <span id='dynamicMsg'>Default Message</span></p><form id='updateForm'><input type='text' id='newMessage' name='newMessage' placeholder='Enter new message'><br><label for='scrollSpeed'>Scroll Speed:</label><input type='range' id='scrollSpeed' name='scrollSpeed' min='10' max='50' value='30'><br><label for='brightness'>Brightness:</label><input type='range' id='brightness' name='brightness' min='0' max='15' value='5'><br><label for='effect'>Select Effect:</label><select id='effect' name='effect'><option value='0'>Scroll Left</option><option value='1'>Scroll Right</option><option value='2'>Fade</option><option value='3'>Slide In</option><option value='4'>Slide Out</option><option value='5'>Blink</option></select><br><input type='button' value='Update Message' onclick='updateMessage()'></form><!-- Restart button --><form><input type='button' value='Restart ESP32' onclick='restartESP32()'></form><script src='https://code.jquery.com/jquery-3.6.4.min.js'></script><script>function updateMessage(){var newMessage=$('#newMessage').val();var scrollSpeed=$('#scrollSpeed').val();var brightness=$('#brightness').val();var effect=$('#effect').val();$.post('/update',{newMessage:newMessage,scrollSpeed:scrollSpeed,brightness:brightness,effect:effect},function(data){alert(data);$('#dynamicMsg').text(newMessage);});}function restartESP32(){$.post('/restart',function(data){alert(data);});}</script></body></html>";
    server.send(200, "text/html", message);
}

void handleUpdate()
{
    if (server.hasArg("newMessage") && server.hasArg("scrollSpeed") && server.hasArg("brightness") && server.hasArg("effect"))
    {
        dynamicMsg = server.arg("newMessage");
        int newScrollSpeed = server.arg("scrollSpeed").toInt();
        newScrollSpeed = constrain(newScrollSpeed, 10, 50);
        SCROLL_SPEED = newScrollSpeed;

        int newBrightness = server.arg("brightness").toInt();
        newBrightness = constrain(newBrightness, 0, 15);
        BRIGHTNESS_LEVEL = newBrightness;

        int selectedEffect = server.arg("effect").toInt();
        if (selectedEffect >= 0 && selectedEffect <= 5) {
            currentEffect = static_cast<ParolaEffect>(selectedEffect);
        }

        msg = new const char *[ARRAY_SIZE(defaultMsg)];
        for (size_t i = 0; i < ARRAY_SIZE(defaultMsg); ++i)
        {
            msg[i] = strdup(defaultMsg[i]);
        }
        msg[0] = dynamicMsg.c_str();

        P.setIntensity(BRIGHTNESS_LEVEL);

        server.send(200, "text/plain", "Message updated successfully!");
    }
    else
    {
        server.send(400, "text/plain", "Bad Request");
    }
}

void handleRestart()
{
    server.send(200, "text/plain", "Restarting ESP32...");
    delay(1000);
    ESP.restart();
}

void setup(void)
{
    invertUpperZone = false; // Set to false for normal orientation

    WiFi.begin("TECH-24-SNAG_HARDWARE", "SPARTANS.LEGACY");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/update", HTTP_POST, handleUpdate);
    server.on("/restart", HTTP_POST, handleRestart);
    server.begin();

    P.begin(MAX_ZONES);
    P.setZone(0, 0, ZONE_SIZE - 1); // Set upper zone
    P.setFont(0, BigFontUpper); // Set font for upper zone
    P.setZone(1, ZONE_SIZE, MAX_DEVICES - 1); // Set lower zone
    P.setFont(1, BigFontLower); // Set font for lower zone
    P.setCharSpacing(P.getCharSpacing() * 2);
    P.setIntensity(BRIGHTNESS_LEVEL);
}

void loop(void)
{
    server.handleClient();
    static uint8_t cycle = 0;
    P.displayAnimate();
    if (P.getZoneStatus(0) && P.getZoneStatus(1))
    {
        P.setFont(0, BigFontLower); // Set font for upper zone
        P.setFont(1, BigFontUpper); // Set font for lower zone
        switch (currentEffect)
        {
            case SCROLL_LEFT:
                P.displayZoneText(0, msg[cycle], PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                P.displayZoneText(1, msg[cycle], PA_LEFT, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                break;
            case SCROLL_RIGHT:
                P.displayZoneText(0, msg[cycle], PA_RIGHT, SCROLL_SPEED, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                P.displayZoneText(1, msg[cycle], PA_RIGHT, SCROLL_SPEED, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                break;
            case FADE:
                P.displayZoneText(0, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_FADE, PA_FADE);
                P.displayZoneText(1, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_FADE, PA_FADE);
                break;
            case SLIDE_IN:
                P.displayZoneText(0, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                P.displayZoneText(1, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
                break;
            case SLIDE_OUT:
                P.displayZoneText(0, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                P.displayZoneText(1, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
                break;
            case BLINK:
                P.displayZoneText(0, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_PRINT, PA_PRINT);
                P.displayZoneText(1, msg[cycle], PA_CENTER, SCROLL_SPEED, 0, PA_PRINT, PA_PRINT);
                break;
            default:
                break;
        }
        cycle = (cycle + 1) % ARRAY_SIZE(defaultMsg);
        P.displayClear();
        P.synchZoneStart();
    }
}

