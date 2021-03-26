#include "apis.h"

Apis::Apis(Display displayIn)
{
    display = displayIn;
}

void Apis::handle_db_transport_rest(Config config)
{
    HTTPClient http;
    String url = "https://v5.db.transport.rest/stops/" + String(config.bahnhof) + "/departures?results=10&duration=120";
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("New data");
        Serial.println(httpResponseCode);
        // Serial.println(response);

        Serial.println("Parsing JSON...");
        DeserializationError error = deserializeJson(doc, response);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            Serial.println("No errors");

            time_t now;
            time(&now);
            Serial.println(now);

            display.fillSprite(0x005);
            display.drawDBTopLine();
            unsigned int departures_length = doc.size();
            unsigned int i = 0;
            unsigned int cnt = 0;

            Serial.print("Departures received: ");
            Serial.println(departures_length);
            while (i < departures_length && cnt < 8)
            {
                // Extract what we are interested in from response
                // based on config
                bool interesting_type = false;
                bool interesting_line = false;
                bool interesting_destination = true;

                for (int j = 0; j < MAX_INCLUDE_TYPE; ++j)
                {
                    Serial.println(config.include_type[j]);
                    if (config.include_type[j] && strcmp(config.include_type[j], "*") == 0)
                    {
                        // We want to see all types
                        interesting_type = true;
                        break;
                    }

                    // TODO make enum for this or something
                    // can be these
                    // {"nationalExpress":true,"national":true,"regionalExp":true,"regional":true,"suburban":true,"bus":true,"ferry":false,"subway":false,"tram":true,"taxi":true}}
                    if (config.include_type[j] && strcmp(config.include_type[j], doc[i]["line"]["product"]) == 0)
                    {
                        // We want to see this type
                        interesting_type = true;
                        break;
                    }
                }

                for (int j = 0; j < MAX_INCLUDE_LINE; ++j)
                {
                    Serial.println(config.include_line[j]);
                    if (config.include_line[j] && strcmp(config.include_line[j], "*") == 0)
                    {
                        // We want to see all lines
                        interesting_line = true;
                        break;
                    }
                    // stuff like "STR 7" here in name field
                    if (config.include_line[j] && strcmp(config.include_line[j], doc[i]["line"]["name"]) == 0)
                    {
                        // We want to see this line
                        interesting_line = true;
                        break;
                    }
                }
                if (interesting_type && interesting_line)
                {
                    for (int j = 0; j < MAX_EXCLUDE_DESTINATION; ++j)
                    {
                        if (config.exclude_destinations[j] && strcmp(config.exclude_destinations[j], doc[i]["direction"]) == 0)
                        {
                            interesting_destination = false;
                            break;
                        }
                    }
                }
                if (interesting_type && interesting_line && interesting_destination)
                {
                    //Calc minutes until departure
                    Serial.println("Delay calculation");
                    unsigned long departure = (convertToTime(doc[i]["when"]));
                    Serial.println(departure);
                    Serial.println(now);

                    unsigned long minutes = 0;
                    if (departure > now)
                    {
                        unsigned long wait = departure - now;
                        Serial.println(wait);
                        minutes = wait / 60;
                        //if (wait % 60 > 30) ++minutes;
                        // doc when already = (plannedWhen + delay)
                        // minutes += doc[i]["delay"].as<int>();
                    }
                    Serial.print("Delay of train: ");
                    Serial.println(minutes);

                    String lineName = doc[i]["line"]["name"].as<String>();

                    // remove space between e.g. "S 7" line
                    if (lineName[0] == 'S' && lineName[1] == ' ') {
                        lineName = lineName.substring(0,1) + lineName.substring(2);
                    }

                    String beforeSpace = getSplitPosition(lineName, ' ', 0);
                    String afterSpace = getSplitPosition(lineName, ' ', 1);
                    if (afterSpace != "") {
                        boolean isRepeatedAfterSpace = (beforeSpace.substring(0,3) == afterSpace.substring(0,3));
                        if (isRepeatedAfterSpace) {
                            lineName = afterSpace;
                        } else {
                            beforeSpace.concat(afterSpace);
                            lineName = beforeSpace;
                        }
                    }

                    display.drawDBDeparture(cnt, lineName, doc[i]["direction"].as<String>(), doc[i]["platform"].as<String>(), minutes);

                    ++cnt;
                }
                ++i;
            }
            display.pushAll();
        }
    }
    else
    {
        Serial.print("Error: Couldn't send GET: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    delay(30000); //Send a request every 30 seconds
};


void Apis::handle_mvg_api(Config config)
{
    HTTPClient http;
    String url = "https://www.mvg.de/api/fahrinfo/departure/" + String(config.bahnhof) + "?footway=0";
    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        Serial.println("New data");
        Serial.println(httpResponseCode);
        // Serial.println(response);

        Serial.println("Parsing JSON...");
        DeserializationError error = deserializeJson(doc, response);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            Serial.println("No errors");
            String servings = doc["departures"][0];
            // Serial.println(servings);

            time_t now;
            time(&now);
            Serial.println(now);

            display.fillSprite(0x005);
            display.drawTopLine();
            unsigned int departures_length = doc["departures"].size();
            unsigned int i = 0;
            unsigned int cnt = 0;

            while (i < departures_length && cnt < 8)
            {
                // Extract what we are interested in from response
                // based on config
                bool interesting_type = false;
                bool interesting_line = false;
                bool interesting_destination = true;

                for (int j = 0; j < MAX_INCLUDE_TYPE; ++j)
                {
                    Serial.println(config.include_type[j]);
                    if (config.include_type[j] && strcmp(config.include_type[j], "*") == 0)
                    {
                        // We want to see all types
                        interesting_type = true;
                        break;
                    }
                    if (config.include_type[j] && strcmp(config.include_type[j], doc["departures"][i]["product"]) == 0)
                    {
                        // We want to see this type
                        interesting_type = true;
                        break;
                    }
                }

                for (int j = 0; j < MAX_INCLUDE_LINE; ++j)
                {
                    Serial.println(config.include_line[j]);
                    if (config.include_line[j] && strcmp(config.include_line[j], "*") == 0)
                    {
                        // We want to see all lines
                        interesting_line = true;
                        break;
                    }
                    if (config.include_line[j] && strcmp(config.include_line[j], doc["departures"][i]["label"]) == 0)
                    {
                        // We want to see this line
                        interesting_line = true;
                        break;
                    }
                }
                if (interesting_type && interesting_line)
                {
                    for (int j = 0; j < MAX_EXCLUDE_DESTINATION; ++j)
                    {
                        if (config.exclude_destinations[j] && strcmp(config.exclude_destinations[j], doc["departures"][i]["destination"]) == 0)
                        {
                            interesting_destination = false;
                            break;
                        }
                    }
                }
                if (interesting_type && interesting_line && interesting_destination)
                {
                    //Calc minutes until departure
                    unsigned long departure = doc["departures"][i]["departureTime"].as<long long>() / 1000; //ms to seconds
                    Serial.println(departure);

                    unsigned long minutes = 0;
                    if (departure > now)
                    {
                        unsigned long wait = departure - now;
                        Serial.println(wait);
                        minutes = wait / 60;
                        //if (wait % 60 > 30) ++minutes;
                        minutes += doc["departures"][i]["delay"].as<int>();
                    }
                    Serial.println(minutes);

                    display.drawDeparture(cnt, doc["departures"][i]["label"].as<String>(), doc["departures"][i]["destination"].as<String>(), doc["departures"][i]["platform"].as<String>(), 0, minutes);

                    ++cnt;
                }
                ++i;
            }
            display.pushAll();
        }
    }
    else
    {
        Serial.print("Error: Couldn't send GET: ");
        Serial.println(httpResponseCode);
    }
    http.end();
    delay(30000); //Send a request every 30 seconds
};

