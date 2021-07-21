void httppost () {
  json = "{\r\n\"operationName\": \"insert_data\",\r\n\"variables\": {\r\n\"data\": {\r\n\"email\": \"admin@gmail.com\",\r\n\"password\": \"admin!2357\",\r\n";
  json.concat("\"s1\": " + String (tekanan, 2) + ",\r\n\"s2\": 0,\r\n\"s3\": 0,\r\n");
  json.concat("\"b\": ");
  json.concat(String(volt, 2));
  json.concat(",\r\n\"signature\": \"\",\r\n");
  json.concat("\"device_code\": \"");
  json.concat(String(ID));
  json.concat("\",\r\n");
  json.concat("\"sTime\": \"");
  json.concat(String(sTime));
  json.concat("\",\r\n");
  json.concat("\"lat\": \"");
  json.concat(String(flat, 6));
  json.concat("\",\r\n");
  json.concat("\"lon\": \"");
  json.concat(String(flon, 6));
  json.concat("\"\r\n}\r\n},\r\n");
  json.concat("\"query\": \"mutation insert_data($data: DataCreateInput!) {insert_one_data(data: $data) {id}}\"\r\n}");

  Serial.println(json);

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin(client,"http://54.255.55.184:4000/graphql");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header
    http.addHeader("Content-Length",String(119 + json.length() + 105));
    httpCode = http.POST(json);   //Send the request
    String payload = http.getString();                  //Get the response payload

    statuscode(httpCode);
    Serial.print(httpCode);   //Print HTTP return code
    Serial.print(" ");
    Serial.println(network);
    Serial.println(payload);    //Print request response payload
    http.end();  //Close connection

    //print code to display
    display.setCursor(100, 52);
    display.println(httpCode);
    display.display();
  } else {
    Serial.println("Error in WiFi connection");
    httpCode = 0;
    statuscode(httpCode);
  }
  //save to sd card
  file = SD.open(filename, FILE_WRITE);
  file.print(',');
  file.print(httpCode);
  file.print(',');
  file.println(network);
  delay(100);
  file.close();
  delay(2000);
}

String statuscode(int w) {
  if (w < 100) {
    network = "Error";
  }
  if (w == 100) {
    network = "Continue";
  }
  if (w == 101) {
    network = "Switching Protocols";
  }
  if (w == 200) {
    network = "OK";
  }
  if (w == 201) {
    network = "Created";
  }
  if (w == 202) {
    network = "Accepted";
  }
  if (w == 203) {
    network = "Non-Authoritative Information";
  }
  if (w == 204) {
    network = "No Content";
  }
  if (w == 205) {
    network = "Reset Content";
  }
  if (w == 206) {
    network = "Partial Content";
  }
  if (w == 300) {
    network = "Multiple Choices";
  }
  if (w == 301) {
    network = "Moved Permanently";
  }
  if (w == 302) {
    network = "Found";
  }
  if (w == 303) {
    network = "See Other";
  }
  if (w == 304) {
    network = "Not Modified";
  }
  if (w == 305) {
    network = "Use Proxy";
  }
  if (w == 307) {
    network = "Temporary Redirect";
  }
  if (w == 400) {
    network = "Bad Request";
  }
  if (w == 401) {
    network = "Unauthorized";
  }
  if (w == 402) {
    network = "Payment Required";
  }
  if (w == 403) {
    network = "Forbidden";
  }
  if (w == 404) {
    network = "Not Found";
  }
  if (w == 405) {
    network = "Method Not Allowed";
  }
  if (w == 406) {
    network = "Not Acceptable";
  }
  if (w == 407) {
    network = "Proxy Authentication Required";
  }
  if (w == 408) {
    network = "Request Time-out";
  }
  if (w == 409) {
    network = "Conflict";
  }
  if (w == 410) {
    network = "Gone";
  }
  if (w == 411) {
    network = "Length Required";
  }
  if (w == 412) {
    network = "Precondition Failed";
  }
  if (w == 413) {
    network = "Request Entity Too Large";
  }
  if (w == 414) {
    network = "Request-URI Too Large";
  }
  if (w == 415) {
    network = "Unsupported Media Type";
  }
  if (w == 416) {
    network = "Requested range not satisfiable";
  }
  if (w == 417) {
    network = "Expectation Failed ";
  }
  if (w == 500) {
    network = "Internal Server Error";
  }
  if (w == 501) {
    network = "Not Implemented";
  }
  if (w == 502) {
    network = "Bad Gateway";
  }
  if (w == 503) {
    network = "Service Unavailable";
  }
  if (w == 504) {
    network = "Gateway Time-out";
  }
  if (w == 505) {
    network = "HTTP Version not supported";
  }
  if (w == 600) {
    network = "Not HTTP PDU";
  }
  if (w == 601) {
    network = "Network Error";
  }
  if (w == 602) {
    network = "No memory";
  }
  if (w == 603) {
    network = "DNS Error";
  }
  if (w == 604) {
    network = "Stack Busy";
  }
  return network;
}
