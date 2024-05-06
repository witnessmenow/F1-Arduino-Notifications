#include <FileFetcher.h>

// i.imgur.com
// USERTrust RSA Certification Authority

const char IMGUR_CERTIFICATE_ROOT[] = R"=EOF=(
-----BEGIN CERTIFICATE-----
MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw
MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV
BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU
aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy
dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK
AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B
3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY
tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/
Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2
VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT
79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6
c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT
Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l
c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee
UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE
Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd
BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G
A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF
Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO
VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3
ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs
8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR
iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze
Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ
XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/
qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB
VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB
L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG
jjxDah2nGN59PRbxYvnKkKj9
-----END CERTIFICATE-----
)=EOF=";

// file name for where to save the image.
#define TRACK_IMAGE "/track.png"

const char *getImageUrlForRace(const char *raceName)
{
  if (strcmp(raceName, "Bahrain") == 0)
  {
    return "https://i.imgur.com/zUqArqi.png";
  }
  else if (strcmp(raceName, "Saudi Arabian") == 0)
  {
    return "https://i.imgur.com/vx6MDSF.png";
  }
  else if (strcmp(raceName, "Australian") == 0)
  {
    return "https://i.imgur.com/ewrhVKU.png";
  }
  else if (strcmp(raceName, "Azerbaijan") == 0)
  {
    return "https://i.imgur.com/H2C6G2Q.png";
  }
  else if (strcmp(raceName, "Miami") == 0)
  {
    return "https://i.imgur.com/mwoQzCm.png";
  }
  else if (strcmp(raceName, "Emilia Romagna Grand Prix") == 0)
  {
    return "https://i.imgur.com/fm6IygV.png";
  }
  else if (strcmp(raceName, "Monaco") == 0)
  {
    return "https://i.imgur.com/Q48IRF1.png";
  }
  else if (strcmp(raceName, "Spanish") == 0)
  {
    return "https://i.imgur.com/GdnHo69.png";
  }
  else if (strcmp(raceName, "Canadian") == 0)
  {
    return "https://i.imgur.com/QNAli6L.png";
  }
  else if (strcmp(raceName, "Austrian") == 0)
  {
    return "https://i.imgur.com/Xu4I91f.png";
  }
  else if (strcmp(raceName, "British") == 0)
  {
    return "https://i.imgur.com/R0snf2W.png";
  }
  else if (strcmp(raceName, "Hungarian") == 0)
  {
    return "https://i.imgur.com/R0snf2W.png";
  }
  else if (strcmp(raceName, "Belgian") == 0)
  {
    return "https://i.imgur.com/Hr3HUGP.png";
  }
  else if (strcmp(raceName, "Dutch") == 0)
  {
    return "https://i.imgur.com/fwyHAy5.png";
  }
  else if (strcmp(raceName, "Italian") == 0)
  {
    return "https://i.imgur.com/KrRzWhh.png";
  }
  else if (strcmp(raceName, "Singapore") == 0)
  {
    return "https://i.imgur.com/di1xFkV.png";
  }
  else if (strcmp(raceName, "Japanese") == 0)
  {
    return "https://i.imgur.com/BINBSn3.png";
  }
  else if (strcmp(raceName, "Qatar") == 0)
  {
    return "https://i.imgur.com/YdpmY5o.png";
  }
  else if (strcmp(raceName, "United States") == 0)
  {
    return "https://i.imgur.com/NzZNjF6.png";
  }
  else if (strcmp(raceName, "Mexican") == 0)
  {
    return "https://i.imgur.com/gvUauKO.png";
  }
  else if (strcmp(raceName, "Brazilian") == 0)
  {
    return "https://i.imgur.com/3g4zz17.png";
  }
  else if (strcmp(raceName, "Las Vegas") == 0)
  {
    return "https://i.imgur.com/er9A6G8.png";
  }
  else if (strcmp(raceName, "Abu Dhabi") == 0)
  {
    return "https://i.imgur.com/QDwWXna.png";
  }

  // Image not found
  return "https://i.imgur.com/FRXJ4do.png";
  //"https://i.imgur.com/tvLjDeo.png"; // This is a called off image
}

int getImage(const char *raceName)
{

  const char *imageUrl = getImageUrlForRace(raceName);

  // In this example I reuse the same filename
  // over and over
  if (SPIFFS.exists(TRACK_IMAGE) == true)
  {
    Serial.println("Removing existing image");
    SPIFFS.remove(TRACK_IMAGE);
  }

  fs::File f = SPIFFS.open(TRACK_IMAGE, "w+");
  if (!f)
  {
    Serial.println("file open failed");
    return -1;
  }

  secured_client.setCACert(IMGUR_CERTIFICATE_ROOT);
  bool gotImage = fileFetcher.getFile((char *)imageUrl, &f);

  // Make sure to close the file!
  f.close();

  return gotImage;
}
