const mqtt = require('mqtt');

const host = 'mqtt.eclipseprojects.io';
const port = '1883';
const connectUrl = `mqtt://${host}:${port}`

var client = mqtt.connect(connectUrl, {
    username: 'ADXL_LUCAS_TESTE',
});

var ssd1306topic = `${client.options.username}/f/ssd1306`;

client.on('connect', function() {
    console.log('connected');

    client.subscribe(ssd1306topic, function(err) {
        if(! err) {
            console.log('subscribed');

            client.publish(ssd1306topic, 'Hello from NodeJS');
        }
    });
});

client.on('message', function(topic, message) {
    console.log(message.toString());
});