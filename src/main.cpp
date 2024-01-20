#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "ooredoo_5AEBA8"
#define WIFI_PASSWORD "7M7E4EX3M4VNM"

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "reghiouiaymenn@gmail.com"
#define AUTHOR_PASSWORD "yvjz oiyz zkgd rwxt"

#define RECIPIENT_EMAIL "reghioui36@gmail.com"

SMTPSession smtp;

void smtpCallback(SMTP_Status status);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  MailClient.networkReconnect(true);

  smtp.debug(1);

  smtp.callback(smtpCallback);

  Session_Config config;

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.login.user_domain = "";

  config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  config.time.gmt_offset = 3;
  config.time.day_light_offset = 0;

  SMTP_Message message;

  message.sender.name = F("ESP");
  message.sender.email = AUTHOR_EMAIL;
  message.subject = F("ESP Test Email");
  message.addRecipient(F("AYMEN"), RECIPIENT_EMAIL);

  String textMsg = "This message is sent from aymen with esp32";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;

  if (!smtp.connect(&config))
  {
    ESP_MAIL_PRINTF("Connection error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    return;
  }

  if (!smtp.isLoggedIn())
  {
    Serial.println("\nNot yet logged in.");
  }
  else
  {
    if (smtp.isAuthenticated())
      Serial.println("\nSuccessfully logged in.");
    else
      Serial.println("\nConnected with no Auth.");
  }

  if (!MailClient.sendMail(&smtp, &message))
    ESP_MAIL_PRINTF("Error, Status Code: %d, Error Code: %d, Reason: %s", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
}

void loop()
{
}

void smtpCallback(SMTP_Status status)
{

  Serial.println(status.info());

  if (status.success())
  {

    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failed: %d\n", status.failedCount());
    Serial.println("----------------\n");

    for (size_t i = 0; i < smtp.sendingResult.size(); i++)
    {

      SMTP_Result result = smtp.sendingResult.getItem(i);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients.c_str());
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject.c_str());
    }
    Serial.println("----------------\n");

    smtp.sendingResult.clear();
  }
}