void Apis::init_geops_api(Config config)
{
    bool connected = client.connect("wss://api.geops.io:443/realtime-ws/v1/?key=5cc87b12d7c5370001c1d655306122aa0a4743c489b497cb1afbec9b");
    if (connected)
    {
        Serial.println("Connected!");
        client.send("GET timetable_" + String(config.bahnhof));
        client.send("SUB timetable_" + String(config.bahnhof)); //Subscribe for Departures at Hauptbahnhof
    }
    else
    {
        Serial.println("Not Connected!");
    }

    client.onEvent([&](websockets::WebsocketsEvent event, String data) {
        if (event == websockets::WebsocketsEvent::ConnectionOpened)
        {
            Serial.println("Connnection Opened");
        }
        else if (event == websockets::WebsocketsEvent::ConnectionClosed)
        {
            Serial.println("Connnection Closed");
            init_geops_api(config);
        }
        else if (event == websockets::WebsocketsEvent::GotPing)
        {
            Serial.println("Got a Ping!");
        }
        else if (event == websockets::WebsocketsEvent::GotPong)
        {
            Serial.println("Got a Pong!");
        }
    });

    // run callback when messages are received
    client.onMessage([&](websockets::WebsocketsMessage message) {
        Serial.println(message.data());
        Serial.println("Parsing JSON...");
        DeserializationError error = deserializeJson(doc, message.data());
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            Serial.println("No errors");
            Serial.print("Cheking for ");
            Serial.println(String(config.bahnhof));
            if (doc["source"] == ("timetable_" + String(config.bahnhof)))
            {
                Departure received_departure;
                received_departure.aimed_time = doc["content"]["ris_aimed_time"].as<double>();
                received_departure.estimated_time = doc["content"]["time"].as<double>();
                received_departure.line = doc["content"]["line"]["name"].as<String>();
                received_departure.destination = doc["content"]["to"][0].as<String>();
                received_departure.platform = doc["content"]["platform"].as<int>();
                received_departure.wagon = doc["content"]["train_type"].as<int>();

                if (departure_list.empty())
                {
                    Serial.println("EMPTY");
                    departure_list.push_back(received_departure);
                }
                else
                {
                    Serial.println("Not empty");
                    list<Departure>::iterator it1;
                    for (it1 = departure_list.begin(); it1 != departure_list.end(); ++it1)
                    {
                        Serial.println("For loop");

                        if (it1->aimed_time == received_departure.aimed_time && it1->line == received_departure.line && it1->destination == received_departure.destination) //Departure schon vorhanden => Update
                        {
                            Serial.println("Update");
                            *it1 = received_departure;
                            //Sorting to be sure we are sill in correct order
                            departure_list.sort([](const Departure &a, const Departure &b) { return a.estimated_time < b.estimated_time; });
                            break;
                        }
                        if (next(it1, 1) == departure_list.end() && departure_list.size() < MAX_DEPARTURE_LIST_LENGTH) //Departure nicht vorhanden
                        {
                            Serial.println("Departure nicht vorhanden");
                            list<Departure>::iterator it2;
                            for (it2 = departure_list.begin(); it2 != departure_list.end(); ++it2)
                            {
                                if (it2->estimated_time > received_departure.estimated_time) //Element richtig einsortieren
                                {
                                    Serial.println("Departure insert");
                                    departure_list.insert(it2, received_departure);
                                    break;
                                }
                                if (next(it2, 1) == departure_list.end()) //Element ganz hinten einfügen da größter Wert
                                {
                                    Serial.println("Departure push_back");
                                    departure_list.push_back(received_departure);
                                    break; //needed cause otherwise it2 != departure_list.end() will never be true
                                }
                            }
                        }
                    }
                }
            }
        }
    });
}

void Apis::handle_geops_api(Config config)
{
    if (client.available())
    {
        client.poll();
    }

    time_t now;
    time(&now);

    //Serial.println("New List");
    display.fillSprite(0x005);
    display.drawTopLine();
    list<Departure>::iterator it;
    it = departure_list.begin();
    int cnt = 0;
    while (!departure_list.empty() && it != departure_list.end() && cnt < 8)
    {
        unsigned long estimated_time_s = it->estimated_time / 1000;
        unsigned long minutes = 0;

        if (estimated_time_s > now)
        {
            unsigned long wait = estimated_time_s - now;
            minutes = wait / 60;
            //if (wait % 60 > 30) ++minutes;
        }
        else //abfahrt in der vergangenheit => aus der liste entfernen
        {
            departure_list.erase(it);
        }

        if (it != departure_list.end())
        {
            display.drawDeparture(cnt, it->line, it->destination, it->platform, it->wagon, minutes);
        }
        ++cnt;
        ++it;
    }
    display.pushAll();
}

void Apis::ping_geops_api(Config config)
{
    if (client.available())
    {
        client.send("PING");
        Serial.println("Send Ping");
    }
    else
    {
        Serial.println("Try to Reconnect geops!");
        init_geops_api(config);
    }
}